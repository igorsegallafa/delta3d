#include "PrecompiledHeader.h"
#include "BoundingBox.h"

namespace Delta3D::Math
{

void BoundingBox::Merge( Vector3& other )
{
	BoundingBox s( other );
	Merge( s );
}

void BoundingBox::Merge( BoundingBox& other )
{
	_mm_storeu_ps( &min.x, _mm_min_ps( _mm_loadu_ps( &min.x ), _mm_loadu_ps( &other.min.x ) ) );
	_mm_storeu_ps( &max.x, _mm_max_ps( _mm_loadu_ps( &max.x ), _mm_loadu_ps( &other.max.x ) ) );
}

void BoundingBox::Transform( const Math::Matrix4& transform )
{
	*this = Transformed( transform );
}

BoundingBox BoundingBox::Transformed( const Math::Matrix4& transform ) const
{
	Math::Vector3 newCenter = transform* Center();
	Math::Vector3 oldEdge = Size()* 0.5f;

	Math::Vector3 newEdge = Vector3(
		abs( transform._11 )* oldEdge.x + abs( transform._12 )* oldEdge.y + abs( transform._13 )* oldEdge.z,
		abs( transform._21 )* oldEdge.x + abs( transform._22 )* oldEdge.y + abs( transform._23 )* oldEdge.z,
		abs( transform._31 )* oldEdge.x + abs( transform._32 )* oldEdge.y + abs( transform._33 )* oldEdge.z
	);

	Math::Vector3 min = newCenter - newEdge;
	Math::Vector3 max = newCenter + newEdge;

	return BoundingBox( min, max );
}

Rect BoundingBox::Projected( const Matrix4& projection ) const
{
	Vector3 projMin = min;
	Vector3 projMax = max;

	if( projMin.z < 0.01f )
		projMin.z = 0.01f;

	if( projMax.z < 0.01f )
		projMax.z = 0.01f;

	Vector3 vertices[8];
	vertices[0] = projMin;
	vertices[1] = Vector3( projMax.x, projMin.y, projMin.z );
	vertices[2] = Vector3( projMin.x, projMax.y, projMin.z );
	vertices[3] = Vector3( projMax.x, projMax.y, projMin.z );
	vertices[4] = Vector3( projMin.x, projMin.y, projMax.z );
	vertices[5] = Vector3( projMax.x, projMin.y, projMax.z );
	vertices[6] = Vector3( projMin.x, projMax.y, projMax.z );
	vertices[7] = projMax;

	Rect rect;
	for( const auto& vertice : vertices )
	{
		Vector3 projected = projection* vertice;
		rect.Merge( Vector2( projected.x, projected.y ) );
	}

	return rect;
}

Math::Vector3* BoundingBox::ComputeCorners( Math::Vector3* array ) const
{
	array[0] = Vector3( min.x, max.y, max.z ); //Back top left
	array[1] = Vector3( max.x, max.y, max.z ); //Back top right
	array[2] = Vector3( max.x, min.y, max.z ); //Back bottom right
	array[3] = Vector3( min.x, min.y, max.z ); //Back bottom left
	array[4] = Vector3( min.x, max.y, min.z ); //Front top left
	array[5] = Vector3( max.x, max.y, min.z ); //Front top right
	array[6] = Vector3( max.x, min.y, min.z ); //Front bottom right
	array[7] = Vector3( min.x, min.y, min.z ); //Front bottom left

	return array;
}

Intersection BoundingBox::IsInside( const Vector3& vector ) const
{
	if( vector.x < min.x || vector.x > max.x || vector.y < min.y || vector.y > max.y || vector.z < min.z || vector.z > max.z )
		return Intersection::Outside;
	else
		return Intersection::Inside;
}

Intersection BoundingBox::IsInside( const BoundingBox& other ) const
{
	if( other.max.x < min.x || other.min.x > max.x || other.max.x < min.x || other.min.x > max.x || other.max.z < min.z || other.min.z > max.z )
		return Intersection::Outside;
	else if( other.min.x < min.x || other.max.x > max.x || other.min.x < min.x || other.max.x > max.x ||
		other.min.z < min.z || other.max.z > max.z )
		return Intersection::Intersects;
	else
		return Intersection::Inside;
}

Intersection BoundingBox::IsInsideFast( const BoundingBox& other ) const
{	
	if( other.max.x < min.x || other.min.x > max.x || other.max.y < min.y || other.min.y > max.y ||
		other.max.z < min.z || other.min.z > max.z )
		return Intersection::Outside;
	else
		return Intersection::Inside;
}

std::string BoundingBox::ToString() const
{
	return "Min: (" + std::to_string( min.x ) + "," + std::to_string( min.y ) + "," + std::to_string( min.z ) + ")" +
		"Max: (" + std::to_string( max.x ) + "," + std::to_string( max.y ) + "," + std::to_string( max.z ) + ")";
}

}