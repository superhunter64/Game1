#pragma once
#include "../components/Sprite.h"

namespace Animator
{
	void Update(AnimatedSprite& sprite);
	void Play(AnimatedSprite& sprite, const std::string& animName);
}