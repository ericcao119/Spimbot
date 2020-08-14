/* SPIM S20 MIPS simulator.
   Definitions for the SPIM S20.

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
#ifndef SPIM_H
#define SPIM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#ifndef NULL
#define NULL 0
#endif

/* This declaration must match the endianness of the machine SPIM is running on. You CANNOT
   set SPIM to simulate a different endianness than the machine that executes it. Almost
   every processor (notably the x86) is little endian today. If your machine is big endian,
   define -DSPIM_BIGENDIAN in the Makefile. */

#ifndef SPIM_BIGENDIAN
#define SPIM_LITTLENDIAN
#endif

/* Type declarations for portability.  They work for DEC's Alpha (64 bits)
   and 32 bit machines */

using int32 = int32_t;
using uint32 = uint32_t;
using intptr_union = intptr_t;

bool streq(const char *s1, const char *s2) { return !strcmp(s1, s2); }

/* Round V to next greatest B boundary */
#define ROUND_UP(V, B) (((int)V + (B - 1)) & ~(B - 1))
#define ROUND_DOWN(V, B) (((int)V) & ~(B - 1))

/* Sign-extend an int16 to an int32 */
#define SIGN_EX(X) (((X)&0x8000) ? ((X) | 0xffff0000) : (X))

#ifdef MIN /* Some systems define these in system includes */
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

template <typename T>
constexpr auto MIN(const T& a, const T& b) {
   return std::min(a, b);
}

template <typename T>
constexpr auto MAX(const T& a, const T& b) {
   return std::max(a, b);
}

/* Useful and pervasive declarations: */

#ifdef NEED_MEM_FUNCTIONS
#define memcpy(T, F, S) bcopy((void *)F, (void *)T, S)
#define memclr(B, S) bzero(B, S)
#define memcmp(S1, S2, N) bcmp(S1, S2, N)
#else
#include <memory.h>
#define memclr(B, S) memset((void *)B, 0, S)
#endif

#define QSORT_FUNC int (*)(const void *, const void *)

constexpr int32_t K = 1024;

/* Type of a memory address.  Must be a 32-bit quantity to match MIPS.  */

using mem_addr = uint32_t; /*@alt int @*/

constexpr int32_t BYTES_PER_WORD = 4; /* On the MIPS32 processor */

/* Sizes of memory segments. */

/* Initial size of text segment. */

#ifndef TEXT_SIZE
constexpr uint32_t TEXT_SIZE = (256 * K); /* 1/4 MB */
#endif

/* Initial size of k_text segment. */

#ifndef K_TEXT_SIZE
constexpr uint32_t K_TEXT_SIZE = (64 * K); /* 64 KB */
#endif

/* The data segment must be larger than 64K since we immediate grab
   64K for the small data segment pointed to by $gp. The data segment is
   expanded by an sbrk system call. */

/* Initial size of data segment. */

#ifndef DATA_SIZE
constexpr uint32_t DATA_SIZE = (256 * K); /* 1/4 MB */
#endif

/* Maximum size of data segment. */

#ifndef DATA_LIMIT
constexpr uint32_t DATA_LIMIT = (16 * K * K); /* 16 MB */
#endif

/* Initial size of k_data segment. */

#ifndef K_DATA_SIZE
constexpr uint32_t K_DATA_SIZE = (64 * K); /* 64 KB */
#endif

/* Maximum size of k_data segment. */

#ifndef K_DATA_LIMIT
constexpr uint32_t K_DATA_LIMIT = (K * K); /* 1 MB */
#endif

/* The stack grows down automatically. */

/* Initial size of stack segment. */

#ifndef STACK_SIZE
constexpr uint32_t STACK_SIZE = (64 * K); /* 64 KB */
#endif

/* Maximum size of stack segment. */

#ifndef STACK_LIMIT
constexpr uint32_t STACK_LIMIT = (256 * K); /* 1/4 MB */
#endif

/* Name of the function to invoke at start up */

constexpr char *DEFAULT_RUN_LOCATION = "__start";

/* Name of the symbol marking the end of the exception handler */

constexpr char *END_OF_TRAP_HANDLER_SYMBOL = "__eoth";

/* Default number of instructions to execute. */

constexpr int32_t DEFAULT_RUN_STEPS = 2147483647;

/* Address to branch to when exception occurs */
#ifdef MIPS1
/* MIPS R2000 */
constexpr int32_t EXCEPTION_ADDR = 0x80000080;
#else
/* MIPS32 */
constexpr int32_t EXCEPTION_ADDR = 0x80000180;
#endif

/* Maximum size of object stored in the small data segment pointed to by $gp */

constexpr int32_t SMALL_DATA_SEG_MAX_SIZE = 8;

// Unused
#ifndef DIRECT_MAPPED
#define DIRECT_MAPPED 0
#define TWO_WAY_SET 1
#endif

/* Interval (in instructions) at which memory-mapped IO registers are
   checked and updated. (This is to reduce overhead from making system calls
   to check for IO. It can be set as low as 1.) */

constexpr uint32_t IO_INTERVAL = 100;  // Unused

/* Number of IO_INTERVALs that a character remains in receiver buffer,
   even if another character is available. */

constexpr uint32_t RECV_INTERVAL = 100;  // Used for console input/output

/* Number of IO_INTERVALs that it takes to write a character. */

constexpr uint32_t TRANS_LATENCY = 100;  // Unused

/* Iterval (milliseconds) for the hardware timer in CP0. */

constexpr uint32_t TIMER_TICK_MS = 10; /* 100 times per second (Unused) */

/* A port is either a Unix file descriptor (an int) or a FILE* pointer. */

union port {
    int i;
    FILE *f;
};

/* Exported functions (from spim.c or xspim.c or spim_support.cpp): */

int console_input_available();
void error(char *fmt, ...);
void fatal_error(char *fmt, ...);
char get_console_char();
void put_console_char(char c);
void read_input(char *str, int n);
void run_error(char *fmt, ...);
void write_output(port, char *fmt, ...);

/* Exported variables: */

// TODO: Try to remove

// extern bool bare_machine;         /* => simulate bare machine */
// extern bool accept_pseudo_insts;  /* => parse pseudo instructions  */
// extern bool delayed_branches;     /* => simulate delayed branches */
// extern bool delayed_loads;        /* => simulate delayed loads */
// extern bool quiet;                /* => no warning messages */
// extern char *exception_file_name; /* File containing exception handler */
// extern bool force_break;          /* => stop interpreter loop  */
// extern bool parser_error_occurred; /* => parse resulted in error */
// extern int spim_return_value;     /* Value returned when spim exits */
/* Actual type of structure pointed to depends on X/terminal interface */
// extern port message_out, console_out, console_in;
// extern bool mapped_io;		/* => activate memory-mapped IO */
// extern int initial_text_size;
// extern int initial_data_size;
// extern mem_addr initial_data_limit;
// extern int initial_stack_size;
// extern mem_addr initial_stack_limit;
// extern int initial_k_text_size;
// extern int initial_k_data_size;
// extern mem_addr initial_k_data_limit;

/* SpimBOT stuff */
// extern bool map_click;

#endif

// XXX: Must recheck for errors