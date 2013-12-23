#pragma once
#include "component_base.h"

typedef enum class HorizontalOrientEnum
{
	left,
	right
} HOrient;

class HorizontalOrientComponent : public Component
{
public:
	HorizontalOrientComponent() : Component(C::chorient), direction{ HOrient::right } {}
	HorizontalOrientEnum direction;
};