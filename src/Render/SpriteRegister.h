#pragma once

class Sprite;
class Entity;
class BGSprite;
class ShipSprite;
class AlienSprite;
class HUDRend;
class RadarSprite;

class SpriteRegister
{
private:
	std::vector<std::unique_ptr<Sprite>> _sprites;
	std::unordered_map<Entity*, Sprite*> _sprites_by_entity;
	class impl;
	std::unique_ptr<impl> pimpl;
public:
	SpriteRegister();
	~SpriteRegister();
	void registerSprite(Sprite* sprite);
	void registerBackground(BGSprite* background);
	void registerShipSprite(ShipSprite* ship_sprite);
	void registerSprite(Sprite* sprite, Entity* entity);
	void registerHUD(HUDRend* hud);
	void registerRadar(RadarSprite* radar);
	void unregisterSpriteForEntity(Entity* entity);
	const Sprite& getSprite(Entity* entity) const;
	const BGSprite& getBackground() const;
	const ShipSprite& getPlayerShip() const;
	const HUDRend& getHUD() const;
	const RadarSprite& getRadar() const;
	const std::vector<std::unique_ptr<Sprite>>& getSprites() const;
};