#pragma once
#ifndef SPIMBOT_PARSER_DIRECTIVE_SYMBOLS_H
#define SPIMBOT_PARSER_DIRECTIVE_SYMBOLS_H

#include <string>
#include <vector>

#include "../expression/ast.h"
#include "../parser_helpers.h"
#include "directive_ast.h"

namespace mips_parser {

/**
 * Table used for collecting keywords. Potential use in the future, but defer to the constants above
 */
struct DirectiveTbl : x3::symbols<int> {
    DirectiveTbl() {
        *this += ALIAS_DIR, ALIGN_DIR, ASCII_DIR, ASCIIZ_DIR, ASM0_DIR, BGNB_DIR, BYTE_DIR, COMM_DIR, DATA_DIR,
            DOUBLE_DIR, END_DIR, ENDB_DIR, ENDR_DIR, ENT_DIR, ERR_DIR, EXTERN_DIR, FILE_DIR, FLOAT_DIR, FMASK_DIR,
            FRAME_DIR, GLOBAL_DIR, HALF_DIR, KDATA_DIR, KTEXT_DIR, LABEL_DIR, LCOMM_DIR, LIVEREG_DIR, LOC_DIR,
            MASK_DIR, NOALIAS_DIR, OPTIONS_DIR, RDATA_DIR, REPEAT_DIR, SDATA_DIR, SET_DIR, SPACE_DIR, STRUCT_DIR,
            TEXT_DIR, VERSTAMP_DIR, VREG_DIR, WORD_DIR;

        // XXX: Add macro support T.T
    }
};

const auto directive_ = DirectiveTbl{};

}  // namespace mips_parser
#endif