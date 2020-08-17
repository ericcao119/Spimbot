#pragma once
#include "primitives/comment.h"

namespace mips_parser {

/**
 * Since the MIPS grammar uses \n to indicate end of line our skippers will ignore the ones the end of line
 *
 * The following should be valid:
 * ```
 * add $s0 (block comment) $s0 $s0
 * ```
 */
const auto default_skipper = COMMENT | (x3::blank);

}  // namespace mips_parser