#pragma once
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdexcept>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

void ThrowIfFailed(HRESULT hr);

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
    {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

D3D12_SHADER_BYTECODE CompileShader(const std::wstring& filePath, const std::string& entryPoint, const std::string& target);