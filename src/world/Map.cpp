#include "Map.h"

World::Map::Map(const MapParameters& parameters)
{
	m_width = parameters.width;
	m_height = parameters.height;
	m_tileSet = parameters.tileSet;
}

World::Map::~Map()
{
	m_tiles.clear();
}

// Later generate based on map width and height (procedural?)
void World::Map::Generate()
{
	m_tiles = {
		GRASS_TOP_LEFT, GRASS_TOP_1, GRASS_TOP_3, GRASS_TOP_2, GRASS_TOP_3, GRASS_TOP_RIGHT, NONE, NONE, NONE, NONE,
		GRASS_MID_LEFT, GRASS_MID_1, GRASS_MID_3, GRASS_MID_2, GRASS_MID_3, GRASS_MID_RIGHT, NONE, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
		NONE, GRASS_TOP_LEFT, GRASS_TOP_1, GRASS_TOP_3, GRASS_TOP_2, GRASS_TOP_3, GRASS_TOP_RIGHT, NONE, NONE, NONE,
		NONE, GRASS_MID_LEFT, GRASS_MID_1, GRASS_MID_3, GRASS_MID_2, GRASS_MID_3, GRASS_MID_RIGHT, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
		NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	};
}

void World::Map::Draw(SDL_Renderer* renderer)
{
	int mapIndex = 0;
	for (auto& tile : m_tiles)
	{
		int srcX = ((int)tile % m_tileSet.columns) * m_tileSet.gridWidth;
		int srcY = ((int)tile / m_tileSet.columns) * m_tileSet.gridHeight;

		SDL_FRect src{
			(float)srcX,
			(float)srcY,
			(float)m_tileSet.gridWidth,
			(float)m_tileSet.gridHeight
		};

		int destX = (mapIndex % m_width) * m_tileSet.gridWidth;
		int destY = (mapIndex / m_height) * m_tileSet.gridHeight;

		SDL_FRect dest{
			(float)destX,
			(float)destY,
			(float)m_tileSet.gridWidth,
			(float)m_tileSet.gridHeight
		};

		SDL_RenderTexture(renderer, m_tileSet.texture, &src, &dest);
		mapIndex++;
	}
}
