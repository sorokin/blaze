#include "lexer.h"
#include "test_utils.h"
#include <iostream>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace fs = boost::filesystem;

template <typename T>
void accumulate_min(boost::optional<T>& accum, T const& newval)
{
    if (accum)
        *accum = std::min(*accum, newval);
    else
        accum = newval;
}

template <typename T>
void accumulate_max(boost::optional<T>& accum, T const& newval)
{
    if (accum)
        *accum = std::max(*accum, newval);
    else
        accum = newval;
}

void print_time(std::ostream& os, std::chrono::high_resolution_clock::duration dur)
{
    os << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << "us";
}

struct histogram_accumulator
{
    typedef std::chrono::high_resolution_clock::duration value_type;

    struct bar
    {
        value_type lower_bound;
        size_t number_of_points;
    };

    struct histogram
    {
        std::vector<bar> bars;
        value_type upper_bound;
        size_t number_of_points;
        size_t max_number_of_points;
    };

    void push(value_type dur)
    {
        values.push_back(dur);
    }

    size_t size() const
    {
        return values.size();
    }

    bool empty() const
    {
        return values.empty();
    }

    histogram build(double step = 0.02) const
    {
        assert(!empty());
        std::sort(values.begin(), values.end());

        auto global_lower_bound = values.front();
        auto one_bar_size       = values.front() * step;

        size_t max_number_of_points = 0;
        std::vector<bar> result;
        for (auto i = values.begin(); i != values.end();)
        {
            size_t bar_index = result.size();
            auto lower_bound = global_lower_bound + one_bar_size * bar_index;
            auto upper_bound = global_lower_bound + one_bar_size * (bar_index + 1);
            size_t n = 0;

            while (i != values.end() && *i < upper_bound) // TODO: binary search?
            {
                ++n;
                ++i;
            }

            result.push_back(bar{std::chrono::duration_cast<value_type>(lower_bound), n});
            max_number_of_points = std::max(max_number_of_points, n);
        }

        auto upper_bound = global_lower_bound + one_bar_size * result.size();
        return histogram{std::move(result), std::chrono::duration_cast<value_type>(upper_bound), values.size(), max_number_of_points};
    }

private:
    mutable std::vector<value_type> values;
};

void print_histogram(histogram_accumulator::histogram const& hist, size_t width = 140)
{
    double chars_per_point = (double)width / hist.max_number_of_points;
    for (auto const& bar : hist.bars)
    {
        print_time(std::cout, bar.lower_bound);
        std::cout << ' ';
        size_t nchars = size_t(chars_per_point * bar.number_of_points);
        for (size_t i = 0; i != nchars; ++i)
            std::cout << '#';
        std::cout << " (" << bar.number_of_points << ")";
        std::cout << '\n';
    }
    print_time(std::cout, hist.upper_bound);
}

void run_test_on_file(fs::path const& filename)
{
    std::vector<char> input = read_whole_file(filename);
    input.push_back(guard_value);

    std::cout << filename << '\t' << input.size() << " bytes" << "\n";

    histogram_accumulator acc;
    size_t const N = 10000;
    for (size_t i = 0; i != N; ++i)
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        lexer lex(input.data(), input.data() + input.size() - 1);
        token tok = lex.fetch(true);
        while (tok.tok_type != token_type::eof)
            tok = lex.fetch(false);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto dur = end_time - start_time;
        //print_time(std::cout, dur);
        //std::cout << '\t';
        acc.push(dur);
    }
    print_histogram(acc.build());
    /*std::cout << "\n\tmin = ";
    print_time(std::cout, *min);
    std::cout << " max = ";
    print_time(std::cout, *max);
    std::cout << " avg = ";
    print_time(std::cout, sum / N);
    std::cout << " MB/s = " << (double)input.size() / std::chrono::duration_cast<std::chrono::microseconds>(sum).count() * N;
    std::cout << '\n';*/
}

int main()
{
    for (fs::directory_iterator i = fs::directory_iterator("./performance_tests/"), end; i != end; ++i)
        if (i->path().extension() == ".txt")
            run_test_on_file(i->path());
}
