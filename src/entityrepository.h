#pragma once
#include <memory>
#include "component\component_base.h"

class PlayField;
struct Window;

typedef enum EntityType
{
	eship,
	ealien,
	estation,
	ebullet,
	eenemybomb,
	eplayfield
} E;

class Entity
{
public:
	unsigned int id;
	EntityType type;
};

struct Range
{
	unsigned int lower;
	unsigned int upper;
	unsigned int current;
	bool hasCurrent() const { return current != std::numeric_limits<unsigned int>::max(); }
	unsigned int reserve() 
	{ 
		if (!hasCurrent())
			current = lower - 1;
		return ++current;
	}
	Range(unsigned int lower = 0, unsigned int upper = 0) : lower{ lower }, upper{ upper }, current{ std::numeric_limits<unsigned int>::max() } {}
};

class EntityRepository
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	EntityRepository(std::initializer_list<ComponentType> ctypes, std::initializer_list<std::pair<EntityType, Range>> etypes);
	~EntityRepository();
	std::vector<Component*> EntityRepository::getComponentsOfType(ComponentType ctype) const;
	
	std::vector<unsigned int> getEntityIds(EntityType etype) const;
	std::vector<unsigned int> getEntitiesWithComponent(ComponentType ctype) const;
	Component* getComponentOfTypeForEntity(unsigned int entity_id, ComponentType ctype) const;

	bool hasEntity(EntityType type) const;
	PlayField* getPlayField() const;

	void registerEntity(EntityType type, std::initializer_list<Component*> components);
	void registerPlayField(Window window);
};

extern std::unique_ptr<EntityRepository> db;