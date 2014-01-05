#include "stdafx.h"
#include <numeric>
#include <unordered_map>
#include "entityrepository.h"
#include "playfield.h"
#include "render\rendersystem.h"
#include "render\textrender.h"
#include <sstream>

EntityRepository::EntityRepository() = default;
EntityRepository::~EntityRepository() = default;

unsigned int EntityRepository::debug_getcomponentsoftype_calls{ 0 };
unsigned int EntityRepository::debug_getentitiesoftype_calls{ 0 };
unsigned int EntityRepository::debug_getcomponentsoftypeforentity_calls{ 0 };
unsigned int EntityRepository::debug_getentitieswithcomponent_calls{ 0 };

std::vector<unsigned int> EntityRepository::getEntitiesOfType(EntityType etype) const
{
	debug_getentitiesoftype_calls++;
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

unsigned int EntityRepository::registerEntity(EntityType etype)
{
	auto& entity_ids = entity_type_ids_[etype];
	auto entity_id = nextId();
	entity_ids.push_back(entity_id);
	return entity_id;
}

std::string EntityRepository::getInfo() const
{
	std::stringstream ss;
	unsigned int total_bytes{ 0 };
	
	ss << "Entity sizes:\n";
	for (auto ids : entity_type_ids_)
	{
		for (auto id : ids)
		{
			auto entity_size = entity_sizes_.at(id);
			ss << "[" << entity_size << " bytes]: entity {" << id << "}" << "\n";
			total_bytes += entity_size;
		}
	}
	ss << "--------------\n";
	ss << "[" << total_bytes << " bytes] total\n";

	ss << "\nComponent sizes:\n";
	ss << "[" << sizeof(Component) << " B]: " << "Component (base)\n";
	ss << "[" << sizeof(Vector2D) << " B]: " << "Vector2D\n";
	ss << "[" << sizeof(Vector2Di) << " B]: " << "Vector2Di\n";
	for (auto c : component_sizes_)
	{
		auto component_type_index = c.first;
		auto component_size = c.second;
		ss << "[" << component_size << " bytes]: " << component_type_index.name() << "\n";
	}

	return ss.str();
}