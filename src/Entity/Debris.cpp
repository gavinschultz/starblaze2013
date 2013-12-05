#include "stdafx.h"
#include "Debris.h"

Debris::Debris(const State2D& origin)
{
	auto item = new DebrisItem();
	item->state.current.pos.x = origin.pos.x + 10;
	item->state.current.pos.y = origin.pos.y + 15;
	this->_items.push_back(std::unique_ptr<DebrisItem>(item));

	item = new DebrisItem();
	item->state.current.pos.x = origin.pos.x - 7;
	item->state.current.pos.y = origin.pos.y - 18;
	this->_items.push_back(std::unique_ptr<DebrisItem>(item));
}

Debris::~Debris() {}

const std::vector<std::unique_ptr<DebrisItem>>& Debris::getItems() const
{
	return this->_items;
}

void Debris::tick(double dt)
{
	for (auto& item : this->_items)
	{
		if (!item->isAlive())
			continue;

		item->lifetime -= dt;
	}
}