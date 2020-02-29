#include "PrecompiledHeader.h"
#include "Texture.h"

namespace Delta3D::Graphics
{

std::shared_ptr<Texture> Texture::Default = nullptr;

Texture::Texture( IDirect3DTexture9* texture_ ) : 
	texture( texture_ ), 
	isLocked( false )
{
	if( texture )
	{
		texture->AddRef();
		UpdateSurfaceDesc();
	}
}

Texture::Texture( IDirect3DTexture9* texture_, std::string filePath_, bool colorKey_ ) : 
	texture( texture_ ), 
	filePath( filePath_ ), 
	colorKey( colorKey_ ), 
	isLocked( false )
{
	if( texture )
	{
		texture->AddRef();
		UpdateSurfaceDesc();
	}
}

Texture::~Texture()
{
	if( texture )
	{
		texture->Release();
		texture = nullptr;
	}
}

void Texture::Release()
{
	if( texture )
	{
		texture->Release();
		texture = nullptr;
	}
}

void Texture::Renew( IDirect3DTexture9* texture_ )
{
	//Release old Texture
	if( texture )
	{
		texture->Release();
		texture = nullptr;
	}

	//Set and load new texture
	texture = texture_;
	if( texture )
	{
		texture->AddRef();
		UpdateSurfaceDesc();
	}
}

const bool Texture::Lock()
{
	if( isLocked )
		return false;

	if( !texture )
		return false;

	if( SUCCEEDED( texture->LockRect( 0, &lockedRect, nullptr, D3DLOCK_DISCARD ) ) )
		isLocked = true;

	return true;
}

void Texture::Unlock()
{
	if( !texture )
		return;

	if( isLocked )
	{
		texture->UnlockRect( 0 );
		isLocked = false;
	}
}

void Texture::SetPixelData( void* data, unsigned int size, int offset )
{
	if( isLocked )
		memcpy( (void*)((DWORD)lockedRect.pBits + offset), data, size );
}

void Texture::UpdateSurfaceDesc()
{
	if( texture )
	{
		D3DSURFACE_DESC surfaceDesc;
		if( SUCCEEDED( texture->GetLevelDesc( 0, &surfaceDesc ) ) )
		{
			info.format = surfaceDesc.Format;
			info.width = surfaceDesc.Width;
			info.height = surfaceDesc.Height;
		}
	}
}

TextureFactory::TextureFactory( Graphics* graphics_ ) : graphics( graphics_ )
{
}

void TextureFactory::OnLostDevice()
{
	for( auto& texture : dynamicTextures )
	{
		texture->Release();
	}
}

void TextureFactory::OnResetDevice()
{
	for( auto& texture : dynamicTextures )
	{
		//Create Texture
		IDirect3DTexture9* d3dtexture = CreateDynamicTexture( texture->Width(), texture->Height(), texture->Format() );

		//Renew Texture (adds Reference)
		texture->Renew( d3dtexture );

		//Release our Reference
		if( d3dtexture )
			d3dtexture->Release();
	}
}

void TextureFactory::Reload()
{
	for( auto& texture : cache )
	{
		//Create Texture
		IDirect3DTexture9* d3dtexture = CreateTextureFromFile( texture.second->FilePath(), texture.second->UseColorKey() );

		//Renew Texture (adds Reference)
		texture.second->Renew( d3dtexture );

		//Release our Reference
		if( d3dtexture )
			d3dtexture->Release();
	}
}

std::shared_ptr<Texture> TextureFactory::Create( const std::string& filePath, const bool temporary, const bool useColorKey, int defaultMipLevels, bool useTemporaryCache, unsigned int reduceQualityLevel )
{
	//Verify if texture is already on cache
	if( !temporary && cache.find( filePath ) != cache.end() )
		return cache[filePath];
	else if( useTemporaryCache && temporaryCache.find( filePath ) != temporaryCache.end() )
		return temporaryCache[filePath];

	//Create Texture Object
	IDirect3DTexture9* d3dtexture = CreateTextureFromFile( filePath, useColorKey, defaultMipLevels, reduceQualityLevel );

	//Created Object successfully?
	if( d3dtexture )
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>( d3dtexture, filePath );
		d3dtexture->Release();

		//Put it on Cache
		if( !temporary && texture )
			cache[filePath] = texture;
		else if( useTemporaryCache && texture )
			temporaryCache[filePath] = texture;

		return texture;
	}

	return nullptr;
}

std::shared_ptr<Texture> TextureFactory::Create( IDirect3DTexture9* texture )
{
	return std::make_shared<Texture>( texture );
}

std::shared_ptr<Texture> TextureFactory::CreateDynamicTexture( int width, int height )
{
	//Look for dynamic texture on cache
	for( auto& texture : dynamicTextures )
		if( texture->Width() == width && texture->Height() == height )
			return texture;

	//Create Texture Object
	IDirect3DTexture9* d3dtexture = CreateDynamicTexture( width, height, D3DFMT_A32B32G32R32F );

	//Created Object successfully?
	if( d3dtexture )
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>( d3dtexture );
		d3dtexture->Release();

		dynamicTextures.push_back( texture );

		return texture;
	}

	return nullptr;
}

std::shared_ptr<Texture> TextureFactory::CreateSolidColor()
{
	return Create( "game\\textures\\misc\\white.bmp", false, false );
}

IDirect3DTexture9* TextureFactory::CreateTextureFromFile( const std::string& filePath, const bool useColorKey, int defaultMipLevels, unsigned int reduceQualityLevel )
{
	filesystem::path fs( filePath );

	//Get File Extension
	if( filesystem::exists( fs ) && filesystem::is_regular_file( fs ) && fs.has_extension() )
	{
		const std::string ext = fs.extension().string();
		std::fstream file( filePath, std::fstream::in | std::fstream::binary );

		if( file.is_open() )
		{
			file.seekg( 0, std::ios::end );
			std::streamoff fileSize = file.tellg();
			file.seekg( 0, std::ios::beg );

			if( fileSize > 0 )
			{
				char* fileBuffer = new char[(unsigned int)fileSize];
				file.read( fileBuffer, fileSize );

				//Can Load texture
				bool loadTexture = true;

				//Color Key
				Math::Color colorKey( 0.0f, 0.0f, 0.0f, 0.0f );

				//Decrypt Images Firstly
				if( _strnicmp( ext.c_str(), ".bmp", ext.length() ) == 0 )
				{
					loadTexture = DecryptBMP( fileBuffer, (unsigned int)fileSize );
					colorKey = Math::Color( 0.0f, 0.0f, 0.0f, 1.0f );
				}
				else if( _strnicmp( ext.c_str(), ".tga", ext.length() ) == 0 )
					loadTexture = DecryptTGA( fileBuffer, (unsigned int)fileSize );

				//Create Texture
				if( loadTexture )
				{
					int mipLevels = defaultMipLevels;

					//Look for Mip Level of Texture
					std::string mipMapFilePath = filePath.substr( 0, filePath.length() - 3 ) + "mip";
					std::fstream mipMapFile( mipMapFilePath, std::fstream::in | std::fstream::binary );

					if( mipMapFile.is_open() )
					{
						mipMapFile.read( (char*)&mipLevels, sizeof( int ) );
						mipMapFile.close();
					}

					HRESULT hr;
					IDirect3DTexture9* texture = CreateTextureFromFileInMemory( hr, fileBuffer, (unsigned int)fileSize, mipLevels == 0 ? D3DX_FROM_FILE : mipLevels, useColorKey ? colorKey : Math::Color( 0.0f, 0.0f ,0.0f, 0.0f ), reduceQualityLevel );

					//Error
					if( !texture || FAILED( hr ) )
						DELTA3D_LOGERROR( "Could not Create Texture from File (%s) [%08X]", filePath.c_str(), hr );

					//Delete File Buffer
					delete[] fileBuffer;

					return texture;
				}

				//Delete File Buffer
				delete[] fileBuffer;
			}
		}
	}

	return nullptr;
}

IDirect3DTexture9* TextureFactory::CreateTextureFromFileInMemory( HRESULT& hr, const char* buffer, unsigned int size, int mipLevels, Math::Color colorKey, unsigned int reduceQualityLevel )
{
	unsigned int width = D3DX_DEFAULT_NONPOW2;
	unsigned int height = D3DX_DEFAULT_NONPOW2;

	D3DXIMAGE_INFO ii = { 0 };

	//Reduce Texture Quality
	if( reduceQualityLevel > 0 )
	{
		hr = D3DXGetImageInfoFromFileInMemory( buffer, size, &ii );

		if( FAILED( hr ) )
			return nullptr;

		width = ii.Width >> reduceQualityLevel;
		height = ii.Height >> reduceQualityLevel;
	}

	//Create Texture
	IDirect3DTexture9* texture = nullptr;

	hr = D3DXCreateTextureFromFileInMemoryEx( graphics->GetDevice(), buffer, size, width, height, mipLevels, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT,
		D3DX_DEFAULT, colorKey.ToUInt(), &ii, NULL, &texture );

	//Error
	if( FAILED( hr ) )
		return nullptr;

	return texture;
}

IDirect3DTexture9* TextureFactory::CreateDynamicTexture( int width, int height, D3DFORMAT format )
{
	IDirect3DTexture9* d3dtexture = nullptr;

	//Create Texture Object
	HRESULT hr = graphics->GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_DYNAMIC, format, D3DPOOL_DEFAULT, &d3dtexture, NULL );

	//Created Object successfully?
	if( d3dtexture && SUCCEEDED( hr ) )
		return d3dtexture;
	else
		DELTA3D_LOGERROR( "Could not Create Dynamic Texture[%08X]", hr );

	return nullptr;
}

bool TextureFactory::DecryptBMP( char* buffer, unsigned int size )
{
	if( size < 14 )
		return false;

	if( (buffer[0] != 'B') || (buffer[1] != 'M') )
	{
		buffer[0] = 'B';
		buffer[1] = 'M';

		for( unsigned char c = 2; c < 14; c++ )
			buffer[c] -= ( c* c );
	}

	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = 0;

	return true;
}

bool TextureFactory::DecryptTGA( char* buffer, unsigned int size )
{
	if( size < 18 )
		return false;

	if( (buffer[0] == 'G') && (buffer[1] == '8') )
	{
		buffer[0] = 0;
		buffer[1] = 0;

		for( unsigned char c = 2; c < 18; c++ )
			buffer[c] -= ( c* c );
	}

	return true;
}

}