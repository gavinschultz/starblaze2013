#include "stdafx.h"
#include "Phys.h"
#include "Collidable.h"
#include "Entity.h"
#include "Util.h"

// for colliders
#include "AlienBomb.h"
#include "Bullet.h"
#include "Station.h"
#include "Alien.h"
#include "Ship.h"
#include "Util.h"
#include "Game.h"

//TODO: make templates
bool collide::areRectanglesIntersecting(const Rect& a, const Rect& b)
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

bool collide::areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b)
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

bool collide::isCollidingByRectangles(Collidable& a, Collidable& b)
{
	if (!collide::areRectanglesIntersecting(a.getOuterBox(), b.getOuterBox()))
		return false;

	for (auto& coll_a : a.getCollisionBoxes())
	{
		for (auto& coll_b : b.getCollisionBoxes())
		{
			if (collide::areRectanglesIntersecting(coll_a, coll_b))
			{
				a.setCollided();	// don't really like having mutability in a "is..." function, but this is convenient
				b.setCollided();
				return true;
			}
		}
	}
	return false;
}
bool collide::isCollidingByTemporalPositionLine(const Collidable& collidable, const TemporalState2D& temporal_position)
{
	return false;
}


NormalCollidable::NormalCollidable(
	const Rect& outer_box,
	const std::vector<Rect>& collision_boxes,
	const std::vector<const Collider*>& colliders)
	: _outer_box{ outer_box },
	_collision_boxes{ collision_boxes },
	_colliders{ colliders }
{}

void NormalCollidable::updateCollisionBoxes(Point2D pos, bool hflip)
{
	this->reset();
	const auto& base_rects = this->getBaseCollisionBoxes();

	if (!hflip)
	{
		for (unsigned int i = 0; i < base_rects.size(); i++)
		{
			_collision_boxes[i].x = pos.x + base_rects[i].x;
			_collision_boxes[i].y = pos.y + base_rects[i].y;
		}
	}
	else
	{
		// Reverse X positions if flipped
		auto base_rects = getBaseCollisionBoxes();
		for (unsigned int i = 0; i < base_rects.size(); i++)
		{
			_collision_boxes[i].x = pos.x + (_outer_box.w - (base_rects[i].x + base_rects[i].w));
			_collision_boxes[i].y = pos.y + base_rects[i].y;
		}
	}
}

bool ShipToAlienCollider::test(Entity* a) const
{
	auto ship = (Ship*)a;
	for (auto& alien : game->entity_register.getAliens())
	{
		if (collide::isCollidingByRectangles(ship->getCollidable(), alien->getCollidable()))
		{
			collide(*ship, *alien.get());
			return true;
		}
		return false;
	}
}

void ShipToAlienCollider::collide(Ship& ship, Alien& alien) const
{
	alien.damage(100);
	ship.damage(100);
}

bool ShipToStationCollider::test(Entity* a) const
{
	return false;
}

void ShipToStationCollider::collide(Entity* a, Entity* b) const
{
	auto ship = (Ship*)a;
	auto station = (Station*)b;

	if (ship->state.current.vel.x > 100)
		return;

	//station->can_dock = true;
}

bool BulletToAlienCollider::test(Entity* a) const
{
	auto bullet = (Bullet*)a;
	for (auto& alien : game->entity_register.getAliens())
	{
		if (collide::isCollidingByRectangles(bullet->getCollidable(), alien->getCollidable()))
		{
			collide(*bullet, *alien.get());
			return true;
		}
		return false;
	}
}
void BulletToAlienCollider::collide(Bullet& bullet, Alien& alien) const
{
	bullet.damage(100);
	alien.damage(100);
}

bool AlienBombToStationCollider::test(Entity* a) const
{
	return false;
}
void AlienBombToStationCollider::collide(AlienBomb& bomb, Station& station) const
{
	bomb.damage(100);
	station.damage(100);
}

bool AlienBombToShipCollider::test(Entity* a) const
{
	return false;
}
void AlienBombToShipCollider::collide(AlienBomb& bomb, Ship& ship) const
{
	bomb.damage(100);
	ship.damage(100);
}

const std::unique_ptr<ShipToAlienCollider> defaultShipToAlienCollider{ new ShipToAlienCollider() };
const std::unique_ptr<ShipToStationCollider> defaultShipToStationCollider{ new ShipToStationCollider() };
const std::unique_ptr<BulletToAlienCollider> defaultBulletToAlienCollider{ new BulletToAlienCollider() };
const std::unique_ptr<AlienBombToStationCollider> defaultBombToStationCollider{ new AlienBombToStationCollider() };
const std::unique_ptr<AlienBombToShipCollider> defaultBombToShipCollider{ new AlienBombToShipCollider() };