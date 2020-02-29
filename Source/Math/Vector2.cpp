#include "PrecompiledHeader.h"
#include "Vector2.h"

namespace Delta3D::Math
{

const Vector2Int Vector2Int::Null( 0, 0 );
const Vector2 Vector2::Null( 0.0f, 0.0f );

std::string Vector2Int::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y );
}

std::string Vector2::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y );
}

}