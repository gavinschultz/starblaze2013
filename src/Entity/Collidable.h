#pragma once
#include <vector>
#include <typeinfo>
#include "Phys.h"
#include "Entity.h"

class Collider;

class Collidable
{
public:
	Collidable();
	~Collidable();
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const = 0;
	virtual const std::vector<Rect>& getCollisionBoxes() const = 0;
	virtual const Rect getOuterBox() const = 0;
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false) = 0;
	virtual void reset() = 0;
	virtual bool isCollided() const = 0;
	virtual void setCollided() = 0;
	//virtual const std::vector<const Collider*>& getColliders() const = 0;
};

class NonCollidable : public Collidable
{
private:
	static const std::vector<Rect> empty_rect;
public:
	NonCollidable() {}
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const { return empty_rect; }
	virtual const std::vector<Rect>& getCollisionBoxes() const { return empty_rect; }
	virtual const Rect getOuterBox() const { return Rect{}; }
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false) { }
	virtual void reset() { }
	virtual bool isCollided() const { return false; }
	virtual void setCollided() { }
	//virtual const std::vector<const Collider*>& getColliders() const { return std::vector<const Collider*>{}; }
};

class NormalCollidable : public Collidable
{
private:
	bool _is_collided{ false };
	const std::vector<Rect> _base_collision_boxes;
	std::vector<Rect> _collision_boxes;
	const Rect _outer_box;
	//const std::vector<const Collider*> _colliders;
public:
	NormalCollidable(
		const Rect& outer_collision_box, 
		const std::vector<Rect>& collision_boxes
		//,const std::vector<const Collider*>& colliders
		);
	virtual const std::vector<Rect>& getBaseCollisionBoxes() const { return _base_collision_boxes; }
	virtual const std::vector<Rect>& getCollisionBoxes() const { return _collision_boxes; }
	virtual const Rect getOuterBox() const { return _outer_box; }
	virtual void updateCollisionBoxes(Point2D pos, bool hflip = false);
	virtual void reset() { _is_collided = false; }
	virtual bool isCollided() const { return _is_collided; }
	virtual void setCollided() { _is_collided = true; }
	//virtual const std::vector<const Collider*>& getColliders() const { return _colliders; }
};

class DockingCollidable : public Collidable
{

};

namespace collide
{
	bool areRectanglesIntersecting(const Rect& a, const Rect& b);
	bool areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b);
	bool isCollidingByRectangles(Collidable& a, Collidable& b);
	bool isCollidingByTemporalPositionLine(const Collidable& collidable, const TemporalState2D& temporal_position);
}

class Entity;
class Ship;
class Alien;
class Bullet;
class Station;
class AlienBomb;

class Collider
{
public:
	virtual bool test(Entity* a) const = 0;
};

class ShipToAlienCollider : public Collider
{
private:
	void collide(Ship& ship, Alien& alien) const;
public:
	bool test(Entity* a) const;
};

class ShipToStationCollider : public Collider
{
private:
	void collide(Entity* a, Entity* b) const;
public:
	bool test(Entity* a) const;
};

class BulletToAlienCollider : public Collider
{
private:
	void collide(Bullet& bullet, Alien& alien) const;
public:
	bool test(Entity* a) const;
};

class AlienBombToStationCollider : public Collider
{
private:
	void collide(AlienBomb& bomb, Station& station) const;
public:
	bool test(Entity* a) const;
};

class AlienBombToShipCollider : public Collider
{
private:
	void collide(AlienBomb& bomb, Ship& ship) const;
public:
	bool test(Entity* a) const;
};
//
//extern const std::unique_ptr<ShipToAlienCollider> defaultShipToAlienCollider;
//extern const std::unique_ptr<ShipToStationCollider> defaultShipToStationCollider;
//extern const std::unique_ptr<BulletToAlienCollider> defaultBulletToAlienCollider;
//extern const std::unique_ptr<AlienBombToStationCollider> defaultBombToStationCollider;
//extern const std::unique_ptr<AlienBombToShipCollider> defaultBombToShipCollider;