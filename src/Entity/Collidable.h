#pragma once
#include <vector>
#include <typeinfo>
#include "Phys.h"
#include "Entity.h"

class Collidable
{
public:
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const = 0;
	virtual const std::vector<Rect>& getCollisionBoxes() const = 0;
	virtual const Rect getOuterBox() const = 0;
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false) = 0;
	virtual void reset() = 0;
	virtual bool canCollideWith(const Entity& entity) const = 0;
	virtual bool isCollided() const = 0;
	virtual bool testBoxCollision(Collidable& collidable) = 0;
	virtual void setCollided() = 0;
};

class NonCollidable : Collidable
{
public:
	NonCollidable() {}
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const { return std::vector<Rect>{}; }
	virtual const std::vector<Rect>& getCollisionBoxes() const { return std::vector<Rect>{}; }
	virtual const Rect getOuterBox() const { return Rect{}; }
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false) { }
	virtual void reset() { }
	virtual bool canCollideWith(const Entity& entity) const { return false; }
	virtual bool isCollided() const { return false; }
	virtual bool testBoxCollision(Collidable& collidable) { return false; }
	virtual void setCollided() { }
};

class NormalCollidable : Collidable
{
private:
	bool _is_collided{ false };
	const std::vector<Rect> _base_collision_boxes;
	std::vector<Rect> _collision_boxes;
	const std::vector<const std::type_info*> _collidable_types;
	const Rect _outer_box;
public:
	NormalCollidable(const Rect& outer_collision_box, const std::vector<Rect>& collision_boxes, const std::vector<const std::type_info*>& collidable_types);
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const { return _base_collision_boxes; }
	virtual const std::vector<Rect>& getCollisionBoxes() const { return _collision_boxes; }
	virtual const Rect getOuterBox() const { return _outer_box; }
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false);
	virtual void reset() { _is_collided = false; }
	virtual bool canCollideWith(const Entity& entity) const { return std::find(_collidable_types.cbegin(), _collidable_types.cend(), typeid(entity).name()) == _collidable_types.cend(); }
	virtual bool isCollided() const { return _is_collided; }
	virtual bool testBoxCollision(Collidable& collidable);
	virtual void setCollided() { _is_collided = true; }
};

#include "AlienBomb.h"
#include "Bullet.h"
#include "Station.h"
#include "Alien.h"
#include "Ship.h"
#include "Util.h"
#include "Game.h"

namespace collider
{
	bool areRectanglesIntersecting(const Rect& a, const Rect& b);
	bool areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b);

	bool isCollidingByRectangles(const Collidable& a, const Collidable& b)
	{
		if (!collider::areRectanglesIntersecting(a.getOuterBox(), b.getOuterBox()))
			return false;

		for (auto& coll_a : a.getCollisionBoxes())
		{
			for (auto& coll_b : b.getCollisionBoxes())
			{
				if (collider::areRectanglesIntersecting(coll_a, coll_b))
				{
					return true;
				}
			}
		}
		return false;
	}
	bool isCollidingByTemporalPositionLine(const Collidable& collidable, const TemporalState2D& temporal_position)
	{
		return false;
	}
}

//TODO: make templates
bool collider::areRectanglesIntersecting(const Rect& a, const Rect& b)
{
	if (a.x + a.w < b.x)
		return false;
	if (b.x + b.w < a.x)
		return false;
	if (a.y + a.h < b.y)
		return false;
	if (b.y + b.h < a.y)
		return false;
	return true;
}
bool collider::areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b)
{
	if (a.x + a.w < b.x)
		return false;
	if (b.x + b.w < a.x)
		return false;
	if (a.y + a.h < b.y)
		return false;
	if (b.y + b.h < a.y)
		return false;
	return true;
	//return SDL_HasIntersection(&a, &b);
}

class Collider
{
public:
	virtual bool test(Entity* a);
};

class ShipToAlienCollider : Collider
{
	bool test(Entity* a)
	{
		auto ship = (Ship*)a;
		for (auto& alien : game->entity_register.getAliens())
		{
			if (collider::isCollidingByRectangles(ship->getCollidable(), alien->getCollidable()))
			{
				collide(*ship, *alien.get());
				return true;
			}
			return false;
		}
	}
	void collide(Ship& ship, Alien& alien)
	{
		alien.damage(100);
		ship.damage(100);
	}
};

class ShipToStationCollider : Collider
{
	void collide(Entity* a, Entity* b)
	{
		auto ship = (Ship*)a;
		auto station = (Station*)b;

		if (ship->state.current.vel.x > 100)
			return;

		//station->can_dock = true;
	}
};

class BulletToAlienCollider : Collider
{
	bool test(Entity* a)
	{
		auto bullet = (Bullet*)a;
		for (auto& alien : game->entity_register.getAliens())
		{
			if (collider::isCollidingByRectangles(bullet->getCollidable(), alien->getCollidable()))
			{
				collide(*bullet, *alien.get());
				return true;
			}
			return false;
		}
	}
	void collide(Bullet& bullet, Alien& alien)
	{
		bullet.damage(100);
		alien.damage(100);
	}
};

class AlienBombToStationCollider : Collider
{
	bool test(Entity* a)
	{
		return false;
	}
	void collide(AlienBomb& bomb, Station& station)
	{
		bomb.damage(100);
		station.damage(100);
	}
};

class AlienBombToShipCollider : Collider
{
	bool test(Entity* a)
	{
		return false;
	}
	void collide(AlienBomb& bomb, Ship& ship)
	{
		bomb.damage(100);
		ship.damage(100);
	}
};