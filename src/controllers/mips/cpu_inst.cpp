#include "cpu.h"
#include "parser_yacc.h"

/*
 * INSTRUCTION SPECIFIC METHODS
 */

/* Increment the current text segement PC. */

inline void CPU::increment_text_pc(int delta) {
    if (this->registers.in_kernel) {
        this->registers.next_k_text_pc += delta;
        if (this->memory.k_text_top <= this->registers.next_k_text_pc) {
            run_error("Can't expand kernel text segment\n");
        }
    } else {
        this->registers.next_text_pc += delta;
        if (this->memory.text_top <= this->registers.next_text_pc)
            run_error("Can't expand text segment\n");
    }
}

/* If FLAG is true, next instruction goes to kernel text segment,
 * otherwise it goes to user segment.
 */

inline void CPU::user_kernel_text_segment(bool to_kernel) { this->registers.in_kernel = to_kernel; }

/* Store an INSTRUCTION in memory at the next location. */

inline void CPU::store_instruction(instruction *inst) {
    if (data_dir) {
        store_word(inst->inst_encode());
        free_inst(inst);
    } else if (text_dir) {
        this->registers.exception_occurred = false;
        this->set_mem_inst(this->registers.INST_PC(), inst);
        if (this->registers.exception_occurred) {
            error("Invalid address (0x%08x) for instruction\n", this->registers.INST_PC());
        } else {
            increment_text_pc(BYTES_PER_WORD);
        }
        if (inst != nullptr) {
            inst->SET_SOURCE(source_line());
            if (inst->ENCODING() == 0) {
                inst->SET_ENCODING(inst->inst_encode());
            }
        }
    }
}

/* Print the instruction stored at the memory ADDRESS. */

void CPU::print_inst(port message_out, mem_addr addr) {
    std::string inst_str = this->inst_to_string(addr);
    const char *c_str = inst_str.c_str();
    write_output(message_out, c_str);
}

std::string CPU::inst_to_string(mem_addr addr) {
    reg_image_t &reg_image = this->registers;
    reg_image.exception_occurred = false;

    instruction *inst = this->read_mem_inst(addr);

    if (reg_image.exception_occurred) {
        error("Can't print instruction not in text segment (0x%08x)\n", addr);
        return "";
    }

    std::stringstream ss;
    instruction::format_an_inst(ss, inst, addr);
    return ss.str();
}

/* Return an address expression of the form SYMBOL +/- IOFFSET (REGISTER).
Any of the three parts may be omitted. */

addr_expr *CPU::make_addr_expr(int offs, char *sym, int reg_no) {
    addr_expr *expr = (addr_expr *)xmalloc(sizeof(addr_expr));
    label *lab;

    if (reg_no == 0 && sym != nullptr && (lab = symbol_table.lookup_label(sym))->gp_flag) {
        expr->reg_no = REG_GP;
        expr->imm = imm_expr::make_imm_expr(offs + lab->addr - this->memory.gp_midpoint, nullptr, false);
    } else {
        expr->reg_no = (unsigned char)reg_no;
        expr->imm = imm_expr::make_imm_expr(offs, (sym ? str_copy(sym) : sym), false);
    }
    return (expr);
}

/* Return true if a breakpoint is set at ADDR. */

bool CPU::inst_is_breakpoint(mem_addr addr) const {
    return this->breakpoints.find(addr) != this->breakpoints.end();
}

/* Return a jump-type instruction with the given OPCODE and TARGET
   fields. NB, even the immediate value may not fit in the field, this
   routine will not produce more than one instruction. */

void CPU::j_type_inst(int opcode, imm_expr *target) {
    instruction *inst = new instruction();

    inst->SET_OPCODE(opcode);
    target->offset = 0; /* Not PC relative */
    target->pc_relative = false;
    inst->SET_EXPR(imm_expr::copy_imm_expr(target));
    if (target->symbol == nullptr || SYMBOL_IS_DEFINED(target->symbol)) {
        resolve_a_label(target->symbol, inst);
    } else {
        this->symbol_table.record_inst_uses_symbol(inst, target->symbol);
    }
    store_instruction(inst);
}

/* Return a register-type instruction with the given OPCODE, FD, FS, and FT
   fields. */

void CPU::r_co_type_inst(int opcode, int fd, int fs, int ft) {
    instruction *inst = instruction::make_r_type_inst(opcode, fs, 0, ft);
    inst->SET_FD(fd);
    store_instruction(inst);
}

/* Return a register-type instruction with the given OPCODE, RD, RS, and RT
   fields. */

void CPU::r_type_inst(int opcode, int rd, int rs, int rt) {
    store_instruction(instruction::make_r_type_inst(opcode, rd, rs, rt));
}

/* Return a register-shift instruction with the given OPCODE, RD, RT, and
   SHAMT fields.*/

void CPU::r_sh_type_inst(int opcode, int rd, int rt, int shamt) {
    instruction *inst = instruction::make_r_type_inst(opcode, rd, 0, rt);
    inst->SET_SHAMT(shamt & 0x1f);
    store_instruction(inst);
}

/* Return a floating-point compare instruction with the given OPCODE,
   FS, FT, and CC fields.*/

void CPU::r_cond_type_inst(int opcode, int fs, int ft, int cc) {
    instruction *inst = instruction::make_r_type_inst(opcode, fs, 0, ft);
    inst->SET_FD(cc << 2);
    switch (opcode) {
        case Y_C_EQ_D_OP:
        case Y_C_EQ_S_OP: {
            inst->SET_COND(COND_EQ);
            break;
        }

        case Y_C_LE_D_OP:
        case Y_C_LE_S_OP: {
            inst->SET_COND(COND_IN | COND_LT | COND_EQ);
            break;
        }

        case Y_C_LT_D_OP:
        case Y_C_LT_S_OP: {
            inst->SET_COND(COND_IN | COND_LT);
            break;
        }

        case Y_C_NGE_D_OP:
        case Y_C_NGE_S_OP: {
            inst->SET_COND(COND_IN | COND_LT | COND_UN);
            break;
        }

        case Y_C_NGLE_D_OP:
        case Y_C_NGLE_S_OP: {
            inst->SET_COND(COND_IN | COND_UN);
            break;
        }

        case Y_C_NGL_D_OP:
        case Y_C_NGL_S_OP: {
            inst->SET_COND(COND_IN | COND_EQ | COND_UN);
            break;
        }

        case Y_C_NGT_D_OP:
        case Y_C_NGT_S_OP: {
            inst->SET_COND(COND_IN | COND_LT | COND_EQ | COND_UN);
            break;
        }

        case Y_C_OLT_D_OP:
        case Y_C_OLT_S_OP: {
            inst->SET_COND(COND_LT);
            break;
        }

        case Y_C_OLE_D_OP:
        case Y_C_OLE_S_OP: {
            inst->SET_COND(COND_LT | COND_EQ);
            break;
        }

        case Y_C_SEQ_D_OP:
        case Y_C_SEQ_S_OP: {
            inst->SET_COND(COND_IN | COND_EQ);
            break;
        }

        case Y_C_SF_D_OP:
        case Y_C_SF_S_OP: {
            inst->SET_COND(COND_IN);
            break;
        }

        case Y_C_F_D_OP:
        case Y_C_F_S_OP: {
            inst->SET_COND(0);
            break;
        }

        case Y_C_UEQ_D_OP:
        case Y_C_UEQ_S_OP: {
            inst->SET_COND(COND_EQ | COND_UN);
            break;
        }

        case Y_C_ULT_D_OP:
        case Y_C_ULT_S_OP: {
            inst->SET_COND(COND_LT | COND_UN);
            break;
        }

        case Y_C_ULE_D_OP:
        case Y_C_ULE_S_OP: {
            inst->SET_COND(COND_LT | COND_EQ | COND_UN);
            break;
        }

        case Y_C_UN_D_OP:
        case Y_C_UN_S_OP: {
            inst->SET_COND(COND_UN);
            break;
        }
    }
    store_instruction(inst);
}

/* Produce an immediate instruction with the OPCODE, RT, RS, and IMM
   fields.  NB, because the immediate value may not fit in the field,
   this routine may produce more than one instruction.	On the bare
   machine, we resolve symbolic address, but they better produce values
   that fit into instruction's immediate field. */

void CPU::i_type_inst(int opcode, int rt, int rs, imm_expr *expr) {
    instruction *inst = new instruction();

    inst->SET_OPCODE(opcode);
    inst->SET_RS(rs);
    inst->SET_RT(rt);
    inst->SET_EXPR(imm_expr::copy_imm_expr(expr));

    if (expr->symbol == nullptr || SYMBOL_IS_DEFINED(expr->symbol)) {
        /* Evaluate the instruction's expression. */
        int32_t value = expr->eval_imm_expr();

        if (!config.bare_machine &&
            (((opcode == Y_ADDI_OP || opcode == Y_ADDIU_OP || opcode == Y_SLTI_OP ||
               opcode == Y_SLTIU_OP || opcode == Y_TEQI_OP || opcode == Y_TGEI_OP ||
               opcode == Y_TGEIU_OP || opcode == Y_TLTI_OP || opcode == Y_TLTIU_OP ||
               opcode == Y_TNEI_OP || (opcode_is_load_store(opcode) && expr->bits == 0))
                  // Sign-extended immediate values:
                  ? ((value & 0xffff8000) != 0 && (value & 0xffff8000) != 0xffff8000)
                  // Not sign-extended:
                  : (value & 0xffff0000) != 0))) {
            // Non-immediate value
            free_inst(inst);
            i_type_inst_full_word(opcode, rt, rs, expr, 1, value);
            return;
        } else {
            resolve_a_label(expr->symbol, inst);
        }
    } else if (config.bare_machine || expr->bits != 0) { /* Don't know expression's value, but only
                                                    needed upper/lower 16-bits anyways. */
        this->symbol_table.record_inst_uses_symbol(inst, expr->symbol);
    } else {
        /* Don't know the expressions's value and want all of its bits,
           so assume that it will not produce a small result and generate
           sequence for 32 bit value. */
        free_inst(inst);

        i_type_inst_full_word(opcode, rt, rs, expr, 0, 0);
        return;
    }

    store_instruction(inst);
}

void CPU::i_type_inst_free(int opcode, int rt, int rs, imm_expr *expr) {
    i_type_inst(opcode, rt, rs, expr);
    delete expr;
}

void CPU::free_inst(instruction *inst) {
    if (inst != break_inst) {
        /* Don't free the breakpoint insructions since we only have one. */
        delete inst;
        inst = nullptr;
    }
}

/* The immediate value for an instruction will (or may) not fit in 16 bits.
   Build the value from its piece with separate instructions. */

void CPU::i_type_inst_full_word(int opcode, int rt, int rs, imm_expr *expr, int value_known,
                                int32 value) {  // XXX: FIXME
    if (opcode_is_load_store(opcode)) {
        int32 offset;

        if (expr->symbol != nullptr && expr->symbol->gp_flag && rs == 0 &&
            (int32)IMM_MIN <= (offset = expr->symbol->addr + expr->offset) &&
            offset <= (int32)IMM_MAX) {
            i_type_inst_free(opcode, rt, REG_GP, imm_expr::make_imm_expr(offset, nullptr, false));
        } else if (value_known) {
            int low, high;

            high = (value >> 16) & 0xffff;
            low = value & 0xffff;

            if (!(high == 0 && !(low & 0x8000)) && !(high == 0xffff && (low & 0x8000))) {
                /* Some of high 16 bits are non-zero */
                if (low & 0x8000) {
                    /* Adjust high 16, since load sign-extends low 16*/
                    high += 1;
                }

                i_type_inst_free(Y_LUI_OP, 1, 0, imm_expr::const_imm_expr(high));
                if (rs != 0) /* Base register */
                {
                    r_type_inst(Y_ADDU_OP, 1, 1, rs);
                }
                i_type_inst_free(opcode, rt, 1,
                                 imm_expr::lower_bits_of_expr(imm_expr::const_imm_expr(low)));
            } else {
                /* Special case, sign-extension of low 16 bits sets high to 0xffff */
                i_type_inst_free(opcode, rt, rs, imm_expr::const_imm_expr(low));
            }
        } else {
            /* Use $at */
            /* Need to adjust if lower bits are negative */
            i_type_inst_free(Y_LUI_OP, 1, 0, imm_expr::upper_bits_of_expr(expr));
            if (rs != 0) /* Base register */
            {
                r_type_inst(Y_ADDU_OP, 1, 1, rs);
            }
            i_type_inst_free(opcode, rt, 1, imm_expr::lower_bits_of_expr(expr));
        }
    } else if (opcode_is_branch(opcode)) {
        /* This only allows branches +/- 32K, which is not correct! */
        i_type_inst_free(opcode, rt, rs, imm_expr::lower_bits_of_expr(expr));
    } else {
        /* Computation instruction */
        int offset;

        if (expr->symbol != nullptr && expr->symbol->gp_flag && rs == 0 &&
            (int32)IMM_MIN <= (offset = expr->symbol->addr + expr->offset) &&
            offset <= (int32)IMM_MAX) {
            i_type_inst_free((opcode == Y_LUI_OP ? Y_ADDIU_OP : opcode), rt, REG_GP,
                             imm_expr::make_imm_expr(offset, nullptr, false));
        } else {
            /* Use $at */
            if ((opcode == Y_ORI_OP || opcode == Y_ADDI_OP || opcode == Y_ADDIU_OP ||
                 opcode == Y_LUI_OP) &&
                rs == 0) {
                expr->produce_immediate(rt, value_known, value);
            } else {
                expr->produce_immediate(1, value_known, value);
                r_type_inst(imm_op_to_op(opcode), rt, rs, 1);
            }
        }
    }
}