#pragma once

namespace Delta3D::IO::SMD
{
struct TextureLink
{
	float u[3];
	float v[3];

	void* texHandle;

	TextureLink* next;
};
}