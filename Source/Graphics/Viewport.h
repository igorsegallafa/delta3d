#pragma once

namespace Delta3D::Graphics
{
class Viewport
{
public:
	//! Construct a default Viewport.
	Viewport() : x( 0 ), y( 0 ), width( 1024 ), height( 768 ), minZ( 0.0f ), maxZ( 1.0f ) {}

	//! Construct a specified Viewport.
	Viewport( const DWORD x_, const DWORD y_, const DWORD width_, const DWORD height_, const float minZ_, const float maxZ_ ) : 
		x( x_ ), y( y_ ), width( width_ ), height( height_ ), minZ( minZ_ ), maxZ( maxZ_ ) {}

	//! Construct a specified Viewport without depth range.
	Viewport( const DWORD x_, const DWORD y_, const DWORD width_, const DWORD height_ ) : 
		x( x_ ), y( y_ ), width( width_ ), height( height_ ), minZ( 0.0f ), maxZ( 1.0f ) {}

	//! Construct a specified Viewport only with width and height.
	Viewport( const DWORD width_, const DWORD height_ ) :  x( 0 ), y( 0 ), width( width_ ), height( height_ ), minZ( 0.0f ), maxZ( 1.0f ) {}

	/**
	* Get Matrix on D3D Format.
	 */
	D3DVIEWPORT9 Get() const { D3DVIEWPORT9 s; memcpy_s( &s, sizeof( D3DVIEWPORT9 ), this, sizeof( Viewport ) ); return s; }

	/**
	* Return a formatted string for this Viewport.
	 */
	std::string ToString() const;

	DWORD x;		//!< X Coordinate
	DWORD y;		//!< Y Coordinate
	DWORD width;	//!< Width Value
	DWORD height;	//!< Height Value
	float minZ;		//!< Depth Min Value
	float maxZ;		//!< Depth Max Value
};
}