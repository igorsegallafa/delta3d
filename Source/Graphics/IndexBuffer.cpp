#include "PrecompiledHeader.h"
#include "IndexBuffer.h"

namespace Delta3D::Graphics
{

IndexBuffer::IndexBuffer( IDirect3DIndexBuffer9* indexBuffer_, unsigned int primitiveSize_, unsigned int primitiveCount_ ) : 
	indexBuffer( indexBuffer_ ), 
	primitiveSize( primitiveSize_ ), 
	primitiveCount( primitiveCount_ )
{
	if( indexBuffer )
		indexBuffer->AddRef();
}

IndexBuffer::~IndexBuffer()
{
	if( indexBuffer )
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}
}

void* IndexBuffer::Lock()
{
	void* p;
	HRESULT hr = indexBuffer->Lock( 0, 0, &p, 0 );

	if( SUCCEEDED( hr ) )
		return p;

	return nullptr;
}

void IndexBuffer::Unlock()
{
	indexBuffer->Unlock();
}

}