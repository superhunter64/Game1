#include "DXHelper.h"


void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

D3D12_SHADER_BYTECODE CompileShader(const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
{
	ComPtr<ID3DBlob> shader;

#ifdef _DEBUG
	static const UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	static const UINT compileFlags = 0;
#endif

	ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr, 
		entryPoint.c_str(), target.c_str(), compileFlags, 0, &shader, nullptr));

	return CD3DX12_SHADER_BYTECODE(shader.Get());
}
