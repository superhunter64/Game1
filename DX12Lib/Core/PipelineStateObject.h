#pragma once

#include "Gpu.h"

enum ParamInitFlag
{
	DESCRIPTOR_TABLE
};

struct DescriptorRange : CD3DX12_DESCRIPTOR_RANGE1
{
	D3D12_DESCRIPTOR_RANGE_TYPE Type;
	UINT NumDescriptors;
	UINT BaseShaderRegister;
	UINT RegisterSpace;
	D3D12_DESCRIPTOR_RANGE_FLAGS Flags;
};

struct RootParameter : CD3DX12_ROOT_PARAMETER1
{
	ParamInitFlag InitFlag;
	D3D12_SHADER_VISIBILITY ShaderVisibility;
};

class DescriptorTable
{
public:
	void AddRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors, UINT baseShaderRegister, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags);
	void AddParam(D3D12_SHADER_VISIBILITY shaderVisibility);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC InitDesc(D3D12_STATIC_SAMPLER_DESC* sampler = nullptr);

private:
	std::vector<DescriptorRange>	m_ranges = {};
	std::vector<RootParameter>		m_rootParams = {};
};

class RootSignature
{
public:

	RootSignature() {};

	ID3D12RootSignature* Get() { return m_rootSignature.Get(); }
	const ID3D12RootSignature* Get() const { return m_rootSignature.Get(); }

	void Create(DescriptorTable* table);

private:

	DescriptorTable m_descriptorTable;
	ComPtr<ID3D12RootSignature> m_rootSignature;
};

class PipelineStateObject
{
public:
	PipelineStateObject() {};

	void SetRootSignature(const RootSignature& rootSignature) 
		{ m_rootSignature = &rootSignature; }
	
protected:

	const RootSignature* m_rootSignature;

	DescriptorTable m_descriptorTable = {};
};

typedef PipelineStateObject PSO;

class GraphicsPipelineStateObject : public PSO
{
public:
	GraphicsPipelineStateObject() {};

	void SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescs);
	void SetVertexShader(D3D12_SHADER_BYTECODE byteCode);
	void SetPixelShader(D3D12_SHADER_BYTECODE byteCode);

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetDesc() { return &m_desc; }

private:

	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc = {};
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout = {};
};

typedef GraphicsPipelineStateObject GraphicsPSO;