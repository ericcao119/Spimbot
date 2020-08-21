#include <catch2/catch.hpp>
#include <unordered_map>

#include "../table.h"
#include "../test_parser.h"
#include "parser/expression/expression.h"
#include "parser/expression/expression_lst.h"
#include "parser/skipper.h"

client::ast::evaluator<lookup> eval;

template <typename P>
uint32_t eval_expr(char const* input, P const& p, bool full_match = true) {
    using boost::spirit::x3::phrase_parse;

    client::ast::expression expr;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, mips_parser::default_skipper, expr) && (!full_match || (f == l))) {
        // client::ast::printer print;
        // print(expr);
        // std::cout << std::endl;
        return eval(expr);
    } else {
        throw parse_failed_exception();
    }
}

template <typename P, typename V>
void parse_expression(char const* input, P const& p, V& value, bool full_match = true) {
    using boost::spirit::x3::phrase_parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, mips_parser::default_skipper, value) && (!full_match || (f == l))) {
        return;
    } else {
        throw parse_failed_exception();
    }
}

TEST_CASE("Expression parsing", "[parser][expressions]") {
    using mips_parser::expression;

    SECTION("Constants") {
        REQUIRE(eval_expr("1", expression) == 1);
        REQUIRE(eval_expr("20", expression) == 20);
        REQUIRE(eval_expr("a", expression) == 1);
        REQUIRE(eval_expr("a0", expression) == 3);
        REQUIRE(eval_expr("deadbeef", expression) == 0xdeadbeef);
        REQUIRE(test_parser("asda", expression));
    }

    SECTION("Unary Operators") {
        REQUIRE(eval_expr("- 0b111110000", expression) == (uint32_t)-496);
        REQUIRE(eval_expr("-0xff0000", expression) == (uint32_t)-16711680);

        // TOO MANY DIGITS!
        REQUIRE_THROWS(eval_expr("-0xfffffffff", expression) == (uint32_t)-16711680);
    }

    SECTION("Mul/Div Operators") {
        REQUIRE(eval_expr("1*2", expression) == (uint32_t)2);
        REQUIRE(eval_expr("1 / 2", expression) == (uint32_t)0);
        REQUIRE(eval_expr("1 / 2 * 2", expression) == (uint32_t)0);
        REQUIRE(eval_expr("1 * 2 / 2", expression) == (uint32_t)1);
        REQUIRE(eval_expr("1 * 2 / -2", expression) == (uint32_t)-1);
        REQUIRE(eval_expr("-1 * 2 / -2", expression) == (uint32_t)1);
        REQUIRE(eval_expr("-1 * -2 / -2", expression) == (uint32_t)-1);

        REQUIRE_THROWS(eval_expr("2 ** -1", expression));
    }

    SECTION("Add/Sub Operators") {
        REQUIRE(eval_expr("1+2", expression) == (uint32_t)3);
        REQUIRE(eval_expr("1 - 2", expression) == (uint32_t)-1);
    }

    SECTION("Add/Sub Operators") {
        REQUIRE(eval_expr("1+2", expression) == (uint32_t)3);
        REQUIRE(eval_expr("1 - 2", expression) == (uint32_t)-1);
    }

    SECTION("Add/Sub Operators") {
        REQUIRE(eval_expr("1+2", expression) == (uint32_t)3);
        REQUIRE(eval_expr("1 - 2", expression) == (uint32_t)-1);
    }

    SECTION("Or Operators") {
        REQUIRE(eval_expr("1 | 2", expression) == (uint32_t)3);
        REQUIRE(eval_expr("1 | deadbeef", expression) == (uint32_t)(1 | 0xdeadbeef));
        REQUIRE(eval_expr("-1 | 2", expression) == (uint32_t)-1);
        REQUIRE(eval_expr("2 | -1", expression) == (uint32_t)-1);

        REQUIRE_THROWS(eval_expr("2 || -1", expression));
    }

    SECTION("ANd Operators") {
        REQUIRE(eval_expr("1 & 2", expression) == (uint32_t)0);
        REQUIRE(eval_expr("1 & deadbeef", expression) == (uint32_t)(1 & 0xdeadbeef));
        REQUIRE(eval_expr("-1 & 2", expression) == (uint32_t)2);
        REQUIRE(eval_expr("2 & -1", expression) == (uint32_t)2);

        REQUIRE_THROWS(eval_expr("2 && -1", expression));
    }

    SECTION("Parentheses") {
        REQUIRE(eval_expr("(1 & 2) | 3", expression) == (uint32_t)3);
        REQUIRE(eval_expr("(0b10101 | 0b1010) + (5 * 5)", expression) == (uint32_t)56);
        REQUIRE(eval_expr("(0b10101 | 0b1010) & (5 * 5)", expression) == (uint32_t)25);
        REQUIRE(eval_expr("-(-1)", expression) == (uint32_t)1);

        REQUIRE_THROWS(eval_expr("(0b10101 | 0b1010", expression));
    }

    SECTION("Order of Operations") {
        REQUIRE(eval_expr("1 & 2 * 3 | +4 + ~5 - 1", expression) == (uint32_t)-3);
        REQUIRE(eval_expr("(1 & (2 * 3 | +4) + ~5) - 1", expression) == (uint32_t)-1);
    }

    SECTION("Literal List") {
        client::ast::LiteralLst<client::ast::expression> expr_lst;
        parse_expression("0 1 2 3", mips_parser::LITERAL_EXPR_LST, expr_lst);

        REQUIRE(expr_lst.size() == 4);
        REQUIRE(eval(expr_lst[0]) == 0);
        REQUIRE(eval(expr_lst[1]) == 1);
        REQUIRE(eval(expr_lst[2]) == 2);
        REQUIRE(eval(expr_lst[3]) == 3);
    }

    SECTION("Repeat List") {
        client::ast::RepeatLst<client::ast::expression> expr_lst;
        parse_expression("1 : 10", mips_parser::REPEAT_EXPR, expr_lst);

        REQUIRE(eval(expr_lst.repeat_num) == 10);
        REQUIRE(eval(expr_lst.repeat_value) == 1);
    }
}

// int main() {

//     parse_string("0b111110000");
//     parse_string("-s&r|0xff0000*0b111110000");
//     parse_string("-s*s");
//     parse_string("s&r|r+s");

//     // Fail:
//     try {
//         parse_string("--s");
//     } catch (...) {
//     }
// }