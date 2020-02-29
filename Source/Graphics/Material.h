#pragma once

#include "Graphics.h"
#include "Shader.h"
#include "Renderer.h"

#include "../Math/Vector3.h"
#include "../Resource/AttributeAnimation.h"

namespace Delta3D::Graphics
{

class Texture;
class Material : public GraphicsImpl
{
public:
	enum TextureTransform
	{
		None,

		Scrolling = 4,
		Reflex = 5,
		Scrolling2x = 6,
		Scrolling4x = 8,
	};

	enum MeshTransform
	{
		Water = 0x200,
	};

	//! Default Constructor for Material.
	Material() : 
		GraphicsImpl(), 
		effect( nullptr ), 
		blendingMaterial( nullptr ), 
		useBlendingMaterial( false ), 
		selfIllumBlendingMode( 0 ), 
		addColor( 0.0f, 0.0f, 0.0f, 0.0f ), 
		customMaterial( false ), 
		meshTransform( 0 ), 
		useCount( 0 ), 
		type( 0 ), 
		twoSided( false ), 
		useBlendingMap( false ),
		textureTransform{ 0 }, 
		textureStageState{ 0 }, 
		shading( true ), 
		serialID( -1 ), 
		selfIlluminationAmount( 0.f ), 
		meshFlags( 0 ),
		isAnimated( false ), 
		hasOpacityMap( false ), 
		frameTotal( 0 ), 
		frameSpeed( 0 ), 
		colorTransform( 0 ), 
		blendType( StateBlock::None ),
		animationFrame( 0 )
	{
	}

	//! Deconstructor.
	~Material() { textures.clear(); animatedTextures.clear(); attributeAnimations.clear(); }

	//! Prepare Blending Material.
	void PrepareBlendingMaterial();

	//! Prepare Material to be used on Renderer.
	bool Prepare();

	//! Apply Material to Device.
	void Apply();

	//! Build Material.
	bool Build( FILE* file, bool skinned, bool loadTextures = true, bool useVertexColor = false, unsigned int mipMapsDefault = 0, bool temporaryTextures = false, bool use3D = false );

	//! Load Material from XML File.
	bool Load( const std::string& filePath, bool defaultSettings = false, bool use3D = false );

	//! Set a Blending Material.
	void SetBlendingMaterial( Material* material, bool forceUseBlendingMap = false );

	//! Clone a Material.
	Material* Clone( Material* other );

	//! Effect Getter.
	std::shared_ptr<Shader> GetEffect() const { return effect; }
public:
	int useCount;	//!< Use Counter

	int textureStageState[8];	//!< Texture Stage State to each texture
	int textureTransform[8];	//!< Texture Transform
	int	type;	//!< Type

	bool hasOpacityMap;	//!< Flag to determinate if texture has opacity map
	bool isAnimated;	//!< Flag to animated textures
	int blendType;	//!< Blending Type

	bool shading;	//!< Should shade material
	bool twoSided;	//!< Material is two sided
	DWORD serialID;	//!< Serial ID

	Math::Color diffuseColor;	//!< Diffuse Color
	float selfIlluminationAmount;	//!< Self Illumination Value

	int	colorTransform;	//!< Color Material Transform
	int	meshFlags;	//!< Mesh Flags
	int meshTransform;	//!< Mesh Transform

	int	frameTotal;	//!< Total of Frames on Animated Material
	int	frameSpeed;	//!< Step Frame Speed of Animated Material
	int	animationFrame;	//!< Current Frame of Animation

	bool useBlendingMaterial;	//!< Flag to determinate if Material will be blended with other
	int selfIllumBlendingMode;

	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr<Texture>> animatedTextures;
	std::vector<std::shared_ptr<Resource::AttributeAnimation>> attributeAnimations;

	Material* blendingMaterial;	//!< Extra Material to combine with other (blending)
	bool useBlendingMap;	//!< Use Blending Map
	Math::Color addColor;	//!< Add Color

	std::shared_ptr<Shader> effect;

	bool customMaterial;
};
}