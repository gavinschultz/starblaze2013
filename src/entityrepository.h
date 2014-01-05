#pragma once
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include "component\component_base.h"

/*
I'm aware at this point that this implementation is naive and bad, but I'm going to let it go because I can still make a working game with it - performance is not
great but it's not killing the whole project.

But for future reference:

1) The idea of the vector in the components_by_type_ structure was to have contiguous memory for components, but it's not really contiguous - the pointers
are contiguous but the actual memory the components are stored at is completely arbitrary.

Use memory pools such that the components can actually be stored contiguously.
Some good ideas here on using intrusive lists and pooling (specifically using boost libraries, but could make our own): http://stackoverflow.com/a/17838320/78216

2) Separating the component types via the unordered_map is flawed for a similar reason - typically we need coordination between many different components (e.g.
~5 components in the physics update), but keeping them separate and looking them up one-by-one also bounces all over our memory space.

Instead, store redundant component data in larger component aggregates - e.g. a "physics" component may store copies of all the other 5 components
worth of data (at least, the bits it needs) so that all the data is stored contiguously and close together

3) Item removal is slow because we need to iterate through all of the component lists in components_by_type_and_entity_.

*/

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

class EntityRepository
{
private:
	std::unordered_map<std::type_index, unsigned int> component_sizes_;
	std::map<unsigned int, unsigned int> entity_sizes_;
	std::unique_ptr<PlayField> playfield_;
	std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> components_by_type_;					// indexed by component type; provides a vector of components of that type
	//std::array<EntityIdRange, 10> entity_type_ids_;																		// indexed by entity type; provides the lower/upper range of IDs for the entity
	std::array<std::vector<unsigned int>, 10> entity_type_ids_;
	std::unordered_map<std::type_index, std::unordered_map<unsigned int, Component*>> components_by_type_and_entity_;

	unsigned int nextId()
	{
		static unsigned int max_entity_id = 0;
		max_entity_id++;
		return max_entity_id;
	}

public:
	EntityRepository();
	~EntityRepository();

	static unsigned int debug_getcomponentsoftype_calls;
	static unsigned int debug_getentitiesoftype_calls;
	static unsigned int debug_getcomponentsoftypeforentity_calls;
	static unsigned int debug_getentitieswithcomponent_calls;

	// Note that the get functions here are intentionally not const, as the operator[] (which is not const) is marginally more performant than .at()
	// Some error-checking omitted for the same reason

	template<typename T>
	void registerComponent(T* component, unsigned int entity_id)
	{
		auto component_type = std::type_index(typeid(T));
		auto& components = components_by_type_[component_type];
		size_t component_index = components.size();
		components.push_back(std::unique_ptr<Component>(component));
		components_by_type_and_entity_[component_type][entity_id] = component;

		if (component_sizes_.count(component_type) == 0)
			component_sizes_[component_type] = sizeof(T);
		entity_sizes_[entity_id] += sizeof(T);
	}

	template<typename T>
	T* getComponentOfTypeForEntity(unsigned int entity_id) // currently only allows a single component of a type per entity
	{
		debug_getcomponentsoftypeforentity_calls++;
		static const auto component_type = std::type_index(typeid(T));
		auto component = components_by_type_and_entity_[component_type][entity_id];
		return static_cast<T*>(component);
	}

	template<typename T>
	const std::vector<unsigned int> getEntitiesWithComponent()
	{
		debug_getentitieswithcomponent_calls++;
		static const auto component_type = std::type_index(typeid(T));
		auto& id_map = components_by_type_and_entity_[component_type];
		auto ids = std::vector<unsigned int>();
		ids.reserve(id_map.size());
		for (auto map_item : id_map)
		{
			ids.push_back(map_item.first);
		}
		return ids;
	}

	template<typename T>
	const std::vector<T*> getComponentsOfType()
	{
		debug_getcomponentsoftype_calls++;
		static const auto component_type = std::type_index(typeid(T));
		auto& components_with_type = components_by_type_[component_type];
		std::vector<T*> components;
		components.reserve(components_with_type.size());
		for (auto& c : components_with_type)
		{
			components.push_back(static_cast<T*>(c.get()));
		}
		return components;
	}

	template<typename T>
	const std::vector<T*> getComponentsOfTypeForEntityType(EntityType etype)
	{
		std::vector<T*> components;

		static const auto component_type = std::type_index(typeid(T));
		if (!hasEntity(etype)) // TODO:delete?
			return std::vector<T*>{};
		auto& eids = entity_type_ids_[etype];

		auto& components_with_type = components_by_type_[component_type];
		for (auto eid : eids)
		{
			auto component = components_by_type_and_entity_[component_type][eid];
			components.push_back(static_cast<T*>(component));
		}

		return components;
	}

	std::vector<unsigned int> getEntitiesOfType(EntityType etype) const;

	bool hasEntity(EntityType type) const;

	unsigned int registerEntity(EntityType type);
	void unregisterEntity(EntityType etype, unsigned int entity_id)
	{
		//auto& ids = entity_type_ids_[etype];
		//for (auto components_by_entity : components_by_type_and_entity_)
		//{
		//	auto c = components_by_entity.second.find(entity_id);
		//	components_by_entity.second.at(entity_id) = nullptr;
		//}
	}

	void registerPlayField(Window window);
	PlayField* getPlayField() const;
	std::string getInfo() const;
};

extern std::unique_ptr<EntityRepository> db;