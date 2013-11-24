#pragma once
//#include <vector>
#include <unordered_set>
//#include <list>
#include <unordered_map>
#include "Entity.h"
#include "Sprite.h"

class SpriteRegister
{
private:
	std::vector<std::unique_ptr<Sprite>> _sprites;
	std::unordered_map<Entity*, Sprite*> _sprites_by_entity;
public:
	void registerSprite(Sprite* sprite);
	void registerSprite(Sprite* sprite, Entity* entity);
	void unregisterSpriteForEntity(Entity* entity);
	const Sprite& getSprite(Entity* entity) const;
	const std::vector<std::unique_ptr<Sprite>>& getSprites();
};