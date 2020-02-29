#pragma once

#include "Vector3.h"

namespace Delta3D::Math
{
class Quad
{
public:
	Quad() {}
	Quad( const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4 ) : p1( p1 ), p2( p2 ), p3( p3 ), p4( p4 ) {}
	Quad( const Vector3 p[4] ) : p1( p[0] ), p2( p[1] ), p3( p[2] ), p4( p[3] ) {}

	Vector3 p1;
	Vector3 p2;
	Vector3 p3;
	Vector3 p4;
};
}