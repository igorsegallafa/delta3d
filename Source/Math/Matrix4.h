#pragma once

#include "Vector3.h"

namespace Delta3D::Math
{
class Quaternion;

class Matrix4
{
public:
	//! Construct an identity matrix.
	Matrix4()
	{
		_mm_storeu_ps( &_11, _mm_set_ps( 0.f, 0.f, 0.f, 1.f ) );
		_mm_storeu_ps( &_21, _mm_set_ps( 0.f, 0.f, 1.f, 0.f ) );
		_mm_storeu_ps( &_31, _mm_set_ps( 0.f, 1.f, 0.f, 0.f ) );
		_mm_storeu_ps( &_41, _mm_set_ps( 1.f, 0.f, 0.f, 0.f ) );
	}

	//! Construct a specified matrix.
	Matrix4( const float m11, const float m12, const float m13, const float m14, const float m21, const float m22, const float m23, const float m24,
		const float m31, const float m32, const float m33, const float m34, const float m41, const float m42, const float m43, const float m44 ) :
		_11( m11 ), _12( m12 ), _13( m13 ), _14( m14 ), _21( m21 ), _22( m22 ), _23( m23 ), _24( m24 ), _31( m31 ), _32( m32 ), _33( m33 ), _34( m34 ), 
		_41( m41 ), _42( m42 ), _43( m43 ), _44( m44 ){}

	//! Multiply operator with a Matrix4.
	Matrix4 operator *( const Matrix4& other ) const;

	//! Multiply operator with a Vector3.
	Vector3 operator *( const Vector3& other ) const;

	/** 
	 * Set Translate Transformation on Matrix
	 */
	void Translate( const Vector3& translation ) { _41 = translation.x; _42 = translation.y; _43 = translation.z; }

	/**
	 * Rotate Matrix around arbitrary axis
	 * Angle is clockwise when viewed from the positive side of the arbitrary axis towards the origin
	 * @param angle in radians
	 */
	void RotateAxis( const Vector3& axis, const float angle );

	void TranslateAxis( const Vector3& origin, const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ );

	/**
	 * Rotate matrix around X coordinate
	 * @param angle in radians
	 */
	void RotateX( const float rad );

	/**
	 * Rotate matrix around Y coordinate
	 * @param angle in radians
	 */
	void RotateY( const float rad );

	/**
	 * Rotate matrix around Z coordinate
	 * @param angle in radians
	 */
	void RotateZ( const float rad );

	/**
	 * Scale matrix
	 * @param scale Scale Value
	 */
	void Scale( const float scale )
	{
		_11 = scale;
		_22 = scale;
		_33 = scale;
	}

	/**
	 * Scale matrix
	 * @param scale Scale Value
	 */
	void Scale( const Math::Vector3& scale )
	{
		_11 = scale.x;
		_22 = scale.y;
		_33 = scale.z;
	}

	/**
	 * Return the Flipped Matrix.
	 */
	Matrix4 FlippedYZ();

	/**
	 * Flip Y and Z Coordinates of Matrix.
	 */
	Matrix4& FlipYZ() { *this = FlippedYZ(); return *this; }

	/**
	 * Return the inverse from current Matrix.
	 */
	Matrix4 Inverse() const;

	/**
	 * Invert current Matrix.
	 */
	Matrix4& Invert() { *this = Inverse(); return *this; }


	/**
	 * Get Matrix on D3D Format.
	 */
	D3DXMATRIX Get() const{ return D3DXMATRIX( f ); }

	static void BulkTransposeTo3x4( float* dest, const float* src, unsigned int count );

	union
	{
		/** 
		 * Matrix Elements 
		 */
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;

		};

		float f[16];
		float m[4][4]; //!< Matrix  array format
	};

	static const Matrix4 Null;	//!< Nulled Matrix
	static const Matrix4 Identity;	//!< Identity Matrix
};
}