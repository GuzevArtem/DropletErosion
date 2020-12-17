#pragma once

#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

#include "Grid.hpp"
#include <glm/glm.hpp>
#include "NormapMapGenerator.hpp"

class Terrain
{
	using height_map_type = Grid<double>;
	using normal_map_type = Grid<glm::f64vec3>;
	using water_map_type = Grid<double>;

	using height_map_ptr = std::shared_ptr <height_map_type>;
	using normal_map_ptr = std::shared_ptr <normal_map_type>;
	using water_map_ptr = std::shared_ptr <water_map_type>;

public:
	Terrain() :
		min_eval(0),
		max_eval(1),
		size_x(100),
		size_y(100),
		heightMap (std::make_shared<height_map_type>(size_x, size_y)),
		normalMap (std::make_shared<normal_map_type> (size_x, size_y)),
		waterMap (std::make_shared<water_map_type> (size_x, size_y)),
		pixel_to_meter_ratio_x(1),
		pixel_to_meter_ratio_y(1)
	{}

	Terrain (const double min_eval, const double max_eval) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (100),
		size_y (100),
		heightMap (std::make_shared<height_map_type> (size_x, size_y)),
		normalMap (std::make_shared<normal_map_type> (size_x, size_y)),
		waterMap (std::make_shared<water_map_type> (size_x, size_y)),
		pixel_to_meter_ratio_x (1),
		pixel_to_meter_ratio_y (1)
	{}

	Terrain (const double min_eval, const double max_eval, uint32_t size_x, uint32_t size_y) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (size_x),
		size_y (size_y),
		heightMap (std::make_shared<height_map_type> (size_x, size_y)),
		normalMap (std::make_shared<normal_map_type> (size_x, size_y)),
		waterMap (std::make_shared<water_map_type> (size_x, size_y)),
		pixel_to_meter_ratio_x (1),
		pixel_to_meter_ratio_y (1)
	{}

	Terrain (const double min_eval, const double max_eval, uint32_t size_x, uint32_t size_y, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (size_x),
		size_y (size_y),
		heightMap (std::make_shared<height_map_type> (size_x, size_y)),
		normalMap (std::make_shared<normal_map_type> (size_x, size_y)),
		waterMap (std::make_shared<water_map_type> (size_x, size_y)),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (height_map_type heightMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap(std::make_shared<height_map_type>(heightMap)),
		normalMap (std::make_shared<normal_map_type> (heightMap.get_x_size (), heightMap.get_y_size ())),
		waterMap (std::make_shared<water_map_type> (heightMap.get_x_size (), heightMap.get_y_size ())),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (height_map_type heightMap, normal_map_type normalMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap (std::make_shared<height_map_type> (heightMap)),
		normalMap (std::make_shared<normal_map_type> (normalMap)),
		waterMap (std::make_shared<water_map_type> (heightMap.get_x_size (), heightMap.get_y_size ())),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size ()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (height_map_type heightMap, normal_map_type normalMap, water_map_type waterMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap (std::make_shared<height_map_type> (heightMap)),
		normalMap (std::make_shared<normal_map_type> (normalMap)),
		waterMap (std::make_shared<water_map_type> (waterMap)),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size ()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	const height_map_ptr& getHeightMap () const noexcept
	{
		return heightMap;
	}

	height_map_ptr& getHeightMap () noexcept
	{
		return heightMap;
	}

	void setHeightMap (height_map_type heightMap) noexcept
	{
		this->heightMap = std::make_shared<height_map_type>(heightMap);
	}

	const normal_map_ptr& getNormalMap () const noexcept
	{
		return normalMap;
	}

	normal_map_ptr& getNormalMap () noexcept
	{
		return normalMap;
	}

	void setNormalMap (normal_map_type normalMap) noexcept
	{
		this->normalMap = std::make_shared<normal_map_type>(normalMap);
	}

	const water_map_ptr& getWaterMap () const noexcept
	{
		return waterMap;
	}

	water_map_ptr& getWaterMap () noexcept
	{
		return waterMap;
	}

	void setWaterMap (water_map_type waterMap) noexcept
	{
		this->waterMap = std::make_shared<water_map_type>(waterMap);
	}

	void generateNormalMap ()
	{
		setNormalMap(NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (*this->heightMap, this->pixel_to_meter_ratio_x, this->pixel_to_meter_ratio_y));
	}

public:
	const double min_eval;
	const double max_eval;

	const uint32_t size_x;
	const uint32_t size_y;

	const double pixel_to_meter_ratio_x;
	const double pixel_to_meter_ratio_y;

private:

	height_map_ptr heightMap;
	normal_map_ptr normalMap;
	water_map_ptr waterMap;
};

#endif // !_TERRAIN_HPP_

