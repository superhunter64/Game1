#pragma once
#include "GpuResource.h"

class GpuBuffer : public GpuResource
{
public:
	void Create();

protected:

	GpuBuffer(const std::wstring& name, uint32_t numElements, uint32_t elementSize) {};

};