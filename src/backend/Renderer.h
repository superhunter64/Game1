#pragma once
#include <SDL3/SDL.h>

#include "win_com.h"

enum GFX_API
{
	DIRECTX11,
	DIRECTX12
};

class Renderer
{
public:
	Renderer(GFX_API api) : API(api) {};
	GFX_API API;

private:

};