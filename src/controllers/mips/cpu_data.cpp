/* SPIM S20 MIPS simulator.
   Code to manipulate data segment directives.

   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "cpu.h"
// #include "parser-yacc.h"

/* The first 64K of the data segment are dedicated to small data
   segment, which is pointed to by $gp. This register points to the
   middle of the segment, so we can use the full offset field in an
   instruction. */

inline mem_addr CPU::DATA_PC() const {
    return this->registers.in_kernel ? this->registers.next_k_data_pc
                                     : this->registers.next_data_pc;
}

/* If TO_KERNEL is true, subsequent data will be placed in the
   kernel data segment.  If false, data will go to the user's data
   segment.*/

void CPU::user_kernel_data_segment(bool to_kernel) { this->registers.in_kernel = to_kernel; }

void CPU::end_of_assembly_file() {
    this->registers.in_kernel = false;
    this->registers.auto_alignment = true;
}

/* Set the point at which the first datum is stored to be ADDRESS +
   64K.	 The 64K increment allocates an area pointed to by register
   $gp, which is initialized. */

void CPU::data_begins_at_point(mem_addr addr) {
    if (config.bare_machine) {
        this->registers.next_data_pc = addr;
    } else {
        this->registers.next_gp_item_addr = addr;
        this->memory.gp_midpoint = addr + 32 * K;
        this->registers.R[REG_GP] = this->memory.gp_midpoint;
        this->registers.next_data_pc = addr + 64 * K;
    }
}

/* Set the point at which the first datum is stored in the kernel's
   data segment. */

void CPU::k_data_begins_at_point(mem_addr addr) {
    this->registers.next_k_data_pc = addr;
}

/* Arrange that the next datum is stored on a memory boundary with its
   low ALIGNMENT bits equal to 0.  If argument is 0, disable automatic
   alignment.*/

void CPU::align_data(int alignment) {
    if (alignment == 0) {
        this->registers.auto_alignment = false;
    } else if (this->registers.in_kernel) {
        this->registers.next_k_data_pc =
            (this->registers.next_k_data_pc + (1 << alignment) - 1) & (-1 << alignment);
        fix_current_label_address(this->registers.next_k_data_pc);
    } else {
        this->registers.next_data_pc =
            (this->registers.next_data_pc + (1 << alignment) - 1) & (-1 << alignment);
        fix_current_label_address(this->registers.next_data_pc);
    }
}

void CPU::set_data_alignment(int alignment) {
    if (this->registers.auto_alignment) {
        align_data(alignment);
    }
}

void CPU::enable_data_alignment() { this->registers.auto_alignment = true; }

/* Set the location (in user or kernel data space) for the next datum. */

void CPU::set_data_pc(mem_addr addr) {
    if (this->registers.in_kernel) {
        this->registers.next_k_data_pc = addr;
    } else {
        this->registers.next_data_pc = addr;
    }
}

/* Return the address at which the next datum will be stored.  */

mem_addr CPU::current_data_pc() const { return DATA_PC(); }

/* Bump the address at which the next data will be stored by DELTA
   bytes. */

void CPU::increment_data_pc(int delta) {
    if (this->registers.in_kernel) {
        this->registers.next_k_data_pc += delta;
        if (this->memory.k_data_top <= this->registers.next_k_data_pc) {
            expand_k_data(
                current_image,
                ROUND_UP(this->registers.next_k_data_pc - this->memory.k_data_top + 1, 64 * K));
        }
    } else {
        this->registers.next_data_pc += delta;
        if (this->memory.data_top <= this->registers.next_data_pc) {
            expand_data(current_image,
                        ROUND_UP(this->registers.next_data_pc - this->memory.data_top + 1, 64 * K));
        }
    }
}

/* Process a .extern NAME SIZE directive. */

void CPU::extern_directive(const std::string& name, int size) {
    label *sym = this->symbol_table.make_label_global(name);

    if (!config.bare_machine && !sym->gp_flag  // Not already a global symbol
        && size > 0 && size <= SMALL_DATA_SEG_MAX_SIZE &&
        this->registers.next_gp_item_addr + size < this->memory.gp_midpoint + 32 * K) {
        sym->gp_flag = 1;
        sym->addr = this->registers.next_gp_item_addr;
        this->registers.next_gp_item_addr += size;
    }
}

/* Process a .lcomm NAME SIZE directive. */

void CPU::lcomm_directive(const std::string& name, int size) {
    if (!config.bare_machine && size > 0 && size <= SMALL_DATA_SEG_MAX_SIZE &&
        this->registers.next_gp_item_addr + size < this->memory.gp_midpoint + 32 * K) {
        label *sym = record_label(name, this->registers.next_gp_item_addr, true);
        sym->gp_flag = 1;

        this->registers.next_gp_item_addr += size;
        /* Don't need to initialize since memory starts with 0's */
    } else {
        (void)record_label(name, this->registers.next_data_pc, true);

        for (; size > 0; size--) {
            store_byte(0);
        }
    }
}

/* Process a .ascii STRING or .asciiz STRING directive. */

void CPU::store_string(char *string, int length, bool null_terminate) {
    for (; length > 0; string++, length--) {
        store_byte(*string);
    }
    if (null_terminate) {
        store_byte(0);
    }
}

/* Process a .byte EXPR directive. */

void CPU::store_byte(int value) {
    set_mem_byte(DATA_PC(), value);
    increment_data_pc(1);
}

/* Process a .half EXPR directive. */

void CPU::store_half(int value) {
    if ((DATA_PC() & 0x1) != 0) {
#ifdef SPIM_BIGENDIAN
        store_byte((value >> 8) & 0xff);
        store_byte(value & 0xff);
#else
        store_byte(value & 0xff);
        store_byte((value >> 8) & 0xff);
#endif
    } else {
        set_mem_half(DATA_PC(), value);
        increment_data_pc(BYTES_PER_WORD / 2);
    }
}

/* Process a .word EXPR directive. */

void CPU::store_word(int value) {
    if ((DATA_PC() & 0x3) != 0) {
#ifdef SPIM_BIGENDIAN
        store_half((value >> 16) & 0xffff);
        store_half(value & 0xffff);
#else
        store_half(value & 0xffff);
        store_half((value >> 16) & 0xffff);
#endif
    } else {
        set_mem_word(DATA_PC(), value);
        increment_data_pc(BYTES_PER_WORD);
    }
}

/* Process a .double EXPR directive. */

void CPU::store_double(double *value) {
    if ((DATA_PC() & 0x7) != 0) {
        store_word(*((mem_word *)value));
        store_word(*(((mem_word *)value) + 1));
    } else {
        set_mem_word(DATA_PC(), *((mem_word *)value));
        increment_data_pc(BYTES_PER_WORD);
        set_mem_word(DATA_PC(), *(((mem_word *)value) + 1));
        increment_data_pc(BYTES_PER_WORD);
    }
}

/* Process a .float EXPR directive. */

void CPU::store_float(double *value) {
    float val = (float)*value;
    float *vp = &val;

    if ((DATA_PC() & 0x3) != 0) {
        store_half(*(mem_word *)vp & 0xffff);
        store_half((*(mem_word *)vp >> 16) & 0xffff);
    } else {
        set_mem_word(DATA_PC(), *((mem_word *)vp));
        increment_data_pc(BYTES_PER_WORD);
    }
}
