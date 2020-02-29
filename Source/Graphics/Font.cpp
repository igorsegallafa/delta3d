#include "PrecompiledHeader.h"
#include "Font.h"

#include "Sprite.h"

namespace Delta3D::Graphics
{

std::shared_ptr<Font> Font::Default = nullptr;

Font::Font( std::shared_ptr<Sprite> sprite_, ID3DXFont* font_, D3DXFONT_DESC fontDesc_ ) : sprite( sprite_ ), font( font_ ), fontDesc( fontDesc_ )
{
	if( font )
	{
		//Add our reference
		font->AddRef();

		//Get Width of Whitespace
		RECT rect = { 0 };

		font->DrawText( nullptr, "@", 1, &rect, DT_CALCRECT, -1 );
		widthAt = rect.right - rect.left;

		font->DrawText( nullptr, " @", 1, &rect, DT_CALCRECT, -1 );
		widthWhitespace = rect.right - rect.left - widthAt;
	}
}

Font::~Font()
{
	if( font )
	{
		font->Release();
		font = nullptr;
	}
}

void Font::OnLostDevice()
{
	font->OnLostDevice();
}

void Font::OnResetDevice()
{
	font->OnResetDevice();
}

void Font::Draw( const std::string& text, unsigned int length, const Math::RectInt& rect, unsigned int format, const Math::Color& color )
{
	Draw( text, length, format, rect, color );
}

void Font::Draw( int x, int y, const std::string& text, unsigned int format, const Math::Color& color )
{
	Draw( text, text.length(), format, Math::RectInt( x, y ), color );
}

const unsigned int Font::GetTextWidth( const std::string& text, unsigned int length, unsigned int format )
{
	//String for measure width
	std::string measureText = text + '@';

	//Get Text Width
	RECT rect = { 0 };
	font->DrawText( nullptr, measureText.c_str(), length + 1, &rect, format | DT_CALCRECT, -1 );

	return rect.right - rect.left - widthAt;
}

const unsigned int Font::GetTextHeight( const std::string& text, unsigned int length, unsigned int format )
{
	//Get Text Width
	RECT rect = { 0 };
	font->DrawText( nullptr, text.c_str(), length, &rect, format | DT_CALCRECT, -1 );

	return rect.bottom - rect.top;
}

void Font::Draw( const std::string& text, unsigned int length, unsigned int format, const Math::RectInt& rect, const Math::Color& color )
{
	//Check if is using sprite to draw it
	if( sprite )
	{
		sprite->PushTranslation( Math::Vector2() );
		sprite->Begin( true );
	}

	//Draw Text
	RECT r = { rect.x, rect.y, rect.w, rect.h };
	font->DrawText( sprite ? sprite->Get() : nullptr, text.c_str(), length, &r, format, color.ToUInt() );

	if( sprite )
	{
		sprite->PopTransform();
		sprite->End();
	}
}

FontFactory::FontFactory( Graphics* graphics_ ) : graphics( graphics_ )
{
}

void FontFactory::OnLostDevice()
{
	for( const auto& font : cache )
		font->OnLostDevice();
}

void FontFactory::OnResetDevice()
{
	for( const auto& font : cache )
		font->OnResetDevice();
}

std::shared_ptr<Font> FontFactory::Create( std::shared_ptr<Sprite> sprite, const std::string& fontName, int height, int width, bool bold, bool italic )
{
	//Build Font Description
	D3DXFONT_DESC fontDesc = { 0 };
	fontDesc.Height = height;
	fontDesc.Width = width;
	fontDesc.Weight = bold ? FW_BOLD : FW_NORMAL;
	fontDesc.MipLevels = 1;
	fontDesc.Italic = italic;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = CLEARTYPE_QUALITY;
	fontDesc.PitchAndFamily = VARIABLE_PITCH;
	strcpy_s( fontDesc.FaceName, fontName.c_str() );

	//Find Font firstly on Cache
	for( const auto& font : cache )
		if( memcmp( &font->Description(), &fontDesc, sizeof( D3DXFONT_DESC ) ) == 0 )
			return font;

	//Create Font Object
	LPD3DXFONT fontd3d;
	HRESULT hr = D3DXCreateFontIndirect( graphics->GetDevice(), &fontDesc, &fontd3d );

	if( FAILED( hr ) )
	{
		DELTA3D_LOGERROR( "Could not create the Font (%s)", fontName.c_str() );
		return nullptr;
	}

	//Create Font
	auto font = std::make_shared<Font>( sprite, fontd3d, fontDesc );

	//Put it on Cache
	cache.push_back( font );

	//Release our Reference
	fontd3d->Release();

	return font;
}

void FontFactory::LoadFontFile( const std::string& filePath )
{
	if( AddFontResourceEx( filePath.c_str(), FR_PRIVATE, 0 ) == 0 )
		DELTA3D_LOGERROR( "Could not load the Font File (%s)", filePath.c_str() );
}

}