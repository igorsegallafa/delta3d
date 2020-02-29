#include "PrecompiledHeader.h"
#include "MaterialCollection.h"

#include "Material.h"
#include "Texture.h"

namespace Delta3D::Graphics
{
MaterialCollection::MaterialCollection() : 
	GraphicsImpl(), 
	header( -1 ), 
	materials( nullptr ),
	materialsCount( 0 ), 
	materialType( 0 ), 
	name()
{
}

MaterialCollection::MaterialCollection( const std::string& name_ ) : 
	GraphicsImpl(), 
	header( -1 ), 
	materials( nullptr ), 
	materialsCount( 0 ), 
	materialType( 0 ), 
	name( name_ )
{
}

MaterialCollection::MaterialCollection( int materialsCount_ ) : 
	GraphicsImpl(), 
	header( -1 ), 
	materialsCount( 0 ), 
	materialType( 0 ), 
	name()
{
	materials = new Material[materialsCount_];
	memset( materials, 0, sizeof( Material )* materialsCount_ );
}

MaterialCollection::~MaterialCollection()
{
	if( materials )
	{
		delete[] materials;
		materials = nullptr;
	}
}

int MaterialCollection::CreateMaterial( std::string textureFilePath, bool twoSided, bool shading, int blendType, bool useColorKey )
{
	if( materials == nullptr )
		return -1;

	Material* material = &materials[materialsCount];

	auto texture = graphics->GetTextureFactory()->Create( textureFilePath, false, useColorKey );

	if( texture )
		material->textures.push_back( texture );

	material->useCount = 1;
	material->shading = shading;
	material->twoSided = twoSided;
	material->type = 0;
	material->textureStageState[0] = 0;
	material->textureTransform[0] = Material::TextureTransform::None;
	material->hasOpacityMap = false;
	material->diffuseColor.a = 0;
	material->colorTransform = 0;
	material->isAnimated = false;
	material->blendType = blendType;
	material->useCount++;

	return materialsCount++;
}

void MaterialCollection::AddTexture( int materialID, std::string textureFilePath )
{
	auto texture = graphics->GetTextureFactory()->Create( textureFilePath, false, false );

	if( texture )
		if( materials )
			materials[materialID].textures.push_back( texture );
}

void MaterialCollection::AddAnimatedTexture( Material* material, std::vector<std::string> textureList, int blendType, bool autoAnimate )
{
	for( const auto& textureName : textureList )
		material->animatedTextures.push_back( graphics->GetTextureFactory()->Create( textureName, false, false ) );

	material->hasOpacityMap = FALSE;
	material->diffuseColor.a = 0;
	material->frameTotal = textureList.size() - 1;
	material->frameSpeed = 6;
	material->colorTransform = 0;
	material->isAnimated = true;
	material->blendType = blendType;

	if( autoAnimate )
		material->animationFrame = 0x100;
	else
		material->animationFrame = 0;

	material->useCount++;
}

int MaterialCollection::SetFrame( Material* material, int frame )
{
	int retValue = material->animationFrame;
	material->animationFrame = frame;
	return retValue;
}

Material* MaterialCollection::GetMaterialByTexture( std::string textureName )
{
	if( materials )
	{
		for( int i = 0; i < materialsCount; i++ )
		{
			Material* material = materials + i;

			if( material->useCount )
			{
				//Looking for normal textures first
				for( auto& texture : material->textures )
					if( _strcmpi( texture->FilePath().c_str(), textureName.c_str() ) == 0 )
						return material;

				//Looking for animated textures
				for( auto& animTexture : material->animatedTextures )
					if( _strcmpi( animTexture->FilePath().c_str(), textureName.c_str() ) == 0 )
						return material;
			}
		}
	}

	return nullptr;
}

void MaterialCollection::SetBlendingMaterial( Material* material, bool useBlendingMap )
{
	if( materials )
		for( int i = 0; i < materialsCount; i++ )
			materials[i].SetBlendingMaterial( material, useBlendingMap );
}

bool MaterialCollection::Build( FILE* file, bool skinned, bool loadTextures, bool useVertexColor, unsigned int mipMapsDefault, bool temporaryTextures, bool use3D )
{
	if( file )
	{
		fread( &header, sizeof( materialType ) + offsetof( MaterialCollection, materialType ) - offsetof( MaterialCollection, header ), 1, file );
		fseek( file, 72, SEEK_CUR );

		//Allocate Materials
		materials = new Material[materialsCount];

		//Read Each Material
		for( int i = 0; i < materialsCount; i++ )
			materials[i].Build( file, skinned, loadTextures, useVertexColor, mipMapsDefault, temporaryTextures, use3D );

		return true;
	}

	return false;
}

bool MaterialCollection::Load( const std::string& filePath, bool use3D )
{
	filesystem::path p( filePath );

	if( p.has_filename() )
	{
		std::ifstream file( filePath );

		if( file.is_open() )
		{
			std::string line;
			unsigned int materialID = 0;

			while( std::getline( file, line ) )
			{
				//Check if this material exists on XML Format
				if( filesystem::exists( line ) && filesystem::is_regular_file( line ) )
					if( line.find( "NoTexture") == std::string::npos )
						if( materials )
							materials[materialID].Load( line, use3D );

				materialID++;
			}

			file.close();

			return true;
		}
	}

	return false;
}

}