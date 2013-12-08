#pragma once
#include <vector>
#include <memory>

class Entity;

typedef enum ComponentType
{
	input,
	player
} C;

typedef enum EntityType
{
	ship,
	alien,
	station,
	bullet,
	enemybomb
} E;

class Component
{
	
};

class EntityRepository
{
public:
	std::vector<Component*> getComponentsOfType(ComponentType type);
	std::vector<Entity*> getEntitiesWithComponentType(ComponentType type);
	std::vector<Entity*> getFirstEntityWithComponentType(ComponentType type);

	Component* getPlayer();
};

extern std::unique_ptr<EntityRepository> db;

#include "phys.h"
class TemporalStateComponent
{
public:
	State2D current;
	State2D prev;
	Point2D interpolated;
};

class PlayerComponent
{
public:
	int lives{ 0 };
};

class RenderComponent
{

};