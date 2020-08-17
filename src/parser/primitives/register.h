#pragma once
#ifndef SPIMBOT_PARSER_REGISTER_H
#define SPIMBOT_PARSER_REGISTER_H

#include <unordered_map>

#include "../parser_helpers.h"

namespace mips_parser {
/*
 * Register Number	Conventional Name	Usage
 * $0	$zero	Hard-wired to 0
 * $1	$at	Reserved for pseudo-instructions
 * $2 - $3	$v0, $v1	Return values from functions
 * $4 - $7	$a0 - $a3	Arguments to functions - not preserved by subprograms
 * $8 - $15	$t0 - $t7	Temporary data, not preserved by subprograms
 * $16 - $23	$s0 - $s7	Saved registers, preserved by subprograms
 * $24 - $25	$t8 - $t9	More temporary registers, not preserved by subprograms
 * $26 - $27	$k0 - $k1	Reserved for kernel. Do not use.
 * $28	$gp	Global Area Pointer (base of global data segment)
 * $29	$sp	Stack Pointer
 * $30	$fp	Frame Pointer
 * $31	$ra	Return Address
 * $f0 - $f3	-	Floating point return values
 * $f4 - $f10	-	Temporary registers, not preserved by subprograms
 * $f12 - $f14	-	First two arguments to subprograms, not preserved by subprograms
 * $f16 - $f18	-	More temporary registers, not preserved by subprograms
 * $f20 - $f31	-	Saved registers, preserved by subprograms
 */

/*
 * Mapping of registers to their names.
 *
 * Note that there is overlap between the register numbers of
 * general purpose registers and floating point registers
 */
const std::unordered_map<std::string, int> registers = {
    {"$0", 0},    {"$1", 1},    {"$2", 2},    {"$3", 3},    {"$4", 4},    {"$5", 5},    {"$6", 6},    {"$7", 7},
    {"$8", 8},    {"$9", 9},    {"$10", 10},  {"$11", 11},  {"$12", 12},  {"$13", 13},  {"$14", 14},  {"$15", 15},
    {"$16", 16},  {"$17", 17},  {"$18", 18},  {"$19", 19},  {"$20", 20},  {"$21", 21},  {"$22", 22},  {"$23", 23},
    {"$24", 24},  {"$25", 25},  {"$26", 26},  {"$27", 27},  {"$28", 28},  {"$29", 29},  {"$30", 30},  {"$31", 31},

    {"$zero", 0}, {"$at", 1},   {"$v0", 2},   {"$v1", 3},   {"$a0", 4},   {"$a1", 5},   {"$a2", 6},   {"$a3", 7},
    {"$t0", 8},   {"$t1", 9},   {"$t2", 10},  {"$t3", 11},  {"$t4", 12},  {"$t5", 13},  {"$t6", 14},  {"$t7", 15},
    {"$s0", 16},  {"$s1", 17},  {"$s2", 18},  {"$s3", 19},  {"$s4", 20},  {"$s5", 21},  {"$s6", 22},  {"$s7", 23},
    {"$t8", 24},  {"$t9", 25},  {"$k0", 26},  {"$k1", 27},  {"$gp", 28},  {"$sp", 29},  {"$fp", 30},  {"$ra", 31},

    {"$f0", 0},   {"$f1", 1},   {"$f2", 2},   {"$f3", 3},   {"$f4", 4},   {"$f5", 5},   {"$f6", 6},   {"$f7", 7},
    {"$f8", 8},   {"$f9", 9},   {"$f10", 10}, {"$f11", 11}, {"$f12", 12}, {"$f13", 13}, {"$f14", 14}, {"$f15", 15},
    {"$f16", 16}, {"$f17", 17}, {"$f18", 18}, {"$f19", 19}, {"$f20", 20}, {"$f21", 21}, {"$f22", 22}, {"$f23", 23},
    {"$f24", 24}, {"$f25", 25}, {"$f26", 26}, {"$f27", 27}, {"$f28", 28}, {"$f29", 29}, {"$f30", 30}, {"$f31", 31},
};

struct GeneralRegister : x3::symbols<int> {
    GeneralRegister() {
        add("$0", 0)("$1", 1)("$2", 2)("$3", 3)("$4", 4)("$5", 5)("$6", 6)("$7", 7)("$8", 8)("$9", 9)("$10", 10)(
            "$11", 11)("$12", 12)("$13", 13)("$14", 14)("$15", 15)("$16", 16)("$17", 17)("$18", 18)("$19", 19)(
            "$20", 20)("$21", 21)("$22", 22)("$23", 23)("$24", 24)("$25", 25)("$26", 26)("$27", 27)("$28", 28)(
            "$29", 29)("$30", 30)("$31", 31);

        add("$zero", 0)("$at", 1)("$v0", 2)("$v1", 3)("$a0", 4)("$a1", 5)("$a2", 6)("$a3", 7)("$t0", 8)("$t1", 9)(
            "$t2", 10)("$t3", 11)("$t4", 12)("$t5", 13)("$t6", 14)("$t7", 15)("$s0", 16)("$s1", 17)("$s2", 18)(
            "$s3", 19)("$s4", 20)("$s5", 21)("$s6", 22)("$s7", 23)("$t8", 24)("$t9", 25)("$k0", 26)("$k1", 27)(
            "$gp", 28)("$sp", 29)("$fp", 30)("$ra", 31);
    }
};

struct FloatRegister : x3::symbols<int> {
    FloatRegister() {
        add("$f0", 0)("$f1", 1)("$f2", 2)("$f3", 3)("$f4", 4)("$f5", 5)("$f6", 6)("$f7", 7)("$f8", 8)("$f9", 9)(
            "$f10", 10)("$f11", 11)("$f12", 12)("$f13", 13)("$f14", 14)("$f15", 15)("$f16", 16)("$f17", 17)("$f18", 18)(
            "$f19", 19)("$f20", 20)("$f21", 21)("$f22", 22)("$f23", 23)("$f24", 24)("$f25", 25)("$f26", 26)("$f27", 27)(
            "$f28", 28)("$f29", 29)("$f30", 30)("$f31", 31);
    }
};

const auto fp_register_ = FloatRegister{}; // TODO: Make keywords!
const auto register_ = GeneralRegister{}; // TODO: Make keywords!

}  // namespace mips_parser
#endif
