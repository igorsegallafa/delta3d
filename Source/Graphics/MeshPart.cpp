#include "PrecompiledHeader.h"
#include "MeshPart.h"

namespace Delta3D::Graphics
{

MeshRenderResult MeshPart::CanRender()
{
	//Already checked
	if( canRender != MeshRenderResult::Undefined )
		return canRender;

	if( material == nullptr )
		canRender = MeshRenderResult::NotRender;
	else if( material->textures.empty() && material->animatedTextures.empty() )
		canRender = MeshRenderResult::NotRender;
	else if( material->colorTransform& 0x0400 )
		canRender = MeshRenderResult::NotRender;
	else if( material->diffuseColor.a <= 0.95f )
		canRender = MeshRenderResult::Transparent;
	else if( material->hasOpacityMap )
		canRender = MeshRenderResult::Opacity;
	else
		canRender = MeshRenderResult::Render;

	return canRender;
}

bool MeshPart::Render( std::shared_ptr<VertexBuffer> vertexBuffer, bool skinnedMesh )
{
	//Not Render
	if( canRender == MeshRenderResult::NotRender )
		return false;

	//Index Buffer already created?
	if( indexBuffer )
	{
		//Set Index Buffer
		if( FAILED( device->SetIndices( indexBuffer->Get() ) ) )
			return false;

		//Set Vertex Declaration
		if( FAILED( device->SetVertexDeclaration( skinnedMesh ? VertexDeclaration::SkinnedTex[1]->Get() : VertexDeclaration::Tex[2]->Get() ) ) )
			return false;

		//Prepare Material and Render IT!
		if( material->Prepare() )
		{
			if( material->GetEffect()->Begin() > 0 )
			{
				for( unsigned int i = 0; i < material->GetEffect()->NumPasses(); i++ )
				{
					if( material->GetEffect()->BeginPass( i ) )
					{
						device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexBuffer->ElementCount(), 0, indexBuffer->PrimitiveCount() / 3 );
						material->GetEffect()->EndPass();
					}
				}

				material->GetEffect()->End();
			}

			return true;
		}
	}
	else
	{
		//Build Index Buffer before Render
		Build();

		return true;
	}

	return false;
}

void MeshPart::Build()
{
	if( !indices.empty() )
	{
		//Create Index Buffer
		indexBuffer = graphics->CreateIndexBuffer( sizeof( unsigned short ), indices.size() );

		//Created successfully?
		if( indexBuffer )
		{
			unsigned short* indicesArray = (unsigned short*)indexBuffer->Lock();

			//Build Indices Array
			if( indicesArray )
			{
				int curIndice = 0;

				//Append it to Array
				for( const auto& i : indices )
					indicesArray[curIndice++] = i;

				indexBuffer->Unlock();
			}
		}
	}
}

}