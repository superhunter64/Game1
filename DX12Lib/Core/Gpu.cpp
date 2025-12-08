#include "Gpu.h"

using namespace Util;

namespace GPU
{
	void DisplayAdapters()
    {
        {
            IDXGIFactory1* factory = nullptr;

            HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));

            UINT i = 0;
            IDXGIAdapter* adapter = nullptr;
            if (SUCCEEDED(hr))
            {
                Logf("%s", "Display Adapters:");
                while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
                {
                    DXGI_ADAPTER_DESC desc;
                    adapter->GetDesc(&desc);
                    Logf("--> %ws", desc.Description);
                    ++i;
                }
            }
        }
    }

	void Init()
	{
        Log("***Initializing DX12***");
#ifdef _DEBUG
        {
            Log("Enabling debug layer...");
            ComPtr<ID3D12Debug> debug = nullptr;
            ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
            debug->EnableDebugLayer();
        }
#endif

        // Determine maximum supported feature level for this device
        static const D3D_FEATURE_LEVEL pFeatureLevels[] =
        {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
        {
            _countof(pFeatureLevels), pFeatureLevels, D3D_FEATURE_LEVEL_11_0
        };

        D3D_FEATURE_LEVEL pFeatureLevel;
        HRESULT hr = gDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
        if (SUCCEEDED(hr))
        {
            pFeatureLevel = featLevels.MaxSupportedFeatureLevel;
        }
        else
        {
            pFeatureLevel = D3D_FEATURE_LEVEL_11_0;
        }

        hr = D3D12CreateDevice(
            nullptr,	// default adapter
            pFeatureLevel,
            IID_PPV_ARGS(&gDevice)
        );
	}
}