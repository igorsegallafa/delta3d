#pragma once

namespace Delta3D::IO::SMD
{
struct ObjectInfo
{
	char objectName[32];
	int length;
	int filePointerToObject;
};
}