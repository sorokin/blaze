#include <iostream>
#include <boost/filesystem.hpp>

#include "source_manager.h"
#include "preprocessor.h"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cerr << argv[0] << ": no input files\n";
        return EXIT_FAILURE;
    }

    try
    {
        source_manager src_man;
        for (int i = 1; i != argc; ++i)
        {
            auto abs_path = boost::filesystem::canonical(argv[i]);
            std::cout << abs_path << "\n";
            file_record* file_rec = src_man.load_file(abs_path.string());

            xxx pp(file_rec);
            for (;;)
            {
                token_type tt = pp.tt();
                std::cout << tt << '\n';

                if (tt == token_type::eof)
                    break;

                pp.next();
            }
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;

}
