#pragma once

namespace Delta3D::Math
{
class Vector2Int
{
public:
	//! Construct a nulled vector.
	Vector2Int() : x( 0 ), y( 0 ) {}

	//! Construct a specified vector.
	Vector2Int( const int x_, const int y_ ) : x( x_ ), y( y_ ) {}

	//! Left Shift operator.
	Vector2Int operator	<<( int i ) const { return Vector2Int( x << i, y << i ); }

	//! Right Shift operator.
	Vector2Int operator	>>( int i ) const { return Vector2Int( x >> i, y >> i ); }

	//! Equality operator.
	bool operator ==( const Vector2Int& v ) const{ return v.x == x && v.y == y; }

	//! Inequality operator.
	bool operator !=( const Vector2Int& v ) const{ return v.x != x || v.y != y; }

	//! Add operator.
	Vector2Int operator +( const Vector2Int& v ) const{ return Vector2Int( x + v.x, y + v.y ); }

	//! Subtract operator.
	Vector2Int operator -( const Vector2Int& v ) const{ return Vector2Int( x - v.x, y - v.y ); }

	//! Divide operator with a scalar.
	Vector2Int operator /( const int scalar ) const { return Vector2Int( x / scalar, y / scalar ); }

	//! Divide operator with a vector.
	Vector2Int operator /( const Vector2Int& v ) const { return Vector2Int( x / v.x, y / v.y ); }

	//! Multiply operator with a scalar.
	Vector2Int operator *( const int scalar ) const { return Vector2Int( x * scalar, y * scalar ); }

	//! Multiply operator with a vector.
	Vector2Int operator *( const Vector2Int& v ) const { return Vector2Int( x * v.x, y * v.y ); }

	/**
	 * Return Length from vector.
	 */
	float Length() const { return sqrtf( (float)(x * x + y * y) ); }

	/**
	 * Return Distance between two vectors.
	 * @param point Vector where distance will be calculated.
	 */
	float Distance( const Vector2Int& point ) const { return ( *this - point ).Length(); }

	/**
	 * Return a formatted string for this vector.
	 */
	std::string ToString() const;

	union
	{
		struct
		{
			int x;	//!< X Coordinate
			int y;	//!< Y Coordinate
		};

		struct
		{
			int width;	//!< Width Value
			int height;	//!< Height Value
		};

		struct
		{
			int u;	//!< U Coordinate
			int v;	//!< V Coordinate
		};
	};

	static const Vector2Int Null;	//!< Nulled Vector
};

class Vector2
{
public:
	//! Construct a nulled vector.
	Vector2() : x( 0.0f ), y( 0.0f ) {}

	//! Construct a specified vector.
	Vector2( const float value_ ) : x( value_ ), y( value_ ) {}

	//! Construct a specified vector.
	Vector2( const float x_, const float y_ ) : x( x_ ), y( y_ ) {}

	//! Equality operator.
	bool operator ==( const Vector2& v ) const { return v.x == x && v.y == y; }

	//! Inequality operator.
	bool operator !=( const Vector2& v ) const { return v.x != x || v.y != y; }

	//! Add operator.
	Vector2 operator +( const Vector2& v ) const { return Vector2( x + v.x, y + v.y ); }

	//! Subtract operator.
	Vector2 operator -( const Vector2& v ) const { return Vector2( x - v.x, y - v.y ); }

	//! Divide operator with a scalar.
	Vector2 operator /( const float scalar ) const { return Vector2( x / scalar, y / scalar ); }

	//! Divide operator with a vector.
	Vector2 operator /( const Vector2& v ) const { return Vector2( x / v.x, y / v.y ); }

	//! Multiply operator with a scalar.
	Vector2 operator *( const float scalar ) const { return Vector2( x * scalar, y * scalar ); }

	//! Multiply operator with a vector.
	Vector2 operator *( const Vector2& v ) const { return Vector2( x * v.x, y * v.y ); }

	//! Negation operator.
	Vector2 operator -() const { return Vector2( -x, -y ); }

	/**
	 * Return Length from vector.
	 */
	float Length() const { return sqrtf( x * x + y * y ); }

	/**
	 * Return Distance between two vectors.
	 * @param point Vector where distance will be calculated.
	 */
	float Distance( const Vector2& point ) const { return ( *this - point ).Length(); }

	/**
	 * Get Vector2 as D3D Object.
	 */
	D3DXVECTOR2 Get() { return D3DXVECTOR2( f ); }

	/**
	 * Return a formatted string for this vector.
	 */
	std::string ToString() const;

	union
	{
		struct
		{
			float x;	//!< X Coordinate
			float y;	//!< Y Coordinate
		};

		struct
		{
			float u;	//!< U Coordinate
			float v;	//!< V Coordinate
		};

		float f[2];
	};

	static const Vector2 Null;	//!< Nulled Vector
};

// Multiply Vector2 with a scalar.
inline Vector2 operator *( float lhs, const Vector2& rhs ) { return rhs * lhs; }

}