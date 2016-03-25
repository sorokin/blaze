cmake_minimum_required(VERSION 3.3)

project(blaze)

include_directories("/nix/store/dilhxsy4nkswdq8r1dbbxwmy7k1rn75g-boost-1.58.0-dev/include")
link_directories("/nix/store/zj7wwm2fr632v7myx1wdf9kvw75g68r7-boost-1.58.0-lib/lib")

add_definitions("-std=c++11" "-Wno-parentheses")

add_library(common STATIC
    test_utils.h
    test_utils.cpp
    lexer.h
    fnv.h
    hashed.h
)

add_executable(test_correctness
    test_correctness_main.cpp
)

target_link_libraries(test_correctness common boost_unit_test_framework boost_filesystem)

add_executable(test_performance
    test_performance_main.cpp
)

target_link_libraries(test_performance common boost_unit_test_framework boost_filesystem)

add_executable(token_count
    token_count_main.cpp
)

target_link_libraries(token_count common boost_filesystem)
