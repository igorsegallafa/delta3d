#pragma once
#include "../Math/Intersection.h"
#include "../Math/Vector3.h"

namespace Delta3D::Math
{
class BoundingBox;
class Sphere
{
public:
	//! Default Constructor for Sphere.
	Sphere() : radius( 0.0f ), center( Vector3::Null ) {}

	//! Construct a specified Sphere by radius and center vector.
	Sphere( float radius_, Vector3 center_ ) : radius( radius_ ), center( center_ ) {}

	//! Merge current Sphere with another Sphere.
	void Merge( Sphere& other );

	//! Merge current Sphere with a Bounding Box.
	void Merge( BoundingBox& box );

	//! Merge current Sphere with a Vector3.
	void Merge( Vector3 point );

	/**
	 * Return Sphere Transformed by Translation.
	 */
	Sphere Transformed( const Math::Vector3& translation ) const;

	/**
	 * Check if a Sphere is inside of this Sphere.
	 * @param vector Point.
	 * @return Intersection Type.
	 */
	Intersection IsInside( const Sphere& other ) const;

	/**
	 * Check if a Vector3 is inside of this Sphere.
	 * @param vector Point.
	 * @return Intersection Type.
	 */
	Intersection IsInside( const Vector3& vector ) const;

	float radius;	//!< Radius of Sphere
	Vector3 center;	//!< Sphere Center
};
}