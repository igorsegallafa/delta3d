#pragma once

namespace Delta3D::Graphics
{
class VertexBuffer
{
public:
	//! Default Constructor for Vertex Buffer.
	VertexBuffer( IDirect3DVertexBuffer9* vertexBuffer_, unsigned int elementSize_, unsigned int  elementCount_, bool dynamic_ = false );

	//! Deconstructor.
	~VertexBuffer();

	//! Lock Vertex Buffer.
	void* Lock();

	//! Unlock Vertex Buffer.
	void Unlock();

	//! Getter Dynamic Vertex Buffer.
	const bool IsDynamic() const { return dynamic; }

	//! Release Vertex Buffer.
	void Release();

	//! Set a new Vertex Buffer Object.
	void Renew( IDirect3DVertexBuffer9* vertexBuffer_ );

	//! Get Vertex Buffer D3D Pointer.
	IDirect3DVertexBuffer9* Get() { return vertexBuffer; }

	//! Element Size Getter.
	const unsigned int ElementSize() const{ return elementSize; }

	//! Element Count Getter.
	const unsigned int ElementCount() const{ return elementCount; }
private:
	IDirect3DVertexBuffer9* vertexBuffer;	//!< Vertex Buffer D3D Pointer
	unsigned int elementSize;	//!< Element Size
	unsigned int elementCount;	//!< Element Count
	bool dynamic;	//!< Dynamic Vertex Buffer
};
}