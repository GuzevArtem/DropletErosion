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

	std::shared_ptr<std::function<void (Droplet*)>> onSpawn{};
	std::shared_ptr<std::function<void (Droplet*, glm::f64vec3)>> onMove{};
	std::shared_ptr<std::function<void (Droplet*)>> onStop{};
	std::shared_ptr<std::function<void (Droplet*, double)>> onSoilPick{};
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
	void setOnMove (const std::function<void (Droplet*, glm::f64vec3)>& func)
	{
		onMove = std::make_shared < std::function<void (Droplet*, glm::f64vec3)>> (func);
	}
	void setOnStop (const std::function<void (Droplet*)>& func)
	{
		onStop = std::make_shared < std::function<void (Droplet*)>> (func);
	}
	void setOnSoilPick (const std::function<void (Droplet*, double)>& func)
	{
		onSoilPick = std::make_shared < std::function<void (Droplet*, double)>> (func);
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

	double calcAmountToPick (const Droplet& d)
	{
		if ( !d.isDead )
		{
			const double water_density = 999.83; //TODO
			const double soil_density = 1800.36; //TODO
			const double water_to_soil = (std::clamp(d.volume / d.soil, 0.0, 1.0) * water_density) / soil_density;
			const double max_picked = d.volume * water_to_soil;
			if ( max_picked > d.soil )
			{
				const double available_to_pick = max_picked - d.soil;
				const double proportion = (max_picked - d.soil)/ max_picked;
				const double diffusion_speed = 0.5; //TODO
				const double will_be_picked = proportion * diffusion_speed * configuration::TIME_STEP;
				return std::min(will_be_picked, std::lerp(0.5, max_picked, d.volume-d.soil)); //some adjust function to prevent carrying more soil than total volume
			}
		}
		return 0.0;
	}

	double calcAmountToDrop (const Droplet& d)
	{
		if ( !d.isDead )
		{
			if ( d.soil > 0 )
			{
				const double water_density = 999.83; //TODO
				const double soil_density = 1800.36; //TODO
				const double water_to_soil = (std::clamp (d.volume / d.soil, 0.0, 1.0) * water_density) / soil_density;
				const double max_picked = d.volume * water_to_soil;
				if ( max_picked < d.soil )
				{
					const double available_to_drop = d.soil - max_picked;
					const double proportion = (d.soil - max_picked) / d.soil;
					const double diffusion_speed = 0.5;
					const double will_be_dropped = proportion * diffusion_speed * configuration::TIME_STEP;
					return std::min (will_be_dropped, std::lerp (0.5, 0, d.soil));
				}
			}
			return 0.0;
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
			if ( true )// d.pos.z >= getHeightAt (d.pos) + getWaterAt (d.pos) ) //if we are on top
			{
				const double wing_speed = glm::length(glm::f64vec2(d.speed));//3; // m/s
				const double evap_coef = 25 + 19 * wing_speed; // kg/(m*m*h)
				const double water_surface_area = terrain.pixel_to_meter_ratio_x * terrain.pixel_to_meter_ratio_y;
				const double humidity_ratio_diff = 0.004859; // kg/kg
				const double amount = evap_coef * water_surface_area * humidity_ratio_diff * (configuration::TIME_STEP / 10 ); // / 3600 );
				return amount;
			}
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
		return 0.0;
	}

	void addDropletToMap (const Droplet& d)
	{
		const uint32_t x = (uint32_t)std::floor (std::clamp (d.pos.x, 0.0, (double)terrain.size_x));
		const uint32_t y = (uint32_t)std::floor (std::clamp (d.pos.y, 0.0, (double)terrain.size_y));
		terrain.getWaterMap ()->assign_unchecked (x, y, getWaterAt(x,y) + d.volume);
	}

	void removeDropletFromMap (const Droplet& d)
	{
		const uint32_t x = (uint32_t)std::floor (std::clamp (d.pos.x, 0.0, (double)terrain.size_x));
		const uint32_t y = (uint32_t)std::floor (std::clamp (d.pos.y, 0.0, (double)terrain.size_y));
		terrain.getWaterMap ()->assign_unchecked (x, y, getWaterAt (x, y) - d.volume);
	}

protected:

	//height
	[[nodiscard]]
	inline double getHeightAt (const glm::f64vec3& pos) const noexcept
	{
		return getHeightAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline double getHeightAt (const glm::f64vec2& pos) const noexcept
	{
		return getHeightAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline double getHeightAt (const double x, const double y) const noexcept
	{
		return terrain.getHeightMap ()->at ((uint32_t)x, (uint32_t)y);
	}

	//normal

	[[nodiscard]]
	inline glm::f64vec3 getNormalAt (const glm::f64vec3& pos) const noexcept
	{
		return getNormalAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline glm::f64vec3 getNormalAt (const glm::f64vec2& pos) const noexcept
	{
		return getNormalAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline glm::f64vec3 getNormalAt (const double x, const double y) const noexcept
	{
		return terrain.getNormalMap ()->at ((uint32_t)x, (uint32_t)y);
	}

	//water

	[[nodiscard]]
	inline double getWaterAt (const glm::f64vec3& pos) const noexcept
	{
		return getWaterAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline double getWaterAt (const glm::f64vec2& pos) const noexcept
	{
		return getWaterAt (pos.x, pos.y);
	}

	[[nodiscard]]
	inline double getWaterAt (const double x, const double y) const noexcept
	{
		return terrain.getWaterMap ()->at ((uint32_t)x, (uint32_t)y);
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
			d.volume = configuration::WATER_DROPLET_VOLUME_M;
			d.isDead = false;
			d.isMoving = true;
			//TODO: other properties

			if ( this->onSpawn )		d.onSpawn = std::make_shared<std::function<void (Droplet*)>> (*this->onSpawn.get ());
			if ( this->onMove )			d.onMove = std::make_shared<std::function<void (Droplet*, glm::f64vec3)>> (*this->onMove.get ());
			if ( this->onStop )			d.onStop = std::make_shared<std::function<void (Droplet*)>> (*this->onStop.get ());
			if ( this->onSoilPick )		d.onSoilPick = std::make_shared<std::function<void (Droplet*, double)>> (*this->onSoilPick.get ());
			if ( this->onEvapprate )	d.onEvapprate = std::make_shared<std::function<void (Droplet*, double)>> (*this->onEvapprate.get ());
			if ( this->onSoilDrop )		d.onSoilDrop = std::make_shared<std::function<void (Droplet*, double)>> (*this->onSoilDrop.get ());
			if ( this->onDead )			d.onDead = std::make_shared<std::function<void (Droplet*)>> (*this->onDead.get ());

			addDropletToMap (d);
		}
		return droplets;
	}

	void pick ()
	{
		for ( auto& d : droplets )
		{
			const double amount = calcAmountToPick (d);
			d.pick_soil (amount);
			terrain.getHeightMap ()->assign ((uint32_t)d.pos.x, (uint32_t)d.pos.y, getHeightAt (d.pos) - amount);
		}
	}

	void drop ()
	{
		for ( auto& d : droplets )
		{
			const double amount = calcAmountToDrop (d);
			d.soil_drop (amount);
			terrain.getHeightMap ()->assign ((uint32_t)d.pos.x, (uint32_t)d.pos.y, getHeightAt (d.pos) + amount);
		}
	}

	void move ()
	{
		for ( auto& d : droplets )
		{
			removeDropletFromMap (d);
			const double start_height = d.pos.z;
			const glm::f64vec3 speed_per_time_step = d.speed * configuration::TIME_STEP;

			 //TODO: teleporting droplets
			const glm::f64vec3 target_pos = d.pos + speed_per_time_step;

			if ( target_pos.x <= 0.0 || target_pos.x >= terrain.size_x
				|| target_pos.y <= 0.0 || target_pos.y >= terrain.size_y )
			{
				d.dead (); //Out of bounds
				continue;
			}

			const double target_height = getWaterAt (target_pos) + getHeightAt (target_pos);

			if ( target_height > start_height )
			{
				const glm::f64vec3 planar_speed{ speed_per_time_step.x, speed_per_time_step.y, 0 };
				if ( glm::length (planar_speed) < 2.0 ) //TODO: adjust value
				{
					d.soil_drop (d.soil);
					d.dead ();
					continue;
				}
				else
				{
					//try next iteration
					d.stop (); //prevent evaporation
					d.speed /= 2;
				}
			}
			else
			{
				d.move (target_pos);
				//recalculate speed
				const glm::f64vec3 end_normal = getNormalAt (d.pos);
				d.speed = glm::f64vec3{ end_normal.x, end_normal.y, end_normal.z };
			}
			addDropletToMap (d);
		}
	}

	void evaporate ()
	{
		for ( auto& d : droplets )
		{
			if ( d.isMoving )
			{
				removeDropletFromMap (d);
				d.evaporate (calcAmountToEvaporate (d));
				if ( !d.isDead )
				{
					addDropletToMap (d);
				}
			}
		}
	}

	uint32_t delete_dead ()
	{
		std::vector<Droplet> new_vec;
		const uint32_t initial = (uint32_t)droplets.size ();
		std::copy_if (droplets.begin (), droplets.end (), std::back_inserter (new_vec), [](Droplet& d) -> bool
					  {
						  return !d.isDead;
					  });
		const uint32_t deleted = initial - (uint32_t)new_vec.size ();
		droplets = new_vec;
		return deleted;
	}

	void clear ()
	{
		droplets.clear ();
	}

	void pick_or_drop ()
	{
		for ( auto& d : droplets )
		{
			const double amount_to_pick = calcAmountToPick (d);
			const double amount_to_drop = calcAmountToDrop (d);

			if ( amount_to_pick > amount_to_drop )
			{
				const double diff = amount_to_pick - amount_to_drop;
				d.pick_soil (diff);
				terrain.getHeightMap ()->assign ((uint32_t)d.pos.x, (uint32_t)d.pos.y, getHeightAt (d.pos) - diff);
			}
			else
			{
				const double diff = amount_to_drop - amount_to_pick;
				d.soil_drop (amount_to_drop);
				terrain.getHeightMap ()->assign ((uint32_t)d.pos.x, (uint32_t)d.pos.y, getHeightAt (d.pos) + diff);
			}
		}
	}

	void iteration ()
	{
		move ();
		pick_or_drop ();
		evaporate ();
		const auto deleted = delete_dead ();
		generate (deleted); //recreate dead
		terrain.generateNormalMap ();
	}
};

#endif //!_DROPLET_SERVICE_HPP_
