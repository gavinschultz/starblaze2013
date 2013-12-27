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

//struct EntityIdRange
//{
//	unsigned int lower;
//	unsigned int upper;
//	unsigned int current;
//	bool hasCurrent() const { return current != std::numeric_limits<unsigned int>::max(); }
//	unsigned int reserve()
//	{
//		if (!hasCurrent())
//			current = lower - 1;
//		return ++current;
//	}
//	EntityIdRange(unsigned int lower = 0, unsigned int upper = 0) : lower{ lower }, upper{ upper }, current{ std::numeric_limits<unsigned int>::max() } {}
//};

class EntityRepository
{
private:
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

	// Note that the get functions here are intentionally not const, as the operator[] (which is not const) is marginally more performant than .at()
	// Some error-checking omitted for the same reason

	template<typename T>
	T* getComponentOfTypeForEntity(unsigned int entity_id) // currently only allows a single component of a type per entity
	{
		static const auto component_type = std::type_index(typeid(T));
		auto component = components_by_type_and_entity_[component_type][entity_id];
		return static_cast<T*>(component);
	}

	template<typename T>
	const std::vector<unsigned int> getEntitiesWithComponent()
	{
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

	void registerEntity(EntityType type, std::initializer_list<Component*> components);
	void unregisterEntity(EntityType etype, unsigned int entity_id)
	{
		auto& ids = entity_type_ids_[etype];
		
	}

	void registerPlayField(Window window);
	PlayField* getPlayField() const;
};

extern std::unique_ptr<EntityRepository> db;