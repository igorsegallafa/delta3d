#include "PrecompiledHeader.h"
#include "VertexDeclaration.h"

namespace Delta3D::Graphics
{

std::shared_ptr<VertexDeclaration> VertexDeclaration::Tex[9] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
std::shared_ptr<VertexDeclaration> VertexDeclaration::SkinnedTex[9] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

VertexDeclaration::VertexDeclaration( IDirect3DVertexDeclaration9* vertexDeclaration_ ) : vertexDeclaration( vertexDeclaration_ )
{
	if( vertexDeclaration )
		vertexDeclaration->AddRef();
}

VertexDeclaration::~VertexDeclaration()
{
	if( vertexDeclaration )
	{
		vertexDeclaration->Release();
		vertexDeclaration = nullptr;
	}
}

}