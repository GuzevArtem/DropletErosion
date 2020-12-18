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

cv::Mat1d deadMap;
cv::Mat3d speedMap;
cv::Mat1d soilPciked;
cv::Mat1d soilDropped;
cv::Mat1d evaporated;

void initTempaMaps (uint32_t size_x, uint32_t size_y)
{
    //soilPciked = cv::Mat1d::zeros (size_x, size_y);
    soilDropped = cv::Mat1d::zeros (size_x, size_y);
    //speedMap = cv::Mat3d::zeros (size_x, size_y);
    deadMap = cv::Mat1d::zeros (size_x, size_y);
    //evaporated = cv::Mat1d::zeros (size_x, size_y);


    //utils::opencv::display (speedMap, Window::DROPLET_SPEED.name ());
    utils::opencv::display (deadMap, Window::DEAD.name ());
    utils::opencv::display (soilDropped, Window::SOIL_DROPPED.name ());
}

void displayTempMaps ()
{
    //utils::opencv::refresh (soilPciked, Window::SOIL_PICKED.name ());
    utils::opencv::refresh (soilDropped, Window::SOIL_DROPPED.name ());
    //utils::opencv::refresh (speedMap, Window::DROPLET_SPEED.name ());
    utils::opencv::refresh (deadMap, Window::DEAD.name ());
    //utils::opencv::display (evaporated, Window::EVAPORATE.name ());
}

void display (const Terrain& terrain)
{
    const cv::Mat1d waterMap = converter::to_Mat1d_image<double> (*terrain.getWaterMap (), 0.0, 1.0);

    const auto normal = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (*terrain.getHeightMap ());

    const cv::Mat1d heightMap = converter::to_Mat1d_image<double> (*terrain.getHeightMap (),
                                                                   terrain.min_eval,
                                                                   terrain.max_eval);
    utils::opencv::refresh (waterMap, Window::WATER.name ());
    utils::opencv::refresh (converter::prepareNormal (normal), Window::NORMAL.name ());
    utils::opencv::refresh (heightMap, Window::TERRAIN.name ());
}

void save (const Terrain& terrain, const std::string path = "../images/")
{
    const auto normal = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (*terrain.getHeightMap ());

    const cv::Mat1d heightMap = converter::to_Mat1d_image<double> (*terrain.getHeightMap (),
                                                                       terrain.min_eval,
                                                                       terrain.max_eval);

    utils::opencv::saveImage<double> (heightMap, path, Window::HEIGHT_MAP.name () + ".jpg");
    utils::opencv::saveImage<cv::Vec3d> (converter::prepareNormal (normal), path, Window::NORMAL.name () + ".jpg");
    utils::opencv::saveImage<double> (soilDropped, path, Window::SOIL_DROPPED.name () + ".jpg");
}

int main ()
{
    std::cout << "Start\n";

    const uint32_t x_size = configuration::MAP_SIZE_X;
    const uint32_t y_size = configuration::MAP_SIZE_Y;
    const double min_eval = configuration::TERRAIN_MINIMUM_ELEVATION;
    const double max_eval = configuration::TERRAIN_MAXIMUM_ELEVATION;
    const double frequency = 256;


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

    const auto normal_recalced = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (*terrain.getHeightMap ());
    utils::opencv::display (converter::prepareNormal (normal_recalced), Window::NORMAL.name ());


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
        const double z = terrain.getHeightMap ()->at_unchecked ((uint32_t)x, (uint32_t)y);
        return {x, y, z};
    });

    initTempaMaps (terrain.size_x, terrain.size_y);

    dropletService.setOnDead ([](Droplet* d)->void
                              {
                                  deadMap.at<double> (d->pos.x, d->pos.y) = deadMap.at<double> (d->pos.x, d->pos.y) + 0.01;
                              });

    
    /*dropletService.setOnMove ([](Droplet* d, glm::f64vec3 speed)->void
                              {
                                  speedMap.at<cv::Vec3d> (d->pos.x, d->pos.y) = cv::Vec3d{ speed.x/5000, speed.y / 5000, speed.z / 5000 };
                              });*/

    dropletService.setOnSoilDrop ([](Droplet* d, double amount)->void
                                  {
                                      soilDropped.at<double> (d->pos.x, d->pos.y) = soilDropped.at<double> (d->pos.x, d->pos.y) + amount / 100;
                                  });

    /*dropletService.setOnSoilPick ([](Droplet* d, double amount)->void
                                  {
                                      soilPciked.at<double> (d->pos.x, d->pos.y) = soilPciked.at<double> (d->pos.x, d->pos.y) + amount / 100;
                                  });

    dropletService.setOnEvapprate ([](Droplet* d, double amount)->void
                                   {
                                       evaporated.at<double> (d->pos.x, d->pos.y) = evaporated.at<double> (d->pos.x, d->pos.y) + amount ;
                                   });
                                   */


    utils::opencv::display (converter::to_Mat1d_image<double> (*terrain.getWaterMap (), 0.0, 1.0), Window::WATER.name ());


    save (terrain, "D:\\Dev\\Cpp\\ai\\images\\initial\\");

    dropletService.generate (configuration::INITIAL_MAXIMUM_DROPLET_COUNT);

    size_t iteration = 0;

    std::cout << "Hold any button to finish.\n";
    while ( iteration < configuration::MAX_STEPS  && cv::waitKey (5) == -1 )
    {
        //std::cout << "\rIteration " << iteration;
        dropletService.iteration ();
        display (terrain);
        displayTempMaps ();
        iteration++;
    }
    std::cout << "Total iterations count: "<< iteration << "\n";
    save (terrain, "D:\\Dev\\Cpp\\ai\\images\\processed\\");
    std::cout << "Press any button to exit.\n";
    utils::opencv::getUserInput ();
    utils::opencv::destroyAllWindows ();

}
