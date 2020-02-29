#include "PrecompiledHeader.h"
#include "Quadtree.h"

#include "Renderer.h"
#include "Model.h"
#include "Mesh.h"

namespace Delta3D::Graphics
{

std::unordered_map<Mesh*, Node*> meshNodes;

Quadtree::~Quadtree()
{
	//Clean Quadtree
	Clean();
}

bool Quadtree::Build()
{
	//Update Bounding Boxes before start
	model->SetFrame( 0 );

	//Allocate Root Node
	root = new Node();

	//Creat Root Node
	CreateNode( root, model->worldBoundingBox );

	//Translation Transform
	translation = Math::Matrix4::Identity;
	translation.Translate( model->position );

	meshNodes.clear();

	return false;
}

void Quadtree::Render()
{
	Render( root );

	//Render Opacity Batches
	while( !opacityMeshes.empty() )
	{
		opacityMeshes.front()->Render();
		opacityMeshes.pop();
	}

	//Render Transparent Batches
	while( !transparentMeshes.empty() )
	{
		SortMeshJob job = transparentMeshes.top();

		if( job.mesh )
			job.mesh->Render();

		transparentMeshes.pop();
	}
}

void Quadtree::CreateNode( Node* node, Math::BoundingBox box )
{
	auto containedMeshes = ContainedMeshes( box );

	//Initialize children Nodes
	for( size_t  i = 0; i < node->nodes.max_size(); i++ )
		node->nodes[i] = nullptr;

	node->boundingBox = box;

	//Doesn't found any Mesh
	if( containedMeshes.empty() )
		return;

	//Insert Node Contained Meshes
	for( auto& mesh : containedMeshes )
	{
		auto meshIt = meshNodes.find( mesh );

		if( meshIt != meshNodes.end() )
			(*meshIt).second->meshes.remove( mesh );

		node->meshes.push_back( mesh );
		meshNodes[mesh] = node;
	}

	//Split it into 8 Nodes
	if( containedMeshes.size() > maxMeshes )
	{
		float offsetX = box.Size().x / 2.0f;
		float offsetZ = box.Size().z / 2.0f;

		Math::BoundingBox boxes[8];
		boxes[0].min = Math::Vector3( box.min.x, box.min.y, box.min.z );
		boxes[0].max = Math::Vector3( box.max.x - offsetX, box.max.y, box.max.z - offsetZ );

		boxes[1].min = Math::Vector3( box.min.x + offsetX, box.min.y, box.min.z );
		boxes[1].max = Math::Vector3( box.max.x, box.max.y, box.max.z - offsetZ );

		boxes[2].min = Math::Vector3( box.min.x, box.min.y, box.min.z + offsetZ );
		boxes[2].max = Math::Vector3( box.max.x - offsetX, box.max.y, box.max.z );

		boxes[3].min = Math::Vector3( box.min.x + offsetX, box.min.y, box.min.z + offsetZ );
		boxes[3].max = Math::Vector3( box.max.x, box.max.y, box.max.z );

		//Create Nodes
		for( size_t i = 0; i < node->nodes.max_size(); i++ )
		{
			if( ContainedMeshes( boxes[i] ).size() > 0 )
			{
				node->nodes[i] = new Node();
				node->nodes[i]->parentNode = node;
				CreateNode( node->nodes[i], boxes[i] );
			}
		}
	}
}

void Quadtree::DeleteNodes( Node* node )
{
	for( size_t i = 0; i < node->nodes.max_size(); i++ )
	{
		if( node->nodes[i] )
		{
			DeleteNodes( node->nodes[i] );

			delete node->nodes[i];
			node->nodes[i] = nullptr;
		}
	}
}

void Quadtree::Render( Node* node )
{
	if( renderer->GetCamera()->Frustum().IsInside( node->boundingBox.Transformed( translation ) ) == Intersection::Outside )
		return;

	for( size_t i = 0; i < node->nodes.max_size(); i++ )
		if( node->nodes[i] )
			Render( node->nodes[i] );

	for( const auto& mesh : node->meshes )
	{
		//Post Render Meshes
		if( mesh->postRender )
		{
			graphics->renderer->PushPostRenderMesh( mesh );
			continue;
		}

		//Can Render Mesh immediately?
		MeshRenderResult ret = mesh->CanRender();

		if( ret == MeshRenderResult::Transparent )
		{
			SortMeshJob sortJob;
			sortJob.mesh = mesh;
			sortJob.distance = renderer->GetCamera()->Eye().Distance( mesh->worldBoundingBox.Transformed( mesh->translation ).Center() );
			transparentMeshes.push( sortJob );
		}
		else if( ret == MeshRenderResult::Opacity )
		{
			opacityMeshes.push( mesh );
		}
		else if( ret != MeshRenderResult::Undefined )
		{
			mesh->Render();
		}
	}

	//Render Debug
	if( renderer->IsDebugGeometry( DebugGeometry::DebugTerrainQuadtree ) )
		renderer->DrawDebugAABB( node->boundingBox.Transformed( translation ) );
}

const std::vector<Mesh*> Quadtree::ContainedMeshes( const Math::BoundingBox& box )
{
	std::vector<Mesh*> containedMeshes;

	for( auto& mesh : model->meshes )
		if( box.IsInside( mesh->worldBoundingBox ) == Intersection::Inside )
			containedMeshes.push_back( mesh );

	return containedMeshes;
}

}