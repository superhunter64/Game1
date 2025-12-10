#pragma once

#include "Gpu.h"
#include "DescriptorHeap.h"
#include "GpuResource.h"

class Display
{
public:
	Display(){}

	void CreateFactory();
	void CreateSwapchain(ID3D12CommandQueue* cmdQueue, int w, int h);
	void SetDepthStencil();

	IDXGISwapChain4* GetSwapChain() { return m_swapChain.Get(); }
	const IDXGISwapChain4* GetSwapChain() const { return m_swapChain.Get(); }

	IDXGIFactory4* GetDxgiFactory() { return m_dxgiFactory.Get(); }
	const IDXGIFactory4* GetDxgiFactory() const { return m_dxgiFactory.Get(); }

	UINT GetCurrentBackBufferIndex() { return m_swapChain->GetCurrentBackBufferIndex(); }

	const DescriptorHeap* GetRTVDescriptorHeap() const { return &m_rtvHeap; }
	const DescriptorHeap* GetDSVDescriptorHeap() const { return &m_dsvHeap; }

	CD3DX12_RESOURCE_BARRIER Render();
	CD3DX12_RESOURCE_BARRIER Present();

protected:

	ID3D12CommandQueue* m_commandQueue;

	ComPtr<IDXGISwapChain4> m_swapChain;
	ComPtr<IDXGIFactory4> m_dxgiFactory;

	DescriptorHeap m_rtvHeap;
	DescriptorHeap m_dsvHeap;

	GpuResource m_renderTarget[BACK_BUFFER_COUNT];
	GpuResource m_depthStencil;

	int m_width;
	int m_height;
};