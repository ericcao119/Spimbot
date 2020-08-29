/* SPIM S20 MIPS simulator.
   Code to build assembly instructions and resolve symbolic labels.

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

#include "inst.h"

#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <memory>

// Helper function for reusing format strings
// Source: https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
    if (size <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    // XXX: Check for off by one error
    return std::string(buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
}

void instruction::format_an_inst(std::stringstream &ss, instruction *inst, mem_addr addr) {
    name_val_val *entry;
    int line_start =
        ss.tellp();  // XXX: Check if actual length of stream is needed, but I doubt it.

    if (inst_is_breakpoint(addr)) {
        delete_breakpoint(addr);
        ss << "*";
        ss << read_mem_inst(0, addr)->format_an_inst(addr);  // XXX:
        add_breakpoint(addr);
        return;
    }

    ss << string_format("[0x%08x]\t", addr);
    if (inst == nullptr) {
        ss << "<none>\n";
        return;
    }

    entry = map_int_to_name_val_val(name_tbl, inst->OPCODE());
    if (entry == nullptr) {
        ss << string_format("<unknown instruction %d>\n", inst->OPCODE());
        return;
    }

    ss << string_format("0x%08x  %s", (uint32)inst->ENCODING(), entry->name);
    switch (entry->value2) {
        case BC_TYPE_INST:
            ss << string_format("%d %d", inst->CC(), inst->IDISP());
            break;

        case B1_TYPE_INST:
            ss << string_format(" $%d %d", inst->RS(), inst->IDISP());
            break;

        case I1s_TYPE_INST:
            ss << string_format(" $%d, %d", inst->RS(), inst->IMM());
            break;

        case I1t_TYPE_INST:
            ss << string_format(" $%d, %d", inst->RT(), inst->IMM());
            break;

        case I2_TYPE_INST:
            ss << string_format(" $%d, $%d, %d", inst->RT(), inst->RS(), inst->IMM());
            break;

        case B2_TYPE_INST:
            ss << string_format(" $%d, $%d, %d", inst->RS(), inst->RT(), inst->IDISP());
            break;

        case I2a_TYPE_INST:
            ss << string_format(" $%d, %d($%d)", inst->RT(), inst->IMM(), inst->BASE());
            break;

        case R1s_TYPE_INST:
            ss << string_format(" $%d", inst->RS());
            break;

        case R1d_TYPE_INST:
            ss << string_format(" $%d", inst->RD());
            break;

        case R2td_TYPE_INST:
            ss << string_format(" $%d, $%d", inst->RT(), inst->RD());
            break;

        case R2st_TYPE_INST:
            ss << string_format(" $%d, $%d", inst->RS(), inst->RT());
            break;

        case R2ds_TYPE_INST:
            ss << string_format(" $%d, $%d", inst->RD(), inst->RS());
            break;

        case R2sh_TYPE_INST:
            if (inst->ENCODING() == 0) {
                ss.seekp(-3, std::ios_base::end); /* zap sll */
                ss << "nop";
            } else {
                ss << string_format(" $%d, $%d, %d", inst->RD(), inst->RT(), inst->SHAMT());
            }
            break;

        case R3_TYPE_INST:
            ss << string_format(" $%d, $%d, $%d", inst->RD(), inst->RS(), inst->RT());
            break;

        case R3sh_TYPE_INST:
            ss << string_format(" $%d, $%d, $%d", inst->RD(), inst->RT(), inst->RS());
            break;

        case FP_I2a_TYPE_INST:
            ss << string_format(" $f%d, %d($%d)", inst->FT(), inst->IMM(), inst->BASE());
            break;

        case FP_R2ds_TYPE_INST:
            ss << string_format(" $f%d, $f%d", inst->FD(), inst->FS());
            break;

        case FP_R2ts_TYPE_INST:
            ss << string_format(" $%d, $f%d", inst->RT(), inst->FS());
            break;

        case FP_CMP_TYPE_INST:
            if (inst->FD() == 0) {
                ss << string_format(" $f%d, $f%d", inst->FS(), inst->FT());
            } else {
                ss << string_format(" %d, $f%d, $f%d", inst->FD() >> 2, inst->FS(), inst->FT());
            }
            break;

        case FP_R3_TYPE_INST:
            ss << string_format(" $f%d, $f%d, $f%d", inst->FD(), inst->FS(), inst->FT());
            break;

        case MOVC_TYPE_INST:
            ss << string_format(" $%d, $%d, %d", inst->RD(), inst->RS(), inst->RT() >> 2);
            break;

        case FP_MOVC_TYPE_INST:
            ss << string_format(" $f%d, $f%d, %d", inst->FD(), inst->FS(), inst->CC());
            break;

        case J_TYPE_INST:
            ss << string_format(" 0x%08x", inst->TARGET() << 2);
            break;

        case NOARG_TYPE_INST:
            break;

        default:
            fatal_error("Unknown instruction type in print_inst\n");
    }

    if (inst->EXPR() != nullptr && inst->EXPR()->symbol != nullptr) {
        ss << " [";
        if (opcode_is_load_store(inst->OPCODE())) {
            imm_expr::format_imm_expr(ss, inst->EXPR(), inst->BASE());
        } else {
            imm_expr::format_imm_expr(ss, inst->EXPR(), -1);
        }
        ss << "]";
    }

    if (inst->SOURCE() != nullptr) {
        /* Comment is source line text of current line. */
        int end = ss.tellp();  // End of where we write

        int gap_length = 57 - (end - line_start);
        for (; 0 < gap_length; gap_length -= 1) {
            ss << " ";
        }

        ss << "; ";
        ss << string_format("%s", inst->SOURCE());
    }

    ss << "\n";
}

/* An immediate expression has the form: SYMBOL +/- IOFFSET, where either
   part may be omitted. */

/* Make and return a new immediate expression */

imm_expr *imm_expr::make_imm_expr(int offs, char *sym, bool is_pc_relative) {
    imm_expr *expr = new imm_expr();

    expr->offset = offs;
    expr->bits = 0;
    expr->pc_relative = is_pc_relative;
    if (sym != nullptr) {
        expr->symbol = lookup_label(sym);
    } else {
        expr->symbol = nullptr;
    }
    return (expr);
}

/* Print the EXPRESSION. */

void imm_expr::format_imm_expr(std::stringstream &ss, imm_expr *expr, int base_reg) {
    if (expr->symbol != nullptr) {
        ss << string_format("%s", expr->symbol->name);
    }

    if (expr->pc_relative) {
        ss << string_format("-0x%08x", (unsigned int)-expr->offset);
    } else if (expr->offset < -10) {
        ss << string_format("-%d (-0x%08x)", -expr->offset, (unsigned int)-expr->offset);
    } else if (expr->offset > 10) {
        ss << string_format("+%d (0x%08x)", expr->offset, (unsigned int)expr->offset);
    }

    if (base_reg != -1 && expr->symbol != nullptr && (expr->offset > 10 || expr->offset < -10)) {
        if (expr->offset == 0 && base_reg != 0) {
            ss << "+0";
        }

        if (expr->offset != 0 || base_reg != 0) {
            ss << string_format("($%d)", base_reg);
        }
    }
}

constexpr int32_t REGS(uint16_t R, uint32_t O) { return (R & 0x1f) << O; }

int32_t instruction::inst_encode(bool correct_branches) const {
    const instruction *inst = this;
    int32_t a_opcode = 0;
    name_val_val *entry;

    if (inst == nullptr) {
        return (0);
    }

    entry = map_int_to_name_val_val(i_opcode_tbl, sizeof(i_opcode_tbl) / sizeof(name_val_val),
                                    inst->OPCODE());
    if (entry == nullptr) {
        return 0;
    }

    a_opcode = entry->value2;
    entry =
        map_int_to_name_val_val(name_tbl, sizeof(name_tbl) / sizeof(name_val_val), inst->OPCODE());

    switch (entry->value2) {
        case BC_TYPE_INST:
            return (a_opcode | REGS(inst->CC() << 2, 16) |
                    ((inst->IOFFSET() - (int16_t)correct_branches) & 0xffff));

        case B1_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) |
                    ((inst->IOFFSET() - (int16_t)correct_branches) & 0xffff));

        case I1s_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | (inst->IMM() & 0xffff));

        case I1t_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16) |
                    (inst->IMM() & 0xffff));

        case I2_TYPE_INST:
        case B2_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16) |
                    ((inst->IOFFSET() - (int16_t)correct_branches) & 0xffff));

        case I2a_TYPE_INST:
            return (a_opcode | REGS(inst->BASE(), 21) | REGS(inst->RT(), 16) |
                    (inst->IOFFSET() & 0xffff));

        case R1s_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21));

        case R1d_TYPE_INST:
            return (a_opcode | REGS(inst->RD(), 11));

        case R2td_TYPE_INST:
            return (a_opcode | REGS(inst->RT(), 16) | REGS(inst->RD(), 11));

        case R2st_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16));

        case R2ds_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RD(), 11));

        case R2sh_TYPE_INST:
            return (a_opcode | REGS(inst->RT(), 16) | REGS(inst->RD(), 11) |
                    REGS(inst->SHAMT(), 6));

        case R3_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16) | REGS(inst->RD(), 11));

        case R3sh_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16) | REGS(inst->RD(), 11));

        case FP_I2a_TYPE_INST:
            return (a_opcode | REGS(inst->BASE(), 21) | REGS(inst->RT(), 16) |
                    (inst->IOFFSET() & 0xffff));

        case FP_R2ds_TYPE_INST:
            return (a_opcode | REGS(inst->FS(), 11) | REGS(inst->FD(), 6));

        case FP_R2ts_TYPE_INST:
            return (a_opcode | REGS(inst->RT(), 16) | REGS(inst->FS(), 11));

        case FP_CMP_TYPE_INST:
            return (a_opcode | REGS(inst->FT(), 16) | REGS(inst->FS(), 11) | REGS(inst->FD(), 6) |
                    inst->COND());

        case FP_R3_TYPE_INST:
            return (a_opcode | REGS(inst->FT(), 16) | REGS(inst->FS(), 11) | REGS(inst->FD(), 6));

        case MOVC_TYPE_INST:
            return (a_opcode | REGS(inst->RS(), 21) | REGS(inst->RT(), 16) | REGS(inst->RD(), 11));

        case FP_MOVC_TYPE_INST:
            return (a_opcode | REGS(inst->CC(), 18) | REGS(inst->FS(), 11) | REGS(inst->FD(), 6));

        case J_TYPE_INST:
            return (a_opcode | inst->TARGET());

        case NOARG_TYPE_INST:
            return (a_opcode);

        default:
            fatal_error("Unknown instruction type in inst_encoding\n");
            return (0); /* Not reached */
    }
}

/* Factory Methods */

/* Return a register-type instruction with the given OPCODE, RD, RS, and RT
   fields. */

instruction *instruction::make_r_type_inst(int opcode, int rd, int rs, int rt) {
    instruction *inst = new instruction();

    inst->SET_OPCODE(opcode);
    inst->SET_RS(rs);
    inst->SET_RT(rt);
    inst->SET_RD(rd);
    inst->SET_SHAMT(0);
    return inst;
}

instruction *instruction::inst_decode(int32_t val) {
    int32_t a_opcode = val & 0xfc000000;
    name_val_val *entry;
    int32 i_opcode;

    /* Field classes: (opcode is continued in other part of instruction): */
    if (a_opcode == 0 || a_opcode == 0x70000000) { /* SPECIAL or SPECIAL2 */
        a_opcode |= (val & 0x3f);
    } else if (a_opcode == 0x04000000) { /* REGIMM */
        a_opcode |= (val & 0x001f0000);
    } else if (a_opcode == 0x40000000) { /* COP0 */
        a_opcode |= (val & 0x03e00000) | (val & 0x1f);
    } else if (a_opcode == 0x44000000) { /* COP1 */

        a_opcode |= (val & 0x03e00000);
        if ((val & 0xff000000) == 0x45000000) {
            a_opcode |= (val & 0x00010000); /* BC1f/t */
        } else {
            a_opcode |= (val & 0x3f);
        }
    } else if (a_opcode == 0x48000000 /* COPz */
               || a_opcode == 0x4c000000) {
        a_opcode |= (val & 0x03e00000);
    }

    entry = map_int_to_name_val_val(a_opcode_tbl, sizeof(a_opcode_tbl) / sizeof(name_val_val),
                                    a_opcode);
    if (entry == nullptr) {
        return (mk_r_inst(val, 0, 0, 0, 0, 0)); /* Invalid inst */
    }

    i_opcode = entry->value2;

    switch (map_int_to_name_val_val(name_tbl, sizeof(name_tbl) / sizeof(name_val_val), i_opcode)
                ->value2) {
        case BC_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), val & 0xffff));

        case B1_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), 0, val & 0xffff));

        case I1s_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), 0, val & 0xffff));

        case I1t_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), val & 0xffff));

        case I2_TYPE_INST:
        case B2_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), val & 0xffff));

        case I2a_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), val & 0xffff));

        case R1s_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), 0, 0, 0));

        case R1d_TYPE_INST:
            return (mk_r_inst(val, i_opcode, 0, 0, BIN_RD(val), 0));

        case R2td_TYPE_INST:
            return (mk_r_inst(val, i_opcode, 0, BIN_RT(val), BIN_RD(val), 0));

        case R2st_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), 0, 0));

        case R2ds_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), 0, BIN_RD(val), 0));

        case R2sh_TYPE_INST:
            return (mk_r_inst(val, i_opcode, 0, BIN_RT(val), BIN_RD(val), BIN_SA(val)));

        case R3_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), BIN_RD(val), 0));

        case R3sh_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), BIN_RD(val), 0));

        case FP_I2a_TYPE_INST:
            return (mk_i_inst(val, i_opcode, BIN_BASE(val), BIN_FT(val), val & 0xffff));

        case FP_R2ds_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_FS(val), 0, BIN_FD(val), 0));

        case FP_R2ts_TYPE_INST:
            return (mk_r_inst(val, i_opcode, 0, BIN_RT(val), BIN_FS(val), 0));

        case FP_CMP_TYPE_INST: {
            instruction *inst = mk_r_inst(val, i_opcode, BIN_FS(val), BIN_FT(val), BIN_FD(val), 0);
            inst->SET_COND(val & 0xf);
            return (inst);
        }

        case FP_R3_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_FS(val), BIN_FT(val), BIN_FD(val), 0));

        case MOVC_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_RS(val), BIN_RT(val), BIN_RD(val), 0));

        case FP_MOVC_TYPE_INST:
            return (mk_r_inst(val, i_opcode, BIN_FS(val), BIN_RT(val), BIN_FD(val), 0));

        case J_TYPE_INST:
            return (mk_j_inst(val, i_opcode, val & 0x2ffffff));

        case NOARG_TYPE_INST:
            return (mk_r_inst(val, i_opcode, 0, 0, 0, 0));

        default:
            return (mk_r_inst(val, 0, 0, 0, 0, 0)); /* Invalid inst */
    }
}

instruction instruction::mk_r_inst(int32_t val, int opcode, int rs, int rt, int rd, int shamt) {
    instruction inst {};

    inst.SET_OPCODE(opcode);
    inst.SET_RS(rs);
    inst.SET_RT(rt);
    inst.SET_RD(rd);
    inst.SET_SHAMT(shamt);
    inst.SET_ENCODING(val);
    inst.SET_EXPR(nullptr);
    return inst;
}

instruction instruction::mk_i_inst(int32_t val, int opcode, int rs, int rt, int offset) {
    instruction inst {};

    inst.SET_OPCODE(opcode);
    inst.SET_RS(rs);
    inst.SET_RT(rt);
    inst.SET_IOFFSET(offset);
    inst.SET_ENCODING(val);
    inst.SET_EXPR(nullptr);
    return inst;
}

instruction instruction::mk_j_inst(int32_t val, int opcode, int target) {
    instruction inst {};

    inst.SET_OPCODE(opcode);
    inst.SET_TARGET(target);
    inst.SET_ENCODING(val);
    inst.SET_EXPR(nullptr);
    return inst;
}
