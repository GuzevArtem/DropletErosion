#include <iostream>
#include <stdint.h>

#include "StaticConfig.hpp"

#include "Utils.hpp"
#include "GridToOpenCVConverter.hpp"

#include "RngService.hpp"

#include "WindowNames.hpp"
#include "TerrainGenerator.hpp"
#include "NormapMapGenerator.hpp"

#include "Grid.hpp"
#include "Droplet.hpp"

//TODO move random

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
static inline T rand (random_engine& eng, const T from, const T to)
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
    return rand<T> (eng, from, to);
}

//TODO move droplet generation

inline std::vector<Droplet> spawnDroplets (uint32_t count, uint32_t max_x, uint32_t max_y, Grid<glm::f64vec1> heightMap, uint32_t seed)
{
    std::vector<Droplet> new_droplets;
    std::seed_seq ssq{ seed };
    std::default_random_engine eng{ ssq };
    for ( uint32_t i = 0; i < count; i++ )
    {
        const uint32_t x = rand (eng, (uint32_t)0, max_x);
        const uint32_t y = rand (eng, (uint32_t)0, max_y);

        const double z = heightMap.at_unchecked (x, y) [0];

        const glm::f64vec3 pos{ (double) x, (double) y, z };
        Droplet& d = new_droplets.emplace_back ();
        d.pos = pos;
        d.volume = 1.0;
        //TODO: other properties
    }
    return new_droplets;
}

Grid<glm::f64vec1> map_droplets (const std::vector<Droplet>& droplets, uint32_t size_x, uint32_t size_y)
{
    Grid<glm::f64vec1> water (size_x, size_y);

    for ( const auto& d : droplets )
    {
        water.assign((uint32_t)d.pos.x, (uint32_t)d.pos.y, water.at ((uint32_t)d.pos.x, (uint32_t)d.pos.y) + d.volume);
    }

    return water;
}

#include <limits>

int main ()
{
    std::cout << "Start\n";

    std::seed_seq ss{ 5479U };

    RNGService<uint32_t> serv;

    serv.create_sequence ("default", ss, 0, 100, 10, 10);

    const cv::Mat1d seeds = converter::to_Mat1d_image<uint32_t> (serv.get ("default").value().get_last_value (),
                                                                 0,
                                                                 100,
                                                                 [](uint32_t value) -> double
                                                                 {
                                                                     return (double)value;
                                                                 });

    utils::opencv::display (seeds, Window::SEED.name ());

    utils::opencv::getUserInput ();
    utils::opencv::destroyAllWindows ();
    std::cout << "End\n";
}

int main1 ()
{
    const uint32_t x_size = configuration::MAP_SIZE_X;
    const uint32_t y_size = configuration::MAP_SIZE_Y;
    const double min_eval = configuration::TERRAIN_MINIMUM_ELEVATION;
    const double max_eval = configuration::TERRAIN_MAXIMUM_ELEVATION;
    const double frequency = 5;

    const auto terrain_grid = TerrainGenerator<1, double>::createPerlinNoise (x_size, y_size, min_eval, max_eval, frequency);

    const Terrain terrain = TerrainGenerator<1, double>::createPerlinNoiseTerrain (x_size, y_size, min_eval, max_eval, frequency);



    const cv::Mat1d heightMap = converter::to_Mat1d_image<glm::f64vec1> (terrain.getHeightMap (),
                                                                         glm::f64vec1 (terrain.min_eval),
                                                                         glm::f64vec1 (terrain.max_eval),
                                                                         [](glm::f64vec1 value) -> double
                                                                         {
                                                                             return (double)value[0];
                                                                         });
    utils::opencv::display (heightMap, Window::TERRAIN.name ());



    utils::opencv::display (converter::prepareNormal (terrain.getNormalMap ()), Window::NORMAL.name ());
    const auto normal_recalced = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (terrain.getHeightMap ());
    utils::opencv::display (converter::prepareNormal (normal_recalced), Window::NORMAL.name () + "_generated");



    std::vector<Droplet> droplets = spawnDroplets (100000, terrain.size_x-1, terrain.size_y-1, terrain.getHeightMap(), 1);
    const cv::Mat1d waterMap = converter::to_Mat1d_image<glm::f64vec1> (map_droplets(droplets, terrain.size_x, terrain.size_y),
                                                                         glm::f64vec1 (0),
                                                                         glm::f64vec1 (3),
                                                                         [](glm::f64vec1 value) -> double
                                                                         {
                                                                             return (double)value[0];
                                                                         });
    utils::opencv::display (waterMap, Window::WATER.name ());

    utils::opencv::getUserInput ();
    utils::opencv::destroyAllWindows ();
}
