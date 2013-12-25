#include "stdafx.h"
#include <numeric>
#include <unordered_map>
#include "entityrepository.h"
#include "playfield.h"
#include "render\rendersystem.h"
#include "render\textrender.h"

EntityRepository::EntityRepository(std::initializer_list<std::pair<EntityType, Range>> etypes)
{
	for (auto etype : etypes)
	{
		entity_type_ids[etype.first] = etype.second;
	}
}
EntityRepository::~EntityRepository() = default;

std::vector<Component*> EntityRepository::getComponentsOfType(ComponentType ctype) const
{
	// TODO: replace all this with:
	//		return components_by_type[ctype];
	// have to make changes so clients will accept std::unique_ptr though
	std::vector<Component*> components;
	components.reserve(components_by_type[ctype].size());
	for (auto& c : components_by_type[ctype])
	{
		components.push_back(c.get());
	}
	return components;
}

std::vector<unsigned int> EntityRepository::getEntitiesOfType(EntityType etype) const
{
	if (!hasEntity(etype))
		return std::vector<unsigned int>{ 0 };
	auto id_range = entity_type_ids[etype];
	auto ids = std::vector<unsigned int>(id_range.current - id_range.lower + 1);
	std::iota(ids.begin(), ids.end(), id_range.lower);
	return ids;
}

std::vector<unsigned int> EntityRepository::getEntitiesWithComponent(ComponentType ctype) const
{
	auto id_map = component_indexes_by_entity[ctype];
	auto ids = std::vector<unsigned int>();
	ids.reserve(id_map.size());
	for (auto map_item : id_map)
	{
		ids.push_back(map_item.first);
	}
	return ids;
}

PlayField* EntityRepository::getPlayField() const
{
	return playfield_.get();
}

bool EntityRepository::hasEntity(EntityType type) const
{
	return entity_type_ids[type].hasCurrent();
}

void EntityRepository::registerPlayField(Window window)
{
	playfield_ = std::make_unique<PlayField>(window);
}

void EntityRepository::registerEntity(EntityType etype, std::initializer_list<Component*> components)
{
	auto& entity_ids = entity_type_ids[etype];
	entity_ids.reserve();
	for (auto c : components)
	{
		auto& components = components_by_type[c->type];
		size_t component_index = components.size();
		components.push_back(std::unique_ptr<Component>(c));
		component_indexes_by_entity[c->type][entity_ids.current] = component_index;
	}
}