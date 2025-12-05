#include "DX12_Pipeline.h"


D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView(const DX12_DeviceResource* dr, UINT backBufferIndex)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		dr->Heap->GetCPUDescriptorHandleForHeapStart(),
		backBufferIndex,
		dr->DescriptorSize
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(const DX12_DeviceResource* dr)
{
	return dr->Heap->GetCPUDescriptorHandleForHeapStart();
}

void DX12_Pipeline::Init(HWND hwnd)
{
	SDL_Log("***Initializing DX12***");
#ifdef _DEBUG
	{
		SDL_Log("Getting debug interface...");
		ComPtr<ID3D12Debug> debug = nullptr;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
		debug->EnableDebugLayer();
	}
#endif

	SDL_Log("Creating DXGI Factory...");
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));


	SDL_Log("Creating DX12 Device...");
	HRESULT hr = D3D12CreateDevice(
		nullptr,	// default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device)
	);

	if (FAILED(hr))
	{
		SDL_Log("Device creation failed, fallback to warp adapter");
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		
		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(), 
			D3D_FEATURE_LEVEL_11_0, 
			IID_PPV_ARGS(&m_device)));
	}

	m_hwnd = hwnd;


	SDL_Log("Creating DX12 Fence...");
	m_fence = CreateFence();

	SDL_Log("Creating device resources...");
	m_rtv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrv = CreateDeviceResource(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	SDL_Log("Creating command objects...");
	m_cmdDirect = CreateCommandObjects(D3D12_COMMAND_LIST_TYPE_DIRECT);
	
	SDL_Log("Creating swap chain...");
	m_swapChain = CreateSwapChain(1920, 1080, 2);

	SDL_Log("***Finished DX12 Initialization***\n\n");

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

DX12_SwapChain DX12_Pipeline::CreateSwapChain(int width, int height, UINT backBufferCount)
{
	ComPtr<IDXGISwapChain1> swapChain;

	swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd = {};
	sd.Width = width;
	sd.Height = height;
	sd.Format = m_backBufferFormat;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SampleDesc = { 1, 0 };
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = backBufferCount;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdDirect.Queue.Get(),
		m_hwnd,
		&sd,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()));

	return { 0, backBufferCount, swapChain };
}

DX12_DeviceResource DX12_Pipeline::CreateDeviceResource(HEAPTYPE type, UINT numDescriptors)
{
	DX12_DeviceResource dr = {};
	dr.DescriptorSize = m_device->GetDescriptorHandleIncrementSize(type);
	dr.Type = type;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, 
		IID_PPV_ARGS(dr.Heap.GetAddressOf())));

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

