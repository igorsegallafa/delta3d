#pragma once

#include "Plane.h"
#include "Vector3.h"
#include "BoundingBox.h"
#include "Intersection.h"

namespace Delta3D::Math
{

enum class FrustumPlane
{
	Near,
	Left,
	Right,
	UP,
	Down,
	Far,
};	DEFINE_ENUM_FLAG_OPERATORS( FrustumPlane );

class Frustum 
{
public:
	//! Default Constructor for Frustum.
	Frustum() : nearDistance( 0.f ), farDistance( 0.f ), nearWidth( 0.f ), nearHeight( 0.f ), farWidth( 0.f ), farHeight( 0.f ), p{}, vertices{}{}

	//! Define a Frustum Culling from Perspective Settings.
	void Define( float fov, float zoom, float aspectRatio, float nearClip, float farClip );

	//! Transform Frustum Culling by Camera Position.
	void Transform( const Vector3& eye, const Vector3& lookAt, const Vector3& up );

	//! Update Frustum Planes.
	void UpdatePlanes();

	/**
	 * Get the intersection between Frustum and some Object
	 * @param object Object
	 * @return Intersection Type
	 */
	template <class T>
	Intersection IsInside( const T& object ) const
	{
		for( int i = 0; i < 6; i++ )
			if( p[i].Side( object ) == PlaneSide::Back )
				return Intersection::Outside;

		return Intersection::Inside;
	}

	float nearDistance;	//!< Near Distance
	float farDistance;	//!< Far Distance
	float nearWidth;	//!< Near Width
	float nearHeight;	//!< Near Height
	float farWidth;	//!< Far Width
	float farHeight;	//!< Far Height

	Plane p[6];	//!< Frustum Planes
	Vector3 vertices[8];	//!< Frustum Vertices
};
}