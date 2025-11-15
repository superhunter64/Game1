#pragma once
#include "Entity.h"
#include "Sprite.h"

struct Character : Entity
{
	std::string name;
	AnimatedSprite sprite;
};