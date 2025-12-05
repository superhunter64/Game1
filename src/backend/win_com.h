#pragma once
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdexcept>
#include <SDL3/SDL_log.h>

typedef Microsoft::WRL::Wrappers::Event WinEvent;

void ThrowIfFailed(HRESULT hr, const char* msg = "");

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

