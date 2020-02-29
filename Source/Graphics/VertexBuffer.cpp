#include "PrecompiledHeader.h"
#include "VertexBuffer.h"

namespace Delta3D::Graphics
{

VertexBuffer::VertexBuffer( IDirect3DVertexBuffer9* vertexBuffer_, unsigned int elementSize_, unsigned int elementCount_, bool dynamic_ ) : 
	vertexBuffer( vertexBuffer_ ),
	elementSize( elementSize_ ), 
	elementCount( elementCount_ ), 
	dynamic( dynamic_ )
{
	if( vertexBuffer )
		vertexBuffer->AddRef();
}

VertexBuffer::~VertexBuffer()
{
	if( vertexBuffer )
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
}

void* VertexBuffer::Lock()
{
	void* p;
	HRESULT hr = vertexBuffer->Lock( 0, 0, &p, dynamic ? D3DLOCK_DISCARD : 0 );

	if( SUCCEEDED( hr ) )
		return p;

	return nullptr;
}

void VertexBuffer::Unlock()
{
	vertexBuffer->Unlock();
}

void VertexBuffer::Release()
{
	if( vertexBuffer )
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
}

void VertexBuffer::Renew( IDirect3DVertexBuffer9* vertexBuffer_ )
{
	//Release old Vertex Buffer
	if( vertexBuffer )
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	//Set and load new vertex buffer
	vertexBuffer = vertexBuffer_;

	if( vertexBuffer )
		vertexBuffer->AddRef();
}

}