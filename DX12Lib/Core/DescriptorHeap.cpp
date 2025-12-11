#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "DescriptorHeap.h"

/*
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
*/
void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = numDescriptors;
	desc.NodeMask = 1;
	desc.Flags = flags;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));

    m_heap->SetName(L"Static heap 1");

    m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(type);

    NAME_D3D12_OBJECT(m_heap);
}

void DescriptorHeap::BuildSrv(ID3D12Resource* resource, DXGI_FORMAT format)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(resource, &srvDesc, GetCpuHandle());
}