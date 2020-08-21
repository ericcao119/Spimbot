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
        std::cout << std::string(f, l) << std::endl;
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
        parse_dir(".byte 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 1);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123);

        parse_dir(".byte 123 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[1]) == 123);

        parse_dir(".byte 123, 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[1]) == 123);

        parse_dir(".byte 123, 123, 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[2]) == 124);

        parse_dir(".byte 123, 123 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[2]) == 124);

        parse_dir(".byte 123 + a0 123 + deadbeef 124 + 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[0]) == 123 + variable_table.at("a0"));
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[1]) == 123 + variable_table.at("deadbeef"));
        REQUIRE(eval(boost::get<ByteDir>(directive).expression_list[2]) == 124 + 0xdeadbeef);

        parse_dir(".byte 123: 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteRepeatDir>(directive).repeat_list.repeat_value) == 123);
        REQUIRE(eval(boost::get<ByteRepeatDir>(directive).repeat_list.repeat_num) == 124);

        parse_dir(".byte 123 + 1:124 + 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<ByteRepeatDir>(directive).repeat_list.repeat_value) == 124);
        REQUIRE(eval(boost::get<ByteRepeatDir>(directive).repeat_list.repeat_num) == 125);

        REQUIRE_THROWS(parse_dir(".byte", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".byte ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".byte :", mips_parser::ASM_DIRECTIVE, directive));
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

    SECTION("DOUBLE") {
        parse_dir(".double 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list.size() == 1);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[0] == 123);

        parse_dir(".double 123 124.1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list.size() == 2);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[0] == 123);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[1] == 124.1);

        parse_dir(".double 123, 1.5", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list.size() == 2);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[0] == 123);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[1] == 1.5);

        parse_dir(".double 1.23, 1.23, 1.24", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list.size() == 3);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[0] == 1.23);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[1] == 1.23);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[2] == 1.24);

        parse_dir(".double 1.23, 1.23 1.24", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list.size() == 3);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[0] == 1.23);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[1] == 1.23);
        REQUIRE(boost::get<DoubleDir>(directive).expression_list[2] == 1.24);

        parse_dir(".double 1e-5: 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleRepeatDir>(directive).repeat_list.repeat_value == 1e-5);
        REQUIRE(eval(boost::get<DoubleRepeatDir>(directive).repeat_list.repeat_num) == 124);

        parse_dir(".double 0.5:124 + 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<DoubleRepeatDir>(directive).repeat_list.repeat_value == 0.5);
        REQUIRE(eval(boost::get<DoubleRepeatDir>(directive).repeat_list.repeat_num) == 125);

        REQUIRE_THROWS(parse_dir(".double", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".double ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".double :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("END") {
        parse_dir(".end symno", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<EndDir>(directive).proc_name.has_value());
        REQUIRE(boost::get<EndDir>(directive).proc_name.value() == "symno");

        parse_dir(".end", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<EndDir>(directive).proc_name.has_value() == false);

        REQUIRE_THROWS(parse_dir(".end 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".end $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ENDB") {
        parse_dir(".endb 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<EndbDir>(directive).symno == 1);

        REQUIRE_THROWS(parse_dir(".endb", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".endb abs", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".endb $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ENDR") {
        parse_dir(".endr", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE_NOTHROW(boost::get<EndrDir>(directive));

        REQUIRE_THROWS(parse_dir(".endr 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".endr abs", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".endr $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ENT") {
        parse_dir(".ent proc_name 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<EntDir>(directive).proc_name == "proc_name");
        REQUIRE(boost::get<EntDir>(directive).lex_level.has_value());
        REQUIRE(boost::get<EntDir>(directive).lex_level.value() == 1);

        parse_dir(".ent proc_name", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<EntDir>(directive).proc_name == "proc_name");
        REQUIRE(boost::get<EntDir>(directive).lex_level.has_value() == false);

        REQUIRE_THROWS(parse_dir(".ent 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ent $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("EXTERN") {
        parse_dir(".extern proc_name 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<ExternDir>(directive).name == "proc_name");
        REQUIRE(boost::get<ExternDir>(directive).number.has_value());
        REQUIRE(boost::get<ExternDir>(directive).number.value() == 1);

        parse_dir(".extern proc_name", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<ExternDir>(directive).name == "proc_name");
        REQUIRE(boost::get<ExternDir>(directive).number.has_value() == false);

        REQUIRE_THROWS(parse_dir(".extern 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".extern $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("ERR") {
        parse_dir(".err", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE_NOTHROW(boost::get<ErrDir>(directive));

        REQUIRE_THROWS(parse_dir(".err 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".err abs", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".err $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("FILE") {
        parse_dir(R"(.file 123 "abc\t")", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FileDir>(directive).file_no == 123);
        REQUIRE(boost::get<FileDir>(directive).filename == "abc\t");

        REQUIRE_THROWS(parse_dir(".file 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".file abs", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".file $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("FLOAT") {
        parse_dir(".float 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatDir>(directive).expression_list.size() == 1);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[0] == 123);

        parse_dir(".float 123 124.1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatDir>(directive).expression_list.size() == 2);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[0] == 123);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[1] == 124.1);

        parse_dir(".float 123, 1.5", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatDir>(directive).expression_list.size() == 2);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[0] == 123);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[1] == 1.5);

        parse_dir(".float 1.23, 1.23, 1.24", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatDir>(directive).expression_list.size() == 3);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[0] == 1.23);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[1] == 1.23);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[2] == 1.24);

        parse_dir(".float 1.23, 1.23 1.24", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatDir>(directive).expression_list.size() == 3);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[0] == 1.23);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[1] == 1.23);
        REQUIRE(boost::get<FloatDir>(directive).expression_list[2] == 1.24);

        parse_dir(".float 1e-5: 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatRepeatDir>(directive).repeat_list.repeat_value == 1e-5);
        REQUIRE(eval(boost::get<FloatRepeatDir>(directive).repeat_list.repeat_num) == 124);

        parse_dir(".float 0.5:124 + 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FloatRepeatDir>(directive).repeat_list.repeat_value == 0.5);
        REQUIRE(eval(boost::get<FloatRepeatDir>(directive).repeat_list.repeat_num) == 125);

        REQUIRE_THROWS(parse_dir(".float", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".float ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".float :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("FMASK") {
        parse_dir(".fmask 12 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FmaskDir>(directive).mask == 12);
        REQUIRE(boost::get<FmaskDir>(directive).offset == 123);

        parse_dir(".fmask 0x12 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FmaskDir>(directive).mask == 0x12);
        REQUIRE(boost::get<FmaskDir>(directive).offset == 123);

        parse_dir(".fmask 0b10 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FmaskDir>(directive).mask == 0b10);
        REQUIRE(boost::get<FmaskDir>(directive).offset == 123);

        REQUIRE_THROWS(parse_dir(".fmask", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".fmask 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".fmask ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".fmask :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("FRAME") {
        parse_dir(".frame $0 123 $ra", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<FrameDir>(directive).frame_register == 0);
        REQUIRE(boost::get<FrameDir>(directive).frame_size == 123);
        REQUIRE(boost::get<FrameDir>(directive).return_pc_register == 31);

        REQUIRE_THROWS(parse_dir(".frame", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".frame 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".frame 1 1 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".frame ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".frame :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("GLOBAL") {
        parse_dir(".globl proc_name", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<GlobalDir>(directive).id == "proc_name");

        REQUIRE_THROWS(parse_dir(".globl", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".globl 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".globl $a0", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("HALF") {
        parse_dir(".half 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 1);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123);

        parse_dir(".half 123 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[1]) == 123);

        parse_dir(".half 123, 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[1]) == 123);

        parse_dir(".half 123, 123, 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[2]) == 124);

        parse_dir(".half 123, 123 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[2]) == 124);

        parse_dir(".half 123 + a0 123 + deadbeef 124 + 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[0]) == 123 + variable_table.at("a0"));
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[1]) == 123 + variable_table.at("deadbeef"));
        REQUIRE(eval(boost::get<HalfDir>(directive).expression_list[2]) == 124 + 0xdeadbeef);

        parse_dir(".half 123: 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfRepeatDir>(directive).repeat_list.repeat_value) == 123);
        REQUIRE(eval(boost::get<HalfRepeatDir>(directive).repeat_list.repeat_num) == 124);

        parse_dir(".half 123 + 1:124 + 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<HalfRepeatDir>(directive).repeat_list.repeat_value) == 124);
        REQUIRE(eval(boost::get<HalfRepeatDir>(directive).repeat_list.repeat_num) == 125);

        REQUIRE_THROWS(parse_dir(".half", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".half ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".half :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("LABEL") {
        parse_dir(".lab label", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<LabelDir>(directive).label_name == "label");

        REQUIRE_THROWS(parse_dir(".lab", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lab 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lab ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lab :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("LCOMM") {
        parse_dir(".lcomm name 1 + 2 * 3", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<LcommDir>(directive).name == "name");
        REQUIRE(eval(boost::get<LcommDir>(directive).expr) == 7);

        REQUIRE_THROWS(parse_dir(".lcomm", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lcomm name", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lcomm 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lcomm ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".lcomm :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("LIVEREG") {
        parse_dir(".livereg 0x1000 0b10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<LiveregDir>(directive).int_bitmask == 0x1000);
        REQUIRE(boost::get<LiveregDir>(directive).fp_bitmask == 0b10);
        
        parse_dir(".livereg 1000 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<LiveregDir>(directive).int_bitmask == 1000);
        REQUIRE(boost::get<LiveregDir>(directive).fp_bitmask == 10);

        REQUIRE_THROWS(parse_dir(".livereg", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".livereg name", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".livereg 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".livereg ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".livereg :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("LOC") {
        parse_dir(".loc 1000 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<LocDir>(directive).file_number == 1000);
        REQUIRE(boost::get<LocDir>(directive).line_number == 10);

        REQUIRE_THROWS(parse_dir(".loc 0x1000 0b10", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".loc", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".loc name", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".loc 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".loc ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".loc :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("MASK") {
        parse_dir(".mask 1000 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<MaskDir>(directive).mask == 1000);
        REQUIRE(boost::get<MaskDir>(directive).offset == 10);

        parse_dir(".mask 0x1000 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<MaskDir>(directive).mask == 0x1000);
        REQUIRE(boost::get<MaskDir>(directive).offset == 10);

        parse_dir(".mask 0b1000 10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<MaskDir>(directive).mask == 0b1000);
        REQUIRE(boost::get<MaskDir>(directive).offset == 10);
        
        REQUIRE_THROWS(parse_dir(".mask 1000 0x10", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask 0x1000 0b10", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask name", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".mask :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("NOALIAS") {
        parse_dir(".noalias $0 $ra", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<NoaliasDir>(directive).reg1 == 0);
        REQUIRE(boost::get<NoaliasDir>(directive).reg2 == 31);
        
        REQUIRE_THROWS(parse_dir(".noalias $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".noalias", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".noalias name", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".noalias 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".noalias ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".noalias :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("OPTIONS") {
        parse_dir(".option noat", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<OptionDir>(directive).option == "noat");
        
        REQUIRE_THROWS(parse_dir(".option $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".option", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".option 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".option ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".option :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("REPEAT") {
        parse_dir(".repeat deadbeef + 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<RepeatDir>(directive).repeat_num) == 0xdeadbeef + 123);
        parse_dir(".repeat deadbeef+123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<RepeatDir>(directive).repeat_num) == 0xdeadbeef + 123);
        
        REQUIRE_THROWS(parse_dir(".repeat $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".repeat", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".repeat ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".repeat :", mips_parser::ASM_DIRECTIVE, directive));
        parse_dir(".repeat name", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE_THROWS(eval(boost::get<RepeatDir>(directive).repeat_num));
    }

    SECTION("RDATA") {
        parse_dir(".rdata 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<RDataDir>(directive).address.has_value());
        REQUIRE(boost::get<RDataDir>(directive).address.value() == 0xdeadbeef);

        parse_dir(".rdata 0b1010101", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<RDataDir>(directive).address.has_value());
        REQUIRE(boost::get<RDataDir>(directive).address.value() == 0b1010101);

        parse_dir(".rdata 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<RDataDir>(directive).address.has_value());
        REQUIRE(boost::get<RDataDir>(directive).address.value() == 123);

        parse_dir(".rdata", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<RDataDir>(directive).address.has_value() == false);

        REQUIRE_THROWS(parse_dir(".rdata $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".rdata ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".rdata :", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".rdata name", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("SDATA") {
        parse_dir(".sdata 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SDataDir>(directive).address.has_value());
        REQUIRE(boost::get<SDataDir>(directive).address.value() == 0xdeadbeef);

        parse_dir(".sdata 0b1010101", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SDataDir>(directive).address.has_value());
        REQUIRE(boost::get<SDataDir>(directive).address.value() == 0b1010101);

        parse_dir(".sdata 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SDataDir>(directive).address.has_value());
        REQUIRE(boost::get<SDataDir>(directive).address.value() == 123);

        parse_dir(".sdata", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SDataDir>(directive).address.has_value() == false);

        REQUIRE_THROWS(parse_dir(".sdata $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".sdata ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".sdata :", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".sdata name", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("SET") {
        parse_dir(".set at", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SetDir>(directive).option == "at");

        parse_dir(".set noat", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<SetDir>(directive).option == "noat");

        REQUIRE_THROWS(parse_dir(".set $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".set ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".set :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("SPACE") {
        parse_dir(".space deadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<SpaceDir>(directive).num_bytes) == 0xdeadbeef);

        parse_dir(".space 0b10 | 0b1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<SpaceDir>(directive).num_bytes) == 0b11);

        REQUIRE_THROWS(parse_dir(".space", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".space $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".space ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".space :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("STRUCT") {
        parse_dir(".struct deadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<StructDir>(directive).num_bytes) == 0xdeadbeef);

        parse_dir(".struct 0b10 | 0b1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<StructDir>(directive).num_bytes) == 0b11);

        REQUIRE_THROWS(parse_dir(".struct", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".struct $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".struct ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".struct :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("TEXT") {
        parse_dir(".text 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<TextDir>(directive).addr.has_value());
        REQUIRE(boost::get<TextDir>(directive).addr == 0xdeadbeef);

        parse_dir(".text 0b10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<TextDir>(directive).addr.has_value());
        REQUIRE(boost::get<TextDir>(directive).addr == 0b10);

        parse_dir(".text 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<TextDir>(directive).addr.has_value());
        REQUIRE(boost::get<TextDir>(directive).addr == 123);

        parse_dir(".text", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<TextDir>(directive).addr.has_value() == false);

        REQUIRE_THROWS(parse_dir(".text $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".text ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".text :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("KTEXT") {
        parse_dir(".ktext 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KTextDir>(directive).addr.has_value());
        REQUIRE(boost::get<KTextDir>(directive).addr.value() == 0xdeadbeef);

        parse_dir(".ktext 0b10", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KTextDir>(directive).addr.has_value());
        REQUIRE(boost::get<KTextDir>(directive).addr.value() == 0b10);

        parse_dir(".ktext 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KTextDir>(directive).addr.has_value());
        REQUIRE(boost::get<KTextDir>(directive).addr.value() == 123);

        parse_dir(".ktext", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<KTextDir>(directive).addr.has_value() == false);

        REQUIRE_THROWS(parse_dir(".ktext $1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ktext ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".ktext :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("VERSTAMP") {
        parse_dir(".verstamp 12 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<VerstampDir>(directive).major_ver == 12);
        REQUIRE(boost::get<VerstampDir>(directive).minor_ver == 1);

        REQUIRE_THROWS(parse_dir(".verstamp 30 -1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".verstamp -1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".verstamp 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".verstamp ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".verstamp :", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".verstamp :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("VREG") {
        parse_dir(".vreg $ra 1 12", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(boost::get<VregDir>(directive).reg == 31);
        REQUIRE(boost::get<VregDir>(directive).offset == 1);
        REQUIRE(boost::get<VregDir>(directive).symno == 12);

        REQUIRE_THROWS(parse_dir(".vreg 30 -1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg $t0 -1 -1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg -1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg 1", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg :", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".vreg :", mips_parser::ASM_DIRECTIVE, directive));
    }

    SECTION("WORD") {
        parse_dir(".word 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 1);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123);

        parse_dir(".word 123 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[1]) == 123);

        parse_dir(".word 123, 123", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 2);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[1]) == 123);

        parse_dir(".word 123, 123, 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[2]) == 124);

        parse_dir(".word 123, 123 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[1]) == 123);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[2]) == 124);

        parse_dir(".word 123 + a0 123 + deadbeef 124 + 0xdeadbeef", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list.size()) == 3);
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[0]) == 123 + variable_table.at("a0"));
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[1]) == 123 + variable_table.at("deadbeef"));
        REQUIRE(eval(boost::get<WordDir>(directive).expression_list[2]) == 124 + 0xdeadbeef);

        parse_dir(".word 123: 124", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordRepeatDir>(directive).repeat_list.repeat_value) == 123);
        REQUIRE(eval(boost::get<WordRepeatDir>(directive).repeat_list.repeat_num) == 124);

        parse_dir(".word 123 + 1:124 + 1", mips_parser::ASM_DIRECTIVE, directive);
        REQUIRE(eval(boost::get<WordRepeatDir>(directive).repeat_list.repeat_value) == 124);
        REQUIRE(eval(boost::get<WordRepeatDir>(directive).repeat_list.repeat_num) == 125);

        REQUIRE_THROWS(parse_dir(".word", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".word ,", mips_parser::ASM_DIRECTIVE, directive));
        REQUIRE_THROWS(parse_dir(".word :", mips_parser::ASM_DIRECTIVE, directive));
    }
}
