#pragma once

#include "Graphics.h"

namespace Delta3D::Graphics
{

class DepthStencilBuffer : GraphicsImpl, public std::enable_shared_from_this<DepthStencilBuffer>
{
friend class DepthStencilBufferFactory;
public:
	//! Default Constructor for Depth Stencil.
	DepthStencilBuffer( bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		surface( nullptr ), 
		width( 0 ), 
		height ( 0 ),
		sizeShiftRight( 0 )
	{
	}

	//! Constructor for Depth Stencil with a Size Shifted.
	DepthStencilBuffer( const unsigned int sizeShiftRight_, bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		surface( nullptr ), 
		width( 0 ), 
		height( 0 ), 
		sizeShiftRight( sizeShiftRight_ )
	{
	}

	//! Constructor for Depth Stencil with a Width and Height specified.
	DepthStencilBuffer( const unsigned int width_, const unsigned int height_, bool sharable_ ) : 
		GraphicsImpl::GraphicsImpl(), 
		sharable( sharable_ ), 
		surface( nullptr ), 
		width( width_ ), 
		height( height_ ), 
		sizeShiftRight( 0 )
	{
	}

	//! Deconstructor.
	~DepthStencilBuffer();

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Return boolean if Depth Stencil is sharable.
	const bool IsSharable() const { return sharable; }

	//! Get Depth Stencil Surface.
	IDirect3DSurface9* Surface() const { return surface; }

	//! Get Depth Stencil Width.
	const unsigned int Width();

	//! Get Depth Stencil Height.
	const unsigned int Height();

	//! Get Depth Stencil Absolute Width.
	const unsigned int AbsWidth() const{ return width; }

	//! Get Depth Stencil Absolute Height.
	const unsigned int AbsHeight() const{ return height; }

	//! Get Depth Stencil Size Shift Right.
	const unsigned int SizeShiftRight();
private:
	bool sharable;	//!< Sharable boolean

	IDirect3DSurface9* surface;	//!< Depth Stencil Surface

	unsigned int width;	//!< Depth Stencil Width
	unsigned int height;	//!< Depth Stencil Height

	unsigned int sizeShiftRight;
};

class DepthStencilBufferFactory
{
public:
	//! Default Constructor for Depth Stencil Factory.
	DepthStencilBufferFactory( Graphics* graphics_ ) : graphics( graphics_ ) {}

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	/**
	 * Create a specified Depth Stencil with size of Window
	 * @return Pointer to Depth Stencil created
	 */
	std::shared_ptr<DepthStencilBuffer> Create( bool sharable = true );

	/**
	 * Create a specified Depth Stencil with shifted size of Window
	 * @param sizeShiftRight Value to get shifted window size
	 * @param sharable Boolean to set if Depth Stencil will be sharable or not
	 * @return Pointer to Depth Stencil created
	 */
	std::shared_ptr<DepthStencilBuffer> Create( const unsigned int sizeShiftRight, bool sharable = true );

	/**
	 * Create a specified Depth Stencil
	 * @param width Width of Depth Stencil
	 * @param height Height of Depth Stencil
	 * @param sharable Boolean to set if Depth Stencil will be sharable or not
	 * @return Pointer to Depth Stencil created
	 */
	std::shared_ptr<DepthStencilBuffer> Create( const unsigned int width, const unsigned int height, bool sharable = true );

	/**
	 * Build a specified Depth Stencil
	 * This function will create texture and surface for Depth Stencil
	 * @param depthStencil Pointer to the Depth Stencil that will be Builded
	 * @return Boolean if was builded successfully
	 */
	bool Build( std::shared_ptr<DepthStencilBuffer> depthStencilBuffer );
private:
	std::vector<std::shared_ptr<DepthStencilBuffer>> cache;	//!< Cache of Depth Stencil Buffer's

	Graphics* graphics;	//!< Graphics Pointer
};

}