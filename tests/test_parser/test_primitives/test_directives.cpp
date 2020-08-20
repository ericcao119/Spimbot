#include <catch2/catch.hpp>
#include <iostream>

#include "../table.h"
#include "../test_parser.h"
#include "parser/primitives/directive.h"
#include "parser/skipper.h"

using namespace client::ast;

template <typename P>
void parse_dir(char const* input, P const& p, Directive& dir, bool full_match = true) {
    using boost::spirit::x3::phrase_parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, mips_parser::default_skipper, dir) && (!full_match || (f == l))) {
        return;
    } else {
        throw parse_failed_exception();
    }
}

TEST_CASE("Parse directives", "[parser][directive]") {
    Directive directive;
    evaluator<lookup> eval;

    SECTION("ALIAS") {
        parse_dir(".alias $0 $1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AliasDir>(directive).reg1 == 0);
        REQUIRE(boost::get<AliasDir>(directive).reg2 == 1);

        parse_dir(".alias $ra $zero", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AliasDir>(directive).reg1 == 31);
        REQUIRE(boost::get<AliasDir>(directive).reg2 == 0);

        REQUIRE_THROWS(parse_dir(".alias $0$1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".alias$0 $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".alias$0$1", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ALIGN") {
        parse_dir(".align 2", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<AlignDir>(directive).alignment) == 2);

        parse_dir(".align 3 + 4 + 9", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<AlignDir>(directive).alignment) == 16);

        parse_dir(".align 3 + 4 + deadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<AlignDir>(directive).alignment) == 7 + 0xdeadbeef);

        REQUIRE_THROWS(parse_dir(".align 3 + 4 9", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".align 3 + 4 deadbeef", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ASCII") {
        parse_dir(".ascii \"2\"", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AsciiDir>(directive).size() == 1);
        REQUIRE(boost::get<AsciiDir>(directive)[0] == "2");

        parse_dir(".ascii \"2\", \"abc\"", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AsciiDir>(directive).size() == 2);
        REQUIRE(boost::get<AsciiDir>(directive)[0] == "2");
        REQUIRE(boost::get<AsciiDir>(directive)[1] == "abc");

        REQUIRE_THROWS(parse_dir(".ascii", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ascii abc", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ascii \"abc ", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ascii 2", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ASCIIZ") {
        parse_dir(".asciiz \"2\"", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AsciizDir>(directive).size() == 1);
        REQUIRE(boost::get<AsciizDir>(directive)[0] == "2");

        parse_dir(".asciiz \"2\", \"abc\"", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<AsciizDir>(directive).size() == 2);
        REQUIRE(boost::get<AsciizDir>(directive)[0] == "2");
        REQUIRE(boost::get<AsciizDir>(directive)[1] == "abc");

        REQUIRE_THROWS(parse_dir(".asciiz", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".asciiz abc", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".asciiz \"abc ", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".asciiz 2", mips_parser::ASM_DIRECTIVE, directive));
    }
}
