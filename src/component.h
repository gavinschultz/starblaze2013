#pragma once
#include <vector>
#include <memory>

#include"render\textrender.h"	//TODO: get out of here, currently needed for unique_ptr

class Entity;
class PlayField;

typedef enum class ComponentType
{
	input,
	player,
	render,
	temporalstate,
	thrust,
	horient,
	radartrackable,
	textplate,
	firebullets
} C;

typedef enum class EntityType
{
	ship,
	alien,
	station,
	bullet,
	enemybomb,
	playfield
} E;

class Component
{
public:
	Component(ComponentType type) : type{ type } {}
	ComponentType type;
};

class Entity
{
public:
	unsigned int id;
	EntityType type;
};

class Ship : Entity
{
public:
	
};

class EntityRepository
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	EntityRepository();
	~EntityRepository();
	std::vector<Component*> getComponentsOfType(ComponentType ctypes);
	Component* getComponentsOfTypeForEntity(EntityType etype, ComponentType ctype);
	//std::vector<Entity*> getEntitiesWithComponentType(ComponentType type);
	//Entity* getFirstEntityWithComponentType(ComponentType type);

	Entity* getPlayer();
	PlayField* getPlayField();

	void registerEntity(EntityType type, std::initializer_list<Component*> components);
	void registerComponent(Component* component);
};

extern std::unique_ptr<EntityRepository> db;

#include "phys.h"
class TemporalState2DComponent : public Component
{
public:
	TemporalState2DComponent() : Component(C::temporalstate) {}
	State2D current;
	State2D prev;
	Point2D interpolated;
};

class HorizontalOrientComponent : public Component
{
public:
	HorizontalOrientComponent() : Component(C::horient), direction{ HOrient::right } {}
	HorizontalOrientEnum direction;
};

class ThrustComponent : public Component
{
public:
	ThrustComponent() : Component(C::thrust) {}
	Vector2D current;
	Vector2D prev;
	Vector2D max;
	void resetCurrent() { current = { 0, 0 }; }
};

class PlayerComponent : public Component
{
public:
	PlayerComponent() : Component(C::player) {}
	unsigned int lives{ 0 };
	unsigned int bullet_count{ 255 };
	unsigned int shields{ 255 };
	unsigned int radar{ 255 };
	unsigned int fuel{ 255 };
};

class RadarTrackableComponent : public Component
{
public:
	RadarTrackableComponent() : Component(C::radartrackable) {}
	Point2D pos;
	Rect box;
};

class TextPlateComponent : public Component
{
public:
	TextPlateComponent(TextPlate* textplate) : Component(C::textplate), textplate{ textplate } {}
	bool visible{ false };
	std::unique_ptr<TextPlate> textplate;
};

class FireBulletsComponent : public Component
{
private:
	bool fire_{ false };
public:
	FireBulletsComponent() : Component(C::firebullets) {}
	void fire() { fire_ = true; }
	bool isFireRequired() { return fire_; }
	void reset() { fire_ = false; }
};