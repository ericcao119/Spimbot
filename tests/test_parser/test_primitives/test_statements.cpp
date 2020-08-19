#include "test_parser.h"

#include "statement.h"

using mips_parser::LINE;

int main() {
    /* Line comment */
    test_parser("abc:", LINE);
    test_parser("$abcd:", LINE);
    test_parser("$L0:", LINE);
    test_parser("$a0:", LINE);
}