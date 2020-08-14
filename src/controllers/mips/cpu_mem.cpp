#include "cpu.h"
#include "inst.h"
#include "mem.h"

/* Access memory */

void *CPU::mem_reference(mem_addr addr) const { return this->memory.mem_reference(addr); }

instruction *CPU::read_mem_inst(mem_addr addr) {
    mem_image_t &mem_image = this->memory;

    if ((addr >= TEXT_BOT) && (addr < mem_image.text_top) && !(addr & 0x3)) {
        ++mem_image.text_prof[(addr - TEXT_BOT) >> 2];
        return mem_image.text_seg[(addr - TEXT_BOT) >> 2];
    } else if ((addr >= K_TEXT_BOT) && (addr < mem_image.k_text_top) && !(addr & 0x3)) {
        ++mem_image.k_text_prof[(addr - K_TEXT_BOT) >> 2];
        return mem_image.k_text_seg[(addr - K_TEXT_BOT) >> 2];
    } else {
        return this->bad_text_read(addr);
    }
}

reg_word CPU::read_mem_byte(mem_addr addr) {
    mem_image_t &mem_image = this->memory;

    if ((addr >= DATA_BOT) && (addr < mem_image.data_top)) {
        return mem_image.data_seg_b[addr - DATA_BOT];
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP)) {
        return mem_image.stack_seg_b[addr - mem_image.stack_bot];
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top)) {
        return mem_image.k_data_seg_b[addr - K_DATA_BOT];
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP)) {
        return mem_image.special_seg_b[addr - SPECIAL_BOT];
    } else {
        return this->bad_mem_read(addr, 0);
    }
}

reg_word CPU::read_mem_half(mem_addr addr) {
    mem_image_t &mem_image = this->memory;

    if ((addr >= DATA_BOT) && (addr < mem_image.data_top) && !(addr & 0x1)) {
        return mem_image.data_seg_h[(addr - DATA_BOT) >> 1];
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP) && !(addr & 0x1)) {
        return mem_image.stack_seg_h[(addr - mem_image.stack_bot) >> 1];
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top) && !(addr & 0x1)) {
        return mem_image.k_data_seg_h[(addr - K_DATA_BOT) >> 1];
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP) && !(addr & 0x1)) {
        return mem_image.special_seg_h[(addr - SPECIAL_BOT) >> 1];
    } else {
        return this->bad_mem_read(addr, 0x1);
    }
}

reg_word CPU::read_mem_word(mem_addr addr) {
    mem_image_t &mem_image = this->memory;

    if ((addr >= DATA_BOT) && (addr < mem_image.data_top) && !(addr & 0x3)) {
        return mem_image.data_seg[(addr - DATA_BOT) >> 2];
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP) && !(addr & 0x3)) {
        return mem_image.stack_seg[(addr - mem_image.stack_bot) >> 2];
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top) && !(addr & 0x3)) {
        return mem_image.k_data_seg[(addr - K_DATA_BOT) >> 2];
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP) && !(addr & 0x3)) {
        return mem_image.special_seg[(addr - SPECIAL_BOT) >> 2];
    } else {
        return this->bad_mem_read(addr, 0x3);
    }
}

void CPU::set_mem_inst(mem_addr addr, instruction *inst) {  // XXX
    mem_image_t &mem_image = this->memory;

    mem_image.text_modified = true;
    if ((addr >= TEXT_BOT) && (addr < mem_image.text_top) && !(addr & 0x3)) {
        mem_image.text_seg[(addr - TEXT_BOT) >> 2] = inst;
    } else if ((addr >= K_TEXT_BOT) && (addr < mem_image.k_text_top) && !(addr & 0x3)) {
        mem_image.k_text_seg[(addr - K_TEXT_BOT) >> 2] = inst;
    } else {
        this->bad_text_write(addr, inst);  // TODO: UPDATE after fixing bad_text_read
    }
}

void CPU::set_mem_byte(mem_addr addr, reg_word value) {  // XXX
    mem_image_t &mem_image = this->memory;

    mem_image.data_modified = true;
    if ((addr >= DATA_BOT) && (addr < mem_image.data_top)) {
        mem_image.data_seg_b[addr - DATA_BOT] = (BYTE_TYPE)value;
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP)) {
        mem_image.stack_seg_b[addr - mem_image.stack_bot] = (BYTE_TYPE)value;
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top)) {
        mem_image.k_data_seg_b[addr - K_DATA_BOT] = (BYTE_TYPE)value;
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP)) {
        mem_image.special_seg_b[addr - SPECIAL_BOT] = (BYTE_TYPE)value;
    } else {
        this->bad_mem_write(addr, value, 0);  // TODO: UPDATE after fixing bad_text_read
    }
}

void CPU::set_mem_half(mem_addr addr, reg_word value) {  // XXX
    mem_image_t &mem_image = this->memory;

    mem_image.data_modified = true;
    if ((addr >= DATA_BOT) && (addr < mem_image.data_top) && !(addr & 0x1)) {
        mem_image.data_seg_h[(addr - DATA_BOT) >> 1] = (short)value;
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP) && !(addr & 0x1)) {
        mem_image.stack_seg_h[(addr - mem_image.stack_bot) >> 1] = (short)value;
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top) && !(addr & 0x1)) {
        mem_image.k_data_seg_h[(addr - K_DATA_BOT) >> 1] = (short)value;
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP) && !(addr & 0x1)) {
        mem_image.special_seg_h[(addr - SPECIAL_BOT) >> 1] = (short)value;
    } else {
        this->bad_mem_write(addr, value, 0x1);  // TODO: UPDATE after fixing bad_text_read
    }
}

void CPU::set_mem_word(mem_addr addr, reg_word value) {  // XXX
    mem_image_t &mem_image = this->memory;

    mem_image.data_modified = true;
    if ((addr >= DATA_BOT) && (addr < mem_image.data_top) && !(addr & 0x3)) {
        mem_image.data_seg[(addr - DATA_BOT) >> 2] = (mem_word)value;
    } else if ((addr >= mem_image.stack_bot) && (addr < STACK_TOP) && !(addr & 0x3)) {
        mem_image.stack_seg[(addr - mem_image.stack_bot) >> 2] = (mem_word)value;
    } else if ((addr >= K_DATA_BOT) && (addr < mem_image.k_data_top) && !(addr & 0x3)) {
        mem_image.k_data_seg[(addr - K_DATA_BOT) >> 2] = (mem_word)value;
    } else if ((addr >= SPECIAL_BOT) && (addr < SPECIAL_TOP) && !(addr & 0x3)) {
        mem_image.special_seg[(addr - SPECIAL_BOT) >> 2] = (mem_word)value;
    } else {
        this->bad_mem_write(addr, value, 0x3);  // TODO: UPDATE after fixing bad_text_read
    }
}

/* Handle the infrequent and erroneous cases in memory accesses. */

instruction *CPU::bad_text_read(mem_addr addr) {
    bool exception_raised = this->RAISE_EXCEPTION(ExcCode_IBE);
    if (exception_raised) {
        this->registers.CP0_BadVAddr() = addr;
    }
    return (inst_decode(0));
}

void CPU::bad_text_write(mem_addr addr, instruction *inst) {
    if (this->RAISE_EXCEPTION(ExcCode_IBE)) {
        this->registers.CP0_BadVAddr() = addr;
    }
    this->set_mem_word(addr, inst->ENCODING());
}

mem_word CPU::bad_mem_read(mem_addr addr, int mask) {
    reg_image_t &reg_image = this->registers;
    mem_image_t &mem_image = this->memory;

    mem_word tmp;

    if ((addr & mask) != 0) {
        bool exception_raised = this->RAISE_EXCEPTION(ExcCode_AdEL);
        if (exception_raised) {
            reg_image.CP0_BadVAddr() = addr;
        }

    } else if (addr >= TEXT_BOT && addr < mem_image.text_top) {
        switch (mask) {
            case 0x0: {
                tmp = (mem_image.text_seg[(addr - TEXT_BOT) >> 2])->ENCODING();
#ifdef SPIM_BIGENDIAN
                tmp = (unsigned)tmp >> (8 * (3 - (addr & 0x3)));
#else
                tmp = (unsigned)tmp >> (8 * (addr & 0x3));
#endif
                return (0xff & tmp);
            }
            case 0x1: {
                tmp = (mem_image.text_seg[(addr - TEXT_BOT) >> 2])->ENCODING();
#ifdef SPIM_BIGENDIAN
                tmp = (unsigned)tmp >> (8 * (2 - (addr & 0x2)));
#else
                tmp = (unsigned)tmp >> (8 * (addr & 0x2));
#endif
                return (0xffff & tmp);
            }

            case 0x3: {
                instruction *inst = mem_image.text_seg[(addr - TEXT_BOT) >> 2];
                if (inst == nullptr) {
                    return 0;
                } else {
                    return (inst->ENCODING());
                }
            }

            default: {
                run_error("Bad mask (0x%x) in bad_mem_read\n", mask);
            }
        }
    } else if (addr > mem_image.data_top &&
               addr < mem_image.stack_bot
               /* If more than 16 MB below stack, probably is bad data ref */
               && addr > mem_image.stack_bot - 16 * K * K) {
        /* Grow stack segment */
        mem_image.expand_stack(mem_image.stack_bot - addr + 4);
        return 0;
    } else if (SPIMBOT_IO_BOT <= addr && addr <= SPIMBOT_IO_TOP) {
        return (read_spimbot_IO(context, addr));
    } else {
        /* Address out of range */
        bool exception_raised = this->RAISE_EXCEPTION(ExcCode_DBE);

        if (exception_raised) {
            reg_image.CP0_BadVAddr() = addr;
        }
    }
    return (0);
}

void CPU::bad_mem_write(mem_addr addr, mem_word value, int mask) {
    reg_image_t &reg_image = this->registers;
    mem_image_t &mem_image = this->memory;

    mem_word tmp;

    if ((addr & mask) != 0) {
        /* Unaligned address fault */
        bool exception_raised = this->RAISE_EXCEPTION(ExcCode_AdES);
        if (exception_raised) {
            reg_image.CP0_BadVAddr() = addr;
        }
    } else if (addr >= TEXT_BOT && addr < mem_image.text_top) {
        switch (mask) {
            case 0x0: {
                tmp = (mem_image.text_seg[(addr - TEXT_BOT) >> 2])->ENCODING();
#ifdef SPIM_BIGENDIAN
                tmp = ((tmp & ~(0xff << (8 * (3 - (addr & 0x3))))) |
                       (value & 0xff) << (8 * (3 - (addr & 0x3))));
#else
                tmp =
                    ((tmp & ~(0xff << (8 * (addr & 0x3)))) | (value & 0xff) << (8 * (addr & 0x3)));
#endif
                break;
            }
            case 0x1: {
                tmp = (mem_image.text_seg[(addr - TEXT_BOT) >> 2])->ENCODING();
#ifdef SPIM_BIGENDIAN
                tmp = ((tmp & ~(0xffff << (8 * (2 - (addr & 0x2))))) |
                       (value & 0xffff) << (8 * (2 - (addr & 0x2))));
#else
                tmp = ((tmp & ~(0xffff << (8 * (addr & 0x2)))) | (value & 0xffff)
                                                                     << (8 * (addr & 0x2)));
#endif
                break;
            }
            case 0x3: {
                tmp = value;
                break;
            }
            default: {
                tmp = 0;
                run_error("Bad mask (0x%x) in bad_mem_read\n", mask);
            }
        }

        if (mem_image.text_seg[(addr - TEXT_BOT) >> 2] != nullptr) {
            free_inst(mem_image.text_seg[(addr - TEXT_BOT) >> 2]);
        }
        mem_image.text_seg[(addr - TEXT_BOT) >> 2] = inst_decode(tmp);

        mem_image.text_modified = true;
    } else if (addr > mem_image.data_top &&
               addr < mem_image.stack_bot
               /* If more than 16 MB below stack, probably is bad data ref */
               && addr > mem_image.stack_bot - 16 * K * K) {
        /* Grow stack segment */
        mem_image.expand_stack(mem_image.stack_bot - addr + 4);
        if (addr >= mem_image.stack_bot) {
            if (mask == 0) {
                mem_image.stack_seg_b[addr - mem_image.stack_bot] = (char)value;
            } else if (mask == 1) {
                mem_image.stack_seg_h[(addr - mem_image.stack_bot) >> 1] = (short)value;
            } else {
                mem_image.stack_seg[(addr - mem_image.stack_bot) >> 2] = value;
            }
        } else {
            bool exception_raised = this->RAISE_EXCEPTION(ExcCode_DBE);
            if (exception_raised) {
                reg_image.CP0_BadVAddr() = addr;
            }
        }

        mem_image.data_modified = true;
    } else if (SPIMBOT_IO_BOT <= addr && addr <= SPIMBOT_IO_TOP) {
        write_spimbot_IO(context, addr, value);
    } else {
        /* Address out of range */
        bool exception_raised = this->RAISE_EXCEPTION(ExcCode_DBE);
        if (exception_raised) {
            reg_image.CP0_BadVAddr() = addr;
        }
    }
}

/* Memory-mapped IO routines. */

/* Check if input is available and output is possible.  If so, update the
   memory-mapped control registers and buffers. */

void CPU::check_memory_mapped_IO() {
    reg_image_t &reg_image = this->registers;
    mem_image_t &mem_image = this->memory;

    if (mem_image.recv_buffer_full_timer > 0) {
        /* Do not check for more input until this interval expires. */
        mem_image.recv_buffer_full_timer -= 1;
    } else if (console_input_available()) {
        /* Read new char into the buffer and raise an interrupt, if interrupts
         are enabled for device. */
        /* assert(recv_buffer_full_timer == 0); */
        mem_image.recv_buffer = get_console_char();
        mem_image.recv_control |= RECV_READY;
        mem_image.recv_buffer_full_timer = RECV_INTERVAL;
        if (mem_image.recv_control & RECV_INT_ENABLE) {
            this->RAISE_INTERRUPT(RECV_INT_LEVEL);
        }
    }

    if (mem_image.trans_buffer_full_timer > 0) {
        /* Do not allow output until this interval expires. */
        mem_image.trans_buffer_full_timer -= 1;
    } else if (!(mem_image.trans_control & TRANS_READY)) {
        /* Done writing: empty the buffer and raise an interrupt, if interrupts
         are enabled for device. */
        /* assert(trans_buffer_full_timer == 0); */
        mem_image.trans_control |= TRANS_READY;
        if (mem_image.trans_control & TRANS_INT_ENABLE) {
            this->RAISE_INTERRUPT(TRANS_INT_LEVEL);
        }
    }
}

/* Invoked on a write to the memory-mapped IO area. */
// SPIMBOT: we over-ride these functions with our own I/O routines

// static void
// write_memory_mapped_IO (mem_addr addr, mem_word value)
// {
//   reg_image_t &reg_image = reg_images[0];
//
//   switch (addr)
//     {
//     case TRANS_CTRL_ADDR:
//       /* Program can only set the interrupt enable, not ready, bit. */
//       if ((value & TRANS_INT_ENABLE) != 0)
// 	{
// 	  /* Enable interrupts: */
// 	  trans_control |= TRANS_INT_ENABLE;
// 	  if (trans_control & TRANS_READY)
// 	    {
// 	      /* Raise interrupt on enabling a ready transmitter */
// 	      RAISE_INTERRUPT (reg_image, TRANS_INT_LEVEL);
// 	    }
// 	}
//       else
// 	{
// 	  /* Disable interrupts: */
// 	  trans_control &= ~TRANS_INT_ENABLE;
// 	  CLEAR_INTERRUPT (reg_image, TRANS_INT_LEVEL); /* Clear IP bit in Cause */
// 	}
//       break;
//
//     case TRANS_BUFFER_ADDR:
//       /* Ignore write if device is not ready. */
//       if ((trans_control & TRANS_READY) != 0)
// 	{
// 	  /* Write char: */
// 	  trans_buffer = value & 0xff;
// 	  put_console_char ((char)trans_buffer);
// 	  /* Device is busy for a while: */
// 	  trans_control &= ~TRANS_READY;
// 	  trans_buffer_full_timer = TRANS_LATENCY;
//           CLEAR_INTERRUPT (reg_image, TRANS_INT_LEVEL); /* Clear IP bit in Cause */
// 	}
//       break;
//
//     case RECV_CTRL_ADDR:
//       /* Program can only set the interrupt enable, not ready, bit. */
//       if ((value & RECV_INT_ENABLE) != 0)
// 	{
// 	  /* Enable interrupts: */
// 	  recv_control |= RECV_INT_ENABLE;
// 	  if (recv_control & RECV_READY)
// 	    {
// 	      /* Raise interrupt on enabling a ready receiver */
// 	      RAISE_INTERRUPT (reg_image, RECV_INT_LEVEL);
// 	    }
// 	}
//       else
// 	{
// 	  /* Disable interrupts: */
// 	  recv_control &= ~RECV_INT_ENABLE;
// 	  CLEAR_INTERRUPT (reg_image, RECV_INT_LEVEL); /* Clear IP bit in Cause */
// 	}
//       break;
//
//     case RECV_BUFFER_ADDR:
//       /* Nop: program can't change buffer. */
//       break;
//
//     default:
//       run_error ("Write to unused memory-mapped IO address (0x%x)\n", addr);
//     }
// }
//
//
// /* Invoked on a read in the memory-mapped IO area. */
//
// static mem_word
// read_memory_mapped_IO (mem_addr addr)
// {
//   reg_image_t &reg_image = reg_images[0];
//
//   switch (addr)
//     {
//     case TRANS_CTRL_ADDR:
//       return (trans_control);
//
//     case TRANS_BUFFER_ADDR:
//       return (trans_buffer & 0xff);
//
//     case RECV_CTRL_ADDR:
//       return (recv_control);
//
//     case RECV_BUFFER_ADDR:
//       recv_control &= ~RECV_READY; /* Buffer now empty */
//       recv_buffer_full_timer = 0;
//       CLEAR_INTERRUPT (reg_image, RECV_INT_LEVEL); /* Clear IP bit in Cause */
//       return (recv_buffer & 0xff);
//
//     default:
//       run_error ("Read from unused memory-mapped IO address (0x%x)\n", addr);
//       return (0);
//     }
// }

/* Misc. routines */

void CPU::print_mem(mem_addr addr) {
    mem_image_t &mem_image = this->memory;

    mem_word value;

    if ((addr & 0x3) != 0) {
        addr &= ~0x3; /* Address must be word-aligned */
    }

    if (TEXT_BOT <= addr && addr < mem_image.text_top) {
        print_inst(addr);
    } else if (DATA_BOT <= addr && addr < mem_image.data_top) {
        value = this->read_mem_word(addr);
        write_output(config.message_out, "Data seg @ 0x%08x (%d) = 0x%08x (%d)\n", addr, addr,
                     value, value);
    } else if (mem_image.stack_bot <= addr && addr < STACK_TOP) {
        value = this->read_mem_word(addr);
        write_output(config.message_out, "Stack seg @ 0x%08x (%d) = 0x%08x (%d)\n", addr, addr,
                     value, value);
    } else if (K_TEXT_BOT <= addr && addr < mem_image.k_text_top) {
        print_inst(addr);
    } else if (K_DATA_BOT <= addr && addr < mem_image.k_data_top) {
        value = this->read_mem_word(addr);
        write_output(config.message_out, "Kernel Data seg @ 0x%08x (%d) = 0x%08x (%d)\n", addr,
                     addr, value, value);
    } else {
        error("Address 0x%08x (%d) to print_mem is out of bounds\n", addr, addr);
    }
}
