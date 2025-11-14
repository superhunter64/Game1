#pragma once
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <vector>
#include <string>

struct Sprite
{
	// the duration in milliseconds
	Uint32 duration;
	SDL_FRect src;
};

struct Animation
{
	Uint16 from;
	Uint16 to;
};

struct SpriteSheet
{
	std::unordered_map<std::string, Animation> animations;
	std::vector<Sprite> sprites;
	SDL_Texture* texture = NULL;
};

struct AnimatedSprite
{
	SpriteSheet* sheet = nullptr;
	Animation anim;
	Sprite frame;

	int animFrame = 0;
	double frameElapsed = 0;
};