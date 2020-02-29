#include "PrecompiledHeader.h"
#include "Viewport.h"

namespace Delta3D::Graphics
{

std::string Viewport::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y ) + " W: " + std::to_string( width ) + " H: " + std::to_string( height ) + 
		" minZ: " + std::to_string( minZ ) + " maxZ: " + std::to_string( maxZ );
}

}