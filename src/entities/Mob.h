#pragma once
#include <string>

#include "../components/Sprite.h"
#include "../components/Transform.h"

class Mob
{
public:
	Mob(std::string mobName)
	{
		name = mobName;
	}

	std::string name = {};
	AnimatedSprite sprite = {};
	Transform2D transform = {};
};