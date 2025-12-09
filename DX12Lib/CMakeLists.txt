cmake_minimum_required (VERSION 3.16)
project(DX12Lib)

set(HEADER_FILES
	Core/GpuResource.h
	Core/DXHelper.h
	"Core/Gpu.h" 
	"Core/Util.h"
	"Core/CommandContext.h"
	"Core/GpuBuffer.h"
	"Core/Mesh.h" 
	"Core/stb_image.h" 
	 
	"Core/Shader.h" "Core/StaticHeap.h" "Core/PipelineStateObject.h" "Core/Renderer.h")

set(SOURCE_FILES
	Core/DXHelper.cpp
	"Core/Gpu.cpp"
	"Core/CommandContext.cpp"
	"Core/GpuBuffer.cpp"
	 "Core/StaticHeap.cpp" "Core/PipelineStateObject.cpp" "Core/Renderer.cpp")
 
add_library(DX12Lib STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_compile_features(DX12Lib PUBLIC cxx_std_20)

target_include_directories(DX12Lib PRIVATE "../vendor/DirectX-Headers-1.618.2/include/directx")

target_link_libraries(DX12Lib PRIVATE 
	dxguid
	d3d12
	dxgi
	d3dcompiler
	DirectX-Headers
)