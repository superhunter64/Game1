#pragma once
#include <SDL3/SDL.h>

namespace World
{
	enum Tile
	{
		NONE = -1,
		GRASS_TOP_LEFT,
		GRASS_TOP_1,
		GRASS_TOP_2,
		GRASS_TOP_3,
		GRASS_TOP_RIGHT,

		GRASS_MID_LEFT = 25,
		GRASS_MID_1,
		GRASS_MID_2,
		GRASS_MID_3,
		GRASS_MID_RIGHT,
	};

	struct TileSet
	{
		int gridWidth;
		int	gridHeight;
		int columns;
		SDL_Texture* texture;
	};
}