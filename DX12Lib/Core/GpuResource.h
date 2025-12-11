#pragma once
#include "Gpu.h"
#include "DXHelper.h"
#include "DescriptorHeap.h"

class DirectUploadBuffer;

struct TextureStruct
{
	unsigned char* data;
	UINT width;
	UINT height;
};

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

class Texture2D : public GpuResource
{
	friend DirectUploadBuffer;

public:

	Texture2D() : m_name(L"") {};
	Texture2D(wchar_t* name) : m_name(name) {};

	void Create(unsigned char* data, int width, int height);

protected:

	unsigned char* m_data;
	D3D12_SUBRESOURCE_DATA m_textureData;

	int m_width;
	int m_height;
	std::wstring m_name;
};

class DirectUploadBuffer
{

public:
	DirectUploadBuffer() {};

	void UploadTextures(ID3D12GraphicsCommandList* cmdList, DescriptorHeap* srvHeap, std::vector<Texture2D>& textures);
	void FillBuffer(std::vector<ComPtr<ID3D12Resource>>& resources);

private:

	std::vector<ComPtr<ID3D12Resource>> m_uploads = {};
};
