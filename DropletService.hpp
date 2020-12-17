#pragma once

#ifndef _DROPLET_SERVICE_HPP_
#define _DROPLET_SERVICE_HPP_

#include <vector>
#include <random>
#include <glm/vec3.hpp>
#include "Terrain.hpp"
#include "RngService.hpp"
#include "Droplet.hpp"

#include "StaticConfig.hpp"

class DropletService
{
private:
	Terrain terrain;
	std::vector<Droplet> droplets;

	//required parameters

	std::function<glm::f64vec3 (void)> generatePositionFunc;

	//events processors //TODO: change on producer factory or fill here
	std::shared_ptr<std::function<void (Droplet*)>> onSpawn{};
	std::shared_ptr<std::function<void (Droplet*)>> onMove{};
	std::shared_ptr<std::function<void (Droplet*)>> onStop{};
	std::shared_ptr<std::function<void (Droplet*)>> onSoilPick{};
	std::shared_ptr<std::function<void (Droplet*, double)>> onSoilDrop{};
	std::shared_ptr<std::function<void (Droplet*, double)>> onEvapprate{};
	std::shared_ptr<std::function<void (Droplet*)>> onDead{};

public:

	DropletService (Terrain& terrain, const std::function<glm::f64vec3 (void)>& generatePositionFunc)
		: terrain (terrain), generatePositionFunc (generatePositionFunc)
	{}

	void setOnSpawn (const std::function<void (Droplet*)>& func)
	{
		onSpawn = std::make_shared < std::function<void (Droplet*)>>(func);
	}
	void setOnMove (const std::function<void (Droplet*)>& func)
	{
		onMove = std::make_shared < std::function<void (Droplet*)>> (func);
	}
	void setOnStop (const std::function<void (Droplet*)>& func)
	{
		onStop = std::make_shared < std::function<void (Droplet*)>> (func);
	}
	void setOnSoilPick (const std::function<void (Droplet*)>& func)
	{
		onSoilPick = std::make_shared < std::function<void (Droplet*)>> (func);
	}
	void setOnSoilDrop (const std::function<void (Droplet*, double)>& func)
	{
		onSoilDrop = std::make_shared < std::function<void (Droplet*, double)>> (func);
	}
	void setOnEvapprate (const std::function<void (Droplet*, double)>& func)
	{
		onEvapprate = std::make_shared < std::function<void (Droplet*, double)>> (func);
	}
	void setOnDead (const std::function<void (Droplet*)>& func)
	{
		onDead = std::make_shared < std::function<void (Droplet*)>> (func);
	}

private:

	double calcAmountToDrop (const Droplet& d)
	{
		if ( !d.isDead )
		{
			//TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		else
		{
			return d.soil;
		}
	}

	double calcAmountToEvaporate (const Droplet& d)
	{
		if ( !d.isDead )
		{
			const double wing_speed = 3; // m/s
			const double evap_coef = 25 + 19 * wing_speed; // kg/(m*m*h)
			const double water_surface_area = terrain.pixel_to_meter_ratio_x * terrain.pixel_to_meter_ratio_y; //TODO: check if it's a top droplet
			const double humidity_ratio_diff = 0.004859; // kg/kg

			const double amount = evap_coef* water_surface_area* humidity_ratio_diff * (configuration::TIME_STEP/3600);

			return amount;
			/*for ( uint32_t x = d.pos.x < 1 ? 0 : std::round (d.pos.x); //check is round could cause out of bounds
				 x < waterMap.get_x_size ();
				 x++ )
			{
				for ( uint32_t y = d.pos.y < 1 ? 0 : std::round (d.pos.y);
					 y < waterMap.get_y_size ();
					 y++ )
				{

				}
			}*/
		}
		else
		{
			return d.volume;
		}
	}

	void addDropletToMap (const Droplet& d)
	{
		terrain.getWaterMap ()->assign_unchecked (std::clamp(d.pos.x, 0.0, (double)terrain.size_x),
												 std::clamp (d.pos.y, 0.0, (double)terrain.size_y),
												 d.volume);
	}

	void removeDropletFromMap (const Droplet& d)
	{
		terrain.getWaterMap ()->assign_unchecked (std::clamp (d.pos.x, 0.0, (double)terrain.size_x),
												 std::clamp (d.pos.y, 0.0, (double)terrain.size_y),
												 -d.volume);
	}

public:

	const std::vector<Droplet> get_droplets () const noexcept
	{
		return droplets;
	}

	std::vector<Droplet>& get_droplets () noexcept
	{
		return droplets;
	}

	inline std::vector<Droplet> generate (uint32_t count)
	{
		droplets.reserve (droplets.size () + count); //WARN: possible overflow
		for ( uint32_t i = 0; i < count; i++ )
		{
			const glm::f64vec3 pos = generatePositionFunc ();
			Droplet& d = droplets.emplace_back ();
			d.pos = pos;
			d.volume = 1.0;
			//TODO: other properties

			if ( this->onSpawn )		d.onSpawn = std::make_shared<std::function<void (Droplet*)>> (*this->onSpawn.get ());
			if ( this->onMove )			d.onMove = std::make_shared<std::function<void (Droplet*)>> (*this->onMove.get ());
			if ( this->onStop )			d.onStop = std::make_shared<std::function<void (Droplet*)>> (*this->onStop.get ());
			if ( this->onSoilPick )		d.onSoilPick = std::make_shared<std::function<void (Droplet*)>> (*this->onSoilPick.get ());
			if ( this->onEvapprate )	d.onEvapprate = std::make_shared<std::function<void (Droplet*, double)>> (*this->onEvapprate.get ());
			if ( this->onSoilDrop )		d.onSoilDrop = std::make_shared<std::function<void (Droplet*, double)>> (*this->onSoilDrop.get ());
			if ( this->onDead )			d.onDead = std::make_shared<std::function<void (Droplet*)>> (*this->onDead.get ());

			addDropletToMap (d);
		}
		return droplets;
	}

	void move ()
	{
		for ( auto& d : droplets )
		{
			removeDropletFromMap (d);
			d.move ();
			addDropletToMap (d);
		}
	}

	void evaporate ()
	{
		for ( auto& d : droplets )
		{
			d.evaporate (calcAmountToEvaporate(d));
		}
	}

	size_t delete_dead ()
	{
		std::remove_if (droplets.begin (), droplets.end (), [] (Droplet& d) -> bool { return d.isDead; });
	}

	void clear ()
	{
		droplets.clear ();
	}
};

#endif //!_DROPLET_SERVICE_HPP_
