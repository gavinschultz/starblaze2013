#pragma once

typedef enum ComponentType
{
	cinput = 0,
	cplayer,
	crender,
	ctemporalstate,
	cthrust,
	chorient,
	cradartrackable,
	ctextplate,
	cfirebullets,
	cpoweredbody,
	cphysical,
	ccollision,
	cstation
} C;

class Component
{
public:
	Component(ComponentType type) : type{ type } {}
	virtual ~Component() = default;
	ComponentType type;
private:
	Component(const Component&) {};	// no copying
	Component& operator=(const Component&) {};	// no copying
};