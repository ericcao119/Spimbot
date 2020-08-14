/* SPIM S20 MIPS simulator.
   Execute SPIM instructions.

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

#ifdef mips
#define _IEEE 1
#include <nan.h>
#else

bool NaN(float X) { return X != X; }
bool NaN(double X) { return X != X; }

#endif

#include <math.h>
#include <stdio.h>

#ifdef _WIN32
#define VC_EXTRALEAN
#include <Windows.h>
#else
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#endif

#include "inst.h"
#include "mem.h"
#include "parser_yacc.h"
#include "reg.h"
#include "run.h"
#include "spim-utils.h"
#include "spim.h"
#include "string-stream.h"
#include "sym-tbl.h"
#include "syscall.h"

#ifdef _MSC_BUILD
/* Disable MS VS warning about constant predicate in conditional. */
#pragma warning(disable : 4127)
#endif

/* Local functions: */

static void set_fpu_cc(reg_image_t &reg_image, int cond, int cc, int less, int equal,
                       int unordered);
static void signed_multiply(size_t context, reg_word v1, reg_word v2);
static void unsigned_multiply(size_t context, reg_word v1, reg_word v2);

constexpr uint32_t SIGN_BIT(uint32_t X) { return X & 0x80000000; }
constexpr int32_t SIGN_BIT(int32_t X) { return X & 0x80000000; }

// #define ARITH_OVFL(RESULT, OP1, OP2) (SIGN_BIT(OP1) == SIGN_BIT(OP2) && SIGN_BIT(OP1) !=
// SIGN_BIT(RESULT))

constexpr bool ARITH_OVFL(uint32_t RESULT, uint32_t OP1, uint32_t OP2) {
    return SIGN_BIT(OP1) == SIGN_BIT(OP2) && SIGN_BIT(OP1) != SIGN_BIT(RESULT);
}

constexpr bool ARITH_OVFL(int32_t RESULT, int32_t OP1, int32_t OP2) {
    return SIGN_BIT(OP1) == SIGN_BIT(OP2) && SIGN_BIT(OP1) != SIGN_BIT(RESULT);
}

bool CPU::run_spim(bool display) {
    // Initialize variables for use in lambas
    reg_image_t &reg_image = this->registers;

    static reg_word *delayed_load_addr1 = nullptr;
    static reg_word *delayed_load_addr2 = nullptr;
    static reg_word delayed_load_value1 = 0;
    static reg_word delayed_load_value2 = 0;

    reg_image.R[0] = 0; /* Maintain invariant value */

    /*
     * Executed delayed branch and jump instructions by running the
     * instruction from the delay slot before transfering control.  Note,
     * in branches that don't jump, the instruction in the delay slot is
     * executed by falling through normally.
     *
     * We take advantage of the MIPS architecture, which leaves undefined
     * the result of executing a delayed instruction in a delay slot.  Here
     * we execute the second branch.
     */

    // Convenient lambdas to replace macros
    auto JUMP_INST = [=](mem_addr TARGET) {
        if (this->config.delayed_branches) {
            running_in_delay_slot = 1;
            this->run_spim(display);
            running_in_delay_slot = 0;
        } /* -4 since PC is bumped after this inst */
        this->registers.PC = TARGET - BYTES_PER_WORD;
    };

    auto BRANCH_INST = [=](bool TEST, mem_addr TARGET, bool NULLIFY) {
        if (TEST) {
            mem_addr target = TARGET;
            if (this->config.delayed_branches) {
                /* +4 since jump in delay slot */
                target += BYTES_PER_WORD;
            }
            JUMP_INST(target);
        } else if (NULLIFY) {
            /* If test fails and nullify bit set, skip
           instruction in delay slot. */
            this->registers.PC += BYTES_PER_WORD;
        }
    };

    /* If the delayed_load flag is false, the result from a load is available
     * immediate.  If the delayed_load flag is true, the result from a load is
     * not available until the subsequent instruction has executed (as in the
     * real machine). We need a two element shift register for the value and its
     * destination, as the instruction following the load can itself be a load
     * instruction.
     */
    auto LOAD_INST_BASE = [=](reg_word *DEST_A, reg_word VALUE) {
        if (this->config.delayed_loads) {
            delayed_load_addr1 = (DEST_A);
            delayed_load_value1 = (VALUE);
        } else {
            *(DEST_A) = (VALUE);
        }
    };

    auto LOAD_INST = [=](reg_word *DEST_A, reg_word LD, reg_word MASK) {
        LOAD_INST_BASE(DEST_A, (LD & (MASK)));
    };

    auto DO_DELAYED_UPDATE = [=]() {
        if (this->config.delayed_loads) { /* Check for delayed updates */
            if (delayed_load_addr2 != nullptr) {
                *delayed_load_addr2 = delayed_load_value2;
            }
            delayed_load_addr2 = delayed_load_addr1;
            delayed_load_value2 = delayed_load_value1;
            delayed_load_addr1 = nullptr;
        }
    };

    instruction *inst = this->read_mem_inst(reg_image.PC);
    if (reg_image.exception_occurred) {
        reg_image.exception_occurred = false;
        this->handle_exception();
        return true;
    } else if (inst == nullptr) {
        run_error("Attempt to execute non-instruction at 0x%08x\n", reg_image.PC);
        return false;
    } else if (inst->EXPR() != nullptr && inst->EXPR()->symbol != nullptr &&
               inst->EXPR()->symbol->addr == 0) {
        run_error("Instruction references undefined symbol at 0x%08x\n  %s", reg_image.PC,
                  inst_to_string(reg_image.PC));
        return false;
    }

    if (display) {
        print_inst(config.message_out, reg_image.PC);
    }

#ifdef TEST_ASM
    test_assembly(inst);  // XXX: Need to check
#endif

    DO_DELAYED_UPDATE();

    switch (inst->OPCODE()) {
        case Y_ADD_OP: {
            reg_word vs = reg_image.R[inst->RS()], vt = reg_image.R[inst->RT()];
            reg_word sum = vs + vt;

            if (ARITH_OVFL(sum, vs, vt)) {
                bool exception_raised = this->RAISE_EXCEPTION(ExcCode_Ov);
                if (exception_raised) {
                    break;
                }
            }
            reg_image.R[inst->RD()] = sum;
            break;
        }

        case Y_ADDI_OP: {
            reg_word vs = reg_image.R[inst->RS()], imm = (short)inst->IMM();
            reg_word sum = vs + imm;

            if (ARITH_OVFL(sum, vs, imm)) {
                bool exception_raised = this->RAISE_EXCEPTION(ExcCode_Ov);
                if (exception_raised) {
                    break;
                }
            }
            reg_image.R[inst->RT()] = sum;
            break;
        }

        case Y_ADDIU_OP: {
            reg_image.R[inst->RT()] = reg_image.R[inst->RS()] + (short)inst->IMM();
            break;
        }
        case Y_ADDU_OP: {
            reg_image.R[inst->RD()] = reg_image.R[inst->RS()] + reg_image.R[inst->RT()];
            break;
        }
        case Y_AND_OP: {
            reg_image.R[inst->RD()] = reg_image.R[inst->RS()] & reg_image.R[inst->RT()];
            break;
        }
        case Y_ANDI_OP: {
            reg_image.R[inst->RT()] = reg_image.R[inst->RS()] & (0xffff & inst->IMM());
            break;
        }
        case Y_BC2F_OP:
        case Y_BC2FL_OP:
        case Y_BC2T_OP:
        case Y_BC2TL_OP: {
            bool exception_raised = this->RAISE_EXCEPTION(ExcCode_CpU);
            break;
        }
        case Y_BEQ_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] == reg_image.R[inst->RT()],
                        reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BEQL_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] == reg_image.R[inst->RT()],
                        reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BGEZ_OP: {
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) == 0, reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BGEZL_OP: {
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) == 0, reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BGEZAL_OP: {
            reg_image.R[31] = reg_image.PC +
                              (this->config.delayed_branches ? 2 * BYTES_PER_WORD : BYTES_PER_WORD);
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) == 0, reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BGEZALL_OP: {
            reg_image.R[31] = reg_image.PC +
                              (this->config.delayed_branches ? 2 * BYTES_PER_WORD : BYTES_PER_WORD);
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) == 0, reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BGTZ_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] != 0 && SIGN_BIT(reg_image.R[inst->RS()]) == 0,
                        reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BGTZL_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] != 0 && SIGN_BIT(reg_image.R[inst->RS()]) == 0,
                        reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BLEZ_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] == 0 || SIGN_BIT(reg_image.R[inst->RS()]) != 0,
                        reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BLEZL_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] == 0 || SIGN_BIT(reg_image.R[inst->RS()]) != 0,
                        reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BLTZ_OP: {
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) != 0, reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BLTZL_OP: {
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) != 0, reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BLTZAL_OP: {
            reg_image.R[31] = reg_image.PC +
                              (this->config.delayed_branches ? 2 * BYTES_PER_WORD : BYTES_PER_WORD);
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) != 0, reg_image.PC + inst->IDISP(), 0);
            break;
        }

        case Y_BLTZALL_OP: {
            reg_image.R[31] = reg_image.PC +
                              (this->config.delayed_branches ? 2 * BYTES_PER_WORD : BYTES_PER_WORD);
            BRANCH_INST(SIGN_BIT(reg_image.R[inst->RS()]) != 0, reg_image.PC + inst->IDISP(), 1);
            break;
        }

        case Y_BNE_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] != reg_image.R[inst->RT()],
                        reg_image.PC + inst->IDISP(), 0);
            break;
        }
        case Y_BNEL_OP: {
            BRANCH_INST(reg_image.R[inst->RS()] != reg_image.R[inst->RT()],
                        reg_image.PC + inst->IDISP(), 1);
            break;
        }
        case Y_BREAK_OP: {
            if (inst->RD() == 1) {  // XXX: Double check this
                /* Debugger breakpoint */
                bool exception_raised = this->RAISE_EXCEPTION(ExcCode_Bp);
                if (exception_raised) {
                    return true;
                }
            } else {
                bool exception_raised = this->RAISE_EXCEPTION(ExcCode_Bp);
                if (exception_raised) {
                    break;
                }
            }
        }
        case Y_CACHE_OP: {
            break; /* Memory details not implemented */
        }
        case Y_CFC0_OP: {
            reg_image.R[inst->RT()] = reg_image.CCR[0][inst->RD()];
            break;
        }
        case Y_CFC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU);
            break;
        }
        case Y_CLO_OP: {
            reg_word val = reg_image.R[inst->RS()];
            int i;
            for (i = 31; 0 <= i; i -= 1) {
                if (((val >> i) & 0x1) == 0) {
                    break;
                }
            }

            reg_image.R[inst->RD()] = 31 - i;
            break;
        }

        case Y_CLZ_OP: {
            reg_word val = reg_image.R[inst->RS()];
            int i;
            for (i = 31; 0 <= i; i -= 1) {
                if (((val >> i) & 0x1) == 1) {
                    break;
                }
            }

            reg_image.R[inst->RD()] = 31 - i;
            break;
        }

        case Y_COP2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }
        case Y_CTC0_OP: {
            reg_image.CCR[0][inst->RD()] = reg_image.R[inst->RT()];
            break;
        }

        case Y_CTC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_DIV_OP: { /* The behavior of this instruction is undefined on divide by
                 zero or overflow. */
            if (reg_image.R[inst->RT()] != 0 &&
                !(reg_image.R[inst->RS()] == (reg_word)0x80000000 &&
                  reg_image.R[inst->RT()] == (reg_word)0xffffffff)) {
                reg_image.LO =
                    (reg_word)reg_image.R[inst->RS()] / (reg_word)reg_image.R[inst->RT()];
                reg_image.HI =
                    (reg_word)reg_image.R[inst->RS()] % (reg_word)reg_image.R[inst->RT()];
            }
            break;
        }

        case Y_DIVU_OP: { /* The behavior of this instruction is undefined on divide by
                 zero or overflow. */
            if (reg_image.R[inst->RT()] != 0 &&
                !(reg_image.R[inst->RS()] == (reg_word)0x80000000 &&
                  reg_image.R[inst->RT()] == (reg_word)0xffffffff)) {
                reg_image.LO =
                    (u_reg_word)reg_image.R[inst->RS()] / (u_reg_word)reg_image.R[inst->RT()];
                reg_image.HI =
                    (u_reg_word)reg_image.R[inst->RS()] % (u_reg_word)reg_image.R[inst->RT()];
            }
            break;
        }

        case Y_ERET_OP: {
            reg_image.CP0_Status() &= ~CP0_Status_EXL; /* Clear EXL bit */
            JUMP_INST(reg_image.CP0_EPC());            /* Jump to EPC */
            break;
        }

        case Y_J_OP: {
            JUMP_INST(((reg_image.PC & 0xf0000000) | inst->TARGET() << 2));
            break;
        }

        case Y_JAL_OP: {
            if (this->config.delayed_branches) {
                reg_image.R[31] = reg_image.PC + 2 * BYTES_PER_WORD;
            } else {
                reg_image.R[31] = reg_image.PC + BYTES_PER_WORD;
            }
            JUMP_INST(((reg_image.PC & 0xf0000000) | (inst->TARGET() << 2)));
            break;
        }

        case Y_JALR_OP: {
            mem_addr tmp = reg_image.R[inst->RS()];

            if (this->config.delayed_branches) {
                reg_image.R[inst->RD()] = reg_image.PC + 2 * BYTES_PER_WORD;
            } else {
                reg_image.R[inst->RD()] = reg_image.PC + BYTES_PER_WORD;
            }
            JUMP_INST(tmp);
        } break;

        case Y_JR_OP: {
            mem_addr tmp = reg_image.R[inst->RS()];
            JUMP_INST(tmp);
            break;
        }

        case Y_LB_OP: {
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_byte(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffffffff);
            break;
        }

        case Y_LBU_OP: {
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_byte(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xff);
            break;
        }

        case Y_LH_OP: {
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_half(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffffffff);
            break;
        }

        case Y_LHU_OP: {
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_half(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffff);
            break;
        }

        case Y_LL_OP: { /* Uniprocess, so this instruction is just a load */
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffffffff);
            break;
        }

        case Y_LUI_OP: {
            reg_image.R[inst->RT()] = (inst->IMM() << 16) & 0xffff0000;
            break;
        }

        case Y_LW_OP: {
            LOAD_INST(&reg_image.R[inst->RT()],
                      read_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffffffff);
            break;
        }

        case Y_LDC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_LWC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_LWL_OP: {
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            reg_word word; /* Can't be register */
            int byte = addr & 0x3;
            reg_word reg_val = reg_image.R[inst->RT()];

            word = read_mem_word(addr & 0xfffffffc);
            if (!reg_image.exception_occurred) {
#ifdef SPIM_BIGENDIAN
                switch (byte) {
                    case 0: {
                        word = word;
                        break;
                    }

                    case 1: {
                        word = ((word & 0xffffff) << 8) | (reg_val & 0xff);
                        break;
                    }
                    case 2: {
                        word = ((word & 0xffff) << 16) | (reg_val & 0xffff);
                        break;
                    }
                    case 3: {
                        word = ((word & 0xff) << 24) | (reg_val & 0xffffff);
                        break;
                    }
                }
#else
                switch (byte) {
                    case 0: {
                        word = ((word & 0xff) << 24) | (reg_val & 0xffffff);
                        break;
                    }

                    case 1: {
                        word = ((word & 0xffff) << 16) | (reg_val & 0xffff);
                        break;
                    }
                    case 2: {
                        word = ((word & 0xffffff) << 8) | (reg_val & 0xff);
                        break;
                    }
                    case 3: {
                        break;
                    }
                }
#endif
            }
            LOAD_INST_BASE(&reg_image.R[inst->RT()], word);
            break;
        }

        case Y_LWR_OP: {
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            reg_word word; /* Can't be register */
            int byte = addr & 0x3;
            reg_word reg_val = reg_image.R[inst->RT()];

            word = read_mem_word(addr & 0xfffffffc);
            if (!reg_image.exception_occurred) {
#ifdef SPIM_BIGENDIAN
                switch (byte) {
                    case 0: {
                        word = (reg_val & 0xffffff00) | ((unsigned)(word & 0xff000000) >> 24);
                        break;
                    }

                    case 1: {
                        word = (reg_val & 0xffff0000) | ((unsigned)(word & 0xffff0000) >> 16);
                        break;
                    }
                    case 2: {
                        word = (reg_val & 0xff000000) | ((unsigned)(word & 0xffffff00) >> 8);
                        break;
                    }
                    case 3: {
                        word = word;
                        break;
                    }
                }
#else
                switch (byte) {
                    case 0: {
                        break;
                    }

                    case 1: {
                        word = (reg_val & 0xff000000) | ((word & 0xffffff00) >> 8);
                        break;
                    }
                    case 2: {
                        word = (reg_val & 0xffff0000) | ((word & 0xffff0000) >> 16);
                        break;
                    }
                    case 3: {
                        word = (reg_val & 0xffffff00) | ((word & 0xff000000) >> 24);
                        break;
                    }
                }
#endif
            }
            LOAD_INST_BASE(&reg_image.R[inst->RT()], word);
            break;
        }

        case Y_MADD_OP:
        case Y_MADDU_OP: {
            reg_word lo = reg_image.LO, hi = reg_image.HI;
            reg_word tmp;
            if (inst->OPCODE() == Y_MADD_OP) {
                signed_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            } else {
                /* Y_MADDU_OP */
                unsigned_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            }
            tmp = lo + reg_image.LO;
            if ((unsigned)tmp < (unsigned)reg_image.LO || (unsigned)tmp < (unsigned)lo) {
                /* Addition of low-order word overflows */
                hi += 1;
            }
            reg_image.LO = tmp;
            reg_image.HI = hi + reg_image.HI;
            break;
        }

        case Y_MFC0_OP: {
            reg_image.R[inst->RT()] = reg_image.CPR[0][inst->FS()];
            break;
        }

        case Y_MFC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_MFHI_OP: {
            reg_image.R[inst->RD()] = reg_image.HI;
            break;
        }

        case Y_MFLO_OP: {
            reg_image.R[inst->RD()] = reg_image.LO;
            break;
        }
        case Y_MOVN_OP: {
            if (reg_image.R[inst->RT()] != 0) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RS()];
            }
            break;
        }

        case Y_MOVZ_OP: {
            if (reg_image.R[inst->RT()] == 0) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RS()];
            }
            break;
        }

        case Y_MSUB_OP:
        case Y_MSUBU_OP: {
            reg_word lo = reg_image.LO, hi = reg_image.HI;
            reg_word tmp;

            if (inst->OPCODE() == Y_MSUB_OP) {
                signed_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            } else {
                /* Y_MSUBU_OP */
                unsigned_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            }

            tmp = lo - reg_image.LO;
            if ((unsigned)reg_image.LO > (unsigned)lo) {
                /* Subtraction of low-order word borrows */
                hi -= 1;
            }
            reg_image.LO = tmp;
            reg_image.HI = hi - reg_image.HI;
            break;
        }

        case Y_MTC0_OP: {
            reg_image.CPR[0][inst->FS()] = reg_image.R[inst->RT()];
            switch (inst->FS()) {
                case CP0_Compare_Reg: {
                    reg_image.CP0_Cause() &= ~CP0_Cause_IP7; /* Writing clears HW interrupt 5 */
                    break;
                }

                case CP0_Status_Reg: {
                    reg_image.CP0_Status() &= CP0_Status_Mask;
                    reg_image.CP0_Status() |= ((CP0_Status_CU & 0x30000000) | CP0_Status_UM);
                    break;
                }

                case CP0_Cause_Reg: {
                    reg_image.CPR[0][inst->FS()] &= CP0_Cause_Mask;
                    break;
                }

                case CP0_Config_Reg: {
                    reg_image.CPR[0][inst->FS()] &= CP0_Config_Mask;
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }

        case Y_MTC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_MTHI_OP: {
            reg_image.HI = reg_image.R[inst->RS()];
            break;
        }
        case Y_MTLO_OP: {
            reg_image.LO = reg_image.R[inst->RS()];
            break;
        }
        case Y_MUL_OP: {
            signed_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            reg_image.R[inst->RD()] = reg_image.LO;
            break;
        }
        case Y_MULT_OP: {
            signed_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            break;
        }
        case Y_MULTU_OP: {
            unsigned_multiply(reg_image, reg_image.R[inst->RS()], reg_image.R[inst->RT()]);
            break;
        }
        case Y_NOR_OP: {
            reg_image.R[inst->RD()] = ~(reg_image.R[inst->RS()] | reg_image.R[inst->RT()]);
            break;
        }
        case Y_OR_OP: {
            reg_image.R[inst->RD()] = reg_image.R[inst->RS()] | reg_image.R[inst->RT()];
            break;
        }
        case Y_ORI_OP: {
            reg_image.R[inst->RT()] = reg_image.R[inst->RS()] | (0xffff & inst->IMM());
            break;
        }

        case Y_PREF_OP: {
            break; /* Memory details not implemented */
        }

        case Y_RFE_OP: {
#ifdef MIPS1
            /* This is MIPS-I, not compatible with MIPS32 or the
                 definition of the bits in the CP0 Status register in that
                 architecture. */
            reg_image.CP0_Status() =
                (reg_image.CP0_Status() & 0xfffffff0) | ((reg_image.CP0_Status() & 0x3c) >> 2);
#else
            this->RAISE_EXCEPTION(ExcCode_RI); /* Not MIPS32 instruction */
#endif
            break;
        }

        case Y_SB_OP: {
            this->set_mem_byte(reg_image.R[inst->BASE()] + inst->IOFFSET(),
                               reg_image.R[inst->RT()]);
            break;
        }

        case Y_SC_OP: { /* Uniprocessor, so instruction is just a store */
            this->set_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET(),
                               reg_image.R[inst->RT()]);
            break;
        }

        case Y_SDC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }

        case Y_SH_OP: {
            this->set_mem_half(reg_image.R[inst->BASE()] + inst->IOFFSET(),
                               reg_image.R[inst->RT()]);
            break;
        }

        case Y_SLL_OP: {
            int shamt = inst->SHAMT();

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RT()] << shamt;
            } else {
                reg_image.R[inst->RD()] = reg_image.R[inst->RT()];
            }
            break;
        }

        case Y_SLLV_OP: {
            int shamt = (reg_image.R[inst->RS()] & 0x1f);

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RT()] << shamt;
            } else {
                reg_image.R[inst->RD()] = reg_image.R[inst->RT()];
            }
            break;
        }

        case Y_SLT_OP: {
            if (reg_image.R[inst->RS()] < reg_image.R[inst->RT()]) {
                reg_image.R[inst->RD()] = 1;
            } else {
                reg_image.R[inst->RD()] = 0;
            }
            break;
        }

        case Y_SLTI_OP: {
            if (reg_image.R[inst->RS()] < (short)inst->IMM()) {
                reg_image.R[inst->RT()] = 1;
            } else {
                reg_image.R[inst->RT()] = 0;
            }
            break;
        }
        case Y_SLTIU_OP: {
            int x = (short)inst->IMM();

            if ((u_reg_word)reg_image.R[inst->RS()] < (u_reg_word)x) {
                reg_image.R[inst->RT()] = 1;
            } else {
                reg_image.R[inst->RT()] = 0;
            }
            break;
        }

        case Y_SLTU_OP: {
            if ((u_reg_word)reg_image.R[inst->RS()] < (u_reg_word)reg_image.R[inst->RT()]) {
                reg_image.R[inst->RD()] = 1;
            } else {
                reg_image.R[inst->RD()] = 0;
            }
            break;
        }

        case Y_SRA_OP: {
            int shamt = inst->SHAMT();
            reg_word val = reg_image.R[inst->RT()];

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = val >> shamt;
            } else {
                reg_image.R[inst->RD()] = val;
            }
            break;
        }

        case Y_SRAV_OP: {
            int shamt = reg_image.R[inst->RS()] & 0x1f;
            reg_word val = reg_image.R[inst->RT()];

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = val >> shamt;
            } else {
                reg_image.R[inst->RD()] = val;
            }
            break;
        }

        case Y_SRL_OP: {
            int shamt = inst->SHAMT();
            u_reg_word val = reg_image.R[inst->RT()];

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = val >> shamt;
            } else {
                reg_image.R[inst->RD()] = val;
            }
            break;
        }

        case Y_SRLV_OP: {
            int shamt = reg_image.R[inst->RS()] & 0x1f;
            u_reg_word val = reg_image.R[inst->RT()];

            if (shamt >= 0 && shamt < 32) {
                reg_image.R[inst->RD()] = val >> shamt;
            } else {
                reg_image.R[inst->RD()] = val;
            }
            break;
        }

        case Y_SUB_OP: {
            reg_word vs = reg_image.R[inst->RS()], vt = reg_image.R[inst->RT()];
            reg_word diff = vs - vt;

            if (SIGN_BIT(vs) != SIGN_BIT(vt) && SIGN_BIT(vs) != SIGN_BIT(diff)) {
                bool exception_raised = this->RAISE_EXCEPTION(ExcCode_Ov);
                if (exception_raised) {
                    break;
                }
            }
            reg_image.R[inst->RD()] = diff;
            break;
        }

        case Y_SUBU_OP: {
            reg_image.R[inst->RD()] =
                (u_reg_word)reg_image.R[inst->RS()] - (u_reg_word)reg_image.R[inst->RT()];
            break;
        }

        case Y_SW_OP: {
            this->set_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET(),
                               reg_image.R[inst->RT()]);
            break;
        }
        case Y_SWC2_OP: {
            this->RAISE_EXCEPTION(ExcCode_CpU); /* No Coprocessor 2 */
            break;
        }
        case Y_SWL_OP: {
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            mem_word data;
            reg_word reg = reg_image.R[inst->RT()];
            int byte = addr & 0x3;

            data = this->read_mem_word(addr & 0xfffffffc);
#ifdef SPIM_BIGENDIAN
            switch (byte) {
                case 0: {
                    data = reg;
                    break;
                }

                case 1: {
                    data = (data & 0xff000000) | (reg >> 8 & 0xffffff);
                    break;
                }

                case 2: {
                    data = (data & 0xffff0000) | (reg >> 16 & 0xffff);
                    break;
                }

                case 3: {
                    data = (data & 0xffffff00) | (reg >> 24 & 0xff);
                    break;
                }
            }
#else
            switch (byte) {
                case 0: {
                    data = (data & 0xffffff00) | (reg >> 24 & 0xff);
                    break;
                }

                case 1: {
                    data = (data & 0xffff0000) | (reg >> 16 & 0xffff);
                    break;
                }

                case 2: {
                    data = (data & 0xff000000) | (reg >> 8 & 0xffffff);
                    break;
                }

                case 3: {
                    data = reg;
                    break;
                }
            }
#endif
            this->set_mem_word(addr & 0xfffffffc, data);
            break;
        }

        case Y_SWR_OP: {
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            mem_word data;
            reg_word reg = reg_image.R[inst->RT()];
            int byte = addr & 0x3;

            data = this->read_mem_word(addr & 0xfffffffc);
#ifdef SPIM_BIGENDIAN
            switch (byte) {
                case 0: {
                    data = ((reg << 24) & 0xff000000) | (data & 0xffffff);
                    break;
                }

                case 1: {
                    data = ((reg << 16) & 0xffff0000) | (data & 0xffff);
                    break;
                }

                case 2: {
                    data = ((reg << 8) & 0xffffff00) | (data & 0xff);
                    break;
                }

                case 3: {
                    data = reg;
                    break;
                }
            }
#else
            switch (byte) {
                case 0: {
                    data = reg;
                    break;
                }

                case 1: {
                    data = ((reg << 8) & 0xffffff00) | (data & 0xff);
                    break;
                }

                case 2: {
                    data = ((reg << 16) & 0xffff0000) | (data & 0xffff);
                    break;
                }

                case 3: {
                    data = ((reg << 24) & 0xff000000) | (data & 0xffffff);
                    break;
                }
            }
#endif
            this->set_mem_word(addr & 0xfffffffc, data);
            break;
        }

        case Y_SYNC_OP: {
            break; /* Memory details not implemented */
        }

        case Y_SYSCALL_OP: {
            if (!do_syscall()) {
                return false;
            }
            break;
        }
        case Y_TEQ_OP: {
            if (reg_image.R[inst->RS()] == reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TEQI_OP: {
            if (reg_image.R[inst->RS()] == inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TGE_OP: {
            if (reg_image.R[inst->RS()] >= reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TGEI_OP: {
            if (reg_image.R[inst->RS()] >= inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }
        case Y_TGEIU_OP: {
            if ((u_reg_word)reg_image.R[inst->RS()] >= (u_reg_word)inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TGEU_OP: {
            if ((u_reg_word)reg_image.R[inst->RS()] >= (u_reg_word)reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TLBP_OP: {
            this->RAISE_EXCEPTION(ExcCode_RI); /* TLB not implemented */
            break;
        }

        case Y_TLBR_OP: {
            this->RAISE_EXCEPTION(ExcCode_RI); /* TLB not implemented */
            break;
        }

        case Y_TLBWI_OP: {
            this->RAISE_EXCEPTION(ExcCode_RI); /* TLB not implemented */
            break;
        }

        case Y_TLBWR_OP: {
            this->RAISE_EXCEPTION(ExcCode_RI); /* TLB not implemented */
            break;
        }

        case Y_TLT_OP: {
            if (reg_image.R[inst->RS()] < reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TLTI_OP: {
            if (reg_image.R[inst->RS()] < inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TLTIU_OP: {
            if ((u_reg_word)reg_image.R[inst->RS()] < (u_reg_word)inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TLTU_OP: {
            if ((u_reg_word)reg_image.R[inst->RS()] < (u_reg_word)reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TNE_OP: {
            if (reg_image.R[inst->RS()] != reg_image.R[inst->RT()]) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_TNEI_OP: {
            if (reg_image.R[inst->RS()] != inst->IMM()) {
                this->RAISE_EXCEPTION(ExcCode_Tr);
            }
            break;
        }

        case Y_XOR_OP: {
            reg_image.R[inst->RD()] = reg_image.R[inst->RS()] ^ reg_image.R[inst->RT()];
            break;
        }

        case Y_XORI_OP: {
            reg_image.R[inst->RT()] = reg_image.R[inst->RS()] ^ (0xffff & inst->IMM());
            break;
        }

            /* FPA Operations */

        case Y_ABS_S_OP: {
            reg_image.SET_FPR_S(inst->FD(), fabs(reg_image.FPR_S(inst->FS())));
            break;
        }

        case Y_ABS_D_OP: {
            reg_image.SET_FPR_D(inst->FD(), fabs(reg_image.FPR_D(inst->FS())));
            break;
        }

        case Y_ADD_S_OP: {
            reg_image.SET_FPR_S(inst->FD(),
                                reg_image.FPR_S(inst->FS()) + reg_image.FPR_S(inst->FT()));
            /* Should trap on inexact/overflow/underflow */
            break;
        }

        case Y_ADD_D_OP: {
            reg_image.SET_FPR_D(inst->FD(),
                                reg_image.FPR_D(inst->FS()) + reg_image.FPR_D(inst->FT()));
            /* Should trap on inexact/overflow/underflow */
            break;
        }

        case Y_BC1F_OP:
        case Y_BC1FL_OP:
        case Y_BC1T_OP:
        case Y_BC1TL_OP: {
            int cc = inst->CC();
            int nd = inst->ND(); /* 1 => nullify */
            int tf = inst->TF(); /* 0 => BC1F, 1 => BC1T */
            BRANCH_INST((reg_image.FCCR() & (1 << cc)) == (tf << cc), reg_image.PC + inst->IDISP(),
                        nd);
            break;
        }

        case Y_C_F_S_OP:
        case Y_C_UN_S_OP:
        case Y_C_EQ_S_OP:
        case Y_C_UEQ_S_OP:
        case Y_C_OLT_S_OP:
        case Y_C_OLE_S_OP:
        case Y_C_ULT_S_OP:
        case Y_C_ULE_S_OP:
        case Y_C_SF_S_OP:
        case Y_C_NGLE_S_OP:
        case Y_C_SEQ_S_OP:
        case Y_C_NGL_S_OP:
        case Y_C_LT_S_OP:
        case Y_C_NGE_S_OP:
        case Y_C_LE_S_OP:
        case Y_C_NGT_S_OP: {
            float v1 = reg_image.FPR_S(inst->FS());
            float v2 = reg_image.FPR_S(inst->FT());
            double dv1 = v1, dv2 = v2;
            int cond = inst->COND();
            int cc = inst->FD();

            if (NaN(dv1) || NaN(dv2)) {
                if (cond & COND_IN) {
                    bool exception_raised = this->RAISE_EXCEPTION(ExcCode_FPE);
                    if (exception_raised) {
                        break;
                    }
                }
                set_fpu_cc(reg_image, cond, cc, 0, 0, 1);
            } else {
                set_fpu_cc(reg_image, cond, cc, v1 < v2, v1 == v2, 0);
            }
            break;
        }

        case Y_C_F_D_OP:
        case Y_C_UN_D_OP:
        case Y_C_EQ_D_OP:
        case Y_C_UEQ_D_OP:
        case Y_C_OLT_D_OP:
        case Y_C_OLE_D_OP:
        case Y_C_ULT_D_OP:
        case Y_C_ULE_D_OP:
        case Y_C_SF_D_OP:
        case Y_C_NGLE_D_OP:
        case Y_C_SEQ_D_OP:
        case Y_C_NGL_D_OP:
        case Y_C_LT_D_OP:
        case Y_C_NGE_D_OP:
        case Y_C_LE_D_OP:
        case Y_C_NGT_D_OP: {
            double v1 = reg_image.FPR_D(inst->FS()), v2 = reg_image.FPR_D(inst->FT());
            int cond = inst->COND();
            int cc = inst->FD();

            if (NaN(v1) || NaN(v2)) {
                if (cond & COND_IN) {
                    bool exception_raised = this->RAISE_EXCEPTION(ExcCode_FPE);
                    if (exception_raised) {
                        break;
                    }
                }
                set_fpu_cc(reg_image, cond, cc, 0, 0, 1);
            } else {
                set_fpu_cc(reg_image, cond, cc, v1 < v2, v1 == v2, 0);
            }
        } break;

        case Y_CFC1_OP: {
            reg_image.R[inst->RT()] = reg_image.FCR()[inst->FS()];
            break;
        }

        case Y_CTC1_OP: {
            reg_image.FCR()[inst->FS()] = reg_image.R[inst->RT()];

            if (FIR_REG == inst->FS()) {
                /* Read only register */
                reg_image.FIR() = FIR_MASK;
            } else if (FCCR_REG == inst->FS()) {
                /* FCC bits in FCSR and FCCR linked */
                reg_image.FCSR() = (reg_image.FCSR() & ~0xfe400000) |
                                   ((reg_image.FCCR() & 0xfe) << 24) |
                                   ((reg_image.FCCR() & 0x1) << 23);
                reg_image.FCCR() &= FCCR_MASK;
            } else if (FCSR_REG == inst->FS()) {
                /* FCC bits in FCSR and FCCR linked */
                reg_image.FCCR() =
                    ((reg_image.FCSR() >> 24) & 0xfe) | ((reg_image.FCSR() >> 23) & 0x1);
                reg_image.FCSR() &= FCSR_MASK;
                if ((reg_image.R[inst->RT()] & ~FCSR_MASK) != 0) {
                    /* Trying to set unsupported mode */
                    this->RAISE_EXCEPTION(ExcCode_FPE);
                }
            }
            break;
        }

        case Y_CEIL_W_D_OP: {
            double val = reg_image.FPR_D(inst->FS());
            reg_image.SET_FPR_W(inst->FD(), (int32)ceil(val));
            break;
        }

        case Y_CEIL_W_S_OP: {
            double val = (double)reg_image.FPR_S(inst->FS());
            reg_image.SET_FPR_W(inst->FD(), (int32)ceil(val));
            break;
        }

        case Y_CVT_D_S_OP: {
            double val = reg_image.FPR_S(inst->FS());
            reg_image.SET_FPR_D(inst->FD(), val);
            break;
        }

        case Y_CVT_D_W_OP: {
            double val = (double)reg_image.FPR_W(inst->FS());
            reg_image.SET_FPR_D(inst->FD(), val);
            break;
        }

        case Y_CVT_S_D_OP: {
            float val = (float)reg_image.FPR_D(inst->FS());

            reg_image.SET_FPR_S(inst->FD(), val);
            break;
        }

        case Y_CVT_S_W_OP: {
            float val = (float)reg_image.FPR_W(inst->FS());

            reg_image.SET_FPR_S(inst->FD(), val);
            break;
        }

        case Y_CVT_W_D_OP: {
            int val = (int32)reg_image.FPR_D(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), val);
            break;
        }

        case Y_CVT_W_S_OP: {
            int val = (int32)reg_image.FPR_S(inst->FS());
            reg_image.SET_FPR_W(inst->FD(), val);
            break;
        }

        case Y_DIV_S_OP: {
            reg_image.SET_FPR_S(inst->FD(),
                                reg_image.FPR_S(inst->FS()) / reg_image.FPR_S(inst->FT()));
            break;
        }

        case Y_DIV_D_OP: {
            reg_image.SET_FPR_D(inst->FD(),
                                reg_image.FPR_D(inst->FS()) / reg_image.FPR_D(inst->FT()));
            break;
        }

        case Y_FLOOR_W_D_OP: {
            double val = reg_image.FPR_D(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)floor(val));
            break;
        }

        case Y_FLOOR_W_S_OP: {
            double val = (double)reg_image.FPR_S(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)floor(val));
            break;
        }

        case Y_LDC1_OP: {
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            if ((addr & 0x3) != 0) {
                if (this->RAISE_EXCEPTION(ExcCode_AdEL)) {
                    reg_image.CP0_BadVAddr() = addr;
                }
            }

            LOAD_INST((reg_word *)&reg_image.FPR_S(inst->FT()), this->read_mem_word(addr),
                      0xffffffff);
            LOAD_INST((reg_word *)&reg_image.FPR_S(inst->FT() + 1),
                      this->read_mem_word(addr + sizeof(mem_word)), 0xffffffff);
            break;
        }

        case Y_LWC1_OP: {
            LOAD_INST((reg_word *)&reg_image.FPR_S(inst->FT()),
                      this->read_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET()), 0xffffffff);
            break;
        }

        case Y_MFC1_OP: {
            float val = reg_image.FPR_S(inst->FS());
            reg_word *vp = (reg_word *)&val;

            reg_image.R[inst->RT()] = *vp; /* Fool coercion */
            break;
        }

        case Y_MOV_S_OP: {
            reg_image.SET_FPR_S(inst->FD(), reg_image.FPR_S(inst->FS()));
            break;
        }

        case Y_MOV_D_OP: {
            reg_image.SET_FPR_D(inst->FD(), reg_image.FPR_D(inst->FS()));
            break;
        }

        case Y_MOVF_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) == 0) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RS()];
            }
            break;
        }

        case Y_MOVF_D_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) == 0) {
                reg_image.SET_FPR_D(inst->FD(), reg_image.FPR_D(inst->FS()));
            }
            break;
        }

        case Y_MOVF_S_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) == 0) {
                reg_image.SET_FPR_S(inst->FD(), reg_image.FPR_S(inst->FS()));
            }
            break;
        }

        case Y_MOVN_D_OP: {
            if (reg_image.R[inst->RT()] != 0) {
                reg_image.SET_FPR_D(inst->FD(), reg_image.FPR_D(inst->FS()));
            }
            break;
        }

        case Y_MOVN_S_OP: {
            if (reg_image.R[inst->RT()] != 0) {
                reg_image.SET_FPR_S(inst->FD(), reg_image.FPR_S(inst->FS()));
            }
            break;
        }

        case Y_MOVT_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) != 0) {
                reg_image.R[inst->RD()] = reg_image.R[inst->RS()];
            }
            break;
        }

        case Y_MOVT_D_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) != 0) {
                reg_image.SET_FPR_D(inst->FD(), reg_image.FPR_D(inst->FS()));
            }
            break;
        }

        case Y_MOVT_S_OP: {
            int cc = inst->CC();
            if ((reg_image.FCCR() & (1 << cc)) != 0) {
                reg_image.SET_FPR_S(inst->FD(), reg_image.FPR_S(inst->FS()));
            }
            break;
        }

        case Y_MOVZ_D_OP: {
            if (reg_image.R[inst->RT()] == 0) {
                reg_image.SET_FPR_D(inst->FD(), reg_image.FPR_D(inst->FS()));
            }
            break;
        }

        case Y_MOVZ_S_OP: {
            if (reg_image.R[inst->RT()] == 0) {
                reg_image.SET_FPR_S(inst->FD(), reg_image.FPR_S(inst->FS()));
            }
            break;
        }

        case Y_MTC1_OP: {
            reg_word word = reg_image.R[inst->RT()];
            float *wp = (float *)&word;

            reg_image.SET_FPR_S(inst->FS(), *wp); /* fool coercion */
            break;
        }

        case Y_MUL_S_OP: {
            reg_image.SET_FPR_S(inst->FD(),
                                reg_image.FPR_S(inst->FS()) * reg_image.FPR_S(inst->FT()));
            break;
        }

        case Y_MUL_D_OP: {
            reg_image.SET_FPR_D(inst->FD(),
                                reg_image.FPR_D(inst->FS()) * reg_image.FPR_D(inst->FT()));
            break;
        }

        case Y_NEG_S_OP: {
            reg_image.SET_FPR_S(inst->FD(), -reg_image.FPR_S(inst->FS()));
            break;
        }

        case Y_NEG_D_OP: {
            reg_image.SET_FPR_D(inst->FD(), -reg_image.FPR_D(inst->FS()));
            break;
        }

        case Y_ROUND_W_D_OP: {
            double val = reg_image.FPR_D(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)(val + 0.5)); /* Casting truncates */
            break;
        }

        case Y_ROUND_W_S_OP: {
            double val = (double)reg_image.FPR_S(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)(val + 0.5)); /* Casting truncates */
            break;
        }

        case Y_SDC1_OP: {
            double val = reg_image.FPR_D(inst->RT());
            reg_word *vp = (reg_word *)&val;
            mem_addr addr = reg_image.R[inst->BASE()] + inst->IOFFSET();
            if ((addr & 0x3) != 0) {
                bool exception_raised = RAISE_EXCEPTION(ExcCode_AdEL);
                if (exception_raised) {
                    reg_image.CP0_BadVAddr() = addr;
                }
            }

            this->set_mem_word(addr, *vp);
            this->set_mem_word(addr + sizeof(mem_word), *(vp + 1));
            break;
        }

        case Y_SQRT_D_OP: {
            reg_image.SET_FPR_D(inst->FD(), sqrt(reg_image.FPR_D(inst->FS())));
            break;
        }

        case Y_SQRT_S_OP: {
            reg_image.SET_FPR_S(inst->FD(), sqrt(reg_image.FPR_S(inst->FS())));
            break;
        }

        case Y_SUB_S_OP: {
            reg_image.SET_FPR_S(inst->FD(),
                                reg_image.FPR_S(inst->FS()) - reg_image.FPR_S(inst->FT()));
            break;
        }

        case Y_SUB_D_OP: {
            reg_image.SET_FPR_D(inst->FD(),
                                reg_image.FPR_D(inst->FS()) - reg_image.FPR_D(inst->FT()));
            break;
        }

        case Y_SWC1_OP: {
            float val = reg_image.FPR_S(inst->RT());
            reg_word *vp = (reg_word *)&val;

            this->set_mem_word(reg_image.R[inst->BASE()] + inst->IOFFSET(), *vp);
            break;
        }

        case Y_TRUNC_W_D_OP: {
            double val = reg_image.FPR_D(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)val); /* Casting truncates */
            break;
        }

        case Y_TRUNC_W_S_OP: {
            double val = (double)reg_image.FPR_S(inst->FS());

            reg_image.SET_FPR_W(inst->FD(), (int32)val); /* Casting truncates */
            break;
        }

        default: {
            fatal_error("Unknown instruction type: %d\n", inst->OPCODE());
            break;
        }
    }

    /* After instruction executes: */
    reg_image.PC += BYTES_PER_WORD;

    if (reg_image.exception_occurred) {
        if ((reg_image.CP0_Cause() >> 2) > LAST_REAL_EXCEPT) {
            reg_image.CP0_EPC() = reg_image.PC - BYTES_PER_WORD;
        }
        this->handle_exception();
    }

    return true;
}

/* Multiply two 32-bit numbers, V1 and V2, to produce a 64 bit result in
   the HI/LO registers.	 The algorithm is high-school math:

         A B
       x C D
       ------
       AD || BD
 AC || CB || 0

 where A and B are the high and low short words of V1, C and D are the short
 words of V2, AD is the product of A and D, and X || Y is (X << 16) + Y.
 Since the algorithm is programmed in C, we need to be careful not to
 overflow. */

static void unsigned_multiply(reg_image_t &reg_image, reg_word v1, reg_word v2) {
    u_reg_word a, b, c, d;
    u_reg_word bd, ad, cb, ac;
    u_reg_word mid, mid2, carry_mid = 0;

    a = (v1 >> 16) & 0xffff;
    b = v1 & 0xffff;
    c = (v2 >> 16) & 0xffff;
    d = v2 & 0xffff;

    bd = b * d;
    ad = a * d;
    cb = c * b;
    ac = a * c;

    mid = ad + cb;
    if (mid < ad || mid < cb) {
        /* Arithmetic overflow or carry-out */
        carry_mid = 1;
    }

    mid2 = mid + ((bd >> 16) & 0xffff);
    if (mid2 < mid || mid2 < ((bd >> 16) & 0xffff)) {
        /* Arithmetic overflow or carry-out */
        carry_mid += 1;
    }

    reg_image.LO = (bd & 0xffff) | ((mid2 & 0xffff) << 16);
    reg_image.HI = ac + (carry_mid << 16) + ((mid2 >> 16) & 0xffff);
}

static void signed_multiply(reg_image_t &reg_image, reg_word v1, reg_word v2) {
    int neg_sign = 0;

    if (v1 < 0) {
        v1 = -v1;
        neg_sign = 1;
    }
    if (v2 < 0) {
        v2 = -v2;
        neg_sign = !neg_sign;
    }

    unsigned_multiply(reg_image, v1, v2);
    if (neg_sign) {
        reg_image.LO = ~reg_image.LO;
        reg_image.HI = ~reg_image.HI;
        reg_image.LO += 1;
        if (reg_image.LO == 0) {
            reg_image.HI += 1;
        }
    }
}

static void set_fpu_cc(reg_image_t &reg_image, int cond, int cc, int less, int equal,
                       int unordered) {
    int result = 0;
    int fcsr_bit;

    if (cond & COND_LT) {
        result |= less;
    }
    if (cond & COND_EQ) {
        result |= equal;
    }
    if (cond & COND_UN) {
        result |= unordered;
    }

    reg_image.FCCR() = (reg_image.FCCR() & ~(1 << cc)) | (result << cc);
    if (0 == cc) {
        fcsr_bit = 23;
    } else {
        fcsr_bit = 24 + cc;
    }
    reg_image.FCSR() = (reg_image.FCSR() & ~(1 << fcsr_bit)) | (result << fcsr_bit);
}

void CPU::_raise_exception(int excode) {
    reg_image_t &reg_image = this->registers;

    if (ExcCode_Int != excode ||
        ((reg_image.CP0_Status() & CP0_Status_IE) /* Allow interrupt if IE and !EXL */
         && !(reg_image.CP0_Status() & CP0_Status_EXL))) {
        /* Ignore interrupt exception when interrupts disabled.  */
        reg_image.exception_occurred = true;
        this->last_exception_addr = reg_image.PC;
        if (this->running_in_delay_slot) {
            /* In delay slot */
            if ((reg_image.CP0_Status() & CP0_Status_EXL) == 0) {
                /* Branch's addr */
                reg_image.CP0_EPC() = ROUND_DOWN(reg_image.PC - BYTES_PER_WORD, BYTES_PER_WORD);
                /* Set BD bit to record that instruction is in delay slot */
                reg_image.CP0_Cause() |= CP0_Cause_BD;
            }
        } else {
            /* Not in delay slot */
            if ((reg_image.CP0_Status() & CP0_Status_EXL) == 0) {
                /* Faulting instruction's address */
                reg_image.CP0_EPC() = ROUND_DOWN(reg_image.PC, BYTES_PER_WORD);
            }
        }
        /* ToDo: set CE field of Cause register to coprocessor causing exception */

        /* Record cause of exception */
        reg_image.CP0_Cause() = (reg_image.CP0_Cause() & ~CP0_Cause_ExcCode) | (excode << 2);

        /* Turn on EXL bit to prevent subsequent interrupts from affecting EPC */
        reg_image.CP0_Status() |= CP0_Status_EXL;

#ifdef MIPS1
        reg_image.CP0_Status() =
            (reg_image.CP0_Status() & 0xffffffc0) | ((reg_image.CP0_Status() & 0xf) << 2);
#endif
    }
}

/**
 * Returns true if you should
 */
bool CPU::RAISE_EXCEPTION(int CAUSE) {
    if ((CAUSE != ExcCode_Int) &&
        this->should_fail_on_exception) {  // fail_on_exception used to be spimbot_tournament
        // This is a special case where we want to abort on failure rather than simply raising
        // it
        this->done = 1;
        return false;
    } else {
        // Raises the exception normally
        this->_raise_exception(CAUSE);
        return true;
    }
}

void CPU::RAISE_INTERRUPT(int32_t LEVEL) {
    /* Set IP (pending) bit for interrupt level. */
    this->registers.RAISE_INTERRUPT(LEVEL);
}

void CPU::CLEAR_INTERRUPT(int32_t LEVEL) {
    /* Clear IP (pending) bit for interrupt level. */
    this->registers.CLEAR_INTERRUPT(LEVEL);
}
