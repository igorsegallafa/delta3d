#include "PrecompiledHeader.h"
#include "Material.h"

#include "Texture.h"

namespace Delta3D::Graphics
{

const std::vector<std::string> materialType = { "DIFFUSEMAP", "SELFILLUMINATIONMAP" };

bool Material::Prepare()
{
	//Prepare Material effect based on Current Scene
	if( renderer->Prepare( effect ) )
	{
		Math::Color finalDiffuseColor;
		Math::Vector2 finalDiffuseTranslation;

		//Material Settings
		if( customMaterial )
			finalDiffuseColor = diffuseColor;
		else
			finalDiffuseColor.a = diffuseColor.a;

		//Apply Attribute Animations for Material
		for( const auto& attributeAnimation : attributeAnimations )
		{
			if( attributeAnimation->GetType() == Resource::AttributeAnimationType::Color )
				finalDiffuseColor = attributeAnimation->GetValue().color_;
			else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::ScrollUV )
				finalDiffuseTranslation = attributeAnimation->GetValue().vector2_;
		}
		
		//Set Material for Shader
		if( effect )
		{
			effect->SetInt( "SelfIlluminationBlendingMode", selfIllumBlendingMode );
			effect->SetBool( "UseBlendingMaterial", useBlendingMaterial );
			effect->SetBool( "UseBlendingMap", useBlendingMap );
			effect->SetFloatArray( "DiffuseColor", &finalDiffuseColor.r, 4 );
			effect->SetFloatArray( "DiffuseScrolling", &finalDiffuseTranslation.x, 2 );
			effect->SetFloatArray( "AddColor", &addColor.r, 4 );
			effect->SetTechnique( "LitSolid" );

			//Check if Self Illumination Map has scrolling
			if( textureTransform[1] == TextureTransform::Scrolling || textureTransform[1] == TextureTransform::Scrolling2x || textureTransform[1] == TextureTransform::Scrolling4x )
				effect->SetBool( "ApplyScrollingSelfIllumination", true );
			else
				effect->SetBool( "ApplyScrollingSelfIllumination", false );

			//Has Self Illumination Map?
			if( textures.size() >= 2 )
				effect->SetFloat( "SelfIlluminationAmount", 1.f - (selfIlluminationAmount == 1.f ? 0.f : selfIlluminationAmount) );

			//Prepare Blending Material
			if( blendingMaterial && useBlendingMaterial )
				PrepareBlendingMaterial();

			//Commit Changes to Effect
			effect->CommitChanges();
		}

		//Apply Material to Device
		Apply();	

		return true;
	}

	return false;
}

void Material::PrepareBlendingMaterial()
{
	if( blendingMaterial == nullptr )
		return;

	if( blendingMaterial->attributeAnimations.empty() )
		return;

	Math::Color finalDiffuseColor( 0.0f, 0.0f, 0.0f, 0.0f );
	Math::Color finalBlinkingColor( 0.0f, 0.0f, 0.0f, 0.0f );
	Math::Vector2 finalDiffuseTranslation( 0.0f, 0.0f );

	//Apply Attribute Animations for Material
	for( const auto& attributeAnimation : blendingMaterial->attributeAnimations )
	{
		if( attributeAnimation->GetType() == Resource::AttributeAnimationType::Color )
			finalDiffuseColor = attributeAnimation->GetValue().color_;
		else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::BlinkingColor )
			finalBlinkingColor = attributeAnimation->GetValue().color_;
		else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::ScrollUV )
			finalDiffuseTranslation = attributeAnimation->GetValue().vector2_;
	}

	//Set to Effect
	if( effect )
	{
		effect->SetFloatArray( "OverlayColor", &finalDiffuseColor.r, 4 );
		effect->SetFloatArray( "OverlayBlinkingColor", &finalBlinkingColor.r, 4 );
		effect->SetFloatArray( "OverlayScrolling", &finalDiffuseTranslation.x, 2 );
	}
}

void Material::Apply()
{
	//Animated Texture?
	if( isAnimated && !animatedTextures.empty() )
	{
		//Loop
		if( animationFrame == 0x100 )
			animationFrame = 0;

		//Step texture frame
		animationFrame = (int)(16.0f* renderer->GetWorldTime()) % animatedTextures.size();

		//Set texture frame
		if( animationFrame >= 0 && animationFrame < (int)animatedTextures.size() )
			device->SetTexture( 0, animatedTextures[animationFrame]->Get() );
	}
	else
	{
		for( int i = 0; i < 8; i++ )
		{
			device->SetTexture( i, nullptr );

			//Set Texture
			if( (i < (int)textures.size()) && textures[i] )
				device->SetTexture( i, textures[i]->Get() );
		}

		//Set Blending Material for Device
		if( useBlendingMaterial && blendingMaterial )
			if( !blendingMaterial->textures.empty() )
				device->SetTexture( 2, blendingMaterial->textures[0]->Get() );
	}

	//Texture Blending
	graphics->StateBlockBegin( (StateBlock)(blendType + StateBlock::Blend_None) );

	//Two Sided
	if( twoSided )
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	else
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

bool Material::Build( FILE* file, bool skinned, bool loadTextures, bool useVertexColor, unsigned int mipMapsDefault, bool temporaryTextures, bool use3D )
{
	if( file )
	{
		unsigned int texturesCount = 0;
		unsigned int animTexturesCount = 0;

		auto ReadBooleanFile = [=]( bool& out ) { BOOL b; fread( &b, sizeof( BOOL ), 1, file ); out = b; };

		fread( &useCount, sizeof( DWORD ), 1, file );
		fread( &texturesCount, sizeof( int ), 1, file );
		fseek( file, 32, SEEK_CUR );
		fread( &textureStageState, sizeof( type ) + offsetof( Material, type ) - offsetof( Material, textureStageState ), 1, file );
		ReadBooleanFile( hasOpacityMap );
		ReadBooleanFile( isAnimated );
		fread( &blendType, sizeof( int ), 1, file );
		ReadBooleanFile( shading );
		ReadBooleanFile( twoSided );
		fread( &serialID, sizeof( selfIlluminationAmount ) + offsetof( Material, selfIlluminationAmount ) - offsetof( Material, serialID ), 1, file );
		fseek( file, 12, SEEK_CUR );
		fread( &colorTransform, sizeof( meshTransform ) + offsetof( Material, meshTransform ) - offsetof( Material, colorTransform ), 1, file );
		fseek( file, 128, SEEK_CUR );
		fread( &animTexturesCount, sizeof( int ), 1, file );
		fread( &frameTotal, sizeof( animationFrame ) + offsetof( Material, animationFrame ) - offsetof( Material, frameTotal ), 1, file );

		if( loadTextures == true )
		{	
			std::vector<std::string> attributeAnimationsFile;
			std::vector<ShaderDefine> defines;

			//Fix Transparency
			diffuseColor.a = 1.0f - diffuseColor.a;

			if( useCount )
			{
				//Read Static and Animated Textures
				if( texturesCount != -1 && animTexturesCount != -1 )
				{
					int  stringLength = 0;
					char materialName[4096] = { 0 };

					//Read String Name and Length
					fread( &stringLength, sizeof( int ), 1, file );
					fread_s( materialName, sizeof( materialName ), stringLength, 1, file );

					char* curMaterialName = materialName;

					//Read Textures from Material
					for( unsigned int j = 0; j < texturesCount; j++ )
					{
						//Blending Mode
						if( textureStageState[j] == D3DTOP_ADD )
							selfIllumBlendingMode = 1;

						//Texture Name
						char* name = curMaterialName;
						curMaterialName += strlen( name ) + 1;
						curMaterialName += strlen( curMaterialName ) + 1;

						//Add Texture to Texture Handler
						auto texture = graphics->GetTextureFactory()->Create( name, false, false, mipMapsDefault, temporaryTextures, use3D ? graphics->reduceQualityTexture : 0 );

						if( texture )
						{
							if( j >= 0 && j < materialType.size() )
								defines.push_back( ShaderDefine{ materialType[j].c_str(), "1" } );

							textures.push_back( texture );
						}
					}

					//Read Animated Textures from Material
					for( unsigned int j = 0; j < animTexturesCount; j++ )
					{
						//Texture Name
						char* name = curMaterialName;
						curMaterialName += strlen( name ) + 1;
						curMaterialName += strlen( curMaterialName ) + 1;

						//Add Texture to Texture Handler
						auto texture = graphics->GetTextureFactory()->Create( name, false, false, mipMapsDefault, temporaryTextures, use3D ? graphics->reduceQualityTexture : 0 );

						if( texture )
							animatedTextures.push_back( texture );

						isAnimated = true;
					}
				}
			}

			//Texture Transform
			for( int i = 0; i < _countof( textureTransform ); i++ )
			{
				//Ignore Texture Transform Reflex
				if( textureTransform[i] == TextureTransform::Reflex )
					continue;

				if( textureTransform[i] == TextureTransform::Scrolling )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll1.xml" );
				else if( textureTransform[i] == TextureTransform::Scrolling2x )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll2.xml" );
				else if( textureTransform[i] == TextureTransform::Scrolling4x )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll4.xml" );
				else if( textureTransform[i] >= TextureTransform::Scrolling )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll2.xml" );
			}

			//Water Material
			if( meshTransform& MeshTransform::Water )
				attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll1.xml" );

			//Load Attribute Animations File
			for( auto file : attributeAnimationsFile )
			{
				int index = 0;
				while( true )
				{
					auto animation = graphics->GetAttributeAnimationFactory()->Create( file, index );

					if( animation )
					{
						if( animation->GetType() == Resource::AttributeAnimationType::ScrollUV )
							defines.push_back( ShaderDefine{ "SCROLLUV", "1" } );

						attributeAnimations.push_back( animation );
					}
					else
						break;

					index++;
				}
			}

			//Prepare Effect Defines
			if( skinned && !graphics->useSoftwareSkinning )
				defines.push_back( ShaderDefine{ "SKINNED", "1" } );

			//Vertex Color Define
			if( useVertexColor )
				defines.push_back( ShaderDefine{ "VERTEXCOLOR", "1" } );

			//Supports Pixel Shader 3.0
			if( graphics->pixelShaderVersionMajor == 3 )
				defines.push_back( ShaderDefine{ "_PS_3_0", "1" } );

			//Create Effect
			effect = graphics->GetShaderFactory()->Create( "game\\scripts\\shaders\\LitSolid.fx", defines );
		}
		else
		{
			unsigned int offset = 0;
			fread( &offset, sizeof( int ), 1, file );
			fseek( file, offset, SEEK_CUR );
		}

		return true;
	}

	return false;
}

bool Material::Load( const std::string& filePath, bool defaultSettings, bool use3D )
{
	//Set Default Settings for material
	if( defaultSettings )
	{
		useCount = 1;
		shading = false;
		twoSided = false;
		type = 0;
		textureStageState[0] = 0;
		textureTransform[0] = TextureTransform::None;
		hasOpacityMap = false;
		diffuseColor.a = 0;
		colorTransform = 0;
		isAnimated = false;
		blendType = StateBlock::Blend_None;
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( filePath.c_str() );

	if( !result )
		return false;

	pugi::xml_node material = doc.child( "Material" );
	customMaterial = true;

	std::vector<std::string> definesString;

	//Read Effects from XML and prepare Defines structure
	if( !material.child( "Effect" ).attribute( "defines" ).empty() )
	{
		std::istringstream buffer( material.child( "Effect" ).attribute( "defines" ).value() );
		definesString = std::vector<std::string>{ std::istream_iterator<std::string>( buffer ), std::istream_iterator<std::string>() };
	}

	//Read Textures from XML
	for( pugi::xml_node textureNode : material.children( "Texture" ) )
	{
		if( textureNode.attribute( "blendMode" ) )
		{
			if( _strcmpi( textureNode.attribute( "blendMode" ).value(), "Add" ) == 0 )
				selfIllumBlendingMode = 1;
		}

		//Add Texture to Texture Handler
		auto texture = graphics->GetTextureFactory()->Create( textureNode.attribute("file").value(), false, false, 3, false, use3D ? graphics->reduceQualityTexture : 0 );

		if( texture )
			textures.push_back( texture );
	}

	//Read Animated Textures from XML
	for( pugi::xml_node textureNode : material.children( "AnimatedTexture" ) )
	{
		int count = atoi( textureNode.attribute( "count" ).value() );

		if( count > 0 )
		{
			for( int i = 0; i < count; i++ )
			{
				char fileName[256] = { 0 };
				sprintf_s( fileName, textureNode.attribute( "file" ).value(), i );

				//Add Texture to Texture Handler
				auto texture = graphics->GetTextureFactory()->Create( fileName, false, false, 3, false, use3D ? graphics->reduceQualityTexture : 0 );

				if( texture )
					animatedTextures.push_back( texture );
			}

			isAnimated = true;
		}
	}

	//Read Attributes
	for( pugi::xml_node attributeNode : material.children( "Attribute" ) )
	{
		//Diffuse Color
		if( _strcmpi( attributeNode.attribute( "type" ).value(), "DiffuseColor" ) == 0 )
			sscanf_s( attributeNode.attribute( "value" ).value(), "%f %f %f %f", &diffuseColor.r, &diffuseColor.g, &diffuseColor.b, &diffuseColor.a );
	}

	//Read Attribute Animations from File
	for( pugi::xml_node attributeAnimationNode : material.children( "AttributeAnimation" ) )
	{
		std::string animationFilePath = attributeAnimationNode.attribute( "file" ).value();

		if( !animationFilePath.empty() )
		{
			int index = 0;
			while( true )
			{
				auto animation = graphics->GetAttributeAnimationFactory()->Create( animationFilePath, index );

				if( animation )
				{
					if( animation->GetType() == Resource::AttributeAnimationType::ScrollUV )
						definesString.push_back( "SCROLLUV" );

					attributeAnimations.push_back( animation );
				}
				else
					break;

				index++;
			}
		}
	}

	//Prepare Effect Defines
	std::vector<ShaderDefine> defines;

	for( const auto& str : definesString )
		defines.push_back( ShaderDefine{ str.c_str(), "1" } );

	//Create Effect
	if( material.child( "Effect" ) )
		effect = graphics->GetShaderFactory()->Create( material.child( "Effect" ).attribute( "file" ).value(), defines );

	return true;
}

void Material::SetBlendingMaterial( Material* material, bool forceUseBlendingMap )
{
	blendingMaterial = material;
	useBlendingMap = forceUseBlendingMap;

	if( material == nullptr )
		useBlendingMaterial = false;
	else
		useBlendingMaterial = true;
}

Material* Material::Clone( Material* other )
{
	memcpy( this, other, sizeof( animationFrame ) + offsetof( Material, animationFrame ) - offsetof( Material, useCount ) );
	customMaterial = true;

	return this;
}
}