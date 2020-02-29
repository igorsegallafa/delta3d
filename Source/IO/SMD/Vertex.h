#pragma once

namespace Delta3D::IO::SMD
{
struct Vertex
{
	int x;
	int y;
	int z;

	int nx;
	int ny;
	int nz;
};

struct VertexColor
{
	float r;
	float g;
	float b;
};

struct PackedVertex
{
	Math::Vector3 position;
	Math::Vector2 uv[8];
	int boneIndex;
	D3DCOLOR color;

	bool operator<( const PackedVertex other ) const
	{
		return memcmp( (void*)this, (void*)&other, sizeof( PackedVertex ) ) > 0;
	};
};

}