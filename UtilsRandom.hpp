#pragma once

#include <random>

namespace utils_random
{

    [[nodiscard]]
    static inline uint64_t hashSeed (const uint64_t seed, const uint32_t x, const uint32_t y, const uint32_t iteration)
    {
        return seed ^ ((uint64_t)x << 32 | (uint64_t)y) ^ ((uint64_t)iteration << 48 | (uint64_t)iteration << 24 | (uint64_t)iteration);
    }

    [[nodiscard]]
    static inline std::default_random_engine getRandGenerator (const uint64_t seed, const uint32_t x, const uint32_t y, const uint32_t iteration)
    {
        std::seed_seq ssq{ hashSeed (seed, x, y, iteration) };
        std::default_random_engine eng{ ssq };
        return eng;
    }

    template<typename T, typename random_engine>
    [[nodiscard]]
    static inline T rand (random_engine eng, const T from, const T to)
    {
        if constexpr ( std::is_floating_point<T>::value )
        {
            std::uniform_real_distribution<T> urd (from, to);
            return urd (eng);
        }
        else
        {
            std::uniform_int_distribution<T> urd (from, to);
            return urd (eng);
        }
    }

    template<typename T>
    [[nodiscard]]
    T rand (const uint32_t x, const uint32_t y, const uint32_t iteration, const T from, const T to, uint32_t seed) noexcept
    {
        std::default_random_engine eng = getRandGenerator (seed, x, y, iteration);
        return rand<T> (&eng, from, to);
    }
}
