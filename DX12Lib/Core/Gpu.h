#pragma once

#include <memory>

#include "GpuResource.h"
#include "Util.h"

#define BACK_BUFFER_COUNT 2

namespace GPU
{
	inline ID3D12Device* gDevice;
	inline HWND gWindow;

	void DisplayAdapters();
	void Init();
}