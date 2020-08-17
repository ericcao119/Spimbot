#pragma once
#ifndef SPIMBOT_PARSER_COMMENT_H
#define SPIMBOT_PARSER_COMMENT_H
#include "../parser_helpers.h"

namespace mips_parser {

/**
 * Since the MIPS grammar uses \n to indicate end of line our skippers will ignore the ones the end of line
 *
 * The following should be valid:
 * ```
 * add $s0 (block comment) $s0 $s0
 * ```
 */

const auto LINE_COMMENT = (lit("//") | lit("#")) >> *(char_ - eol);
const auto BLOCK_COMMENT = "/*" >> *(char_ - "*/") > "*/";
const auto COMMENT = LINE_COMMENT | BLOCK_COMMENT;
}  // namespace mips_parser

#endif