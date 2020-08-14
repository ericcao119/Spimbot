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

/* To the one refactoring this later on. This is admittedly really bad code, but
 * it is meant to preserve the original usage with macros and defines without
 * too much changes. Feel free to refactor in the future.
 *
 * This holds all information relevant to the registers in the MIPS32 CPU.
 *
 * WARNING:
 * - There is currently no bound checking and this does not use anything like C++20 concepts
 */

#pragma once
#ifndef REG_H
#define REG_H

#include <stddef.h>
#include <stdint.h>

#include <array>

#include "spim.h"

using reg_word = int32_t; /*@alt unsigned int @*/
using u_reg_word = uint32_t;

/* General purpose registers: */

constexpr size_t R_LENGTH = 32;

/* Argument passing registers */

constexpr size_t REG_V0 = 2;
constexpr size_t REG_A0 = 4;
constexpr size_t REG_A1 = 5;
constexpr size_t REG_A2 = 6;
constexpr size_t REG_A3 = 7;
constexpr size_t REG_FA0 = 12;
constexpr size_t REG_SP = 29;

/* Result registers */

constexpr size_t REG_RES = 2;
constexpr size_t REG_FRES = 0;

/* $gp registers */

constexpr size_t REG_GP = 28;

constexpr char *int_reg_names[32] = {
    "r0", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"};

/* Exeception handling registers (Coprocessor 0): */

/* BadVAddr register: */
constexpr size_t CP0_BadVAddr_Reg = 8;

/* Count register: */
constexpr size_t CP0_Count_Reg = 9;

/* Compare register: */
constexpr size_t CP0_Compare_Reg = 11;

/* Status register: */
constexpr size_t CP0_Status_Reg = 12;

/* Implemented fields: */
constexpr reg_word CP0_Status_CU = 0xf0000000;
constexpr reg_word CP0_Status_IM = 0x0000ff00;
constexpr reg_word CP0_Status_IM7 = 0x00008000; /* HW Int 5 */
constexpr reg_word CP0_Status_IM6 = 0x00004000; /* HW Int 4 */
constexpr reg_word CP0_Status_IM5 = 0x00002000; /* HW Int 3 */
constexpr reg_word CP0_Status_IM4 = 0x00001000; /* HW Int 2 */
constexpr reg_word CP0_Status_IM3 = 0x00000800; /* HW Int 1 */
constexpr reg_word CP0_Status_IM2 = 0x00000400; /* HW Int 0 */
constexpr reg_word CP0_Status_IM1 = 0x00000200; /* SW Int 1 */
constexpr reg_word CP0_Status_IM0 = 0x00000100; /* SW Int 0 */
constexpr reg_word CP0_Status_UM = 0x00000010;
constexpr reg_word CP0_Status_EXL = 0x00000002;
constexpr reg_word CP0_Status_IE = 0x00000001;
constexpr reg_word CP0_Status_Mask =
    (CP0_Status_CU | CP0_Status_UM | CP0_Status_IM | CP0_Status_EXL | CP0_Status_IE);

/* Cause register: */
constexpr size_t CP0_Cause_Reg = 13;

/* Implemented fields: */
constexpr reg_word CP0_Cause_BD = 0x80000000;
constexpr reg_word CP0_Cause_IP = 0x0000ff00;
constexpr reg_word CP0_Cause_IP7 = 0x00008000; /* HW Int 5 */
constexpr reg_word CP0_Cause_IP6 = 0x00004000; /* HW Int 4 */
constexpr reg_word CP0_Cause_IP5 = 0x00002000; /* HW Int 3 */
constexpr reg_word CP0_Cause_IP4 = 0x00001000; /* HW Int 2 */
constexpr reg_word CP0_Cause_IP3 = 0x00000800; /* HW Int 1 */
constexpr reg_word CP0_Cause_IP2 = 0x00000400; /* HW Int 0 */
constexpr reg_word CP0_Cause_IP1 = 0x00000200; /* SW Int 1 */
constexpr reg_word CP0_Cause_IP0 = 0x00000100; /* SW Int 0 */
constexpr reg_word CP0_Cause_ExcCode = 0x0000007c;
constexpr reg_word CP0_Cause_Mask =
    (CP0_Cause_BD | CP0_Cause_IP | CP0_Cause_IP7 | CP0_Cause_IP6 | CP0_Cause_IP5 | CP0_Cause_IP4 |
     CP0_Cause_IP3 | CP0_Cause_IP2 | CP0_Cause_ExcCode);

/* EPC register: */
constexpr size_t CP0_EPC_Reg = 14;

/* Config register: */
constexpr size_t CP0_Config_Reg = 16;

/* Implemented fields: */
constexpr reg_word CP0_Config_BE = 0x000080000;
constexpr reg_word CP0_Config_AT = 0x000060000;
constexpr reg_word CP0_Config_AR = 0x00001c000;
constexpr reg_word CP0_Config_MT = 0x000000380;
constexpr reg_word CP0_Config_Mask =
    (CP0_Config_BE | CP0_Config_AT | CP0_Config_AR | CP0_Config_MT);

/* Floating Point Coprocessor (1) registers.

   This is the MIPS32, Revision 1 FPU register set. It contains 32, 32-bit
   registers (either 32 single or 16 double precision), as in the R2010.
   The MIPS32, Revision 2 or MIPS64 register set has 32 of each type of
   register. */

constexpr size_t FGR_LENGTH = 32;
constexpr size_t FPR_LENGTH = 16;

/* Floating point control registers: */

constexpr size_t FIR_REG = 0;

/* Implemented fields: */
constexpr reg_word FIR_W = 0x0008000;
constexpr reg_word FIR_D = 0x0001000;
constexpr reg_word FIR_S = 0x0000800;
constexpr reg_word FIR_MASK = (FIR_W | FIR_D | FIR_S);

constexpr size_t FCCR_REG = 25;

/* Implemented fields: */
constexpr reg_word FCCR_FCC = 0x000000ff;
constexpr reg_word FCCR_MASK = FCCR_FCC;

constexpr size_t FEXR_REG = 26;

/* No implemented fields */

constexpr size_t FENR_REG = 28;

/* No implemented fields */

constexpr size_t FCSR_REG = 31;

/* Implemented fields: */
constexpr reg_word FCSR_FCC = 0xfe800000;
constexpr reg_word FCSR_MASK = FCSR_FCC;

/* Floating point Cause (not implemented): */
constexpr reg_word FCSR_Cause_E = 0x00020000;
constexpr reg_word FCSR_Cause_V = 0x00010000;
constexpr reg_word FCSR_Cause_Z = 0x00008000;
constexpr reg_word FCSR_Cause_O = 0x00004000;
constexpr reg_word FCSR_Cause_U = 0x00002000;
constexpr reg_word FCSR_Cause_I = 0x00001000;
/* Floating point Enables (not implemented): */
constexpr reg_word FCSR_Enable_V = 0x00000800;
constexpr reg_word FCSR_Enable_Z = 0x00000400;
constexpr reg_word FCSR_Enable_O = 0x00000200;
constexpr reg_word FCSR_Enable_U = 0x00000100;
constexpr reg_word FCSR_Enable_I = 0x00000080;
/* Floating point Flags (not implemented): */
constexpr reg_word FCSR_Flag_V = 0x00000040;
constexpr reg_word FCSR_Flag_Z = 0x00000020;
constexpr reg_word FCSR_Flag_O = 0x00000010;
constexpr reg_word FCSR_Flag_U = 0x00000008;
constexpr reg_word FCSR_Flag_I = 0x00000004;

struct reg_image_t {
    int RFE_cycle;

    /* General purpose registers: */
    // reg_word R[R_LENGTH];
    std::array<reg_word, R_LENGTH> R;
    reg_word HI, LO;
    mem_addr PC, nPC;

    /* Floating Point Coprocessor (1) registers: */
    std::array<double, FPR_LENGTH> FPR;  // FWR and FGR are arrays that share the same space as FPR
    float *FGR;                          /* is possible */
    int *FWR;                            /* is possible */

    /* Coprocessor registers: */
    // reg_word CCR[4][32], CPR[4][32];
    // CCR = Co-processor control register.
    // CPR = Co-processor general register.
    std::array<std::array<reg_word, 32>, 4> CCR, CPR;

    bool exception_occurred;

    bool in_kernel; /* => data goes to kdata, not data */

    mem_addr next_text_pc;
    mem_addr next_k_text_pc;
    mem_addr next_data_pc;      /* Location for next datum in user process */
    mem_addr next_k_data_pc;    /* Location for next datum in kernel */
    mem_addr next_gp_item_addr; /* Address of next item accessed off $gp */
    bool auto_alignment;

    reg_image_t() {
        this->FPR.fill(0.0);
        this->FGR = (float *)FPR.data();
        this->FWR = (int *)FPR.data();

        this->R.fill(0);
        this->R[REG_SP] = STACK_TOP - BYTES_PER_WORD - 4096; /* Initialize $sp */
        this->HI = 0;
        this->LO = 0;
        this->PC = TEXT_BOT;

        this->CP0_BadVAddr() = 0;
        this->CP0_Count() = 0;
        this->CP0_Compare() = 0;
        this->CP0_Status() = (CP0_Status_CU & 0x30000000) | CP0_Status_IM | CP0_Status_UM;
        this->CP0_Cause() = 0;
        this->CP0_EPC() = 0;
#ifdef SPIM_BIGENDIAN
        this->CP0_Config() = CP0_Config_BE;
#else
        this->CP0_Config() = 0;
#endif

        this->FIR() = FIR_W | FIR_D | FIR_S; /* Word, double, & single implemented */
        this->FCSR() = 0x0;
        this->FCCR() = 0x0;
        this->FEXR() = 0x0;
        this->FENR() = 0x0;

        this->RFE_cycle = 0;
        this->auto_alignment = true;
    }

    inline reg_word &CP0_BadVAddr() { return this->CPR[0][CP0_BadVAddr_Reg]; }

    inline reg_word &CP0_Count() { return this->CPR[0][CP0_Count_Reg]; }

    inline reg_word &CP0_Compare() { return this->CPR[0][CP0_Compare_Reg]; }

    inline reg_word &CP0_Status() { return this->CPR[0][CP0_Status_Reg]; }

    inline reg_word &CP0_Cause() { return this->CPR[0][CP0_Cause_Reg]; }

    inline reg_word &CP0_ExCode() { (this->CP0_Cause() & CP0_Cause_ExcCode) >> 2; }

    inline reg_word &CP0_EPC() { return this->CPR[0][CP0_EPC_Reg]; }

    inline reg_word &CP0_Config() { return this->CPR[0][CP0_Config_Reg]; }

    // inline float FPR_S(size_t regno) const { return this->FGR[regno]; }
    inline float &FPR_S(size_t regno) { return this->FGR[regno]; }

    inline double FPR_D(size_t regno) const {
        // The odd case is awful to work with. So much so that we abandon convention and use
        // accessors and setters
        return (regno & 0x1) ? (run_error("Odd FP double register number\n"), 0.0)
                             : this->FPR[regno / 2];
    }

    inline int FPR_W(size_t regno) const { return this->FWR[regno]; }

    inline void SET_FPR_S(size_t regno, float value) { this->FGR[regno] = (float)(value); }

    inline void SET_FPR_D(size_t regno, double value) {
        if (regno & 0x1) {
            run_error("Odd FP double register number\n");
        } else {
            this->FPR[(regno) / 2] = (double)(value);
        }
    }

    inline void SET_FPR_W(size_t regno, int value) { this->FWR[regno] = (uint32_t)(value); }

    inline reg_word (&FCR())[32] {
        return this->CPR[1];
    }  // Returns a reference to an array of 32 reg_words

    inline reg_word &FIR() { return this->FCR()[FIR_REG]; }

    inline reg_word &FCCR() { return this->FCR()[FCCR_REG]; }

    inline reg_word &FEXR() { return this->FCR()[FEXR_REG]; }

    inline reg_word &FENR() { return this->FCR()[FENR_REG]; }

    inline reg_word &FCSR() { return this->FCR()[FCSR_REG]; }

    inline reg_word INTERRUPTS_ON() { return this->CP0_Status() & CP0_Status_IE; }
    inline reg_word IN_INTERRUPT_HANDLER() { return this->CP0_Status() & CP0_Status_EXL; }

    // #define RAISE_EXCEPTION(REGIMAGE, CAUSE, MISC)                \
//     {                                                         \
//         if (((CAUSE) != ExcCode_Int) && spimbot_tournament) { \
//             robots[(REGIMAGE).context].done = 1;              \
//         } else {                                              \
//             raise_exception((REGIMAGE).context, (CAUSE));     \
//             MISC;                                             \
//         }                                                     \
//     }

    inline void RAISE_INTERRUPT(int32_t LEVEL) {
        /* Set IP (pending) bit for interrupt level. */
        CP0_Cause() |= (1 << ((LEVEL) + 8));
    }

    inline void CLEAR_INTERRUPT(int32_t LEVEL) {
        /* Clear IP (pending) bit for interrupt level. */
        CP0_Cause() &= ~(1 << ((LEVEL) + 8));
    }

    /* Method specific to instructions */

    inline mem_addr INST_PC() {
        return this->in_kernel ? this->next_k_text_pc : this->next_text_pc;
    }

    /* Set ADDRESS at which the next instruction is stored. */

    inline void text_begins_at_point(mem_addr addr) { this->next_text_pc = addr; }

    inline void k_text_begins_at_point(mem_addr addr) { this->next_k_text_pc = addr; }

    /* Set the location (in user or kernel text space) for the next instruction. */

    inline void set_text_pc(mem_addr addr) {
        if (this->in_kernel) {
            this->next_k_text_pc = addr;
        } else {
            this->next_text_pc = addr;
        }
    }

    /* Return address for next instruction, in appropriate text segment. */

    inline mem_addr current_text_pc() { return this->INST_PC(); }
};

#endif
