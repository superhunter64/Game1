#pragma once

#include "Gpu.h"
#include "DXHelper.h"
#include "Util.h"

class CommandContext
{
public:

	CommandContext() {};
	~CommandContext() {};

	void Init();
	void Init(ID3D12Device* device);

	void CreateDirect();
	
	void Record(ID3D12PipelineState* pipelineState, UINT backBufferIndex);
	void Execute();

protected:

	ID3D12Device* m_device = nullptr;

	ComPtr<ID3D12GraphicsCommandList>	m_commandList;
	ComPtr<ID3D12CommandAllocator>		m_commandAllocators[BACK_BUFFER_COUNT];
	ComPtr<ID3D12CommandQueue>			m_commandQueue;

	void check_device();
	void init_allocators();
};