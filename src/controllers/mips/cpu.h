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

#include <stddef.h>

#include <unordered_map>

#include "TODO/spim-utils.h"
#include "config.h"
#include "inst.h"
#include "mem.h"
#include "reg.h"
#include "scanner.h"
#include "sym-tbl.h"
#include "syscall.h"

#include "../../engine/controller.h"

/**
 * Wrapper class that defines the topology and owns all the components
 */
class CPU {
   private:
    int id;  // CPU id (used for easy identification). Be sure to use this for logging statements
    CPUConfig config;
    mem_image_t memory;
    reg_image_t registers;
    SymbolTable symbol_table;

    std::unordered_map<mem_addr, bkpt> breakpoints;

    mem_addr last_exception_addr;

    bool force_break;           /* => stop interpreter loop  */
    bool parser_error_occurred; /* => parse resulted in error */
    int spim_return_value;      /* Value returned when spim exits */

    /* True when delayed_branches is true and instruction is executing in delay
     * slot of another instruction.
     */
    int running_in_delay_slot = 0;

    /* Actual type of structure pointed to depends on X/terminal interface */

    /* SpimBOT stuff */
    bool map_click;

    bool done;  // Should the simulation stop with no hope of restarting?

    /* Helper functions */
    // raises the exception. Try to move this to register if possible
    void _raise_exception(int excode);

    // Resolves things based on config
    bool is_file_io_enabled;

    bool is_syscalls_enabled;  // TODO: Make this a bitset to detemine which syscalls are enabled

    bool debug;  // TODO: Convert to enum and move to logger

    bool quiet;

    bool should_fail_on_exception;  // Find a better name

    bool data_dir;
    bool text_dir;

    /*
     * MEMORY SPECIFIC METHODS
     */

    /* Handle the infrequent and erroneous cases in memory accesses. */
    instruction *bad_text_read(mem_addr addr);
    void bad_text_write(mem_addr addr, instruction *inst);
    mem_word bad_mem_read(mem_addr addr, int mask);
    void bad_mem_write(mem_addr addr, mem_word value, int mask);

    /* Memory-mapped IO routines. */

    /**
     * Check if input is available and output is possible.  If so, update the
     * memory-mapped control registers and buffers.
     */
    void check_memory_mapped_IO();

    /* Misc. routines */
    void print_mem(mem_addr addr);

    /* Instruction Routines */
    void resolve_a_label_sub(label *sym, instruction *inst, mem_addr pc);

    /* Data methods */
    inline mem_addr DATA_PC() const;

   public:
    CPU(const CPUConfig &config);

    bool at_breakpoint() const {return inst_is_breakpoint(this->registers.PC)}

    MemoryBusMessage single_step(bool display, bool cont_bkpt, bool* continuable, UIEvent event);

    bool run_program(int steps, bool display, bool cont_bkpt, bool *continuable);

    /* Utilities */

    /* Read file NAME, which should contain assembly code. Return true if
       successful and false otherwise. */
    bool read_assembly_file(const char *fpath, const char *file_name);

    mem_addr starting_address();

    /* Set a breakpoint at memory location ADDR. */

    void add_breakpoint(mem_addr addr);

    /* Delete all breakpoints at memory location ADDR. */

    void delete_breakpoint(mem_addr addr);

    void delete_all_breakpoints();

    /* List all breakpoints. */

    void list_breakpoints();

    /* Initialize the SPIM stack with ARGC, ARGV, and ENVP data. */
    void initialize_run_stack(int argc, char **argv);

    mem_addr copy_str_to_stack(char *s);
    mem_addr copy_int_to_stack(int n);

    /* Decides which syscall to execute or simulate.
     *
     * Config Params:
     *  - enable_syscalls (if spimbot_tournament is true this should be false)
     *  - enable_file_io (if spimbot_tournament is true this should also be false)
     *  - debug (allows for print statements)
     *
     * Returns zero upon exit syscall and non-zero to continue execution.
     */
    // int do_syscall(bool enable_syscalls = true, bool enable_file_io = false, bool debug = false);
    int do_syscall();

    /* Handles eceptions raised by the CPU
     *
     * Config Params:
     *  - fail_on_exception (if spimbot_tournament is true this should also be true)
     *  - quiet (disables error printouts)
     *
     * Returns zero upon exit syscall and non-zero to continue execution.
     */
    // void handle_exception(bool fail_on_exception = false, bool quiet = false);
    void handle_exception();

    /*
     * Run the program stored in memory, starting at address PC for
     * STEPS_TO_RUN instruction executions.  If flag DISPLAY is true, print
     * each instruction before it executes. Return true if program's
     * execution can continue.
     */
    bool run_spim(bool display);

    /**
     * Returns true if you should
     */
    bool RAISE_EXCEPTION(int CAUSE);
    void RAISE_INTERRUPT(int32_t LEVEL);
    void CLEAR_INTERRUPT(int32_t LEVEL);

    /*
     * MEMORY SPECIFIC METHODS
     * Convert to Interface
     */
    void *mem_reference(mem_addr addr) const;

    instruction *read_mem_inst(mem_addr addr);
    reg_word read_mem_byte(mem_addr addr);
    reg_word read_mem_half(mem_addr addr);
    reg_word read_mem_word(mem_addr addr);

    void set_mem_inst(mem_addr addr, instruction *inst);
    void set_mem_byte(mem_addr addr, reg_word value);
    void set_mem_half(mem_addr addr, reg_word value);
    void set_mem_word(mem_addr addr, reg_word value);

    /*
     * INSTRUCTION SPECIFIC METHODS
     */

    /* Increment the current text segement PC. */
    void increment_text_pc(int delta);

    /**
     * If FLAG is true, next instruction goes to kernel text segment,
     * otherwise it goes to user segment.
     */
    void user_kernel_text_segment(bool to_kernel);

    /* Store an INSTRUCTION in memory at the next location. */
    void store_instruction(instruction *inst);

    /* Print the instruction stored at the memory ADDRESS. */

    void print_inst(port message_out, mem_addr addr);

    std::string inst_to_string(mem_addr addr);

    /* Return an address expression of the form SYMBOL +/- IOFFSET (REGISTER).
   Any of the three parts may be omitted. */

    addr_expr *make_addr_expr(int offs, char *sym, int reg_no);

    /* Symbol Table Methods */

    /**
     * Resolve the newly-defined label in INSTRUCTION.
     */
    void resolve_a_label(label *sym, instruction *inst);

    /**
     * Given a newly-defined LABEL, resolve the previously encountered
     * instructions and data locations that refer to the label.
     */
    void resolve_label_uses(label *sym);

    /**
     * Record that the label named NAME refers to ADDRESS.	If RESOLVE_USES is
     * true, resolve all references to it.  Return the label structure.
     */
    label *record_label(const std::string &name, mem_addr address, bool resolve_uses) {
        label *l = symbol_table.lookup_label(name);

        if (!l->gp_flag) {
            if (l->addr != 0) {
                yyerror("Label is defined for the second time");
                return (l);
            }
            l->addr = address;
        }

        if (resolve_uses) {
            resolve_label_uses(l);
        }

        if (!l->global_flag) {
            symbol_table.local_labels.push_front(std::ref(*l));
        }
        return l;
    }

    /* Return true if a breakpoint is set at ADDR. */
    bool inst_is_breakpoint(mem_addr addr) const;

    /* Return a jump-type instruction with the given OPCODE and TARGET
       fields. NB, even the immediate value may not fit in the field, this
       routine will not produce more than one instruction. */
    void j_type_inst(int opcode, imm_expr *target);

    /* Return a register-type instruction with the given OPCODE, FD, FS, and FT
       fields. */
    void r_co_type_inst(int opcode, int fd, int fs, int ft);

    /* Return a register-type instruction with the given OPCODE, RD, RS, and RT
       fields. */

    void r_type_inst(int opcode, int rd, int rs, int rt);

    /* Return a register-shift instruction with the given OPCODE, RD, RT, and
       SHAMT fields.*/

    void r_sh_type_inst(int opcode, int rd, int rt, int shamt);

    /* Return a floating-point compare instruction with the given OPCODE,
       FS, FT, and CC fields.*/

    void r_cond_type_inst(int opcode, int fs, int ft, int cc);

    /* Produce an immediate instruction with the OPCODE, RT, RS, and IMM
   fields.  NB, because the immediate value may not fit in the field,
   this routine may produce more than one instruction.	On the bare
   machine, we resolve symbolic address, but they better produce values
   that fit into instruction's immediate field. */

    void i_type_inst(int opcode, int rt, int rs, imm_expr *expr);

    void i_type_inst_free(int opcode, int rt, int rs, imm_expr *expr);

    void free_inst(instruction *inst);

    void i_type_inst_full_word(int opcode, int rt, int rs, imm_expr *expr, int value_known,
                               int32 value);

    /* Code to test encode/decode of instructions. */

    inline void test_assembly(instruction *inst) {
        instruction *new_inst =
            instruction::inst_decode(inst->inst_encode(config.correct_branches));

        if (inst != new_inst) {
            std::stringstream stream;

            stream << "=================== Not Equal ===================\n";
            instruction::format_an_inst(stream, inst, 0);
            instruction::format_an_inst(stream, new_inst, 0);
            stream << "=================== Not Equal ===================\n";
        }
        free_inst(new_inst);
    }

    /* DATA Methods */
    void align_data(int alignment);
    mem_addr current_data_pc() const;
    void data_begins_at_point(mem_addr addr);
    void enable_data_alignment();
    void end_of_assembly_file();
    void extern_directive(const std::string &name, int size);
    void increment_data_pc(int value);
    void k_data_begins_at_point(mem_addr addr);
    void lcomm_directive(const std::string &name, int size);
    void set_data_alignment(int);
    void set_data_pc(mem_addr addr);
    void set_text_pc(mem_addr addr);
    void store_byte(int value);
    void store_double(double *value);
    void store_float(double *value);
    void store_half(int value);
    void store_string(char *string, int length, bool null_terminate);
    void store_word(int value);
    void user_kernel_data_segment(bool to_kernel);

    friend struct mem_image_t;
    friend struct reg_image_t;
    friend struct instruction;
    friend struct SymbolTable;
};