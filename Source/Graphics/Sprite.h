#pragma once

#include "../Math/Color.h"
#include "../Math/Rect.h"

#include "Graphics.h"
#include "RenderTarget.h"

namespace Delta3D::Graphics
{
class Texture;

class Sprite : GraphicsImpl, public std::enable_shared_from_this<Sprite>
{
friend class SpriteFactory;
public:
	//! Default Constructor for Sprite.
	Sprite( ID3DXSprite* sprite_, bool sharable_ = true );

	//! Deconstructor.
	~Sprite();

	//! Get boolean to determinate if sprite is sharable.
	const bool IsSharable() const { return sharable; }

	//! Get Sprite Object.
	ID3DXSprite* Get() const { return sprite; }

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Begin Sprite Object.
	bool Begin( const bool alphaBlending );

	/**
	 * Draw Sprite
	 * @param texture Pointer to texture object
	 * @param color Color of sprite
	 * @param source Source Rectangle
	 */
	void Draw( std::shared_ptr<Texture> texture, const Math::Color& color = Math::Color::White, const Math::RectInt& source = Math::RectInt::Null );

	/**
	 * Draw Sprite
	 * @param texture Pointer to texture direct3d object
	 * @param color Color of sprite
	 * @param source Source Rectangle
	 */
	void Draw( IDirect3DTexture9* texture, const Math::Color& color = Math::Color::White, const Math::RectInt& source = Math::RectInt::Null );

	/**
	 * Draw Sprite
	 * @param texture Pointer to render target object
	 */
	void Draw( std::shared_ptr<RenderTarget> renderTarget );

	//! Flush Sprite Object.
	void Flush() { sprite->Flush(); }

	//! End Sprite Object.
	void End();

	//! Push Scaling Transformation 2D.
	void PushScaling( const Math::Vector2& scaling, const Math::Vector2& scalingCenter = Math::Vector2::Null );

	//! Push Rotation Transformation 2D.
	void PushRotation( float rotation, const Math::Vector2& rotationCenter = Math::Vector2::Null );

	//! Push Translation Transformation 2D.
	void PushTranslation( const Math::Vector2& translation );

	//! Push Matrix Transform.
	void PushTransform( const D3DXMATRIX& matrix );

	//! Pop Matrix Transform.
	void PopTransform( int popCount = 1 );
public:
	static std::shared_ptr<Sprite> Default;	//!< Default Sprite
private:
	ID3DXSprite* sprite;	//!< Sprite Object
	ID3DXMatrixStack* transformMatrixStack;	//!< Transformation Matrix Stack

	bool sharable;	//!< Boolean do determinate if sprite is sharable
	int useCount;	//!< Use Counter of sprite (Begin() and End())
};

class SpriteFactory
{
public:
	//! Default Constructor for Sprite Factory.
	SpriteFactory( Graphics* graphics_ );

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Create Sprite.
	std::shared_ptr<Sprite> Create( const bool sharable = true );
private:
	std::vector<std::shared_ptr<Sprite>> cache;	//!< Cache of Sprite's

	Graphics* graphics;	//!< Graphics Pointer
};

}