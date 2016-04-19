#include "translation_unit.h"

#include "source_manager.h"

translation_unit::translation_unit(source_manager& src_man, const boost::filesystem::path &source_path)
{
    assert(source_path.is_absolute());
    src_man.load_file(source_path);
}
