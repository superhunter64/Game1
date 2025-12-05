#include "DeviceResources.h"

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

void DeviceResources::SetWindow(HWND hwnd)
{
	m_window = hwnd;
}


