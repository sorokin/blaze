#include "test_utils.h"
#include "lexer.h"

#include <iostream>

#include <signal.h>

void disable_sigint()
{
    sigset_t base_mask;
    sigemptyset (&base_mask);
    sigaddset (&base_mask, SIGINT);
    sigprocmask (SIG_SETMASK, &base_mask, NULL);
}

bool is_sigint_pending()
{
    sigset_t waiting_mask;
    sigpending(&waiting_mask);
    return sigismember(&waiting_mask, SIGINT);
}

int main()
{
    disable_sigint();

    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<size_t> dist(1, 20);

    auto gen = std::bind(dist, mersenne_engine);

    size_t i = 0;
    for (;;)
    {
        if ((i % (16 * 1024)) == 0)
        {
            if (is_sigint_pending())
                break;
            std::cout << '.' << std::flush;
        }

        size_t size = gen();
        std::vector<char> input = random_vector(size, mersenne_engine);
        /*write_string_screened(std::cout, std::string(input.begin(), input.end()));
        std::cout << std::endl;*/

        input.push_back(guard_value);

        lexer lex(input.data(), input.data() + input.size() - 1);
        for (;;)
        {
            if (lex.tt() == token_type::eof)
                break;

            lex.next();
        }

        ++i;
    }

    std::cout << "run " << i << " times\n";
}
