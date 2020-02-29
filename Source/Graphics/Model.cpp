#include "PrecompiledHeader.h"
#include "Model.h"

#include "Renderer.h"

namespace Delta3D::Graphics
{
std::function<void( Mesh* )> Model::customRenderer( nullptr );
std::function<void( MaterialCollection* )> Model::customMaterialCollection( nullptr );

Model::Model() : 
	GraphicsImpl::GraphicsImpl(), 
	Core::TimerImpl::TimerImpl(), 
	skeleton( nullptr ), 
	materialCollection( nullptr ), 
	maxFrame( 0 ), 
	frame( 0 ), 
	useFrustumCulling( true ), 
	lastAnimationFrame( -1 ), 
	lastRotation( -1, -1, -1 ), 
	scaling( 1.0f, 1.0f, 1.0f ), 
	autoAnimate( false ), 
	bonesTexture( nullptr ), 
	version( ModelVersion::SMDModelHeader62 ),
	bonesWorldMatrices( nullptr ), 
	bonesTransformations( nullptr ), 
	forceUpdate( false )
{
}

Model::~Model()
{
	if( !meshes.empty() )
	{
		for( auto& mesh : meshes )
		{
			delete mesh;
			mesh = nullptr;
		}
	}

	meshes.clear();
	orderedMeshes.clear();

	if( materialCollection )
	{
		delete materialCollection;
		materialCollection = nullptr;
	}

	if( bonesTransformations )
	{
		delete[] bonesTransformations;
		bonesTransformations = nullptr;
	}

	if( bonesWorldMatrices )
	{
		delete[] bonesWorldMatrices;
		bonesWorldMatrices = nullptr;
	}

	Core::Timer::DeleteTimer( this );
}

bool Model::AddMesh( Mesh* mesh )
{
	//Mesh is valid?
	if( mesh )
	{
		//First mesh?
		if( meshes.empty() )
			position = Math::Vector3( mesh->baseFrame._41, mesh->baseFrame._42, mesh->baseFrame._43 );

		meshes.push_back( mesh );

		int newMaxFrame = 0;

		//Check max frame of Mesh
		if( mesh->frameRotationCount > 0 && mesh->frameRotation )
			newMaxFrame = mesh->frameRotation[mesh->frameRotationCount-1].frame;

		if( mesh->framePositionCount > 0 && mesh->framePosition )
			newMaxFrame = mesh->framePosition[mesh->framePositionCount-1].frame;

		//Set new max Frame of Model
		if( newMaxFrame > maxFrame )
			maxFrame = newMaxFrame;

		//If bounding box its bigger than of Model, so set new bounding box based on Mesh
		if( size.x < mesh->boundingBox.max.x )
			size.x = mesh->boundingBox.max.x;

		if( size.x < mesh->boundingBox.max.z )
			size.x = mesh->boundingBox.max.z;

		if( size.y < mesh->boundingBox.max.y )
			size.y = mesh->boundingBox.max.y;

		return true;
	}

	return false;
}

void Model::ReorderMeshes()
{
	if( meshes.empty() )
		return;

	for( auto& mesh : meshes )
	{
		if( mesh->parentName[0] != 0 )
		{
			for( auto& parentMesh : meshes )
			{
				if( _strnicmp( mesh->parentName, parentMesh->name, 32 ) == 0 )
				{
					mesh->parent = parentMesh;
					break;
				}
			}
		}
	}

	//Clear List of Ordered Meshes
	orderedMeshes.clear();

	//Build a Reserve
	orderedMeshes.reserve( meshes.size() );

	//Put Root Meshes on Top of the List
	for( auto mesh : meshes )
	{
		if( mesh->parent == nullptr )
			orderedMeshes.push_back( mesh );
	}

	//Put Child Meshes in the List
	for( auto mesh : meshes )
	{
		if( mesh->parent == nullptr )
			continue;

		std::list<Mesh*> parentList;

		auto parentMesh = mesh->parent;
		while( parentMesh )
		{
			bool existsInList = false;

			for( auto orderedMesh : orderedMeshes )
			{
				if( orderedMesh == parentMesh )
				{
					existsInList = true;
					break;
				}
			}

			if( existsInList )
				break;

			parentList.push_front( parentMesh );
			parentMesh = parentMesh->parent;
		}

		for( auto mesh : parentList )
			orderedMeshes.push_back( mesh );

		orderedMeshes.push_back( mesh );
	}
}

void Model::SetParent( Model* modelParent, Mesh* meshParent )
{
	skeleton = modelParent;

	if( !meshes.empty() )
		for( auto& mesh : meshes )
			if( mesh )
				mesh->parent = meshParent;
}

Mesh* Model::GetMesh( std::string meshName )
{
	if( meshes.empty() )
		return nullptr;

	for( auto& mesh : meshes )
		if( mesh )
			if( _strcmpi( mesh->name, meshName.c_str() ) == 0 )
				return mesh;

	return nullptr;
}

int Model::GetOrderedMeshIndex( const std::string& meshName )
{
	if( orderedMeshes.empty() )
		return -1;

	for( size_t i = 0; i < orderedMeshes.size(); i++ )
	{
		if( orderedMeshes[i] )
			if( _strcmpi( orderedMeshes[i]->name, meshName.c_str() ) == 0 )
				return i;
	}

	return -1;
}

int Model::GetMeshIndex( const std::string& meshName )
{
	if( meshes.empty() )
		return -1;

	for( size_t i = 0; i < meshes.size(); i++ )
	{
		if( meshes[i] )
			if( _strcmpi( meshes[i]->name, meshName.c_str() ) == 0 )
				return i;
	}

	return -1;
}

std::vector<Mesh*> Model::GetMeshes( std::string meshName )
{
	std::vector<Mesh*> ret;

	for( auto& mesh : meshes )
		if( mesh )
			if( _strcmpi( mesh->name, meshName.c_str() ) == 0 )
				ret.push_back( mesh );

	return ret;
}

void Model::SetFrame( int frame_, IO::SMD::FrameInfo* frameInfo )
{
	if( frame_ >= 0 )
	{
		if( skeleton )
		{
			skeleton->Animate( frame_, rotation, frameInfo );

			//Update World Matrices
			if( !meshes.empty() )
				for( auto& mesh : meshes )
					if( mesh->parent )
						mesh->world = mesh->parent->world;
		}
		else
			Animate( frame_, rotation, frameInfo );
	}

	//Update Bounding Volumes not Forced
	UpdateBoundingVolumes();
}

void Model::Animate( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo* frameInfo )
{
	if( (forceUpdate == false) && lastAnimationFrame == frame_ && lastRotation == rotation_ )
		return;

	lastAnimationFrame = frame_;
	lastRotation = rotation_;

	//Animate Meshes
	int boneIndex = 0;
	for( const auto& mesh : orderedMeshes )
	{
		mesh->Animate( frame_, rotation_, frameInfo );

		if( bonesWorldMatrices )
			bonesWorldMatrices[boneIndex++] = mesh->world;
	}

	//Update Bones Transformations
	UpdateBonesTransformations();
}

void Model::SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ )
{
	if( position_ && rotation_ )
	{
		position.x = position_->x;
		position.y = position_->y;
		position.z = position_->z;

		rotation.x = rotation_->x;
		rotation.y = rotation_->y;
		rotation.z = rotation_->z;

		if( !meshes.empty() )
			for( const auto& mesh : meshes )
				if( mesh )
					mesh->SetPositionRotation( position_, rotation_ );
	}
}

void Model::UpdateBoundingVolumes( bool forceUpdate )
{
	if( boundingSphere.radius == 0.0f || forceUpdate == true )
	{
		//Reset Bounding Box
		BoundingBox boundingBox = BoundingBox();

		for( const auto& mesh : orderedMeshes )
		{
			if( mesh )
			{
				mesh->UpdateBoundingVolumes();
				boundingBox.Merge( mesh->worldBoundingBox );
			}
		}

		//Update Bounding Sphere Radius and Center
		boundingSphere.radius = std::max( { boundingBox.Size().x,  boundingBox.Size().y, boundingBox.Size().z } )* 0.5f;
		boundingSphere.center = boundingBox.Center();

		worldBoundingBox.min = boundingBox.min;
		worldBoundingBox.max = boundingBox.max;
	}
}

void Model::UpdateBonesTransformations()
{
	if( bonesTexture && bonesTransformations && bonesWorldMatrices )
	{
		Math::Matrix4::BulkTransposeTo3x4( bonesTransformations, (float*)bonesWorldMatrices, orderedMeshes.size() );

		//Set Bone Transformations Data to Texture Data
		if( bonesTexture )
		{
			if( bonesTexture->Lock() )
			{
				bonesTexture->SetPixelData( bonesTransformations, orderedMeshes.size()* sizeof( float )* 12 );
				bonesTexture->Unlock();
			}
		}
	}
}

void Model::SetDiffuseColor( Math::Color color )
{
	if( materialCollection )
	{
		if( materialCollection->materials )
		{
			for( int i = 0; i < materialCollection->materialsCount; i++ )
			{
				auto material = materialCollection->materials + i;

				material->customMaterial = true;
				material->diffuseColor = color;
			}
		}
	}
}

void Model::SetAddColor( Math::Color color )
{
	if( materialCollection )
	{
		if( materialCollection->materials )
		{
			for( int i = 0; i < materialCollection->materialsCount; i++ )
			{
				auto material = materialCollection->materials + i;

				material->customMaterial = true;
				material->addColor = color;
			}
		}
	}
}

void Model::Update( float timeElapsed )
{
	//Update Meshes
	for( const auto& mesh : meshes )
		if( mesh )
			mesh->Update( timeElapsed );

	//Auto Animate Model?
	if( autoAnimate )
	{
		frame += (int)(( maxFrame / 4800.0f )* timeElapsed);

		//Repeat Animation
		if( frame > maxFrame )
			frame = 160;
	}
}

bool Model::Render( IO::SMD::FrameInfo* frameInfo, ModelGroup* modelGroup )
{
	bool useCustomRenderer = (materialCollection) && materialCollection->materialType && !skeleton && customRenderer;

	//Apply Material Collection to old renderer
	if( customMaterialCollection )
		customMaterialCollection( materialCollection );

	//Check Frustum Culling
	if( !useCustomRenderer && useFrustumCulling && boundingSphere.radius != 0.0f && renderer->GetCamera()->Frustum().IsInside( boundingSphere.Transformed( position ) ) == Intersection::Outside )
		return false;

	//Set Model Frame
	SetFrame( frame, frameInfo );

	//Draw Bounding Sphere on Debug Mode
	if( renderer->IsDebugGeometry( DebugGeometry::DebugModel ) )
		renderer->DrawDebugSphere( boundingSphere.Transformed( position ) );

	//Drawable Meshes
	auto CanRenderMesh = [&]( const Mesh* p )
	{
		if( modelGroup == nullptr )
			return true;

		if( modelGroup->meshes.empty() )
			return true;
		else
		{
			for( const auto& meshGroup : modelGroup->meshes )
				if( p == meshGroup )
					return true;
		}

		return false;
	};

	//Render Meshes
	for( auto& mesh : meshes )
	{
		if( useCustomRenderer )
			customRenderer( mesh );
		else if( mesh->postRender )
			graphics->renderer->PushPostRenderMesh( mesh );
		else if( CanRenderMesh( mesh ) )
			mesh->Render();
	}

	return true;
}

bool Model::Load( std::string filePath, Model* skeleton_, bool temporaryTextures )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "rb" );

	if( file )
	{
		//Get Model Name
		filesystem::path f( filePath );
		std::string name = f.filename().string();
		name = name.substr( 0, name.size() - 4 );

		IO::SMD::Header	header = { 0 };
		IO::SMD::ObjectInfo* objectsInfo = nullptr;

		//Read Header
		fread( &header, sizeof(IO::SMD::Header), 1, file );

		//Identiy SMD Version
		if( _strcmpi( header.header, "SMD Model data Ver 0.64" ) == 0 )
			version = ModelVersion::SMDModelHeader64;

		//Read Objects
		objectsInfo = new IO::SMD::ObjectInfo[header.objectCount];
		fread( objectsInfo, sizeof( IO::SMD::ObjectInfo )* header.objectCount, 1, file );

		//Push Animations Frame Info
		for( int i = 0; i < header.frameCount; i++ )
			animationsFrameInfo.push_back( header.frames[i] );

		//Load Materials
		if( header.materialCount )
		{
			std::string materialListPath = filePath.substr( 0, filePath.size() - 4 ) + ".txt";
			filesystem::path p( materialListPath );

			materialCollection = new MaterialCollection( name );

			if( materialCollection )
			{
				if( version == ModelVersion::SMDModelHeader64 )
				{
					materialCollection->Build( file, skeleton_ ? true : false, filesystem::exists( p ) ? false : true, true, 3, temporaryTextures, true );
					materialCollection->Load( materialListPath, true );
					materialCollection->materialType = 0;
				}
				else
					materialCollection->Build( file, skeleton_ ? true : false, filesystem::exists( p ) ? false : true, false, 0, temporaryTextures, true );
			}
		}

		//Add Objects to Pattern
		for( int i = 0; i < header.objectCount; i++ )
		{
			//Set File Pointer
			if(  version != ModelVersion::SMDModelHeader64 )
				fseek( file, objectsInfo[i].filePointerToObject, SEEK_SET );

			Mesh* mesh = new Mesh();
			if( mesh )
			{
				mesh->modelParent = this;
				mesh->Build( file, skeleton_, version == ModelVersion::SMDModelHeader64 );
				AddMesh( mesh );
			}
		}

		//Link Objets Parent
		ReorderMeshes();

		//Set Model Skeleton
		skeleton = skeleton_;

		//Create Texture for bones fetch
		if( skeleton && graphics->useSoftwareSkinning == false )
		{
			skeleton->bonesTexture = graphics->GetTextureFactory()->CreateDynamicTexture( 384, 1 );
			skeleton->bonesWorldMatrices = new Math::Matrix4[skeleton->orderedMeshes.size()];
			skeleton->bonesTransformations = new float[skeleton->orderedMeshes.size()* 12];
		}

		//Delete Objects Info Pointer
		delete[] objectsInfo;
		fclose( file );

		return true;
	}

	return false;
}

}