#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "win_com.h"

namespace DX
{
	struct CameraBuffer
	{
		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT4X4 ViewMatrix;
		DirectX::XMFLOAT4X4 ProjectionMatrix;
		DirectX::XMFLOAT4 _padding[4];			// required to keep the constant buffer 256-byte aligned
	};

	class FrameResource
	{

	private:

		CameraBuffer* m_cameraBuffer;

		ComPtr<ID3D12Resource> m_vertexBuffer;
		ComPtr<ID3D12Resource> m_indexBuffer;
		ComPtr<ID3D12Resource> m_perFrameConstants;

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
		D3D12_GPU_VIRTUAL_ADDRESS m_constantDataGpuAddr;

		// not sure if i'll need these
		ComPtr<ID3D12CommandList>		m_bundle;
		ComPtr<ID3D12CommandAllocator>	m_bundleAllocator;
	};
}