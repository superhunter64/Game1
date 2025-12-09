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

class StaticHeap
{
public:
	StaticHeap(D3D12_DESCRIPTOR_HEAP_TYPE type): m_type(type), m_heap(nullptr) {};
	~StaticHeap() {};

	void Create(ID3D12Device* device, UINT numDescriptors);
	void CreateTexture2D(const std::string& filename);

private:

	ID3D12Device* m_device;

	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	ComPtr<ID3D12DescriptorHeap> m_heap;

	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;

	std::vector<const ID3D12Resource*> m_textures;
};