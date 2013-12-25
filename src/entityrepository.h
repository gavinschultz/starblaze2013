#pragma once
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <typeindex>
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
	std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> components_by_type_;	// indexed by component type; provides a vector of components of that type
	//std::array<std::vector<std::unique_ptr<Component>>, 20> components_by_type; // indexed by component type; provides a vector of components of that type
	std::array<Range, 10> entity_type_ids_;										// indexed by entity type; provides the lower/upper range of IDs for the entity
	std::unordered_map<std::type_index, std::unordered_map<unsigned int, unsigned int>> component_indexes_by_entity_;	// indexed by [component type][entity id]; provides an index into the components_by_type vector
	//std::array<std::unordered_map<unsigned int, unsigned int>, 20> component_indexes_by_entity;		// indexed by [component type][entity id]; provides an index into the components_by_type vector

public:
	EntityRepository(std::initializer_list<std::pair<EntityType, Range>> etypes);
	~EntityRepository();

	template<typename T>
	T* getComponentOfTypeForEntity(unsigned int entity_id, ComponentType ctype) const // currently only allows a single component of a type per entity
	{
		static auto component_type = std::type_index(typeid(T));
		if (component_indexes_by_entity_.count(component_type) == 0)
			return nullptr;
		auto& indexes = component_indexes_by_entity_.at(component_type);
		if (indexes.count(entity_id) == 0)
			return nullptr;
		auto component_index = indexes.at(entity_id);
		auto& component = components_by_type_.at(component_type).at(component_index);
		return static_cast<T*>(component.get());
	}

	template<typename T>
	const std::vector<unsigned int> getEntitiesWithComponent() const
	{
		static auto component_type = std::type_index(typeid(T));
		auto& id_map = component_indexes_by_entity_.at(component_type);
		auto ids = std::vector<unsigned int>();
		ids.reserve(id_map.size());
		for (auto map_item : id_map)
		{
			ids.push_back(map_item.first);
		}
		return ids;
	}

	template<typename T>
	std::vector<T*> getComponentsOfType() const
	{
		static auto component_type = std::type_index(typeid(T));
		if (components_by_type_.count(component_type) == 0)
			return std::vector<T*> {};
		auto& components_with_type = components_by_type_.at(component_type);
		std::vector<T*> components;
		components.reserve(components_with_type.size());
		for (auto& c : components_with_type)
		{
			components.push_back(static_cast<T*>(c.get()));
		}
		return components;
	}

	std::vector<unsigned int> getEntitiesOfType(EntityType etype) const;
	//std::vector<unsigned int> getEntitiesWithComponent(ComponentType ctype) const;

	bool hasEntity(EntityType type) const;

	void registerEntity(EntityType type, std::initializer_list<Component*> components);

	void registerPlayField(Window window);
	PlayField* getPlayField() const;
};

extern std::unique_ptr<EntityRepository> db;