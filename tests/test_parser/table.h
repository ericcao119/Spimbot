#ifndef TEST_PARSER_TABLE_H
#define TEST_PARSER_TABLE_H

#include <unordered_map>
#include <string>

const std::unordered_map<std::string, uint32_t> variable_table = {
    {"a", 1},
    {"ab", 2},
    {"a0", 3},

    // hex strings
    {"deadbeef", 0xdeadbeef},
    {"deadc0de", 0xdeadc0de},
    {"ffffffff", 0xffffffff},

    // Small ints
    {"one", 1},
    {"two", 2},
    {"three", 3},
};

inline uint32_t lookup(const std::string& str) { return variable_table.at(str); }

#endif