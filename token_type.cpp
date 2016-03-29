#include "token_type.h"
#include <cassert>

std::ostream& operator<<(std::ostream& stream, token_type tt)
{
    switch (tt)
    {
#define TT(x) { case token_type::x: stream << #x; break; }
        TT(eof);
        TT(ident);
        TT(numeral);
        TT(char_literal);
        TT(string_literal);
        TT(slash);
        TT(hash);
        TT(hashhash);
        TT(lpar);
        TT(rpar);
        TT(comma);
        TT(lbracket);
        TT(rbracket);
        TT(lbrace);
        TT(rbrace);
        TT(eq);
        TT(plus);
        TT(minus);
        TT(star);
        TT(plus_plus);
        TT(minus_minus);
        TT(minus_greater);
        TT(plus_eq);
        TT(minus_eq);
        TT(star_eq);
        TT(slash_eq);
        TT(eq_eq);
        TT(excl);
        TT(excl_eq);
        TT(amp);
        TT(amp_amp);
        TT(amp_eq);
        TT(pipe);
        TT(pipe_pipe);
        TT(pipe_eq);
        TT(tilde);
        TT(percent);
        TT(percent_eq);
        TT(question);
        TT(less);
        TT(less_less);
        TT(less_less_eq);
        TT(less_eq);
        TT(greater);
        TT(greater_greater);
        TT(greater_greater_eq);
        TT(greater_eq);
        TT(colon);
        TT(colon_colon);
        TT(semicolon);
        TT(minus_greater_star);
        TT(dot);
        TT(dot_star);
        TT(ellipsis);
        TT(circumflex);
        TT(circumflex_eq);
#define T(x) TT(x##_keyword)
        ALL_C_KEYWORDS
#undef T
        TT(unknown);
#undef TT
    default:
        assert(false);
        stream << "<error>";
        break;
    }

    return stream;
}
