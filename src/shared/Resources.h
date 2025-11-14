#pragma once
#include <SDL3/SDL.h>
#include "../components/Sprite.h"
#include <unordered_map>

namespace Resources
{
	inline std::unordered_map<std::string, SpriteSheet> SpriteSheets;
	inline std::unordered_map<std::string, SDL_Texture*> Textures;
	void LoadTextures(const std::string& path);
	void LoadSpriteSheets(const std::string& path);
}