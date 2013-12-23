#pragma once
#include <memory>
#include "component_base.h"

class TextPlate;

class TextPlateComponent : public Component
{
public:
	TextPlateComponent(TextPlate* textplate) : Component(C::textplate), textplate{ textplate } {}
	bool visible{ false };
	std::unique_ptr<TextPlate> textplate;
};