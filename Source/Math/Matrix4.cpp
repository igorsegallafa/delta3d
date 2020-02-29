#include "PrecompiledHeader.h"
#include "Matrix4.h"
#include "Quaternion.h"

namespace Delta3D::Math
{

const Matrix4 Matrix4::Null( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
const Matrix4 Matrix4::Identity( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );

Matrix4 Matrix4::operator*( const Matrix4& other ) const
{
	Matrix4 result;

	__m128 r0 = _mm_loadu_ps( &other._11 );
	__m128 r1 = _mm_loadu_ps( &other._21 );
	__m128 r2 = _mm_loadu_ps( &other._31 );
	__m128 r3 = _mm_loadu_ps( &other._41 );

	__m128 l = _mm_loadu_ps( &_11 );
	__m128 t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	__m128 t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	__m128 t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	__m128 t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._11, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_21 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._21, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_31 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._31, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_41 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._41, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	return result;
}

Vector3 Matrix4::operator*( const Vector3& other ) const
{
	Vector3 result;

	result.x = (m[0][0]* other.x) + (m[1][0]* other.y) + (m[2][0]* other.z) + m[3][0];
	result.y = (m[0][1]* other.x) + (m[1][1]* other.y) + (m[2][1]* other.z) + m[3][1];
	result.z = (m[0][2]* other.x) + (m[1][2]* other.y) + (m[2][2]* other.z) + m[3][2];

	return result;
}

void Matrix4::RotateAxis( const Vector3& axis, const float angle )
{
	Quaternion q( axis, angle );

	*this = q.ToMatrix();
}

void Matrix4::TranslateAxis( const Vector3& origin, const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ )
{
	_11 = axisX.x;  _12 = axisX.y;  _13 = axisX.z;  _14 = 0.0f;
	_21 = axisY.x;  _22 = axisY.y;  _23 = axisY.z;  _24 = 0.0f;
	_31 = axisZ.x;  _32 = axisZ.y;  _33 = axisZ.z;  _34 = 0.0f;
	_41 = origin.x; _42 = origin.y; _43 = origin.z; _44 = 1.0f;
}

void Matrix4::RotateX( const float rad )
{
	m[1][1] = cosf( rad );
	m[2][2] = m[1][1];
	m[1][2] = sinf( rad );
	m[2][1] = -m[1][2];
}

void Matrix4::RotateY( const float rad )
{
	m[0][0] = cosf( rad );
	m[2][2] = m[0][0];
	m[2][0] = sinf( rad );
	m[0][2] = -m[2][0];
}

void Matrix4::RotateZ( const float rad )
{
	m[0][0] = cosf( rad );
	m[1][1] = m[0][0];
	m[0][1] = sinf( rad );
	m[1][0] = -m[0][1];
}

Matrix4 Matrix4::FlippedYZ()
{
	Math::Matrix4 result = *this;

	for( int i = 0; i < 4; i++ )
		std::swap( result.m[i][1], result.m[i][2] );

	return result;
}

Matrix4 Matrix4::Inverse() const
{
	float v0 = m[2][0]* m[3][1] - m[2][1]* m[3][0];
    float v1 = m[2][0]* m[3][2] - m[2][2]* m[3][0];
    float v2 = m[2][0]* m[3][3] - m[2][3]* m[3][0];
    float v3 = m[2][1]* m[3][2] - m[2][2]* m[3][1];
    float v4 = m[2][1]* m[3][3] - m[2][3]* m[3][1];
    float v5 = m[2][2]* m[3][3] - m[2][3]* m[3][2];

    float i00 = (v5* m[1][1] - v4* m[1][2] + v3* m[1][3]);
    float i10 = -(v5* m[1][0] - v2* m[1][2] + v1* m[1][3]);
    float i20 = (v4* m[1][0] - v2* m[1][1] + v0* m[1][3]);
    float i30 = -(v3* m[1][0] - v1* m[1][1] + v0* m[1][2]);

    float invDet = 1.0f / (i00* m[0][0] + i10* m[0][1] + i20* m[0][2] + i30* m[0][3]);

    i00 *= invDet;
    i10 *= invDet;
    i20 *= invDet;
    i30 *= invDet;

    float i01 = -(v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i11 = (v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i21 = -(v4* m[0][0] - v2* m[0][1] + v0* m[0][3])* invDet;
    float i31 = (v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    v0 = m[1][0]* m[3][1] - m[1][1]* m[3][0];
    v1 = m[1][0]* m[3][2] - m[1][1]* m[3][0];
    v2 = m[1][0]* m[3][3] - m[1][3]* m[3][0];
    v3 = m[1][1]* m[3][2] - m[1][2]* m[3][1];
    v4 = m[1][1]* m[3][3] - m[1][3]* m[3][1];
    v5 = m[1][2]* m[3][3] - m[1][3]* m[3][2];

    float i02 = (v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i12 = -(v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i22 = (v4* m[0][0] - v2* m[0][1] + v0* m[0][3])* invDet;
    float i32 = -(v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    v0 = m[2][1]* m[1][0] - m[2][0]* m[1][1];
    v1 = m[2][2]* m[1][0] - m[2][0]* m[1][2];
    v2 = m[2][3]* m[1][0] - m[2][0]* m[1][3];
    v3 = m[2][2]* m[1][1] - m[2][1]* m[1][2];
    v4 = m[2][3]* m[1][1] - m[2][1]* m[1][3];
    v5 = m[2][3]* m[1][2] - m[2][2]* m[1][3];

    float i03 = -(v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i13 = (v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i23 = -(v4* m[0][0] - v2* m[0][2] + v0* m[0][3])* invDet;
    float i33 = (v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    return Matrix4( i00, i01, i02, i03, i10, i11, i12, i13, i20, i21, i22, i23, i30, i31, i32, i33);
}

void Matrix4::BulkTransposeTo3x4( float* dest, const float* src, unsigned int count )
{
	for( unsigned int i = 0; i < count; ++i )
	{
		__m128 m0 = _mm_loadu_ps( src );
		__m128 m1 = _mm_loadu_ps( src + 4 );
		__m128 m2 = _mm_loadu_ps( src + 8 );
		__m128 m3 = _mm_loadu_ps( src + 12 );
		_MM_TRANSPOSE4_PS( m0, m1, m2, m3 );
		_mm_storeu_ps( dest, m0 );
		_mm_storeu_ps( dest + 4, m2 );
		_mm_storeu_ps( dest + 8, m1 );

		dest += 12;
		src += 16;
	}
}


}