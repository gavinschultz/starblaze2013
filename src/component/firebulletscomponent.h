#pragma once
#include "component_base.h"

class FireBulletsComponent : public Component
{
private:
	bool fire_{ false };
public:
	FireBulletsComponent() : Component(C::cfirebullets) {}
	void fire() { fire_ = true; }
	bool isFireRequired() { return fire_; }
	void reset() { fire_ = false; }
};