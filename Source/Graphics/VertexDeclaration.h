#pragma once

namespace Delta3D::Graphics
{

class VertexDeclaration
{
public:
	//! Default Constructor for Vertex Declaration.
	VertexDeclaration( IDirect3DVertexDeclaration9* vertexDeclaration_ );

	//! Deconstructor.
	~VertexDeclaration();

	//! Get Vertex Declaration D3D Pointer.
	IDirect3DVertexDeclaration9* Get() { return vertexDeclaration; }

	static std::shared_ptr<VertexDeclaration> Tex[9];	//!< Vertex Declaration with 8 textures
	static std::shared_ptr<VertexDeclaration> SkinnedTex[9];	//!< Skinned Vertex Declaration with 8 textures
private:
	IDirect3DVertexDeclaration9* vertexDeclaration;	//!< Vertex Declaration D3D Pointer.
};
}