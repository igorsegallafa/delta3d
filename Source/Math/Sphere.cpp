#include "PrecompiledHeader.h"
#include "Sphere.h"

#include "BoundingBox.h"

namespace Delta3D::Math
{

void Sphere::Merge( Sphere& other )
{
	if( radius < 0.0f )
	{
		center = other.center;
		radius = other.radius;
		return;
	}

	Vector3 offset = other.center - center;
	float dist = offset.Length();

	//If sphere fits inside, do nothing
	if( dist + other.radius < radius )
		return;

	//If we fit inside the other sphere, become it
	if( dist + radius < other.radius )
	{
		center = other.center;
		radius = other.radius;
	}
	else
	{
		Vector3 NormalizedOffset = offset / dist;

		Vector3 min = center - (NormalizedOffset* radius);
		Vector3 max = other.center + (NormalizedOffset* other.radius);
		center = ( min + max )* 0.5f;
		radius = ( max - center ).Length();
	}
}

void Sphere::Merge( BoundingBox& box )
{
	const Vector3& min = box.min;
	const Vector3& max = box.max;

	radius = 0.0f;
	center = Vector3::Null;

	Merge( Vector3( max.x, min.y, min.z ) );
	Merge( Vector3( min.x, max.y, min.z ) );
	Merge( Vector3( max.x, max.y, min.z ) );
	Merge( Vector3( min.x, min.y, max.z ) );
	Merge( Vector3( max.x, min.y, max.z ) );
	Merge( Vector3( min.x, max.y, max.z ) );
	Merge( max );
}

void Sphere::Merge( Vector3 point )
{
	if( radius < 0.0f )
	{
		center = point;
		radius = 0.0f;
		return;
	}

	Vector3 offset = point - center;
	float dist = offset.Length();

	if( dist > radius )
	{
		float half = ( dist - radius )* 0.5f;
		radius += half;
		center = center + ( offset* ( half / dist ) );
	}
}

Sphere Sphere::Transformed( const Math::Vector3& translation ) const
{
	return Sphere( radius, center + translation );
}

Intersection Sphere::IsInside( const Sphere& other ) const
{
	float dist = (other.center - center).Length();

	if( dist >= other.radius + radius )
		return Intersection::Outside;
	else if( dist + other.radius < radius )
		return Intersection::Inside;
	else
		return Intersection::Intersects;
}

Intersection Sphere::IsInside( const Vector3& vector ) const
{
	float distSquared = ( vector - center ).LengthSquared();

	if( distSquared < radius* radius )
		return Intersection::Inside;
	else
		return Intersection::Outside;
}

}