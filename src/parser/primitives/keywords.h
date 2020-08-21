#pragma once
#ifndef SPIMBOT_PARSER_PRIMITIVES_KEYWORDS_H
#define SPIMBOT_PARSER_PRIMITIVES_KEYWORDS_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../directive/symbols.h"
#include "../parser_helpers.h"
#include "instruction.h"
#include "register.h"

namespace mips_parser {
namespace x3 = boost::spirit::x3;

/* Categories of keywords */

/**
 * All keywords for non-bare machine
 */
const auto DEFAULT_KEYWORDS = [=]() {
    symbols<int> sym;
    const auto insert_sym = [&](auto&& symbol, auto&& data) { sym.add(symbol, data); };

    directive_.for_each(insert_sym);
    instruction_.for_each(insert_sym);
    register_.for_each(insert_sym);
    fp_register_.for_each(insert_sym);

    return sym;
}();

/**
 * All keywords for bare machine
 */
const auto BARE_MACHINE_KEYWORDS = [=]() {
    symbols<int> sym;
    sym = DEFAULT_KEYWORDS;
    // XXX: Remove psuedo ops
    return sym;
}();

const auto FP_REG = to_keyword(fp_register_);
const auto REG = to_keyword(register_);

const auto RESERVED = lexeme[DEFAULT_KEYWORDS >> !ident_];
const auto BARE_MACHINE_RESERVED = lexeme[DEFAULT_KEYWORDS >> !ident_];

const auto IDENT = as<std::string>(lexeme[(first_ident_ >> *ident_) - (RESERVED | x3::eol | x3::eoi)]);
const auto BARE_MACHINE_IDENT =
    as<std::string>(lexeme[(first_ident_ >> *ident_) - (BARE_MACHINE_RESERVED | x3::eol | x3::eoi)]);

// const auto IDENT = lexeme[+ident_ - RESERVED];
// const auto BARE_MACHINE_IDENT = lexeme[+ident_ - BARE_MACHINE_RESERVED];

}  // namespace mips_parser

#endif