#include "PipelineStateObject.h"

void DescriptorTable::AddRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors, UINT baseShaderRegister, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags)
{
	DescriptorRange r = {};
	r.Type = type;
	r.NumDescriptors = numDescriptors;
	r.BaseShaderRegister = baseShaderRegister;
	r.RegisterSpace = registerSpace;
	r.Flags = flags;

	m_ranges.push_back(r);
}

void DescriptorTable::AddParam(D3D12_SHADER_VISIBILITY shaderVisibility)
{
	RootParameter rp = {};
	rp.InitFlag = ParamInitFlag::DESCRIPTOR_TABLE;
	rp.ShaderVisibility = shaderVisibility;

	m_rootParams.push_back(rp);
}

CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC DescriptorTable::InitDesc(D3D12_STATIC_SAMPLER_DESC* sampler)
{
	for (int i = 0; i < m_ranges.size(); i++)
	{
		auto& r = m_ranges.at(i);
		auto& p = m_rootParams.at(i);

		r.Init(r.Type, r.NumDescriptors, r.BaseShaderRegister, r.RegisterSpace, r.Flags);
		p.InitAsDescriptorTable(1, &m_ranges.at(i), p.ShaderVisibility);
	}

	UINT numStaticSamplers = 0;
	if (sampler)
	{
		numStaticSamplers = 1;
	}

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_1(m_rootParams.size(), &m_rootParams.at(0),
		numStaticSamplers, sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	return rootSignatureDesc;
}

void RootSignature::Create(DescriptorTable* table)
{
	auto featureData = GPU::GetFeatureData();
}

void GraphicsPSO::SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& input)
{
	for (auto& desc : input)
	{
		m_inputLayout.push_back(desc);
	}
}

void GraphicsPSO::SetVertexShader(D3D12_SHADER_BYTECODE byteCode)
{
	m_desc.VS = byteCode;
}

void GraphicsPSO::SetPixelShader(D3D12_SHADER_BYTECODE byteCode)
{
	m_desc.PS = byteCode;
}