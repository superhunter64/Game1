#pragma once

#include <memory>

#include "GpuResource.h"
#include "Util.h"

#define BACK_BUFFER_COUNT 2
#define BACK_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace GPU
{
	inline ID3D12Device* gDevice;
	inline HWND gWindow;

	void SetWindow(HWND hwnd);
	void DisplayAdapters();
	void Init();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE GetFeatureData();
}