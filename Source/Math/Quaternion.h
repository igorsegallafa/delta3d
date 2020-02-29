#pragma once

#include "Matrix4.h"
#include "Vector3.h"

namespace Delta3D::Math
{
class Quaternion
{
public:
	//! Construct an identity Quaternion.
	Quaternion() : x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 1.0f ) {}

	//! Construct a specified Quaternion from Axis and Angle.
	Quaternion( const Vector3& axis, const float angle ) { FromAxisAngle( axis, angle ); }

	//! Construct a specified Quaternion from Euler Angles.
	Quaternion( const float x_, const float y_, const float z_ ) { FromEulerAngles( x_, y_, z_ ); }

	//! Construct a specified Quaternion.
	Quaternion( const float x_, const float y_, const float z_, const float w_ ) : x( x_ ), y( y_ ), z( z_ ), w( w_ ) {}

	//! Construct a specified Quaternion with SSE.
	explicit Quaternion( __m128 xyzw ) noexcept
	{
		_mm_storeu_ps( &x, xyzw );
	}

	//! Multiply operator with a scalar.
	Quaternion operator *( const float scalar ) const { return Quaternion( _mm_mul_ps( _mm_loadu_ps( &x ), _mm_set1_ps( scalar ) ) ); }

	//! Add operator.
	Quaternion operator +( const Quaternion& other ) const { return Quaternion( _mm_add_ps( _mm_loadu_ps( &x ), _mm_loadu_ps( &other.x ) ) ); }

	//! Subtract operator.
	Quaternion operator -( const Quaternion& other ) const { return Quaternion( _mm_sub_ps( _mm_loadu_ps( &x ), _mm_loadu_ps( &other.x ) ) ); }

	//! Return negation.
	Quaternion operator -() const
	{
		return Quaternion( _mm_xor_ps( _mm_loadu_ps( &x ), _mm_castsi128_ps( _mm_set1_epi32( (int)0x80000000UL ) ) ) );
	}

	//! Multiply a Vector3.
	Vector3 operator *( const Vector3& other ) const
	{
		Vector3 qVec( x, y, z );
		Vector3 cross1( qVec.CrossProduct( other ) );
		Vector3 cross2( qVec.CrossProduct( cross1 ) );

		return other + 2.0f * ( cross1 * w + cross2 );
	}

	/**
	 * Get quaternion on Matrix Format.
	 */
	Matrix4 ToMatrix() const;

	/**
	 * Spherical interpolation between quaternions.
	 */
	Quaternion Slerp( const Quaternion& other, float t ) const;

	/**
	 * Normalized linear interpolation between quaternions.
	 */
	Quaternion Nlerp( const Quaternion& other, float t, bool shortestPath ) const;

	/**
	 * Dot Product between quaternions.
	 */
	float DotProduct( const Quaternion& other ) const
	{
		__m128 q1 = _mm_loadu_ps( &x );
		__m128 q2 = _mm_loadu_ps( &other.x );
		__m128 n = _mm_mul_ps( q1, q2 );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 2, 3, 0, 1 ) ) );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 0, 1, 2, 3 ) ) );
		return _mm_cvtss_f32( n );
	}

	/**
	 * Normalize Quaternion.
	 */
	void Normalize()
	{
		__m128 q = _mm_loadu_ps( &x );
		__m128 n = _mm_mul_ps( q, q );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 2, 3, 0, 1 ) ) );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 0, 1, 2, 3 ) ) );
		__m128 e = _mm_rsqrt_ps( n );
		__m128 e3 = _mm_mul_ps( _mm_mul_ps( e, e ), e );
		__m128 half = _mm_set1_ps( 0.5f );
		n = _mm_add_ps( e, _mm_mul_ps( half, _mm_sub_ps( e, _mm_mul_ps( n, e3 ) ) ) );
		_mm_storeu_ps( &x, _mm_mul_ps( q, n ) );
	}

	/**
	 * Return Normalized Quaternion.
	 */
	Quaternion Normalized() const
	{
		__m128 q = _mm_loadu_ps( &x );
		__m128 n = _mm_mul_ps( q, q );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 2, 3, 0, 1 ) ) );
		n = _mm_add_ps( n, _mm_shuffle_ps( n, n, _MM_SHUFFLE( 0, 1, 2, 3 ) ) );
		__m128 e = _mm_rsqrt_ps( n );
		__m128 e3 = _mm_mul_ps( _mm_mul_ps( e, e ), e );
		__m128 half = _mm_set1_ps( 0.5f );
		n = _mm_add_ps( e, _mm_mul_ps( half, _mm_sub_ps( e, _mm_mul_ps( n, e3 ) ) ) );
		return Quaternion( _mm_mul_ps( q, n ) );
	}

	/**
	 * Rotate around arbitrary axis
	 * @param axis axis to rotate about
	 * @param angle in radians
	 * @return this reference
	 */
	Quaternion& FromAxisAngle( Vector3 axis, const float angle );

	/**
	 * Rotate quaternion from Euler Angles in Degree
	 */
	Quaternion& FromEulerAngles( float x_, float y_, float z_ );

	float x;	//!< X Coordinate
	float y;	//!< Y Coordinate
	float z;	//!< Z Coordinate
	float w;	//!< W Coordinate

	static const Quaternion Identity;	//!< Identity Quaternion
};
}