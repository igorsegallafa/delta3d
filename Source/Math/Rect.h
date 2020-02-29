#pragma once

#include "Vector2.h"

namespace Delta3D::Math
{
class RectInt
{
public:
	//! Construct a nulled vector.
	RectInt() : x( 0 ), y( 0 ), w( 0 ), h( 0 ) {}

	//! Construct a specified rectangle.
	RectInt( const int x_, const int y_, const int w_, const int h_ ) : x( x_ ), y( y_ ), w( w_ ), h( h_ ) {}

	//! Construct a specified rectangle with only x and y.
	RectInt( const int x_, const int y_ ) : x( x_ ), y( y_ ), w( 0 ), h( 0 ) {}

	//! Construct a rectangle from vector 2d.
	RectInt( const Vector2Int& v ) : x( v.x ), y( v.y ), w( 0 ), h( 0 ) {}

	//! Left Shift operator.
	RectInt operator <<( int i ) const { return RectInt( x << i, y << i, w << i, h << i ); }

	//! Right Shift operator.
	RectInt operator >>( int i ) const { return RectInt( x >> i, y >> i, w >> i, h >> i ); }

	//! Equality operator.
	bool operator ==( const RectInt& v ) const{ return v.x == x && v.y == y && v.w == w && v.h == h; }

	//! Inequality operator.
	bool operator !=( const RectInt& v ) const{ return v.x != x || v.y != y || v.w != w || v.h != h; }

	//! Add operator.
	RectInt operator +( const RectInt& v ) const{ return RectInt( x + v.x, y + v.y, w + v.w, h + v.h ); }

	//! Subtract operator.
	RectInt operator -( const RectInt& v ) const{ return RectInt( x - v.x, y - v.y, w - v.w, h - v.h ); }

	//! Divide operator with a scalar.
	RectInt operator /( const int scalar ) const { return RectInt( x / scalar, y / scalar, w / scalar, h / scalar ); }

	//! Divide operator with a vector.
	RectInt operator /( const RectInt& v ) const { return RectInt( x / v.x, y / v.y, w / v.w, h / v.h ); }

	//! Multiply operator with a scalar.
	RectInt operator *( const int scalar ) const { return RectInt( x * scalar, y * scalar, w * scalar, h * scalar ); }

	//! Multiply operator with a vector.
	RectInt operator *( const RectInt& v ) const { return RectInt( x * v.x, y * v.y, w * v.w, h * v.h ); }

	/**
	 * Return if specified point is inside of rectangle.
	 * @param point Vector where the condition will be checked.
	 */
	bool IsInside( const Vector2Int& v ) const { return v.x >= x && v.x <= x + w && v.y >= y && v.y <= y + h; }

	/**
	 * Return a formatted string for this rectangle.
	 */
	std::string ToString() const;

	union
	{
		struct
		{
			int x;	//!< X Coordinate
			int y;	//!< Y Coordinate
			int w;	//!< Width
			int h;	//!< Height
		};

		struct
		{
			int left;	//!< Left Coordinate
			int top;	//!< Top Coordinate
			int right;	//!< Right Coordinate
			int bottom;	//!< Bottom Coordinate
		};
	};

	static const RectInt Null;	//!< Nulled Rectangle
};

class Rect
{
public:
	//! Construct a nulled vector.
	Rect() : x( 0.0f ), y( 0.0f ), w( 0.0f ), h( 0.0f ) {}

	//! Construct a specified rectangle with only x and y.
	Rect( const float x_, const float y_ ) : x( x_ ), y( y_ ), w( 0.0f ), h( 0.0f ) {}

	//! Construct a specified rectangle.
	Rect( const float x_, const float y_, const float w_, const float h_ ) : x( x_ ), y( y_ ), w( w_ ), h( h_ ) {}

	//! Construct a rectangle from vector 2d.
	Rect( const Vector2& v ) : x( v.x ), y( v.y ), w( 0.0f ), h( 0.0f ) {}

	//! Equality operator.
	bool operator ==( const Rect& v ) const { return v.x == x && v.y == y && v.w == w && v.h == h; }

	//! Inequality operator.
	bool operator !=( const Rect& v ) const { return v.x != x || v.y != y || v.w != w || v.h != h; }

	//! Add operator.
	Rect operator +( const Rect& v ) const { return Rect( x + v.x, y + v.y, w + v.w, h + v.h ); }

	//! Subtract operator.
	Rect operator -( const Rect& v ) const { return Rect( x - v.x, y - v.y, w - v.w, h - v.h ); }

	//! Divide operator with a scalar.
	Rect operator /( const float scalar ) const { return Rect( x / scalar, y / scalar, w / scalar, h / scalar ); }

	//! Divide operator with a vector.
	Rect operator /( const Rect& v ) const { return Rect( x / v.x, y / v.y, w / v.w, h / v.h ); }

	//! Multiply operator with a scalar.
	Rect operator *( const float scalar ) const { return Rect( x * scalar, y * scalar, w * scalar, h * scalar ); }

	//! Multiply operator with a vector.
	Rect operator *( const Rect& v ) const { return Rect( x * v.x, y * v.y, w * v.w, h * v.h ); }

	/**
	 * Merge Rectangle with a Point
	 */
	void Merge( const Vector2& point )
	{
		if( point.x < x )
			x = point.x;
		if( point.x > w )
			w = point.x;
		if( point.y < y )
			y = point.y;
		if( point.y > h )
			h = point.y;
	}

	/**
	 * Return if specified point is inside of rectangle.
	 * @param point Vector where the condition will be checked.
	 */
	bool IsInside( const Vector2& v ) const { return v.x >= x && v.x <= x + w && v.y >= y && v.y <= y + h; }

	/**
	 * Return a formatted string for this rectangle.
	 */
	std::string ToString() const;

	union
	{
		struct
		{
			float x;	//!< X Coordinate
			float y;	//!< Y Coordinate
			float w;	//!< Width
			float h;	//!< Height
		};

		struct
		{
			float left;	//!< Left Coordinate
			float top;	//!< Top Coordinate
			float right;	//!< Right Coordinate
			float bottom;	//!< Bottom Coordinate
		};
	};

	static const Rect Null;	//!< Nulled Rectangle
};
}