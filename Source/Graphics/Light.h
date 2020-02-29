#pragma once

#include "../Math/Vector3.h"
#include "../Math/Color.h"

namespace Delta3D::Graphics
{
enum class LightType
{
	None,
	Point,
};

enum LightEffect
{
	Undefined,

	Night = 0x00001,
	Lens = 0x00002,
	Pulsing = 0x00004,
	Object = 0x00008,
	Dynamic = 0x80000,
};	DEFINE_ENUM_FLAG_OPERATORS( LightEffect );

class Light
{
public:
	//! Default Constructor for Light.
	Light( unsigned int id_, float range_ ) : id( id_ ), range( range_ ), type( LightType::None ), effect( LightEffect::Undefined ) {}

	//! Deconstructor
	~Light() {}

	unsigned int id;	//!< Light ID
	LightType type;	//!< Light Type
	LightEffect effect;	//!< Light Effect
	Math::Vector3 position;	//!< Light Position
	float range;	//!< Light Range
	Math::Color color;	//!< Light Color
};
}