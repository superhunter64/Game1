#include "DX12_Pipeline.h"

void DX12_Pipeline::Init()
{
#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> debug = nullptr;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
		debug->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

	HRESULT hr = D3D12CreateDevice(
		nullptr,	// default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device)
	);

	if (FAILED(hr))
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		
		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(), 
			D3D_FEATURE_LEVEL_11_0, 
			IID_PPV_ARGS(&m_device)));
	}

	m_fence = CreateFence();
	m_rtv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_cmdDirect = CreateCommandObjects(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// check 4x MSAA Quality Support
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQual = {};
	msQual.Format = m_backBufferFormat;
}

ComPtr<ID3D12Fence1> DX12_Pipeline::CreateFence()
{
	ComPtr<ID3D12Fence1> fence = nullptr;
	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	return fence;
}

ComPtr<IDXGISwapChain1> DX12_Pipeline::CreateSwapChain(int width, int height)
{
	ComPtr<IDXGISwapChain1> swapChain = nullptr;

	swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;


	return swapChain;
}

DX12_DeviceResource DX12_Pipeline::CreateDeviceResource(HEAPTYPE type)
{
	DX12_DeviceResource dr = {};
	dr.DescriptorSize = m_device->GetDescriptorHandleIncrementSize(type);
	return dr;
}

DX12_Command DX12_Pipeline::CreateCommandObjects(D3D12_COMMAND_LIST_TYPE listType, ID3D12PipelineState* pipelineState)
{
	DX12_Command cmd = {};

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = listType;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmd.Queue)));

	ThrowIfFailed(m_device->CreateCommandAllocator(
		listType,
		IID_PPV_ARGS(cmd.Allocator.GetAddressOf())
	));

	ThrowIfFailed(m_device->CreateCommandList(
		0,
		listType,
		cmd.Allocator.Get(),
		pipelineState,
		IID_PPV_ARGS(cmd.List.GetAddressOf())
	));

	cmd.List->Close();

	return cmd;
}

