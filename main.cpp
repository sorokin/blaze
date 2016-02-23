#define BOOST_TEST_MODULE initd_config
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <chrono>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "lexer.h"

namespace fs = boost::filesystem;

std::vector<char> read_whole_file(fs::path const& filename)
{
    std::vector<char> data;

    FILE* f = fopen(filename.string().c_str(), "rb");
    if (!f)
        throw std::runtime_error("can not open file");

    fseek(f, 0, SEEK_END);
    data.resize(ftell(f));
    fseek(f, 0, SEEK_SET);

    fread(data.data(), 1, data.size(), f);

    fclose(f);
    return data;
}

void write_string_screened(std::ostream& os, std::string const& str)
{
    for (char c : str)
    {
        if (c >= ' ' && c < 0x7f)
            os << c;
        else
        {
            unsigned char uc = (unsigned char)c;
            os << std::hex << "\\x" << uc;
        }
    }
}

boost::optional<std::vector<char>> read_whole_file_if_exists(fs::path const& filename)
{
    std::vector<char> data;

    FILE* f = fopen(filename.string().c_str(), "rb");
    if (!f)
    {
        if (errno == ENOENT)
            return boost::none;

        throw std::runtime_error("can not open file");
    }

    fseek(f, 0, SEEK_END);
    data.resize(ftell(f));
    fseek(f, 0, SEEK_SET);

    fread(data.data(), 1, data.size(), f);

    fclose(f);
    return data;
}

void write_entire_file(fs::path const& filename, std::string const& data)
{
    FILE* f = fopen(filename.string().c_str(), "wb");
    if (!f)
        throw std::runtime_error("can not open file");

    fwrite(data.data(), 1, data.size(), f);

    fclose(f);
}

std::string run_test(std::vector<char> const& input)
{
    std::stringstream ss;

    lexer lex(input.data(), input.data() + input.size());
    for (;;)
    {
        ss << (lex.token_start() - lex.file_start()) << ':'
           << (lex.token_end() - lex.file_start()) << '\t';
        token_type tt = lex.tt();
        switch (tt)
        {
        case token_type::eof:
            ss << "eof";
            break;
        case token_type::identifier:
            ss << "ident";
            break;
        case token_type::whitespace:
            ss << "ws";
            break;
        case token_type::unknown:
            ss << "unknown";
            break;
        case token_type::numeral:
            ss << "numeral";
            break;
        default:
            assert(false);
            ss << "<error>";
            break;
        }
        ss << '\t';
        write_string_screened(ss, std::string(lex.token_start(), lex.token_end()));
        ss << '\n';

        if (tt == token_type::eof)
            break;

        lex.next();
    }

    return ss.str();
}

void run_test_on_file(fs::path const& filename)
{
    std::vector<char> input = read_whole_file(filename);
    std::string output = run_test(input);
    std::vector<char> output_data{output.begin(), output.end()};

    fs::path gold_filename = filename;
    gold_filename.replace_extension(".gold");

    fs::path out_filename = filename;
    out_filename.replace_extension(".out");

    boost::optional<std::vector<char> > gold_data = read_whole_file_if_exists(gold_filename);

    if (!gold_data)
    {
        if (!output_data.empty())
        {
            std::stringstream ss;
            ss << "missing gold file \"" << gold_filename << "\"\n"
               << "    less " << out_filename << "\n"
               << "    mv " << out_filename << ' ' << gold_filename;
            BOOST_CHECK_MESSAGE(false, ss.str());
            write_entire_file(out_filename, output);
        }
        return;
    }

    if (output_data != *gold_data)
    {
        std::stringstream ss;
        ss << "gold/output mismatch\n"
           << "    diff " << gold_filename << " " << out_filename << " | colordiff\n"
           << "    mv " << out_filename << " " << gold_filename;

        BOOST_CHECK_MESSAGE(false, ss.str());
        write_entire_file(out_filename, output);
    }
}

bool init_function()
{
    for (fs::directory_iterator i = fs::directory_iterator("./tests/"), end; i != end; ++i)
        if (i->path().extension() == ".txt")
        {
            boost::unit_test::framework::master_test_suite().
                    add( boost::unit_test::make_test_case( boost::unit_test::callback0<>(boost::bind(&run_test_on_file, i->path())), "run_test_on_file(\"" + i->path().string() + "\")"));

        }

    return true;
}

int main( int argc, char* argv[] )
{
    return ::boost::unit_test::unit_test_main( &init_function, argc, argv );
}

/*
int main()
{
    std::vector<char> input = read_whole_file("test.input");

    for (size_t i = 0; i != 10; ++i)
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        lexer lex(input.data(), input.data() + input.size());
        while (lex.tt() != token_type::eof)
            lex.next();

        auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "lexing has taken " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "us\n";
    }

    return 0;
}

*/
