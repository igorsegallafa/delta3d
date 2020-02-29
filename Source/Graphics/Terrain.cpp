#include "PrecompiledHeader.h"
#include "Terrain.h"

namespace Delta3D::Graphics
{
Terrain::Terrain() : GraphicsImpl(), quadTree( nullptr ), model( nullptr ), id( -1 )
{
}

Terrain::~Terrain()
{
	if( model )
	{
		delete model;
		model = nullptr;
	}

	if( quadTree )
	{
		delete quadTree;
		quadTree = nullptr;
	}

	for( auto& animModel : animatedModels )
	{
		if( animModel )
		{
			delete animModel;
			animModel = nullptr;
		}
	}

	animatedModels.clear();
}

bool Terrain::Load( const std::string& filePath )
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( filePath.c_str() );

	if( !result )
		return false;

	pugi::xml_node terrain = doc.child( "Terrain" );

	//Read Terrain
	id = atoi( terrain.attribute( "id" ).value() );
	std::string strTerrainFilePath = terrain.attribute( "file" ).value();
	int quadTreeMaxObjects = atoi( terrain.attribute( "quadTreeMaxObjets" ).value() );

	//Read Attributes
	Math::Vector3 position;

	for( pugi::xml_node attributeNode : terrain.children( "Attribute" ) )
	{
		//Diffuse Color
		if( _strcmpi( attributeNode.attribute( "type" ).value(), "Position" ) == 0 )
			sscanf_s( attributeNode.attribute( "value" ).value(), "%f %f %f", &position.x, &position.y, &position.z );
	}

	//Create Model
	model = new Model();

	//Load Model
	if( model->Load( strTerrainFilePath, nullptr, true ) )
	{
		//Reset Position and Frustum Culling
		model->SetPositionRotation( &position, &Math::Vector3Int() );
		model->SetUseFrustumCulling( false );

		//Create Quadtree
		quadTree = new Quadtree( model, quadTreeMaxObjects );
		quadTree->Build();

		//Read Animated Models from XML
		for( pugi::xml_node animationNode : terrain.children("Animation") )
		{
			Model* animatedModel = new Model();

			if( animatedModel->Load( animationNode.attribute( "file" ).value(), nullptr, true ) )
			{
				animatedModel->materialCollection->materialType = 0;
				animatedModel->SetPositionRotation( &Math::Vector3(), &Math::Vector3Int() );
				animatedModel->SetAutoAnimate( true );
				animatedModels.push_back( animatedModel );
			}
			else
				delete animatedModel;
		}

		//After everything already loaded, we can clear temporary cache
		graphics->GetTextureFactory()->Clear();

		return true;
	}

	return false;
}

void Terrain::Render()
{
	if( model )
	{
		//Render Animated Models
		for( const auto& animatedModel : animatedModels )
			animatedModel->Render();

		if( quadTree )
			quadTree->Render();
	}
}

}