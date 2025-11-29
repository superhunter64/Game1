#pragma once
#include "Entity.h"
#include "Sprite.h"
#include "Transform.h"

struct Character : Entity
{
	std::string name;
	AnimatedSprite sprite;
};