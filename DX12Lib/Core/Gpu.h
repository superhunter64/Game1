#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include <memory>
#include <vector>

#include "Util.h"
#include "DXHelper.h"

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

	inline const D3D12_INPUT_ELEMENT_DESC StandardVertexLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	inline const UINT StandardVertexStride = 32;

	inline const D3D12_INPUT_ELEMENT_DESC FlatVertexLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	inline const UINT FlatVertexStride = 20;
}