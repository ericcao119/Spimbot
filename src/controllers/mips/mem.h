/* SPIM S20 MIPS simulator.
   Macros for accessing memory.

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

// XXX: Depends on inst.h to be completed

#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#include <string>

#include "cpu.h"
#include "inst.h"
#include "reg.h"

/* A note on directions:  "Bottom" of memory is the direction of
   decreasing addresses.  "Top" is the direction of increasing addresses.*/

/* Type of contents of a memory word. */

using mem_word = int32_t; /*@alt unsigned int @*/

using BYTE_TYPE = signed char;

/* Memory is allocated in five chunks:
        text, data, stack, kernel text, and kernel data.

   The arrays are independent and have different semantics.

   text is allocated from 0x400000 up and only contains INSTRUCTIONs.
   It does not expand.

   data is allocated from 0x10000000 up.  It can be extended by the
   SBRK system call.  Programs can only read and write this segment.

   stack grows from 0x7fffefff down.  It is automatically extended.
   Programs can only read and write this segment.

   k_text is like text, except its is allocated from 0x80000000 up.

   k_data is like data, but is allocated from 0x90000000 up.

   Both kernel text and kernel data can only be accessed in kernel mode.
*/

// XXX: Really needs a constructor and destructor
struct mem_image_t {
    /**
     * Constants
     */

    // int32_t text_size, data_size, stack_size, k_text_size, k_data_size;
    int32_t data_limit, stack_limit, k_data_limit;

    /* The text segment. */
    std::vector<instruction *> text_seg;
    std::vector<unsigned> text_prof;
    bool text_modified; /* => text segment was written */
    mem_addr text_top;

    /* The data segment. */
    std::vector<mem_word> data_seg;
    bool data_modified;    /* => a data segment was written */
    short *data_seg_h;     /* Points to same vector as DATA_SEG */
    BYTE_TYPE *data_seg_b; /* Ditto */
    mem_addr data_top;
    mem_addr gp_midpoint; /* Middle of $gp area */

    /* The stack segment. */
    std::vector<mem_word> stack_seg;
    short *stack_seg_h;     /* Points to same vector as STACK_SEG */
    BYTE_TYPE *stack_seg_b; /* Ditto */
    mem_addr stack_bot;

    /* Used for SPIMbot stuff. */
    std::vector<mem_word> special_seg;
    short *special_seg_h;
    BYTE_TYPE *special_seg_b;

    /* The kernel text segment. */
    std::vector<instruction *> k_text_seg;
    std::vector<unsigned> k_text_prof;
    mem_addr k_text_top;

    /* The kernel data segment. */
    std::vector<mem_word> k_data_seg;
    short *k_data_seg_h;
    BYTE_TYPE *k_data_seg_b;
    mem_addr k_data_top;

    // MMIO Data

    int recv_control = 0; /* No input */
    int recv_buffer;
    int recv_buffer_full_timer = 0;

    int trans_control = TRANS_READY; /* Ready to write */
    int trans_buffer;
    int trans_buffer_full_timer = 0;

    /**
     * As a warning, the constructor is only used to clear all the variables as if it was
     * statically initialized. Do not use it before calling make_memory with the appropriate
     * config information
     */

    mem_image_t();

    // Methods
    void mem_dump_profile(const std::string &prof_file_name) const;

    void make_memory(int text_size, int data_size, int data_limit, int stack_size, int stack_limit,
                     int k_text_size, int k_data_size, int k_data_limit);

    /* Expand the data segment by adding N bytes. */
    void expand_data(int addl_bytes);

    /* Expand the stack segment by adding N bytes.  Can't use REALLOC
     * since it copies from bottom of memory blocks and stack grows down from
     * top of its block.
     */
    void expand_stack(int addl_bytes);

    /* Expand the kernel data segment by adding N bytes. */
    void expand_k_data(int addl_bytes);

    /* Access memory */
    void *mem_reference(mem_addr addr) const;

   private:
    /* Free the storage used by the old instructions in memory. */
    void free_instructions(std::vector<instruction *> &, int n);
};

// extern mem_image_t mem_images[2];

/* The text boundaries. */
constexpr mem_addr TEXT_BOT = ((mem_addr)0x400000);
/* Amount to grow text segment when we run out of space for instructions. */
constexpr size_t TEXT_CHUNK_SIZE = 4096;

/* The data boundaries. */
constexpr mem_addr DATA_BOT = ((mem_addr)0x10000000);

/* The stack boundaries. */
/* Exclusive, but include 4K at top of stack. */
constexpr mem_addr STACK_TOP = ((mem_addr)0x80000000);

/* The kernel text boundaries. */
constexpr mem_addr K_TEXT_BOT = ((mem_addr)0x80000000);

/* The Kernel data boundaries. */
constexpr mem_addr K_DATA_BOT = ((mem_addr)0x90000000);

/* Memory-mapped IO area: */
constexpr mem_addr MM_IO_BOT = ((mem_addr)0xffff0000);
constexpr mem_addr MM_IO_TOP = ((mem_addr)0xffffffff);

constexpr mem_addr SPECIAL_BOT = ((mem_addr)0xfffe0000);
constexpr mem_addr SPECIAL_TOP = ((mem_addr)0xffff0000);

/* Read from console: */
constexpr mem_addr RECV_CTRL_ADDR = ((mem_addr)0xffff0000);
constexpr mem_addr RECV_BUFFER_ADDR = ((mem_addr)0xffff0004);

constexpr uint32_t RECV_READY = 0x1;
constexpr uint32_t RECV_INT_ENABLE = 0x2;

constexpr uint32_t RECV_INT_LEVEL = 3; /* HW Interrupt 1 */

/* Write to console: */
constexpr mem_addr TRANS_CTRL_ADDR = ((mem_addr)0xffff0008);
constexpr mem_addr TRANS_BUFFER_ADDR = ((mem_addr)0xffff000c);

constexpr uint32_t TRANS_READY = 0x1;
constexpr uint32_t TRANS_INT_ENABLE = 0x2;

constexpr uint32_t TRANS_INT_LEVEL = 2; /* HW Interrupt 0 */

/* Exported functions: */

void check_memory_mapped_IO(size_t context);
void expand_data(size_t context, int addl_bytes);
void expand_k_data(size_t context, int addl_bytes);
void expand_stack(size_t context, int addl_bytes);
void make_memory(size_t context, int text_size, int data_size, int data_limit, int stack_size,
                 int stack_limit, int k_text_size, int k_data_size, int k_data_limit);
void mem_dump_profile();
void *mem_reference(size_t context, mem_addr addr);
void print_mem(mem_addr addr);
instruction *read_mem_inst(size_t context, mem_addr addr);
reg_word read_mem_byte(size_t context, mem_addr addr);
reg_word read_mem_half(size_t context, mem_addr addr);
reg_word read_mem_word(size_t context, mem_addr addr);
void set_mem_inst(size_t context, mem_addr addr, instruction *inst);
void set_mem_byte(size_t context, mem_addr addr, reg_word value);
void set_mem_half(size_t context, mem_addr addr, reg_word value);
void set_mem_word(size_t context, mem_addr addr, reg_word value);

#endif
