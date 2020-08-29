/* SPIM S20 MIPS simulator.
   Description of a SPIM S20 instruction.

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
#pragma once
#ifndef INST_H
#define INST_H

#include <cstdint>
#include <sstream>
#include <variant>

#include "../parser_helpers.h"

constexpr size_t NUM_OPERATIONS = 381;

constexpr uint8_t COND_UN = 0x1;
constexpr uint8_t COND_EQ = 0x2;
constexpr uint8_t COND_LT = 0x4;
constexpr uint8_t COND_IN = 0x8;

/* Minimum and maximum values that fit in instruction's imm field */
constexpr int32_t IMM_MIN = 0xffff8000;
constexpr int32_t IMM_MAX = 0x00007fff;

constexpr uint32_t UIMM_MIN = (unsigned)0;
constexpr uint32_t UIMM_MAX = ((unsigned)((1 << 16) - 1));

/* Represenation of the expression that produce a value for an instruction's
   immediate field.  Immediates have the form: label +/- offset. */

struct imm_expr {
    int offset;       /* Offset from symbol */
    label *symbol;    /* Symbolic label */
    short bits;       /* > 0 => 31..16, < 0 => 15..0 */
    bool pc_relative; /* => offset from label in code */

    /* Factory Methods */

    /* An immediate expression has the form: SYMBOL +/- IOFFSET, where either
   part may be omitted. */
    static imm_expr *make_imm_expr(int offs, char *sym, bool is_pc_relative);

    /* Return an instruction expression for a constant VALUE. */
    static imm_expr *const_imm_expr(int32_t value) { return (make_imm_expr(value, nullptr, false)); }

    /* Return a shallow copy of the EXPRESSION. */
    static imm_expr *copy_imm_expr(imm_expr *old_expr) {
        imm_expr *expr = new imm_expr();

        *expr = *old_expr;
        /*memcpy ((void*)expr, (void*)old_expr, sizeof (imm_expr));*/
        return (expr);
    }

    /* Return a shallow copy of the EXPRESSION with the offset field
   incremented by the given amount. */

    static imm_expr *incr_expr_offset(imm_expr *expr, int32 value) {
        imm_expr *new_expr = copy_imm_expr(expr);

        new_expr->offset += value;
        return (new_expr);
    }

    /* Return a shallow copy of an EXPRESSION that only uses the upper
   sixteen bits of the expression's value. */

    static imm_expr *upper_bits_of_expr(imm_expr *old_expr) {
        imm_expr *expr = copy_imm_expr(old_expr);

        expr->bits = 1;
        return (expr);
    }

    /* Return a shallow copy of the EXPRESSION that only uses the lower
       sixteen bits of the expression's value. */

    static imm_expr *lower_bits_of_expr(imm_expr *old_expr) {
        imm_expr *expr = copy_imm_expr(old_expr);

        expr->bits = -1;
        return (expr);
    }

    /* Return the value of the EXPRESSION. */

    inline int32_t eval_imm_expr() {
        imm_expr *expr = this;
        int32_t value;

        if (expr->symbol == nullptr) {
            value = expr->offset;
        } else if (SYMBOL_IS_DEFINED(expr->symbol)) {
            value = expr->offset + expr->symbol->addr;
        } else {
            error("Evaluated undefined symbol: %s\n", expr->symbol->name);
            value = 0;
        }
        if (expr->bits > 0) {
            return ((value >> 16) & 0xffff); /* Use upper bits of result */
        } else if (expr->bits < 0) {
            return (value & 0xffff); /* Use lower bits */
        } else {
            return (value);
        }
    }

    /* Return true if the EXPRESSION is a constant 0. */

    bool is_zero_imm() { return (this->offset == 0 && this->symbol == nullptr); }

    static void format_imm_expr(std::stringstream &ss, imm_expr *expr, int base_reg);

    void produce_immediate(int rt, int value_known,
                           int32 value) {  // XXX: FIXME
        if (value_known && (value & 0xffff) == 0) {
            i_type_inst_free(Y_LUI_OP, rt, 0, upper_bits_of_expr(this));
        } else if (value_known && (value & 0xffff0000) == 0) {
            i_type_inst_free(Y_ORI_OP, rt, 0, lower_bits_of_expr(this));
        } else {
            i_type_inst_free(Y_LUI_OP, 1, 0, upper_bits_of_expr(this));
            i_type_inst_free(Y_ORI_OP, rt, 1, lower_bits_of_expr(this));
        }
    }
};

/* Representation of the expression that produce an address for an
   instruction.  Address have the form: label +/- offset (register). */

struct addr_expr {
    unsigned char reg_no; /* Register number */
    imm_expr *imm;        /* The immediate part */

    imm_expr *addr_expr_imm() { return (this->imm); }
    inline int addr_expr_reg() { return (this->reg_no); }
};

namespace client {
namespace ast {

using namespace boost::spirit;

struct RInst : x3::position_tagged {
    int16_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
};

struct I_Inst : x3::position_tagged {
    int16_t opcode;
    uint8_t rs;
    uint8_t rt;
    std::variant <std::string, imm16_t>;
};
}  // namespace ast
}  // namespace client

struct instruction {
    int16_t opcode;

    union {
        /* R-type or I-type: */
        struct {
            uint8_t rs;
            uint8_t rt;

            union {
                int16_t imm;

                struct {
                    uint8_t rd;
                    uint8_t shamt;
                } r;
            } r_i;
        } r_i;

        /* J-type: */
        uint32_t target;
    } r_t;

    int32_t encoding;
    imm_expr *expr;
    char *source_line;  // no ownership

    // Rule of 5
    inline instruction() {
        opcode = 0;
        r_t.target = 0;
        encoding = 0;
        expr = nullptr;
        source_line = nullptr;
    }

    inline ~instruction() {
        if (this->EXPR() != nullptr) {
            delete this->EXPR();
        }
    }

    inline instruction(const instruction &other)
        : opcode(other.opcode), r_t(other.r_t), encoding(other.encoding), source_line(other.source_line) {
        if (other.expr == nullptr) {
            this->expr == nullptr;
        } else {
            this->expr = new imm_expr(*(other.expr));
        }
    }

    inline instruction(instruction &&other) noexcept
        : opcode(std::exchange(other.opcode, 0)),
          r_t(std::exchange(other.r_t, 0)),
          encoding(std::exchange(other.encoding, 0)),
          expr(std::exchange(other.expr, nullptr)),
          source_line(std::exchange(other.source_line, nullptr)) {}

    instruction &operator=(const instruction &other) {
        if (&other == this) {
            return *this;
        }
        return *this = instruction(other);
    }

    instruction &operator=(instruction &&other) noexcept {  // XXX: Make sure that this does not leak memory
        if (&other != this) {
            return *this;
        }
        std::swap(opcode, other.opcode);
        std::swap(r_t, other.r_t);
        std::swap(encoding, other.encoding);
        std::swap(expr, other.expr);
        std::swap(source_line, other.source_line);
        return *this;
    }

    /* Factory Methods */
    static instruction *make_r_type_inst(int opcode, int rd, int rs, int rt);
    static instruction *inst_decode(int32_t val);
    static instruction *mk_r_inst(int32 val, int opcode, int rs, int rt, int rd, int shamt);
    static instruction *mk_i_inst(int32 val, int opcode, int rs, int rt, int offset);
    static instruction *mk_j_inst(int32 val, int opcode, int target);

    /**
     * Compares equality of instructions. Source line does not influence this
     */
    inline bool operator==(const instruction &rhs) const {
        const instruction &lhs = *this;

        return (lhs.opcode == rhs.opcode) && (lhs.r_t == rhs.r_t) && (lhs.encoding == rhs.encoding) &&
               (lhs.expr == rhs.expr);
    }

    static void format_an_inst(std::stringstream &ss, instruction *inst, mem_addr addr);

    /* Make and return a deep copy of INST. */

    instruction *copy_inst(instruction *inst) {
        instruction *new_inst = new instruction();

        // Eric's Note: This originally did not actually make a deep copy unlike what
        // the documentation may lead you to believe. I have modified instruction so that it does.
        // The original C code is below
        *new_inst = *inst;
        /*memcpy ((void*)new_inst, (void*)inst , sizeof (instruction));*/
        new_inst->SET_EXPR(imm_expr::copy_imm_expr(inst->EXPR()));

        return (new_inst);
    }

    int32_t inst_encode(bool correct_branches = false) const;

    /* Convenience Accessors */

    int16_t OPCODE() const { return this->opcode; }
    void SET_OPCODE(int16_t value) { this->opcode = (short)(value); }

    uint8_t RS() const { return this->r_t.r_i.rs; }
    void SET_RS(uint8_t VALUE) { this->r_t.r_i.rs = (unsigned char)(VALUE); }

    uint8_t RT() const { return this->r_t.r_i.rt; }
    void SET_RT(uint8_t VAL) { this->r_t.r_i.rt = (unsigned char)(VAL); }

    uint8_t RD() const { return this->r_t.r_i.r_i.r.rd; }
    void SET_RD(uint8_t VAL) { this->r_t.r_i.r_i.r.rd = (unsigned char)(VAL); }

    uint8_t FS() const { return this->RD(); }
    void SET_FS(uint8_t VAL) { this->SET_RD(VAL); }

    uint8_t FT() const { return this->RT(); }
    void SET_FT(uint8_t VAL) { this->SET_RT(VAL); }

    uint8_t SHAMT() const { return this->r_t.r_i.r_i.r.shamt; }
    void SET_SHAMT(uint8_t VAL) { this->r_t.r_i.r_i.r.shamt = (unsigned char)(VAL); }

    uint8_t FD() const { return this->SHAMT(); }
    void SET_FD(uint8_t VAL) { this->SET_SHAMT(VAL); }

    int16_t IMM() const { return this->r_t.r_i.r_i.imm; }
    void SET_IMM(int16_t VAL) { this->r_t.r_i.r_i.imm = (short)(VAL); }

    uint8_t BASE() const { this->RS(); }
    void SET_BASE(uint8_t VAL) { this->SET_RS(VAL); }

    int16_t IOFFSET() const { return this->IMM(); }
    void SET_IOFFSET(int16_t VAL) { this->SET_IMM(VAL); }
    mem_addr IDISP() const { return SIGN_EX(this->IOFFSET() << 2); }

    uint8_t COND() const { return this->RS(); }
    void SET_COND(uint8_t VAL) { this->SET_RS(VAL); }

    uint8_t CC() const { return this->RT() >> 2; }
    uint8_t ND() const { return (this->RT() & 0x2) >> 1; }
    uint8_t TF() const { return this->RT() & 0x1; }

    mem_addr TARGET() const { return this->r_t.target; }
    void SET_TARGET(mem_addr VAL) { this->r_t.target = (mem_addr)(VAL); }

    int32_t ENCODING() const { return this->encoding; }
    void SET_ENCODING(int32_t VAL) { this->encoding = (int32)(VAL); }

    imm_expr *EXPR() { return this->expr; }
    void SET_EXPR(imm_expr *VAL) { this->expr = (imm_expr *)(VAL); }

    char *SOURCE() { return this->source_line; }
    void SET_SOURCE(char *VAL) { this->source_line = (char *)(VAL); }
};

/* Recognized exceptions: */

constexpr uint32_t ExcCode_Int = 0;       /* Interrupt */
constexpr uint32_t ExcCode_Mod = 1;       /* TLB modification (not implemented) */
constexpr uint32_t ExcCode_TLBL = 2;      /* TLB exception (not implemented) */
constexpr uint32_t ExcCode_TLBS = 3;      /* TLB exception (not implemented) */
constexpr uint32_t ExcCode_AdEL = 4;      /* Address error (load/fetch) */
constexpr uint32_t ExcCode_AdES = 5;      /* Address error (store) */
constexpr uint32_t ExcCode_IBE = 6;       /* Bus error, instruction fetch */
constexpr uint32_t ExcCode_DBE = 7;       /* Bus error, data reference */
constexpr uint32_t ExcCode_Sys = 8;       /* Syscall exception */
constexpr uint32_t ExcCode_Bp = 9;        /* Breakpoint exception */
constexpr uint32_t ExcCode_RI = 10;       /* Reserve instruction */
constexpr uint32_t ExcCode_CpU = 11;      /* Coprocessor unusable */
constexpr uint32_t ExcCode_Ov = 12;       /* Arithmetic overflow */
constexpr uint32_t ExcCode_Tr = 13;       /* Trap */
constexpr uint32_t ExcCode_FPE = 15;      /* Floating point */
constexpr uint32_t ExcCode_C2E = 18;      /* Coprocessor 2 (not impelemented) */
constexpr uint32_t ExcCode_MDMX = 22;     /* MDMX unusable (not implemented) */
constexpr uint32_t ExcCode_WATCH = 23;    /* Reference to Watch (not impelemented) */
constexpr uint32_t ExcCode_MCheck = 24;   /* Machine check (not implemented) */
constexpr uint32_t ExcCode_CacheErr = 30; /* Cache error (not impelemented) */

constexpr uint32_t LAST_REAL_EXCEPT = ExcCode_CacheErr;

/* Fields in binary representation of instructions: */

constexpr int32_t BIN_REG(int32_t value, int32_t offset) { return (value >> offset) & 0x1f; }
constexpr int32_t BIN_RS(int32_t value) { return BIN_REG(value, 21); }
constexpr int32_t BIN_RT(int32_t value) { return BIN_REG(value, 16); }
constexpr int32_t BIN_RD(int32_t value) { return BIN_REG(value, 11); }
constexpr int32_t BIN_SA(int32_t value) { return BIN_REG(value, 6); }
constexpr int32_t BIN_BASE(int32_t value) { return BIN_REG(value, 21); }
constexpr int32_t BIN_FT(int32_t value) { return BIN_REG(value, 16); }
constexpr int32_t BIN_FS(int32_t value) { return BIN_REG(value, 11); }
constexpr int32_t BIN_FD(int32_t value) { return BIN_REG(value, 6); }

/* Exported functions: */

/* Return true if SPIM OPCODE (e.g. Y_...) represents a conditional
   branch. */
constexpr bool opcode_is_branch(int opcode) {  // DONE
    switch (opcode) {
        case Y_BC1F_OP:
        case Y_BC1FL_OP:
        case Y_BC1T_OP:
        case Y_BC1TL_OP:
        case Y_BC2F_OP:
        case Y_BC2FL_OP:
        case Y_BC2T_OP:
        case Y_BC2TL_OP:
        case Y_BEQ_OP:
        case Y_BEQL_OP:
        case Y_BEQZ_POP:
        case Y_BGE_POP:
        case Y_BGEU_POP:
        case Y_BGEZ_OP:
        case Y_BGEZAL_OP:
        case Y_BGEZALL_OP:
        case Y_BGEZL_OP:
        case Y_BGT_POP:
        case Y_BGTU_POP:
        case Y_BGTZ_OP:
        case Y_BGTZL_OP:
        case Y_BLE_POP:
        case Y_BLEU_POP:
        case Y_BLEZ_OP:
        case Y_BLEZL_OP:
        case Y_BLT_POP:
        case Y_BLTU_POP:
        case Y_BLTZ_OP:
        case Y_BLTZAL_OP:
        case Y_BLTZALL_OP:
        case Y_BLTZL_OP:
        case Y_BNE_OP:
        case Y_BNEL_OP:
        case Y_BNEZ_POP:
            return true;

        default:
            return false;
    }
}

/* Return true if SPIM OPCODE represents a nullified (e.g., Y_...L_OP)
   conditional branch. */
constexpr bool opcode_is_nullified_branch(int opcode) {  // DONE
    switch (opcode) {
        case Y_BC1FL_OP:
        case Y_BC1TL_OP:
        case Y_BC2FL_OP:
        case Y_BC2TL_OP:
        case Y_BEQL_OP:
        case Y_BGEZALL_OP:
        case Y_BGEZL_OP:
        case Y_BGTZL_OP:
        case Y_BLEZL_OP:
        case Y_BLTZALL_OP:
        case Y_BLTZL_OP:
        case Y_BNEL_OP:
            return true;

        default:
            return false;
    }
}

/* Return true if SPIM OPCODE (e.g. Y_...) represents a conditional
   branch on a true condition. */
constexpr bool opcode_is_true_branch(int opcode) {  // DONE
    switch (opcode) {
        case Y_BC1T_OP:
        case Y_BC1TL_OP:
        case Y_BC2T_OP:
        case Y_BC2TL_OP:
            return true;

        default:
            return false;
    }
}

/* Return true if SPIM OPCODE (e.g. Y_...) is a direct unconditional
   branch (jump). */
constexpr bool opcode_is_jump(int opcode) {  // DONE
    switch (opcode) {
        case Y_J_OP:
        case Y_JAL_OP:
            return true;

        default:
            return false;
    }
}

/* Return true if SPIM OPCODE (e.g. Y_...) is a load or store. */
constexpr bool opcode_is_load_store(int opcode) {  // DONE
    switch (opcode) {
        case Y_LB_OP:
        case Y_LBU_OP:
        case Y_LH_OP:
        case Y_LHU_OP:
        case Y_LL_OP:
        case Y_LDC1_OP:
        case Y_LDC2_OP:
        case Y_LW_OP:
        case Y_LWC1_OP:
        case Y_LWC2_OP:
        case Y_LWL_OP:
        case Y_LWR_OP:
        case Y_SB_OP:
        case Y_SC_OP:
        case Y_SH_OP:
        case Y_SDC1_OP:
        case Y_SDC2_OP:
        case Y_SW_OP:
        case Y_SWC1_OP:
        case Y_SWC2_OP:
        case Y_SWL_OP:
        case Y_SWR_OP:
            return true;

        default:
            return false;
    }
}

void i_type_inst_free(int opcode, int rt, int rs, imm_expr *expr);
void initialize_inst_tables();

#endif
