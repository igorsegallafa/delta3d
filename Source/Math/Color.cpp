#include "PrecompiledHeader.h"
#include "Color.h"

namespace Delta3D::Math
{

const Color Color::White( 1.0f, 1.0f, 1.0f, 1.0f );

const unsigned int Color::ToUInt() const
{
	unsigned int r_ = std::clamp( (int)(r* 255.0f), 0, 255 );
	unsigned int g_ = std::clamp( (int)(g* 255.0f), 0, 255 );
	unsigned int b_ = std::clamp( (int)(b* 255.0f), 0, 255 );
	unsigned int a_ = std::clamp( (int)(a* 255.0f), 0, 255 );
	return (a_ << 24u) | (r_ << 16u) | (g_ << 8u) | b_;
}

void Color::FromUInt( unsigned int color )
{
	a = ( (color >> 24u)& 0xffu ) / 255.0f;
	r = ( (color >> 16u)& 0xffu ) / 255.0f;
	g = ( (color >> 8u )& 0xffu ) / 255.0f;
	b = ( (color >> 0u )& 0xffu ) / 255.0f;
}

std::string Color::ToString() const
{
	return "R: " + std::to_string( r ) + " G: " + std::to_string( g ) + " B: " + std::to_string( b ) + " A: " + std::to_string( a );
}

}
