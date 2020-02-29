#pragma once

namespace Delta3D::Math
{

enum class EasingAnimation
{
	BackIn,
	BackInOut,
	BackOut,
	BounceIn,
	BounceInOut,
	BounceOut,
	CircIn,
	CircInOut,
	CircOut,
	CubicIn,
	CubicInOut,
	CubicOut,
	ElasticIn,
	ElasticInOut,
	ElasticOut,
	ExpoIn,
	ExpoInOut,
	ExpoOut,
	Linear,
	QuadIn,
	QuadInOut,
	QuadOut,
	QuartIn,
	QuartInOut,
	QuartOut,
	QuintIn,
	QuintInOut,
	QuintOut,
	SineIn,
	SineInOut,
	SineOut,
};

class BackInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		float s = 1.70158f;
		float postFix = t /= d;
		return static_cast<T>( c * (postFix)*t*( ( s + 1 )*t - s ) + b );
	}
};

class BackInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		float s = 1.70158f;
		if( ( t /= d / 2 ) < 1 ) return static_cast<T>( c / 2 * ( t*t*( ( ( s *= ( 1.525f ) ) + 1 )*t - s ) ) + b );
		float postFix = t -= 2;
		return static_cast<T>( c / 2 * ( (postFix)*t*( ( ( s *= ( 1.525f ) ) + 1 )*t + s ) + 2 ) + b );
	}
};

class BackOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		float s = 1.70158f;
		return static_cast<T>( c * ( ( t = t / d - 1 )*t*( ( s + 1 )*t + s ) + 1 ) + b );
	}
};

class BounceOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d ) < ( 1 / 2.75f ) )
		{
			return static_cast<T>( c * ( 7.5625f*t*t ) + b );
		}
		else if( t < ( 2 / 2.75f ) )
		{
			float postFix = t -= ( 1.5f / 2.75f );
			return static_cast<T>( c * ( 7.5625f*(postFix)*t + .75f ) + b );
		}
		else if( t < ( 2.5 / 2.75 ) )
		{
			float postFix = t -= ( 2.25f / 2.75f );
			return static_cast<T>( c * ( 7.5625f*(postFix)*t + .9375f ) + b );
		}
		else
		{
			float postFix = t -= ( 2.625f / 2.75f );
			return static_cast<T>( c * ( 7.5625f*(postFix)*t + .984375f ) + b );
		}
	}
};

class BounceInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c - BounceOutEasing::run<T>( d - t, 0, c, d ) + b );
	}
};

class BounceInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( t < d / 2 ) return BounceInEasing::run<T>( t * 2, 0, c, d ) * .5f + b;
		else return BounceOutEasing::run<T>( t * 2 - d, 0, c, d ) * .5f + c * .5f + b;
	}
};

class CircInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( -c * ( sqrt( 1 - ( t /= d )*t ) - 1 ) + b );
	}
};

class CircInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( -c / 2 * ( sqrt( 1 - t * t ) - 1 ) + b );

		return static_cast<T>( c / 2 * ( sqrt( 1 - t * ( t -= 2 ) ) + 1 ) + b );
	}
};

class CircOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * sqrt( 1 - ( t = t / d - 1 )*t ) + b );
	}
};

class CubicInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( t /= d )*t*t + b );
	}
};

class CubicInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( c / 2 * t*t*t + b );

		return static_cast<T>( c / 2 * ( ( t -= 2 )*t*t + 2 ) + b );
	}
};

class CubicOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( ( t = t / d - 1 )*t*t + 1 ) + b );
	}
};

class ElasticInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( t == 0 ) return b;  if( ( t /= d ) == 1 ) return b + c;
		float p = d * .3f;
		T a = c;
		float s = p / 4;
		T postFix = a * powf( 2, 10 * ( t -= 1 ) ); // this is a fix, again, with post-increment operators
		return static_cast<T>( -( postFix * sin( ( t*d - s )*( 2 * D3DX_PI ) / p ) ) + b );
	}
};

class ElasticInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( t == 0 ) return b;  if( ( t /= d / 2 ) == 2 ) return b + c;
		float p = d * ( .3f*1.5f );
		T a = c;
		float s = p / 4;

		if( t < 1 )
		{
			T postFix = a * powf( 2, 10 * ( t -= 1 ) ); // postIncrement is evil
			return static_cast<T>( -.5f*( postFix* sin( ( t*d - s )*( 2 * D3DX_PI ) / p ) ) + b );
		}
		T postFix = a * powf( 2, -10 * ( t -= 1 ) ); // postIncrement is evil
		return static_cast<T>( postFix * sin( ( t*d - s )*( 2 * D3DX_PI ) / p )*.5f + c + b );
	}
};

class ElasticOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( t == 0 ) 
			return b;  
		
		if( ( t /= d ) == 1 ) 
			return b + c;

		float p = d * .3f;
		T a = c;
		float s = p / 4;
		return static_cast<T>( ( a*powf( 2, -10 * t ) * sin( ( t*d - s )*( 2 * D3DX_PI ) / p ) + c + b ) );
	}
};

class ExpoInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( ( t == 0 ) ? b : c * powf( 2, 10 * ( t / d - 1 ) ) + b );
	}
};

class ExpoInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( t == 0 ) 
			return b;

		if( t == d ) 
			return b + c;

		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( c / 2 * powf( 2, 10 * ( t - 1 ) ) + b );

		return static_cast<T>( c / 2 * ( -powf( 2, -10 * --t ) + 2 ) + b );
	}
};

class ExpoOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( ( t == d ) ? b + c : c * ( -powf( 2, -10 * t / d ) + 1 ) + b );
	}
};

class LinearEasing
{
public:	
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c*t / d + b );
	}
};

class QuadInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( t /= d )*t + b );
	}
};

class QuadInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( ( ( c / 2 )*( t*t ) ) + b );

		return static_cast<T>( -c / 2 * ( ( ( t - 2 )*( --t ) ) - 1 ) + b);
	}
};


class QuadOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( -c * ( t /= d )*( t - 2 ) + b );
	}
};

class QuartInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( t /= d )*t*t*t + b );
	}
};

class QuartInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( c / 2 * t*t*t*t + b );

		return static_cast<T>( -c / 2 * ( ( t -= 2 )*t*t*t - 2 ) + b );
	}
};

class QuartOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( -c * ( ( t = t / d - 1 )*t*t*t - 1 ) + b );
	}
};

class QuintInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( t /= d )*t*t*t*t + b );
	}
};

class QuintInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		if( ( t /= d / 2 ) < 1 ) 
			return static_cast<T>( c / 2 * t*t*t*t*t + b );

		return static_cast<T>( c / 2 * ( ( t -= 2 )*t*t*t*t + 2 ) + b );
	}
};

class QuintOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * ( ( t = t / d - 1 )*t*t*t*t + 1 ) + b );
	}
};

class SineInEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( -c * cos( t / d * ( D3DX_PI / 2 ) ) + c + b );
	}
};

class SineInOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( -c / 2 * ( cos( D3DX_PI*t / d ) - 1 ) + b );
	}
};

class SineOutEasing
{
public:
	template<typename T>
	static T run( float t, T b, T c, float d )
	{
		return static_cast<T>( c * sin( t / d * ( D3DX_PI / 2 ) ) + b );
	}
};

namespace Easing
{
	static const BackInEasing BackIn = BackInEasing{};
	static const BackInOutEasing BackInOut = BackInOutEasing{};
	static const BackOutEasing BackOut = BackOutEasing{};
	static const BounceInEasing BounceIn = BounceInEasing{};
	static const BounceInOutEasing BounceInOut = BounceInOutEasing{};
	static const BounceOutEasing BounceOut = BounceOutEasing{};
	static const CircInEasing CircIn = CircInEasing{};
	static const CircInOutEasing CircInOut = CircInOutEasing{};
	static const CircOutEasing CircOut = CircOutEasing{};
	static const CubicInEasing CubicIn = CubicInEasing{};
	static const CubicInOutEasing CubicInOut = CubicInOutEasing{};
	static const CubicOutEasing CubicOut = CubicOutEasing{};
	static const ElasticInEasing ElasticIn = ElasticInEasing{};
	static const ElasticInOutEasing ElasticInOut = ElasticInOutEasing{};
	static const ElasticOutEasing ElasticOut = ElasticOutEasing{};
	static const ExpoInEasing ExpoIn = ExpoInEasing{};
	static const ExpoInOutEasing ExpoInOut = ExpoInOutEasing{};
	static const ExpoOutEasing ExpoOut = ExpoOutEasing{};
	static const LinearEasing Linear = LinearEasing{};
	static const QuadInEasing QuadIn = QuadInEasing{};
	static const QuadInOutEasing QuadInOut = QuadInOutEasing{};
	static const QuadOutEasing QuadOut = QuadOutEasing{};
	static const QuartInEasing QuartIn = QuartInEasing{};
	static const QuartInOutEasing QuartInOut = QuartInOutEasing{};
	static const QuartOutEasing QuartOut = QuartOutEasing{};
	static const QuintInEasing QuintIn = QuintInEasing{};
	static const QuintInOutEasing QuintInOut = QuintInOutEasing{};
	static const QuintOutEasing QuintOut = QuintOutEasing{};
	static const SineInEasing SineIn = SineInEasing{};
	static const SineInOutEasing SineInOut = SineInOutEasing{};
	static const SineOutEasing SineOut = SineOutEasing{};
}

inline EasingAnimation GetEasingFromString( const std::string& str )
{
	if( str.compare( "BackIn" ) == 0 )
		return EasingAnimation::BackIn;
	else if( str.compare( "BackInOut" ) == 0 )
		return EasingAnimation::BackInOut;
	else if( str.compare( "BackOut" ) == 0 )
		return EasingAnimation::BackOut;
	else if( str.compare( "BounceIn" ) == 0 )
		return EasingAnimation::BounceIn;
	else if( str.compare( "BounceInOut" ) == 0 )
		return EasingAnimation::BounceInOut;
	else if( str.compare( "BounceOut" ) == 0 )
		return EasingAnimation::BounceOut;
	else if( str.compare( "CircIn" ) == 0 )
		return EasingAnimation::CircIn;
	else if( str.compare( "CircInOut" ) == 0 )
		return EasingAnimation::CircInOut;
	else if( str.compare( "CircOut" ) == 0 )
		return EasingAnimation::CircOut;
	else if( str.compare( "CubicIn" ) == 0 )
		return EasingAnimation::CubicIn;
	else if( str.compare( "CubicInOut" ) == 0 )
		return EasingAnimation::CubicInOut;
	else if( str.compare( "CubicOut" ) == 0 )
		return EasingAnimation::CubicOut;
	else if( str.compare( "ElasticIn" ) == 0 )
		return EasingAnimation::ElasticIn;
	else if( str.compare( "ElasticInOut" ) == 0 )
		return EasingAnimation::ElasticInOut;
	else if( str.compare( "ElasticOut" ) == 0 )
		return EasingAnimation::ElasticOut;
	else if( str.compare( "ExpoIn" ) == 0 )
		return EasingAnimation::ExpoIn;
	else if( str.compare( "ExpoInOut" ) == 0 )
		return EasingAnimation::ExpoInOut;
	else if( str.compare( "ExpoOut" ) == 0 )
		return EasingAnimation::ExpoOut;
	else if( str.compare( "Linear" ) == 0 )
		return EasingAnimation::Linear;
	else if( str.compare( "QuadIn" ) == 0 )
		return EasingAnimation::QuadIn;
	else if( str.compare( "QuadInOut" ) == 0 )
		return EasingAnimation::QuadInOut;
	else if( str.compare( "QuadOut" ) == 0 )
		return EasingAnimation::QuadOut;
	else if( str.compare( "QuartIn" ) == 0 )
		return EasingAnimation::QuartIn;
	else if( str.compare( "QuartInOut" ) == 0 )
		return EasingAnimation::QuartInOut;
	else if( str.compare( "QuartOut" ) == 0 )
		return EasingAnimation::QuartOut;
	else if( str.compare( "QuintIn" ) == 0 )
		return EasingAnimation::QuintIn;
	else if( str.compare( "QuintInOut" ) == 0 )
		return EasingAnimation::QuintInOut;
	else if( str.compare( "QuintOut" ) == 0 )
		return EasingAnimation::QuintOut;
	else if( str.compare( "SineIn" ) == 0 )
		return EasingAnimation::SineIn;
	else if( str.compare( "SineInOut" ) == 0 )
		return EasingAnimation::SineInOut;
	else if( str.compare( "SineOut" ) == 0 )
		return EasingAnimation::SineOut;

	return EasingAnimation::Linear;
}

}