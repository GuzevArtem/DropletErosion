#pragma once

#ifndef _DROPLET_HPP_
#define _DROPLET_HPP_

#include <vector>
#include <glm/glm.hpp>
#include <functional>

class Droplet
{
public:
	//static methods

	static void populateEventProcessors (const Droplet * const example, Droplet value)
	{
		if ( example->onSpawn ) value.onSpawn = std::make_shared<std::function<void (Droplet*)>> (*example->onSpawn.get ());
		if ( example->onMove ) value.onMove = std::make_shared<std::function<void (Droplet*)>> (*example->onMove.get ());
		if ( example->onStop ) value.onStop = std::make_shared<std::function<void (Droplet*)>> (*example->onStop.get ());
		if ( example->onSoilPick ) value.onSoilPick = std::make_shared<std::function<void (Droplet*)>> (*example->onSoilPick.get ());
		if ( example->onEvapprate ) value.onEvapprate = std::make_shared<std::function<void (Droplet*, double)>> (*example->onEvapprate.get ());
		if ( example->onSoilDrop ) value.onSoilDrop = std::make_shared<std::function<void (Droplet*, double)>> (*example->onSoilDrop.get ());
		if ( example->onDead ) value.onDead = std::make_shared<std::function<void (Droplet*)>> (*example->onDead.get ());
	}

	static void populateEventProcessors (const Droplet& example, Droplet value)
	{
		if ( example.onSpawn ) value.onSpawn = std::make_shared<std::function<void (Droplet*)>> (*example.onSpawn.get ());
		if ( example.onMove ) value.onMove = std::make_shared<std::function<void (Droplet*)>> (*example.onMove.get ());
		if ( example.onStop ) value.onStop = std::make_shared<std::function<void (Droplet*)>> (*example.onStop.get ());
		if ( example.onSoilPick ) value.onSoilPick = std::make_shared<std::function<void (Droplet*)>> (*example.onSoilPick.get ());
		if ( example.onEvapprate ) value.onEvapprate = std::make_shared<std::function<void (Droplet*, double)>> (*example.onEvapprate.get ());
		if ( example.onSoilDrop ) value.onSoilDrop = std::make_shared<std::function<void (Droplet*, double)>> (*example.onSoilDrop.get ());
		if ( example.onDead ) value.onDead = std::make_shared<std::function<void (Droplet*)>> (*example.onDead.get ());
	}

	static void populateEventProcessors (const Droplet& example, std::vector<Droplet> values)
	{
		for ( const auto& v : values )
		{
			populateEventProcessors (example, v);
		}
	}

public:

	std::vector<Droplet> split (const std::vector<glm::f64vec4>& offsets_and_proportions)
	{
		std::vector<Droplet> new_droplets;

		double total_proportion = 0;
		for ( const auto& pos_and_prop : offsets_and_proportions )
		{
			const glm::f64vec3 offset{ pos_and_prop };
			const double proportion = pos_and_prop[3];
			Droplet new_d;
			new_d.pos = offset; //TODO: Bad but why not?
			new_d.soil = proportion; //TODO: Bad but why not?
			new_droplets.push_back (new_d);
		}

		for ( auto& d : new_droplets )
		{
			d.isMoving = true;
			d.soil = (d.soil * this->soil) / total_proportion;

			d.speed = d.pos + this->speed; //with movement inertia (TODO: add mass coef?)
			d.pos = this->pos + d.pos;

			populateEventProcessors (this, d);
		}

		if ( total_proportion >= 1.0 )
		{
			dead ();
		}
		else
		{
			this->soil *= (1.0 - total_proportion);
		}

		return new_droplets;
	}

	//event functions

	void spawn (const glm::f64vec3 pos)
	{
		this->pos = pos;
		if ( onSpawn )
		{
			onSpawn->operator()(this);
		}
	}

	void pick_soil ()
	{
		if ( onSoilPick )
		{
			onSoilPick->operator()(this);
		}
	}

	void soil_drop (const double amount)
	{
		const double to_drop = this->soil - amount < 0 ? this->soil : amount;

		this->soil -= to_drop;

		if ( onSoilDrop )
		{
			onSoilDrop->operator()(this, to_drop);
		}
	}

	void evaporate (const double amount)
	{
		if ( amount >= this->volume )
		{
			if ( onEvapprate )
			{
				onEvapprate->operator()(this, this->volume);
			}
			dead ();
			return;
		}
		else
		{
			this->volume -= amount;
		}

		if ( onEvapprate )
		{
			onEvapprate->operator()(this, amount);
		}
	}

	void move ()
	{
		isMoving = true;
		if ( onMove )
		{
			onMove->operator()(this);
		}
	}

	void stop ()
	{
		this->speed = glm::f64vec3{0,0,0};
		this->isMoving = false;
		if ( onStop )
		{
			onStop->operator()(this);
		}
	}

	//droplet marked as dead should not be processed anymore
	void dead ()
	{
		isDead = true;
		isMoving = false;
		if ( onDead )
		{
			onDead->operator()(this);
		}
	}

public:
	//active

	glm::f64vec3 pos{};
	glm::f64vec3 speed{};

	double volume = 0.0;
	double soil = 0.0;

	//state

	bool isDead = false;
	bool isMoving = true;

	//statistic

	double path_passed = 0.0;
	size_t livetime = 0;

	//events processors
	std::shared_ptr<std::function<void (Droplet*)>> onSpawn{};
	std::shared_ptr<std::function<void (Droplet*)>> onMove{};
	std::shared_ptr<std::function<void (Droplet*)>> onStop{};
	std::shared_ptr<std::function<void (Droplet*)>> onSoilPick{};
	std::shared_ptr<std::function<void (Droplet*, double)>> onSoilDrop{};
	std::shared_ptr<std::function<void (Droplet*, double)>> onEvapprate{};
	std::shared_ptr<std::function<void (Droplet*)>> onDead{};
};

#endif // !_DROPLET_HPP_
