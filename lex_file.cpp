#include "lex_file.h"
#include "test_utils.h"
#include "lexer.h"

void lex_file(std::ostream& os, char const* start, char const* end)
{
    lexer lex(start, end);
    token tok = lex.fetch(true);
    for (;;)
    {
        os << (tok.tok_start   - start) << ':'
           << (lex.token_end() - start) << '\t';
        token_type tt = tok.tok_type;
        os << tt << '\t';
        write_string_screened(os, std::string(tok.tok_start, lex.token_end()));
        os << '\n';

        if (tt == token_type::eof)
            break;

        tok = lex.fetch(false);
    }
}
