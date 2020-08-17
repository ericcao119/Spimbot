#include <catch2/catch.hpp>

#include "../test_parser.h"
#include "parser/primitives/keywords.h"
#include "parser/primitives/register.h"

TEST_CASE("Register Parsing", "[parser][register]") {
    using mips_parser::FP_REG;
    using mips_parser::REG;

    SECTION("GENERAL REGISTER") {
        REQUIRE(test_parser("$0", REG));
        REQUIRE(test_parser("$31", REG));
        REQUIRE(test_parser("$zero", REG));
        REQUIRE(test_parser("$ra", REG));
        REQUIRE(test_parser("$t0", REG));
        REQUIRE(test_parser("$t9", REG));
        REQUIRE(test_parser("$s0", REG));
        REQUIRE(test_parser("$s7", REG));
        REQUIRE(test_parser("$sp", REG));
        
        int result;
        test_parser("$0", REG, result, true);
        REQUIRE(result == 0);
        test_parser("$31", REG, result, true);
        REQUIRE(result == 31);

        test_parser("$zero", REG, result, true);
        REQUIRE(result == 0);
        test_parser("$ra", REG, result, true);
        REQUIRE(result == 31);

        // Fail
        REQUIRE_FALSE(test_parser("$-1", REG));
        REQUIRE_FALSE(test_parser("$32", REG));
        REQUIRE_FALSE(test_parser("$ras", REG));
        REQUIRE_FALSE(test_parser("$zeros", REG));
        REQUIRE_FALSE(test_parser("zero", REG));
        REQUIRE_FALSE(test_parser("$r0", REG));
    }

    SECTION("FLOATING POINT REGISTER") {
        REQUIRE(test_parser("$f0", FP_REG));
        REQUIRE(test_parser("$f31", FP_REG));
        
        int result;
        REQUIRE(test_parser("$f0", FP_REG, result, true));
        REQUIRE(result == 0);
        REQUIRE(test_parser("$f31", FP_REG, result, true));
        REQUIRE(result == 31);


        // Fail
        REQUIRE_FALSE(test_parser("$32", FP_REG));
        REQUIRE_FALSE(test_parser("$ras", FP_REG));
        REQUIRE_FALSE(test_parser("$zeros", FP_REG));
        REQUIRE_FALSE(test_parser("zero", FP_REG));
        REQUIRE_FALSE(test_parser("$r0", FP_REG));
        REQUIRE_FALSE(test_parser("$-1", FP_REG));
        
        REQUIRE_FALSE(test_parser("$ra", FP_REG, result, true));
        REQUIRE_FALSE(test_parser("$zero", FP_REG, result, true));
    }
    
}