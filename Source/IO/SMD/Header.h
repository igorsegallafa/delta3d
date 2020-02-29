#pragma once

#include "Frame.h"

namespace Delta3D::IO::SMD
{
struct Header
{
	char header[24];

	int	objectCount;
	int	materialCount;

	int filePointerToMaterials;
	int filePointerToFirstObject;

	int frameCount;
	Frame frames[32];	//!< Multiple frame file connection information (Character Motions Files)(MX.smb)
};
}