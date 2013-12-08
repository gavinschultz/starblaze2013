#pragma once

//// detecting collisions
//class CollisionSystem
//{
//public:
//
//};
//
//// moving the player based on velocity, calculating acceleration, thrust etc
//class MovementSystem
//{
//
//};
//
//class 

class CollisionSystem
{

};

class ShipCollisionSystem
{
	void update()
	{
		auto ship = (Ship*)entities.get(E::Ship);
		if (!ship)
			return;

		for (auto& alien : entities.getActive(E::Alien))
		{
			if (collide::isCollidingByRectangles(ship->getComponent(C::Collide), alien->getComponent(C::Collide)))
			{
				alien.damage(100);
				ship.damage(100);
				break;
			}
		}

		auto station = entities.get(E::Station);
		if (station)
		{
			if (ship->state.current.vel.x > 100)
				return;

			if (collide::isCollidingByRectangles(ship->getComponent(C::Collide), alien->getComponent(C::Collide)))
			{
				alien.damage(100);
				ship.damage(100);
				continue;
			}

			//station->can_dock = true;
		}
	}
};

class BulletCollisionSystem
{

};

class BombCollisionSystem
{

};

void main()
{

}