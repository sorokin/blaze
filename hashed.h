#ifndef HASHED_H
#define HASHED_H

#include <cstdint>
#include <limits>
#include <cstring>
#include <functional>

#include "fnv.h"

struct hashed
{
    hashed();
    hashed(char const* start, char const* end, uint32_t hash);

    static hashed literal(char const*);

    char const* start;
    uint32_t size;
    uint32_t hash;
};

namespace std
{
    template <>
    struct hash<hashed>
    {
        size_t operator()(hashed a) const
        {
            return a.hash;
        }
    };
}

inline hashed::hashed()
{}

inline hashed::hashed(char const* start, char const* end, uint32_t hash)
    : start(start)
    , size(static_cast<uint32_t>(end - start))
    , hash(hash)
{
    std::ptrdiff_t bsize = end - start;
    assert(bsize >= 0);
    assert(bsize <= std::numeric_limits<uint32_t>::max());
}

inline hashed hashed::literal(char const* lit)
{
    char const* p = lit;
    fnv::accumulator acc;
    while (*p != '\0')
    {
        acc(*p);
        ++p;
    }

    return hashed(lit, p, acc.get_value());
}

inline bool operator==(hashed a, hashed b)
{
    if (a.hash != b.hash)
        return false;

    if (a.size != b.size)
        return false;

    return memcmp(a.start, b.start, a.size) == 0;
}

inline bool operator!=(hashed a, hashed b)
{
    if (a.hash != b.hash)
        return true;

    if (a.size != b.size)
        return true;

    return memcmp(a.start, b.start, a.size) != 0;
}

#endif

