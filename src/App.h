#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_textengine.h>

#include "Windows.h"

namespace App
{
	inline int Width = 1920;
	inline int Height = 1080;
	inline SDL_Window* Window = nullptr;
	inline SDL_Renderer* Renderer = nullptr;
	inline SDL_AppResult Result = SDL_APP_CONTINUE;
	inline TTF_TextEngine* TextEngine = nullptr;
	inline TTF_Font* TypeFont = nullptr;
	inline double DeltaTime = 0;
	inline Uint64 NOW = SDL_GetPerformanceCounter();
	inline Uint64 LAST = 0;
	inline bool DebugEnabled = false;

	inline HWND hwnd;
	
	inline void Update()
	{
		auto time = SDL_GetTicks();

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		DeltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
	}

	inline void GetWindowSize(int* w, int* h)
	{
		SDL_GetWindowSize(Window, w, h);
	}

	inline void CleanUp()
	{
		TTF_DestroyRendererTextEngine(TextEngine);
		SDL_DestroyRenderer(Renderer);
		SDL_DestroyWindow(Window);
	}
}