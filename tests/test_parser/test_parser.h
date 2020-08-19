#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <parser/parser_helpers.h>
#include <iostream>

namespace x3 = boost::spirit::x3;

using x3::alnum;
using x3::alpha;
using x3::char_;
using x3::lexeme;
using x3::lit;

template <typename P>
bool test_parser(char const* input, P const& p, bool full_match = true) {
    using boost::spirit::x3::parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (parse(f, l, p) && (!full_match || (f == l))) {
        return true;
    } else {
        return false;
    }
}

template <typename P>
bool test_phrase_parser(char const* input, P const& p, bool full_match = true) {
    using boost::spirit::x3::phrase_parse;
    using boost::spirit::x3::ascii::space;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, space) && (!full_match || (f == l))) {
        return true;
    } else {
        return false;
    }
}

template <typename P, typename A>
bool test_parser(char const* input, P const& p, A& a, bool full_match = true) {
    using boost::spirit::x3::parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (parse(f, l, p, a) && (!full_match || (f == l))) {
        return true;
    } else {
        return false;
    }
}

template <typename P, typename A>
bool test_phrase_parser(char const* input, P const& p, A& a, bool full_match = true) {
    using boost::spirit::x3::phrase_parse;
    using boost::spirit::x3::ascii::space;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, a, space) && (!full_match || (f == l))) {
        return true;
    } else {
        return false;
    }
}

class parse_failed_exception{};

#endif