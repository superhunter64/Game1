#pragma once
#include "win_com.h"

#include <Windows.h>
#include <dxgi1_6.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <d3dx12.h>

namespace DX
{
	class DeviceResources
	{
		DeviceResources() {}

	public:
		DeviceResources() {}
		void SetWindow(HWND hwnd);


	private:
		// Set up device resources
		void InitDevice();
		
		const static size_t MAX_BACK_BUFFER_COUNT = 3;

		// Direct3D objects
		ComPtr<ID3D12Device>				m_device;
		ComPtr<ID3D12CommandQueue>			m_commandQueue;
		ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		ComPtr<ID3D12CommandAllocator>		m_commandAllocator;

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

		HWND								m_window;
	};
}