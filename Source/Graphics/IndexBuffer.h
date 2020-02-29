#pragma once

namespace Delta3D::Graphics
{
class IndexBuffer
{
public:
	//! Default Constructor for Index Buffer.
	IndexBuffer( IDirect3DIndexBuffer9* indexBuffer_, unsigned int primitiveSize_, unsigned int primitiveCount_ );

	//! Deconstructor.
	~IndexBuffer();

	//! Lock Index Buffer.
	void* Lock();

	//! Unlock Index Buffer.
	void Unlock();

	//! Get Inder Buffer D3D Pointer.
	IDirect3DIndexBuffer9* Get() { return indexBuffer; }

	//! Primitive Size Getter.
	const unsigned int PrimitiveSize() const { return primitiveSize; }

	//! Primitive Count Getter.
	const unsigned int PrimitiveCount() const { return primitiveCount; }
private:
	IDirect3DIndexBuffer9* indexBuffer;	//!< Index Buffer D3D Pointer
	unsigned int primitiveSize;	//!< Primitive Size
	unsigned int primitiveCount;	//!< Primitive Count
};
}