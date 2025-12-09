#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "StaticHeap.h"

void Texture2D::LoadFromFile(const std::string& filename)
{
    HRESULT hr;
    std::string fullPath = "C:/dev/Game1/Assets/Textures/" + filename;

    int nrChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &m_width, &m_height, &nrChannels, 4);

    // describe and create a texture2d on the cpu
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = m_width;
    textureDesc.Height = m_height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

void StaticHeap::Create(ID3D12Device* device, UINT numDescriptors)
{
	m_device = device;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = m_type;
	desc.NumDescriptors = numDescriptors;
	desc.NodeMask = 0;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));

    m_heap->SetName(L"Static heap 1");
    m_cpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
}

void StaticHeap::CreateTexture2D(const std::string& filename)
{
    auto tex = Texture2D();
    tex.LoadFromFile(filename);

    //m_textures.push_back(tex.Get());

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
   // m_device->CreateShaderResourceView(m_textures.at(0), &srvDesc, m_cpuHandle);
}