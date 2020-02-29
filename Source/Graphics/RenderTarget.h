#pragma once

#include "Graphics.h"

namespace Delta3D::Graphics
{

class RenderTarget : GraphicsImpl, public std::enable_shared_from_this<RenderTarget>
{
friend class RenderTargetFactory;
public:
	//! Default Constructor for Render Target.
	RenderTarget( bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		texture( nullptr ), 
		surface( nullptr ), 
		width( 0 ), 
		height ( 0 ), 
		sizeShiftRight( 0 )
	{
	}

	//! Constructor for Render Target with a Size Shifted.
	RenderTarget( const unsigned int sizeShiftRight_, bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		texture( nullptr ), 
		surface( nullptr ), 
		width( 0 ), 
		height( 0 ), 
		sizeShiftRight( sizeShiftRight_ )
	{
	}

	//! Constructor for Render Target with a Width and Height specified.
	RenderTarget( const unsigned int width_, const unsigned int height_, bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		texture( nullptr ), 
		surface( nullptr ), 
		width( width_ ), 
		height( height_ ), 
		sizeShiftRight( 0 )
	{
	}

	//! Deconstructor.
	~RenderTarget();

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Return boolean if render target is sharable.
	const bool IsSharable() const { return sharable; }

	//! Get Render Target Texture.
	IDirect3DTexture9* Texture() const { return texture; }

	//! Get Render Target Surface.
	IDirect3DSurface9* Surface() const { return surface; }

	//! Get Render Target Width.
	const unsigned int Width();

	//! Get Render Target Height.
	const unsigned int Height();

	//! Get Render Target Absolute Width.
	const unsigned int AbsWidth() const{ return width; }

	//! Get Render Target Absolute Height.
	const unsigned int AbsHeight() const{ return height; }

	//! Get Render Target Size Shift Right.
	const unsigned int SizeShiftRight();
private:
	bool sharable;	//!< Sharable boolean

	IDirect3DTexture9* texture;	//!< Render Target Texture
	IDirect3DSurface9* surface;	//!< Render Target Surface

	unsigned int width;	//!< Render Target Width
	unsigned int height;	//!< Render Target Height

	unsigned int sizeShiftRight;
};

class RenderTargetFactory
{
public:
	//! Default Constructor for Render Target Factory.
	RenderTargetFactory( Graphics* graphics_ ) : graphics( graphics_ ) {}

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	/**
	 * Create a specified Render Target with size of Window
	 * @param sharable Boolean to set if Render Target will be sharable or not
	 * @return Pointer to Render Target created
	 */
	std::shared_ptr<RenderTarget> Create( bool sharable = true );

	/**
	 * Create a specified Render Target with shifted size of Window
	 * @param sizeShiftRight Value to get shifted window size
	 * @param sharable Boolean to set if Render Target will be sharable or not
	 * @return Pointer to Render Target created
	 */
	std::shared_ptr<RenderTarget> Create( const unsigned int sizeShiftRight, bool sharable = true );

	/**
	 * Create a specified Render Target
	 * @param width Width of Render Target
	 * @param height Height of Render Target
	 * @param sharable Boolean to set if Render Target will be sharable or not
	 * @return Pointer to Render Target created
	 */
	std::shared_ptr<RenderTarget> Create( const unsigned int width, const unsigned int height, bool sharable = true );

	/**
	 * Build a specified Render Target
	 * This function will create texture and surface for render target
	 * @param renderTarget Pointer to the Render Target that will be Builded
	 * @return Boolean if was builded successfully
	 */
	bool Build( std::shared_ptr<RenderTarget> renderTarget );
private:
	std::vector<std::shared_ptr<RenderTarget>> cache;	//!< Cache of Render Target's

	Graphics* graphics;	//!< Graphics Pointer
};

}