#pragma once

#include "TextureLink.h"

namespace Delta3D::IO::SMD
{
struct Face
{
	union
	{
		struct
		{
			unsigned short vertices[3];
			unsigned short materialID;
		};

		unsigned short v[4];
	};
	
	float  padding[3][2];
	TextureLink* textureLink;
};
}