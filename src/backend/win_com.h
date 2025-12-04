#pragma once
#include <wrl/client.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdexcept>
#include <SDL3/SDL_log.h>

void ThrowIfFailed(HRESULT hr);

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

