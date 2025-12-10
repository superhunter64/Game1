#pragma once

#include <DirectXMath.h>

typedef DirectX::XMFLOAT2 XMFLOAT2;
typedef DirectX::XMFLOAT3 XMFLOAT3;
typedef DirectX::XMFLOAT4 XMFLOAT4;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

struct FlatVertex
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};