#ifndef LEXER_H
#define LEXER_H

#include <cassert>
#include <ostream>
#include <unordered_map>
#include "fnv.h"
#include "token_type.h"

inline bool is_identifier_trail(char c)
{
#define IDENTIFIER_TRAIL_IMPLEMENTATION 3
#if IDENTIFIER_TRAIL_IMPLEMENTATION == 0
    return c >= 'A' && c <= 'Z'
        || c >= 'a' && c <= 'z'
        || c >= '0' && c <= '9'
        || c == '_';
#elif IDENTIFIER_TRAIL_IMPLEMENTATION == 1
    return !(c < '0'
          || c > '9' && c < 'A'
          || c > 'Z' && c < '_'
          || c > 'z'
          || c == '`');
#elif IDENTIFIER_TRAIL_IMPLEMENTATION == 2
    int cc =(int)c;
    int isnt_small_letter = (cc - 'a') | ('z' - cc);
    int isnt_capital_letter = (cc - 'A') | ('Z' - cc);
    int isnt_digit = (cc - '0') | ('9' - cc);
    int isnt_underscore = (cc - '_') | ('_' - cc);
    return bool(~(isnt_small_letter
               & isnt_capital_letter
               & isnt_digit
               & isnt_underscore) >> 31);
#elif IDENTIFIER_TRAIL_IMPLEMENTATION == 3
    static bool const identifier_trails[256] =
    {
        false, false, false, false, false, false, false, false, // 00
        false, false, false, false, false, false, false, false, // 08
        false, false, false, false, false, false, false, false, // 10
        false, false, false, false, false, false, false, false, // 18
        false, false, false, false, false, false, false, false, // 20
        false, false, false, false, false, false, false, false, // 28
        true , true , true , true , true , true , true , true , // 30
        true , true , false, false, false, false, false, false, // 38
        false, true , true , true , true , true , true , true , // 40
        true , true , true , true , true , true , true , true , // 48
        true , true , true , true , true , true , true , true , // 50
        true , true , true , false, false, false, false, true , // 58
        false, true , true , true , true , true , true , true , // 60
        true , true , true , true , true , true , true , true , // 68
        true , true , true , true , true , true , true , true , // 70
        true , true , true , false, false, false, false, false, // 78
        false, false, false, false, false, false, false, false, // 80
        false, false, false, false, false, false, false, false, // 88
        false, false, false, false, false, false, false, false, // 90
        false, false, false, false, false, false, false, false, // 98
        false, false, false, false, false, false, false, false, // a0
        false, false, false, false, false, false, false, false, // a8
        false, false, false, false, false, false, false, false, // b0
        false, false, false, false, false, false, false, false, // b8
        false, false, false, false, false, false, false, false, // c0
        false, false, false, false, false, false, false, false, // c8
        false, false, false, false, false, false, false, false, // d0
        false, false, false, false, false, false, false, false, // d8
        false, false, false, false, false, false, false, false, // e0
        false, false, false, false, false, false, false, false, // e8
        false, false, false, false, false, false, false, false, // f0
        false, false, false, false, false, false, false, false, // f8
    };

    return identifier_trails[(unsigned char)c];
#elif IDENTIFIER_TRAIL_IMPLEMENTATION == 4
    static unsigned const identifier_trails[] =
    {
        0x00000000, // 00
        0x03ff0000, // 20
        0x87fffffe, // 40
        0x07fffffe, // 60
        0x00000000, // 80
        0x00000000, // a0
        0x00000000, // c0
        0x00000000, // e0
    };

    unsigned char cc = (unsigned char)c;
    return identifier_trails[cc >> 5] & (1 << (cc & 31));
#else
#error "invalid value of IDENTIFIER_TRAIL_IMPLEMENTATION"
#endif
}

inline bool is_numeral_trail(char c)
{
    return is_identifier_trail(c);
}

inline bool is_whitespace(char c)
{
    return c <= ' ';
}

#include "hashed.h"

typedef std::unordered_map<hashed, token_type> identifier_map_t;

inline identifier_map_t calc_identifier_map()
{
    identifier_map_t result
    {
#define T(x) {hashed::literal(#x), token_type::x##_keyword},
        ALL_C_KEYWORDS
#undef T
    };

    return result;
}

static identifier_map_t identifiers = calc_identifier_map();

const char guard_value = '`';

struct lexer
{
    lexer(char const* start, char const* end)
        : start(start)
        , end(end)
        , current(start)
        , line_number(0)
    {
        assert(*end == guard_value);
        fetch(true);
    }

    token_type tt() const
    {
        return tok_type;
    }

    char const* file_start() const
    {
        return start;
    }

    char const* file_end() const
    {
        return end;
    }

    char const* token_start() const
    {
        return tok_start;
    }

    char const* token_end() const
    {
        return current;
    }

    void next()
    {
        fetch(false);
    }

private:
    void fetch(bool newline) __attribute__((noinline))
    {
    again_with_ws:
        skip_ws(newline);
        tok_start = current;

        char c = *current;
        switch (c)
        {
        case 'a'...'z':
        case 'A'...'Z':
        case '_':
            {
                //fnv::accumulator acc;
                //acc(c);
                char const* curr = current;
                ++curr;
                while (is_identifier_trail(*curr))
                {
                    //acc(*curr);
                    ++curr;
                }
                current = curr;
                //identifier_hash = acc.get_value();

                //auto i = identifiers.find(hashed(tok_start, current, acc.get_value()));
                //if (i == identifiers.end())
                    tok_type = token_type::ident;
                //else
                //    tok_type = i->second;
                break;
            }
        case '0'...'9':
            {
                tok_type = token_type::numeral;
                ++current;
                while (/*current != end && */is_numeral_trail(*current))
                    ++current;
                break;
            }
        case '/':
            {
                ++current;
                c = *current;
                if (c == '/')
                {
                    while (current != end && *current != '\n')
                        ++current;

                    if (current != end)
                        ++current;
                    goto again_with_ws;
                }
                else if (c == '*')
                {
                    ++current;

                    if (current != end)
                        ++current;

                    for (;;)
                    {
                        if (current == end)
                        {
                            report_error_unfinished_multiline_comment();
                            break;
                        }

                        while (current != end && *current != '/')
                            ++current;

                        if (current == end)
                        {
                            report_error_unfinished_multiline_comment();
                            break;
                        }

                        if (*(current - 1) == '*')
                            break;
                        else
                            ++current;
                    }

                    if (current != end)
                        ++current;
                    goto again_with_ws;
                }
                else if (c == '=')
                {
                    tok_type = token_type::slash_eq;
                    ++current;
                }
                else
                {
                    tok_type = token_type::slash;
                }
                break;
            }
        case '#':
            {
                ++current;
                if (/*current != end && */*current == '#')
                {
                    tok_type = token_type::hashhash;
                    ++current;
                }
                else
                {
                    if (newline)
                        tok_type = token_type::hash;
                    else
                    {
                        report_error_stray_hash();
                        goto again_with_ws;
                    }
                }
                break;
            }
        case '(':
            {
                tok_type = token_type::lpar;
                ++current;
                break;
            }
        case ')':
            {
                tok_type = token_type::rpar;
                ++current;
                break;
            }
        case ',':
            {
                tok_type = token_type::comma;
                ++current;
                break;
            }
        case '[':
            {
                tok_type = token_type::lbracket;
                ++current;
                break;
            }
        case ']':
            {
                tok_type = token_type::rbracket;
                ++current;
                break;
            }
        case '{':
            {
                tok_type = token_type::lbrace;
                ++current;
                break;
            }
        case '}':
            {
                tok_type = token_type::rbrace;
                ++current;
                break;
            }
        case '=':
            {
                ++current;
                if (*current != '=')
                    tok_type = token_type::eq;
                else
                {
                    tok_type = token_type::eq_eq;
                }
                break;
            }
        case '+':
            {
                ++current;
                c = *current;
                if (c == '+')
                {
                    tok_type = token_type::plus_plus;
                    ++current;
                }
                else if (c == '=')
                {
                    tok_type = token_type::plus_eq;
                    ++current;
                }
                else
                    tok_type = token_type::plus;
                break;
            }
        case '-':
            {
                ++current;
                c = *current;
                if (c == '-')
                {
                    tok_type = token_type::minus_minus;
                    ++current;
                }
                else if (c == '=')
                {
                    tok_type = token_type::minus_eq;
                    ++current;
                }
                else if (c == '>')
                {
                    ++current;
                    if (*current != '*')
                        tok_type = token_type::minus_greater;
                    else
                    {
                        tok_type = token_type::minus_greater_star;
                        ++current;
                    }
                }
                else
                {
                    tok_type = token_type::minus;
                }
                break;
            }
        case '*':
            {
                ++current;
                c = *current;
                if (c != '=')
                    tok_type = token_type::star;
                else
                {
                    tok_type = token_type::star_eq;
                    ++current;
                }
                break;
            }
        case '!':
            {
                ++current;
                c = *current;
                if (c != '=')
                    tok_type = token_type::excl;
                else
                {
                    tok_type = token_type::excl_eq;
                    ++current;
                }
                break;
            }
        case guard_value:
            {
                if (current == end)
                {
                    tok_type = token_type::eof;
                    //tok_end = current;
                    return;
                }
                else
                {
                    tok_type = token_type::unknown;
                    ++current;
                    break;
                }
            }
        case '&':
            {
                ++current;
                c = *current;
                if (c == '&')
                {
                    tok_type = token_type::amp_amp;
                    ++current;
                }
                else if (c == '=')
                {
                    tok_type = token_type::amp_eq;
                    ++current;
                }
                else
                    tok_type = token_type::amp;
                break;
            }
        case '|':
            {
                ++current;
                c = *current;
                if (c == '|')
                {
                    tok_type = token_type::pipe_pipe;
                    ++current;
                }
                else if (c == '=')
                {
                    tok_type = token_type::pipe_eq;
                    ++current;
                }
                else
                    tok_type = token_type::pipe;
                break;
            }
        case '~':
            {
                ++current;
                tok_type = token_type::tilde;
                break;
            }
        case '%':
            {
                ++current;
                c = *current;
                if (c != '=')
                    tok_type = token_type::percent;
                else
                {
                    tok_type = token_type::percent_eq;
                    ++current;
                }
                break;
            }
        case '?':
            {
                ++current;
                tok_type = token_type::question;
                break;
            }
        case '<':
            {
                ++current;
                c = *current;
                if (c == '<')
                {
                    ++current;
                    if (*current != '=')
                        tok_type = token_type::less_less;
                    else
                    {
                        tok_type = token_type::less_less_eq;
                        ++current;
                    }
                }
                else if (c == '=')
                {
                    tok_type = token_type::less_eq;
                    ++current;
                }
                else
                    tok_type = token_type::less;
                break;
            }
        case '>':
            {
                ++current;
                c = *current;
                if (c == '>')
                {
                    ++current;
                    if (*current != '=')
                        tok_type = token_type::greater_greater;
                    else
                    {
                        tok_type = token_type::greater_greater_eq;
                        ++current;
                    }
                }
                else if (c == '=')
                {
                    tok_type = token_type::greater_eq;
                    ++current;
                }
                else
                    tok_type = token_type::greater;
                break;
            }
        case ':':
            {
                ++current;
                if (*current == ':')
                {
                    ++current;
                    tok_type = token_type::colon_colon;
                }
                else
                    tok_type = token_type::colon;
                break;
            }
        case ';':
            {
                ++current;
                tok_type = token_type::semicolon;
                break;
            }
        case '.':
            {
                ++current;
                c = *current;
                if (c == '*')
                {
                    tok_type = token_type::dot_star;
                    ++current;
                }
                else if (c == '.' && *(current + 1) == '.')
                {
                    tok_type = token_type::ellipsis;
                    current += 2;
                }
                else
                {
                    tok_type = token_type::dot;
                }
                break;
            }
        case '^':
            {
                ++current;
                if (*current != '=')
                    tok_type = token_type::circumflex;
                else
                {
                    tok_type = token_type::circumflex_eq;
                    ++current;
                }
                break;
            }
        case '\'':
            {
                ++current;
                while (current != end && *current != '\'')
                    ++current;

                if (current != end)
                    ++current;
                else
                    report_unfinished_literal();

                tok_type = token_type::char_literal;
                break;
            }
        case '"':
            {
                ++current;
                while (current != end && *current != '"')
                    ++current;

                if (current != end)
                    ++current;
                else
                    report_unfinished_literal();

                tok_type = token_type::string_literal;
                break;
            }
        default:
            {
                tok_type = token_type::unknown;
                ++current;
                break;
            }
        }

        //tok_end = current;
    }

    void skip_ws(bool newline)
    {
        char const* curr = current;
        char c = *current;
        if (!is_whitespace(c))
            return;

        for (;;)
        {
            if (c == '\n')
            {
                newline = true;
                ++line_number;
            }

            ++curr;
            c = *curr;
            if (!is_whitespace(c))
                break;
        }
        current = curr;
    }

    void report_error_unfinished_multiline_comment()
    {

    }

    void report_error_stray_hash()
    {
        
    }

    void report_unfinished_literal()
    {

    }

private:
    char const* start;
    char const* end;
    char const* current;
    token_type tok_type;
    char const* tok_start;
    //char const* tok_end;
    size_t line_number;
    union
    {
        struct
        {
            uint32_t identifier_hash;
        };
    };
};

#endif // LEXER_H
