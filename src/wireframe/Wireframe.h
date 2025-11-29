#pragma once
#include <SDL3/SDL.h>
#include "../components/Transform.h"
#include <vector>
#include <memory>
#include "../App.h"

namespace Wireframe
{
	inline std::vector<Transform2D*> transforms = {};

	inline void Add(Transform2D* transform)
	{
		transforms.push_back(transform);
	}

	inline void Draw()
	{
		for (auto& transform : transforms)
		{
			SDL_SetRenderDrawColor(App::Renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderRect(App::Renderer, &transform->location);
		}
	}

	inline void Draw(std::vector<SDL_FRect> trans)
	{
		for (auto transform : trans)
		{
			SDL_SetRenderDrawColor(App::Renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderRect(App::Renderer, &transform);
		}
	}
}