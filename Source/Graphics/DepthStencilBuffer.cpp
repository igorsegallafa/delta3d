#include "PrecompiledHeader.h"
#include "DepthStencilBuffer.h"

namespace Delta3D::Graphics
{

DepthStencilBuffer::~DepthStencilBuffer()
{
	if( surface )
	{
		surface->Release();
		surface = nullptr;
	}
}

void DepthStencilBuffer::OnLostDevice()
{
	if( surface )
	{
		surface->Release();
		surface = nullptr;
	}
}

void DepthStencilBuffer::OnResetDevice()
{
	graphics->GetDepthStencilBufferFactory()->Build( shared_from_this() );
}

const unsigned int DepthStencilBuffer::Width()
{
	return width ? width : graphics->GetBackBufferInfo().width >> sizeShiftRight;
}

const unsigned int DepthStencilBuffer::Height()
{
	return height ? height : graphics->GetBackBufferInfo().height >> sizeShiftRight;
}

const unsigned int DepthStencilBuffer::SizeShiftRight()
{
	return sizeShiftRight;
}

void DepthStencilBufferFactory::OnLostDevice()
{
	for( const auto& depthStencilBuffer : cache )
		depthStencilBuffer->OnLostDevice();
}

void DepthStencilBufferFactory::OnResetDevice()
{
	for( const auto& depthStencilBuffer : cache )
		depthStencilBuffer->OnResetDevice();
}

std::shared_ptr<DepthStencilBuffer> DepthStencilBufferFactory::Create( bool sharable )
{
	//Check if have some depth stencil buffer sharable with same format
	if( sharable )
		for( const auto& depthStencilBuffer : cache )
			if( depthStencilBuffer->IsSharable() && depthStencilBuffer->SizeShiftRight() == 0 && depthStencilBuffer->AbsWidth() == 0 && depthStencilBuffer->AbsHeight() == 0 )
				return depthStencilBuffer;

	//Create a new Depth Stencil Buffer and build it
	auto depthStencilBuffer = std::make_shared<DepthStencilBuffer>( sharable );

	//Build Render Target
	if( Build( depthStencilBuffer ) )
	{
		cache.push_back( depthStencilBuffer );
		return depthStencilBuffer;
	}

	return nullptr;
}

std::shared_ptr<DepthStencilBuffer> DepthStencilBufferFactory::Create( const unsigned int sizeShiftRight, bool sharable )
{
	//Check if have some depth stencil buffer sharable with same format
	if( sharable )
		for( const auto& depthStencilBuffer : cache )
			if( depthStencilBuffer->IsSharable() && depthStencilBuffer->SizeShiftRight() == sizeShiftRight && depthStencilBuffer->AbsWidth() == 0 && depthStencilBuffer->AbsHeight() == 0 )
				return depthStencilBuffer;

	//Create a new Depth Stencil Buffer and build it
	auto depthStencilBuffer = std::make_shared<DepthStencilBuffer>( sizeShiftRight, sharable );

	//Build Render Target
	if( Build( depthStencilBuffer ) )
	{
		cache.push_back( depthStencilBuffer );
		return depthStencilBuffer;
	}

	return nullptr;
}

std::shared_ptr<DepthStencilBuffer> DepthStencilBufferFactory::Create( const unsigned int width, const unsigned int height, bool sharable )
{
	//Check if have some depth stencil buffer sharable with same format
	if( sharable )
		for( const auto& depthStencilBuffer : cache )
			if( depthStencilBuffer->IsSharable() && depthStencilBuffer->SizeShiftRight() == 0 && depthStencilBuffer->AbsWidth() == width && depthStencilBuffer->AbsHeight() == height )
				return depthStencilBuffer;

	//Create a new Depth Stencil Buffer and build it
	auto depthStencilBuffer = std::make_shared<DepthStencilBuffer>( width, height, sharable );

	//Build Render Target
	if( Build( depthStencilBuffer ) )
	{
		cache.push_back( depthStencilBuffer );
		return depthStencilBuffer;
	}

	return nullptr;
}

bool DepthStencilBufferFactory::Build( std::shared_ptr<DepthStencilBuffer> depthStencilBuffer )
{
	//Create Surface
	IDirect3DSurface9* surface;
	HRESULT hr = graphics->GetDevice()->CreateDepthStencilSurface( depthStencilBuffer->Width(), depthStencilBuffer->Height(), graphics->GetDepthStencilBufferInfo().format, D3DMULTISAMPLE_NONE, 0, FALSE, &surface, NULL );

	if( FAILED( hr ) )
	{
		DELTA3D_LOGERROR( "Could not create Depth Stencil Buffer Surface [%08X]", hr );

		return false;
	}

	//Update Surface Pointer
	depthStencilBuffer->surface = surface;

	return true;
}

}