#ifndef SPIMBOT_PARSER_EXPRESSION_LST_H
#define SPIMBOT_PARSER_EXPRESSION_LST_H

#include "../parser_helpers.h"
#include "expression.h"

namespace mips_parser {

const auto set_value = [](auto& ctx) { _val(ctx).repeat_value = _attr(ctx); };
const auto set_repeat_num = [](auto& ctx) { _val(ctx).repeat_num = _attr(ctx); };

// Repeats
const auto REPEAT_FLOAT = x3::rule<class repeat_float, client::ast::RepeatLst<double>>{"repeat double"} =
    double_[set_value] >> ":" >> expression[set_repeat_num];

const auto FP_EXPR_LST = REPEAT_FLOAT | double_ % (-x3::lit(","));


const auto REPEAT_EXPR = x3::rule<class repeat_expr, client::ast::RepeatLst<client::ast::expression>>{"repeat expr"} =
    expression[set_value] >> ":" >> expression[set_repeat_num];

const auto LITERAL_EXPR_LST =
    x3::rule<class literal_expr_lst, client::ast::LiteralLst<client::ast::expression>>{"literal expr"} =
        expression % (-x3::lit(","));
}  // namespace mips_parser
#endif