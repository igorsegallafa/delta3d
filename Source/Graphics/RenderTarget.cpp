#include "PrecompiledHeader.h"
#include "RenderTarget.h"

namespace Delta3D::Graphics
{

RenderTarget::~RenderTarget()
{
	if( texture )
	{
		texture->Release();
		texture = nullptr;
	}

	if( surface )
	{
		surface->Release();
		surface = nullptr;
	}
}

void RenderTarget::OnLostDevice()
{
	if( texture )
	{
		texture->Release();
		texture = nullptr;
	}

	if( surface )
	{
		surface->Release();
		surface = nullptr;
	}
}

void RenderTarget::OnResetDevice()
{
	graphics->GetRenderTargetFactory()->Build( shared_from_this() );
}

const unsigned int RenderTarget::Width()
{
	return width ? width : graphics->GetBackBufferInfo().width >> sizeShiftRight;
}

const unsigned int RenderTarget::Height()
{
	return height ? height : graphics->GetBackBufferInfo().height >> sizeShiftRight;
}

const unsigned int RenderTarget::SizeShiftRight()
{
	return sizeShiftRight;
}

void RenderTargetFactory::OnLostDevice()
{
	for( const auto& renderTarget : cache )
		renderTarget->OnLostDevice();
}

void RenderTargetFactory::OnResetDevice()
{
	for( const auto& renderTarget : cache )
		renderTarget->OnResetDevice();
}

std::shared_ptr<RenderTarget> RenderTargetFactory::Create( bool sharable )
{
	//Check if have some render target sharable with same format
	if( sharable )
		for( const auto& renderTarget : cache )
			if( renderTarget->IsSharable() && renderTarget->SizeShiftRight() == 0 && renderTarget->AbsWidth() == 0 && renderTarget->AbsHeight() == 0 )
				return renderTarget;

	//Create a new Render Target and build it
	auto renderTarget = std::make_shared<RenderTarget>( sharable );

	//Build Render Target
	if( Build( renderTarget ) )
	{
		cache.push_back( renderTarget );
		return renderTarget;
	}

	return nullptr;
}

std::shared_ptr<RenderTarget> RenderTargetFactory::Create( const unsigned int sizeShiftRight, bool sharable )
{
	//Check if have some render target sharable with same format
	if( sharable )
		for( const auto& renderTarget : cache )
			if( renderTarget->IsSharable() && renderTarget->SizeShiftRight() == sizeShiftRight && renderTarget->AbsWidth() == 0 && renderTarget->AbsHeight() == 0 )
				return renderTarget;

	//Create a new Render Target and build it
	auto renderTarget = std::make_shared<RenderTarget>( sizeShiftRight, sharable );

	//Build Render Target
	if( Build( renderTarget ) )
	{
		cache.push_back( renderTarget );
		return renderTarget;
	}

	return nullptr;
}

std::shared_ptr<RenderTarget> RenderTargetFactory::Create( const unsigned int width, const unsigned int height, bool sharable )
{
	//Check if have some render target sharable with same format
	if( sharable )
		for( const auto& renderTarget : cache )
			if( renderTarget->IsSharable() && renderTarget->SizeShiftRight() == 0 && renderTarget->AbsWidth() == width && renderTarget->AbsHeight() == height )
				return renderTarget;

	//Create a new Render Target and build it
	auto renderTarget = std::make_shared<RenderTarget>( width, height, sharable );

	//Build Render Target
	if( Build( renderTarget ) )
	{
		cache.push_back( renderTarget );
		return renderTarget;
	}

	return nullptr;
}

bool RenderTargetFactory::Build( std::shared_ptr<RenderTarget> renderTarget )
{
	//Create Texture
	IDirect3DTexture9* texture;
	HRESULT hr = graphics->GetDevice()->CreateTexture( renderTarget->Width(), renderTarget->Height(), 1, D3DUSAGE_RENDERTARGET,  graphics->GetRenderTargetInfo().format == D3DFMT_X4R4G4B4 ? D3DFMT_A4R4G4B4 : D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL );

	if( FAILED( hr ) )
	{
		DELTA3D_LOGERROR( "Could not create Render Target Texture [%08X]", hr );

		return false;
	}

	//Create Surface
	IDirect3DSurface9* surface;
	hr = texture->GetSurfaceLevel( 0, &surface );

	if( FAILED( hr ) )
	{
		texture->Release();
		DELTA3D_LOGERROR( "Could not create Render Target Surface [%08X]", hr );

		return false;
	}

	//Update Texture and Surface Pointers
	renderTarget->texture = texture;
	renderTarget->surface = surface;

	return true;
}

}