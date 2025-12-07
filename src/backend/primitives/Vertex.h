#pragma once
#include <DirectXMath.h>

typedef DirectX::XMFLOAT3 XMFLOAT3;
typedef DirectX::XMFLOAT4 XMFLOAT4;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};