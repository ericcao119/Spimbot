#pragma once
#ifndef SPIMBOT_PARSER_AST_H
#define SPIMBOT_PARSER_AST_H

#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <iostream>
#include <list>
#include <vector>

namespace client {
namespace ast {
namespace x3 = boost::spirit::x3;

struct unary;
struct expression;

struct label : x3::position_tagged {
    std::string name = "";
};

struct operand : x3::variant<unsigned int, label, x3::forward_ast<unary>, x3::forward_ast<expression> > {
    using base_type::base_type;
    using base_type::operator=;
};

enum optoken {
    op_plus,
    op_minus,
    op_times,
    op_divide,
    op_positive,
    op_negative,
    op_bitwise_not,
    op_bitwise_and,
    op_bitwise_or
};  // , op_and, op_or ;

char to_string(const optoken token) {
    switch (token) {
        case op_plus:
            return '+';
        case op_minus:
            return '-';
        case op_times:
            return '*';
        case op_divide:
            return '/';
        case op_positive:
            return '+';
        case op_negative:
            return '-';
        case op_bitwise_not:
            return '~';
        case op_bitwise_and:
            return '&';
        case op_bitwise_or:
            return '|';
        default:
            return 'Q';
    }
}

struct unary {
    optoken operator_;
    operand operand_;
};

struct operation : x3::position_tagged {
    optoken operator_;
    operand operand_;
};

struct expression : x3::position_tagged {
    operand first;
    std::vector<operation> rest;
};

/**
 * Dumb printing of contained expression. This does not do any work in simplifying the expression!
 *
 * Handles everything as an int except the return type!
 */
struct printer {
    typedef void result_type;

    void operator()(int n) const { std::cout << n; }

    void operator()(const label& lbl) const {
        std::cout << "LBL: ";
        std::cout << lbl.name;
        // std::cout << " :END LABEL";
    }

    void operator()(const unary& op) const {
        std::cout << to_string(op.operator_);
        boost::apply_visitor(*this, op.operand_);
    }

    void operator()(const x3::forward_ast<unary>& forward) const {
        // std::cout << "BEGIN UNARY: ";
        const auto& op = forward.get();
        (*this)(op);
        // std::cout << " :END UNARY";
    }

    void operator()(const expression& ex) const {
        std::cout << "(";

        boost::apply_visitor(*this, ex.first);
        if (ex.rest.size() != 0) {
            std::cout << ' ';
        }

        // std::cout << ex.rest.size() << std::endl;
        for (const auto& op : ex.rest) {
            std::cout << to_string(op.operator_) << ' ';
            boost::apply_visitor(*this, op.operand_);

            // Print space if not last element
            if (&op != &ex.rest.back()) {
                std::cout << ' ';
            }
        }
        std::cout << ")";
    }

    void operator()(const x3::forward_ast<expression>& forward) const {
        const auto& ex = forward.get();
        (*this)(ex);
    }

    void operator()(const operand& x) const {
        std::cout << "Begin OPERAND: ";
        boost::apply_visitor(*this, x);
        std::cout << " :End OPERAND";
    }
};

class unexpected_operator {};

template <uint32_t (*Pred)(const std::string&)>
struct evaluator {
   private:
    // const auto
   public:
    typedef uint32_t result_type;
    uint32_t operator()(int n) const { return n; }

    uint32_t operator()(const label& lbl) const { return Pred(lbl.name); }

    uint32_t operator()(const unary& op) const {
        uint32_t operand = boost::apply_visitor(*this, op.operand_);

        switch (op.operator_) {
            case op_positive:
                return +((int32_t)operand);
            case op_negative:
                return -((int32_t)operand);
            case op_bitwise_not:
                return ~operand;
            default:
                throw unexpected_operator();
        }
    }

    uint32_t operator()(const x3::forward_ast<unary>& forward) const {
        const auto& op = forward.get();
        return (*this)(op);
    }

    uint32_t operator()(const expression& ex) const {
        uint32_t result = boost::apply_visitor(*this, ex.first);

        for (const auto& op : ex.rest) {
            switch (op.operator_) {
                case op_plus:
                    result = (int32_t)result + (int32_t)boost::apply_visitor(*this, op.operand_);
                    break;
                case op_minus:
                    result = (int32_t)result - (int32_t)boost::apply_visitor(*this, op.operand_);
                    break;
                case op_times:
                    result = (int32_t)result * (int32_t)boost::apply_visitor(*this, op.operand_);
                    break;
                case op_divide:
                    result = (int32_t)result / (int32_t)boost::apply_visitor(*this, op.operand_);
                    break;
                case op_bitwise_and:
                    result &= boost::apply_visitor(*this, op.operand_);
                    break;
                case op_bitwise_or:
                    result |= boost::apply_visitor(*this, op.operand_);
                    break;
                default:
                    throw unexpected_operator();
            }
        }
        return result;
    }

    uint32_t operator()(const x3::forward_ast<expression>& forward) const {
        const auto& ex = forward.get();
        return (*this)(ex);
    }
};

using boost::fusion::operator<<;
}  // namespace ast

}  // namespace client

BOOST_FUSION_ADAPT_STRUCT(client::ast::label, name)

BOOST_FUSION_ADAPT_STRUCT(client::ast::unary, operator_, operand_)

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation, operator_, operand_)

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression, first, rest)

#endif