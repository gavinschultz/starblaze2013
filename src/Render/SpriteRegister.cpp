#include "stdafx.h"
#include <algorithm>
#include <memory>
#include "BGSprite.h"
#include "ShipSprite.h"
#include "SpriteRegister.h"
#include "Debug.h"

class SpriteRegister::impl
{
public:
	std::unique_ptr<BGSprite> _background;
	std::unique_ptr<ShipSprite> _ship_sprite;
};

SpriteRegister::SpriteRegister() : pimpl{ new impl{} } {}
SpriteRegister::~SpriteRegister() {}

void SpriteRegister::registerSprite(Sprite* sprite)
{
	this->_sprites.push_back(std::unique_ptr<Sprite>{sprite});
}

void SpriteRegister::registerSprite(Sprite* sprite, Entity* entity)
{
	registerSprite(sprite);
	this->_sprites_by_entity[entity] = sprite;
}

class IsSpriteEqual
{
public:
	IsSpriteEqual(const Sprite* sprite) : _sprite{ sprite }{}
	bool operator()(const std::unique_ptr<Sprite>& s2) const
	{
		return _sprite == s2.get();
	}
private:
	const Sprite* _sprite;
};

void SpriteRegister::unregisterSpriteForEntity(Entity* entity)
{
	auto it = this->_sprites_by_entity.find(entity);
	if (it == this->_sprites_by_entity.end())
	{
		console_debug({ "WARNING: Sprite not found for entity, cannot remove." });
		return;
	}

	_sprites.erase(std::remove_if(_sprites.begin(), _sprites.end(), IsSpriteEqual(it->second)), _sprites.end());
}

const Sprite& SpriteRegister::getSprite(Entity* entity) const
{
	auto it = this->_sprites_by_entity.find(entity);
	if (it == this->_sprites_by_entity.end())
	{
		console_debug({ "WARNING: Sprite not found for entity, cannot return." });
		return *(_sprites[0].get());	// TODO: definitely shouldn't be returning a random sprite if not found!
	}
	return *((*it).second);
}

void SpriteRegister::registerBackground(BGSprite* background)
{
	pimpl->_background = std::unique_ptr<BGSprite>{ background };
}

const BGSprite& SpriteRegister::getBackground() const
{
	auto& a = *(pimpl->_background.get());
	return a;
}

void SpriteRegister::registerShipSprite(ShipSprite* ship_sprite)
{
	pimpl->_ship_sprite = std::unique_ptr<ShipSprite>{ ship_sprite };
}

const ShipSprite& SpriteRegister::getPlayerShip() const
{
	auto& x = *(pimpl->_ship_sprite.get());
	return x;
}

const std::vector<std::unique_ptr<Sprite>>& SpriteRegister::getSprites() const
{
	return this->_sprites;
}