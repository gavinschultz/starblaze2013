#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite() : _scaling{ 1 } {}
Sprite::Sprite(unsigned int scaling) : _scaling{ scaling } {}
Sprite::~Sprite() {}