#pragma once
#include <SDL3/SDL.h>

struct Transform2D
{
	SDL_FRect location;
	float rotation;
	SDL_FPoint scale;
};