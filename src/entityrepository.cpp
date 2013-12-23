#include "stdafx.h"
#include <numeric>
#include <unordered_map>
#include "entityrepository.h"
#include "playfield.h"
#include "render\rendersystem.h"
#include "render\textrender.h"

class EntityRepository::impl
{
public:
	std::unique_ptr<PlayField> playfield_;
	std::array<std::vector<std::unique_ptr<Component>>, 20> components_by_type; // indexed by component type; provides a vector of components of that type
	std::array<Range, 10> entity_type_ids;										// indexed by entity type; provides the lower/upper range of IDs for the entity
	std::array<std::unordered_map<unsigned int, unsigned int>, 20> component_indexes_by_entity;		// indexed by [component type][entity id]; provides an index into the components_by_type vector
};

EntityRepository::EntityRepository(std::initializer_list<ComponentType> ctypes, std::initializer_list<std::pair<EntityType, Range>> etypes) : pi{ new impl() }
{
	//for (auto ctype : ctypes)
	//{
	//	pi->components_by_type[ctype] = std::vector<std::unique_ptr<Component>>();
	//	pi->component_indexes_by_entity[ctype] = std::unordered_map<unsigned int, unsigned int>();
	//}
	for (auto etype : etypes)
	{
		pi->entity_type_ids[etype.first] = etype.second;
	}
}
EntityRepository::~EntityRepository() = default;

std::vector<Component*> EntityRepository::getComponentsOfType(ComponentType ctype) const
{
	//return pi->components_by_type[ctype];
	// TODO: replace all this with:
	//		return pi->components_by_type[ctype];
	// have to make changes so clients will accept std::unique_ptr though
	std::vector<Component*> components;
	for (auto& c : pi->components_by_type[ctype])
	{
		components.push_back(c.get());
	}
	return components;
}

std::vector<unsigned int> EntityRepository::getEntityIds(EntityType etype) const
{
	if (!hasEntity(etype))
		return std::vector<unsigned int>{};
	auto id_range = pi->entity_type_ids[etype];
	auto ids = std::vector<unsigned int>(id_range.current - id_range.lower + 1);
	std::iota(ids.begin(), ids.end(), id_range.lower);
	return ids;
}

Component* EntityRepository::getComponentOfTypeForEntity(unsigned int entity_id, ComponentType ctype) const
{
	if (pi->component_indexes_by_entity[ctype].count(entity_id) == 0)
		return nullptr;
	auto component_index = pi->component_indexes_by_entity[ctype][entity_id];
	auto& component = pi->components_by_type[ctype][component_index];
	return component.get();
}

PlayField* EntityRepository::getPlayField() const
{
	return pi->playfield_.get();
}

bool EntityRepository::hasEntity(EntityType type) const
{
	return pi->entity_type_ids[type].hasCurrent();
}

void EntityRepository::registerPlayField(Window window)
{
	pi->playfield_ = std::make_unique<PlayField>(window);
}

void EntityRepository::registerEntity(EntityType etype, std::initializer_list<Component*> components)
{
	auto& entity_ids = pi->entity_type_ids[etype];
	entity_ids.reserve();
	for (auto c : components)
	{
		auto& components_by_type = pi->components_by_type[c->type];
		unsigned int component_index = components_by_type.size();
		components_by_type.push_back(std::unique_ptr<Component>(c));
		pi->component_indexes_by_entity[c->type][entity_ids.current] = component_index;
	}
}