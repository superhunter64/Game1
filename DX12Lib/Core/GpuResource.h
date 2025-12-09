#pragma once
#include "DXHelper.h"
#include <d3d12.h>
#include <dxgi1_6.h>

class GpuResource
{
public:
	GpuResource() :
		m_resource(nullptr),
		m_gpuVirtualAddr(0)
	{
	}

	GpuResource(ID3D12Resource* resource) :
		m_resource(resource),
		m_gpuVirtualAddr(0)
	{
	}

	~GpuResource() { Destroy(); }

	virtual void Destroy()
	{
		m_resource = nullptr;
		m_gpuVirtualAddr = 0;
	}

	ID3D12Resource* operator->() { return m_resource.Get(); }
	const ID3D12Resource* operator->() const { return m_resource.Get(); }

	ID3D12Resource* GetResource() { return m_resource.Get(); }
	const ID3D12Resource* GetResource() const { return m_resource.Get(); }
	
	ID3D12Resource** GetAddressOf() { return m_resource.GetAddressOf(); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_gpuVirtualAddr; }

protected:

	// the underlying d3d12 objects
	ComPtr<ID3D12Resource> m_resource;
	D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddr;
};

class UploadBuffer : public GpuResource
{
public:



};