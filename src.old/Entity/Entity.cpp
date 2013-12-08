#include "stdafx.h"
#include "Entity.h"
#include "Collidable.h"

Entity::Entity() {}
Entity::~Entity() {}

Collidable& Entity::getCollidable() const 
{ 
	return *(_collidable.get()); 

}
void Entity::updateCollisionBoxes() 
{ 
	_collidable->updateCollisionBoxes(this->state.current.pos); 
}