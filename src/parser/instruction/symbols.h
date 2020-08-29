#pragma once
#ifndef SPIMBOT_PARSER_INSTRUCTION_SYMBOLS_H
#define SPIMBOT_PARSER_INSTRUCTION_SYMBOLS_H

#include <unordered_map>

#include "../parser_helpers.h"
#include "op.h"

namespace mips_parser {

struct InstructionTbl : x3::symbols<int> {
    InstructionTbl() {
        for (const auto& pair : INSTRUCTION_TEMPLATES) {
            this->add(pair.first);
        }
    }
};

const auto instruction_ = InstructionTbl{};

}  // namespace mips_parser

#endif