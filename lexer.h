#ifndef LEXER_H
#define LEXER_H

enum class token_type
{
    eof,
    identifier,
    whitespace,
    numeral,
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
    {
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
        return current;
    }

    void next()
    {
        fetch();
    }

private:
    void fetch()
    {
        tok_start = current;
        if (current == end)
        {
            tok_type = token_type::eof;
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
        else if (is_whitespace(c))
        {
            tok_type = token_type::whitespace;
            ++current;
            while (current != end && is_whitespace(*current))
                ++current;
        }
        else if (is_numeral_start(c))
        {
            tok_type = token_type::numeral;
            ++current;
            while (current != end && is_numeral_trail(*current))
                ++current;
        }
        else
        {
            tok_type = token_type::unknown;
            ++current;
        }
    }

private:
    char const* start;
    char const* end;
    char const* current;
    token_type tok_type;
    char const* tok_start;
};

#endif // LEXER_H
