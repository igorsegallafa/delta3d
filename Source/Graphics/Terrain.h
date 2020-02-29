#pragma once

#include "../Math/Color.h"

#include "Renderer.h"
#include "Quadtree.h"
#include "Model.h"

namespace Delta3D::Graphics
{
class Terrain : public GraphicsImpl
{
public:
	//! Default Constructor for Terrain.
	Terrain();

	//! Deconstructor.
	~Terrain();

	//! Load Terrain File.
	bool Load( const std::string& filePath );

	//! Render Terrain.
	void Render();
private:
	int id;	//!< Terrain ID

	Quadtree* quadTree;	//!< QuadTree used for Culling
	Model* model;	//!< Main model from Terrain

	std::vector<Model*> animatedModels;	//!< Animated Models from Terrain
};
}