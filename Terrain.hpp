#pragma once

#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

#include "Grid.hpp"
#include <glm/glm.hpp>
#include "NormapMapGenerator.hpp"

class Terrain
{
public:
	Terrain() :
		min_eval(0),
		max_eval(1),
		size_x(100),
		size_y(100),
		heightMap (size_x, size_y),
		normalMap (size_x, size_y),
		waterMap (size_x, size_y),
		pixel_to_meter_ratio_x(1),
		pixel_to_meter_ratio_y(1)
	{}

	Terrain (const double min_eval, const double max_eval) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (100),
		size_y (100),
		heightMap (size_x, size_y),
		normalMap (size_x, size_y),
		waterMap (size_x, size_y),
		pixel_to_meter_ratio_x (1),
		pixel_to_meter_ratio_y (1)
	{}

	Terrain (const double min_eval, const double max_eval, uint32_t size_x, uint32_t size_y) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (size_x),
		size_y (size_y),
		heightMap (size_x, size_y),
		normalMap (size_x, size_y),
		waterMap (size_x, size_y),
		pixel_to_meter_ratio_x (1),
		pixel_to_meter_ratio_y (1)
	{}

	Terrain (const double min_eval, const double max_eval, uint32_t size_x, uint32_t size_y, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (size_x),
		size_y (size_y),
		heightMap (size_x, size_y),
		normalMap (size_x, size_y),
		waterMap (size_x, size_y),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (Grid<glm::f64vec1> heightMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap(heightMap),
		normalMap (heightMap.get_x_size (), heightMap.get_y_size ()),
		waterMap (heightMap.get_x_size (), heightMap.get_y_size ()),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (Grid<glm::f64vec1> heightMap, Grid<glm::f64vec3> normalMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap (heightMap),
		normalMap(normalMap),
		waterMap(heightMap.get_x_size(), heightMap.get_y_size()),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size ()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	Terrain (Grid<glm::f64vec1> heightMap, Grid<glm::f64vec3> normalMap, Grid<glm::f64vec1> waterMap, const double min_eval, const double max_eval, double pixel_to_meter_ratio_x, double pixel_to_meter_ratio_y) :
		heightMap (heightMap),
		normalMap (normalMap),
		waterMap (waterMap),
		min_eval (min_eval),
		max_eval (max_eval),
		size_x (heightMap.get_x_size ()),
		size_y (heightMap.get_y_size ()),
		pixel_to_meter_ratio_x (pixel_to_meter_ratio_x),
		pixel_to_meter_ratio_y (pixel_to_meter_ratio_y)
	{}

	const Grid<glm::f64vec1>& getHeightMap () const noexcept
	{
		return heightMap;
	}

	Grid<glm::f64vec1>& getHeightMap () noexcept
	{
		return heightMap;
	}

	void setHeightMap (Grid<glm::f64vec1> heightMap) noexcept
	{
		this->heightMap = heightMap;
	}

	const Grid<glm::f64vec3>& getNormalMap () const noexcept
	{
		return normalMap;
	}

	Grid<glm::f64vec3>& getNormalMap () noexcept
	{
		return normalMap;
	}

	void setNormalMap (Grid<glm::f64vec3> normalMap) noexcept
	{
		this->normalMap = normalMap;
	}

	const Grid<glm::f64vec1>& getWaterMap () const noexcept
	{
		return waterMap;
	}

	Grid<glm::f64vec1>& getWaterMap () noexcept
	{
		return waterMap;
	}

	void setWaterMap (Grid<glm::f64vec1> waterMap) noexcept
	{
		this->waterMap = waterMap;
	}

	void generateNormalMap ()
	{
		this->normalMap = NormalMapGenerator::caclulateWorldSpaceNormalFromHeightMap (this->heightMap, this->pixel_to_meter_ratio_x, this->pixel_to_meter_ratio_y);
	}

public:
	const double min_eval;
	const double max_eval;

	const uint32_t size_x;
	const uint32_t size_y;

	const double pixel_to_meter_ratio_x;
	const double pixel_to_meter_ratio_y;

private:
	Grid<glm::f64vec1> heightMap;
	Grid<glm::f64vec3> normalMap;
	Grid<glm::f64vec1> waterMap;
};

#endif // !_TERRAIN_HPP_

