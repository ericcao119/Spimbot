cmake_minimum_required(VERSION 3.9)

option(USE_QT4 "Use QT4 instead of QT5" OFF) #OFF by default


project(QtSpimbot VERSION 0.1.0 LANGUAGES CXX)

# This will allow us to print a feature summary.
# https://cmake.org/cmake/help/v3.11/module/FeatureSummary.html
include(FeatureSummary)


# As of now we are not using ccache
# # We attempt to use ccache to speed up the build.
# find_program(CCACHE_FOUND "ccache")
# if(CCACHE_FOUND)
#     message(STATUS "Using ccache for building.")
#     set_property(GLOBAL PROPERTY LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
# endif(CCACHE_FOUND)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Find packages
add_subdirectory(libs/spdlog)
add_subdirectory(libs/catch2)
add_subdirectory(src/)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests/)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)