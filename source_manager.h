#ifndef SOURCE_MANAGER_H
#define SOURCE_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>

struct file_record
{
    file_record(boost::filesystem::path absolute_path, std::vector<char> text);

    char const* text_begin() const;
    char const* text_end() const;

    boost::filesystem::path absolute_path;
    std::vector<char> text;
};

struct source_manager
{
    source_manager();

    file_record* load_file(boost::filesystem::path const& absolute_path);

private:
    std::map<boost::filesystem::path, std::unique_ptr<file_record>> files;
};

#endif // SOURCE_MANAGER_H
