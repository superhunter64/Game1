#include <SDL3/SDL.h>

namespace App
{
	inline SDL_Window* Window = nullptr;
	inline SDL_Renderer* Renderer = nullptr;
	inline SDL_AppResult Result = SDL_APP_CONTINUE;
	inline double DeltaTime = 0;
	inline Uint64 NOW = SDL_GetPerformanceCounter();
	inline Uint64 LAST = 0;
	
	inline void Update()
	{
		auto time = SDL_GetTicks();

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		DeltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
	}
}