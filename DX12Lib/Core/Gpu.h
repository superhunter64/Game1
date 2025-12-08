#pragma once

#include "GpuResource.h"

#include <memory>a

namespace GPU
{
	inline std::unique_ptr<ID3D12Device> Device = nullptr;


	void Init();
}