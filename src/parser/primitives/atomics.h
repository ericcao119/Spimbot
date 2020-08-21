#include "../expression/expression.h"
#include "../parser_helpers.h"
#include "keywords.h"

namespace mips_parser {

const auto ABS_ADDR = int_ | int_ >> "+" >> int_ | int_ >> "-" >> int_;

const auto IMM32 = ABS_ADDR | x3::eps >> "(" >> ABS_ADDR >> ")" >> ">>" >> int_ | IDENT | IDENT >> "+" >> ABS_ADDR |
                   IDENT >> "-" >> ABS_ADDR;

const auto ADDR = x3::eps >> "(" >> REG >> ")" | ABS_ADDR | ABS_ADDR >> '(' >> REG >> ")" |
                  IDENT >> '(' >> REG >> ")" | IDENT >> '+' >> ABS_ADDR | ABS_ADDR >> '+' >> IDENT |
                  IDENT >> '-' >> ABS_ADDR | IDENT >> '+' >> ABS_ADDR >> '(' >> REG >> ')' |
                  IDENT >> '-' >> ABS_ADDR >> '(' >> REG >> ')' | IDENT;

const auto BR_IMM32 = IMM32;

// TODO: Needs boundary check
const auto IMM16 = IMM32;

// TODO: Needs boudnary check
const auto UIMM16 = IMM32;

const auto SRC1 = REG;
const auto SRC2 = REG;
const auto DEST = REG;

const auto F_DEST = FP_REG;
const auto F_SRC1 = FP_REG;
const auto F_SRC2 = FP_REG;

const auto COP_REG = REG | FP_REG;

// XXX: Force this to be between 0 and 7 inclusive
const auto CC_REG = REG;

const auto LABEL = IDENT;

const auto STR = QUOTE_STRING | QUOTE_STRING >> ":" >> int_ | IDENT;

// TODO: Check these for parity with old MIPS grammar
const auto STR_LST = STR % (x3::blank | ",");

const auto OPTIONAL_ID = -IDENT;
}  // namespace mips_parser