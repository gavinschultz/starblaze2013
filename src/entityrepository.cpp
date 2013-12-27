#include "stdafx.h"
#include <numeric>
#include <unordered_map>
#include "entityrepository.h"
#include "playfield.h"
#include "render\rendersystem.h"
#include "render\textrender.h"

EntityRepository::EntityRepository(std::initializer_list<std::pair<EntityType, EntityIdRange>> etypes)
{
	for (auto etype : etypes)
	{
		entity_type_ids_[etype.first] = etype.second;
	}
}
EntityRepository::~EntityRepository() = default;

std::vector<unsigned int> EntityRepository::getEntitiesOfType(EntityType etype) const
{
	if (!hasEntity(etype))
		return std::vector<unsigned int>{ 0 };
	auto id_range = entity_type_ids_.at(etype);
	auto ids = std::vector<unsigned int>(id_range.current - id_range.lower + 1);
	std::iota(ids.begin(), ids.end(), id_range.lower);
	return ids;
}

PlayField* EntityRepository::getPlayField() const
{
	return playfield_.get();
}

bool EntityRepository::hasEntity(EntityType type) const
{
	return entity_type_ids_[type].hasCurrent();
}

void EntityRepository::registerPlayField(Window window)
{
	playfield_ = std::make_unique<PlayField>(window);
}

void EntityRepository::registerEntity(EntityType etype, std::initializer_list<Component*> components)
{
	auto& entity_ids = entity_type_ids_[etype];
	entity_ids.reserve();
	for (auto c : components)
	{
		auto component_type = std::type_index(typeid(*c));
		auto& components = components_by_type_[component_type];
		size_t component_index = components.size();
		components.push_back(std::unique_ptr<Component>(c));
		components_by_type_and_entity_[component_type][entity_ids.current] = c;
	}
}