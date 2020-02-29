#include "PrecompiledHeader.h"
#include "Sprite.h"

#include "Texture.h"

namespace Delta3D::Graphics
{

std::shared_ptr<Sprite> Sprite::Default = nullptr;

Sprite::Sprite( ID3DXSprite* sprite_, bool sharable_ ) : 
	GraphicsImpl::GraphicsImpl(), 
	sprite( sprite_ ), 
	sharable( sharable_ ), 
	useCount( 0 )
{
	if( sprite )
	{
		sprite->AddRef();

		D3DXCreateMatrixStack( 0, &transformMatrixStack );
	}
}

Sprite::~Sprite()
{
	if( sprite )
	{
		sprite->Release();
		sprite = nullptr;
	}

	if( transformMatrixStack )
	{
		transformMatrixStack->Release();
		transformMatrixStack = nullptr;
	}
}

void Sprite::OnLostDevice()
{
	sprite->OnLostDevice();
}

void Sprite::OnResetDevice()
{
	sprite->OnResetDevice();
}

bool Sprite::Begin( const bool alphaBlending )
{
	//Can not begin sprite repeatedly
	if( useCount == 0 )
	{
		HRESULT hr;
		hr = sprite->Begin( alphaBlending ? D3DXSPRITE_ALPHABLEND : 0 );

		if( SUCCEEDED( hr ) )
		{
			useCount++;
			return true;
		}
	}
	else
		useCount = 0;

	return false;
}

void Sprite::Draw( std::shared_ptr<Texture> texture, const Math::Color& color, const Math::RectInt& source )
{
	if( useCount > 0 )
	{
		RECT srcRect = RECT{ source.x, source.y, source.w, source.h };

		if( (texture) && texture->Get() )
		{
			sprite->Draw( texture->Get(), source != Math::RectInt::Null ? &srcRect : nullptr, nullptr, nullptr, color.ToUInt() );
			sprite->Flush();
		}
	}
}

void Sprite::Draw( IDirect3DTexture9* texture, const Math::Color& color, const Math::RectInt& source )
{
	if( useCount > 0 )
	{
		RECT srcRect = RECT{ source.x, source.y, source.x + source.w, source.y + source.h };

		if( texture )
		{
			sprite->Draw( texture, source != Math::RectInt::Null ? &srcRect : nullptr, nullptr, nullptr, color.ToUInt() );
			sprite->Flush();
		}
	}
}

void Sprite::Draw( std::shared_ptr<RenderTarget> renderTarget )
{
	if( useCount > 0 )
	{
		bool popTransform = false;

		if( (renderTarget->Width() != graphics->GetRenderTargetInfo().width) || (renderTarget->Height() != graphics->GetRenderTargetInfo().height) )
		{
			PushScaling( Math::Vector2( (float)graphics->GetRenderTargetInfo().width / (float)renderTarget->Width(), (float)graphics->GetRenderTargetInfo().height / (float)renderTarget->Height() ) );
			popTransform = true;
		}

		sprite->Draw( renderTarget->Texture(), NULL, NULL, NULL, D3DCOLOR_XRGB( 255, 255, 255 ) );
		sprite->Flush();

		if( popTransform )
			PopTransform();
	}
}

void Sprite::End()
{
	if( useCount > 0 )
		useCount--;

	if( useCount == 0 )
		sprite->End();
	else
		sprite->Flush();
}

void Sprite::PushScaling( const Math::Vector2& scaling, const Math::Vector2& scalingCenter )
{
	D3DXVECTOR2 scalingd3d;
	scalingd3d.x = scaling.x;
	scalingd3d.y = scaling.y;

	D3DXVECTOR2 scalingCenterd3d;
	scalingCenterd3d.x = scalingCenter.x;
	scalingCenterd3d.y = scalingCenter.y;

	D3DXMATRIX m;
	D3DXMatrixTransformation2D( &m, &scalingCenterd3d, 0.0f, &scalingd3d, NULL, 0.0f, NULL );

	PushTransform( m );
}

void Sprite::PushRotation( float rotation, const Math::Vector2& rotationCenter )
{
	D3DXVECTOR2 rotationcenterd3d;
	rotationcenterd3d.x = rotationCenter.x;
	rotationcenterd3d.y = rotationCenter.y;

	D3DXMATRIX m;
	D3DXMatrixTransformation2D( &m, NULL, 0.0f, NULL, &rotationcenterd3d, rotation, NULL );

	PushTransform( m );
}

void Sprite::PushTranslation( const Math::Vector2& translation )
{
	D3DXVECTOR2 translationd3d;
	translationd3d.x = translation.x;
	translationd3d.y = translation.y;

	D3DXMATRIX m;
	D3DXMatrixTransformation2D( &m, NULL, 0.0f, NULL, NULL, 0.0f, &translationd3d );

	PushTransform( m );
}

void Sprite::PushTransform( const D3DXMATRIX& matrix )
{
	transformMatrixStack->Push();
	transformMatrixStack->MultMatrix( &matrix );

	sprite->SetTransform( transformMatrixStack->GetTop() );
}

void Sprite::PopTransform( int popCount )
{
	for( int i = 0; i < popCount; i++ )
		transformMatrixStack->Pop();

	sprite->SetTransform( transformMatrixStack->GetTop() );
}

SpriteFactory::SpriteFactory( Graphics* graphics_ ) : graphics( graphics_ )
{
}

void SpriteFactory::OnLostDevice()
{
	for( const auto& sprite : cache )
		sprite->OnLostDevice();
}

void SpriteFactory::OnResetDevice()
{
	for( const auto& sprite : cache )
		sprite->OnResetDevice();
}

std::shared_ptr<Sprite> SpriteFactory::Create( const bool sharable )
{
	//Find firstly on cache
	if( sharable )
		for( auto& sprite : cache )
			if( sprite->IsSharable() )
				return sprite;

	//Create Sprite Object
	LPD3DXSPRITE sprited3d;
	HRESULT hr = D3DXCreateSprite( graphics->GetDevice(), &sprited3d );

	if( FAILED( hr ) )
		return nullptr;

	//Create Sprite
	auto sprite = std::make_shared<Sprite>( sprited3d, sharable );

	//Put it on Cache
	cache.push_back( sprite );

	//Release our reference
	sprited3d->Release();

	return sprite;
}

}