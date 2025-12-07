#include "DeviceResources.h"
#include "../App.h"
#include "File.h"
#include <cstdio>

using namespace DX;

void DeviceResources::InitDevice()
{
    {
        IDXGIFactory1* factory = nullptr;

        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));

        UINT i = 0;
        IDXGIAdapter* adapter = nullptr;
        if (SUCCEEDED(hr))
        {
            while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_ADAPTER_DESC desc;
                adapter->GetDesc(&desc);
                SDL_Log("Adapter: %ws", desc.Description);
                ++i;
            }
        }
        SDL_Log("******\n\n");
    }

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
}

void DeviceResources::SetWindow()
{
	m_window = App::hwnd;
}

void DeviceResources::CreateDeviceResources()
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

    // Determine maximum supported feature level for this device
    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
    {
        _countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    hr = m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
    {
        m_featureLevel = featLevels.MaxSupportedFeatureLevel;
    }
    else
    {
        m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    }

    // Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        
        m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
    }

    // Create descriptor heaps for rtv and dsv
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
        rtvDesc.NumDescriptors = m_backBufferCount;
        rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap)));
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

            ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsvDescriptorHeap)));
        }
    }

    // Create a command allocator for each back buffer
    for (UINT i = 0; i < m_backBufferCount; i++)
    {
        ThrowIfFailed(m_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            IID_PPV_ARGS(&m_commandAllocators[i])));
    }

    // Create command list
    ThrowIfFailed(m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocators[0].Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList)
    ));

    ThrowIfFailed(m_commandList->Close());

    // Create a fence for tracking GPU execution progress
    ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValues[m_backBufferIndex]++;

    m_fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    if (!m_fenceEvent.IsValid()) ThrowIfFailed(E_FAIL);
}

void DeviceResources::CreateWindowDependentResources()
{
    if (not m_window)
        ThrowIfFailed(E_HANDLE, "Call SetWindow with a valid Win32 HWND window handle");

    // Implement later
    //WaitForGpu();

    // Release resources tied to the swap chain and update fence values
    for (UINT i = 0; i < m_backBufferCount; i++)
    {
        m_renderTargets[i].Reset();
        m_fenceValues[i] = m_fenceValues[m_backBufferIndex];
    }

    int w, h;
    App::GetWindowSize(&w, &h);

    // If the swap chain was already created, resize it, otherwise create a new one
    if (m_swapChain)
    {
        ThrowIfFailed(m_swapChain->ResizeBuffers(
            m_backBufferCount,
            w,
            h,
            m_backBufferFormat,
            0
        ));
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = w;
        sd.Height = h;
        sd.Format = m_backBufferFormat;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.SampleDesc = { 1, 0 };
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = m_backBufferCount;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ComPtr<IDXGISwapChain1> swapChain;

        ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            m_window,
            &sd,
            nullptr,
            nullptr,
            swapChain.GetAddressOf()), "failed to create swapchain");

        ThrowIfFailed(swapChain.As(&m_swapChain), "failed to convert swapchain1 to swapchain4 comptr");
    }

    // Obtain the back buffers for the window which will be the final render targets
    // and create render target views for each of them
    for (UINT i = 0; i < m_backBufferCount; i++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target: %u", i);
        m_renderTargets[i]->SetName(name);

        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = m_backBufferFormat;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
            m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            i,
            m_rtvDescriptorSize
        );

        m_device->CreateRenderTargetView(
            m_renderTargets[i].Get(),
            &desc,
            rtvDescriptor
       );
    }

    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // set up the depth stencil buffer
    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            m_depthBufferFormat,
            w,
            h,
            1,  // The depth stencil view only has 1 texture
            1   // use a single mipmap level
        );
        depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthOptimizeClearValue = {};
        depthOptimizeClearValue.Format = m_depthBufferFormat;
        depthOptimizeClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizeClearValue.DepthStencil.Stencil = 0;

        ThrowIfFailed(m_device->CreateCommittedResource(&depthHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizeClearValue,
            IID_PPV_ARGS(&m_depthStencil)
        ));

        m_depthStencil->SetName(L"Depth stencil");
        
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = m_depthBufferFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    }


}

void DeviceResources::LoadAssets()
{
    // Create an empty root signature
    {
        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state -- compile and load shaders
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile(FS::FullPathW(L"", L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(FS::FullPathW(L"", L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
    }
}
