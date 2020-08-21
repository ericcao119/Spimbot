#pragma once
#ifndef SPIMBOT_PARSER_PRIMITIVES_DIRECTIVE
#define SPIMBOT_PARSER_PRIMITIVES_DIRECTIVE

#include "../expression/expression.h"
#include "../expression/expression_lst.h"
#include "atomics.h"
#include "keywords.h"

namespace mips_parser {

/* Rule declarations */
const x3::rule<class asm_directive, client::ast::Directive> ASM_DIRECTIVE = "asm directive rule";
const x3::rule<class alias_rule, client::ast::AliasDir> ALIAS_RULE = "alias rule";
const x3::rule<class align_rule, client::ast::AlignDir> ALIGN_RULE = "align rule";
const x3::rule<class ascii_rule, client::ast::AsciiDir> ASCII_RULE = "ascii rule";
const x3::rule<class asciiz_rule, client::ast::AsciizDir> ASCIIZ_RULE = "asciiz rule";
const x3::rule<class asm0_rule, client::ast::Asm0Dir> ASM0_RULE = "asm0 rule";
const x3::rule<class bgnb_rule, client::ast::BgnbDir> BNGB_RULE = "bngb rule";
const x3::rule<class byte_repeat_rule, client::ast::ByteRepeatDir> BYTE_REPEAT_RULE = "byte repeat rule";
const x3::rule<class byte_literal_rule, client::ast::ByteDir> BYTE_RULE = "byte rule";
const x3::rule<class comm_rule, client::ast::CommDir> COMM_RULE = "comm rule";
const x3::rule<class data_rule, client::ast::DataDir> DATA_RULE = "data rule";
const x3::rule<class kdata_rule, client::ast::KDataDir> KDATA_RULE = "kdata rule";

const x3::rule<class double_repeat_rule, client::ast::DoubleRepeatDir> DOUBLE_REPEAT_RULE = "double repeat rule";
const x3::rule<class double_rule, client::ast::DoubleDir> DOUBLE_RULE = "double rule";

const x3::rule<class end_rule, client::ast::EndDir> END_RULE = "end rule";
const x3::rule<class endb_rule, client::ast::EndbDir> ENDB_RULE = "endb rule";
const x3::rule<class endr_rule, client::ast::EndrDir> ENDR_RULE = "endr rule";
const x3::rule<class ent_rule, client::ast::EntDir> ENT_RULE = "ent rule";
const x3::rule<class extern_rule, client::ast::ExternDir> EXTERN_RULE = "extern rule";
const x3::rule<class err_rule, client::ast::ErrDir> ERR_RULE = "err rule";
const x3::rule<class file_rule, client::ast::FileDir> FILE_RULE = "file rule";
const x3::rule<class float_rule, client::ast::FloatDir> FLOAT_RULE = "float rule";
const x3::rule<class fmask_rule, client::ast::FmaskDir> FMASK_RULE = "fmask rule";
const x3::rule<class frame_rule, client::ast::FrameDir> FRAME_RULE = "frame rule";
const x3::rule<class global_rule, client::ast::GlobalDir> GLOBAL_RULE = "global rule";
const x3::rule<class half_rule, client::ast::HalfDir> HALF_RULE = "half rule";
const x3::rule<class label_rule, client::ast::LabelDir> LABEL_RULE = "label rule";
const x3::rule<class lcomm_rule, client::ast::LcommDir> LCOMM_RULE = "lcomm rule";
const x3::rule<class livereg_rule, client::ast::LiveregDir> LIVEREG_RULE = "livereg rule";
const x3::rule<class loc_rule, client::ast::LocDir> LOC_RULE = "loc rule";
const x3::rule<class mask_rule, client::ast::MaskDir> MASK_RULE = "mask rule";
const x3::rule<class noalias_rule, client::ast::NoaliasDir> NOALIAS_RULE = "noalias rule";
const x3::rule<class option_rule, client::ast::OptionDir> OPTIONS_RULE = "options rule";  // ?
const x3::rule<class repeat_rule, client::ast::RepeatDir> REPEAT_RULE = "repeat rule";
const x3::rule<class rdata_rule, client::ast::RDataDir> RDATA_RULE = "rdata rule";
const x3::rule<class sdata_rule, client::ast::SDataDir> SDATA_RULE = "sdata rule";
const x3::rule<class set_rule, client::ast::SetDir> SET_RULE = "set rule";
const x3::rule<class space_rule, client::ast::SpaceDir> SPACE_RULE = "space rule";
const x3::rule<class struct_rule, client::ast::StructDir> STRUCT_RULE = "struct rule";
const x3::rule<class text_rule, client::ast::TextDir> TEXT_RULE = "text rule";
const x3::rule<class ktext_rule, client::ast::KTextDir> KTEXT_RULE = "ktext rule";
const x3::rule<class verstamp_rule, client::ast::VerstampDir> VERSTAMP_RULE = "verstamp rule";
const x3::rule<class vreg_rule, client::ast::VregDir> VREG_RULE = "vreg rule";
const x3::rule<class word_rule, client::ast::WordDir> WORD_RULE = "word rule";

/* Rule definitions */

// const auto ASCII_RULE_def = ASCII_DIR_KW;

const auto ASM_DIRECTIVE_def =
    ALIAS_RULE | ALIGN_RULE | ASCII_RULE | ASCIIZ_RULE | ASM0_RULE | BNGB_RULE | BYTE_REPEAT_RULE | BYTE_RULE |
    COMM_RULE | DATA_RULE | KDATA_RULE | DOUBLE_REPEAT_RULE | DOUBLE_RULE;/* | END_RULE | ENDB_RULE | ENDR_RULE | ENT_RULE | EXTERN_RULE | ERR_RULE | FILE_RULE |
    FLOAT_RULE | FMASK_RULE | FRAME_RULE | GLOBAL_RULE | HALF_RULE | LABEL_RULE | LCOMM_RULE | LIVEREG_RULE | LOC_RULE |
    MASK_RULE | NOALIAS_RULE | OPTIONS_RULE | REPEAT_RULE | RDATA_RULE | SDATA_RULE | SET_RULE | SPACE_RULE |
    STRUCT_RULE | TEXT_RULE | KTEXT_RULE | VERSTAMP_RULE | VREG_RULE | WORD_RULE; */

const auto ALIAS_RULE_def = ALIAS_DIR_KW >> REG >> REG;
const auto ALIGN_RULE_def = ALIGN_DIR_KW >> expression;
const auto ASCII_RULE_def = ASCII_DIR_KW >> (QUOTE_STRING % ",");
const auto ASCIIZ_RULE_def = ASCIIZ_DIR_KW >> (QUOTE_STRING % ",");
const auto ASM0_RULE_def = ASM0_DIR_KW;
const auto BNGB_RULE_def = BGNB_DIR_KW >> uint_;
const auto BYTE_REPEAT_RULE_def = BYTE_DIR_KW >> REPEAT_EXPR_LST;        // >> EXPR_LST;
const auto BYTE_RULE_def = BYTE_DIR_KW >> EXPR_LST;  // >> EXPR_LST;
const auto COMM_RULE_def = COMM_DIR_KW >> IDENT >> "," >> expression;
const auto DATA_RULE_def = DATA_DIR_KW >> -(x3::lexeme[("0x" > hex)] | x3::lexeme[("0b" > bin)] | uint_);
const auto KDATA_RULE_def = KDATA_DIR_KW >> -(x3::lexeme[("0x" > hex)] | x3::lexeme[("0b" > bin)] | uint_);


const auto DOUBLE_RULE_def = DOUBLE_DIR_KW >> FP_EXPR_LST;
const auto DOUBLE_REPEAT_RULE_def = DOUBLE_DIR_KW >> FP_REPEAT_EXPR_LST;

const auto END_RULE_def = END_DIR_KW >> -(IDENT);
const auto ENDB_RULE_def = ENDB_DIR_KW >> -(uint_);
const auto ENDR_RULE_def = ENDR_DIR_KW;
const auto ENT_RULE_def = ENT_DIR_KW >> IDENT >> uint_;
const auto EXTERN_RULE_def = EXTERN_DIR_KW >> IDENT >> expression;
const auto ERR_RULE_def = ERR_DIR_KW;
const auto FILE_RULE_def = FILE_DIR_KW >> uint_ >> QUOTE_STRING;
const auto FLOAT_RULE_def = FLOAT_DIR_KW >> *(float_);
const auto FMASK_RULE_def = FMASK_DIR_KW >> uint_ >> uint_;
const auto FRAME_RULE_def = FRAME_DIR_KW >> REG >> uint_ >> REG;
const auto GLOBAL_RULE_def = GLOBAL_DIR_KW >> IDENT;
const auto HALF_RULE_def = HALF_DIR_KW >> *(expression);
const auto LABEL_RULE_def = LABEL_DIR_KW >> IDENT;
const auto LCOMM_RULE_def = LCOMM_DIR_KW >> IDENT >> expression;
const auto LIVEREG_RULE_def = LIVEREG_DIR_KW >> uint_ >> uint_;
const auto LOC_RULE_def = LOC_DIR_KW >> uint_ >> uint_;
const auto MASK_RULE_def = MASK_DIR_KW >> uint_ >> uint_;
const auto NOALIAS_RULE_def = NOALIAS_DIR_KW >> REG >> REG;
const auto OPTIONS_RULE_def = OPTIONS_DIR_KW >> IDENT;  // ?
const auto REPEAT_RULE_def = REPEAT_DIR_KW >> expression;
const auto RDATA_RULE_def = RDATA_DIR_KW >> -(uint_);
const auto SDATA_RULE_def = SDATA_DIR_KW >> -(uint_);
const auto SET_RULE_def = SET_DIR_KW >> IDENT;
const auto SPACE_RULE_def = SPACE_DIR_KW >> expression;
const auto STRUCT_RULE_def = STRUCT_DIR_KW >> expression;
const auto TEXT_RULE_def = TEXT_DIR_KW >> -(uint_);
const auto KTEXT_RULE_def = KTEXT_DIR_KW >> -(uint_);
const auto VERSTAMP_RULE_def = VERSTAMP_DIR_KW >> uint_ >> uint_;
const auto VREG_RULE_def = VREG_DIR_KW >> REG >> uint_ >> uint_;
const auto WORD_RULE_def = WORD_DIR_KW >> *(expression);

BOOST_SPIRIT_DEFINE(ASM_DIRECTIVE, ALIAS_RULE, ALIGN_RULE, ASCII_RULE, ASCIIZ_RULE, ASM0_RULE, BNGB_RULE,
                    BYTE_REPEAT_RULE, BYTE_RULE, COMM_RULE, DATA_RULE, KDATA_RULE, DOUBLE_REPEAT_RULE, DOUBLE_RULE/*, END_RULE,
                    ENDB_RULE, ENDR_RULE, ENT_RULE, EXTERN_RULE, ERR_RULE, FILE_RULE, FLOAT_RULE, FMASK_RULE,
                    FRAME_RULE, GLOBAL_RULE, HALF_RULE, LABEL_RULE, LCOMM_RULE, LIVEREG_RULE, LOC_RULE, MASK_RULE,
                    NOALIAS_RULE, OPTIONS_RULE, REPEAT_RULE, RDATA_RULE, SDATA_RULE, SET_RULE, SPACE_RULE, STRUCT_RULE,
                    TEXT_RULE, KTEXT_RULE, VERSTAMP_RULE, VREG_RULE, WORD_RULE*/)

}  // namespace mips_parser
// ASM_DIRECTIVE:	Y_ALIAS_DIR	Y_REG	Y_REG

//     |	Y_ALIGN_DIR	EXPR
//         {
//           align_data ($2.i);
//         }

//     |	Y_ASCII_DIR {null_term = false;}	STR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_ASCIIZ_DIR {null_term = true;}	STR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_ASM0_DIR

//     |	Y_BGNB_DIR	Y_INT

//     |	Y_BYTE_DIR
//         {store_op = (void(*)(void*))store_byte;}
//         EXPR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_COMM_DIR	ID	EXPR
//         {
//           align_data (2);
//           if (lookup_label ((char*)$2.p)->addr == 0)
//           {
//             (void)record_label ((char*)$2.p, current_data_pc (), 1);
//             free ((char*)$2.p);
//           }
//           increment_data_pc ($3.i);
//         }

//     |	Y_DATA_DIR
//         {user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//         }

//     |	Y_DATA_DIR	Y_INT
//         {
//           user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//           set_data_pc ($2.i);
//         }

//     |	Y_K_DATA_DIR
//         {
//                     user_kernel_data_segment (true);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//         }

//     |	Y_K_DATA_DIR	Y_INT
//         {
//                     user_kernel_data_segment (true);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//           set_data_pc ($2.i);
//         }

//     |	Y_DOUBLE_DIR
//         {
//           store_op = (void(*)(void*))store_double;
//           if (data_dir) set_data_alignment (3);
//         }
//         FP_EXPR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_END_DIR	OPTIONAL_ID

//     |	Y_ENDB_DIR	Y_INT

//     |	Y_ENDR_DIR

//     |	Y_ENT_DIR	ID

//     |	Y_ENT_DIR	ID	Y_INT

//     |	Y_EXTERN_DIR	ID	EXPR
//         {
//           extern_directive ((char*)$2.p, $3.i);
//         }

//     |	Y_ERR_DIR
//         {
//           fatal_error ("File contains an .err directive\n");
//         }

//     |	Y_FILE_DIR	Y_INT	Y_STR

//     |	Y_FLOAT_DIR
//         {
//           store_op = (void(*)(void*))store_float;
//           if (data_dir) set_data_alignment (2);
//         }
//         FP_EXPR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_FMASK_DIR	Y_INT	Y_INT

//     |	Y_FRAME_DIR	REGISTER	Y_INT	REGISTER

//     |	Y_GLOBAL_DIR	ID
//         {
//           (void)make_label_global ((char*)$2.p);
//           free ((char*)$2.p);
//         }

//     |	Y_HALF_DIR
//         {
//           store_op = (void(*)(void*))store_half;
//           if (data_dir) set_data_alignment (1);
//         }
//         EXPR_LST
//         {
//           if (text_dir)
//             yyerror ("Can't put data in text segment");
//         }

//     |	Y_LABEL_DIR	ID
//         {
//           (void)record_label ((char*)$2.p,
//                       text_dir ? current_text_pc () : current_data_pc (),
//                       1);
//           free ((char*)$2.p);
//         }

//     |	Y_LCOMM_DIR	ID	EXPR
//         {
//           lcomm_directive ((char*)$2.p, $3.i);
//         }

//         /* Produced by cc 2.10 */
//     |	Y_LIVEREG_DIR	Y_INT	Y_INT

//     |	Y_LOC_DIR	Y_INT	Y_INT

//     |	Y_MASK_DIR	Y_INT	Y_INT

//     |	Y_NOALIAS_DIR	Y_REG	Y_REG

//     |	Y_OPTIONS_DIR	ID

//     |	Y_REPEAT_DIR	EXPR
//         {
//           yyerror ("Warning: repeat directive ignored");
//         }

//     |	Y_RDATA_DIR
//         {
//           user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//         }

//     |	Y_RDATA_DIR	Y_INT
//         {
//           user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//           set_data_pc ($2.i);
//         }

//     |	Y_SDATA_DIR
//         {
//           user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//         }

//     |	Y_SDATA_DIR	Y_INT
//         {
//           user_kernel_data_segment (false);
//           data_dir = true; text_dir = false;
//           enable_data_alignment ();
//           set_data_pc ($2.i);
//         }

//     |	Y_SET_DIR	ID
//         {
//           if (streq ((char*)$2.p, "noat"))
//             noat_flag = true;
//           else if (streq ((char*)$2.p, "at"))
//             noat_flag = false;
//         }

//     |	Y_SPACE_DIR	EXPR
//         {
//           if (data_dir)
//             increment_data_pc ($2.i);
//           else if (text_dir)
//             increment_text_pc ($2.i);
//         }

//     |	Y_STRUCT_DIR	EXPR
//         {
//           yyerror ("Warning: struct directive ignored");
//         }

//     |	Y_TEXT_DIR
//         {
//           user_kernel_text_segment (false);
//           data_dir = false; text_dir = true;
//           enable_data_alignment ();
//         }

//     |	Y_TEXT_DIR	Y_INT
//         {
//           user_kernel_text_segment (false);
//           data_dir = false; text_dir = true;
//           enable_data_alignment ();
//           set_text_pc ($2.i);
//         }

//     |	Y_K_TEXT_DIR
//         {
//           user_kernel_text_segment (true);
//           data_dir = false; text_dir = true;
//           enable_data_alignment ();
//         }

//     |	Y_K_TEXT_DIR	Y_INT
//         {
//           user_kernel_text_segment (true);
//           data_dir = false; text_dir = true;
//           enable_data_alignment ();
//           set_text_pc ($2.i);
//         }

//     |	Y_VERSTAMP_DIR	Y_INT	Y_INT

//     |	Y_VREG_DIR	REGISTER	Y_INT	Y_INT

//     |	Y_WORD_DIR
//         {
//           store_op = (void(*)(void*))store_word_data;
//           if (data_dir) set_data_alignment (2);
//         }
//         EXPR_LST

//     ;
#endif