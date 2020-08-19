#pragma once
#ifndef SPIMBOT_PARSER_EXPRESSION_H
#define SPIMBOT_PARSER_EXPRESSION_H

#include <cstdint>

#include "../parser_helpers.h"
#include "../primitives/keywords.h"
#include "ast.h"

namespace mips_parser {

using x3::bin;
using x3::hex;
using x3::int_;
using x3::uint_;

////////////////////////////////////////////////////////////////////////////
// Tokens
////////////////////////////////////////////////////////////////////////////

const x3::symbols<client::ast::optoken> additive_op = []() {
    x3::symbols<client::ast::optoken> additive_op;
    additive_op.add("+", client::ast::op_plus)("-", client::ast::op_minus);
    return additive_op;
}();

const x3::symbols<client::ast::optoken> multiplicative_op = []() {
    x3::symbols<client::ast::optoken> multiplicative_op;
    multiplicative_op.add("*", client::ast::op_times)("/", client::ast::op_divide);
    return multiplicative_op;
}();

const x3::symbols<client::ast::optoken> unary_op = []() {
    x3::symbols<client::ast::optoken> unary_op;
    unary_op.add("+", client::ast::op_positive)("-", client::ast::op_negative)("~", client::ast::op_bitwise_not);
    return unary_op;
}();

const x3::symbols<client::ast::optoken> bitwise_or_op = []() {
    x3::symbols<client::ast::optoken> op;
    op.add("|", client::ast::op_bitwise_or);
    return op;
}();

const x3::symbols<client::ast::optoken> bitwise_and_op = []() {
    x3::symbols<client::ast::optoken> op;
    op.add("&", client::ast::op_bitwise_and);
    return op;
}();

////////////////////////////////////////////////////////////////////////////
// Rules
////////////////////////////////////////////////////////////////////////////

const x3::rule<class expression_class, client::ast::expression> expression = "expression_type";
const x3::rule<class bit_or_class, client::ast::expression> bitwise_or_expr = "bitwise_expr_or_type";
const x3::rule<class bit_and_class, client::ast::expression> bitwise_and_expr = "bitwise_expr_and_type";
const x3::rule<class add_expr_class, client::ast::expression> additive_expr = "additive_expr_type";
const x3::rule<class mul_expr_class, client::ast::expression> multiplicative_expr = "multiplicative_expr_type";
const x3::rule<class unary_expr_class, client::ast::operand> unary_expr = "unary_expr_type";
const x3::rule<class primary_expr_class, client::ast::operand> primary_expr = "primary_expr_type";
const x3::rule<class label_expr_class, client::ast::label> label_expr = "label_expr_type";

const auto expression_def = bitwise_or_expr;
const auto bitwise_or_expr_def = bitwise_and_expr >> *(bitwise_or_op > bitwise_and_expr);
const auto bitwise_and_expr_def = additive_expr >> *(bitwise_and_op > additive_expr);
const auto additive_expr_def = multiplicative_expr >> *(additive_op > multiplicative_expr);
const auto multiplicative_expr_def = unary_expr >> *(multiplicative_op > unary_expr);
const auto unary_expr_def = (unary_op > primary_expr) | primary_expr;
const auto primary_expr_def =
    x3::lexeme[("0x" > hex)] | x3::lexeme[("0b" > bin)] | label_expr | uint_ | '(' > expression > ')';
const auto label_expr_def = IDENT;

BOOST_SPIRIT_DEFINE(expression, bitwise_or_expr, bitwise_and_expr, additive_expr, multiplicative_expr, unary_expr,
                    primary_expr, label_expr)

}  // namespace mips_parser

#endif