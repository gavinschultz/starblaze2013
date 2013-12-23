#pragma once

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
	firebullets,
	poweredbody,
	physical,
	collision,
	station
} C;

class Component
{
public:
	Component(ComponentType type) : type{ type } {}
	ComponentType type;
};