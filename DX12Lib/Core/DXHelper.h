#pragma once
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdexcept>

void ThrowIfFailed(HRESULT hr);

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;