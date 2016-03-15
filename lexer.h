#ifndef LEXER_H
#define LEXER_H

enum class token_type
{
    eof,
    identifier,
    numeral,
    slash,
    hash,
    hashhash,
    lpar,
    rpar,
    unknown,
};

bool is_identifier_start(char c)
{
    return c >= 'A' && c <= 'Z'
        || c >= 'a' && c <= 'z'
        || c == '_';
}

bool is_identifier_trail(char c)
{
    return c >= 'A' && c <= 'Z'
        || c >= 'a' && c <= 'z'
        || c >= '0' && c <= '9'
        || c == '_';
}

bool is_numeral_start(char c)
{
    return c >= '0' && c <= '9';
}

bool is_numeral_trail(char c)
{
    return is_identifier_trail(c);
}

bool is_whitespace(char c)
{
    return c <= ' ';
}

struct lexer
{
    lexer(char const* start, char const* end)
        : start(start)
        , end(end)
        , current(start)
        , newline(true)
    {
        skip_ws();
        fetch();
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
        return tok_end;
    }

    void next()
    {
        fetch();
    }

private:
    void fetch()
    {
    again:
        tok_start = current;
        if (current == end)
        {
            tok_type = token_type::eof;
            tok_end = current;
            return;
        }

        char c = *current;
        if (is_identifier_start(c))
        {
            tok_type = token_type::identifier;
            ++current;
            while (current != end && is_identifier_trail(*current))
                ++current;
        }
        else if (is_numeral_start(c))
        {
            tok_type = token_type::numeral;
            ++current;
            while (current != end && is_numeral_trail(*current))
                ++current;
        }
        else if (c == '/')
        {
            ++current;
            if (current == end)
            {
                tok_type = token_type::slash;
            }
            else
            {
                c = *current;
                if (c == '/')
                {
                    while (current != end && *current != '\n')
                        ++current;

                    if (current != end)
                        ++current;
                    skip_ws();
                    goto again;
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

                        if (*(current - 1) == '*')
                            break;
                    }

                    if (current != end)
                        ++current;
                    skip_ws();
                    goto again;
                }
                else
                {
                    tok_type = token_type::slash;
                }
            }
        }
        else if (c == '#')
        {
            ++current;
            if (current != end && *current == '#')
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
                    skip_ws();
                    goto again;
                }
            }
        }
        else if (c == '(')
        {
            tok_type = token_type::lpar;
            ++current;
        }
        else if (c == ')')
        {
            tok_type = token_type::rpar;
            ++current;
        }
        else
        {
            tok_type = token_type::unknown;
            ++current;
        }

        tok_end = current;
        newline = false;
        skip_ws();
    }

    void skip_ws()
    {
        for (;;)
        {
            if (current == end)
                break;

            char c = *current; 
            if (!is_whitespace(c))
                break;

            if (c == '\n')
                newline = true;

            ++current;
        }
    }

    void report_error_unfinished_multiline_comment()
    {

    }

    void report_error_stray_hash()
    {
        
    }

private:
    char const* start;
    char const* end;
    char const* current;
    token_type tok_type;
    char const* tok_start;
    char const* tok_end;
    bool newline;
};

#endif // LEXER_H

