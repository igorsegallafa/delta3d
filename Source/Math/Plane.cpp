#include "PrecompiledHeader.h"
#include "Plane.h"

namespace Delta3D::Math
{

Plane::Plane( const Vector3& v1, const Vector3& v2, const Vector3& v3 )
{
	auto normal = (( v2 - v1 ).CrossProduct( v3 - v1 ).Normalize())* -1.0f;

	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -normal.DotProduct( v1 );
}

PlaneSide Plane::Side( const Vector3& point) const 
{
	const float d = Distance( point );

	if( ( d > -FLT_EPSILON ) && ( d < FLT_EPSILON ) )
		return PlaneSide::Intersect;

	return d > 0.0f ? PlaneSide::Front : PlaneSide::Back;
}

PlaneSide Plane::Side( const BoundingBox& box ) const
{
	Vector3 corners[8];
	box.ComputeCorners( corners );

	const float d1 = Distance( corners[0] );

	if( !std::isnormal( d1 ) )
		return PlaneSide::Intersect;

	if( fabs( d1 ) < FLT_EPSILON )
		return PlaneSide::Intersect;

	const bool positiveSide = d1 > 0.0f;

	for (int i = 1; i < 8; i++) 
	{
		const float dx = Distance(corners[i]);

		if (fabs(dx) < FLT_EPSILON )
			return PlaneSide::Intersect;

		if( !std::isnormal( dx ) )
			return PlaneSide::Intersect;

		if (positiveSide) 
		{
			if (dx < 0.0f)
				return PlaneSide::Intersect;
		} 
		else 
		{
			if (dx > 0.0f)
				return PlaneSide::Intersect;
		}
	}

	return positiveSide ? PlaneSide::Front : PlaneSide::Back;
}

PlaneSide Plane::Side( const Sphere& sphere ) const
{
	const float d = Distance( sphere.center );

	if( fabs( d ) <= sphere.radius )
		return PlaneSide::Intersect;

	return ( d > 0.0f ? PlaneSide::Front : PlaneSide::Back );
}

}