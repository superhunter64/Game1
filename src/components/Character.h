#pragma once
#include "Entity.h"
#include "Sprite.h"
#include "Transform.h"

struct Character
{
	std::string name;
	AnimatedSprite sprite;
	Transform2D transform;
};