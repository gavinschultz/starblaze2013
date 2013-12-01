#pragma once
#include <memory>
#include <SDL.h>
#include "Entity.h"
#include "Phys.h"

enum class ShipDirection { left, right };

class Ship : public Entity
{
private:
	class impl; std::unique_ptr<impl> pimpl;
protected:
	const std::vector<SDL_Rect>* Ship::getBaseCollisionBoxes() const;
public:
	Ship();
	~Ship();
	void fire();
	ShipDirection direction{ ShipDirection::right };
	unsigned int bullets{ 128 };
	int shields{ 100 };
	int radar{ 100 };
	int fuel{ 10000 };
	bool isGearDown();
	const double reverse_thrust_factor{ 1.6 };
	const double takeoff_speed{ 250.0 };
	const Vector2D max_thrust{ 400.0, 12.0 };
	const double max_velocity{ 20000.0 };
	double getDecelerationFactorX() const;
	double getDecelerationFactorY() const;
};