#pragma once
#include <SDL3/SDL.h>
#include "Tile.h"
#include <vector>

namespace World
{
	struct MapParameters
	{
		int width;
		int height;
		TileSet tileSet;
	};

	class Map
	{
	public:
		Map() {};
		Map(const MapParameters& parameters);
		~Map();

		void Generate();
		void Draw(SDL_Renderer* renderer);

	private:
		std::vector<Tile> m_tiles{};
		TileSet m_tileSet{};
		int m_width = 0;
		int m_height = 0;

		SDL_Texture* m_mapTexture = nullptr;
	};
}