#include "preprocessor.h"
#include "hashed.h"
#include "source_manager.h"

namespace
{
    static const hashed include_directive = hashed::literal("include");
}

sequencer::~sequencer()
{}

file_sequencer::file_sequencer(char const* start, char const* end)
    : lex(start, end)
{
    fetch();
}

token_type file_sequencer::tt()
{
    return tok.tok_type;
}

void file_sequencer::next()
{
    tok = lex.fetch(false);
    fetch();
}

void file_sequencer::fetch()
{
again:
    token_type tt = tok.tok_type;
    if (tt != token_type::hash)
    {
        tok.tok_type = tt;
        return;
    }

    //lex.skip_until_eoln();
    goto again;
}

xxx::xxx(file_record* file_rec)
{
    seqs.push_back(std::make_unique<file_sequencer>(file_rec->text_begin(), file_rec->text_end()));
}

token_type xxx::tt()
{
    return seqs.back()->tt();
}

void xxx::next()
{
    seqs.back()->next();
}

