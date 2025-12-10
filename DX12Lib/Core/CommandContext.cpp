#include "CommandContext.h"

void CommandContext::check_device()
{
	if (not m_device)
	{
		Util::Log("Initialize the command context with a valid device before continuing.");
		throw std::exception();
	}
}

void CommandContext::Init()
{
	m_device = GPU::gDevice;
}

void CommandContext::Init(ID3D12Device* device)
{
	m_device = device;
}

void CommandContext::init_allocators()
{
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
	}

	for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
	{
		ThrowIfFailed(m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_commandAllocators[i])));
	}
}

void CommandContext::Record(ID3D12PipelineState* pipelineState, UINT backBufferIndex)
{

}

void CommandContext::Execute()
{

}

void CommandContext::CreateDirect()
{
	check_device();

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
	}

	for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
	{
		ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_commandAllocators[i])));
	}

	ThrowIfFailed(m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		nullptr,
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	));

	m_commandList->Close();
}