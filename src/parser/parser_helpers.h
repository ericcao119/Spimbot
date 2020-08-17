#ifndef SPIMBOT_PARSER_HELPER_H
#define SPIMBOT_PARSER_HELPER_H

#include <boost/spirit/home/x3.hpp>

namespace mips_parser {

namespace x3 = boost::spirit::x3;

using namespace x3;

/**
 * Quoted String parser. Note that we do not handle \0.
 */
const auto QUOTE_STRING = x3::lexeme['"' > *(
             "\\n" >> x3::attr('\n')
           | "\\b" >> x3::attr('\b')
           | "\\f" >> x3::attr('\f')
           | "\\t" >> x3::attr('\t')
           | "\\v" >> x3::attr('\v')
           | "\\r" >> x3::attr('\r')
           | "\\n" >> x3::attr('\n')
           | "\\"  >> x3::char_("\"\\")
           | "\\" >> x3::int_parser<char, 8, 1, 3>()
           | "\\x" >> x3::int_parser<char, 16, 2, 2>()
           | ~x3::char_('"')
       ) > '"'];

}  // namespace mips_parser

#endif