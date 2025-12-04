#pragma once
#include "../App.h"
#include "win_com.h"

#include <Windows.h>
#include <dxgi1_6.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>

typedef D3D12_DESCRIPTOR_HEAP_TYPE HEAPTYPE;

struct DX12_DeviceResource
{
	HEAPTYPE Type;
	ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
	UINT DescriptorSize = 0;
};

struct DX12_Command
{
	ComPtr<ID3D12CommandQueue> Queue = nullptr;
	ComPtr<ID3D12GraphicsCommandList> List = nullptr;
	ComPtr<ID3D12CommandAllocator> Allocator = nullptr;
};

class DX12_Pipeline
{
public:

	void Init(HWND hwnd);

	ComPtr<ID3D12Fence1> CreateFence();
	ComPtr<IDXGISwapChain1> CreateSwapChain(int width, int height);
	
	DX12_DeviceResource CreateDeviceResource(HEAPTYPE type);
	DX12_Command CreateCommandObjects(D3D12_COMMAND_LIST_TYPE listType, ID3D12PipelineState* pipelineState = nullptr);

private:

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ComPtr<IDXGISwapChain1> m_swapChain = nullptr;
	ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;
	ComPtr<ID3D12Device9> m_device = nullptr;
	ComPtr<ID3D12Fence1> m_fence = nullptr;
	
	DX12_Command m_cmdDirect = {};

	DX12_DeviceResource m_rtv = {};		// Render Target View
	DX12_DeviceResource m_dsv = {};		// Depth/Stencil View
	DX12_DeviceResource m_CbvSrv = {};	// Constant Buffer, Shader Resource View

	HWND m_hwnd = nullptr;
};
