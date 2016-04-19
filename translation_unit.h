#ifndef TRANSLATION_UNIT_H
#define TRANSLATION_UNIT_H

#include <boost/filesystem/path.hpp>

struct file_record;
struct source_manager;

struct translation_unit
{
    translation_unit(source_manager& src_man, boost::filesystem::path const& source_path);
};

#endif // TRANSLATION_UNIT_H
