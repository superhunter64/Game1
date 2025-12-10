#pragma once
#include "Gpu.h"
#include <vector>

class Texture2D
{
public:
	Texture2D(): m_resource(nullptr), m_width(0), m_height(0) {};
	~Texture2D() {};

	void LoadFromFile(const std::string& filename);
	const ID3D12Resource* Get() { return m_resource.Get(); }

private:

	int m_width;
	int m_height;

	ComPtr<ID3D12Resource> m_resource;
};


class DescriptorHeap
{
public:
	DescriptorHeap(): m_device(GPU::gDevice) {}
	DescriptorHeap(ID3D12Device* device) : m_device(device) {}

	~DescriptorHeap() { m_device = nullptr; }

	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	void CreateTexture2D(const std::string& filename);

	ID3D12DescriptorHeap* Get() { return m_heap.Get(); }
	const ID3D12DescriptorHeap* Get() const { return m_heap.Get(); }

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_heap->GetGPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_heap->GetCPUDescriptorHandleForHeapStart(); }

	UINT Size() const { return m_descriptorSize; }

private:

	ID3D12Device* m_device;

	D3D12_DESCRIPTOR_HEAP_TYPE m_type = {};
	ComPtr<ID3D12DescriptorHeap> m_heap = nullptr;
	UINT m_descriptorSize;

	std::vector<const ID3D12Resource*> m_textures;
};