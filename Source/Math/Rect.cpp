#include "PrecompiledHeader.h"
#include "Rect.h"

namespace Delta3D::Math
{

const RectInt RectInt::Null( 0, 0, 0, 0 );
const Rect Rect::Null( 0.0f, 0.0f, 0.0f, 0.0f );

std::string RectInt::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y ) + " W: " + std::to_string( w ) + " H: " + std::to_string( h );
}

std::string Rect::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y ) + " W: " + std::to_string( w ) + " H: " + std::to_string( h );
}

}