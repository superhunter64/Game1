#pragma once

#include "Gpu.h"
#include "DXHelper.h"
#include "Util.h"

class CommandContext
{

public:
	CommandContext() {};
	~CommandContext() {};

	void Init(ID3D12Device* device);

	void CreateDirect();

protected:

	ComPtr<ID3D12Device> mDevice = nullptr;

	ComPtr<ID3D12GraphicsCommandList>	mCommandList;
	ComPtr<ID3D12CommandAllocator>		mCommandAllocators[BACK_BUFFER_COUNT];
	ComPtr<ID3D12CommandQueue>			mCommandQueue;

	void check_device();
};