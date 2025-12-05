#include "win_com.h"

void ThrowIfFailed(HRESULT hr, const char* msg)
{
	if (FAILED(hr))
	{
		SDL_Log("Direct3D Failure: %s", msg);
		throw std::exception();
	}
}