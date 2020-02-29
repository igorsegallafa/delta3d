#pragma once

namespace Delta3D::Graphics
{
class Graphics;
class Renderer;

class GraphicsImpl
{
public:
	//! Default Constructor for GraphicsImpl.
	GraphicsImpl();

	//! Deconstructor.
	~GraphicsImpl();

	Graphics* graphics;	//!< Graphics Pointer
	IDirect3DDevice9* device;	//!< Direct3D Device
	Renderer* renderer;	//!< Renderer Pointer
};
}