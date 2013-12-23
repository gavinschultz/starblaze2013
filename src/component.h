#pragma once
#include <vector>
#include <memory>
#include "component\component_base.h"
#include "component\temporalstatecomponent.h"
#include "component\physicalcomponent.h"
#include "component\thrustcomponent.h"
#include "component\horizontalorientcomponent.h"
#include "component\collisioncomponent.h"
#include "component\poweredbodycomponent.h"
#include "component\radartrackablecomponent.h"
#include "component\textplatecomponent.h"
#include "component\playercomponent.h"
#include "component\firebulletscomponent.h"
#include "component\stationcomponent.h"

#include"render\textrender.h"	//TODO: get out of here, currently needed for unique_ptr

class PlayField;

typedef enum class EntityType
{
	ship,
	alien,
	station,
	bullet,
	enemybomb,
	playfield
} E;

class Entity
{
public:
	unsigned int id;
	EntityType type;
};

class EntityRepository
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	EntityRepository();
	~EntityRepository();
	std::vector<Component*> getComponentsOfType(ComponentType ctypes);
	Component* getComponentsOfTypeForEntity(EntityType etype, ComponentType ctype);
	//std::vector<Entity*> getEntitiesWithComponentType(ComponentType type);
	//Entity* getFirstEntityWithComponentType(ComponentType type);

	Entity* getPlayer();
	PlayField* getPlayField();

	void registerEntity(EntityType type, std::initializer_list<Component*> components);
	void registerComponent(Component* component);
};

extern std::unique_ptr<EntityRepository> db;