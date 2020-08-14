/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.

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
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#include "reg.h"

enum class Syscall : reg_word {
    // Print syscalls
    PRINT_INT = 1,
    PRINT_FLOAT = 2,
    PRINT_DOUBLE = 3,
    PRINT_STRING = 4,

    // Read Syscalls
    READ_INT = 5,
    READ_FLOAT = 6,
    READ_DOUBLE = 7,
    READ_STRING = 8,

    // Allocate memory
    SBRK = 9,

    // Exit return code 0
    EXIT = 10,

    // More Syscalls
    PRINT_CHARACTER = 11,
    READ_CHARACTER = 12,

    // File syscalls
    OPEN = 13,
    READ = 14,
    WRITE = 15,
    CLOSE = 16,

    // Exit with custom return code
    EXIT2 = 17,

    // Custom print syscalls
    PRINT_HEX = 34,
};

#endif