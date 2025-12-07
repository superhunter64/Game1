#pragma once
#include "win_com.h"

#include <Windows.h>
#include <dxgi1_6.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

#include <SDL3/SDL.h>

namespace DX
{
	class DeviceResources
	{
		DeviceResources() {}

	public:
//		DeviceResources() {}
		void SetWindow();

		void CreateDeviceResources();
		void CreateWindowDependentResources();

		// from https://learn.microsoft.com/en-us/windows/win32/direct3d12/creating-a-basic-direct3d-12-component#loadassets
		void LoadAssets();

	private:
		// Set up device resources
		void InitDevice();
		
		const static size_t					MAX_BACK_BUFFER_COUNT = 3;
		
		// Direct3D objects
		ComPtr<ID3D12Device>				m_device;
		ComPtr<ID3D12CommandQueue>			m_commandQueue;
		ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		ComPtr<ID3D12CommandAllocator>		m_commandAllocators[MAX_BACK_BUFFER_COUNT];

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
		D3D12_VIEWPORT						m_screenViewport;
		D3D12_RECT							m_scissorRect;

		// Device properties
		HWND								m_window;
		D3D_FEATURE_LEVEL                   m_featureLevel;

		// D3D Properties
		DXGI_FORMAT							m_backBufferFormat;
		DXGI_FORMAT							m_depthBufferFormat;
		UINT								m_backBufferIndex;
		UINT								m_backBufferCount;

		// I think this is just establishing vertex information and might be moved out since they're not really 'device resources'
		ComPtr<ID3DBlob> m_rootSignature;
	};
}