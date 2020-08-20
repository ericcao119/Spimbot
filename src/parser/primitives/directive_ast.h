#pragma once
#ifndef SPIMBOT_PARSER_DIRECTIVE_AST_H
#define SPIMBOT_PARSER_DIRECTIVE_AST_H

#include "../expression/ast.h"
#include "../parser_helpers.h"

/**
 * Declare basic constants to be used!
 */
namespace mips_parser {
using namespace std::literals;

/* Directives */
const auto ALIAS_DIR = ".alias"s;
const auto ALIGN_DIR = ".align"s;
const auto ASCII_DIR = ".ascii"s;
const auto ASCIIZ_DIR = ".asciiz"s;
const auto ASM0_DIR = ".asm0"s;
const auto BGNB_DIR = ".bgnb"s;
const auto BYTE_DIR = ".byte"s;
const auto COMM_DIR = ".comm"s;
const auto DATA_DIR = ".data"s;
const auto KDATA_DIR = ".kdata"s;
const auto DOUBLE_DIR = ".double"s;
const auto END_DIR = ".end"s;
const auto ENDB_DIR = ".endb"s;
const auto ENDR_DIR = ".endr"s;
const auto ENT_DIR = ".ent"s;
const auto ERR_DIR = ".err"s;
const auto EXTERN_DIR = ".extern"s;
const auto FILE_DIR = ".file"s;
const auto FLOAT_DIR = ".float"s;
const auto FMASK_DIR = ".fmask"s;
const auto FRAME_DIR = ".frame"s;
const auto GLOBAL_DIR = ".globl"s;
const auto HALF_DIR = ".half"s;
const auto LABEL_DIR = ".lab"s;
const auto LCOMM_DIR = ".lcomm"s;
const auto LIVEREG_DIR = ".livereg"s;
const auto LOC_DIR = ".loc"s;
const auto MASK_DIR = ".mask"s;
const auto NOALIAS_DIR = ".noalias"s;
const auto OPTIONS_DIR = ".option"s;
const auto RDATA_DIR = ".rdata"s;
const auto REPEAT_DIR = ".repeat"s;
const auto SDATA_DIR = ".sdata"s;
const auto SET_DIR = ".set"s;
const auto SPACE_DIR = ".space"s;
const auto STRUCT_DIR = ".struct"s;
const auto TEXT_DIR = ".text"s;
const auto KTEXT_DIR = ".ktext"s;
const auto VERSTAMP_DIR = ".verstamp"s;
const auto VREG_DIR = ".vreg"s;
const auto WORD_DIR = ".word"s;

/* Keywords */
const auto ALIAS_DIR_KW = make_keyword(ALIAS_DIR);
const auto ALIGN_DIR_KW = make_keyword(ALIGN_DIR);
const auto ASCII_DIR_KW = make_keyword(ASCII_DIR);
const auto ASCIIZ_DIR_KW = make_keyword(ASCIIZ_DIR);
const auto ASM0_DIR_KW = make_keyword(ASM0_DIR);
const auto BGNB_DIR_KW = make_keyword(BGNB_DIR);
const auto BYTE_DIR_KW = make_keyword(BYTE_DIR);
const auto COMM_DIR_KW = make_keyword(COMM_DIR);
const auto DATA_DIR_KW = make_keyword(DATA_DIR);
const auto KDATA_DIR_KW = make_keyword(KDATA_DIR);
const auto DOUBLE_DIR_KW = make_keyword(DOUBLE_DIR);
const auto END_DIR_KW = make_keyword(END_DIR);
const auto ENDB_DIR_KW = make_keyword(ENDB_DIR);
const auto ENDR_DIR_KW = make_keyword(ENDR_DIR);
const auto ENT_DIR_KW = make_keyword(ENT_DIR);
const auto ERR_DIR_KW = make_keyword(ERR_DIR);
const auto EXTERN_DIR_KW = make_keyword(EXTERN_DIR);
const auto FILE_DIR_KW = make_keyword(FILE_DIR);
const auto FLOAT_DIR_KW = make_keyword(FLOAT_DIR);
const auto FMASK_DIR_KW = make_keyword(FMASK_DIR);
const auto FRAME_DIR_KW = make_keyword(FRAME_DIR);
const auto GLOBAL_DIR_KW = make_keyword(GLOBAL_DIR);
const auto HALF_DIR_KW = make_keyword(HALF_DIR);
const auto LABEL_DIR_KW = make_keyword(LABEL_DIR);
const auto LCOMM_DIR_KW = make_keyword(LCOMM_DIR);
const auto LIVEREG_DIR_KW = make_keyword(LIVEREG_DIR);
const auto LOC_DIR_KW = make_keyword(LOC_DIR);
const auto MASK_DIR_KW = make_keyword(MASK_DIR);
const auto NOALIAS_DIR_KW = make_keyword(NOALIAS_DIR);
const auto OPTIONS_DIR_KW = make_keyword(OPTIONS_DIR);
const auto RDATA_DIR_KW = make_keyword(RDATA_DIR);
const auto REPEAT_DIR_KW = make_keyword(REPEAT_DIR);
const auto SDATA_DIR_KW = make_keyword(SDATA_DIR);
const auto SET_DIR_KW = make_keyword(SET_DIR);
const auto SPACE_DIR_KW = make_keyword(SPACE_DIR);
const auto STRUCT_DIR_KW = make_keyword(STRUCT_DIR);
const auto TEXT_DIR_KW = make_keyword(TEXT_DIR);
const auto KTEXT_DIR_KW = make_keyword(KTEXT_DIR);
const auto VERSTAMP_DIR_KW = make_keyword(VERSTAMP_DIR);
const auto VREG_DIR_KW = make_keyword(VREG_DIR);
const auto WORD_DIR_KW = make_keyword(WORD_DIR);
}  // namespace mips_parser

namespace client {
namespace ast {

namespace x3 = boost::spirit::x3;

/* AST Nodes */
struct AliasDir : x3::position_tagged {
    int reg1;
    int reg2;
};

struct AlignDir : x3::position_tagged {
    /**
     * Alignment is expressed as the exponent. in 2**alignment.
     *
     * So, alignment 3 aligns the addresses to a power of 8.
     */
    client::ast::expression alignment;
};

struct AsciiDir : x3::position_tagged, std::vector<std::string> {};

struct AsciizDir : x3::position_tagged, std::vector<std::string> {};

struct Asm0Dir : x3::position_tagged {};

struct BgnbDir : x3::position_tagged {
    int symno;
};

struct ByteDir : x3::position_tagged {
    std::vector<client::ast::expression> bytes;
};

struct CommDir : x3::position_tagged {
    std::string ident;
    client::ast::expression expr;
};

struct DataDir : x3::position_tagged {
    x3::optional<uint32_t> addr;
};

struct KDataDir : x3::position_tagged {
    x3::optional<uint32_t> addr;
};

struct DoubleDir : x3::position_tagged {
    std::vector<double> expression_list;
};

/**
 * Should be matched with an ENT name for debugging purposes
 */
struct EndDir : x3::position_tagged {
    x3::optional<std::string> proc_name;
};

/**
 * Sets the end of a language block. Matched with Bgnb
 */
struct EndbDir : x3::position_tagged {
    int symno;
};

/**
 * End of repeat block
 */
struct EndrDir : x3::position_tagged {};

/**
 * The beginning of a proc name
 */
struct EntDir : x3::position_tagged {
    std::string proc_name;
    x3::optional<int> lex_level;
};

struct ExternDir : x3::position_tagged {
    /**
     * Symbol defined in other file
     */
    std::string name;

    /**
     * Expected size of the external object
     *
     * XXX: Make this field optional
     */
    client::ast::expression number;
};

struct ErrDir : x3::position_tagged {};

struct FileDir : x3::position_tagged {
    int32_t file_no;
    std::string filename;
};

struct FloatDir : x3::position_tagged {
    std::vector<float> expression_list;
};

/**
 * Sets a mask with a bit turned on for each floating-point register that the current routine saved.
 * The least-significant bit corresponds to register $f0. The offset is the distance in bytes from the
 * virtual frame pointer to where the floating-point registers are saved.
 * - http://ftp.icm.edu.pl/packages/linux-uk/alpha/alpha/asm6.html
 */
struct FmaskDir : x3::position_tagged {
    uint32_t mask;
    uint32_t offset;
};

struct FrameDir : x3::position_tagged {
    uint32_t frame_register;
    uint32_t frame_size;
    uint32_t return_pc_register;
};

struct GlobalDir : x3::position_tagged {
    std::string id;
};

struct HalfDir : x3::position_tagged {
    std::vector<client::ast::expression> expression_list;
};

struct LabelDir : x3::position_tagged {
    std::string label_name;
};

struct LcommDir : x3::position_tagged {
    std::string name;
    client::ast::expression expr;
};

struct LiveregDir : x3::position_tagged {
    uint32_t int_bitmask;
    uint32_t fp_bitmask;
};

struct LocDir : x3::position_tagged {
    int32_t file_number;
    int32_t line_number;
};

/**
 * Sets a mask with a bit turned on for each general-purpose register that the current routine saved.
 * The least significant bit corresponds to register $0. The offset is the distance in bytes from the
 * virtual frame pointer to where the registers are saved.
 *
 * You must use .ent before .mask, and you can use only one .mask for each .ent. Space should be
 * allocated for those registers specified in the .mask.
 *
 * - http://ftp.icm.edu.pl/packages/linux-uk/alpha/alpha/asm6.html
 */
struct MaskDir : x3::position_tagged {
    uint32_t mask;
    uint32_t offset;
};

struct NoaliasDir : x3::position_tagged {
    uint32_t reg1;
    uint32_t reg2;
};

struct OptionDir : x3::position_tagged {
    std::string option;
};

struct RepeatDir : x3::position_tagged {
    client::ast::expression repeat_num;
};

/**
 * Read-only data
 */
struct RDataDir : x3::position_tagged {
    x3::optional<uint32_t> address;
};

/**
 * Initialized data relative accessed with GP relative addressing
 */
struct SDataDir : x3::position_tagged {
    x3::optional<uint32_t> address;
};

struct SetDir : x3::position_tagged {
    // Only at and noat supported
    std::string option;
};

struct SpaceDir : x3::position_tagged {
    client::ast::expression num_bytes;
};

struct StructDir : x3::position_tagged {
    client::ast::expression num_bytes;
};

struct TextDir : x3::position_tagged {
    x3::optional<uint32_t> addr;
};

struct KTextDir : x3::position_tagged {
    x3::optional<uint32_t> addr;
};

/******************************************************/

struct VerstampDir : x3::position_tagged {
    uint32_t major_ver;
    uint32_t minor_ver;
};

struct VregDir : x3::position_tagged {
    uint32_t reg;
    int32_t offset;
    int32_t symno;
};

struct WordDir : x3::position_tagged {
    std::vector<client::ast::expression> expression_list;
};

struct Directive
    : x3::variant<AliasDir, AlignDir, AsciiDir, AsciizDir, Asm0Dir, BgnbDir, ByteDir, CommDir, DataDir, KDataDir,
                  DoubleDir, EndDir, EndbDir, EndrDir, EntDir, ExternDir, ErrDir, FileDir, FloatDir, FmaskDir, FrameDir,
                  GlobalDir, HalfDir, LabelDir, LcommDir, LiveregDir, LocDir, MaskDir, NoaliasDir, OptionDir, RepeatDir,
                  RDataDir, SDataDir, SetDir, SpaceDir, StructDir, TextDir, VerstampDir, VregDir, WordDir> {
    using base_type::base_type;
    using base_type::operator=;
};
}  // namespace ast
}  // namespace client

/**
 * These do not exist due to limitations with single element
 * sequences See: https://www.codevamping.com/2019/01/attribute-hoisting-in-boost-spirit-x3/
 */
// BOOST_FUSION_ADAPT_STRUCT(client::ast::AsciiDir, strings)
// BOOST_FUSION_ADAPT_STRUCT(client::ast::AsciizDir, strings)

BOOST_FUSION_ADAPT_STRUCT(client::ast::AliasDir, reg1, reg2)
BOOST_FUSION_ADAPT_STRUCT(client::ast::AlignDir, alignment)
BOOST_FUSION_ADAPT_STRUCT(client::ast::Asm0Dir, /**/)
BOOST_FUSION_ADAPT_STRUCT(client::ast::BgnbDir, symno)
BOOST_FUSION_ADAPT_STRUCT(client::ast::ByteDir, bytes)
BOOST_FUSION_ADAPT_STRUCT(client::ast::CommDir, ident, expr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::DataDir, addr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::KDataDir, addr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::DoubleDir, expression_list)
BOOST_FUSION_ADAPT_STRUCT(client::ast::EndDir, proc_name)
BOOST_FUSION_ADAPT_STRUCT(client::ast::EndbDir, symno)
BOOST_FUSION_ADAPT_STRUCT(client::ast::EndrDir, /**/)
BOOST_FUSION_ADAPT_STRUCT(client::ast::EntDir, proc_name, lex_level)
BOOST_FUSION_ADAPT_STRUCT(client::ast::ExternDir, name, number)
BOOST_FUSION_ADAPT_STRUCT(client::ast::ErrDir, /**/)
BOOST_FUSION_ADAPT_STRUCT(client::ast::FileDir, file_no, filename)
BOOST_FUSION_ADAPT_STRUCT(client::ast::FloatDir, expression_list)
BOOST_FUSION_ADAPT_STRUCT(client::ast::FmaskDir, mask, offset)
BOOST_FUSION_ADAPT_STRUCT(client::ast::FrameDir, frame_register, frame_size, return_pc_register)
BOOST_FUSION_ADAPT_STRUCT(client::ast::GlobalDir, id)
BOOST_FUSION_ADAPT_STRUCT(client::ast::HalfDir, expression_list)
BOOST_FUSION_ADAPT_STRUCT(client::ast::LabelDir, label_name)
BOOST_FUSION_ADAPT_STRUCT(client::ast::LcommDir, name, expr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::LiveregDir, int_bitmask, fp_bitmask)
BOOST_FUSION_ADAPT_STRUCT(client::ast::LocDir, file_number, line_number)
BOOST_FUSION_ADAPT_STRUCT(client::ast::MaskDir, mask, offset)
BOOST_FUSION_ADAPT_STRUCT(client::ast::NoaliasDir, reg1, reg2)
BOOST_FUSION_ADAPT_STRUCT(client::ast::OptionDir, option)
BOOST_FUSION_ADAPT_STRUCT(client::ast::RepeatDir, repeat_num)
BOOST_FUSION_ADAPT_STRUCT(client::ast::RDataDir, address)
BOOST_FUSION_ADAPT_STRUCT(client::ast::SDataDir, address)
BOOST_FUSION_ADAPT_STRUCT(client::ast::SetDir, option)
BOOST_FUSION_ADAPT_STRUCT(client::ast::StructDir, num_bytes)
BOOST_FUSION_ADAPT_STRUCT(client::ast::TextDir, addr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::KTextDir, addr)
BOOST_FUSION_ADAPT_STRUCT(client::ast::VerstampDir, major_ver, minor_ver)
BOOST_FUSION_ADAPT_STRUCT(client::ast::VregDir, reg, offset, symno)
BOOST_FUSION_ADAPT_STRUCT(client::ast::WordDir, expression_list)
#endif