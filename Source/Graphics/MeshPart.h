#pragma once

#include "Graphics.h"
#include "IndexBuffer.h"
#include "VertexDeclaration.h"
#include "Material.h"
#include "Texture.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "Mesh.h"

namespace Delta3D::Graphics
{
class MeshPart : public GraphicsImpl
{
public:
	//! Default Constructor for Mesh Part.
	MeshPart() : GraphicsImpl(), canRender( MeshRenderResult::Undefined ), material( nullptr ), indexBuffer( nullptr ), mesh( nullptr ){}

	//! Deconstructor.
	~MeshPart() {}

	//! Check if Mesh Part must be rendered now.
	MeshRenderResult CanRender();

	/**
	 * Render Mesh Part
	 * @param vertexBuffer Vertex Buffer from parent Mesh
	 * @param skinnedMesh If is skinned Mesh
	 * @return Boolean to determinate if render was successfully or not
	 */
	bool Render( std::shared_ptr<VertexBuffer> vertexBuffer, bool skinnedMesh = false );

	/**
	 * Build Index Buffer from Mesh Part
	 */
	inline void Build();

	Mesh* mesh;	//!< Parent Mesh
	Material* material;	//!< Material Pointer
	std::vector<unsigned short> indices;	//!< Indices of this Mesh Part
	std::shared_ptr<IndexBuffer> indexBuffer;	//!< Index Buffer
	MeshRenderResult canRender;	//!< Can Render Mesh Part Flag
};
}