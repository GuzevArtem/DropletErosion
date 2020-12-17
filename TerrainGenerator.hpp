#pragma once

#include <tuple>
#include <algorithm>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "PerlinNoise.hpp"
#include "Range.hpp"
#include "Grid.hpp"
#include "Terrain.hpp"

template<size_t terrain_channels, typename terrain_type>
class TerrainGenerator
{
        typedef glm::vec<terrain_channels, terrain_type, glm::defaultp> result_vec;

public:
    static inline double getPerlinNoiseValue(const siv::PerlinNoise perlinNoise, const uint32_t x, const uint32_t y, const double frequency, const uint32_t perlin_octaves_count)
    {
        const double value = perlinNoise.accumulatedOctaveNoise2D_0_1(x * 1.0 / frequency, y * 1.0 / frequency, perlin_octaves_count);
        return value;
    }

    static inline std::tuple<Grid<result_vec>, Grid<glm::f64vec3>> createPerlinNoiseAndNormal(const uint32_t x_size, uint32_t y_size,
                                                                                              const double min_height,
                                                                                              const double max_height,
                                                                                              const double pixel_to_meter_ratio_x = 1,
                                                                                              const double pixel_to_meter_ratio_y = 1,
                                                                                              const double frequency = 256,
                                                                                              const uint32_t perlin_octaves_count = 5,
                                                                                              const uint32_t seed = std::default_random_engine::default_seed)
    {
        siv::PerlinNoise perlinNoise{ seed };

        Grid<result_vec> terrain(x_size, y_size);

        const auto calculateNormalInPoint = [&perlinNoise, &terrain, min_height, max_height, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y, frequency, perlin_octaves_count](const uint32_t x, const uint32_t y) -> glm::f64vec3
        {
            const terrain_type value = TerrainGenerator::getPerlinNoiseValue(perlinNoise, x, y, frequency, perlin_octaves_count);
            const double diff = max_height - min_height;
            for (auto c = 0; c < terrain_channels; c++)
            {
                terrain.at(x, y)[c] = (value * diff) + min_height;
            }

            const double p10 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x + 1, y, frequency, perlin_octaves_count);
            const double p11 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y + 1, frequency, perlin_octaves_count);
            const double p01 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x - 1, y, frequency, perlin_octaves_count);
            const double p00 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y - 1, frequency, perlin_octaves_count);

            const glm::f64vec3 v1{ 2.0 / pixel_to_meter_ratio_x, 0.0, (p10 - p01) * diff };
            const glm::f64vec3 v2{ 0.0, 2.0 / pixel_to_meter_ratio_y, (p00 - p11) * diff };

            const glm::f64vec3 n = glm::cross(v1, v2);
            return glm::normalize(n);
        };

        Grid<glm::f64vec3> normalMap(x_size, y_size);
        //TODO: replace with parallel realization
        normalMap.for_each(calculateNormalInPoint);

        return { terrain, normalMap };
    }

    static inline Grid<result_vec> createPerlinNoise(const uint32_t x_size, uint32_t y_size,
                                                     const double min_height,
                                                     const double max_height,
                                                     const double frequency = 256,
                                                     const uint32_t perlin_octaves_count = 5,
                                                     const uint32_t seed = std::default_random_engine::default_seed)
    {
        siv::PerlinNoise perlinNoise{ seed };

        const auto calculatePerlinInPoint = [&perlinNoise, min_height, max_height, frequency, perlin_octaves_count](const uint32_t x, const uint32_t y) -> result_vec
        {
            result_vec result{};
            const terrain_type value = (terrain_type)TerrainGenerator::getPerlinNoiseValue(perlinNoise, x, y, frequency, perlin_octaves_count);
            for (auto c = 0; c < terrain_channels; c++)
            {
                const double diff = max_height - min_height;
                result[c] = (value * diff) + min_height;
            }
            return result;
        };

        Grid<result_vec> terrain(x_size, y_size);
        //TODO: replace with parallel realization
        terrain.for_each(calculatePerlinInPoint);

        return terrain;
    }

    static inline Terrain createPerlinNoiseTerrain (const uint32_t x_size, uint32_t y_size,
                                                    const double min_height,
                                                    const double max_height,
                                                    const double pixel_to_meter_ratio_x = 1,
                                                    const double pixel_to_meter_ratio_y = 1,
                                                    const double frequency = 256,
                                                    const uint32_t perlin_octaves_count = 5,
                                                    const uint32_t seed = std::default_random_engine::default_seed)
    {
        auto [height, normal] = createPerlinNoiseAndNormal (x_size, y_size,
                                                            min_height,
                                                            max_height,
                                                            pixel_to_meter_ratio_x,
                                                            pixel_to_meter_ratio_y,
                                                            frequency,
                                                            perlin_octaves_count,
                                                            seed);

        return Terrain(height, normal, min_height, max_height, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y);
    }
};

//TODO replace with concept for integral & real types
template<>
class TerrainGenerator<1, double>
{

public:
    static inline double getPerlinNoiseValue (const siv::PerlinNoise perlinNoise, const uint32_t x, const uint32_t y, const double frequency, const uint32_t perlin_octaves_count)
    {
        const double value = perlinNoise.accumulatedOctaveNoise2D_0_1 (x * 1.0 / frequency, y * 1.0 / frequency, perlin_octaves_count);
        return value;
    }

    static inline std::tuple<Grid<double>, Grid<glm::f64vec3>> createPerlinNoiseAndNormal (const uint32_t x_size, uint32_t y_size,
                                                                                               const double min_height,
                                                                                               const double max_height,
                                                                                               const double pixel_to_meter_ratio_x = 1,
                                                                                               const double pixel_to_meter_ratio_y = 1,
                                                                                               const double frequency = 256,
                                                                                               const uint32_t perlin_octaves_count = 5,
                                                                                               const uint32_t seed = std::default_random_engine::default_seed)
    {
        siv::PerlinNoise perlinNoise{ seed };

        Grid<double> terrain (x_size, y_size);

        const auto calculateNormalInPoint = [&perlinNoise, &terrain, min_height, max_height, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y, frequency, perlin_octaves_count](const uint32_t x, const uint32_t y) -> glm::f64vec3
        {
            const double value = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y, frequency, perlin_octaves_count);
            const double diff = max_height - min_height;

            terrain.assign_unchecked(x, y, (value * diff) + min_height);

            const double p10 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x + 1, y, frequency, perlin_octaves_count);
            const double p11 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y + 1, frequency, perlin_octaves_count);
            const double p01 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x - 1, y, frequency, perlin_octaves_count);
            const double p00 = TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y - 1, frequency, perlin_octaves_count);

            const glm::f64vec3 v1{ 2.0 / pixel_to_meter_ratio_x, 0.0, (p10 - p01) * diff };
            const glm::f64vec3 v2{ 0.0, 2.0 / pixel_to_meter_ratio_y, (p00 - p11) * diff };

            const glm::f64vec3 n = glm::cross (v1, v2);
            return glm::normalize (n);
        };

        Grid<glm::f64vec3> normalMap (x_size, y_size);
        //TODO: replace with parallel realization
        normalMap.for_each (calculateNormalInPoint);

        return { terrain, normalMap };
    }

    static inline Grid<double> createPerlinNoise (const uint32_t x_size, uint32_t y_size,
                                                      const double min_height,
                                                      const double max_height,
                                                      const double frequency = 256,
                                                      const uint32_t perlin_octaves_count = 5,
                                                      const uint32_t seed = std::default_random_engine::default_seed)
    {
        siv::PerlinNoise perlinNoise{ seed };

        const auto calculatePerlinInPoint = [&perlinNoise, min_height, max_height, frequency, perlin_octaves_count](const uint32_t x, const uint32_t y) -> double
        {
            const double value = (double)TerrainGenerator::getPerlinNoiseValue (perlinNoise, x, y, frequency, perlin_octaves_count);
            return (value * (max_height - min_height)) + min_height;
        };

        Grid<double> terrain (x_size, y_size);
        //TODO: replace with parallel realization
        terrain.for_each (calculatePerlinInPoint);

        return terrain;
    }

    static inline Terrain createPerlinNoiseTerrain (const uint32_t x_size, uint32_t y_size,
                                                    const double min_height,
                                                    const double max_height,
                                                    const double pixel_to_meter_ratio_x = 1,
                                                    const double pixel_to_meter_ratio_y = 1,
                                                    const double frequency = 256,
                                                    const uint32_t perlin_octaves_count = 5,
                                                    const uint32_t seed = std::default_random_engine::default_seed)
    {
        auto [height, normal] = createPerlinNoiseAndNormal (x_size, y_size,
                                                            min_height,
                                                            max_height,
                                                            pixel_to_meter_ratio_x,
                                                            pixel_to_meter_ratio_y,
                                                            frequency,
                                                            perlin_octaves_count,
                                                            seed);

        return Terrain (height, normal, min_height, max_height, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y);
    }
};
