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

    SECTION("ASM0") {
        parse_dir(".asm0", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE_NOTHROW(boost::get<Asm0Dir>(directive));

        REQUIRE_THROWS(parse_dir(".asm0 abc", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("BGNB") {
        parse_dir(".bgnb 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<BgnbDir>(directive).symno == 123);
        
        REQUIRE_THROWS(parse_dir(".bgnb", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".bgnb abc", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("BYTE") {
        // parse_dir(".byte 123", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 1);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123);

        // parse_dir(".byte 123 123", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 2);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[1]) == 123);

        // parse_dir(".byte 123, 123", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 2);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[1]) == 123);
        
        // parse_dir(".byte 123, 123, 124", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 3);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[1]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[2]) == 124);

        // parse_dir(".byte 123, 123 124", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 3);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[1]) == 123);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[2]) == 124);
        
        // parse_dir(".byte 123 + a0 123 + deadbeef 124 + 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive)).size()) == 3);
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[0]) == 123 + variable_table.at("a0"));
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[1]) == 123 + variable_table.at("deadbeef"));
        // REQUIRE(eval(boost::get<std::vector<expression>>(boost::get<ByteDir>(directive))[2]) == 124 + 0xdeadbeef);

        // REQUIRE_THROWS(parse_dir(".byte", mips_parser::ASM_DIRECTIVE, directive));
        // REQUIRE_THROWS(parse_dir(".byte ,", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("COMM") {
        parse_dir(".comm ncasl, 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<CommDir>(directive).ident == "ncasl");
        REQUIRE(eval(boost::get<CommDir>(directive).expr) == 123);

        REQUIRE_THROWS(parse_dir(".comm ncasl 123", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".comm", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".comm ,", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("DATA") {
        parse_dir(".data", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DataDir>(directive).addr.has_value() == false);

        parse_dir(".data 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DataDir>(directive).addr.has_value());
        REQUIRE(boost::get<DataDir>(directive).addr.value() == 10);

        parse_dir(".data 0x1f", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DataDir>(directive).addr.has_value());
        REQUIRE(boost::get<DataDir>(directive).addr.value() == 0x1f);
        
        parse_dir(".data 0b10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DataDir>(directive).addr.has_value());
        REQUIRE(boost::get<DataDir>(directive).addr.value() == 0b10);

        REQUIRE_THROWS(parse_dir(".data ab", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".data 0b10, as", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("KDATA") {
        parse_dir(".kdata", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KDataDir>(directive).addr.has_value() == false);

        parse_dir(".kdata 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KDataDir>(directive).addr.has_value());
        REQUIRE(boost::get<KDataDir>(directive).addr.value() == 10);

        parse_dir(".kdata 0x1f", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KDataDir>(directive).addr.has_value());
        REQUIRE(boost::get<KDataDir>(directive).addr.value() == 0x1f);
        
        parse_dir(".kdata 0b10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KDataDir>(directive).addr.has_value());
        REQUIRE(boost::get<KDataDir>(directive).addr.value() == 0b10);

        REQUIRE_THROWS(parse_dir(".kdata ab", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".kdata 0b10, as", mips_parser::ASM_DIRECTIVE, directive));
    }
}
