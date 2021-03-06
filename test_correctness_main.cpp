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

#include "lex_file.h"
#include "lexer.h"
#include "test_utils.h"

#include <boost/optional.hpp>

namespace fs = boost::filesystem;

std::string run_test(std::vector<char> const& input)
{
    std::stringstream ss;

    lex_file(ss, input.data(), input.data() + input.size() - 1);

    return ss.str();
}

void run_test_on_file(fs::path const& filename)
{
    std::vector<char> input = read_whole_file(filename);
    input.push_back(guard_value);
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
    for (fs::directory_iterator i = fs::directory_iterator("./correctness_tests/"), end; i != end; ++i)
        if (i->path().extension() == ".txt")
        {
            boost::unit_test::framework::master_test_suite().
                    add( boost::unit_test::make_test_case( boost::bind(&run_test_on_file, i->path()), "run_test_on_file(\"" + i->path().string() + "\")", __FILE__, __LINE__));

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
