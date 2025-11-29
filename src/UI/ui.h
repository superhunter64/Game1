#pragma once
#include "../components/Transform.h"

#include <string>
#include <vector>
#include <SDL3_ttf/SDL_ttf.h>
#include "../App.h"

class Element
{
public:
	Transform2D transform = {};

	virtual void Draw() = 0;
};

class Label : public Element
{
public:
	Label() {};

	Label(const char* text, SDL_Color color = SDL_Color(180, 90, 20, SDL_ALPHA_OPAQUE), SDL_FPoint point = {})
	{
		m_text = std::string(text);
		m_color = color;

		SDL_Surface* surf = TTF_RenderText_Blended(App::TypeFont, m_text.c_str(), m_text.size(), m_color);
		m_textTexture = SDL_CreateTextureFromSurface(App::Renderer, surf);

		transform.location.x = point.x;
		transform.location.y = point.y;
		transform.location.w = m_textTexture->w;
		transform.location.h = m_textTexture->h;
		
		SDL_DestroySurface(surf);
		surf = nullptr;
	}

	inline void Draw()
	{
		if (!SDL_RenderTexture(App::Renderer, m_textTexture, NULL, &transform.location))
		{
			SDL_Log("Could not render texture: %s", SDL_GetError());
		}
	}

private:
	SDL_Texture* m_textTexture;
	std::string m_text;
	SDL_Color m_color;
};

class Canvas
{
public:
	std::vector<Element> elements;
	inline void Draw()
	{
		for (auto& e : elements)
		{
			e.Draw();

		}
	}
};