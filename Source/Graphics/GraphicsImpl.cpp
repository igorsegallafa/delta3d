#include "PrecompiledHeader.h"
#include "GraphicsImpl.h"

#include "Graphics.h"
#include "Renderer.h"

namespace Delta3D::Graphics
{

GraphicsImpl::GraphicsImpl() : graphics( nullptr ), device( nullptr ), renderer( nullptr )
{
	graphics = Graphics::GetInstance();
	device = graphics->GetDevice();
	renderer = graphics->GetRenderer();
}

GraphicsImpl::~GraphicsImpl()
{
	graphics = nullptr;
	device = nullptr;
	renderer = nullptr;
}

}