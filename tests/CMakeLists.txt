add_executable(tests
    test_main.cpp
    test_turn.cpp

    # Parser ---
    test_parser/test_parser.h
    test_parser/test_primitives/test_comment.cpp
    test_parser/test_primitives/test_register.cpp
    test_parser/test_primitives/test_expression.cpp
)
add_test(NAME SpimbotTests COMMAND tests)

message(STATUS "include dir: ${CMAKE_SOURCE_DIR}")
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# target_compile_definitions(QtSpimbot)
target_compile_features(tests PUBLIC cxx_std_17)
target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(tests PRIVATE spdlog Catch2::Catch2)
# target_link_libraries(QtSpimbot Qt5::Widgets)