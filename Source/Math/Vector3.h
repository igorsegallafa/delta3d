#pragma once

#include "Vector2.h"

namespace Delta3D::Math
{
class Matrix4;
class Vector3Int
{
public:
	//! Construct a nulled vector.
	Vector3Int() : x( 0 ), y( 0 ), z( 0 ) {}

	//! Construct a vector 3d from vector 2d.
	Vector3Int( const Vector2Int& v ) : x( v.x ), y( v.y ), z( 0 ) {}

	//! Construct a specified vector.
	Vector3Int( const int x_, const int y_, const int z_ ) : x( x_ ), y( y_ ), z( z_ ) {}

	//! Left Shift operator.
	Vector3Int operator	<<( int i ) const{ return Vector3Int( x << i, y << i, z << i ); }

	//! Right Shift operator.
	Vector3Int operator	>>( int i ) const{ return Vector3Int( x >> i, y >> i, z >> i ); }

	//! Greater operator.
	bool operator >( const Vector3Int& v ) const { return v.x > x && v.y > y && v.z > z; }

	//! Equality operator.
	bool operator ==( const Vector3Int& v ) const{ return v.x == x && v.y == y && v.z == z; }

	//! Inequality operator.
	bool operator !=( const Vector3Int& v ) const{ return v.x != x || v.y != y || v.z != z; }

	//! Add operator.
	Vector3Int operator +( const Vector3Int& v ) const{ return Vector3Int( x + v.x, y + v.y, z + v.z ); }

	//! Subtract operator.
	Vector3Int operator -( const Vector3Int& v ) const{ return Vector3Int( x - v.x, y - v.y, z - v.z ); }

	//! Divide operator with a scalar.
	Vector3Int operator /( const int scalar ) const { return Vector3Int( x / scalar, y / scalar, z / scalar ); }

	//! Divide operator with a vector.
	Vector3Int operator /( const Vector3Int& v ) const { return Vector3Int( x / v.x, y / v.y, z / v.z ); }

	//! Multiply operator with a scalar.
	Vector3Int operator *( const int scalar ) const { return Vector3Int( x * scalar, y * scalar, z * scalar ); }

	//! Multiply operator with a vector.
	Vector3Int operator *( const Vector3Int& v ) const { return Vector3Int( x * v.x, y * v.y, z * v.z ); }

	//! Return Absolute Vector.
	Vector3Int Abs() const
	{
		return Vector3Int( abs( x ), abs( y ), abs( z ) );
	}

	/**
	 * Return Length from vector.
	 */
	float Length() const{ return sqrtf( (float)( x * x + y * y + z * z ) ); }

	/**
	 * Return Distance between two vectors.
	 * @param point Vector where distance will be calculated.
	 */
	float Distance( const Vector3Int& point ) const{ return (*this - point).Length(); }

	/**
	 * Return a formatted string for this vector.
	 */
	std::string ToString() const;

	int x;	//!< X Coordinate
	int y;	//!< Y Coordinate
	int z;	//!< Z Coordinate

	static const Vector3Int Null;	//!< Nulled Vector
};

class Vector3
{
public:
	//! Construct a nulled vector.
	Vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {}

	//! Construct a vector 3d from vector 2d.
	Vector3( const Vector2& v ) : x( v.x ), y( v.y ), z( 0 ) {}

	//! Construct a specified vector with the same coordinates.
	Vector3( const float f ) : x( f ), y( f ), z( f ) {}

	//! Construct a specified vector.
	Vector3( const float x_, const float y_, const float z_ ) : x( x_ ), y( y_ ), z( z_ ) {}

	//! Equality operator.
	bool operator ==( const Vector3& v ) const{ return v.x == x && v.y == y && v.z == z; }

	//! Inequality operator.
	bool operator !=( const Vector3& v ) const{ return v.x != x || v.y != y || v.z != z; }

	//! Add operator.
	Vector3 operator +( const Vector3& v ) const{ return Vector3( x + v.x, y + v.y, z + v.z ); }

	//! Subtract operator.
	Vector3 operator -( const Vector3& v ) const{ return Vector3( x - v.x, y - v.y, z - v.z ); }

	//! Divide operator with a scalar.
	Vector3 operator /( const float scalar ) const { return Vector3( x / scalar, y / scalar, z / scalar ); }

	//! Divide operator with a vector.
	Vector3 operator /( const Vector3Int& v ) const { return Vector3( x / v.x, y / v.y, z / v.z ); }

	//! Multiply operator with a scalar.
	Vector3 operator *( const float scalar ) const { return Vector3( x * scalar, y * scalar, z * scalar ); }

	//! Multiply operator with a vector.
	Vector3 operator *( const Vector3& v ) const { return Vector3( x * v.x, y * v.y, z * v.z ); }

	//! Normalize this vector.
	Vector3& Normalize()
	{
		float len = Length();

		x /= len;
		y /= len;
		z /= len;

		return *this;
	}

	//! Return Normalized Vector.
	Vector3 Normalized() const
	{
		float lenSquared = LengthSquared();
		if( lenSquared != 1.0f && lenSquared > 0.0f )
		{
			float invLen = 1.0f / sqrtf( lenSquared );
			return Vector3( x, y, z ) * invLen;
		}
		else
			return Vector3( x, y, z );
	}

	//! Return Absolute Vector.
	Vector3 Abs() const
	{ 
		return Vector3( abs( x ), abs( y ), abs( z ) ); 
	}

	/**
	 * Return Length Squared from vector.
	 */
	float LengthSquared() const { return x * x + y * y + z * z; }

	/**
	 * Return Length from vector.
	 */
	float Length() const{ return sqrtf( x * x + y * y + z * z ); }

	/**
	 * Return Distance between two vectors.
	 * @param point Vector where distance will be calculated.
	 */
	float Distance( const Vector3& point ) const{ return (*this - point).Length(); }

	/**
	 * Return Dot Product between two vectors.
	 */
	float DotProduct( const Vector3& other ) const 
	{ 
		return x * other.x + y * other.y + z * other.z;
	}

	/**
	 * Return Cross Product between two vectors.
	 */
	Vector3 CrossProduct( const Vector3& other ) const
	{
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	/**
	 * Transform Vector3 v with Matrix m.
	 */
	void Transform( const Vector3& v, const Matrix4& m );

	/**
	 * Get Matrix on D3D Format.
	 */
	D3DXVECTOR3 Get() const{ return D3DXVECTOR3( f ); }

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
			float z;	//!< Z Coordinate
		};

		struct
		{
			float r;	//!< X Coordinate
			float g;	//!< Y Coordinate
			float b;	//!< Z Coordinate
		};

		float f[3];
	};

	static const Vector3 Null;	//!< Nulled Vector
};

inline Vector3 operator *( float scalar, const Vector3& other ) { return other * scalar; }
}