#include "lex_file.h"
#include "test_utils.h"
#include "lexer.h"

void lex_file(std::ostream& os, char const* start, char const* end)
{
    lexer lex(start, end);
    for (;;)
    {
        os << (lex.token_start() - lex.file_start()) << ':'
           << (lex.token_end() - lex.file_start()) << '\t';
        token_type tt = lex.tt();
        os << tt << '\t';
        write_string_screened(os, std::string(lex.token_start(), lex.token_end()));
        os << '\n';

        if (tt == token_type::eof)
            break;

        lex.next();
    }
}
