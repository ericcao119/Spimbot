#ifndef SPIMBOT_PARSER_HELPER_H
#define SPIMBOT_PARSER_HELPER_H

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 50

#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace mips_parser {

namespace x3 = boost::spirit::x3;
using namespace x3;

/**
 * Identifier character sets.
 */
const auto first_ident_ = char_("a-zA-Z$");
const auto ident_ = char_("a-zA-Z0-9$");

// Helper lambdas
auto make_keyword = [](std::string kw) { return lexeme[x3::lit(kw) >> !ident_]; };
auto to_keyword = [](auto kw) { return lexeme[kw >> !ident_]; };

template <typename T> static auto as = [](auto p) { return x3::rule<struct tag, T> {"as"} = p; };

/**
 * Quoted String parser. Note that we do not handle \0.
 */
const x3::rule<class quote_string_rule, std::string> QUOTE_STRING = "quote string rule";
const auto QUOTE_STRING_def =
    x3::lexeme['"' > *("\\n" >> x3::attr('\n') | "\\b" >> x3::attr('\b') | "\\f" >> x3::attr('\f') |
                       "\\t" >> x3::attr('\t') | "\\v" >> x3::attr('\v') | "\\r" >> x3::attr('\r') |
                       "\\n" >> x3::attr('\n') | "\\" >> x3::char_("\"\\") | "\\" >> x3::int_parser<char, 8, 1, 3>() |
                       "\\x" >> x3::int_parser<char, 16, 2, 2>() | ~x3::char_('"')) > '"'];
BOOST_SPIRIT_DEFINE(QUOTE_STRING)
}  // namespace mips_parser

#endif