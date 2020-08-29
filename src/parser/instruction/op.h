/* SPIM S20 MIPS simulator.
   This file describes the MIPS instructions, the assembler pseudo
   instructions, the assembler pseudo-ops, and the spim commands.

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

#ifndef SPIMBOT_PARSER_INSTRUCTION_OP_H
#define SPIMBOT_PARSER_INSTRUCTION_OP_H

/* Type of each entry: */
#include <string>
#include <unordered_map>

#include <cstdint>

enum InstId: uint16_t {
    ABS_D_OP,
    ABS_PS_OP,
    ABS_S_OP,
    ADD_D_OP,
    ADD_OP,
    ADD_PS_OP,
    ADD_S_OP,
    ADDI_OP,
    ADDIU_OP,
    ADDU_OP,
    ALNV_PS_OP,
    AND_OP,
    ANDI_OP,
    BC1F_OP,
    BC1FL_OP,
    BC1T_OP,
    BC1TL_OP,
    BC2F_OP,
    BC2FL_OP,
    BC2T_OP,
    BC2TL_OP,
    BEQ_OP,
    BEQL_OP,
    BGEZ_OP,
    BGEZAL_OP,
    BGEZALL_OP,
    BGEZL_OP,
    BGTZ_OP,
    BGTZL_OP,
    BLEZ_OP,
    BLEZL_OP,
    BLTZ_OP,
    BLTZAL_OP,
    BLTZALL_OP,
    BLTZL_OP,
    BNE_OP,
    BNEL_OP,
    BREAK_OP,
    C_EQ_D_OP,
    C_EQ_PS_OP,
    C_EQ_S_OP,
    C_F_D_OP,
    C_F_PS_OP,
    C_F_S_OP,
    C_LE_D_OP,
    C_LE_PS_OP,
    C_LE_S_OP,
    C_LT_D_OP,
    C_LT_PS_OP,
    C_LT_S_OP,
    C_NGE_D_OP,
    C_NGE_PS_OP,
    C_NGE_S_OP,
    C_NGL_D_OP,
    C_NGL_PS_OP,
    C_NGL_S_OP,
    C_NGLE_D_OP,
    C_NGLE_PS_OP,
    C_NGLE_S_OP,
    C_NGT_D_OP,
    C_NGT_PS_OP,
    C_NGT_S_OP,
    C_OLE_D_OP,
    C_OLE_PS_OP,
    C_OLE_S_OP,
    C_OLT_D_OP,
    C_OLT_PS_OP,
    C_OLT_S_OP,
    C_SEQ_D_OP,
    C_SEQ_PS_OP,
    C_SEQ_S_OP,
    C_SF_D_OP,
    C_SF_PS_OP,
    C_SF_S_OP,
    C_UEQ_D_OP,
    C_UEQ_PS_OP,
    C_UEQ_S_OP,
    C_ULE_D_OP,
    C_ULE_PS_OP,
    C_ULE_S_OP,
    C_ULT_D_OP,
    C_ULT_PS_OP,
    C_ULT_S_OP,
    C_UN_D_OP,
    C_UN_PS_OP,
    C_UN_S_OP,
    CACHE_OP,
    CEIL_L_D_OP,
    CEIL_L_S_OP,
    CEIL_W_D_OP,
    CEIL_W_S_OP,
    CFC0_OP,
    CFC1_OP,
    CFC2_OP,
    CLO_OP,
    CLZ_OP,
    COP2_OP,
    CTC0_OP,
    CTC1_OP,
    CTC2_OP,
    CVT_D_L_OP,
    CVT_D_S_OP,
    CVT_D_W_OP,
    CVT_L_D_OP,
    CVT_L_S_OP,
    CVT_PS_S_OP,
    CVT_S_D_OP,
    CVT_S_L_OP,
    CVT_S_PL_OP,
    CVT_S_PU_OP,
    CVT_S_W_OP,
    CVT_W_D_OP,
    CVT_W_S_OP,
    DERET_OP,
    DI_OP,
    DIV_D_OP,
    DIV_OP,
    DIV_S_OP,
    DIVU_OP,
    EHB_OP,
    EI_OP,
    ERET_OP,
    EXT_OP,
    FLOOR_L_D_OP,
    FLOOR_L_S_OP,
    FLOOR_W_D_OP,
    FLOOR_W_S_OP,
    INS_OP,
    J_OP,
    JAL_OP,
    JALR_HB_OP,
    JALR_OP,
    JR_HB_OP,
    JR_OP,
    LB_OP,
    LBU_OP,
    LDC1_OP,
    LDC2_OP,
    LDXC1_OP,
    LH_OP,
    LHU_OP,
    LL_OP,
    LUI_OP,
    LUXC1_OP,
    LW_OP,
    LWC1_OP,
    LWC2_OP,
    LWL_OP,
    LWR_OP,
    LWXC1_OP,
    MADD_D_OP,
    MADD_OP,
    MADD_PS_OP,
    MADD_S_OP,
    MADDU_OP,
    MFC0_OP,
    MFC1_OP,
    MFC2_OP,
    MFHC1_OP,
    MFHC2_OP,
    MFHI_OP,
    MFLO_OP,
    MOV_D_OP,
    MOV_PS_OP,
    MOV_S_OP,
    MOVF_D_OP,
    MOVF_OP,
    MOVF_PS_OP,
    MOVF_S_OP,
    MOVN_D_OP,
    MOVN_OP,
    MOVN_PS_OP,
    MOVN_S_OP,
    MOVT_D_OP,
    MOVT_OP,
    MOVT_PS_OP,
    MOVT_S_OP,
    MOVZ_D_OP,
    MOVZ_OP,
    MOVZ_PS_OP,
    MOVZ_S_OP,
    MSUB_D_OP,
    MSUB_OP,
    MSUB_PS_OP,
    MSUB_S_OP,
    MSUBU_OP,
    MTC0_OP,
    MTC1_OP,
    MTC2_OP,
    MTHC1_OP,
    MTHC2_OP,
    MTHI_OP,
    MTLO_OP,
    MUL_D_OP,
    MUL_PS_OP,
    MUL_S_OP,
    MUL_OP,
    MULT_OP,
    MULTU_OP,
    NEG_D_OP,
    NEG_PS_OP,
    NEG_S_OP,
    NMADD_D_OP,
    NMADD_PS_OP,
    NMADD_S_OP,
    NMSUB_D_OP,
    NMSUB_PS_OP,
    NMSUB_S_OP,
    NOR_OP,
    OR_OP,
    ORI_OP,
    PFW_OP,
    PLL_PS_OP,
    PLU_PS_OP,
    PREF_OP,
    PREFX_OP,
    PUL_PS_OP,
    PUU_PS_OP,
    RDHWR_OP,
    RDPGPR_OP,
    RECIP_D_OP,
    RECIP_S_OP,
    RFE_OP,
    ROTR_OP,
    ROTRV_OP,
    ROUND_L_D_OP,
    ROUND_L_S_OP,
    ROUND_W_D_OP,
    ROUND_W_S_OP,
    RSQRT_D_OP,
    RSQRT_S_OP,
    SB_OP,
    SC_OP,
    SDBBP_OP,
    SDC1_OP,
    SDC2_OP,
    SDXC1_OP,
    SEB_OP,
    SEH_OP,
    SH_OP,
    SLL_OP,
    SLLV_OP,
    SLT_OP,
    SLTI_OP,
    SLTIU_OP,
    SLTU_OP,
    SQRT_D_OP,
    SQRT_S_OP,
    SRA_OP,
    SRAV_OP,
    SRL_OP,
    SRLV_OP,
    SSNOP_OP,
    SUB_D_OP,
    SUB_OP,
    SUB_PS_OP,
    SUB_S_OP,
    SUBU_OP,
    SUXC1_OP,
    SW_OP,
    SWC1_OP,
    SWC2_OP,
    SWL_OP,
    SWR_OP,
    SWXC1_OP,
    SYNC_OP,
    SYNCI_OP,
    SYSCALL_OP,
    TEQ_OP,
    TEQI_OP,
    TGE_OP,
    TGEI_OP,
    TGEIU_OP,
    TGEU_OP,
    TLBP_OP,
    TLBR_OP,
    TLBWI_OP,
    TLBWR_OP,
    TLT_OP,
    TLTI_OP,
    TLTIU_OP,
    TLTU_OP,
    TNE_OP,
    TNEI_OP,
    TRUNC_L_D_OP,
    TRUNC_L_S_OP,
    TRUNC_W_D_OP,
    TRUNC_W_S_OP,
    WRPGPR_OP,
    WSBH_OP,
    XOR_OP,
    XORI_OP,
    ABS_POP,
    B_POP,
    BAL_POP,
    BEQZ_POP,
    BGE_POP,
    BGEU_POP,
    BGT_POP,
    BGTU_POP,
    BLE_POP,
    BLEU_POP,
    BLT_POP,
    BLTU_POP,
    BNEZ_POP,
    LA_POP,
    LD_POP,
    L_D_POP,
    L_S_POP,
    LI_D_POP,
    LI_POP,
    LI_S_POP,
    MFC1_D_POP,
    MOVE_POP,
    MTC1_D_POP,
    MULO_POP,
    MULOU_POP,
    NEG_POP,
    NEGU_POP,
    NOP_POP,
    NOT_POP,
    REM_POP,
    REMU_POP,
    ROL_POP,
    ROR_POP,
    S_D_POP,
    S_S_POP,
    SD_POP,
    SEQ_POP,
    SGE_POP,
    SGEU_POP,
    SGT_POP,
    SGTU_POP,
    SLE_POP,
    SLEU_POP,
    SNE_POP,
    ULH_POP,
    ULHU_POP,
    ULW_POP,
    USH_POP,
    USW_POP,
};

enum InstructionCategory {
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

struct InstructionBase {
    std::string name;
    InstId id;
    InstructionCategory type;
    uint32_t opcode;
};

const std::unordered_map<std::string, InstructionBase> INSTRUCTION_TEMPLATES = {
    {"abs", {"abs", ABS_POP, PSEUDO_OP, (unsigned) (unsigned) -1}},
    {"abs.d", {"abs.d", ABS_D_OP, FP_R2ds_TYPE_INST, 0x46200005}},
    {"abs.ps", {"abs.ps", ABS_PS_OP, FP_R2ds_TYPE_INST, 0x46600005}}, /* MIPS32 Rev 2 */
    {"abs.s", {"abs.s", ABS_S_OP, FP_R2ds_TYPE_INST, 0x46000005}},

    {"add", {"add", ADD_OP, R3_TYPE_INST, 0x00000020}},
    {"add.d", {"add.d", ADD_D_OP, FP_R3_TYPE_INST, 0x46200000}},
    {"add.ps", {"add.ps", ADD_PS_OP, FP_R3_TYPE_INST, 0x46600000}}, /* MIPS32 Rev 2 */
    {"add.s", {"add.s", ADD_S_OP, FP_R3_TYPE_INST, 0x46000000}},
    {"addi", {"addi", ADDI_OP, I2_TYPE_INST, 0x20000000}},
    {"addiu", {"addiu", ADDIU_OP, I2_TYPE_INST, 0x24000000}},
    {"addu", {"addu", ADDU_OP, R3_TYPE_INST, 0x00000021}},

    {"alnv.ps", {"alnv.ps", ALNV_PS_OP, FP_R4_TYPE_INST, 0x4c00001e}}, /* MIPS32 Rev 2 */

    {"and", {"and", AND_OP, R3_TYPE_INST, 0x00000024}},
    {"andi", {"andi", ANDI_OP, I2_TYPE_INST, 0x30000000}},

    {"b", {"b", B_POP, PSEUDO_OP, (unsigned) -1}},
    {"bal", {"bal", BAL_POP, PSEUDO_OP, (unsigned) -1}},

    {"bc1f", {"bc1f", BC1F_OP, BC_TYPE_INST, 0x45000000}},
    {"bc1fl", {"bc1fl", BC1FL_OP, BC_TYPE_INST, 0x45020000}}, /* MIPS32 */
    {"bc1t", {"bc1t", BC1T_OP, BC_TYPE_INST, 0x45010000}},
    {"bc1tl", {"bc1tl", BC1TL_OP, BC_TYPE_INST, 0x45030000}}, /* MIPS32 */

    {"bc2f", {"bc2f", BC2F_OP, BC_TYPE_INST, 0x49000000}},
    {"bc2fl", {"bc2fl", BC2FL_OP, BC_TYPE_INST, 0x49020000}}, /* MIPS32 */
    {"bc2t", {"bc2t", BC2T_OP, BC_TYPE_INST, 0x49010000}},
    {"bc2tl", {"bc2tl", BC2TL_OP, BC_TYPE_INST, 0x49030000}}, /* MIPS32 */

    {"beq", {"beq", BEQ_OP, B2_TYPE_INST, 0x10000000}},
    {"beql", {"beql", BEQL_OP, B2_TYPE_INST, 0x50000000}}, /* MIPS32 */
    {"beqz", {"beqz", BEQZ_POP, PSEUDO_OP, (unsigned) -1}},
    {"bge", {"bge", BGE_POP, PSEUDO_OP, (unsigned) -1}},
    {"bgeu", {"bgeu", BGEU_POP, PSEUDO_OP, (unsigned) -1}},
    {"bgez", {"bgez", BGEZ_OP, B1_TYPE_INST, 0x04010000}},
    {"bgezal", {"bgezal", BGEZAL_OP, B1_TYPE_INST, 0x04110000}},
    {"bgezall", {"bgezall", BGEZALL_OP, B1_TYPE_INST, 0x04130000}}, /* MIPS32 */
    {"bgezl", {"bgezl", BGEZL_OP, B1_TYPE_INST, 0x04030000}},       /* MIPS32 */
    {"bgt", {"bgt", BGT_POP, PSEUDO_OP, (unsigned) -1}},
    {"bgtu", {"bgtu", BGTU_POP, PSEUDO_OP, (unsigned) -1}},
    {"bgtz", {"bgtz", BGTZ_OP, B1_TYPE_INST, 0x1c000000}},
    {"bgtzl", {"bgtzl", BGTZL_OP, B1_TYPE_INST, 0x5c000000}}, /* MIPS32 */
    {"ble", {"ble", BLE_POP, PSEUDO_OP, (unsigned) -1}},
    {"bleu", {"bleu", BLEU_POP, PSEUDO_OP, (unsigned) -1}},
    {"blez", {"blez", BLEZ_OP, B1_TYPE_INST, 0x18000000}},
    {"blezl", {"blezl", BLEZL_OP, B1_TYPE_INST, 0x58000000}}, /* MIPS32 */
    {"blt", {"blt", BLT_POP, PSEUDO_OP, (unsigned) -1}},
    {"bltu", {"bltu", BLTU_POP, PSEUDO_OP, (unsigned) -1}},
    {"bltz", {"bltz", BLTZ_OP, B1_TYPE_INST, 0x04000000}},
    {"bltzal", {"bltzal", BLTZAL_OP, B1_TYPE_INST, 0x04100000}},
    {"bltzall", {"bltzall", BLTZALL_OP, B1_TYPE_INST, 0x04120000}}, /* MIPS32 */
    {"bltzl", {"bltzl", BLTZL_OP, B1_TYPE_INST, 0x04020000}},       /* MIPS32 */
    {"bne", {"bne", BNE_OP, B2_TYPE_INST, 0x14000000}},
    {"bnel", {"bnel", BNEL_OP, B2_TYPE_INST, 0x54000000}}, /* MIPS32 */
    {"bnez", {"bnez", BNEZ_POP, PSEUDO_OP, (unsigned) -1}},

    {"break", {"break", BREAK_OP, NOARG_TYPE_INST, 0x0000000d}},

    {"c.eq.d", {"c.eq.d", C_EQ_D_OP, FP_CMP_TYPE_INST, 0x46200032}},
    {"c.eq.ps", {"c.eq.ps", C_EQ_PS_OP, FP_CMP_TYPE_INST, 0x46600032}}, /* MIPS32 Rev 2 */
    {"c.eq.s", {"c.eq.s", C_EQ_S_OP, FP_CMP_TYPE_INST, 0x46000032}},
    {"c.f.d", {"c.f.d", C_F_D_OP, FP_CMP_TYPE_INST, 0x46200030}},
    {"c.f.ps", {"c.f.ps", C_F_PS_OP, FP_CMP_TYPE_INST, 0x46600030}}, /* MIPS32 Rev 2 */
    {"c.f.s", {"c.f.s", C_F_S_OP, FP_CMP_TYPE_INST, 0x46000030}},
    {"c.le.d", {"c.le.d", C_LE_D_OP, FP_CMP_TYPE_INST, 0x4620003e}},
    {"c.le.ps", {"c.le.ps", C_LE_PS_OP, FP_CMP_TYPE_INST, 0x4660003e}}, /* MIPS32 Rev 2 */
    {"c.le.s", {"c.le.s", C_LE_S_OP, FP_CMP_TYPE_INST, 0x4600003e}},
    {"c.lt.d", {"c.lt.d", C_LT_D_OP, FP_CMP_TYPE_INST, 0x4620003c}},
    {"c.lt.ps", {"c.lt.ps", C_LT_PS_OP, FP_CMP_TYPE_INST, 0x4660003c}}, /* MIPS32 Rev 2 */
    {"c.lt.s", {"c.lt.s", C_LT_S_OP, FP_CMP_TYPE_INST, 0x4600003c}},
    {"c.nge.d", {"c.nge.d", C_NGE_D_OP, FP_CMP_TYPE_INST, 0x4620003d}},
    {"c.nge.ps", {"c.nge.ps", C_NGE_PS_OP, FP_CMP_TYPE_INST, 0x4660003d}}, /* MIPS32 Rev 2 */
    {"c.nge.s", {"c.nge.s", C_NGE_S_OP, FP_CMP_TYPE_INST, 0x4600003d}},
    {"c.ngl.d", {"c.ngl.d", C_NGL_D_OP, FP_CMP_TYPE_INST, 0x4620003b}},
    {"c.ngl.ps", {"c.ngl.ps", C_NGL_PS_OP, FP_CMP_TYPE_INST, 0x4660003b}}, /* MIPS32 Rev 2 */
    {"c.ngl.s", {"c.ngl.s", C_NGL_S_OP, FP_CMP_TYPE_INST, 0x4600003b}},
    {"c.ngle.d", {"c.ngle.d", C_NGLE_D_OP, FP_CMP_TYPE_INST, 0x46200039}},
    {"c.ngle.ps", {"c.ngle.ps", C_NGLE_PS_OP, FP_CMP_TYPE_INST, 0x46600039}}, /* MIPS32 Rev 2 */
    {"c.ngle.s", {"c.ngle.s", C_NGLE_S_OP, FP_CMP_TYPE_INST, 0x46000039}},
    {"c.ngt.d", {"c.ngt.d", C_NGT_D_OP, FP_CMP_TYPE_INST, 0x4620003f}},
    {"c.ngt.ps", {"c.ngt.ps", C_NGT_PS_OP, FP_CMP_TYPE_INST, 0x4660003f}}, /* MIPS32 Rev 2 */
    {"c.ngt.s", {"c.ngt.s", C_NGT_S_OP, FP_CMP_TYPE_INST, 0x4600003f}},
    {"c.ole.d", {"c.ole.d", C_OLE_D_OP, FP_CMP_TYPE_INST, 0x46200036}},
    {"c.ole.ps", {"c.ole.ps", C_OLE_PS_OP, FP_CMP_TYPE_INST, 0x46600036}}, /* MIPS32 Rev 2 */
    {"c.ole.s", {"c.ole.s", C_OLE_S_OP, FP_CMP_TYPE_INST, 0x46000036}},
    {"c.olt.d", {"c.olt.d", C_OLT_D_OP, FP_CMP_TYPE_INST, 0x46200034}},
    {"c.olt.ps", {"c.olt.ps", C_OLT_PS_OP, FP_CMP_TYPE_INST, 0x46600034}}, /* MIPS32 Rev 2 */
    {"c.olt.s", {"c.olt.s", C_OLT_S_OP, FP_CMP_TYPE_INST, 0x46000034}},
    {"c.seq.d", {"c.seq.d", C_SEQ_D_OP, FP_CMP_TYPE_INST, 0x4620003a}},
    {"c.seq.ps", {"c.seq.ps", C_SEQ_PS_OP, FP_CMP_TYPE_INST, 0x4660003a}}, /* MIPS32 Rev 2 */
    {"c.seq.s", {"c.seq.s", C_SEQ_S_OP, FP_CMP_TYPE_INST, 0x4600003a}},
    {"c.sf.d", {"c.sf.d", C_SF_D_OP, FP_CMP_TYPE_INST, 0x46200038}},
    {"c.sf.ps", {"c.sf.ps", C_SF_PS_OP, FP_CMP_TYPE_INST, 0x46600038}}, /* MIPS32 Rev 2 */
    {"c.sf.s", {"c.sf.s", C_SF_S_OP, FP_CMP_TYPE_INST, 0x46000038}},
    {"c.ueq.d", {"c.ueq.d", C_UEQ_D_OP, FP_CMP_TYPE_INST, 0x46200033}},
    {"c.ueq.ps", {"c.ueq.ps", C_UEQ_PS_OP, FP_CMP_TYPE_INST, 0x46600033}}, /* MIPS32 Rev 2 */
    {"c.ueq.s", {"c.ueq.s", C_UEQ_S_OP, FP_CMP_TYPE_INST, 0x46000033}},
    {"c.ule.d", {"c.ule.d", C_ULE_D_OP, FP_CMP_TYPE_INST, 0x46200037}},
    {"c.ule.ps", {"c.ule.ps", C_ULE_PS_OP, FP_CMP_TYPE_INST, 0x46600037}}, /* MIPS32 Rev 2 */
    {"c.ule.s", {"c.ule.s", C_ULE_S_OP, FP_CMP_TYPE_INST, 0x46000037}},
    {"c.ult.d", {"c.ult.d", C_ULT_D_OP, FP_CMP_TYPE_INST, 0x46200035}},
    {"c.ult.ps", {"c.ult.ps", C_ULT_PS_OP, FP_CMP_TYPE_INST, 0x46600035}}, /* MIPS32 Rev 2 */
    {"c.ult.s", {"c.ult.s", C_ULT_S_OP, FP_CMP_TYPE_INST, 0x46000035}},
    {"c.un.d", {"c.un.d", C_UN_D_OP, FP_CMP_TYPE_INST, 0x46200031}},
    {"c.un.ps", {"c.un.ps", C_UN_PS_OP, FP_CMP_TYPE_INST, 0x46600031}}, /* MIPS32 Rev 2 */
    {"c.un.s", {"c.un.s", C_UN_S_OP, FP_CMP_TYPE_INST, 0x46000031}},

    {"cache", {"cache", CACHE_OP, I2_TYPE_INST, 0xbc000000}}, /* MIPS32 */

    {"ceil.l.d", {"ceil.l.d", CEIL_L_D_OP, FP_R2ds_TYPE_INST, 0x4620000a}}, /* MIPS32 Rev 2 */
    {"ceil.l.s", {"ceil.l.s", CEIL_L_S_OP, FP_R2ds_TYPE_INST, 0x4600000a}}, /* MIPS32 Rev 2 */
    {"ceil.w.d", {"ceil.w.d", CEIL_W_D_OP, FP_R2ds_TYPE_INST, 0x4620000e}}, /* MIPS32 */
    {"ceil.w.s", {"ceil.w.s", CEIL_W_S_OP, FP_R2ds_TYPE_INST, 0x4600000e}}, /* MIPS32 */

    {"cfc0", {"cfc0", CFC0_OP, FP_R2ts_TYPE_INST, 0x40400000}},
    {"cfc1", {"cfc1", CFC1_OP, FP_R2ts_TYPE_INST, 0x44400000}},
    {"cfc2", {"cfc2", CFC2_OP, FP_R2ts_TYPE_INST, 0x48400000}},

    {"clo", {"clo", CLO_OP, R3_TYPE_INST, 0x70000021}},
    {"clz", {"clz", CLZ_OP, R3_TYPE_INST, 0x70000020}},

    {"cop2", {"cop2", COP2_OP, J_TYPE_INST, 0x4a000000}},

    {"ctc0", {"ctc0", CTC0_OP, FP_R2ts_TYPE_INST, 0x40c00000}},
    {"ctc1", {"ctc1", CTC1_OP, FP_R2ts_TYPE_INST, 0x44c00000}},
    {"ctc2", {"ctc2", CTC2_OP, FP_R2ts_TYPE_INST, 0x48c00000}},

    {"cvt.d.l", {"cvt.d.l", CVT_D_L_OP, FP_R2ds_TYPE_INST, 0x46b00021}}, /* MIPS32 Rev 2 */
    {"cvt.d.s", {"cvt.d.s", CVT_D_S_OP, FP_R2ds_TYPE_INST, 0x46000021}},
    {"cvt.d.w", {"cvt.d.w", CVT_D_W_OP, FP_R2ds_TYPE_INST, 0x46200021}},
    {"cvt.l.d", {"cvt.l.d", CVT_L_D_OP, FP_R2ds_TYPE_INST, 0x46200025}},    /* MIPS32 Rev 2 */
    {"cvt.l.s", {"cvt.l.s", CVT_L_S_OP, FP_R2ds_TYPE_INST, 0x46000025}},    /* MIPS32 Rev 2 */
    {"cvt.ps.s", {"cvt.ps.s", CVT_PS_S_OP, FP_R2ds_TYPE_INST, 0x46000026}}, /* MIPS32 Rev 2 */
    {"cvt.s.d", {"cvt.s.d", CVT_S_D_OP, FP_R2ds_TYPE_INST, 0x46200020}},
    {"cvt.s.l", {"cvt.s.l", CVT_S_L_OP, FP_R2ds_TYPE_INST, 0x46b00020}},    /* MIPS32 Rev 2 */
    {"cvt.s.pl", {"cvt.s.pl", CVT_S_PL_OP, FP_R2ds_TYPE_INST, 0x46c00024}}, /* MIPS32 Rev 2 */
    {"cvt.s.pu", {"cvt.s.pu", CVT_S_PU_OP, FP_R2ds_TYPE_INST, 0x46c00020}}, /* MIPS32 Rev 2 */
    {"cvt.s.w", {"cvt.s.w", CVT_S_W_OP, FP_R2ds_TYPE_INST, 0x46800020}},
    {"cvt.w.d", {"cvt.w.d", CVT_W_D_OP, FP_R2ds_TYPE_INST, 0x46200024}},
    {"cvt.w.s", {"cvt.w.s", CVT_W_S_OP, FP_R2ds_TYPE_INST, 0x46000024}},

    {"deret", {"deret", DERET_OP, NOARG_TYPE_INST, 0x4200001f}}, /* MIPS32 Rev 2 */
    {"di", {"di", DI_OP, I1t_TYPE_INST, 0x41606000}},            /* MIPS32 Rev 2 */

    {"div", {"div", DIV_OP, R2st_TYPE_INST, 0x0000001a}},
    {"div.d", {"div.d", DIV_D_OP, FP_R3_TYPE_INST, 0x46200003}},
    {"div.s", {"div.s", DIV_S_OP, FP_R3_TYPE_INST, 0x46000003}},
    {"divu", {"divu", DIVU_OP, R2st_TYPE_INST, 0x0000001b}},

    {"ehb", {"ehb", EHB_OP, NOARG_TYPE_INST, 0x000000c0}},    /* MIPS32 Rev 2 */
    {"ei", {"ei", EI_OP, I1t_TYPE_INST, 0x41606020}},         /* MIPS32 Rev 2 */
    {"eret", {"eret", ERET_OP, NOARG_TYPE_INST, 0x42000018}}, /* MIPS32 */
    {"ext", {"ext", EXT_OP, FP_R2ds_TYPE_INST, 0x7c000000}},  /* MIPS32 Rev 2 */

    {"floor.l.d", {"floor.l.d", FLOOR_L_D_OP, FP_R2ds_TYPE_INST, 0x4620000b}}, /* MIPS32 Rev 2 */
    {"floor.l.s", {"floor.l.s", FLOOR_L_S_OP, FP_R2ds_TYPE_INST, 0x4600000b}}, /* MIPS32 Rev 2 */
    {"floor.w.d", {"floor.w.d", FLOOR_W_D_OP, FP_R2ds_TYPE_INST, 0x4620000f}}, /* MIPS32 */
    {"floor.w.s", {"floor.w.s", FLOOR_W_S_OP, FP_R2ds_TYPE_INST, 0x4600000f}}, /* MIPS32 */

    {"ins", {"ins", INS_OP, FP_R2ds_TYPE_INST, 0x7c000004}}, /* MIPS32 Rev 2 */

    {"j", {"j", J_OP, J_TYPE_INST, 0x08000000}},
    {"jal", {"jal", JAL_OP, J_TYPE_INST, 0x0c000000}},
    {"jalr", {"jalr", JALR_OP, R2ds_TYPE_INST, 0x00000009}},
    {"jalr.hb", {"jalr.hb", JALR_HB_OP, R2ds_TYPE_INST, 0x00000409}}, /* MIPS32 Rev 2 */

    {"jr", {"jr", JR_OP, R1s_TYPE_INST, 0x00000008}},
    {"jr.hb", {"jr.hb", JR_HB_OP, R1s_TYPE_INST, 0x00000408}}, /* MIPS32 Rev 2 */

    {"l.d", {"l.d", L_D_POP, PSEUDO_OP, (unsigned) -1}},
    {"l.s", {"l.s", L_S_POP, PSEUDO_OP, (unsigned) -1}},

    {"la", {"la", LA_POP, PSEUDO_OP, (unsigned) -1}},
    {"lb", {"lb", LB_OP, I2a_TYPE_INST, 0x80000000}},
    {"lbu", {"lbu", LBU_OP, I2a_TYPE_INST, 0x90000000}},
    {"ld", {"ld", LD_POP, PSEUDO_OP, (unsigned) -1}},
    {"ldc1", {"ldc1", LDC1_OP, FP_I2a_TYPE_INST, 0xd4000000}},   /* MIPS32 */
    {"ldc2", {"ldc2", LDC2_OP, I2a_TYPE_INST, 0xd8000000}},      /* MIPS32 */
    {"ldxc1", {"ldxc1", LDXC1_OP, FP_R3_TYPE_INST, 0x4c000001}}, /* MIPS32 Rev 2 */
    {"lh", {"lh", LH_OP, I2a_TYPE_INST, 0x84000000}},
    {"lhu", {"lhu", LHU_OP, I2a_TYPE_INST, 0x94000000}},

    {"li", {"li", LI_POP, PSEUDO_OP, (unsigned) -1}},
    {"li.d", {"li.d", LI_D_POP, PSEUDO_OP, (unsigned) -1}},
    {"li.s", {"li.s", LI_S_POP, PSEUDO_OP, (unsigned) -1}},

    {"ll", {"ll", LL_OP, I2a_TYPE_INST, 0xc0000000}}, /* MIPS32 */

    {"lui", {"lui", LUI_OP, I1t_TYPE_INST, 0x3c000000}},
    {"luxc1", {"luxc1", LUXC1_OP, FP_R3_TYPE_INST, 0x4c000005}}, /* MIPS32 Rev 2 */

    {"lw", {"lw", LW_OP, I2a_TYPE_INST, 0x8c000000}},
    {"lwc1", {"lwc1", LWC1_OP, FP_I2a_TYPE_INST, 0xc4000000}},
    {"lwc2", {"lwc2", LWC2_OP, I2a_TYPE_INST, 0xc8000000}},
    {"lwl", {"lwl", LWL_OP, I2a_TYPE_INST, 0x88000000}},
    {"lwr", {"lwr", LWR_OP, I2a_TYPE_INST, 0x98000000}},
    {"lwxc1", {"lwxc1", LWXC1_OP, FP_R3_TYPE_INST, 0x4c000000}}, /* MIPS32 Rev 2 */

    {"madd", {"madd", MADD_OP, R2st_TYPE_INST, 0x70000000}},           /* MIPS32 */
    {"madd.d", {"madd.d", MADD_D_OP, FP_R4_TYPE_INST, 0x4c000001}},    /* MIPS32 Rev 2 */
    {"madd.ps", {"madd.ps", MADD_PS_OP, FP_R4_TYPE_INST, 0x4c000006}}, /* MIPS32 Rev 2 */
    {"madd.s", {"madd.s", MADD_S_OP, FP_R4_TYPE_INST, 0x4c000000}},    /* MIPS32 Rev 2 */
    {"maddu", {"maddu", MADDU_OP, R2st_TYPE_INST, 0x70000001}},        /* MIPS32 */

    {"mfc0", {"mfc0", MFC0_OP, R2td_TYPE_INST, 0x40000000}},
    {"mfc1", {"mfc1", MFC1_OP, FP_R2ts_TYPE_INST, 0x44000000}},
    {"mfc1.d", {"mfc1.d", MFC1_D_POP, PSEUDO_OP, (unsigned) -1}},
    {"mfc2", {"mfc2", MFC2_OP, R2td_TYPE_INST, 0x48000000}},
    {"mfhc1", {"mfhc1", MFHC1_OP, FP_R2ts_TYPE_INST, 0x44600000}}, /* MIPS32 Rev 2 */
    {"mfhc2", {"mfhc2", MFHC2_OP, R2td_TYPE_INST, 0x48600000}},    /* MIPS32 Rev 2 */
    {"mfhi", {"mfhi", MFHI_OP, R1d_TYPE_INST, 0x00000010}},
    {"mflo", {"mflo", MFLO_OP, R1d_TYPE_INST, 0x00000012}},

    {"mov.d", {"mov.d", MOV_D_OP, FP_R2ds_TYPE_INST, 0x46200006}},
    {"mov.ps", {"mov.ps", MOV_PS_OP, FP_R2ds_TYPE_INST, 0x46c00006}}, /* MIPS32 Rev 2 */
    {"mov.s", {"mov.s", MOV_S_OP, FP_R2ds_TYPE_INST, 0x46000006}},
    {"move", {"move", MOVE_POP, PSEUDO_OP, (unsigned) -1}},

    {"movf", {"movf", MOVF_OP, MOVC_TYPE_INST, 0x00000001}},             /* MIPS32 */
    {"movf.d", {"movf.d", MOVF_D_OP, FP_MOVC_TYPE_INST, 0x46200011}},    /* MIPS32 */
    {"movf.ps", {"movf.ps", MOVF_PS_OP, FP_MOVC_TYPE_INST, 0x46c00011}}, /* MIPS32 Rev 2 */
    {"movf.s", {"movf.s", MOVF_S_OP, FP_MOVC_TYPE_INST, 0x46000011}},    /* MIPS32 */

    {"movn", {"movn", MOVN_OP, R3_TYPE_INST, 0x0000000b}},               /* MIPS32 */
    {"movn.d", {"movn.d", MOVN_D_OP, FP_MOVC_TYPE_INST, 0x46200013}},    /* MIPS32 */
    {"movn.ps", {"movn.ps", MOVN_PS_OP, FP_MOVC_TYPE_INST, 0x46c00013}}, /* MIPS32 Rev 2 */
    {"movn.s", {"movn.s", MOVN_S_OP, FP_MOVC_TYPE_INST, 0x46000013}},    /* MIPS32 */

    {"movt", {"movt", MOVT_OP, MOVC_TYPE_INST, 0x00010001}},             /* MIPS32 */
    {"movt.d", {"movt.d", MOVT_D_OP, FP_MOVC_TYPE_INST, 0x46210011}},    /* MIPS32 */
    {"movt.ps", {"movt.ps", MOVT_PS_OP, FP_MOVC_TYPE_INST, 0x46c10011}}, /* MIPS32 Rev 2 */
    {"movt.s", {"movt.s", MOVT_S_OP, FP_MOVC_TYPE_INST, 0x46010011}},    /* MIPS32 */

    {"movz", {"movz", MOVZ_OP, R3_TYPE_INST, 0x0000000a}},               /* MIPS32 */
    {"movz.d", {"movz.d", MOVZ_D_OP, FP_MOVC_TYPE_INST, 0x46200012}},    /* MIPS32 */
    {"movz.ps", {"movz.ps", MOVZ_PS_OP, FP_MOVC_TYPE_INST, 0x46c00012}}, /* MIPS32 Rev 2 */
    {"movz.s", {"movz.s", MOVZ_S_OP, FP_MOVC_TYPE_INST, 0x46000012}},    /* MIPS32 */

    {"msub", {"msub", MSUB_OP, R2st_TYPE_INST, 0x70000004}},           /* MIPS32 */
    {"msub.d", {"msub.d", MSUB_D_OP, FP_R4_TYPE_INST, 0x4c000021}},    /* MIPS32 Rev 2 */
    {"msub.ps", {"msub.ps", MSUB_PS_OP, FP_R4_TYPE_INST, 0x4c000026}}, /* MIPS32 Rev 2 */
    {"msub.s", {"msub.s", MSUB_S_OP, FP_R4_TYPE_INST, 0x4c000020}},    /* MIPS32 Rev 2 */
    {"msubu", {"msubu", MSUBU_OP, R2st_TYPE_INST, 0x70000005}},        /* MIPS32 */

    {"mtc0", {"mtc0", MTC0_OP, R2td_TYPE_INST, 0x40800000}},
    {"mtc1", {"mtc1", MTC1_OP, FP_R2ts_TYPE_INST, 0x44800000}},
    {"mtc1.d", {"mtc1.d", MTC1_D_POP, PSEUDO_OP, (unsigned) -1}},
    {"mtc2", {"mtc2", MTC2_OP, R2td_TYPE_INST, 0x48800000}},
    {"mthc1", {"mthc1", MTHC1_OP, FP_R2ts_TYPE_INST, 0x44e00000}}, /* MIPS32 Rev 2 */
    {"mthc2", {"mthc2", MTHC2_OP, R2td_TYPE_INST, 0x48e00000}},    /* MIPS32 Rev 2 */

    {"mthi", {"mthi", MTHI_OP, R1s_TYPE_INST, 0x00000011}},
    {"mtlo", {"mtlo", MTLO_OP, R1s_TYPE_INST, 0x00000013}},

    {"mul", {"mul", MUL_OP, R3_TYPE_INST, 0x70000002}}, /* MIPS32 */
    {"mul.d", {"mul.d", MUL_D_OP, FP_R3_TYPE_INST, 0x46200002}},
    {"mul.ps", {"mul.ps", MUL_PS_OP, FP_R3_TYPE_INST, 0x46c00002}}, /* MIPS32 Rev 2 */
    {"mul.s", {"mul.s", MUL_S_OP, FP_R3_TYPE_INST, 0x46000002}},
    {"mulo", {"mulo", MULO_POP, PSEUDO_OP, (unsigned) -1}},
    {"mulou", {"mulou", MULOU_POP, PSEUDO_OP, (unsigned) -1}},
    {"mult", {"mult", MULT_OP, R2st_TYPE_INST, 0x00000018}},
    {"multu", {"multu", MULTU_OP, R2st_TYPE_INST, 0x00000019}},

    {"neg", {"neg", NEG_POP, PSEUDO_OP, (unsigned) -1}},
    {"neg.d", {"neg.d", NEG_D_OP, FP_R2ds_TYPE_INST, 0x46200007}},
    {"neg.ps", {"neg.ps", NEG_PS_OP, FP_R2ds_TYPE_INST, 0x46c00007}}, /* MIPS32 Rev 2 */
    {"neg.s", {"neg.s", NEG_S_OP, FP_R2ds_TYPE_INST, 0x46000007}},
    {"negu", {"negu", NEGU_POP, PSEUDO_OP, (unsigned) -1}},

    {"nmadd.d", {"nmadd.d", NMADD_D_OP, FP_R4_TYPE_INST, 0x4c000031}},    /* MIPS32 Rev 2 */
    {"nmadd.ps", {"nmadd.ps", NMADD_PS_OP, FP_R4_TYPE_INST, 0x4c000036}}, /* MIPS32 Rev 2 */
    {"nmadd.s", {"nmadd.s", NMADD_S_OP, FP_R4_TYPE_INST, 0x4c000030}},    /* MIPS32 Rev 2 */
    {"nmsub.d", {"nmsub.d", NMSUB_D_OP, FP_R4_TYPE_INST, 0x4c000039}},    /* MIPS32 Rev 2 */
    {"nmsub.ps", {"nmsub.ps", NMSUB_PS_OP, FP_R4_TYPE_INST, 0x4c00003e}}, /* MIPS32 Rev 2 */
    {"nmsub.s", {"nmsub.s", NMSUB_S_OP, FP_R4_TYPE_INST, 0x4c000038}},    /* MIPS32 Rev 2 */

    {"nop", {"nop", NOP_POP, PSEUDO_OP, (unsigned) -1}},
    {"nor", {"nor", NOR_OP, R3_TYPE_INST, 0x00000027}},
    {"not", {"not", NOT_POP, PSEUDO_OP, (unsigned) -1}},
    {"or", {"or", OR_OP, R3_TYPE_INST, 0x00000025}},
    {"ori", {"ori", ORI_OP, I2_TYPE_INST, 0x34000000}},

    {"pll.ps", {"pll.ps", PLL_PS_OP, FP_R3_TYPE_INST, 0x46c0002c}}, /* MIPS32 Rev 2 */
    {"plu.ps", {"plu.ps", PLU_PS_OP, FP_R3_TYPE_INST, 0x46c0002d}}, /* MIPS32 Rev 2 */

    {"pref", {"pref", PREF_OP, I2_TYPE_INST, 0xcc000000}},          /* MIPS32 */
    {"prefx", {"prefx", PREFX_OP, R2st_TYPE_INST, 0x4600000f}},     /* MIPS32 Rev 2 */
    {"pul.ps", {"pul.ps", PUL_PS_OP, FP_R3_TYPE_INST, 0x46c0002e}}, /* MIPS32 Rev 2 */
    {"puu.ps", {"puu.ps", PUU_PS_OP, FP_R3_TYPE_INST, 0x46c0002f}}, /* MIPS32 Rev 2 */

    {"rdhwr", {"rdhwr", RDHWR_OP, R3_TYPE_INST, 0x7c00003b}},      /* MIPS32 Rev 2 */
    {"rdpgpr", {"rdpgpr", RDPGPR_OP, R2td_TYPE_INST, 0x41400000}}, /* MIPS32 Rev 2 */

    {"recip.d", {"recip.d", RECIP_D_OP, FP_R2ds_TYPE_INST, 0x46200015}}, /* MIPS32 Rev 2 */
    {"recip.s", {"recip.s", RECIP_S_OP, FP_R2ds_TYPE_INST, 0x46000015}}, /* MIPS32 Rev 2 */

    {"rem", {"rem", REM_POP, PSEUDO_OP, (unsigned) -1}},
    {"remu", {"remu", REMU_POP, PSEUDO_OP, (unsigned) -1}},

    {"rfe", {"rfe", RFE_OP, NOARG_TYPE_INST, 0x42000010}},

    {"rol", {"rol", ROL_POP, PSEUDO_OP, (unsigned) -1}},
    {"ror", {"ror", ROR_POP, PSEUDO_OP, (unsigned) -1}},
    {"rotr", {"rotr", ROTR_OP, R2sh_TYPE_INST, 0x00200002}},    /* MIPS32 Rev 2 */
    {"rotrv", {"rotrv", ROTRV_OP, R2sh_TYPE_INST, 0x00200003}}, /* MIPS32 Rev 2 */

    {"round.l.d", {"round.l.d", ROUND_L_D_OP, FP_R2ds_TYPE_INST, 0x46200008}}, /* MIPS32 Rev 2 */
    {"round.l.s", {"round.l.s", ROUND_L_S_OP, FP_R2ds_TYPE_INST, 0x46000008}}, /* MIPS32 Rev 2 */
    {"round.w.d", {"round.w.d", ROUND_W_D_OP, FP_R2ds_TYPE_INST, 0x4620000c}}, /* MIPS32 */
    {"round.w.s", {"round.w.s", ROUND_W_S_OP, FP_R2ds_TYPE_INST, 0x4600000c}}, /* MIPS32 */

    {"rsqrt.d", {"rsqrt.d", RSQRT_D_OP, FP_R2ds_TYPE_INST, 0x46200016}}, /* MIPS32 Rev 2 */
    {"rsqrt.s", {"rsqrt.s", RSQRT_S_OP, FP_R2ds_TYPE_INST, 0x46000016}}, /* MIPS32 Rev 2 */

    {"s.d", {"s.d", S_D_POP, PSEUDO_OP, (unsigned) -1}},
    {"s.s", {"s.s", S_S_POP, PSEUDO_OP, (unsigned) -1}},

    {"sb", {"sb", SB_OP, I2a_TYPE_INST, 0xa0000000}},
    {"sc", {"sc", SC_OP, I2a_TYPE_INST, 0xe0000000}}, /* MIPS32 */
    {"sd", {"sd", SD_POP, PSEUDO_OP, (unsigned) -1}},
    {"sdbbp", {"sdbbp", SDBBP_OP, NOARG_TYPE_INST, 0x7000003f}}, /* MIPS32 Rev 2*/
    {"sdc1", {"sdc1", SDC1_OP, FP_I2a_TYPE_INST, 0xf4000000}},   /* MIPS32 */
    {"sdc2", {"sdc2", SDC2_OP, I2a_TYPE_INST, 0xf8000000}},      /* MIPS32 */
    {"sdxc1", {"sdxc1", SDXC1_OP, FP_R3_TYPE_INST, 0x46000009}}, /* MIPS32 Rev 2 */

    {"seb", {"seb", SEB_OP, R2td_TYPE_INST, 0x7c000420}}, /* MIPS32 Rev 2 */
    {"seh", {"seh", SEH_OP, R2td_TYPE_INST, 0x7c000620}}, /* MIPS32 Rev 2 */
    {"seq", {"seq", SEQ_POP, PSEUDO_OP, (unsigned) -1}},
    {"sge", {"sge", SGE_POP, PSEUDO_OP, (unsigned) -1}},
    {"sgeu", {"sgeu", SGEU_POP, PSEUDO_OP, (unsigned) -1}},
    {"sgt", {"sgt", SGT_POP, PSEUDO_OP, (unsigned) -1}},
    {"sgtu", {"sgtu", SGTU_POP, PSEUDO_OP, (unsigned) -1}},

    {"sh", {"sh", SH_OP, I2a_TYPE_INST, 0xa4000000}},
    {"sle", {"sle", SLE_POP, PSEUDO_OP, (unsigned) -1}},
    {"sleu", {"sleu", SLEU_POP, PSEUDO_OP, (unsigned) -1}},
    {"sll", {"sll", SLL_OP, R2sh_TYPE_INST, 0x00000000}},
    {"sllv", {"sllv", SLLV_OP, R3sh_TYPE_INST, 0x00000004}},

    {"slt", {"slt", SLT_OP, R3_TYPE_INST, 0x0000002a}},
    {"slti", {"slti", SLTI_OP, I2_TYPE_INST, 0x28000000}},
    {"sltiu", {"sltiu", SLTIU_OP, I2_TYPE_INST, 0x2c000000}},
    {"sltu", {"sltu", SLTU_OP, R3_TYPE_INST, 0x0000002b}},
    {"sne", {"sne", SNE_POP, PSEUDO_OP, (unsigned) -1}},

    {"sqrt.d", {"sqrt.d", SQRT_D_OP, FP_R2ds_TYPE_INST, 0x46200004}}, /* MIPS32 */
    {"sqrt.s", {"sqrt.s", SQRT_S_OP, FP_R2ds_TYPE_INST, 0x46000004}}, /* MIPS32 */

    {"sra", {"sra", SRA_OP, R2sh_TYPE_INST, 0x00000003}},
    {"srav", {"srav", SRAV_OP, R3sh_TYPE_INST, 0x00000007}},
    {"srl", {"srl", SRL_OP, R2sh_TYPE_INST, 0x00000002}},
    {"srlv", {"srlv", SRLV_OP, R3sh_TYPE_INST, 0x00000006}},

    {"ssnop", {"ssnop", SSNOP_OP, R2sh_TYPE_INST, 0x00000040}}, /* MIPS32 */

    {"sub", {"sub", SUB_OP, R3_TYPE_INST, 0x00000022}},
    {"sub.d", {"sub.d", SUB_D_OP, FP_R3_TYPE_INST, 0x46200001}},
    {"sub.ps", {"sub.ps", SUB_PS_OP, FP_R3_TYPE_INST, 0x46600001}}, /* MIPS32 Rev 2 */
    {"sub.s", {"sub.s", SUB_S_OP, FP_R3_TYPE_INST, 0x46000001}},
    {"subu", {"subu", SUBU_OP, R3_TYPE_INST, 0x00000023}},

    {"suxc1", {"suxc1", SUXC1_OP, FP_R3_TYPE_INST, 0x4600000d}}, /* MIPS32 Rev 2 */

    {"sw", {"sw", SW_OP, I2a_TYPE_INST, 0xac000000}},
    {"swc1", {"swc1", SWC1_OP, FP_I2a_TYPE_INST, 0xe4000000}},
    {"swc2", {"swc2", SWC2_OP, I2a_TYPE_INST, 0xe8000000}},
    {"swl", {"swl", SWL_OP, I2a_TYPE_INST, 0xa8000000}},
    {"swr", {"swr", SWR_OP, I2a_TYPE_INST, 0xb8000000}},
    {"swxc1", {"swxc1", SWXC1_OP, FP_R3_TYPE_INST, 0x46000008}}, /* MIPS32 Rev 2 */

    {"sync", {"sync", SYNC_OP, NOARG_TYPE_INST, 0x0000000f}}, /* MIPS32 */
    {"synci", {"synci", SYNCI_OP, I2_TYPE_INST, 0x04140000}}, /* MIPS32 Rev 2 */
    {"syscall", {"syscall", SYSCALL_OP, NOARG_TYPE_INST, 0x0000000c}},

    {"teq", {"teq", TEQ_OP, R2st_TYPE_INST, 0x00000034}},      /* MIPS32 */
    {"teqi", {"teqi", TEQI_OP, I1s_TYPE_INST, 0x040c0000}},    /* MIPS32 */
    {"tge", {"tge", TGE_OP, R2st_TYPE_INST, 0x00000030}},      /* MIPS32 */
    {"tgei", {"tgei", TGEI_OP, I1s_TYPE_INST, 0x04080000}},    /* MIPS32 */
    {"tgeiu", {"tgeiu", TGEIU_OP, I1s_TYPE_INST, 0x04090000}}, /* MIPS32 */
    {"tgeu", {"tgeu", TGEU_OP, R2st_TYPE_INST, 0x00000031}},   /* MIPS32 */

    {"tlbp", {"tlbp", TLBP_OP, NOARG_TYPE_INST, 0x42000008}},
    {"tlbr", {"tlbr", TLBR_OP, NOARG_TYPE_INST, 0x42000001}},
    {"tlbwi", {"tlbwi", TLBWI_OP, NOARG_TYPE_INST, 0x42000002}},
    {"tlbwr", {"tlbwr", TLBWR_OP, NOARG_TYPE_INST, 0x42000006}},

    {"tlt", {"tlt", TLT_OP, R2st_TYPE_INST, 0x00000032}},      /* MIPS32 */
    {"tlti", {"tlti", TLTI_OP, I1s_TYPE_INST, 0x040a0000}},    /* MIPS32 */
    {"tltiu", {"tltiu", TLTIU_OP, I1s_TYPE_INST, 0x040b0000}}, /* MIPS32 */
    {"tltu", {"tltu", TLTU_OP, R2st_TYPE_INST, 0x00000033}},   /* MIPS32 */
    {"tne", {"tne", TNE_OP, R2st_TYPE_INST, 0x00000036}},      /* MIPS32 */
    {"tnei", {"tnei", TNEI_OP, I1s_TYPE_INST, 0x040e0000}},    /* MIPS32 */

    {"trunc.l.d", {"trunc.l.d", TRUNC_L_D_OP, FP_R2ds_TYPE_INST, 0x46200009}}, /* MIPS32 Rev 2 */
    {"trunc.l.s", {"trunc.l.s", TRUNC_L_S_OP, FP_R2ds_TYPE_INST, 0x46000009}}, /* MIPS32 Rev 2 */
    {"trunc.w.d", {"trunc.w.d", TRUNC_W_D_OP, FP_R2ds_TYPE_INST, 0x4620000d}}, /* MIPS32 */
    {"trunc.w.s", {"trunc.w.s", TRUNC_W_S_OP, FP_R2ds_TYPE_INST, 0x4600000d}}, /* MIPS32 */

    {"ulh", {"ulh", ULH_POP, PSEUDO_OP, (unsigned) -1}},
    {"ulhu", {"ulhu", ULHU_POP, PSEUDO_OP, (unsigned) -1}},
    {"ulw", {"ulw", ULW_POP, PSEUDO_OP, (unsigned) -1}},
    {"ush", {"ush", USH_POP, PSEUDO_OP, (unsigned) -1}},
    {"usw", {"usw", USW_POP, PSEUDO_OP, (unsigned) -1}},

    {"wrpgpr", {"wrpgpr", WRPGPR_OP, R2td_TYPE_INST, 0x41c00000}}, /* MIPS32 Rev 2 */
    {"wsbh", {"wsbh", WSBH_OP, R2td_TYPE_INST, 0x7c0000a0}},       /* MIPS32 Rev 2 */

    {"xor", {"xor", XOR_OP, R3_TYPE_INST, 0x00000026}},
    {"xori", {"xori", XORI_OP, I2_TYPE_INST, 0x38000000}},
};

#endif