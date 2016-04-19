#include "source_manager.h"
#include "test_utils.h"
#include "lexer.h"

namespace
{
    void invariant_trailing_guard_value(std::vector<char> const& v)
    {
        assert(!v.empty());
        assert(v.back() == guard_value);
    }
}

file_record::file_record(boost::filesystem::path absolute_path, std::vector<char> text)
    : absolute_path(std::move(absolute_path))
    , text(std::move(text))
{
    invariant_trailing_guard_value(this->text);
}

const char *file_record::text_begin() const
{
    return text.data();
}

const char *file_record::text_end() const
{
    invariant_trailing_guard_value(text);
    return text.data() + text.size() - 1;
}

source_manager::source_manager()
{}

file_record* source_manager::load_file(boost::filesystem::path const& absolute_path)
{
    auto i = files.find(absolute_path);
    if (i == files.end())
    {
        std::vector<char> text = read_whole_file(absolute_path);
        text.push_back(guard_value);
        decltype(files)::value_type p{absolute_path, std::make_unique<file_record>(absolute_path, std::move(text))};
        i = files.insert(i, std::move(p));
    }

    return i->second.get();
}
