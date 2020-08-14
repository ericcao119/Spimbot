/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
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

#ifndef _WIN32
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "cpu.h"
#include "inst.h"
#include "mem.h"
#include "reg.h"
#include "spim.h"
#include "string-stream.h"
#include "sym-tbl.h"
#include "syscall.h"

// XXX: Fix when Mem is done

#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/

#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>

void myInvalidParameterHandler(const wchar_t *expression, const wchar_t *function,
                               const wchar_t *file, unsigned int line, uintptr_t pReserved) {
    if (function != NULL) {
        run_error("Bad parameter to system call: %s\n", function);
    } else {
        run_error("Bad parameter to system call\n");
    }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag) {
    static _invalid_parameter_handler oldHandler;
    static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

    if (flag == 0) {
        oldHandler = _set_invalid_parameter_handler(newHandler);
        _CrtSetReportMode(_CRT_ASSERT, 0);  // Disable the message box for assertions.
    } else {
        newHandler = _set_invalid_parameter_handler(oldHandler);
        _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
}
#endif

/* The address of the last exception. Different from EPC
 * if one exception occurs inside another or an interrupt. */
mem_addr last_exception_addr;

/* Decides which syscall to execute or simulate.  Returns zero upon
   exit syscall and non-zero to continue execution. */

int CPU::do_syscall() {
#ifdef _WIN32
    windowsParameterHandlingControl(0);
#endif

    if (!is_syscalls_enabled) {
        return 1;
    }

    reg_image_t &reg_image = this->registers;
    mem_image_t &mem_image = this->memory;

    /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */

    switch (Syscall(reg_image.R[REG_V0])) {
        case Syscall::PRINT_INT: {
            write_output(config.console_out, "%d", reg_image.R[REG_A0]);
            break;
        }
        case Syscall::PRINT_FLOAT: {
            float val = reg_image.FPR_S(REG_FA0);

            write_output(config.console_out, "%.8f", val);
            break;
        }

        case Syscall::PRINT_DOUBLE: {
            write_output(config.console_out, "%.18g", reg_image.FPR[REG_FA0 / 2]);
            break;
        }
        case Syscall::PRINT_STRING: {
            write_output(config.console_out, "%s", this->memory.mem_reference(reg_image.R[REG_A0]));
            break;
        }
        case Syscall::READ_INT: {
            static char str[256];

            read_input(str, 256);
            reg_image.R[REG_RES] = atol(str);
            break;
        }

        case Syscall::READ_FLOAT: {
            static char str[256];

            read_input(str, 256);
            reg_image.SET_FPR_S(REG_FRES, (float)atof(str));
            break;
        }

        case Syscall::READ_DOUBLE: {
            static char str[256];

            read_input(str, 256);
            reg_image.FPR[REG_FRES] = atof(str);
            break;
        }

        case Syscall::READ_STRING: {
            read_input((char *)this->memory.mem_reference(reg_image.R[REG_A0]),
                       reg_image.R[REG_A1]);
            mem_image.data_modified = true;
            break;
        }

        case Syscall::SBRK: {
            mem_addr x = mem_image.data_top;
            this->memory.expand_data(reg_image.R[REG_A0]);
            reg_image.R[REG_RES] = x;
            mem_image.data_modified = true;
            break;
        }

        case Syscall::PRINT_CHARACTER: {
            write_output(config.console_out, "%c", reg_image.R[REG_A0]);
            break;
        }
        case Syscall::READ_CHARACTER: {
            static char str[2];

            read_input(str, 2);
            if (*str == '\0') {
                *str = '\n'; /* makes xspim = spim */
            }
            reg_image.R[REG_RES] = (long)str[0];
            break;
        }
        case Syscall::EXIT: {
            spim_return_value = 0;
            return 0;
        }
        case Syscall::EXIT2: {
            spim_return_value = reg_image.R[REG_A0]; /* value passed to spim's exit() call */
            return 0;
        }
        case Syscall::OPEN: {
            if (this->is_file_io_enabled) {
#ifdef _WIN32
                reg_image.R[REG_RES] = _open((char *)this->memory.mem_reference(reg_image.R[REG_A0]),
                                             reg_image.R[REG_A1], reg_image.R[REG_A2]);
#else
                reg_image.R[REG_RES] =
                    open((char *)this->memory.mem_reference(reg_image.R[REG_A0]),
                         reg_image.R[REG_A1], reg_image.R[REG_A2]);
#endif
            } else if (debug) {
                printf("Bot: %zu Failed use syscall open since File IO is disabled.\n", this->id);
            }
            break;
        }

        case Syscall::READ: {
            if (this->is_file_io_enabled) {
                /* Test if address is valid */
                (void)this->memory.mem_reference(reg_image.R[REG_A1] + reg_image.R[REG_A2] - 1);
#ifdef _WIN32
                reg_image.R[REG_RES] =
                    _read(reg_image.R[REG_A0], this->memory.mem_reference(reg_image.R[REG_A1]),
                          reg_image.R[REG_A2]);
#else
                reg_image.R[REG_RES] =
                    read(reg_image.R[REG_A0], this->memory.mem_reference(reg_image.R[REG_A1]),
                         reg_image.R[REG_A2]);
#endif
                mem_image.data_modified = true;
            } else if (debug) {
                printf("Bot: %zu Failed use syscall read since File IO is disabled.\n", this->id);
            }
            break;
        }

        case Syscall::WRITE: {
            if (this->is_file_io_enabled) {
                /* Test if address is valid */
                (void)this->memory.mem_reference(reg_image.R[REG_A1] + reg_image.R[REG_A2] - 1);
#ifdef _WIN32
                reg_image.R[REG_RES] =
                    _write(reg_image.R[REG_A0], this->memory.mem_reference(reg_image.R[REG_A1]),
                           reg_image.R[REG_A2]);
#else
                reg_image.R[REG_RES] =
                    write(reg_image.R[REG_A0], this->memory.mem_reference(reg_image.R[REG_A1]),
                          reg_image.R[REG_A2]);
#endif
            } else if (debug) {
                printf("Bot: %zu Failed use syscall write since File IO is disabled.\n", this->id);
            }
            break;
        }

        case Syscall::CLOSE: {
            if (this->is_file_io_enabled) {
#ifdef _WIN32
                reg_image.R[REG_RES] = _close(reg_image.R[REG_A0]);
#else
                reg_image.R[REG_RES] = close(reg_image.R[REG_A0]);
#endif
            } else if (debug) {
                printf("Bot: %zu Failed use syscall close since File IO is disabled.\n", this->id);
            }
            break;
        }

        case Syscall::PRINT_HEX: {
            write_output(config.console_out, "%x", reg_image.R[REG_A0]);
            break;
        }
        default: {
            run_error("Unknown system call: %d\n", reg_image.R[REG_V0]);
            break;
        }
    }

#ifdef _WIN32
    windowsParameterHandlingControl(1);
#endif
    return 1;
}

void CPU::handle_exception() {
    reg_image_t &reg_image = this->registers;
    if (this->should_fail_on_exception && reg_image.CP0_ExCode() != ExcCode_Int) {
        this->done = true;
        return;
    }

    if (!quiet && reg_image.CP0_ExCode() != ExcCode_Int) {
        error("Exception occurred at PC=0x%08x\n", last_exception_addr);
    }

    reg_image.exception_occurred = false;
    reg_image.PC = EXCEPTION_ADDR;

    switch (reg_image.CP0_ExCode()) {
        case ExcCode_Int: {
            break;
        }
        case ExcCode_AdEL: {
            if (!this->quiet) {
                error("  Unaligned address in inst/data fetch: 0x%08x\n", reg_image.CP0_BadVAddr());
            }
            break;
        }

        case ExcCode_AdES: {
            if (!this->quiet) {
                error("  Unaligned address in store: 0x%08x\n", reg_image.CP0_BadVAddr());
            }
            break;
        }

        case ExcCode_IBE: {
            if (!this->quiet) {
                error("  Bad address in text read: 0x%08x\n", reg_image.CP0_BadVAddr());
            }
            break;
        }

        case ExcCode_DBE: {
            if (!this->quiet) {
                error("  Bad address in data/stack read: 0x%08x\n", reg_image.CP0_BadVAddr());
            }
            break;
        }

        case ExcCode_Sys: {
            if (!this->quiet) {
                error("  Error in syscall\n");
            }
            break;
        }

        case ExcCode_Bp: {
            reg_image.exception_occurred = false;
            return;
        }
        case ExcCode_RI: {
            if (!this->quiet) {
                error("  Reserved instruction execution\n");
            }
            break;
        }
        case ExcCode_CpU: {
            if (!this->quiet) {
                error("  Coprocessor unuable\n");
            }
            break;
        }
        case ExcCode_Ov: {
            if (!this->quiet) {
                error("  Arithmetic overflow\n");
            }
            break;
        }
        case ExcCode_Tr: {
            if (!this->quiet) {
                error("  Trap\n");
            }
            break;
        }
        case ExcCode_FPE: {
            if (!this->quiet) {
                error("  Floating point\n");
            }
            break;
        }
        default: {
            if (!this->quiet) {
                error("Unknown exception: %d\n", reg_image.CP0_ExCode());
            }
            break;
        }
    }
}
