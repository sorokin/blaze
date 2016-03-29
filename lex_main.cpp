#include <iostream>
#include "lex_file.h"
#include "lexer.h"
#include "test_utils.h"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cerr << argv[0] << ": no input files\n";
        return EXIT_FAILURE;
    }

    for (int i = 1; i != argc; ++i)
    {
        std::vector<char> input = read_whole_file(argv[1]);
        input.push_back(guard_value);
        lex_file(std::cout, input.data(), input.data() + input.size() - 1);
    }

    return EXIT_SUCCESS;
}
