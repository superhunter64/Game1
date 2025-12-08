cmake_minimum_required (VERSION 3.16)
project(DX12Lib)

set(HEADER_FILES
	Core/GpuResource.h
	Core/DXHelper.h
 "Core/Gpu.h")

set(SOURCE_FILES
	Core/GpuResource.cpp
	Core/DXHelper.cpp
 "Core/Gpu.cpp")
 
add_library(DX12Lib STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_compile_features(DX12Lib PUBLIC cxx_std_20)

target_link_libraries(DX12Lib PRIVATE 
	d3d12
	dxgi
)