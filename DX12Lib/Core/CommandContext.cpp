#include "CommandContext.h"

void CommandContext::check_device()
{
	if (not mDevice)
	{
		Util::Log("Initialize the command context with a valid device before continuing.");
		throw std::exception();
	}
}

void CommandContext::Init(ID3D12Device* device)
{
	mDevice = device;
}

void CommandContext::CreateDirect()
{
	check_device();

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue));
	}


	for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
	{
		ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mCommandAllocators[i])));
	}

	ThrowIfFailed(mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		nullptr,
		nullptr,
		IID_PPV_ARGS(&mCommandList)
	));
	mCommandList->Close();

	{

	}
}