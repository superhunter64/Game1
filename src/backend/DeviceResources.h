#pragma once
#include <Core/stb_image.h>

#include "win_com.h"
#include "primitives/Vertex.h"

#include <Windows.h>
#include <dxgi1_6.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

#include <SDL3/SDL.h>

#include <array>
#include <vector>
#include <algorithm>
#include <iterator>

#include <Core/StaticHeap.h>

namespace DX
{
	using namespace DirectX;

	struct ConstantBuffer
	{
		XMFLOAT4X4 WorldMatrix;
		XMFLOAT4X4 ViewMatrix;
		XMFLOAT4X4 ProjectionMatrix;
		XMFLOAT4 padding[4]; // constant buffer must be 256-byte aligned
	};

	class DeviceResources
	{

	public:
		DeviceResources() {}

		// Set up device resources
		void DisplayAdapters();
		void SetWindow();

		void CreateDeviceResources(UINT backBufferCount);
		void CreateWindowDependentResources();

		// from https://learn.microsoft.com/en-us/windows/win32/direct3d12/creating-a-basic-direct3d-12-component#loadassets
		void LoadAssets();

		template<typename T>
		void LoadVertexBuffer(const std::vector<T>* vertices);
		
		void Render();
		void PopulateCommandList();
		void WaitForGPU();
		void PrepareNextFrame();

		void CreateTextureFromFile(ID3D12Resource* uploadheap, const std::string& filename);

	private:
		
		const static size_t					MAX_BACK_BUFFER_COUNT = 3;
		
		// Window property cache
		int m_width;
		int m_height;

		// Direct3D objects
		ComPtr<ID3D12Device>				m_device;
		ComPtr<ID3D12CommandQueue>			m_commandQueue;
		ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		ComPtr<ID3D12CommandAllocator>		m_commandAllocators[MAX_BACK_BUFFER_COUNT];
		ComPtr<ID3D12RootSignature>			m_rootSignature;
		ComPtr<ID3D12PipelineState>			m_pipelineState;

		ComPtr<ID3D12GraphicsCommandList>	m_bundle;
		ComPtr<ID3D12CommandAllocator>		m_bundleAllocator;

		// Swap chain objects
		ComPtr<IDXGIFactory4>				m_dxgiFactory;
		ComPtr<IDXGISwapChain4>				m_swapChain;
		ComPtr<ID3D12Resource>				m_renderTargets[MAX_BACK_BUFFER_COUNT];
		ComPtr<ID3D12Resource>				m_depthStencil;

		// Sync objects
		ComPtr<ID3D12Fence>					m_fence;
		UINT64								m_fenceValues[MAX_BACK_BUFFER_COUNT];
		WinEvent							m_fenceEvent;

		// Direct3D rendering objects
		ComPtr<ID3D12DescriptorHeap>		m_rtvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap>		m_dsvDescriptorHeap;
		UINT								m_rtvDescriptorSize;
		CD3DX12_VIEWPORT					m_viewport				= CD3DX12_VIEWPORT(0.0f, 0.0f, 1920, 1080);
		CD3DX12_RECT						m_scissorRect			= CD3DX12_RECT(0, 0, 1920, 1080);

		// Device properties
		HWND								m_window;
		D3D_FEATURE_LEVEL                   m_featureLevel;

		// D3D Properties
		DXGI_FORMAT							m_backBufferFormat;
		DXGI_FORMAT							m_depthBufferFormat;
		UINT								m_backBufferIndex;
		UINT								m_backBufferCount;

		// I think this is just establishing vertex information and might be moved out since they're not really 'device resources'
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		ComPtr<ID3D12Resource> m_texture;
		//ComPtr<ID3D12DescriptorHeap> m_textureSrv;
		DescriptorHeap m_textureSrv = {};
	};
}