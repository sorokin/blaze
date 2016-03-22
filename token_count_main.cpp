#include <iostream>
#include <cstdlib>

#include "test_utils.h"
#include "lexer.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " filename.cpp\n";
        return EXIT_SUCCESS;
    }

    std::vector<char> text = read_whole_file(argv[1]);
    size_t text_size = text.size();
    std::cout << "\t" << text_size << " bytes\t";
    text.push_back(guard_value);

    std::vector<size_t> counts((size_t)token_type::token_type_max);
    lexer lex(text.data(), text.data() + text.size() - 1);
    size_t tokens_total = 0;
    for (;;)
    {
        token_type tt = lex.tt();
        ++counts[(size_t)tt];
        ++tokens_total;
        if (tt == token_type::eof)
            break;

        lex.next();
    }

    std::cout << tokens_total << " tokens\t" << (double)text_size / tokens_total << " bytes/token\n";

    std::vector<token_type> tok_types((size_t)token_type::token_type_max);
    for (size_t i = 0, n = tok_types.size(); i != n; ++i)
        tok_types[i] = (token_type)i;

    std::sort(tok_types.begin(), tok_types.end(), [&counts](token_type a, token_type b) {
        return counts[(size_t)a] > counts[(size_t)b];
    });

    for (token_type tt : tok_types)
    {
        std::cout << "#" << (size_t)tt << "\t" << tt << "\t" << counts[(size_t)tt] << "\n";
    }

    return EXIT_SUCCESS;
}
