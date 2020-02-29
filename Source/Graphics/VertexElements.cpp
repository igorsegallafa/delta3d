#include "PrecompiledHeader.h"
#include "VertexElements.h"

namespace Delta3D::Graphics
{

void VertexElements::AddElement( WORD stream, WORD offset, BYTE type, BYTE method, BYTE usage, BYTE usageIndex )
{
	D3DVERTEXELEMENT9 v;
	v.Stream = stream;
	v.Offset = offset;
	v.Type = type;
	v.Method = method;
	v.Usage = usage;
	v.UsageIndex = usageIndex;
	elements.push_back( v );
}

std::vector<D3DVERTEXELEMENT9> VertexElements::GetElements()
{
	std::vector<D3DVERTEXELEMENT9> v;

	for( const auto& c : elements )
		v.push_back( c );

	v.push_back( D3DDECL_END() );

	return v;
}

}