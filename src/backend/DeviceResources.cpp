#include "DeviceResources.h"
#include "../App.h"
#include "../shared/Paths.h"
#include "File.h"
#include <cstdio> 
#include "primitives/Vertex.h"

using namespace DX;

void DeviceResources::DisplayAdapters()
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
}

void DeviceResources::SetWindow()
{
	m_window = App::hwnd;
}

void DeviceResources::CreateDeviceResources(UINT backBufferCount)
{
    m_backBufferCount = backBufferCount;

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

    SDL_Log("Creating command queue...");
    // Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        
        m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
    }

    SDL_Log("Creating render target view and depth stencil view...");
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

    SDL_Log("Creating command objects...");
    // Create a command allocator for each back buffer
    for (UINT i = 0; i < m_backBufferCount; i++)
    {
        ThrowIfFailed(m_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            IID_PPV_ARGS(&m_commandAllocators[i])));
    }

    // Create bundle alloactor
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_bundleAllocator)));

    // Create command list
    // This can be replaced with Device4::CreateCommandList1
    // which initializes the command list as closed since we're not
    // recording any commands after creation
    ThrowIfFailed(m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocators[m_backBufferIndex].Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList)
    )); 
    m_commandList->Close(); 
    
    SDL_Log("Creating fence...");
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


    // Release resources tied to the swap chain and update fence values
    for (UINT i = 0; i < m_backBufferCount; i++)
    {
        m_renderTargets[i].Reset();
        m_fenceValues[i] = m_fenceValues[m_backBufferIndex];
    }

    int w, h;
    App::GetWindowSize(&w, &h);

    m_width = w;
    m_height = h;

    SDL_Log("Creating or resizing swapchain...");
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
        ComPtr<IDXGIFactory4> factory;
        ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = w;
        sd.Height = h;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.SampleDesc = { 1, 0 };
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = m_backBufferCount;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            m_window,
            &sd,
            nullptr,
            nullptr,
            swapChain.GetAddressOf()), "failed to create swapchain");

        ThrowIfFailed(swapChain.As(&m_swapChain), "failed to convert swapchain1 to swapchain4 comptr");
    }

    SDL_Log("Establishing back buffers...");
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

    WaitForGPU();
}

static std::wstring GetShaderPath(LPCWSTR fileName)
{
    return FS::FullPathW(Path::HLSLShaderPath.c_str(), fileName);
}

void DeviceResources::LoadAssets()
{
    SDL_Log("Loading assets...");
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

        ThrowIfFailed(D3DCompileFromFile(GetShaderPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(GetShaderPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }
    
    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
}

template<typename T>
void DeviceResources::LoadVertexBuffer(const std::vector<T>* vertices)
{
    const UINT64 bufferWidth = vertices->size() * sizeof(T);

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferWidth);

    // Create the resource (upload heap in this case)
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    ));

    // Copy the triangle data to the vertex buffer
    {
        UINT8* pVertexData;
        CD3DX12_RANGE readRange(0, 0); // for now, we don't intend to read this resource on the cpu
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexData)));
        memcpy(pVertexData, vertices->data(), static_cast<size_t>(bufferWidth));
        m_vertexBuffer->Unmap(0, nullptr);
    }

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(T);
    m_vertexBufferView.SizeInBytes = bufferWidth;

    // Create the bundle
    {
        ThrowIfFailed(m_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_BUNDLE,
            m_bundleAllocator.Get(),
            m_pipelineState.Get(),
            IID_PPV_ARGS(&m_bundle)
        ));

        m_bundle->SetGraphicsRootSignature(m_rootSignature.Get());
        m_bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_bundle->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        m_bundle->DrawInstanced(3, 1, 0, 0);
        ThrowIfFailed(m_bundle->Close());
    } // (?) the bundle seems to execute a command list without having to record the commands every frame
    
    WaitForGPU();
}

#pragma region Vertex Templates
template void DX::DeviceResources::LoadVertexBuffer<Vertex>(const std::vector<Vertex>*);
#pragma endregion

void DeviceResources::Render()
{
    // record all the command we need to render the scene into the command list
    PopulateCommandList();

    // execute the command list
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // present the frame
    ThrowIfFailed(m_swapChain->Present(1, 0));

    PrepareNextFrame();
}

void DeviceResources::PopulateCommandList()
{
    ThrowIfFailed(m_commandAllocators[m_backBufferIndex]->Reset());

    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_backBufferIndex].Get(), m_pipelineState.Get()));

    // set necessary state
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // indicate that the backbuffer will be used as a render target
    auto renderBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &renderBarrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_backBufferIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // record commands
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    m_commandList->ExecuteBundle(m_bundle.Get());

    // indicate that the backbuffer will now be used to present
    auto presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &presentBarrier);

    ThrowIfFailed(m_commandList->Close());
}

// Wait for pending GPU work to complete
void DeviceResources::WaitForGPU()
{
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_backBufferIndex]));

    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
    WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

    m_fenceValues[m_backBufferIndex]++;
}

// Prepare to render the next frame
void DeviceResources::PrepareNextFrame()
{
    // schedule signal command in the queue
    const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
        WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
    }

    m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
}
