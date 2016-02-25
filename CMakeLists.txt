cmake_minimum_required(VERSION 3.3)

project(blaze)

include_directories("/nix/store/dilhxsy4nkswdq8r1dbbxwmy7k1rn75g-boost-1.58.0-dev/include")
link_directories("/nix/store/zj7wwm2fr632v7myx1wdf9kvw75g68r7-boost-1.58.0-lib/lib")

add_definitions("-std=c++11" "-Wno-parentheses")

add_executable(test_correctness
    main.cpp
    lexer.h
)

target_link_libraries(test_correctness boost_unit_test_framework boost_filesystem)

