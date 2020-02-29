#pragma once

#undef min
#undef max

#include "../Math/Intersection.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Rect.h"

namespace Delta3D::Math
{
class BoundingBox
{
public:
	//! Default Constructor for Bounding Box.
	BoundingBox() : min( INFINITY, INFINITY, INFINITY ), max( -INFINITY, -INFINITY, -INFINITY ), dummyMin( 0.f ), dummyMax( 0.f ) {}

	//! Construct a specified Bounding Box from Vector3.
	BoundingBox( Vector3& vector ) : min( vector ), max( vector ), dummyMin( 0.f ), dummyMax( 0.f ) {}

	//! Construct a specified Bounding Box from Min and Max Vector3.
	BoundingBox( Vector3& min_, Vector3& max_ ) : min( min_ ), max( max_ ), dummyMin( 0.f ), dummyMax( 0.f ) {}

	//! Construct a specified Bounding Box with SSE.
	BoundingBox( __m128 min_, __m128 max_ ) noexcept : dummyMin( 0.f ), dummyMax( 0.f )
	{
		_mm_storeu_ps( &min.x, min_ );
		_mm_storeu_ps( &max.x, max_ );
	}

	//! Reset Bounding Box.
	void Reset() { min.x = INFINITY; min.y = INFINITY; min.z = INFINITY; max.x = -INFINITY; max.y = -INFINITY; max.z = -INFINITY; }

	//! Merge current Bounding Box with another Vector3.
	void Merge( Vector3& other );

	//! Merge current Bounding Box with another Bounding Box.
	void Merge( BoundingBox& other );

	//! Apply Transformation from Matrix to this Bounding Box.
	void Transform( const Math::Matrix4& transform );

	/**
	 * Return Bounding Box Transformed by Matrix4x4.
	 */
	BoundingBox Transformed( const Math::Matrix4& transform ) const;

	/**
	 * Return Bounding Box projected by Camera Projection Matrix.
	 */
	Rect Projected( const Matrix4& projection ) const;

	//! Compute Bounding Box Corners.
	Math::Vector3* ComputeCorners( Math::Vector3* array ) const;

	/**
	 * Check if a Vector3 is inside of this Bounding Box.
	 * @param vector Point.
	 * @return Intersection Type.
	 */
	Intersection IsInside( const Vector3& vector ) const;

	/**
	 * Check if a Bounding Box is inside of this Bounding Box.
	 * @param other Bounding Box.
	 * @return Intersection Type.
	 */
	Intersection IsInside( const BoundingBox& other ) const;

	/**
	 * Check if a Bounding Box is inside (partially) of this Bounding Box.
	 * @param other Bounding Box.
	 * @return Intersection Type.
	 */
	Intersection IsInsideFast( const BoundingBox& other ) const;

	/**
	 * Return Bounding Box Center.
	 */
	Vector3 Center() const { return ( max + min ) * 0.5f; }

	/**
	 * Return Bounding Box Size.
	 */
	Vector3 Size() const { return max - min; }

	/**
	 * Return a formatted string for this vector.
	 */
	std::string ToString() const;

	Vector3 min;	//!< Min Vector
	float dummyMin;

	Vector3 max;	//!< Max Vector
	float dummyMax;
};
}