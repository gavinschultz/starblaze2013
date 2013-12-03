#include "stdafx.h"
#include "Debris.h"

Debris::Debris(const State2D& origin)
{
	auto item = new DebrisItem();
	item->current_state.pos.x = origin.pos.x + 10;
	item->current_state.pos.y = origin.pos.y + 15;
	this->_items.push_back(std::unique_ptr<DebrisItem>(item));

	item = new DebrisItem();
	item->current_state.pos.x = origin.pos.x - 7;
	item->current_state.pos.y = origin.pos.y - 18;
	this->_items.push_back(std::unique_ptr<DebrisItem>(item));
}

Debris::~Debris() {}

const std::vector<std::unique_ptr<DebrisItem>>& Debris::getItems() const
{
	return this->_items;
}