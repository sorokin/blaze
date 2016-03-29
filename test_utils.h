#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <ostream>
#include <random>
#include <boost/filesystem/path.hpp>
#include <boost/optional/optional_fwd.hpp>

std::vector<char> read_whole_file(boost::filesystem::path const& filename);
boost::optional<std::vector<char>> read_whole_file_if_exists(boost::filesystem::path const& filename);
void write_string_screened(std::ostream& os, std::string const& str);
void write_entire_file(boost::filesystem::path const& filename, std::string const& data);

std::vector<char> random_vector(size_t size, std::mt19937& mersenne_engine);

#endif
