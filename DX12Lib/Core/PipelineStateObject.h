#pragma once

#include "Gpu.h"

enum ParamInitFlag
{
	DESCRIPTOR_TABLE
};

struct DescriptorRange : public CD3DX12_DESCRIPTOR_RANGE1
{
	D3D12_DESCRIPTOR_RANGE_TYPE Type;
	UINT NumDescriptors;
	UINT BaseShaderRegister;
	UINT RegisterSpace;
	D3D12_DESCRIPTOR_RANGE_FLAGS Flags;
};

struct RootParameter : public CD3DX12_ROOT_PARAMETER1
{
	ParamInitFlag InitFlag;
	D3D12_SHADER_VISIBILITY ShaderVisibility;
};

struct DescriptorTable
{
	inline void AddRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, 
		UINT NumDescriptors, UINT BaseShaderRegister,
		UINT RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAGS Flags)
	{
		DescriptorRange r = {};
		r.Type = Type;
		r.NumDescriptors = NumDescriptors;
		r.BaseShaderRegister = BaseShaderRegister;
		r.RegisterSpace = RegisterSpace;
		r.Flags = Flags;
	
		Ranges.push_back(r);
	}

	inline void AddParam(D3D12_SHADER_VISIBILITY shaderVisibility)
	{
		RootParameter rp = {};
		rp.InitFlag = ParamInitFlag::DESCRIPTOR_TABLE;
		rp.ShaderVisibility = shaderVisibility;

		RootParams.push_back(rp);
	}

	inline CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC InitDesc(D3D12_STATIC_SAMPLER_DESC* sampler = nullptr)
	{
		for (int i = 0; i < Ranges.size(); i++)
		{
			auto& r = Ranges.at(i);
			auto& p = RootParams.at(i);

			r.Init(r.Type, r.NumDescriptors, r.BaseShaderRegister, r.RegisterSpace, r.Flags);
			p.InitAsDescriptorTable(1, &Ranges.at(i), p.ShaderVisibility);
		}

		UINT numStaticSamplers = 0;
		if (sampler) {
			numStaticSamplers = 1;
		}

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init_1_1(RootParams.size(), &RootParams.at(0), 
			numStaticSamplers, sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
		return rootSignatureDesc;
	}

private:
	std::vector<DescriptorRange>	Ranges = {};
	std::vector<RootParameter>		RootParams = {};
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
	PipelineStateObject(ID3D12Device* d) : m_device(d) {};

	void Init();

protected:

	ID3D12Device* m_device;

	DescriptorTable m_descriptorTable = {};
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
};

typedef PipelineStateObject PSO;