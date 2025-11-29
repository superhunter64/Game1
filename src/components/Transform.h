#pragma once
#include <SDL3/SDL.h>

struct Transform2D
{
	SDL_FRect location = {};
	float rotation = 0;
	SDL_FPoint scale = {1, 1};
};