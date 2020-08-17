#pragma once
#ifndef SPIMBOT_PARSER_DIRECTIVE_H_
#define SPIMBOT_PARSER_DIRECTIVE_H_

#include <string>

#include "../parser_helpers.h"

namespace mips_parser {
using namespace std::literals;

const auto ALIAS_DIR = ".alias"s;
const auto ALIGN_DIR = ".align"s;
const auto ASCII_DIR = ".ascii"s;
const auto ASCIIZ_DIR = ".asciiz"s;
const auto ASM0_DIR = ".asm0"s;
const auto BGNB_DIR = ".bgnb"s;
const auto BYTE_DIR = ".byte"s;
const auto COMM_DIR = ".comm"s;
const auto DATA_DIR = ".data"s;
const auto K_DATA_DIR = ".kdata"s;
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
const auto K_TEXT_DIR = ".ktext"s;
const auto VERSTAMP_DIR = ".verstamp"s;
const auto VREG_DIR = ".vreg"s;
const auto WORD_DIR = ".word"s;

struct Directive : x3::symbols<int> {
    Directive() {
        *this += ALIAS_DIR, ALIGN_DIR, ASCII_DIR, ASCIIZ_DIR, ASM0_DIR, BGNB_DIR, BYTE_DIR, COMM_DIR, DATA_DIR,
            DOUBLE_DIR, END_DIR, ENDB_DIR, ENDR_DIR, ENT_DIR, ERR_DIR, EXTERN_DIR, FILE_DIR, FLOAT_DIR, FMASK_DIR,
            FRAME_DIR, GLOBAL_DIR, HALF_DIR, K_DATA_DIR, K_TEXT_DIR, LABEL_DIR, LCOMM_DIR, LIVEREG_DIR, LOC_DIR,
            MASK_DIR, NOALIAS_DIR, OPTIONS_DIR, RDATA_DIR, REPEAT_DIR, SDATA_DIR, SET_DIR, SPACE_DIR, STRUCT_DIR,
            TEXT_DIR, VERSTAMP_DIR, VREG_DIR, WORD_DIR;

        // XXX: Add macro support T.T
    }
};

const auto directive_ = Directive{};

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

}  // namespace mips_parser
#endif