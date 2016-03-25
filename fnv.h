#ifndef FNV_H
#define FNV_H

#include <cstdint>

namespace fnv
{
    uint32_t const offset_basis = 0x811c9dc5u;
    uint32_t const prime        = 0x01000193u;

    struct accumulator
    {
        accumulator()
            : value(offset_basis)
        {}

        void operator()(uint8_t octet)
        {
            value ^= octet;
            value *= prime;
        }

        uint32_t get_value() const
        {
            return value;
        }

    private:
        uint32_t value;
    };
}

#endif
