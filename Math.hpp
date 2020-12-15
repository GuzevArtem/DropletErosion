#pragma once

#ifndef UTILS_MATH_H
#define UTILS_MATH_H


#define _USE_MATH_DEFINES
#include <math.h> 
#include <array>

namespace math
{
    inline double gauss(const double x, const double sigma, const double mu) noexcept
    {
        return std::exp(-(std::pow(x - mu, 2) / (2 * sigma * sigma))) / (sigma * std::sqrt(2 * M_PI));
    }

    template<typename T, size_t N>
    inline constexpr T lengthSquared(const std::array<T, N> array)
    {
        T acc = {};
        for (auto it = array.begin(); it != array.end(); it++)
        {
            acc += (*it) * (*it);
        }
        return acc;
    }

    template<typename T, size_t N>
    inline constexpr T length(const std::array<T, N> array)
    {
        const T lengthSquare = lengthSquared<T, N>(array);
        return std::sqrt(lengthSquare);
    }

    template<typename T, size_t N>
    inline constexpr std::array<T, N> normalize(std::array<T, N> array)
    {
        T length = math::length(array);
        for (auto it = array.begin(); it != array.end(); it++)
        {
            *it /= length;
        }
        return array;
    }

    inline constexpr std::array<double, 2> grad(const std::array<std::array<double, 3>, 3> array) noexcept
    {
        double x = array[0][0] + array[0][1] + array[0][2]
            - array[2][0] - array[2][1] - array[2][2];
        double y = array[0][0] + array[0][1] + array[0][2]
            - array[2][0] - array[2][1] - array[2][2];

        return { -x, -y };
    }
}
#endif // !UTILS_MATH_H