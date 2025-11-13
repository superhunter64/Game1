#include "../App.h"
#include "Resources.h"
#include <SDL3_image/SDL_image.h>

using namespace Resources;

void LoadTextures(std::string_view path)
{
	// Lookup texture config at path
	// parse
	// create textures from config using SDL_image

	std::string file;

	SDL_Surface* surface = IMG_Load(file.c_str());
	if (not surface)
	{
		SDL_Log("Image loading failed: %s", SDL_GetError());
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(App::Renderer, surface);
	if (not texture)
	{
		SDL_Log("Creating texture failed: %s", SDL_GetError());
	}

	surface = NULL;
}