#pragma once

#include "Mesh.h"
#include "MaterialCollection.h"
#include "Material.h"

#include "../Math/Vector3.h"
#include "../Math/Vector2.h"

namespace Delta3D::Graphics
{
enum class ModelVersion
{
	SMDModelHeader62,
	SMDModelHeader64,
	SMDModelHeader66,
};

struct ModelGroup
{
	std::list<Mesh*> meshes;
};

class Model : public GraphicsImpl, public Core::TimerImpl
{
public:
	//! Default constructor for Model.
	Model();

	//! Deconstructor.
	~Model();

	//! Add Mesh to Model.
	bool AddMesh( Mesh* mesh );

	//! Reorder Meshes considering parent-child relationship by Mesh.
	void ReorderMeshes();

	//! Set a Parent Model or Parent Mesh for meshes.
	void SetParent( Model* modelParent, Mesh* meshParent = nullptr );

	//! Set Auto Animate Model.
	void SetAutoAnimate( bool value ) { autoAnimate = value; }

	/**
	 * Get a specific Mesh from Model by name
	 * @param meshName Name of Mesh
	 * @return Pointer to Mesh if that was found
	 */
	Mesh* GetMesh( std::string meshName );

	/**
	 * Get a specific Mesh from Model by name
	 * @param meshName Name of Mesh
	 * @return Index of Mesh if that was found
	 */
	int GetOrderedMeshIndex( const std::string& meshName );

	/**
	 * Get a specific Mesh from Model by name
	 * @param meshName Name of Mesh
	 * @return Index of Mesh if that was found
	 */
	int GetMeshIndex( const std::string& meshName );

	/**
	 * Get Meshes from Model by name
	 * @param meshName Name of Mesh
	 * @return List with Meshes that was found
	 */
	std::vector<Mesh*> GetMeshes( std::string meshName );

	/**
	 * Set an Animation Frame for Model
	 * @param frame_ Frame desired of Animation
	 * @param frameInfo Animation Info
	 */
	void SetFrame( int frame_, IO::SMD::FrameInfo* frameInfo = nullptr );

	/**
	 * Animate Model
	 * @param frame_ Frame desired to make Animation
	 * @param rotation_ Rotation to Model
	 * @param frameInfo Animation info
	 */
	void Animate( int frame_ = 0, Math::Vector3Int rotation_ = Math::Vector3Int::Null, IO::SMD::FrameInfo* frameInfo = nullptr );

	/**
	 * Set a Position and Rotation for Model
	 * @param position_ Position desired
	 * @param rotation_ Rotation desired
	 */
	void SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ );

	/**
	 * Define a custom Renderer for Meshes
	 * @param renderer Pointer to Renderer Function
	 */
	static void SetCustomRenderer( std::function<void( Mesh* )> const& renderer ) { customRenderer = renderer; }

	/**
	 * Define a custom Material Collection for Meshes
	 * @param renderer Pointer to Material Collection Function
	 */
	static void SetCustomMaterialCollection( std::function<void( MaterialCollection* )> const& materialCollection ) { customMaterialCollection = materialCollection; }

	/**
	 * Update Bounding Volumes from Model
	 * @param force Force to Update Bounding Volumes
	 */
	void UpdateBoundingVolumes( bool forceUpdate = false );

	/**
	 * Update Bones Transformations from Model
	 */
	void UpdateBonesTransformations();

	/**
	 * Set if Model will use Frustum Culling
	 * @param value Boolean
	 */
	void SetUseFrustumCulling( bool value ) { useFrustumCulling = value; }

	/**
	 * Set Custom Diffuse Color for Materials
	 */
	void SetDiffuseColor( Math::Color color );

	/**
	 * Set Custom Add Color for Materials
	 */
	void SetAddColor( Math::Color color );

	//! Getter Material Collection.
	MaterialCollection* GetMaterialCollection() const { return materialCollection;  }

	//! Update Model.
	void Update( float timeElapsed );

	//! Render Model.
	bool Render( IO::SMD::FrameInfo* frameInfo = nullptr, ModelGroup* modelGroup = nullptr );

	//! Load Model.
	bool Load( std::string filePath, Model* skeleton_ = nullptr, bool temporaryTextures = false );
public:
	std::vector<Mesh*> meshes;	//!< Meshes List
	std::vector<Mesh*> orderedMeshes;	//!< Ordered Meshes List

	Model* skeleton;	//!< Skeleton Model (if exists, skinned model)
	MaterialCollection* materialCollection;	//!< Materials used by Model

	int maxFrame;	//!< Max Animation Frame
	int	frame;	//!< Current Frame used by Model

	Math::Vector2 size;	//!< Model Size (Width and Height)
	Math::Vector3 position;	//!< Model Position (xyz)
	Math::Vector3Int rotation;	//!< Model Rotation (xyz)
	Math::Vector3 scaling;	//!< Model Scaling

	Math::Sphere boundingSphere;	//!< Bounding Sphere
	Math::BoundingBox worldBoundingBox;	//!< World Bounding Box

	std::vector<IO::SMD::Frame> animationsFrameInfo;	//!< Animations Frame Info List

	int lastAnimationFrame;	//!< Last Animation Frame (to no repeat same animation twice)
	Math::Vector3Int lastRotation;	//!< Last Animation Rotation (to no repeat same animation twice)

	bool useFrustumCulling;	//!< Flag to determinate if model will make frustum culling
	bool autoAnimate;	//!< Flag to determinate if model is auto animated
	bool forceUpdate;	//!< Flat to force animation update

	std::shared_ptr<Texture> bonesTexture;	//!< Bones Texture Fetch
	Math::Matrix4* bonesWorldMatrices;	//!< World Matrices from Bones
	float* bonesTransformations;

	ModelVersion version;	//!< Model Version

	static std::function<void( Mesh* )> customRenderer;	//!< Define a custom renderer for Model
	static std::function<void( MaterialCollection* )> customMaterialCollection;	//!< Define a custom material collection for Model
};
}