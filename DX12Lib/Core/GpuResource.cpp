#include "GpuResource.h"

void Texture2D::Create(unsigned char* data, int width, int height)
{
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(GPU::gDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_resource)
    ));

	// data might have to be memcpy'd here because if the original
	// pointer goes out of scope before uploading, this might be undefined
	NAME_D3D12_OBJECT(m_resource);

	m_textureData = {};
	m_textureData.pData = &data[0];
	m_textureData.RowPitch = width * 4;
	m_textureData.SlicePitch = m_textureData.RowPitch * height;
}

void DirectUploadBuffer::UploadTextures(ID3D12GraphicsCommandList* cmdList, DescriptorHeap* srvHeap, std::vector<Texture2D>& textures)
{
	m_uploads.resize(textures.size());

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(srvHeap->GetCpuHandle());

    for (UINT i = 0; i < textures.size(); i++)
    {
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(textures[i].GetResource(), 0, 1);

		auto uploadProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		ThrowIfFailed(GPU::gDevice->CreateCommittedResource(
			&uploadProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadBuffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploads[i])
		));

		UpdateSubresources(cmdList, textures[i].GetResource(), m_uploads[i].Get(),
			0, 0, 1, &textures[i].m_textureData);
		
		auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(textures[i].GetResource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		
		cmdList->ResourceBarrier(1, &transitionBarrier);

		srvHeap->BuildSrv(textures[i].GetResource());

		cpuHandle.Offset(srvHeap->Size());
    }
}

void DirectUploadBuffer::FillBuffer(std::vector<ComPtr<ID3D12Resource>>& resources)
{
	for (int i = 0; i < resources.size(); i++)
	{
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(resources[i].Get(), 0, 1);

		auto uploadProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		ThrowIfFailed(GPU::gDevice->CreateCommittedResource(
			&uploadProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadBuffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploads[i])
		));
	}
}