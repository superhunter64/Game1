#pragma once
#include <string>

#include "../components/Sprite.h"
#include "../components/Transform.h"

struct MobInfo
{
	std::string name;

};

class Mob
{
public:
	Mob(std::string mobName)
	{
		name = mobName;
	}

	void Draw() const
	{
		auto dest = SDL_FRect(transform.location.x, transform.location.y, sprite.frame.src.w, sprite.frame.src.h);
		SDL_RenderTexture(App::Renderer, sprite.sheet->texture, &sprite.frame.src, &dest);
	}

	std::string name = {};
	AnimatedSprite sprite = {};
	Transform2D transform = {};
};