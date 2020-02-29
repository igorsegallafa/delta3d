#pragma once

#include "../Math/Vector2.h"
#include "../Math/Matrix4.h"
#include "../Math/Color.h"

#include "../Resource/AttributeAnimation.h"

namespace Effekseer{ class Manager; class Effect; typedef int Handle; struct Matrix44; };
namespace EffekseerRendererDX9{ class Renderer; };

namespace Delta3D::Graphics
{

class RenderTarget;
class RenderTargetFactory;
class DepthStencilBuffer;
class DepthStencilBufferFactory;
class Texture;
class TextureFactory;
class Sprite;
class SpriteFactory;
class Font;
class FontFactory;
class Shader;
class ShaderFactory;
class Renderer;
class VertexDeclaration;
class VertexBuffer;
class IndexBuffer;
class VertexElements;
class Particle;
class ParticleFactory;

using namespace Math;

enum class ClearFlags
{
	Target = 1 << 0,
	ZBuffer = 1 << 1,
	Stencil = 1 << 2,

	All = Target + ZBuffer + Stencil,
};	DEFINE_ENUM_FLAG_OPERATORS( ClearFlags );

enum StateBlock
{
	None,

	Default,
	Particle3D,
	Blend_None,
	Blend_Alpha,
	Blend_Color,
	Blend_Shadow,
	Blend_Lamp,
	Blend_AddColor,
	Blend_InvShadow,
	SelectGlow,
	SelectGlowMask,
	SelectGlowSprite,
	ObjectSky,
};

enum class GraphicEvents
{
	ResetDevice,
	LostDevice,
};

enum class FillMode
{
	Point,
	Wireframe,
	Solid,
};

struct SurfaceInfo
{
	unsigned int width;
	unsigned int height;
	D3DFORMAT format;

	SurfaceInfo() : width( 0 ), height( 0 ), format( D3DFMT_UNKNOWN ) {}
	~SurfaceInfo() = default;
};

class Graphics : public std::enable_shared_from_this<Graphics>, public Core::EventsImpl<GraphicEvents> 
{
public:
	//! Default Constructor for Graphics.
	Graphics();

	//! Deconstructor.
	~Graphics();

	static Graphics* GetInstance() { if( !instance )instance = new Graphics(); return instance; }

	//! Get This.
	std::shared_ptr<Graphics> Get() { return shared_from_this(); }

	//! Get Direct3D Interface.
	IDirect3D9* GetInterface() const { return interfaced3d; }

	//! Get Direct3D Device.
	IDirect3DDevice9* GetDevice() const { return device; }

	//! Get Window Size.
	const Vector2Int& GetSize() const { return size; }

	//! Renderer Getter.
	Renderer* GetRenderer() const { return renderer.get(); }

	//! Factories Getter.
	RenderTargetFactory* GetRenderTargetFactory() const { return renderTargetFactory.get(); }
	DepthStencilBufferFactory* GetDepthStencilBufferFactory() const { return depthStencilBufferFactory.get(); }
	TextureFactory* GetTextureFactory() const { return textureFactory.get(); }
	SpriteFactory* GetSpriteFactory() const { return spriteFactory.get(); }
	FontFactory* GetFontFactory() const { return fontFactory.get(); }
	ShaderFactory* GetShaderFactory() const { return shaderFactory.get(); }
	Resource::AttributeAnimationFactory* GetAttributeAnimationFactory() const { return attributeAnimationFactory.get(); }
	ParticleFactory* GetParticleFactory() const{ return particleFactory.get(); }

	//! Get Vertex and Pixel Shader Version.
	int GetVertexShaderVersionMajor() const{ return vertexShaderVersionMajor; }
	int GetPixelShaderVersionMajor() const{ return pixelShaderVersionMajor; }

	//! Effect Getter.
	Effekseer::Manager* GetEffectManager() const{ return effectManager; }
	EffekseerRendererDX9::Renderer* GetEffectRenderer() const{ return effectRenderer; }

	//! Create Vertex Declaration.
	std::shared_ptr<VertexDeclaration> CreateVertexDeclaration( std::shared_ptr<VertexElements> vertexElements );

	//! Create Dynamic Vertex Buffer.
	std::shared_ptr<VertexBuffer> CreateDynamicVertexBuffer( unsigned int elementSize, unsigned int elementCount );

	//! Create Vertex Buffer.
	std::shared_ptr<VertexBuffer> CreateStaticVertexBuffer( unsigned int elementSize, unsigned int elementCount );

	//! Create Index Buffer.
	std::shared_ptr<IndexBuffer> CreateIndexBuffer( unsigned int primitiveSize, unsigned int primitiveCount );

	//! Set Graphics Fill Mode
	void SetFillMode( FillMode fillMode );

	//! Set Render Target to Graphics.
	bool SetRenderTarget( std::shared_ptr<RenderTarget> renderTarget );

	//! Unset Render Target from Graphics.
	void UnsetRenderTarget();

	//! Set Depth Stencil Buffer to Graphics.
	bool SetDepthStencilBuffer( std::shared_ptr<DepthStencilBuffer> depthStencilBuffer );

	//! Unset Depth Stencil Buffer from Graphics.
	void UnsetDepthStencilBuffer();

	//! Get Back Buffer Information.
	const SurfaceInfo& GetBackBufferInfo() const { return backBufferInfo; }

	//! Get Depth Stencil Buffer Information.
	const SurfaceInfo& GetDepthStencilBufferInfo() const { return depthStencilBufferInfo; }

	//! Get Render Target Information.
	const SurfaceInfo& GetRenderTargetInfo() const { return renderTargetInfo; }

	//! Run Graphics (called everytime)
	void Run();

	//! Check if Device is Lost.
	const bool IsDeviceLost() const{ return isDeviceLost; }

	//! Check if Device is working good.
	bool IsDeviceReady();

	//! Begin Scene.
	bool Begin();

	//! End Scene.
	void End();

	//! Clear Scene.
	void Clear( ClearFlags flags = ClearFlags::All, const Math::Color& color = Math::Color( 0.0f, 0.0f, 0.0f, 0.0f ), bool stencil = false );

	//! Get Current State Block.
	const StateBlock& GetStateBlock() const { return stateBlock; }

	//! State Block Begin.
	void StateBlockBegin( StateBlock stateBlock_ = StateBlock::Default );

	//! State Block End.
	void StateBlockEnd( StateBlock stateBlock_ = StateBlock::None );

	//! State Block Reset.
	void StateBlockReset();

	//! Set Reduce Quality Texture Level (only for 3D).
	void SetReduceQualityTexture( int level ) { reduceQualityTexture = level; }

	//! Draw Primitive UP.
	HRESULT DrawPrimitive( D3DPRIMITIVETYPE primitiveType, DWORD vertexTypeDesc, DWORD primitiveCount, LPVOID vertices, DWORD vertexCount, DWORD flags );

	/**
	 * Initialize Graphics or set a new display mode
	 * @param width_ Width of Window
	 * @param height_ Height of Window
	 * @param windowed_ Window Mode (Windowed or Fullscreen)
	 * @param colorDepth_ Colot Depth of Window (16 or 32BPP)
	 * @param hWnd_ Window Handle
	 * @return True if successful
	 */
	bool SetMode( int width_, int height_, bool windowed_, int colorDepth_ = 32, bool antiAliasing_ = false, bool useSoftwareSkinning_ = false, HWND hWnd_ = nullptr );

private:
	//! Create Direct3D Interface.
	bool CreateInterface();

	//! Create Direct3D Device.
	bool CreateDevice();

	//! Check if all features needed by this is supported.
	bool CheckSupport();

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Push Render Target to Stack.
	void PushRenderTarget();

	//! Pop Render Target from Stack.
	void PopRenderTarget();

	//! Push Depth Stencil Buffer to Stack.
	void PushDepthStencilBuffer();

	//! Pop Depth Stencil Buffer from Stack.
	void PopDepthStencilBuffer();

	//! Update Surfaces Descriptions.
	void UpdateSurfacesDesc( bool onlyRenderTarget = false );

	//! Prince Device capabilities
	void PrintDeviceCaps();
public:
	HWND hWnd;	//!< Window Handle
	Vector2Int size;	//! Window Size
	bool windowed;	//!< Window Mode
	bool vsync;	//!< Window VSync
	bool useSoftwareSkinning;	//!< Force to use Software Skinning
	int colorDepth;	//!< Color Depth
	bool supportStencil32;	//!< Depth Stencil support 32-bit
	bool supportHardwareSkinning;	//!< Support Bones to Fetch Texture
	bool antiAliasing;
	int reduceQualityTexture;

	int vertexShaderVersionMajor;
	int pixelShaderVersionMajor;

	StateBlock stateBlock;	//!< Current State Block

	std::unique_ptr<RenderTargetFactory> renderTargetFactory;	//!< Render Target Factory
	std::unique_ptr<DepthStencilBufferFactory> depthStencilBufferFactory;	//!< Depth Stencil Buffer Factory
	std::unique_ptr<TextureFactory> textureFactory;	//!< Texture Factory
	std::unique_ptr<SpriteFactory> spriteFactory;	//!< Sprite Factory
	std::unique_ptr<FontFactory> fontFactory;	//!< Font Factory
	std::unique_ptr<ShaderFactory> shaderFactory;	//!< Effect Factory
	std::unique_ptr<Resource::AttributeAnimationFactory> attributeAnimationFactory;	//!< Attribute Animation Factory
	std::unique_ptr<ParticleFactory> particleFactory;	//!< Particle Factory

	std::unique_ptr<Renderer> renderer;	//!< Renderer

	static Graphics* instance;	//!< Singleton Object of This
#ifdef _D3D9
	IDirect3D9* interfaced3d;	//!< Direct3D Interface
	IDirect3DDevice9* device;	//!< Direct3D Device
	D3DCAPS9 deviceCaps;	//!< Device capabilities
	D3DPRESENT_PARAMETERS presentParams;	//!< Presentation parameters
	D3DPRESENT_PARAMETERS previousPresentParams;	//!< Backup Presentation parameters
	D3DDISPLAYMODE displayMode; //!< Display Mode

	SurfaceInfo backBufferInfo; //!< Back Buffer Description
	SurfaceInfo depthStencilBufferInfo; //!< Depth Stencil Buffer Description
	SurfaceInfo renderTargetInfo; //!< Render Target Description

	std::stack<LPDIRECT3DSURFACE9> renderTargets;	//!< Render Target's
	std::stack<LPDIRECT3DSURFACE9> depthStencilBuffers;	//!< Depth Stencil Buffer's
	std::vector<std::shared_ptr<VertexBuffer>> dynamicVertexBuffers;	//!< Dynamic Vertex Buffer

	bool isDeviceLost;	//!< Device Is Lost
#endif

	Effekseer::Manager* effectManager;	//!< Effect Manager
	EffekseerRendererDX9::Renderer* effectRenderer;	//!< Effect Renderer
};
}