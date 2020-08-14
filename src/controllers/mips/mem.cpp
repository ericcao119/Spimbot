/* SPIM S20 MIPS simulator.
   Code to create, maintain and access memory.

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
#include "mem.h"

#include <sstream>

#include "inst.h"
#include "reg.h"
#include "spim-utils.h"
#include "spim.h"
#include "string-stream.h"

mem_image_t::mem_image_t() {
    int32_t data_limit, stack_limit, k_data_limit;
    /* The text segment. */
    this->text_seg = {};
    this->text_prof = {};
    this->text_modified = false; /* => text segment was written */
    this->text_top = 0;

    /* The data segment. */
    this->data_seg = {};
    this->data_modified = false; /* => a data segment was written */
    this->data_seg_h = nullptr;  /* Points to same vector as DATA_SEG */
    this->data_seg_b = nullptr;  /* Ditto */
    this->data_top = 0;
    this->gp_midpoint = 0; /* Middle of $gp area */

    /* The stack segment. */
    this->stack_seg = {};
    this->stack_seg_h = nullptr; /* Points to same vector as STACK_SEG */
    this->stack_seg_b = nullptr; /* Ditto */
    this->stack_bot = 0;

    /* Used for SPIMbot stuff. */
    this->special_seg = {};
    this->special_seg_h = nullptr;
    this->special_seg_b = nullptr;

    /* The kernel text segment. */
    k_text_seg = {};
    k_text_prof = {};
    k_text_top = 0;

    /* The kernel data segment. */
    k_data_seg = {};
    k_data_seg_h = nullptr;
    k_data_seg_b = nullptr;
    k_data_top = 0;

    // MMIO Data

    int recv_control = 0; /* No input */
    int recv_buffer = 0;
    int recv_buffer_full_timer = 0;

    int trans_control = TRANS_READY; /* Ready to write */
    int trans_buffer = 0;            // unused
    int trans_buffer_full_timer = 0;
}

void mem_image_t::mem_dump_profile(const std::string &prof_file_name) const {
    const mem_image_t &mem_image = *this;

    // Init string stream

    std::stringstream ss;
    FILE *file = nullptr;

    // No output file specified, so abort
    if (prof_file_name == "") {
        return;
    }

    // Return if con't open file
    file = fopen(prof_file_name.c_str(), "w");
    if (file == nullptr) {
        printf("failed to open profile file: %s\n",
               prof_file_name);  // XXX: Convert to logging statement
        return;
    }

    // Print out text segment
    int text_size = (mem_image.text_top - TEXT_BOT) / BYTES_PER_WORD;
    if (mem_image.text_seg.size() != text_size) {
        printf("Text Size: %d, does not match with number of words: %d", text_size,
               mem_image.text_seg.size());
    }

    for (int i = 0; i < text_size; ++i) {
        instruction *inst = mem_image.text_seg[i];
        if (inst == nullptr) {
            continue;
        }
        unsigned prof_count = mem_image.text_prof[i];
        mem_addr addr = TEXT_BOT + (i << 2);
        fprintf(file, "%9d ", prof_count - 1);

        instruction::format_an_inst(ss, inst, addr);

        // print_inst_internal (&buf[10], sizeof(buf)-12, inst, addr);
        fprintf(file, "%s", ss.str());
        ss.clear();
        fflush(file);
    }

    // Print out kernel text segment
    fprintf(file, "\n\nkernel text segment\n\n");

    int k_text_size = (mem_image.k_text_top - K_TEXT_BOT) / 4;

    if (mem_image.k_text_seg.size() != k_text_size) {
        printf("Text Size: %d, does not match with number of words: %d", k_text_size,
               mem_image.k_text_seg.size());
    }
    for (int i = 0; i < k_text_size; ++i) {
        instruction *inst = mem_image.k_text_seg[i];
        if (inst == nullptr) {
            continue;
        }
        unsigned prof_count = mem_image.k_text_prof[i];
        mem_addr addr = K_TEXT_BOT + (i << 2);
        fprintf(file, "%9d ", prof_count - 1);

        instruction::format_an_inst(ss, inst, addr);

        // print_inst_internal (&buf[10], sizeof(buf)-12, inst, addr);
        fprintf(file, "%s", ss.str().c_str());
        ss.clear();
    }

    fclose(file);
}

/* Expand the data segment by adding N bytes. */

void mem_image_t::expand_data(int addl_bytes) {
    mem_image_t &mem_image = *this;

    int delta = ROUND_UP(addl_bytes, BYTES_PER_WORD); /* Keep word aligned */
    int old_size = mem_image.data_top - DATA_BOT;
    int new_size = old_size + delta;

    if ((addl_bytes < 0) || (new_size > this->data_limit)) {
        error("Can't expand data segment by %d bytes to %d bytes\n", addl_bytes, new_size);
        run_error("Use -ldata # with # > %d\n", new_size);
    }

    try {
        if (new_size % BYTES_PER_WORD != 0) {
            printf("new_size = %d is not a multiple of Word length = %d", new_size,
                   BYTES_PER_WORD);  // XXX: LOG OUT
        }
        mem_image.data_seg.resize(new_size / BYTES_PER_WORD);
    } catch (std::bad_alloc &) {
        fatal_error("realloc failed in expand_data\n");
    }

    mem_image.data_seg_b = (BYTE_TYPE *)mem_image.data_seg.data();
    mem_image.data_seg_h = (short *)mem_image.data_seg.data();
    mem_image.data_top += delta;

    /* Zero new memory. Refactorer Comment: Honestly, I don't know why we don't use memclr */
    for (BYTE_TYPE *p = mem_image.data_seg_b + old_size; p < mem_image.data_seg_b + new_size;) {
        *p++ = 0;
    }
}

/* Expand the stack segment by adding N bytes.  Can't use REALLOC
   since it copies from bottom of memory blocks and stack grows down from
   top of its block. */

void mem_image_t::expand_stack(int addl_bytes) {
    mem_image_t &mem_image = *this;

    int delta = ROUND_UP(addl_bytes, BYTES_PER_WORD); /* Keep word aligned */
    int old_size = STACK_TOP - mem_image.stack_bot;
    int new_size = old_size + MAX(delta, old_size);

    std::vector<mem_word> new_seg = {};

    if ((addl_bytes < 0) || (new_size > this->stack_limit)) {
        run_error(
            "Can't expand stack segment by %d bytes to %d bytes.\nUse -lstack # with # > %d\n",
            addl_bytes, new_size, new_size);
    }

    if (new_size % BYTES_PER_WORD != 0) {
        printf("new_size = %d is not a multiple of Word length = %d", new_size,
               BYTES_PER_WORD);  // XXX: LOG OUT
    }

    new_seg.resize(new_size / BYTES_PER_WORD);
    std::fill(new_seg.begin(), new_seg.end(), 0);

    // Refactorer note: probably po = pointer old, pn = pointer new
    mem_word *po = mem_image.stack_seg.data() + (old_size / BYTES_PER_WORD - 1);
    mem_word *pn = new_seg.data() + (new_size / BYTES_PER_WORD - 1);
    for (; po >= mem_image.stack_seg.data();) {
        *pn-- = *po--;
    }

    // Need Move semantics here
    mem_image.stack_seg = std::move(new_seg);
    mem_image.stack_seg_b = (BYTE_TYPE *)mem_image.stack_seg.data();
    mem_image.stack_seg_h = (short *)mem_image.stack_seg.data();
    mem_image.stack_bot -= (new_size - old_size);
}

/* Expand the kernel data segment by adding N bytes. */

void mem_image_t::expand_k_data(int addl_bytes) {
    mem_image_t &mem_image = *this;

    int delta = ROUND_UP(addl_bytes, BYTES_PER_WORD); /* Keep word aligned */
    int old_size = mem_image.k_data_top - K_DATA_BOT;
    int new_size = old_size + delta;

    if ((addl_bytes < 0) || (new_size > this->k_data_limit)) {
        run_error(
            "Can't expand kernel data segment by %d bytes to %d bytes.\nUse -lkdata # with # > "
            "%d\n",
            addl_bytes, new_size, new_size);
    }

    try {
        if (new_size % BYTES_PER_WORD != 0) {
            printf("new_size = %d is not a multiple of Word length = %d", new_size,
                   BYTES_PER_WORD);  // XXX: LOG OUT
        }
        mem_image.k_data_seg.resize(new_size / BYTES_PER_WORD);
    } catch (std::bad_alloc &) {
        fatal_error("realloc failed in expand_k_data\n");
    }

    mem_image.k_data_seg_b = (BYTE_TYPE *)mem_image.k_data_seg.data();
    mem_image.k_data_seg_h = (short *)mem_image.k_data_seg.data();
    mem_image.k_data_top += delta;

    /* Zero new memory */
    for (BYTE_TYPE *p = mem_image.k_data_seg_b + old_size / BYTES_PER_WORD;
         p < mem_image.k_data_seg_b + new_size / BYTES_PER_WORD;) {
        *p++ = 0;
    }
}

/* The text segments contain pointers to instructions, not actual
   instructions, so they must be allocated large enough to hold as many
   pointers as there would be instructions (the two differ on machines in
   which pointers are not 32 bits long).  The following calculations round
   up in case size is not a multiple of BYTES_PER_WORD.  */

constexpr uint32_t BYTES_TO_INST(uint32_t N) {
    return (N + BYTES_PER_WORD - 1) / BYTES_PER_WORD * sizeof(instruction *);
}

void mem_image_t::make_memory(int text_size, int data_size, int data_limit, int stack_size,
                              int stack_limit, int k_text_size, int k_data_size, int k_data_limit) {
    mem_image_t &mem_image = *this;

    if (data_size <= 65536) {
        data_size = 65536;
    }
    data_size = ROUND_UP(data_size, BYTES_PER_WORD); /* Keep word aligned */

    if (mem_image.text_seg.empty()) {
        // XXX: Change to reserve and use push_back
        mem_image.text_seg.resize(BYTES_TO_INST(text_size) / BYTES_PER_WORD);
        mem_image.text_prof.resize(text_size / sizeof(unsigned));
    } else {
        free_instructions(mem_image.text_seg, (mem_image.text_top - TEXT_BOT) / BYTES_PER_WORD);
        mem_image.text_seg.resize(BYTES_TO_INST(text_size) / BYTES_PER_WORD);
        mem_image.text_prof.resize(text_size / sizeof(unsigned));
    }
    std::fill(mem_image.text_seg.begin(), mem_image.text_seg.end(), nullptr);
    std::fill(mem_image.text_prof.begin(), mem_image.text_prof.end(), 0);
    mem_image.text_top = TEXT_BOT + text_size;

    data_size = ROUND_UP(data_size, BYTES_PER_WORD); /* Keep word aligned */
    if (mem_image.data_seg.empty()) {
        mem_image.data_seg.resize(data_size / BYTES_PER_WORD);
    } else {
        mem_image.data_seg.resize(data_size / BYTES_PER_WORD);
    }
    std::fill(mem_image.data_seg.begin(), mem_image.data_seg.end(), 0);

    mem_image.data_seg_b = (BYTE_TYPE *)mem_image.data_seg.data();
    mem_image.data_seg_h = (short *)mem_image.data_seg.data();
    mem_image.data_top = DATA_BOT + data_size;
    mem_image.data_limit = data_limit;

    stack_size = ROUND_UP(stack_size, BYTES_PER_WORD); /* Keep word aligned */
    if (mem_image.stack_seg.empty()) {
        mem_image.stack_seg.resize(stack_size / BYTES_PER_WORD);
    } else {
        mem_image.stack_seg.resize(stack_size / BYTES_PER_WORD);
    }
    std::fill(mem_image.stack_seg.begin(), mem_image.stack_seg.end(), 0);
    mem_image.stack_seg_b = (BYTE_TYPE *)mem_image.stack_seg.data();
    mem_image.stack_seg_h = (short *)mem_image.stack_seg.data();
    mem_image.stack_bot = STACK_TOP - stack_size;
    mem_image.stack_limit = stack_limit;

    if ((SPECIAL_TOP - SPECIAL_BOT) % BYTES_PER_WORD != 0) {
        printf("The special data section size is not a multiple of 4");  // XXX: Log out
    }
    if (mem_image.special_seg.empty()) {
        mem_image.special_seg.resize((SPECIAL_TOP - SPECIAL_BOT) / BYTES_PER_WORD);
        mem_image.special_seg_b = (BYTE_TYPE *)mem_image.special_seg.data();
        mem_image.special_seg_h = (short *)mem_image.special_seg.data();
    }
    std::fill(mem_image.special_seg.begin(), mem_image.special_seg.end(), 0);

    if (mem_image.k_text_seg.empty()) {
        mem_image.k_text_seg.resize(BYTES_TO_INST(k_text_size) / BYTES_PER_WORD);
        mem_image.k_text_prof.resize(k_text_size / sizeof(unsigned));
    } else {
        free_instructions(mem_image.k_text_seg,
                          (mem_image.k_text_top - K_TEXT_BOT) / BYTES_PER_WORD);
        mem_image.k_text_seg.resize(BYTES_TO_INST(k_text_size) / BYTES_PER_WORD);
        mem_image.k_text_prof.resize(k_text_size / sizeof(unsigned));
    }
    std::fill(mem_image.k_text_seg.begin(), mem_image.k_text_seg.end(), nullptr);
    std::fill(mem_image.k_text_prof.begin(), mem_image.k_text_prof.end(), 0);
    mem_image.k_text_top = K_TEXT_BOT + k_text_size;

    k_data_size = ROUND_UP(k_data_size, BYTES_PER_WORD); /* Keep word aligned */
    if (mem_image.k_data_seg.empty()) {
        mem_image.k_data_seg.resize(k_data_size / BYTES_PER_WORD);
    } else {
        mem_image.k_data_seg.resize(k_data_size / BYTES_PER_WORD);
    }
    std::fill(mem_image.k_data_seg.begin(), mem_image.k_data_seg.end(), 0);
    mem_image.k_data_seg_b = (BYTE_TYPE *)mem_image.k_data_seg.data();
    mem_image.k_data_seg_h = (short *)mem_image.k_data_seg.data();
    mem_image.k_data_top = K_DATA_BOT + k_data_size;
    mem_image.k_data_limit = k_data_limit;

    mem_image.text_modified = true;
    mem_image.data_modified = true;
}

/* Free the storage used by the old instructions in memory. */

void mem_image_t::free_instructions(std::vector<instruction *> &inst, int n) {
    if (n > inst.size()) {
        printf("Too many instructions freed!");  // XXX: Logger
    }

    for (int i = 0; i < n; ++i) {
        if (inst[i] != nullptr) {
            free_inst(inst[i]);
        }
    }
}

/* Access memory */

void *mem_image_t::mem_reference(mem_addr addr) const {
    const mem_image_t &mem_image = *this;

    if ((addr >= TEXT_BOT) && (addr < mem_image.text_top)) {
        return addr - TEXT_BOT + (char *)mem_image.text_seg.data();
    } else if ((addr >= DATA_BOT) && (addr < mem_image.data_top)) {
        return addr - DATA_BOT + (char *)mem_image.data_seg.data();
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP)) {
        return addr - mem_image.stack_bot + (char *)mem_image.stack_seg.data();
    } else if ((addr >= K_TEXT_BOT) && (addr < mem_image.k_text_top)) {
        return addr - K_TEXT_BOT + (char *)mem_image.k_text_seg.data();
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top)) {
        return addr - K_DATA_BOT + (char *)mem_image.k_data_seg.data();
    } else {
        run_error("Memory address out of bounds\n");
        return nullptr;
    }
}
