#include "Display.h"

void Display::CreateSwapchain(ID3D12CommandQueue* cmdQueue, int w, int h)
{
	m_commandQueue = cmdQueue;
    m_width = w;
    m_height = h;

    m_rtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BACK_BUFFER_COUNT);

    if (DEPTH_STENCIL_FORMAT != DXGI_FORMAT_UNKNOWN)
    {
        m_dsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
    }

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = w;
    sd.Height = h;
    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.SampleDesc = { 1, 0 };
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = BACK_BUFFER_COUNT;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue,
        GPU::gWindow,
        &sd,
        nullptr,
        nullptr,
        swapChain.GetAddressOf()));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTarget[i].GetAddressOf())));

#ifdef _DEBUG
        wchar_t name[25] = {};
        swprintf_s(name, L"Render target: %u", i);
        m_renderTarget[i]->SetName(name);
#endif

        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = BACK_BUFFER_FORMAT;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
            m_rtvHeap.GetCpuHandle(),
            i,
            m_rtvHeap.Size()
        );

        GPU::gDevice->CreateRenderTargetView(
            m_renderTarget[i].GetResource(),
            &desc,
            rtvDescriptor
        );
    }
}

void Display::SetDepthStencil()
{
    CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        DEPTH_STENCIL_FORMAT,
        m_width,
        m_height,
        1,  // The depth stencil view only has 1 texture
        1   // use a single mipmap level
    );
    depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthOptimizeClearValue = {};
    depthOptimizeClearValue.Format = DEPTH_STENCIL_FORMAT;
    depthOptimizeClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizeClearValue.DepthStencil.Stencil = 0;

    ThrowIfFailed(GPU::gDevice->CreateCommittedResource(&depthHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizeClearValue,
        IID_PPV_ARGS(m_depthStencil.GetAddressOf())
    ));

    m_depthStencil->SetName(L"Depth stencil");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DEPTH_STENCIL_FORMAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
}

CD3DX12_RESOURCE_BARRIER Display::Render()
{
    return CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget[GetCurrentBackBufferIndex()].GetResource(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
}

CD3DX12_RESOURCE_BARRIER Display::Present()
{
    return CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget[GetCurrentBackBufferIndex()].GetResource(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}