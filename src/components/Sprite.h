#pragma once
#include <SDL3/SDL_render.h>

struct Sprite
{
	SDL_Texture* texture = nullptr;
	SDL_FRect src{};
};