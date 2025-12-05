#pragma once
#include "../App.h"
#include "win_com.h"

#include <Windows.h>
#include <dxgi1_6.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <d3dx12.h>

typedef D3D12_DESCRIPTOR_HEAP_TYPE HEAPTYPE;

/// <summary>
/// A handle to the D3D12 swap chain, the current back buffer index, and the back buffer count
/// </summary>
struct DX12_SwapChain
{
	UINT CurrentBackBufferIndex = 0;
	UINT BackBufferCount = 2;
	ComPtr<IDXGISwapChain1> Handle = nullptr;
};

/// <summary>
/// A DirectX 12 Device Resource:
/// 
/// Holds the descriptor heap, type, and size
/// </summary>
struct DX12_DeviceResource
{
	HEAPTYPE Type;
	ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
	UINT DescriptorSize = 0;
};

/// <summary>
/// DirectX 12 Command Objects: Queue, List, and Allocator
/// </summary>
struct DX12_Command
{
	ComPtr<ID3D12CommandQueue> Queue = nullptr;
	ComPtr<ID3D12GraphicsCommandList> List = nullptr;
	ComPtr<ID3D12CommandAllocator> Allocator = nullptr;
};

D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView(const DX12_DeviceResource* dr, UINT backBufferIndex);
D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(const DX12_DeviceResource* dr);

/// <summary>
/// Establishes a default pipeline for DirectX 12 rendering, and exposes an interface to create custom pipelines by creating DX12 objects
/// </summary>
class DX12_Pipeline
{
public:

	void Init(HWND hwnd);

	ComPtr<ID3D12Fence1> CreateFence();
	DX12_SwapChain CreateSwapChain(int width, int height, UINT backBufferCount = 2);
	
	DX12_DeviceResource CreateDeviceResource(HEAPTYPE type, UINT numDescriptors = 1);
	DX12_Command CreateCommandObjects(D3D12_COMMAND_LIST_TYPE listType, ID3D12PipelineState* pipelineState = nullptr);

private:

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;
	ComPtr<ID3D12Device9> m_device = nullptr;
	ComPtr<ID3D12Fence1> m_fence = nullptr;

	DX12_SwapChain m_swapChain = {};

	DX12_Command m_cmdDirect = {};

	DX12_DeviceResource m_rtv = {};		// Render Target View
	DX12_DeviceResource m_dsv = {};		// Depth/Stencil View
	DX12_DeviceResource m_CbvSrv = {};	// Constant Buffer, Shader Resource View

	HWND m_hwnd = nullptr;
};
