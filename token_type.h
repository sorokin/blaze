#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include "keywords.h"
#include <ostream>

enum class token_type
{
    eof,
    ident,
    numeral,
    char_literal,
    string_literal,
    slash,
    hash,
    hashhash,
    lpar,
    rpar,
    comma,
    lbracket,
    rbracket,
    lbrace,
    rbrace,
    eq,
    plus,
    minus,
    star,
    plus_plus,
    minus_minus,
    minus_greater,
    plus_eq,
    minus_eq,
    star_eq,
    slash_eq,
    eq_eq,
    excl,
    excl_eq,
    amp,
    amp_amp,
    amp_eq,
    pipe,
    pipe_pipe,
    pipe_eq,
    tilde,
    percent,
    percent_eq,
    question,
    less,
    less_less,
    less_less_eq,
    less_eq,
    greater,
    greater_greater,
    greater_greater_eq,
    greater_eq,
    colon,
    colon_colon,
    semicolon,
    minus_greater_star,
    dot,
    dot_star,
    ellipsis,
    circumflex,
    circumflex_eq,

#define T(x) x##_keyword,
    ALL_C_KEYWORDS
#undef T

    unknown,

    token_type_max
};

std::ostream& operator<<(std::ostream& stream, token_type tt);

#endif
