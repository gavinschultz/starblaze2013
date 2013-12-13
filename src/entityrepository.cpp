#include "stdafx.h"
#include "component.h"
#include "playfield.h"

std::vector<Component*> EntityRepository::getComponentsOfType(ComponentType type)
{
	return std::vector<Component*>{};
}

PlayField* EntityRepository::getPlayField()
{
	return playfield.get();
}