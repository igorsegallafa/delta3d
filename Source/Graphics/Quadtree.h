#pragma once

#include "../Math/BoundingBox.h"

namespace Delta3D::Graphics
{
const unsigned int maxMeshesQuadtreeDefault = 4;

class Model;
class Mesh;

struct Node
{
	Node* parentNode;
	Math::BoundingBox boundingBox;
	std::list<Mesh*> meshes;
	std::array<Node*, 4> nodes;
};

struct SortMeshJob
{
	Mesh* mesh;
	float distance;
};

class CompareMeshDistance
{
public:
	bool operator()( const SortMeshJob& lhs, const SortMeshJob& rhs ) const
	{
		if( lhs.distance < rhs.distance )
			return true;

		return false;
	}
};

class Quadtree : public GraphicsImpl
{
public:
	//! Default Constructor for QuadTree.
	Quadtree( Model* model, const unsigned int maxMeshes_ = maxMeshesQuadtreeDefault ) : GraphicsImpl(), model( model ), maxMeshes( maxMeshes_ ), root( nullptr ) {}

	//! Deconstructor.
	~Quadtree();

	//! Build Quadtree.
	bool Build();

	//! Render QuadTree.
	void Render();

	void Clean() { DeleteNodes( root ); delete root; root = nullptr; }
private:
	//! Render QuadTree Node.
	void Render( Node* node );

	//! Create QuadTree Node.
	void CreateNode( Node* node, Math::BoundingBox box );

	//! Delete Children Nodes
	void DeleteNodes( Node* node );

	//! Get a list with contained meshes of Bounding Box.
	const std::vector<Mesh*> ContainedMeshes( const Math::BoundingBox& box );
private:
	std::priority_queue<SortMeshJob, std::vector<SortMeshJob>, CompareMeshDistance> transparentMeshes;	//!< Transparent Meshes
	std::queue<Mesh*> opacityMeshes;	//!< Opacity Meshes

	unsigned int maxMeshes;	//!< Max Meshes per Node
	Model* model;	//!< Parent Model
	Node* root;	//!< Root node from Quadtree

	Math::Matrix4 translation;
};
}