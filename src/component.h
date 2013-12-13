#pragma once
#include <vector>
#include <memory>

class Entity;
class PlayField;

typedef enum class ComponentType
{
	input,
	player,
	render
} C;

typedef enum class EntityType
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
	PlayField* getPlayField();
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

//#include "render/renderable.h"
//class RenderComponent
//{
//public:
//	bool is_visible{ false };
//	std::unique_ptr<Renderable> renderable;
//};