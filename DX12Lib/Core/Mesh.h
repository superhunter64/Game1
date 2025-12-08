#pragma once
#include <vector>
#include <DirectXMath.h>
#include <wtypes.h>

using namespace DirectX;

struct Vertex 
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoords;
};

struct Texture
{

};

class Mesh
{
public:

private:
	
	std::vector<Vertex> mVertices;
	std::vector<UINT> mIndices;
	std::vector<Texture> textures;
};