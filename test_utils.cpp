#include "test_utils.h"

#include <stdexcept>
#include <iomanip>
#include <boost/optional.hpp>

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

void write_string_screened(std::ostream& os, std::string const& str)
{
    for (char c : str)
    {
        if (c >= ' ' && c < 0x7f)
            os << c;
        else
        {
            os << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (unsigned)(unsigned char)c;
        }
    }
}

void write_entire_file(fs::path const& filename, std::string const& data)
{
    FILE* f = fopen(filename.string().c_str(), "wb");
    if (!f)
        throw std::runtime_error("can not open file");

    fwrite(data.data(), 1, data.size(), f);

    fclose(f);
}
