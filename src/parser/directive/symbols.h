#pragma once
#ifndef SPIMBOT_PARSER_DIRECTIVE_SYMBOLS_H
#define SPIMBOT_PARSER_DIRECTIVE_SYMBOLS_H

#include <string>

#include "../parser_helpers.h"

namespace mips_parser {
using namespace std::literals;

/* Directives */
const auto ALIAS_DIR = ".alias"s;
const auto ALIGN_DIR = ".align"s;
const auto ASCII_DIR = ".ascii"s;
const auto ASCIIZ_DIR = ".asciiz"s;
const auto ASM0_DIR = ".asm0"s;
const auto BGNB_DIR = ".bgnb"s;
const auto BYTE_DIR = ".byte"s;
const auto COMM_DIR = ".comm"s;
const auto DATA_DIR = ".data"s;
const auto KDATA_DIR = ".kdata"s;
const auto DOUBLE_DIR = ".double"s;
const auto END_DIR = ".end"s;
const auto ENDB_DIR = ".endb"s;
const auto ENDR_DIR = ".endr"s;
const auto ENT_DIR = ".ent"s;
const auto ERR_DIR = ".err"s;
const auto EXTERN_DIR = ".extern"s;
const auto FILE_DIR = ".file"s;
const auto FLOAT_DIR = ".float"s;
const auto FMASK_DIR = ".fmask"s;
const auto FRAME_DIR = ".frame"s;
const auto GLOBAL_DIR = ".globl"s;
const auto HALF_DIR = ".half"s;
const auto LABEL_DIR = ".lab"s;
const auto LCOMM_DIR = ".lcomm"s;
const auto LIVEREG_DIR = ".livereg"s;
const auto LOC_DIR = ".loc"s;
const auto MASK_DIR = ".mask"s;
const auto NOALIAS_DIR = ".noalias"s;
const auto OPTIONS_DIR = ".option"s;
const auto RDATA_DIR = ".rdata"s;
const auto REPEAT_DIR = ".repeat"s;
const auto SDATA_DIR = ".sdata"s;
const auto SET_DIR = ".set"s;
const auto SPACE_DIR = ".space"s;
const auto STRUCT_DIR = ".struct"s;
const auto TEXT_DIR = ".text"s;
const auto KTEXT_DIR = ".ktext"s;
const auto VERSTAMP_DIR = ".verstamp"s;
const auto VREG_DIR = ".vreg"s;
const auto WORD_DIR = ".word"s;


/* Keywords */
const auto ALIAS_DIR_KW = make_keyword(ALIAS_DIR);
const auto ALIGN_DIR_KW = make_keyword(ALIGN_DIR);
const auto ASCII_DIR_KW = make_keyword(ASCII_DIR);
const auto ASCIIZ_DIR_KW = make_keyword(ASCIIZ_DIR);
const auto ASM0_DIR_KW = make_keyword(ASM0_DIR);
const auto BGNB_DIR_KW = make_keyword(BGNB_DIR);
const auto BYTE_DIR_KW = make_keyword(BYTE_DIR);
const auto COMM_DIR_KW = make_keyword(COMM_DIR);
const auto DATA_DIR_KW = make_keyword(DATA_DIR);
const auto KDATA_DIR_KW = make_keyword(KDATA_DIR);
const auto DOUBLE_DIR_KW = make_keyword(DOUBLE_DIR);
const auto END_DIR_KW = make_keyword(END_DIR);
const auto ENDB_DIR_KW = make_keyword(ENDB_DIR);
const auto ENDR_DIR_KW = make_keyword(ENDR_DIR);
const auto ENT_DIR_KW = make_keyword(ENT_DIR);
const auto ERR_DIR_KW = make_keyword(ERR_DIR);
const auto EXTERN_DIR_KW = make_keyword(EXTERN_DIR);
const auto FILE_DIR_KW = make_keyword(FILE_DIR);
const auto FLOAT_DIR_KW = make_keyword(FLOAT_DIR);
const auto FMASK_DIR_KW = make_keyword(FMASK_DIR);
const auto FRAME_DIR_KW = make_keyword(FRAME_DIR);
const auto GLOBAL_DIR_KW = make_keyword(GLOBAL_DIR);
const auto HALF_DIR_KW = make_keyword(HALF_DIR);
const auto LABEL_DIR_KW = make_keyword(LABEL_DIR);
const auto LCOMM_DIR_KW = make_keyword(LCOMM_DIR);
const auto LIVEREG_DIR_KW = make_keyword(LIVEREG_DIR);
const auto LOC_DIR_KW = make_keyword(LOC_DIR);
const auto MASK_DIR_KW = make_keyword(MASK_DIR);
const auto NOALIAS_DIR_KW = make_keyword(NOALIAS_DIR);
const auto OPTIONS_DIR_KW = make_keyword(OPTIONS_DIR);
const auto RDATA_DIR_KW = make_keyword(RDATA_DIR);
const auto REPEAT_DIR_KW = make_keyword(REPEAT_DIR);
const auto SDATA_DIR_KW = make_keyword(SDATA_DIR);
const auto SET_DIR_KW = make_keyword(SET_DIR);
const auto SPACE_DIR_KW = make_keyword(SPACE_DIR);
const auto STRUCT_DIR_KW = make_keyword(STRUCT_DIR);
const auto TEXT_DIR_KW = make_keyword(TEXT_DIR);
const auto KTEXT_DIR_KW = make_keyword(KTEXT_DIR);
const auto VERSTAMP_DIR_KW = make_keyword(VERSTAMP_DIR);
const auto VREG_DIR_KW = make_keyword(VREG_DIR);
const auto WORD_DIR_KW = make_keyword(WORD_DIR);

/**
 * Table used for collecting keywords. Potential use in the future, but defer to the constants above
 */
struct DirectiveTbl : x3::symbols<int> {
    DirectiveTbl() {
        *this += ALIAS_DIR, ALIGN_DIR, ASCII_DIR, ASCIIZ_DIR, ASM0_DIR, BGNB_DIR, BYTE_DIR, COMM_DIR, DATA_DIR,
            DOUBLE_DIR, END_DIR, ENDB_DIR, ENDR_DIR, ENT_DIR, ERR_DIR, EXTERN_DIR, FILE_DIR, FLOAT_DIR, FMASK_DIR,
            FRAME_DIR, GLOBAL_DIR, HALF_DIR, KDATA_DIR, KTEXT_DIR, LABEL_DIR, LCOMM_DIR, LIVEREG_DIR, LOC_DIR, MASK_DIR,
            NOALIAS_DIR, OPTIONS_DIR, RDATA_DIR, REPEAT_DIR, SDATA_DIR, SET_DIR, SPACE_DIR, STRUCT_DIR, TEXT_DIR,
            VERSTAMP_DIR, VREG_DIR, WORD_DIR;

        // XXX: Add macro support T.T
    }
};

const auto directive_ = DirectiveTbl{};

}  // namespace mips_parser
#endif