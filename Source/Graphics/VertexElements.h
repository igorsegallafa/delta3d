#pragma once

namespace Delta3D::Graphics
{
class VertexElements
{
public:
	//! Add Vertex Element.
	void AddElement( WORD stream, WORD offset, BYTE type, BYTE method, BYTE usage, BYTE usageIndex );

	//! Get Vertex Elements.
	std::vector<D3DVERTEXELEMENT9> GetElements();
private:
	std::vector<D3DVERTEXELEMENT9> elements;	//!< Vertex Elements
};
}