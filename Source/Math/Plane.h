#pragma once

#include "Vector3.h"
#include "BoundingBox.h"
#include "Sphere.h"

namespace Delta3D::Math
{

enum class PlaneSide
{
	Intersect,
	Front,
	Back,
};

class Plane 
{
public:
	//! Default Constructor for Plane.
	Plane() : a( 1.0f ), b( 1.0f ), c( 1.0f ), d( 0.0f ) {}

	//! Construct a Plane from points.
	Plane( const float a_, const float b_, const float c_, const float d_ ) : a( a_ ), b( b_ ), c( c_ ), d( d_ ) {}

	//! Construct a Plane from your Normal and Distance.
	Plane( const Vector3& normal, const float d_ ) : a( normal.x ), b( normal.y ), c( normal.z ), d( d_ ) {}

	//! Construct a Plane from Triangles.
	Plane( const Vector3& v1, const Vector3& v2, const Vector3& v3 );

	/**
	 * Return Normal from Plane
	 */
	Vector3 Normal() const { return Vector3( a, b, c ); }

	/**
	 * Return Distance between Plane and some Point
	 */
	float Distance( const Vector3& point ) const{ return a * point.x + b * point.y + c * point.z + d; }

	/**
	 * Return the side where Point is colliding with Plane
	 */
	PlaneSide Side( const Vector3& point ) const;

	/**
	 * Return the side where Bounding Box is colliding with Plane
	 */
	PlaneSide Side( const BoundingBox& box ) const;

	/**
	 * Return the side where Sphere is colliding with Plane
	 */
	PlaneSide Side( const Sphere& sphere ) const;
public:
	float a;	//!< Vertex A
	float b;	//!< Vertex B
	float c;	//!< Vertex C
	float d;	//!< Vertex D
};

}