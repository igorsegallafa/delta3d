#include "PrecompiledHeader.h"
#include "Quaternion.h"

namespace Delta3D::Math
{

const Quaternion Quaternion::Identity( 0.0f, 0.0f, 0.0f, 1.0f );

Matrix4 Quaternion::ToMatrix() const
{
	Matrix4 result;

	//First Row (LH)
	result.m[0][0] = 1.0f - ( 2.0f* ( ( y* y ) + ( z* z ) ) );
	result.m[0][1] = ( 2.0f* ( ( x* y ) - ( z* w ) ) );
	result.m[0][2] = ( 2.0f* ( ( x* z ) + ( y* w ) ) );

	//Second Row (LH)
	result.m[1][0] = ( 2.0f* ( ( x* y ) + ( z* w ) ) );
	result.m[1][1] = 1.0f - ( 2.0f* ( ( x* x ) + ( z* z ) ) );
	result.m[1][2] = ( 2.0f* ( ( y* z ) - ( x* w ) ) );

	//Third Row (LH)
	result.m[2][0] = ( 2.0f* ( ( x* z ) - ( y* w ) ) );
	result.m[2][1] = ( 2.0f* ( ( y* z ) + ( x* w ) ) );
	result.m[2][2] = 1.0f - ( 2.0f* ( ( x* x ) + ( y* y ) ) );

	return result;
}

Quaternion Quaternion::Slerp( const Quaternion& other, float t ) const
{
	/**
	* Quaternion Slerp Function from Urho3D
	* https://github.com/urho3d/Urho3D/blob/master/Source/Urho3D/Math/Quaternion.cpp
	 */

	//Favor accuracy for native code builds
	float fCosAngle = DotProduct( other );
	float fSignal = 1.0f;

	//Enable shortest path rotation
	if( fCosAngle < 0.0f )
	{
		fCosAngle = -fCosAngle;
		fSignal = -1.0f;
	}

	float fAngle = acosf( fCosAngle );
	float fSinAngle = sinf( fAngle );
	float t1, t2;

	if( fSinAngle > 0.001f )
	{
		float invSinAngle = 1.0f / fSinAngle;
		t1 = sinf( ( 1.0f - t )* fAngle )* invSinAngle;
		t2 = sinf( t* fAngle )* invSinAngle;
	}
	else
	{
		t1 = 1.0f - t;
		t2 = t;
	}

	return *this* t1 + ( other* fSignal )* t2;
}

Quaternion Quaternion::Nlerp( const Quaternion& other, float t, bool shortestPath ) const
{
	Quaternion result;

	float fCos = DotProduct( other );

	if( fCos < 0.0f && shortestPath )
		result = ( *this ) + ( ( ( -other ) - ( *this ) )* t );
	else
		result = ( *this ) + ( ( other - ( *this ) )* t );

	result.Normalize();
	return result;
}

Quaternion& Quaternion::FromAxisAngle( Vector3 axis, const float angle )
{
	axis.Normalize();

	const float angleDiv2 = angle / 2.0f;
	const float sinAngleDiv2 = sinf( angleDiv2 );

	w = cosf( angleDiv2 );
	x = axis.x* sinAngleDiv2;
	y = axis.y* sinAngleDiv2;
	z = axis.z* sinAngleDiv2;

	return *this;
}

Quaternion& Quaternion::FromEulerAngles( float x_, float y_, float z_ )
{
	x_ *= D3DX_PI / 360.f;
	y_ *= D3DX_PI / 360.f;
	z_ *= D3DX_PI / 360.f;

	float sinX = sinf( x_ );
	float cosX = cosf( x_ );
	float sinY = sinf( y_ );
	float cosY = cosf( y_ );
	float sinZ = sinf( z_ );
	float cosZ = cosf( z_ );

	w = cosY* cosX* cosZ + sinY* sinX* sinZ;
	x = cosY* sinX* cosZ + sinY* cosX* sinZ;
	y = sinY* cosX* cosZ - cosY* sinX* sinZ;
	z = cosY* cosX* sinZ - sinY* sinX* cosZ;

	return *this;
}

}