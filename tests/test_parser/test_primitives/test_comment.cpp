#include <catch2/catch.hpp>

#include "../test_parser.h"
#include "engine/turn.h"
#include "parser/primitives/comment.h"

TEST_CASE("Parse comments", "[parser][comment]") {
    using mips_parser::BLOCK_COMMENT;
    using mips_parser::COMMENT;
    using mips_parser::LINE_COMMENT;

    SECTION("Line Comment") {
        REQUIRE(test_parser("//abc", LINE_COMMENT));
        REQUIRE(test_parser("// abc", LINE_COMMENT));
        REQUIRE(test_parser("#abc", LINE_COMMENT));
        REQUIRE(test_parser("# abc", LINE_COMMENT));
        REQUIRE(test_parser("# abc \n", LINE_COMMENT >> x3::eol));
        REQUIRE(test_parser("# abc \n ", LINE_COMMENT >> x3::eol >> x3::space));
        // Fail
        REQUIRE_FALSE(test_parser("# abc \n", LINE_COMMENT));
        REQUIRE_FALSE(test_parser("# abc \n ", LINE_COMMENT));
        REQUIRE_FALSE(test_parser("/*abc*/", LINE_COMMENT));
    }

    SECTION("Block Comment") {
        REQUIRE(test_parser("/*abc*/", BLOCK_COMMENT));
        REQUIRE(test_parser("/* /* abc */", BLOCK_COMMENT));
        REQUIRE(test_parser("/** abc */", BLOCK_COMMENT));
        REQUIRE(test_parser("/* abc \n */", BLOCK_COMMENT));
        REQUIRE(test_parser("/* abc \n *\n */", BLOCK_COMMENT));
        // fail
        REQUIRE_FALSE(test_parser("/*abc*/*/", BLOCK_COMMENT));
        REQUIRE_FALSE(test_parser("/* /* abc */ */", BLOCK_COMMENT));
    }

    SECTION("Comment") {
        REQUIRE(test_parser("//abc", COMMENT));
        REQUIRE(test_parser("// abc", COMMENT));
        REQUIRE(test_parser("#abc", COMMENT));
        REQUIRE(test_parser("# abc", COMMENT));
        REQUIRE(test_parser("# abc \n", COMMENT >> x3::eol));
        REQUIRE(test_parser("# abc \n ", COMMENT >> x3::eol >> x3::space));

        REQUIRE(test_parser("/*abc*/", COMMENT));
        REQUIRE(test_parser("/* /* abc */", COMMENT));
        REQUIRE(test_parser("/** abc */", COMMENT));
        REQUIRE(test_parser("/* abc \n */", COMMENT));
        REQUIRE(test_parser("/* abc \n *\n */", COMMENT));
        // Fail
        REQUIRE_FALSE(test_parser("# abc \n", COMMENT));
        REQUIRE_FALSE(test_parser("# abc \n ", COMMENT));

        REQUIRE_FALSE(test_parser("/*abc*/*/", COMMENT));
        REQUIRE_FALSE(test_parser("/* /* abc */ */", COMMENT));
    }
}

// int main() {
//     // /* Double */
//     // test_parser("0.3", double_list);
//     // test_parser("0.3 0.3", double_list);
//     // test_parser("0.3 0.3", double_list);
//     // test_parser("0.3 0.3", double_list);
//     // test_parser("0.3 0.3, 0.3", double_list);

//     // test_parser("0.3,0.3", double_list);
//     // test_parser("0.3,0.3,0.3", double_list);
//     // test_parser("0.3 ,0.3, 0.3", double_list);
//     // test_parser("0.3, 0.3 , 0.3", double_list);
//     // test_parser("0.3  ,, 0.3 ,, 0.4", double_list);

//     // test_parser("3.0e+10", double_list);
//     // test_parser("3.0e10", double_list);
//     // test_parser("3.0e-10", double_list);
//     // test_parser("3e-10", double_list);
//     // // Fail
//     // test_parser("", double_list);
//     // test_parser("3.0+10", double_list);
//     // test_parser("0.30.3", double_list);
//     // test_parser("3.0e103.0e10", double_list);

//     // std::cout << "END" << std::endl;
// }