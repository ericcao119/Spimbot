#include "parser_helpers.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <iterator>

using namespace boost::spirit;

int main() {

    for (std::string const input : { R"("\ttest\x41\x42\x43 \x031\x032\x033 \"hello\"\r\n")" }) {
        std::string output;
        auto f = begin(input), l = end(input);
        if (x3::phrase_parse(f, l, mips_parser::QUOTE_STRING, x3::blank, output)) {
            std::cout << "[" << output << "]\n";
        } else {
            std::cout << "Failed\n";
        }
        if (f != l) {
            std::cout << "Remaining unparsed: " << std::quoted(std::string(f,l)) << "\n";
        }
    };
}