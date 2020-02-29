#pragma once

namespace Delta3D::Math
{
class Color
{
public:
	//! Construct a default Color (white opaque).
	Color() : r( 1.0f ), g( 1.0f ), b( 1.0f ), a( 1.0f ) {}

	//! Construct a specified Color with custom Alpha.
	Color( const Color& c, float a_ ) : r( c.r ), g( c.g ), b( c.b ), a( a_ ) {}

	//! Construct a specified opaque Color.
	Color( const float r_, const float g_, const float b_ ) : r( r_ ), g( g_ ), b( b_ ), a( 1.0f ) {}

	//! Construct a specified Color.
	Color( const float r_, const float g_, const float b_, const float a_ ) : r( r_ ), g( g_ ), b( b_ ), a( a_ ) {}

	//! Construct a specified opaque Color with values range between 0~255.
	Color( const int r_, const int g_, const int b_ ) : r( r_ / 255.0f ), g( g_ / 255.0f ), b( b_ / 255.0f ), a( 1.0f ) {}

	//! Construct a specified Color with values range between 0~255.
	Color( const int r_, const int g_, const int b_, const int a_ ) : r( r_ / 255.0f ), g( g_ / 255.0f ), b( b_ / 255.0f ), a( a_ / 255.0f ) {}

	//! Construct a specified Color from unsigned int.
	Color( unsigned int color ) { FromUInt( color ); }

	//! Equality operator.
	bool operator ==( const Color& other ) const { return other.r == r && other.g == g && other.b == b && other.a == a; }

	//! Inequality operator.
	bool operator !=( const Color& other ) const { return other.r != r || other.g != g || other.b != b || other.a != a; }

	//! Add operator.
	Color operator +( const Color& v ) const { return Color( r + v.r, g + v.g, b + v.b, a + v.a ); }

	//! Subtract operator.
	Color operator -( const Color& v ) const { return Color( r - v.r, g - v.g, b - v.b, a - v.a ); }

	//! Divide operator with a scalar.
	Color operator /( const float scalar ) const { return Color( r / scalar, g / scalar, b / scalar, a / scalar ); }

	//! Divide operator with a color.
	Color operator /( const Color& v ) const { return Color( r / v.r, g / v.g, b / v.b, a / v.a ); }

	//! Multiply operator with a scalar.
	Color operator *( const float scalar ) const { return Color( r * scalar, g * scalar, b * scalar, a * scalar ); }

	//! Multiply operator with a color.
	Color operator *( const Color& v ) const { return Color( r * v.r, g * v.g, b * v.b, a * v.a ); }

	//! Negation operator.
	Color operator -() const { return Color( -r, -g, -b, -a ); }

	/**
	 * Convert Color structure to unsigned int.
	 */
	const unsigned int ToUInt() const;

	/**
	 * Convert Unsigned int to Color Structure.
	 */
	void FromUInt( unsigned int color );

	/**
	 * Get Colors Value
	 */
	const float Red() const { return r; }
	const float Green() const { return g; }
	const float Blue() const { return b; }
	const float Alpha() const { return a; }

	/**
	 * Return a formatted string for this Color.
	 */
	std::string ToString() const;

	float r;	//!< Red Value
	float g;	//!< Green Value
	float b;	//!< Blue Value
	float a;	//!< Alpha Value

	static const Color White;	//!< White Opaque Color
};

// Multiply Color with a scalar.
inline Color operator *( float lhs, const Color& rhs ) { return rhs * lhs; }

}