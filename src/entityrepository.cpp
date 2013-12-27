#include "stdafx.h"
#include <numeric>
#include <unordered_map>
#include "entityrepository.h"
#include "playfield.h"
#include "render\rendersystem.h"
#include "render\textrender.h"

EntityRepository::EntityRepository() = default;
EntityRepository::~EntityRepository() = default;

std::vector<unsigned int> EntityRepository::getEntitiesOfType(EntityType etype) const
{
	if (!hasEntity(etype))
		return std::vector<unsigned int>{ 0 };
	return entity_type_ids_.at(etype);
}

PlayField* EntityRepository::getPlayField() const
{
	return playfield_.get();
}

bool EntityRepository::hasEntity(EntityType etype) const
{
	return entity_type_ids_[etype].size() > 0;
}

void EntityRepository::registerPlayField(Window window)
{
	playfield_ = std::make_unique<PlayField>(window);
}

void EntityRepository::registerEntity(EntityType etype, std::initializer_list<Component*> components)
{
	auto& entity_ids = entity_type_ids_[etype];
	auto entity_id = nextId();
	entity_ids.push_back(entity_id);
	for (auto c : components)
	{
		auto component_type = std::type_index(typeid(*c));
		auto& components = components_by_type_[component_type];
		size_t component_index = components.size();
		components.push_back(std::unique_ptr<Component>(c));
		components_by_type_and_entity_[component_type][entity_id] = c;
	}
}