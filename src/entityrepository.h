#pragma once
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
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
	std::unique_ptr<PlayField> playfield_;
	std::array<std::vector<std::unique_ptr<Component>>, 20> components_by_type; // indexed by component type; provides a vector of components of that type
	std::array<Range, 10> entity_type_ids;										// indexed by entity type; provides the lower/upper range of IDs for the entity
	std::array<std::unordered_map<unsigned int, unsigned int>, 20> component_indexes_by_entity;		// indexed by [component type][entity id]; provides an index into the components_by_type vector

public:
	EntityRepository(std::initializer_list<std::pair<EntityType, Range>> etypes);
	~EntityRepository();

	std::vector<Component*> getComponentsOfType(ComponentType ctype) const;

	template<typename T>
	T* getComponentOfTypeForEntity(unsigned int entity_id, ComponentType ctype) const // currently only allows a single component of a type per entity
	{
		auto indexes = component_indexes_by_entity[ctype];
		if (indexes.count(entity_id) == 0)
			return nullptr;
		auto component_index = indexes[entity_id];
		auto& component = components_by_type[ctype][component_index];
		return static_cast<T*>(component.get());
	}

	std::vector<unsigned int> getEntitiesOfType(EntityType etype) const;
	std::vector<unsigned int> getEntitiesWithComponent(ComponentType ctype) const;

	bool hasEntity(EntityType type) const;

	void registerEntity(EntityType type, std::initializer_list<Component*> components);

	void registerPlayField(Window window);
	PlayField* getPlayField() const;
};

extern std::unique_ptr<EntityRepository> db;