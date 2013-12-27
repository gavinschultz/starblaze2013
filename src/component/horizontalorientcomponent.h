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
	HorizontalOrientComponent() : Component(), direction{ HOrient::right } {}
	HorizontalOrientEnum direction;
};