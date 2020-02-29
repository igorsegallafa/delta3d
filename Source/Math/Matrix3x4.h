#pragma once

namespace Delta3D::Math
{
class Matrix3x4
{
public:
	//! Construct an identity matrix.
	Matrix3x4()
	{
		_mm_storeu_ps( &_11, _mm_set_ps( 0.f, 0.f, 0.f, 1.f ) );
		_mm_storeu_ps( &_21, _mm_set_ps( 0.f, 0.f, 1.f, 0.f ) );
		_mm_storeu_ps( &_31, _mm_set_ps( 0.f, 1.f, 0.f, 0.f ) );
	}

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

		};

		float f[12];
		float m[3][4]; //!< Matrix  array format
	};
};
}