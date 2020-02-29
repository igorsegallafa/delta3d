#pragma once

#include "../Math/Color.h"

#include "Graphics.h"

namespace Delta3D::Graphics
{
class Texture : public std::enable_shared_from_this<Texture>
{
friend class TextureFactory;
public:
	//! Default Constructor for Texture.
	Texture( IDirect3DTexture9* texture_ );

	//! Constructor for Texture with a Texture File Path specified.
	Texture( IDirect3DTexture9* texture_, std::string filePath_, bool colorKey_ = true );

	//! Deconstructor.
	~Texture();

	//! Release Texture.
	void Release();

	//! Set a new Texture Object.
	void Renew( IDirect3DTexture9* texture_ );

	//! Get Image Texture Object.
	IDirect3DTexture9* Get() const { return texture; }

	//! Get File Path from Image.
	const std::string& FilePath() const { return filePath; }

	//! Get if texture uses Color Key.
	const bool UseColorKey() const { return colorKey; }

	//! Get Texture Width.
	const int Width() const { return info.width; }

	//! Get Texture Height.
	const int Height() const { return info.height; }

	//! Get Texture Format.
	const D3DFORMAT& Format() const { return info.format; }

	//! Lock Texture to update Pixels.
	const bool Lock();

	//! Unlock Texture.
	void Unlock();

	//! Set Pixel Data.
	void SetPixelData( void* data, unsigned int size, int offset = 0 );

	//! Get Image Surface Info.
	const SurfaceInfo& Info() const { return info; }
public:
	static std::shared_ptr<Texture> Default;	//!< Default Texture
private:
	//! Update Surfaces Descriptions.
	void UpdateSurfaceDesc();
private:
	IDirect3DTexture9* texture;	//!< Texture Object
	std::string filePath;	//!< Texture File Path
	SurfaceInfo info;	//!< Texture Info
	bool colorKey;	//!< Is Using Color Key
	bool isLocked;	//!< Flag to determinate if texture is locked
	D3DLOCKED_RECT lockedRect;	//!< Texture Locked Rectangle
};

class TextureFactory
{
public:
	//! Default Constructor for Texture Factory.
	TextureFactory( Graphics* graphics_ );

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Reload Textures.
	void Reload();

	//! Clear Temporary Cache.
	void Clear() { temporaryCache.clear(); }

	/**
	 * Create a specified Texture from File Path
	 * @param filePath File Path from Texture
	 * @param temporary Boolean to determinate if texture will be temporary (is not added for cache)
	 * @return Pointer to Texture created
	 */
	std::shared_ptr<Texture> Create( const std::string& filePath, const bool temporary = false, const bool useColorKey = true, int defaultMipLevels = 0, bool useTemporaryCache = false, unsigned int reduceQualityLevel = 0 );

	/**
	 * Create a specified Texture from Texture Object
	 * @param texture Texture Object
	 * @return Pointer to Texture created
	 */
	std::shared_ptr<Texture> Create( IDirect3DTexture9* texture );

	/**
	 * Create Dynamic Texture
	 * @param width Width of Texture
	 * @param height Height of Texture
	 * @return Pointer to Texture created
	 */
	std::shared_ptr<Texture> CreateDynamicTexture( int width, int height );

	/**
	 * Create a Blank Texture
	 * @return Pointer to Texture created
	 */
	std::shared_ptr<Texture> CreateSolidColor();

	/**
	 * Create Texture Object from specified File
	 * @param filePath File Path from Texture
	 * @return Texture Object
	 */
	IDirect3DTexture9* CreateTextureFromFile( const std::string& filePath, const bool useColorKey, int defaultMipLevels = 0, unsigned int reduceQualityLevel = 0 );
private:
	/**
	 * Create Texture From File Buffer
	 * @param hr Error Handling
	 * @param buffer Buffer from Texture
	 * @param size Size from Buffer
	 * @param mipLevels Mip Map Level
	 * @param colorKey Color Key
	 * @param reduceQualityLevel Reduce Texture Size
	 * @return Texture Object
	 */
	IDirect3DTexture9* CreateTextureFromFileInMemory( HRESULT& hr, const char* buffer, unsigned int size, int mipLevels, Math::Color colorKey, unsigned int reduceQualityLevel );

	/**
	 * Create a Dynamic Texture
	 * @param width Width of Texture
	 * @param height Height of Texture
	 * @param format Format of Texture
	 *@return Texture Object
	 */
	IDirect3DTexture9* CreateDynamicTexture( int width, int height, D3DFORMAT format );

	//! Decrypt BMP Image.
	bool DecryptBMP( char* buffer, unsigned int size );

	//! Decrypt TGA Image.
	bool DecryptTGA( char* buffer, unsigned int size );
private:
	std::unordered_map<std::string,std::shared_ptr<Texture>> cache;	//!< Cache of Texture's
	std::unordered_map<std::string,std::shared_ptr<Texture>> temporaryCache;	//!< Cache for Temporary Texture's
	std::vector<std::shared_ptr<Texture>> dynamicTextures;	//!< Dynamic Textures

	Graphics* graphics;	//!< Graphics Pointer
};

}