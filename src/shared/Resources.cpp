#include "../App.h"
#include "../system/File.h"
#include "Paths.h"
#include "Resources.h"
#include <SDL3_image/SDL_image.h>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

std::string FullPath(const std::string& directory, const std::string& name)
{
	return directory + name;
}

void Resources::LoadTextures(const std::string& path)
{
	// TODO: maybe make a config file format to parse from
	// instead of putting config in code

	std::vector<std::string> files;
	FS::GetFiles(path.c_str(), (void*)&files);

	for (auto& file : files)
	{
		std::string key{};
		std::size_t pos = file.find(".");
		if (pos != std::string::npos)
		{
			key = file.substr(0, pos);
		}
		else
		{
			key = file;
		}

		if (Textures.find(key) != Textures.end())
		{
			continue;
		}

		if (file.find(".png") == std::string::npos) continue;

		SDL_Surface* surface = IMG_Load(FullPath(path, file).c_str());
		if (not surface)
		{
			SDL_Log("Image loading failed: %s", SDL_GetError());
			continue;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(App::Renderer, surface);
		if (not texture)
		{
			SDL_Log("Creating texture failed: %s", SDL_GetError());
		}

		SDL_DestroySurface(surface);
		surface = NULL;

		Textures.emplace(key, texture);
	}
}

void Resources::LoadSpriteSheets(const std::string& path)
{
	std::vector<std::string> files;
	FS::GetFiles(path.c_str(), (void*)&files);
	
	for (auto& file : files)
	{
		if (file.find(".json") == std::string::npos) continue;

		std::string key{};
		std::size_t pos = file.find(".");
		if (pos != std::string::npos)
		{
			key = file.substr(0, pos);
		}
		else
		{
			key = file;
		}

		// parse aseprite json output
		std::ifstream f(FullPath(path, file));
		json data = json::parse(f);

		SpriteSheet sheet{};

		auto& meta = data["meta"];
		auto image = meta["image"].get<std::string>();
		auto& frameTags = meta["frameTags"];
		auto& frames = data["frames"];
		for (auto& frame : frames)
		{
			Sprite sprite{};
			auto src = frame["frame"];

			sprite.duration = frame["duration"].get<int>();
			sprite.src = {
				src["x"].get<float>(),
				src["y"].get<float>(),
				src["w"].get<float>(),
				src["h"].get<float>()
			};

			sheet.sprites.push_back(sprite);
		}

		for (auto& anim : frameTags)
		{
			auto name = anim["name"].get<std::string>();
			auto from = anim["from"].get<Uint16>();
			auto to = anim["to"].get<Uint16>();

			Animation anim { from, to };
			sheet.animations.emplace(name, anim);
		}
		
		if (Textures.find(key) == Textures.end())
		{
			// add the texture
			SDL_Surface* surface = IMG_Load(FullPath(path, image).c_str());
			if (not surface)
			{
				SDL_Log("Image loading failed: %s", SDL_GetError());
				continue;
			}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(App::Renderer, surface);
			if (not texture)
			{
				SDL_Log("Creating texture failed: %s", SDL_GetError());
			}

			SDL_DestroySurface(surface);
			surface = NULL;

			Textures.emplace(key, texture);
			sheet.texture = texture;
		}
	}
}