#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <vector>
#include <memory>

#include "lexer.h"

struct file_record;

struct sequencer
{
    virtual ~sequencer();
    virtual token_type tt() = 0;
    virtual void next() = 0;
};

struct file_sequencer : sequencer
{
    file_sequencer(char const* start, char const* end);

    token_type tt() override;
    void next() override;

private:
    void fetch();

private:
    lexer lex;
    token tok;
};

struct xxx
{
    xxx(file_record*);

    token_type tt();
    void next();

    void foo();

private:
    std::vector<std::unique_ptr<sequencer>> seqs;
};

#endif // PREPROCESSOR_H

