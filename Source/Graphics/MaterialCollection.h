#pragma once

#include "Graphics.h"

namespace Delta3D::Graphics
{
class Material;
class MaterialCollection : public GraphicsImpl
{
public:
	//! Default Constructor for Material Collection.
	MaterialCollection();

	//! Constructor for Material Collection with a specified name.
	MaterialCollection( const std::string& name_ );

	//! Constructor for Material Collection with a specified number of materials.
	MaterialCollection( int materialsCount_ );

	//! Deconstructor.
	~MaterialCollection();

	/**
	 * Create Material and add it to Material Collection
	 * @param textureFilePath File Path of desired Texture to load
	 * @param twoSided Material two sided
	 * @param shading Shading will be applied to material
	 * @param blendType Material Blending Type
	 * @param useColorKey Material will clip a specific color (black pixel)
	 * @return ID of Material Created
	 */
	int CreateMaterial( std::string textureFilePath, bool twoSided, bool shading, int blendType, bool useColorKey = false );
	
	/**
	 * Add Texture for Material
	 * @param materialID Material ID
	 * @param textureFilePath File Path of Texture to add
	 */
	void AddTexture( int materialID , std::string textureFilePath );

	/**
	 * Add Animated Texture for Material (Texture Frames)
	 * @param material Pointer to Material
	 * @param textureList List of Texture Frames
	 * @param blendType Material Blending Type
	 * @param autoAnimate If material is auto animated
	 */
	void AddAnimatedTexture( Material* material, std::vector<std::string> textureList, int blendType , bool autoAnimate );

	/**
	 * Set Material Frame
	 * @param material Pointer to Material
	 * @param frame Frame desired to set
	 * @return Previous Frame
	 */
	int SetFrame( Material* material, int frame );

	/**
	 * Getter of Material by Texture Name
	 * @param textureName Name of texture
	 * @return Pointer to Material
	 */
	Material* GetMaterialByTexture( std::string textureName );

	//! Set Blending Material.
	void SetBlendingMaterial( Material* material, bool useBlendingMap = false );

	//! Build Material Collection.
	bool Build( FILE* file, bool skinned = false, bool loadTextures = true, bool useVertexColor = false, unsigned int mipMapsDefault = 0, bool temporaryTextures = false, bool use3D = false );

	//! Load Material List.
	bool Load( const std::string& filePath, bool use3D = false );
public:
	DWORD header;	//!< Header
	Material* materials;	//!< Materials List
	int materialsCount;	//!< Materials Count
	int materialType;	//!< Material Type

	std::string name;	//!< Material Collection Name
};
}