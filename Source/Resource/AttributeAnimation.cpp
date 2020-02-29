#include "PrecompiledHeader.h"
#include "AttributeAnimation.h"

namespace Delta3D::Resource
{

void AttributeAnimation::Update( float elapsedTime )
{
	//Animation was loaded successfully
	if( loaded == true && !filePath.empty() )
	{
		//Diffuse Position have only one key frame
		if( animationType == AttributeAnimationType::ScrollUV )
			value.vector2_ = vector2Animation->InitialValue();
		else
		{
			//Update and return the specified value
			if( animationType == AttributeAnimationType::Color || animationType == AttributeAnimationType::BlinkingColor )
				value.color_ = colorAnimation->Update( elapsedTime );
		}
	}
}

void AttributeAnimationFactory::Reload()
{
	for( const auto& p : cache )
		if( p.second )
			CreateFromXML( p.second, p.second->GetFilePath(), p.second->GetAnimationIndex() );
}

std::shared_ptr<AttributeAnimation> AttributeAnimationFactory::Create( const std::string& filePath, int index )
{
	std::string filePathKey = filePath + std::to_string( index );

	//Check if have animation on Cache
	auto it = cache.find( filePathKey );
	if( it != cache.end() )
		return (*it).second;

	//Create Attribute Animation
	auto attributeAnimation = std::make_shared<AttributeAnimation>( filePath, index );
	if( attributeAnimation )
	{
		if( !CreateFromXML( attributeAnimation, filePath, index ) )
			return nullptr;

		//Put it on cache
		cache[filePathKey] = attributeAnimation;

		return attributeAnimation;
	}

	return nullptr;
}

bool AttributeAnimationFactory::CreateFromXML( std::shared_ptr<AttributeAnimation> attributeAnimation, const std::string& filePath, int index )
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( filePath.c_str() );

	if( !result )
		return false;

	pugi::xml_node node = doc.child( "AttributeAnimation" );
	pugi::xml_node animation;

	bool found = false;
	int i = 0;

	for( pugi::xml_node animationNode : node.children( "Animation" ) )
	{
		if( i == index )
		{
			animation = animationNode;
			found = true;
			break;
		}
		i++;
	}

	//Animation index wasn't found
	if( !found )
		return false;

	std::string type = animation.attribute( "type" ).value();

	//Create Value Animation based on type
	if( type.compare( "Color" ) == 0 )
	{
		attributeAnimation->animationType = AttributeAnimationType::Color;
		attributeAnimation->colorAnimation = new Math::ValueAnimation<Math::Color>();
	}
	else if( type.compare( "BlinkingColor" ) == 0 )
	{
		attributeAnimation->animationType = AttributeAnimationType::BlinkingColor;
		attributeAnimation->colorAnimation = new Math::ValueAnimation<Math::Color>();
	}
	else if( type.compare( "Scrolling" ) == 0 )
	{
		attributeAnimation->animationType = AttributeAnimationType::ScrollUV;
		attributeAnimation->vector2Animation = new Math::ValueAnimation<Math::Vector2>();
	}

	//Iterate Key Frames and put it on Animation
	for( pugi::xml_node keyFrameNode : animation.children( "KeyFrame" ) )
	{
		float time = (float)atof( keyFrameNode.attribute( "time" ).value() );
		Math::EasingAnimation easing = Math::GetEasingFromString( keyFrameNode.attribute( "easing" ).value() );

		//Color Animation
		if( attributeAnimation->animationType == AttributeAnimationType::Color )
		{
			Math::Color color;
			sscanf_s( keyFrameNode.attribute( "value" ).value(), "%f %f %f %f", &color.r, &color.g, &color.b, &color.a );

			//Set Key Frame
			attributeAnimation->colorAnimation->SetKeyFrame( time, color, easing );
		}
		//Blinking Color
		else if( attributeAnimation->animationType == AttributeAnimationType::BlinkingColor )
		{
			float blinking;
			sscanf_s( keyFrameNode.attribute( "value" ).value(), "%f", &blinking );

			//Set Key Frame
			attributeAnimation->colorAnimation->SetKeyFrame( time, Math::Color( blinking, blinking, blinking, 1.0f ), easing );
		}
		//Scrolling Animation
		else if( attributeAnimation->animationType == AttributeAnimationType::ScrollUV )
		{
			Math::Vector2 vector;
			sscanf_s( keyFrameNode.attribute( "value" ).value(), "%f %f", &vector.x, &vector.y );

			//Set Key Frame
			attributeAnimation->vector2Animation->SetKeyFrame( time, vector, easing );
		}
	}

	attributeAnimation->loaded = true;

	return true;
}

}