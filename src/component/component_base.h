#pragma once

class Component
{
public:
	Component() {} // : type{ type } {}
	virtual ~Component() = default;
	//ComponentType type;
	bool active{ true };
private:
	Component(const Component&) {};	// no copying
	Component& operator=(const Component&) {};	// no copying
};