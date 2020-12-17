#include <iostream>
#include <stdint.h>

#include <limits>

#include "StaticConfig.hpp"

#include "Utils.hpp"
#include "GridToOpenCVConverter.hpp"

#include "RngService.hpp"

#include "WindowNames.hpp"
#include "TerrainGenerator.hpp"
#include "NormapMapGenerator.hpp"

#include "Grid.hpp"
#include "Droplet.hpp"
#include "DropletService.hpp"

//TODO remove or move
Grid<double> map_droplets (const std::vector<Droplet>& droplets, uint32_t size_x, uint32_t size_y)
{
    Grid<double> water (size_x, size_y);

    for ( const auto& d : droplets )
    {
        water.assign((uint32_t)d.pos.x, (uint32_t)d.pos.y, water.at ((uint32_t)d.pos.x, (uint32_t)d.pos.y) + d.volume);
    }

    return water;
}

int main ()
{
    std::cout << "Start\n";

    const uint32_t x_size = configuration::MAP_SIZE_X;
    const uint32_t y_size = configuration::MAP_SIZE_Y;
    const double min_eval = configuration::TERRAIN_MINIMUM_ELEVATION;
    const double max_eval = configuration::TERRAIN_MAXIMUM_ELEVATION;
    const double frequency = 5;


    std::seed_seq ss{ 5479U };

    RNGService<double> serv;

    //serv.create_sequence ("default", ss, 0.0, 1.0, x_size, y_size);
    serv.create_sequence ("droplets", ss, 0.0, 1.0, 1, 1);


    std::cout << "Terrain\n";

    const auto terrain_grid = TerrainGenerator<1, double>::createPerlinNoise (x_size, y_size, min_eval, max_eval, frequency);

    Terrain terrain = TerrainGenerator<1, double>::createPerlinNoiseTerrain (x_size, y_size, min_eval, max_eval, frequency);

    const cv::Mat1d heightMap = converter::to_Mat1d_image<double> (*terrain.getHeightMap (),
                                                                         terrain.min_eval,
                                                                         terrain.max_eval);
    utils::opencv::display (heightMap, Window::TERRAIN.name ());

    std::cout << "Normal\n";

    utils::opencv::display (converter::prepareNormal (*terrain.getNormalMap ()), Window::NORMAL.name ());
    const auto normal_recalced = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (*terrain.getHeightMap ());
    utils::opencv::display (converter::prepareNormal (normal_recalced), Window::NORMAL.name () + "_generated");


    std::cout << "Droplets\n";

    DropletService dropletService (terrain, [&terrain , &ss, &serv, x_size, y_size] () -> glm::f64vec3 {
        std::unique_ptr<RandomNumberStreamHolder<double>> rnsh_ptr;
        if ( serv.get ("droplets").has_value () )
        {
            rnsh_ptr = std::make_unique<RandomNumberStreamHolder<double>> (serv.get ("droplets").value());
        }
        else
        {
            rnsh_ptr = std::make_unique<RandomNumberStreamHolder<double>> (serv.create_sequence ("droplets", ss, 0.0, 1.0, 1, 1));
        }

        const double x = rnsh_ptr->next (0) * x_size; //guaranteed to be in bounds
        const double y = rnsh_ptr->next (0) * y_size; //guaranteed to be in bounds
        const double z = terrain.getHeightMap ()->at_unchecked (x, y);

        return {x, y, z};
    });

    dropletService.generate (100000);

    const cv::Mat1d dropletsMap = converter::to_Mat1d_image<double> (map_droplets(dropletService.get_droplets(), terrain.size_x, terrain.size_y),
                                                                         0.0,
                                                                         3.0);
    utils::opencv::display (dropletsMap, Window::WATER.name ()+"_droplets");


    std::cout << "Water\n";

    const cv::Mat1d waterMap = converter::to_Mat1d_image<double> (*terrain.getWaterMap(), 0.0, 3.0);

    utils::opencv::display (waterMap, Window::WATER.name ());

    utils::opencv::getUserInput ();
    utils::opencv::destroyAllWindows ();
}
