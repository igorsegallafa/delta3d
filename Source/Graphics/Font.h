#pragma once

#include "../Math/Rect.h"
#include "../Math/Color.h"

#include "Graphics.h"

namespace Delta3D::Graphics
{
class Sprite;

class Font : public std::enable_shared_from_this<Font>
{
public:
	//! Default Constructor for Font.
	Font( std::shared_ptr<Sprite> sprite_, ID3DXFont* font_, D3DXFONT_DESC fontDesc_ );

	//! Deconstructor.
	~Font();

	//! Get Font Object.
	ID3DXFont* Get() const { return font; }

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Get Font Description.
	const D3DXFONT_DESC& Description() const { return fontDesc; }

	/**
	 * Draw Text implementation
	 * @param text String of text
	 * @param length Length of text
	 * @param rect Rectangle of text
	 * @param format Text Format
	 * @param color Text Color
	 */
	void Draw( const std::string& text, unsigned int length, const Math::RectInt& rect, unsigned int format, const Math::Color& color );

	/**
	 * Draw Text implementation
	 * @param x Coordinate X
	 * @param y Coordinate Y
	 * @param text String of text
	 * @param format Text Format
	 * @param color Text Color
	 */
	void Draw( int x, int y, const std::string& text, unsigned int format, const Math::Color& color );

	/**
	 * Get Text Width
	 * @param text String of Text
	 * @param length Length of Text
	 * @param Text Format
	 * @return Width of specified Text
	 */
	const unsigned int GetTextWidth( const std::string& text, unsigned int length, unsigned int format );

	/**
	 * Get Text Height
	 * @param text String of Text
	 * @param length Length of Text
	 * @param Text Format
	 * @return Height of specified Text
	 */
	const unsigned int GetTextHeight( const std::string& text, unsigned int length, unsigned int format );
public:
	static std::shared_ptr<Font> Default;	//!< Default Font
private:
	//! Draw Text
	void Draw( const std::string& text, unsigned int length, unsigned int format, const Math::RectInt& rect, const Math::Color& color );
private:
	std::shared_ptr<Sprite> sprite;	//!< Optional Sprite
	ID3DXFont* font;	//!< Font Object
	D3DXFONT_DESC fontDesc;	//!< Font Description
	unsigned int widthWhitespace;	//!< Width of ' ' character
	unsigned int widthAt;	//!< Width of 'at' character
};

class FontFactory
{
public:
	//! Default Constructor for Font Factory.
	FontFactory( Graphics* graphics_ );

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Create Sprite.
	std::shared_ptr<Font> Create( std::shared_ptr<Sprite> sprite, const std::string& fontName, int height = 16, int width = 0, bool bold = false, bool italic = false );

	/**
	 * Load a specified Font File
	 */
	void LoadFontFile( const std::string& filePath );
private:
	std::vector<std::shared_ptr<Font>> cache;	//!< Cache of Font's

	Graphics* graphics;	//!< Graphics Pointer
};
}