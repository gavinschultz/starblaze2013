#pragma once
#include <vector>
#include <memory>

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
	radartrackable
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
	//void registerComponent(Component* component);
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

//#include "render/renderable.h"
//class RenderComponent
//{
//public:
//	bool is_visible{ false };
//	std::unique_ptr<Renderable> renderable;
//};