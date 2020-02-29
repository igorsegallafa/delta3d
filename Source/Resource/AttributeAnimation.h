#pragma once

#include "../Math/Easing.h"
#include "../Math/ValueAnimation.h"
#include "../Math/Color.h"
#include "../Math/Vector2.h"

#include "../Core/TimerImpl.h"

namespace Delta3D::Resource
{

enum class AttributeAnimationType
{
	Undefined,

	Color,
	BlinkingColor,
	ScrollUV,
};

struct RetValue
{
	RetValue() {};

	union
	{
		Math::Color color_;
		Math::Vector2 vector2_;
		Math::Vector2 vector3_;
		float float_;
		int integer_;
	};
};

class AttributeAnimation : public Core::TimerImpl
{
friend class AttributeAnimationFactory;
public:
	//! Default Constructor for Attribute Animation.
	AttributeAnimation( std::string filePath_, int index_ ) : 
		Core::TimerImpl(), 
		loaded( false ),
		filePath( filePath_ ), 
		index( index_ ), 
		animationType( AttributeAnimationType::Undefined ) 
	{
	}

	//! Deconstructor.
	~AttributeAnimation() { Core::Timer::DeleteTimer( this ); loaded = false; if( p ) { delete p; p = nullptr; } }

	//! Update Animation.
	void Update( float elapsedTime );

	//! Get File Path.
	const std::string& GetFilePath() const { return filePath; }

	//! Get Index from Animation.
	int GetAnimationIndex() const { return index; }

	//! Get Final Value from Animation.
	const RetValue& GetValue() const { return value; }

	//! Get Attribute Animation Type.
	const AttributeAnimationType& GetType() const { return animationType; }
private:
	bool loaded;	//!< Animation was loaded

	std::string filePath;	//!< File Path of Animation XML
	int index;	//!< Animation index

	RetValue value;	//!< Return Value
	AttributeAnimationType animationType;	//!< Animation Type

	union
	{
		union
		{
			Math::ValueAnimation<Math::Color>* colorAnimation;
			Math::ValueAnimation<Math::Vector2>* vector2Animation;
			Math::ValueAnimation<float>* floatAnimation;
			Math::ValueAnimation<int>* integerAnimation;
		};

		void* p = nullptr;	//!< Struct to hold all possible animations
	};
};

class AttributeAnimationFactory
{
public:
	//! Default Constructor.
	AttributeAnimationFactory() {}

	//! Deconstructor.
	~AttributeAnimationFactory() {}

	//! Reload Attribute Animations.
	void Reload();

	/**
	* Create Attribute Animation Instance
	* @param filePath File Path of Attribute Animation XML
	* @param index Index of desired animation to create
	* @return Pointer to Attribute Animation Instance Created
	 */
	std::shared_ptr<AttributeAnimation> Create( const std::string& filePath, int index = 0 );
private:
	bool CreateFromXML( std::shared_ptr<AttributeAnimation> attributeAnimation, const std::string& filePath, int index );

	std::unordered_map<std::string, std::shared_ptr<AttributeAnimation>> cache;	//!< Cache
};

}