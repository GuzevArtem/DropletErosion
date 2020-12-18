#pragma once

#define _USE_MATH_DEFINES 
#include <math.h> //for macros
#include <cstdint>

namespace configuration
{
	/* map */

	constexpr size_t MAP_SIZE_X = 512; // in pixels
	constexpr size_t MAP_SIZE_Y = 512; // in pixels

	constexpr size_t MAP_SIZE_X_M = 512; // in meters
	constexpr size_t MAP_SIZE_Y_M = 512; // in meters

	constexpr double calcPixelToMeterRatio(const size_t size_in_meters, const size_t size_in_pixels)
	{
		return size_in_meters * 1.0 / size_in_pixels;
	}

	constexpr double PIXEL_TO_METER_RATIO_X = calcPixelToMeterRatio(MAP_SIZE_X_M, MAP_SIZE_X); // coef how many meters in pixel along x
	constexpr double PIXEL_TO_METER_RATIO_Y = calcPixelToMeterRatio(MAP_SIZE_Y_M, MAP_SIZE_Y); // coef how many meters in pixel along y

	constexpr double METER_TO_PIXEL_RATIO_X = 1.0/ PIXEL_TO_METER_RATIO_X; // coef how many pixels in meter along x
	constexpr double METER_TO_PIXEL_RATIO_Y = 1.0/ PIXEL_TO_METER_RATIO_Y; // coef how many pixels in meter along y

	constexpr double TERRAIN_BASE_ELEVATION = 0; // in meters (default elevation)
	constexpr double TERRAIN_MINIMUM_ELEVATION = -100; // in meters (value 0 on height map)
	constexpr double TERRAIN_MAXIMUM_ELEVATION = +100; // in meters (value 1 on height map)
	constexpr double TERRAIN_HEIGHT = TERRAIN_MAXIMUM_ELEVATION - TERRAIN_MINIMUM_ELEVATION; // in meters
	constexpr double SEA_LEVEL = 0; // in meters


	/* perlin noise */

	constexpr uint32_t PERLIN_NOISE_SEED = 0; // 0 - to generate new seed
	constexpr double PERLIN_NOISE_FREQUENCY = 256;
	constexpr int32_t PERLIN_NOISE_OCTAVES = 5;

	/* erosion */

	constexpr double WATER_DROPLET_RADIUS = 0.6 5; // in pixels

	constexpr double WATER_DROPLET_RADIUS_M = WATER_DROPLET_RADIUS * PIXEL_TO_METER_RATIO_X; // in meters
	constexpr double WATER_DROPLET_VOLUME_M = 4 * M_PI * (WATER_DROPLET_RADIUS * WATER_DROPLET_RADIUS * WATER_DROPLET_RADIUS) / 3; // in meters^3

	/* simulation */

	constexpr double TIME_STEP = 1.0; // < 1.0
	constexpr size_t MAX_STEPS = 2000000;
	constexpr size_t EROSION_STEP = 1000;
	constexpr uint32_t INITIAL_MAXIMUM_DROPLET_COUNT = 10000; // should be less than (size_x*size_y)/10

}
