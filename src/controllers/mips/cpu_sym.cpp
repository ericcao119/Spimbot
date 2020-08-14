#include "cpu.h"

/**
 * Resolve the newly-defined label in INSTRUCTION.
 */
void CPU::resolve_a_label(label *sym, instruction *inst) {
    resolve_a_label_sub(sym, inst, (data_dir ? current_data_pc() : current_text_pc()));
}

/**
 * Given a newly-defined LABEL, resolve the previously encountered
 * instructions and data locations that refer to the label.
 */
void CPU::resolve_label_uses(label *sym) {
    mem_image_t &mem_image = this->memory;

    for (auto &use : sym->uses) {
        resolve_a_label_sub(sym, use.inst, use.addr);
        if (use.inst != nullptr && use.addr >= DATA_BOT && use.addr < mem_image.stack_bot) {
            set_mem_word(use.addr, use.inst->inst_encode());  // XXX: Replace
            free_inst(use.inst);
        }
    }

    sym->uses.clear();
}

void CPU::resolve_a_label_sub(label *sym, instruction *inst, mem_addr pc) {
    if (inst == nullptr) {
        /* Memory data: */
        this->set_mem_word(pc, sym->addr);
    } else {
        /* Instruction: */
        if (inst->EXPR()->pc_relative) {
            /* Instruction may have moved */
            inst->EXPR()->offset = 0 - pc;
        }

        if (inst->EXPR()->symbol == nullptr || SYMBOL_IS_DEFINED(inst->EXPR()->symbol)) {
            int32 value;
            int32 field_mask;

            if (opcode_is_branch(inst->OPCODE())) {
                /* Drop low two bits since instructions are on word boundaries. */
                int val = SIGN_EX(eval_imm_expr(inst->EXPR())); /* 16->32 bits */
                val = (val >> 2) & 0xffff;                      /* right shift, 32->16 bits */

                if (config.delayed_branches) {
                    val -= 1;
                }

                value = val;
                field_mask = 0xffff;
            } else if (opcode_is_jump(inst->OPCODE())) {
                value = eval_imm_expr(inst->EXPR());
                if ((value & 0xf0000000) != (pc & 0xf0000000)) {
                    error(
                        "Target of jump differs in high-order 4 bits from instruction pc "
                        "0x%x\n",
                        pc);
                }
                /* Drop high four bits, since they come from the PC and the
                         low two bits since instructions are on word boundaries. */
                value = (value & 0x0fffffff) >> 2;
                field_mask = 0xffffffff; /* Already checked that value fits in instruction */
            } else if (opcode_is_load_store(inst->OPCODE())) {
                /* Label's location is an address */
                value = eval_imm_expr(inst->EXPR());
                field_mask = 0xffff;

                if (value & 0x8000) {
                    /* LW/SW sign extends offset. Compensate by adding 1 to high 16 bits. */
                    instruction *prev_inst;
                    instruction *prev_prev_inst;
                    prev_inst = this->read_mem_inst(pc - BYTES_PER_WORD);
                    prev_prev_inst = this->read_mem_inst(pc - 2 * BYTES_PER_WORD);

                    if (prev_inst != nullptr && prev_inst->OPCODE() == Y_LUI_OP &&
                        inst->EXPR()->symbol == prev_inst->EXPR()->symbol &&
                        prev_inst->IMM() == 0) {
                        /* Check that previous instruction was LUI and it has no immediate,
                         otherwise it will have compensated for sign-extension */
                        prev_inst->EXPR()->offset += 0x10000;
                    }
                    /* There is an ADDU instruction before the LUI if the
                     LW/SW instruction uses an index register: skip over the ADDU. */
                    else if (prev_prev_inst != nullptr && prev_prev_inst->OPCODE() == Y_LUI_OP &&
                             inst->EXPR()->symbol == prev_prev_inst->EXPR()->symbol &&
                             prev_prev_inst->IMM() == 0) {
                        prev_prev_inst->EXPR()->offset += 0x10000;
                    }
                }
            } else {
                /* Label's location is a value */
                value = eval_imm_expr(inst->EXPR());
                field_mask = 0xffff;
            }

            if ((value & ~field_mask) != (int32)0 && (value & ~field_mask) != (int32)0xffff0000) {
                error("Immediate value is too large for field: ");
                print_inst(config.message_out, pc);
            }
            if (opcode_is_jump(inst->OPCODE())) {
                inst->SET_TARGET(value); /* Don't mask so it is sign-extended */
            } else {
                inst->SET_IMM(value); /* Ditto */
            }
            inst->SET_ENCODING(inst->inst_encode());
        } else {
            error("Resolving undefined symbol: %s\n",
                  (inst->EXPR()->symbol == nullptr) ? "" : inst->EXPR()->symbol->name);
        }
    }
}