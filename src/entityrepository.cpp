#include "stdafx.h"
#include "component.h"
#include "playfield.h"
#include "render\rendersystem.h"

class EntityRepository::impl
{
public:
	std::unique_ptr<Entity> ship_;
	std::unique_ptr<PlayField> playfield_;
	std::vector<std::unique_ptr<Component>> components_;
};

EntityRepository::EntityRepository() : pi{ new impl() } {}
EntityRepository::~EntityRepository() = default;

std::vector<Component*> EntityRepository::getComponentsOfType(ComponentType ctype)
{
	std::vector<Component*> matches;
	for (auto& c : pi->components_)
	{
		if (c->type == ctype)
			matches.push_back(c.get());
	}
	return matches;
}

Component* EntityRepository::getComponentsOfTypeForEntity(EntityType etype, ComponentType ctype)
{
	for (auto& c : pi->components_)
	{
		if (c->type == ctype)
			return c.get();
	}
	return nullptr;
}

PlayField* EntityRepository::getPlayField()
{
	return pi->playfield_.get();
}

Entity* EntityRepository::getPlayer()
{
	return pi->ship_.get();
}

void EntityRepository::registerEntity(EntityType type, std::initializer_list<Component*> components)
{
	if (type == EntityType::ship)
	{
		pi->ship_ = std::make_unique<Entity>();
		for (auto c : components)
		{
			pi->components_.push_back(std::unique_ptr<Component>(c));
		}
	}
	else if (type == EntityType::playfield)
	{
		pi->playfield_ = std::make_unique<PlayField>(renderer->getWindow());
	}
}