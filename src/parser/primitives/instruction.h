#pragma once
#ifndef SPIMBOT_PARSER_INSTRUCTION_H
#define SPIMBOT_PARSER_INSTRUCTION_H

#include <unordered_map>

#include "../parser_helpers.h"

namespace mips_parser {

enum InstructionType {
    PSEUDO_OP = 1,

    BC_TYPE_INST = 10,
    B1_TYPE_INST = 11,
    I1s_TYPE_INST = 12,
    I1t_TYPE_INST = 13,
    I2_TYPE_INST = 14,
    B2_TYPE_INST = 15,
    I2a_TYPE_INST = 16,

    R1s_TYPE_INST = 20,
    R1d_TYPE_INST = 21,
    R2st_TYPE_INST = 22,
    R2ds_TYPE_INST = 23,
    R2td_TYPE_INST = 24,
    R2sh_TYPE_INST = 25,
    R3_TYPE_INST = 26,
    R3sh_TYPE_INST = 27,

    FP_I2a_TYPE_INST = 30,
    FP_R2ds_TYPE_INST = 31,
    FP_R2ts_TYPE_INST = 32,
    FP_CMP_TYPE_INST = 33,
    FP_R3_TYPE_INST = 34,
    FP_R4_TYPE_INST = 35,
    FP_MOVC_TYPE_INST = 36,
    MOVC_TYPE_INST = 37,

    J_TYPE_INST = 40,
    NOARG_TYPE_INST = 42,
};

/* Information on each keyword token that can be read by spim.	Must be
   sorted in alphabetical order. */

struct Operation {
    std::string name;
    InstructionType type;
    uint32_t opcode;
};

const std::unordered_map<std::string, Operation> opcodes = {
    {"abs", {"abs", PSEUDO_OP, (unsigned) -1}},
    {"abs.d", {"abs.d", FP_R2ds_TYPE_INST, 0x46200005}},
    {"abs.ps", {"abs.ps", FP_R2ds_TYPE_INST, 0x46600005}}, /* MIPS32 Rev 2 */
    {"abs.s", {"abs.s", FP_R2ds_TYPE_INST, 0x46000005}},

    {"add", {"add", R3_TYPE_INST, 0x00000020}},
    {"add.d", {"add.d", FP_R3_TYPE_INST, 0x46200000}},
    {"add.ps", {"add.ps", FP_R3_TYPE_INST, 0x46600000}}, /* MIPS32 Rev 2 */
    {"add.s", {"add.s", FP_R3_TYPE_INST, 0x46000000}},
    {"addi", {"addi", I2_TYPE_INST, 0x20000000}},
    {"addiu", {"addiu", I2_TYPE_INST, 0x24000000}},
    {"addu", {"addu", R3_TYPE_INST, 0x00000021}},

    {"alnv.ps", {"alnv.ps", FP_R4_TYPE_INST, 0x4c00001e}}, /* MIPS32 Rev 2 */

    {"and", {"and", R3_TYPE_INST, 0x00000024}},
    {"andi", {"andi", I2_TYPE_INST, 0x30000000}},

    {"b", {"b", PSEUDO_OP, (unsigned) -1}},
    {"bal", {"bal", PSEUDO_OP, (unsigned) -1}},

    {"bc1f", {"bc1f", BC_TYPE_INST, 0x45000000}},
    {"bc1fl", {"bc1fl", BC_TYPE_INST, 0x45020000}}, /* MIPS32 */
    {"bc1t", {"bc1t", BC_TYPE_INST, 0x45010000}},
    {"bc1tl", {"bc1tl", BC_TYPE_INST, 0x45030000}}, /* MIPS32 */

    {"bc2f", {"bc2f", BC_TYPE_INST, 0x49000000}},
    {"bc2fl", {"bc2fl", BC_TYPE_INST, 0x49020000}}, /* MIPS32 */
    {"bc2t", {"bc2t", BC_TYPE_INST, 0x49010000}},
    {"bc2tl", {"bc2tl", BC_TYPE_INST, 0x49030000}}, /* MIPS32 */

    {"beq", {"beq", B2_TYPE_INST, 0x10000000}},
    {"beql", {"beql", B2_TYPE_INST, 0x50000000}}, /* MIPS32 */
    {"beqz", {"beqz", PSEUDO_OP, (unsigned) -1}},
    {"bge", {"bge", PSEUDO_OP, (unsigned) -1}},
    {"bgeu", {"bgeu", PSEUDO_OP, (unsigned) -1}},
    {"bgez", {"bgez", B1_TYPE_INST, 0x04010000}},
    {"bgezal", {"bgezal", B1_TYPE_INST, 0x04110000}},
    {"bgezall", {"bgezall", B1_TYPE_INST, 0x04130000}}, /* MIPS32 */
    {"bgezl", {"bgezl", B1_TYPE_INST, 0x04030000}},     /* MIPS32 */
    {"bgt", {"bgt", PSEUDO_OP, (unsigned) -1}},
    {"bgtu", {"bgtu", PSEUDO_OP, (unsigned) -1}},
    {"bgtz", {"bgtz", B1_TYPE_INST, 0x1c000000}},
    {"bgtzl", {"bgtzl", B1_TYPE_INST, 0x5c000000}}, /* MIPS32 */
    {"ble", {"ble", PSEUDO_OP, (unsigned) -1}},
    {"bleu", {"bleu", PSEUDO_OP, (unsigned) -1}},
    {"blez", {"blez", B1_TYPE_INST, 0x18000000}},
    {"blezl", {"blezl", B1_TYPE_INST, 0x58000000}}, /* MIPS32 */
    {"blt", {"blt", PSEUDO_OP, (unsigned) -1}},
    {"bltu", {"bltu", PSEUDO_OP, (unsigned) -1}},
    {"bltz", {"bltz", B1_TYPE_INST, 0x04000000}},
    {"bltzal", {"bltzal", B1_TYPE_INST, 0x04100000}},
    {"bltzall", {"bltzall", B1_TYPE_INST, 0x04120000}}, /* MIPS32 */
    {"bltzl", {"bltzl", B1_TYPE_INST, 0x04020000}},     /* MIPS32 */
    {"bne", {"bne", B2_TYPE_INST, 0x14000000}},
    {"bnel", {"bnel", B2_TYPE_INST, 0x54000000}}, /* MIPS32 */
    {"bnez", {"bnez", PSEUDO_OP, (unsigned) -1}},

    {"break", {"break", NOARG_TYPE_INST, 0x0000000d}},

    {"c.eq.d", {"c.eq.d", FP_CMP_TYPE_INST, 0x46200032}},
    {"c.eq.ps", {"c.eq.ps", FP_CMP_TYPE_INST, 0x46600032}}, /* MIPS32 Rev 2 */
    {"c.eq.s", {"c.eq.s", FP_CMP_TYPE_INST, 0x46000032}},
    {"c.f.d", {"c.f.d", FP_CMP_TYPE_INST, 0x46200030}},
    {"c.f.ps", {"c.f.ps", FP_CMP_TYPE_INST, 0x46600030}}, /* MIPS32 Rev 2 */
    {"c.f.s", {"c.f.s", FP_CMP_TYPE_INST, 0x46000030}},
    {"c.le.d", {"c.le.d", FP_CMP_TYPE_INST, 0x4620003e}},
    {"c.le.ps", {"c.le.ps", FP_CMP_TYPE_INST, 0x4660003e}}, /* MIPS32 Rev 2 */
    {"c.le.s", {"c.le.s", FP_CMP_TYPE_INST, 0x4600003e}},
    {"c.lt.d", {"c.lt.d", FP_CMP_TYPE_INST, 0x4620003c}},
    {"c.lt.ps", {"c.lt.ps", FP_CMP_TYPE_INST, 0x4660003c}}, /* MIPS32 Rev 2 */
    {"c.lt.s", {"c.lt.s", FP_CMP_TYPE_INST, 0x4600003c}},
    {"c.nge.d", {"c.nge.d", FP_CMP_TYPE_INST, 0x4620003d}},
    {"c.nge.ps", {"c.nge.ps", FP_CMP_TYPE_INST, 0x4660003d}}, /* MIPS32 Rev 2 */
    {"c.nge.s", {"c.nge.s", FP_CMP_TYPE_INST, 0x4600003d}},
    {"c.ngl.d", {"c.ngl.d", FP_CMP_TYPE_INST, 0x4620003b}},
    {"c.ngl.ps", {"c.ngl.ps", FP_CMP_TYPE_INST, 0x4660003b}}, /* MIPS32 Rev 2 */
    {"c.ngl.s", {"c.ngl.s", FP_CMP_TYPE_INST, 0x4600003b}},
    {"c.ngle.d", {"c.ngle.d", FP_CMP_TYPE_INST, 0x46200039}},
    {"c.ngle.ps", {"c.ngle.ps", FP_CMP_TYPE_INST, 0x46600039}}, /* MIPS32 Rev 2 */
    {"c.ngle.s", {"c.ngle.s", FP_CMP_TYPE_INST, 0x46000039}},
    {"c.ngt.d", {"c.ngt.d", FP_CMP_TYPE_INST, 0x4620003f}},
    {"c.ngt.ps", {"c.ngt.ps", FP_CMP_TYPE_INST, 0x4660003f}}, /* MIPS32 Rev 2 */
    {"c.ngt.s", {"c.ngt.s", FP_CMP_TYPE_INST, 0x4600003f}},
    {"c.ole.d", {"c.ole.d", FP_CMP_TYPE_INST, 0x46200036}},
    {"c.ole.ps", {"c.ole.ps", FP_CMP_TYPE_INST, 0x46600036}}, /* MIPS32 Rev 2 */
    {"c.ole.s", {"c.ole.s", FP_CMP_TYPE_INST, 0x46000036}},
    {"c.olt.d", {"c.olt.d", FP_CMP_TYPE_INST, 0x46200034}},
    {"c.olt.ps", {"c.olt.ps", FP_CMP_TYPE_INST, 0x46600034}}, /* MIPS32 Rev 2 */
    {"c.olt.s", {"c.olt.s", FP_CMP_TYPE_INST, 0x46000034}},
    {"c.seq.d", {"c.seq.d", FP_CMP_TYPE_INST, 0x4620003a}},
    {"c.seq.ps", {"c.seq.ps", FP_CMP_TYPE_INST, 0x4660003a}}, /* MIPS32 Rev 2 */
    {"c.seq.s", {"c.seq.s", FP_CMP_TYPE_INST, 0x4600003a}},
    {"c.sf.d", {"c.sf.d", FP_CMP_TYPE_INST, 0x46200038}},
    {"c.sf.ps", {"c.sf.ps", FP_CMP_TYPE_INST, 0x46600038}}, /* MIPS32 Rev 2 */
    {"c.sf.s", {"c.sf.s", FP_CMP_TYPE_INST, 0x46000038}},
    {"c.ueq.d", {"c.ueq.d", FP_CMP_TYPE_INST, 0x46200033}},
    {"c.ueq.ps", {"c.ueq.ps", FP_CMP_TYPE_INST, 0x46600033}}, /* MIPS32 Rev 2 */
    {"c.ueq.s", {"c.ueq.s", FP_CMP_TYPE_INST, 0x46000033}},
    {"c.ule.d", {"c.ule.d", FP_CMP_TYPE_INST, 0x46200037}},
    {"c.ule.ps", {"c.ule.ps", FP_CMP_TYPE_INST, 0x46600037}}, /* MIPS32 Rev 2 */
    {"c.ule.s", {"c.ule.s", FP_CMP_TYPE_INST, 0x46000037}},
    {"c.ult.d", {"c.ult.d", FP_CMP_TYPE_INST, 0x46200035}},
    {"c.ult.ps", {"c.ult.ps", FP_CMP_TYPE_INST, 0x46600035}}, /* MIPS32 Rev 2 */
    {"c.ult.s", {"c.ult.s", FP_CMP_TYPE_INST, 0x46000035}},
    {"c.un.d", {"c.un.d", FP_CMP_TYPE_INST, 0x46200031}},
    {"c.un.ps", {"c.un.ps", FP_CMP_TYPE_INST, 0x46600031}}, /* MIPS32 Rev 2 */
    {"c.un.s", {"c.un.s", FP_CMP_TYPE_INST, 0x46000031}},

    {"cache", {"cache", I2_TYPE_INST, 0xbc000000}}, /* MIPS32 */

    {"ceil.l.d", {"ceil.l.d", FP_R2ds_TYPE_INST, 0x4620000a}}, /* MIPS32 Rev 2 */
    {"ceil.l.s", {"ceil.l.s", FP_R2ds_TYPE_INST, 0x4600000a}}, /* MIPS32 Rev 2 */
    {"ceil.w.d", {"ceil.w.d", FP_R2ds_TYPE_INST, 0x4620000e}}, /* MIPS32 */
    {"ceil.w.s", {"ceil.w.s", FP_R2ds_TYPE_INST, 0x4600000e}}, /* MIPS32 */

    {"cfc0", {"cfc0", FP_R2ts_TYPE_INST, 0x40400000}},
    {"cfc1", {"cfc1", FP_R2ts_TYPE_INST, 0x44400000}},
    {"cfc2", {"cfc2", FP_R2ts_TYPE_INST, 0x48400000}},

    {"clo", {"clo", R3_TYPE_INST, 0x70000021}},
    {"clz", {"clz", R3_TYPE_INST, 0x70000020}},

    {"cop2", {"cop2", J_TYPE_INST, 0x4a000000}},

    {"ctc0", {"ctc0", FP_R2ts_TYPE_INST, 0x40c00000}},
    {"ctc1", {"ctc1", FP_R2ts_TYPE_INST, 0x44c00000}},
    {"ctc2", {"ctc2", FP_R2ts_TYPE_INST, 0x48c00000}},

    {"cvt.d.l", {"cvt.d.l", FP_R2ds_TYPE_INST, 0x46b00021}}, /* MIPS32 Rev 2 */
    {"cvt.d.s", {"cvt.d.s", FP_R2ds_TYPE_INST, 0x46000021}},
    {"cvt.d.w", {"cvt.d.w", FP_R2ds_TYPE_INST, 0x46200021}},
    {"cvt.l.d", {"cvt.l.d", FP_R2ds_TYPE_INST, 0x46200025}},   /* MIPS32 Rev 2 */
    {"cvt.l.s", {"cvt.l.s", FP_R2ds_TYPE_INST, 0x46000025}},   /* MIPS32 Rev 2 */
    {"cvt.ps.s", {"cvt.ps.s", FP_R2ds_TYPE_INST, 0x46000026}}, /* MIPS32 Rev 2 */
    {"cvt.s.d", {"cvt.s.d", FP_R2ds_TYPE_INST, 0x46200020}},
    {"cvt.s.l", {"cvt.s.l", FP_R2ds_TYPE_INST, 0x46b00020}},   /* MIPS32 Rev 2 */
    {"cvt.s.pl", {"cvt.s.pl", FP_R2ds_TYPE_INST, 0x46c00024}}, /* MIPS32 Rev 2 */
    {"cvt.s.pu", {"cvt.s.pu", FP_R2ds_TYPE_INST, 0x46c00020}}, /* MIPS32 Rev 2 */
    {"cvt.s.w", {"cvt.s.w", FP_R2ds_TYPE_INST, 0x46800020}},
    {"cvt.w.d", {"cvt.w.d", FP_R2ds_TYPE_INST, 0x46200024}},
    {"cvt.w.s", {"cvt.w.s", FP_R2ds_TYPE_INST, 0x46000024}},

    {"deret", {"deret", NOARG_TYPE_INST, 0x4200001f}}, /* MIPS32 Rev 2 */
    {"di", {"di", I1t_TYPE_INST, 0x41606000}},         /* MIPS32 Rev 2 */

    {"div", {"div", R2st_TYPE_INST, 0x0000001a}},
    {"div.d", {"div.d", FP_R3_TYPE_INST, 0x46200003}},
    {"div.s", {"div.s", FP_R3_TYPE_INST, 0x46000003}},
    {"divu", {"divu", R2st_TYPE_INST, 0x0000001b}},

    {"ehb", {"ehb", NOARG_TYPE_INST, 0x000000c0}},   /* MIPS32 Rev 2 */
    {"ei", {"ei", I1t_TYPE_INST, 0x41606020}},       /* MIPS32 Rev 2 */
    {"eret", {"eret", NOARG_TYPE_INST, 0x42000018}}, /* MIPS32 */
    {"ext", {"ext", FP_R2ds_TYPE_INST, 0x7c000000}}, /* MIPS32 Rev 2 */

    {"floor.l.d", {"floor.l.d", FP_R2ds_TYPE_INST, 0x4620000b}}, /* MIPS32 Rev 2 */
    {"floor.l.s", {"floor.l.s", FP_R2ds_TYPE_INST, 0x4600000b}}, /* MIPS32 Rev 2 */
    {"floor.w.d", {"floor.w.d", FP_R2ds_TYPE_INST, 0x4620000f}}, /* MIPS32 */
    {"floor.w.s", {"floor.w.s", FP_R2ds_TYPE_INST, 0x4600000f}}, /* MIPS32 */

    {"ins", {"ins", FP_R2ds_TYPE_INST, 0x7c000004}}, /* MIPS32 Rev 2 */

    {"j", {"j", J_TYPE_INST, 0x08000000}},
    {"jal", {"jal", J_TYPE_INST, 0x0c000000}},
    {"jalr", {"jalr", R2ds_TYPE_INST, 0x00000009}},
    {"jalr.hb", {"jalr.hb", R2ds_TYPE_INST, 0x00000409}}, /* MIPS32 Rev 2 */

    {"jr", {"jr", R1s_TYPE_INST, 0x00000008}},
    {"jr.hb", {"jr.hb", R1s_TYPE_INST, 0x00000408}}, /* MIPS32 Rev 2 */

    {"l.d", {"l.d", PSEUDO_OP, (unsigned) -1}},
    {"l.s", {"l.s", PSEUDO_OP, (unsigned) -1}},

    {"la", {"la", PSEUDO_OP, (unsigned) -1}},
    {"lb", {"lb", I2a_TYPE_INST, 0x80000000}},
    {"lbu", {"lbu", I2a_TYPE_INST, 0x90000000}},
    {"ld", {"ld", PSEUDO_OP, (unsigned) -1}},
    {"ldc1", {"ldc1", FP_I2a_TYPE_INST, 0xd4000000}},  /* MIPS32 */
    {"ldc2", {"ldc2", I2a_TYPE_INST, 0xd8000000}},     /* MIPS32 */
    {"ldxc1", {"ldxc1", FP_R3_TYPE_INST, 0x4c000001}}, /* MIPS32 Rev 2 */
    {"lh", {"lh", I2a_TYPE_INST, 0x84000000}},
    {"lhu", {"lhu", I2a_TYPE_INST, 0x94000000}},

    {"li", {"li", PSEUDO_OP, (unsigned) -1}},
    {"li.d", {"li.d", PSEUDO_OP, (unsigned) -1}},
    {"li.s", {"li.s", PSEUDO_OP, (unsigned) -1}},

    {"ll", {"ll", I2a_TYPE_INST, 0xc0000000}}, /* MIPS32 */

    {"lui", {"lui", I1t_TYPE_INST, 0x3c000000}},
    {"luxc1", {"luxc1", FP_R3_TYPE_INST, 0x4c000005}}, /* MIPS32 Rev 2 */

    {"lw", {"lw", I2a_TYPE_INST, 0x8c000000}},
    {"lwc1", {"lwc1", FP_I2a_TYPE_INST, 0xc4000000}},
    {"lwc2", {"lwc2", I2a_TYPE_INST, 0xc8000000}},
    {"lwl", {"lwl", I2a_TYPE_INST, 0x88000000}},
    {"lwr", {"lwr", I2a_TYPE_INST, 0x98000000}},
    {"lwxc1", {"lwxc1", FP_R3_TYPE_INST, 0x4c000000}}, /* MIPS32 Rev 2 */

    {"madd", {"madd", R2st_TYPE_INST, 0x70000000}},        /* MIPS32 */
    {"madd.d", {"madd.d", FP_R4_TYPE_INST, 0x4c000001}},   /* MIPS32 Rev 2 */
    {"madd.ps", {"madd.ps", FP_R4_TYPE_INST, 0x4c000006}}, /* MIPS32 Rev 2 */
    {"madd.s", {"madd.s", FP_R4_TYPE_INST, 0x4c000000}},   /* MIPS32 Rev 2 */
    {"maddu", {"maddu", R2st_TYPE_INST, 0x70000001}},      /* MIPS32 */

    {"mfc0", {"mfc0", R2td_TYPE_INST, 0x40000000}},
    {"mfc1", {"mfc1", FP_R2ts_TYPE_INST, 0x44000000}},
    {"mfc1.d", {"mfc1.d", PSEUDO_OP, (unsigned) -1}},
    {"mfc2", {"mfc2", R2td_TYPE_INST, 0x48000000}},
    {"mfhc1", {"mfhc1", FP_R2ts_TYPE_INST, 0x44600000}}, /* MIPS32 Rev 2 */
    {"mfhc2", {"mfhc2", R2td_TYPE_INST, 0x48600000}},    /* MIPS32 Rev 2 */
    {"mfhi", {"mfhi", R1d_TYPE_INST, 0x00000010}},
    {"mflo", {"mflo", R1d_TYPE_INST, 0x00000012}},

    {"mov.d", {"mov.d", FP_R2ds_TYPE_INST, 0x46200006}},
    {"mov.ps", {"mov.ps", FP_R2ds_TYPE_INST, 0x46c00006}}, /* MIPS32 Rev 2 */
    {"mov.s", {"mov.s", FP_R2ds_TYPE_INST, 0x46000006}},
    {"move", {"move", PSEUDO_OP, (unsigned) -1}},

    {"movf", {"movf", MOVC_TYPE_INST, 0x00000001}},          /* MIPS32 */
    {"movf.d", {"movf.d", FP_MOVC_TYPE_INST, 0x46200011}},   /* MIPS32 */
    {"movf.ps", {"movf.ps", FP_MOVC_TYPE_INST, 0x46c00011}}, /* MIPS32 Rev 2 */
    {"movf.s", {"movf.s", FP_MOVC_TYPE_INST, 0x46000011}},   /* MIPS32 */

    {"movn", {"movn", R3_TYPE_INST, 0x0000000b}},            /* MIPS32 */
    {"movn.d", {"movn.d", FP_MOVC_TYPE_INST, 0x46200013}},   /* MIPS32 */
    {"movn.ps", {"movn.ps", FP_MOVC_TYPE_INST, 0x46c00013}}, /* MIPS32 Rev 2 */
    {"movn.s", {"movn.s", FP_MOVC_TYPE_INST, 0x46000013}},   /* MIPS32 */

    {"movt", {"movt", MOVC_TYPE_INST, 0x00010001}},          /* MIPS32 */
    {"movt.d", {"movt.d", FP_MOVC_TYPE_INST, 0x46210011}},   /* MIPS32 */
    {"movt.ps", {"movt.ps", FP_MOVC_TYPE_INST, 0x46c10011}}, /* MIPS32 Rev 2 */
    {"movt.s", {"movt.s", FP_MOVC_TYPE_INST, 0x46010011}},   /* MIPS32 */

    {"movz", {"movz", R3_TYPE_INST, 0x0000000a}},            /* MIPS32 */
    {"movz.d", {"movz.d", FP_MOVC_TYPE_INST, 0x46200012}},   /* MIPS32 */
    {"movz.ps", {"movz.ps", FP_MOVC_TYPE_INST, 0x46c00012}}, /* MIPS32 Rev 2 */
    {"movz.s", {"movz.s", FP_MOVC_TYPE_INST, 0x46000012}},   /* MIPS32 */

    {"msub", {"msub", R2st_TYPE_INST, 0x70000004}},        /* MIPS32 */
    {"msub.d", {"msub.d", FP_R4_TYPE_INST, 0x4c000021}},   /* MIPS32 Rev 2 */
    {"msub.ps", {"msub.ps", FP_R4_TYPE_INST, 0x4c000026}}, /* MIPS32 Rev 2 */
    {"msub.s", {"msub.s", FP_R4_TYPE_INST, 0x4c000020}},   /* MIPS32 Rev 2 */
    {"msubu", {"msubu", R2st_TYPE_INST, 0x70000005}},      /* MIPS32 */

    {"mtc0", {"mtc0", R2td_TYPE_INST, 0x40800000}},
    {"mtc1", {"mtc1", FP_R2ts_TYPE_INST, 0x44800000}},
    {"mtc1.d", {"mtc1.d", PSEUDO_OP, (unsigned) -1}},
    {"mtc2", {"mtc2", R2td_TYPE_INST, 0x48800000}},
    {"mthc1", {"mthc1", FP_R2ts_TYPE_INST, 0x44e00000}}, /* MIPS32 Rev 2 */
    {"mthc2", {"mthc2", R2td_TYPE_INST, 0x48e00000}},    /* MIPS32 Rev 2 */

    {"mthi", {"mthi", R1s_TYPE_INST, 0x00000011}},
    {"mtlo", {"mtlo", R1s_TYPE_INST, 0x00000013}},

    {"mul", {"mul", R3_TYPE_INST, 0x70000002}}, /* MIPS32 */
    {"mul.d", {"mul.d", FP_R3_TYPE_INST, 0x46200002}},
    {"mul.ps", {"mul.ps", FP_R3_TYPE_INST, 0x46c00002}}, /* MIPS32 Rev 2 */
    {"mul.s", {"mul.s", FP_R3_TYPE_INST, 0x46000002}},
    {"mulo", {"mulo", PSEUDO_OP, (unsigned) -1}},
    {"mulou", {"mulou", PSEUDO_OP, (unsigned) -1}},
    {"mult", {"mult", R2st_TYPE_INST, 0x00000018}},
    {"multu", {"multu", R2st_TYPE_INST, 0x00000019}},

    {"neg", {"neg", PSEUDO_OP, (unsigned) -1}},
    {"neg.d", {"neg.d", FP_R2ds_TYPE_INST, 0x46200007}},
    {"neg.ps", {"neg.ps", FP_R2ds_TYPE_INST, 0x46c00007}}, /* MIPS32 Rev 2 */
    {"neg.s", {"neg.s", FP_R2ds_TYPE_INST, 0x46000007}},
    {"negu", {"negu", PSEUDO_OP, (unsigned) -1}},

    {"nmadd.d", {"nmadd.d", FP_R4_TYPE_INST, 0x4c000031}},   /* MIPS32 Rev 2 */
    {"nmadd.ps", {"nmadd.ps", FP_R4_TYPE_INST, 0x4c000036}}, /* MIPS32 Rev 2 */
    {"nmadd.s", {"nmadd.s", FP_R4_TYPE_INST, 0x4c000030}},   /* MIPS32 Rev 2 */
    {"nmsub.d", {"nmsub.d", FP_R4_TYPE_INST, 0x4c000039}},   /* MIPS32 Rev 2 */
    {"nmsub.ps", {"nmsub.ps", FP_R4_TYPE_INST, 0x4c00003e}}, /* MIPS32 Rev 2 */
    {"nmsub.s", {"nmsub.s", FP_R4_TYPE_INST, 0x4c000038}},   /* MIPS32 Rev 2 */

    {"nop", {"nop", PSEUDO_OP, (unsigned) -1}},
    {"nor", {"nor", R3_TYPE_INST, 0x00000027}},
    {"not", {"not", PSEUDO_OP, (unsigned) -1}},
    {"or", {"or", R3_TYPE_INST, 0x00000025}},
    {"ori", {"ori", I2_TYPE_INST, 0x34000000}},

    {"pll.ps", {"pll.ps", FP_R3_TYPE_INST, 0x46c0002c}}, /* MIPS32 Rev 2 */
    {"plu.ps", {"plu.ps", FP_R3_TYPE_INST, 0x46c0002d}}, /* MIPS32 Rev 2 */

    {"pref", {"pref", I2_TYPE_INST, 0xcc000000}},        /* MIPS32 */
    {"prefx", {"prefx", R2st_TYPE_INST, 0x4600000f}},    /* MIPS32 Rev 2 */
    {"pul.ps", {"pul.ps", FP_R3_TYPE_INST, 0x46c0002e}}, /* MIPS32 Rev 2 */
    {"puu.ps", {"puu.ps", FP_R3_TYPE_INST, 0x46c0002f}}, /* MIPS32 Rev 2 */

    {"rdhwr", {"rdhwr", R3_TYPE_INST, 0x7c00003b}},     /* MIPS32 Rev 2 */
    {"rdpgpr", {"rdpgpr", R2td_TYPE_INST, 0x41400000}}, /* MIPS32 Rev 2 */

    {"recip.d", {"recip.d", FP_R2ds_TYPE_INST, 0x46200015}}, /* MIPS32 Rev 2 */
    {"recip.s", {"recip.s", FP_R2ds_TYPE_INST, 0x46000015}}, /* MIPS32 Rev 2 */

    {"rem", {"rem", PSEUDO_OP, (unsigned) -1}},
    {"remu", {"remu", PSEUDO_OP, (unsigned) -1}},

    {"rfe", {"rfe", NOARG_TYPE_INST, 0x42000010}},

    {"rol", {"rol", PSEUDO_OP, (unsigned) -1}},
    {"ror", {"ror", PSEUDO_OP, (unsigned) -1}},
    {"rotr", {"rotr", R2sh_TYPE_INST, 0x00200002}},   /* MIPS32 Rev 2 */
    {"rotrv", {"rotrv", R2sh_TYPE_INST, 0x00200003}}, /* MIPS32 Rev 2 */

    {"round.l.d", {"round.l.d", FP_R2ds_TYPE_INST, 0x46200008}}, /* MIPS32 Rev 2 */
    {"round.l.s", {"round.l.s", FP_R2ds_TYPE_INST, 0x46000008}}, /* MIPS32 Rev 2 */
    {"round.w.d", {"round.w.d", FP_R2ds_TYPE_INST, 0x4620000c}}, /* MIPS32 */
    {"round.w.s", {"round.w.s", FP_R2ds_TYPE_INST, 0x4600000c}}, /* MIPS32 */

    {"rsqrt.d", {"rsqrt.d", FP_R2ds_TYPE_INST, 0x46200016}}, /* MIPS32 Rev 2 */
    {"rsqrt.s", {"rsqrt.s", FP_R2ds_TYPE_INST, 0x46000016}}, /* MIPS32 Rev 2 */

    {"s.d", {"s.d", PSEUDO_OP, (unsigned) -1}},
    {"s.s", {"s.s", PSEUDO_OP, (unsigned) -1}},

    {"sb", {"sb", I2a_TYPE_INST, 0xa0000000}},
    {"sc", {"sc", I2a_TYPE_INST, 0xe0000000}}, /* MIPS32 */
    {"sd", {"sd", PSEUDO_OP, (unsigned) -1}},
    {"sdbbp", {"sdbbp", NOARG_TYPE_INST, 0x7000003f}}, /* MIPS32 Rev 2*/
    {"sdc1", {"sdc1", FP_I2a_TYPE_INST, 0xf4000000}},  /* MIPS32 */
    {"sdc2", {"sdc2", I2a_TYPE_INST, 0xf8000000}},     /* MIPS32 */
    {"sdxc1", {"sdxc1", FP_R3_TYPE_INST, 0x46000009}}, /* MIPS32 Rev 2 */

    {"seb", {"seb", R2td_TYPE_INST, 0x7c000420}}, /* MIPS32 Rev 2 */
    {"seh", {"seh", R2td_TYPE_INST, 0x7c000620}}, /* MIPS32 Rev 2 */
    {"seq", {"seq", PSEUDO_OP, (unsigned) -1}},
    {"sge", {"sge", PSEUDO_OP, (unsigned) -1}},
    {"sgeu", {"sgeu", PSEUDO_OP, (unsigned) -1}},
    {"sgt", {"sgt", PSEUDO_OP, (unsigned) -1}},
    {"sgtu", {"sgtu", PSEUDO_OP, (unsigned) -1}},

    {"sh", {"sh", I2a_TYPE_INST, 0xa4000000}},
    {"sle", {"sle", PSEUDO_OP, (unsigned) -1}},
    {"sleu", {"sleu", PSEUDO_OP, (unsigned) -1}},
    {"sll", {"sll", R2sh_TYPE_INST, 0x00000000}},
    {"sllv", {"sllv", R3sh_TYPE_INST, 0x00000004}},

    {"slt", {"slt", R3_TYPE_INST, 0x0000002a}},
    {"slti", {"slti", I2_TYPE_INST, 0x28000000}},
    {"sltiu", {"sltiu", I2_TYPE_INST, 0x2c000000}},
    {"sltu", {"sltu", R3_TYPE_INST, 0x0000002b}},
    {"sne", {"sne", PSEUDO_OP, (unsigned) -1}},

    {"sqrt.d", {"sqrt.d", FP_R2ds_TYPE_INST, 0x46200004}}, /* MIPS32 */
    {"sqrt.s", {"sqrt.s", FP_R2ds_TYPE_INST, 0x46000004}}, /* MIPS32 */

    {"sra", {"sra", R2sh_TYPE_INST, 0x00000003}},
    {"srav", {"srav", R3sh_TYPE_INST, 0x00000007}},
    {"srl", {"srl", R2sh_TYPE_INST, 0x00000002}},
    {"srlv", {"srlv", R3sh_TYPE_INST, 0x00000006}},

    {"ssnop", {"ssnop", R2sh_TYPE_INST, 0x00000040}}, /* MIPS32 */

    {"sub", {"sub", R3_TYPE_INST, 0x00000022}},
    {"sub.d", {"sub.d", FP_R3_TYPE_INST, 0x46200001}},
    {"sub.ps", {"sub.ps", FP_R3_TYPE_INST, 0x46600001}}, /* MIPS32 Rev 2 */
    {"sub.s", {"sub.s", FP_R3_TYPE_INST, 0x46000001}},
    {"subu", {"subu", R3_TYPE_INST, 0x00000023}},

    {"suxc1", {"suxc1", FP_R3_TYPE_INST, 0x4600000d}}, /* MIPS32 Rev 2 */

    {"sw", {"sw", I2a_TYPE_INST, 0xac000000}},
    {"swc1", {"swc1", FP_I2a_TYPE_INST, 0xe4000000}},
    {"swc2", {"swc2", I2a_TYPE_INST, 0xe8000000}},
    {"swl", {"swl", I2a_TYPE_INST, 0xa8000000}},
    {"swr", {"swr", I2a_TYPE_INST, 0xb8000000}},
    {"swxc1", {"swxc1", FP_R3_TYPE_INST, 0x46000008}}, /* MIPS32 Rev 2 */

    {"sync", {"sync", NOARG_TYPE_INST, 0x0000000f}}, /* MIPS32 */
    {"synci", {"synci", I2_TYPE_INST, 0x04140000}},  /* MIPS32 Rev 2 */
    {"syscall", {"syscall", NOARG_TYPE_INST, 0x0000000c}},

    {"teq", {"teq", R2st_TYPE_INST, 0x00000034}},    /* MIPS32 */
    {"teqi", {"teqi", I1s_TYPE_INST, 0x040c0000}},   /* MIPS32 */
    {"tge", {"tge", R2st_TYPE_INST, 0x00000030}},    /* MIPS32 */
    {"tgei", {"tgei", I1s_TYPE_INST, 0x04080000}},   /* MIPS32 */
    {"tgeiu", {"tgeiu", I1s_TYPE_INST, 0x04090000}}, /* MIPS32 */
    {"tgeu", {"tgeu", R2st_TYPE_INST, 0x00000031}},  /* MIPS32 */

    {"tlbp", {"tlbp", NOARG_TYPE_INST, 0x42000008}},
    {"tlbr", {"tlbr", NOARG_TYPE_INST, 0x42000001}},
    {"tlbwi", {"tlbwi", NOARG_TYPE_INST, 0x42000002}},
    {"tlbwr", {"tlbwr", NOARG_TYPE_INST, 0x42000006}},

    {"tlt", {"tlt", R2st_TYPE_INST, 0x00000032}},    /* MIPS32 */
    {"tlti", {"tlti", I1s_TYPE_INST, 0x040a0000}},   /* MIPS32 */
    {"tltiu", {"tltiu", I1s_TYPE_INST, 0x040b0000}}, /* MIPS32 */
    {"tltu", {"tltu", R2st_TYPE_INST, 0x00000033}},  /* MIPS32 */
    {"tne", {"tne", R2st_TYPE_INST, 0x00000036}},    /* MIPS32 */
    {"tnei", {"tnei", I1s_TYPE_INST, 0x040e0000}},   /* MIPS32 */

    {"trunc.l.d", {"trunc.l.d", FP_R2ds_TYPE_INST, 0x46200009}}, /* MIPS32 Rev 2 */
    {"trunc.l.s", {"trunc.l.s", FP_R2ds_TYPE_INST, 0x46000009}}, /* MIPS32 Rev 2 */
    {"trunc.w.d", {"trunc.w.d", FP_R2ds_TYPE_INST, 0x4620000d}}, /* MIPS32 */
    {"trunc.w.s", {"trunc.w.s", FP_R2ds_TYPE_INST, 0x4600000d}}, /* MIPS32 */

    {"ulh", {"ulh", PSEUDO_OP, (unsigned) -1}},
    {"ulhu", {"ulhu", PSEUDO_OP, (unsigned) -1}},
    {"ulw", {"ulw", PSEUDO_OP, (unsigned) -1}},
    {"ush", {"ush", PSEUDO_OP, (unsigned) -1}},
    {"usw", {"usw", PSEUDO_OP, (unsigned) -1}},

    {"wrpgpr", {"wrpgpr", R2td_TYPE_INST, 0x41c00000}}, /* MIPS32 Rev 2 */
    {"wsbh", {"wsbh", R2td_TYPE_INST, 0x7c0000a0}},     /* MIPS32 Rev 2 */

    {"xor", {"xor", R3_TYPE_INST, 0x00000026}},
    {"xori", {"xori", I2_TYPE_INST, 0x38000000}},
};

struct Instruction : x3::symbols<int> {
    Instruction() {
        for (const auto& pair : opcodes) {
            this->add(pair.first);
        }
    }
};

const auto instruction_ = Instruction{};

}  // namespace mips_parser

#endif