#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "MeshPart.h"
#include "Renderer.h"
#include "VertexDeclaration.h"
#include "Material.h"
#include "Texture.h"
#include "Model.h"

namespace Delta3D::Graphics
{
Mesh::Mesh() : 
	GraphicsImpl(), 
	modelParent( nullptr ), 
	parent( nullptr ), 
	faces( nullptr ), 
	vertices( nullptr ), 
	texturesCoord( nullptr ), 
	frameRotation( nullptr ),
	framePosition( nullptr ), 
	frameScaling( nullptr ), 
	frameRotationCount( 0 ), 
	framePositionCount( 0 ), 
	frameScalingCount( 0 ),
	postRender( false ),
	loaded( false )
{
}

Mesh::Mesh( int verticesCount_, int facesCount_ ) : 
	GraphicsImpl(), 
	modelParent( nullptr ), 
	parent( nullptr ), 
	faces( nullptr ), 
	vertices( nullptr ), 
	texturesCoord( nullptr ),	
	frameRotation( nullptr ), 
	framePosition( nullptr ),
	frameScaling( nullptr ), 
	frameRotationCount( 0 ),
	framePositionCount( 0 ),
	frameScalingCount( 0 ),
	verticesCount( 0 ),
	facesCount( 0 ),
	texturesCount( 0 ),
	postRender( false ),
	loaded( false )
{
	vertices = new IO::SMD::Vertex[verticesCount_];
	faces = new IO::SMD::Face[facesCount_];
	texturesCoord = new IO::SMD::TextureLink[facesCount_* 2];
}

Mesh::~Mesh()
{
	auto DeleteArrayPointer = []( auto* p ) { delete[]p; p = nullptr; };

	DeleteArrayPointer( frameScaling );
	DeleteArrayPointer( framePosition );
	DeleteArrayPointer( frameRotation );
	DeleteArrayPointer( previousRotation );
	DeleteArrayPointer( texturesCoord );
	DeleteArrayPointer( faces );
	DeleteArrayPointer( vertices );

	for( auto& p : meshParts )
	{
		delete p.second;
		p.second = nullptr;
	}

	meshParts.clear();
	textureCoordsBuffer.clear();
}

int Mesh::AddVertex( int x, int y, int z )
{
	vertices[verticesCount].x = x;
	vertices[verticesCount].y = y;
	vertices[verticesCount].z = z;
	verticesCount++;

	//Save coordinate maximum
	float mx = abs(x) / 256.0f;
	float my = abs(y) / 256.0f;
	float mz = abs(z) / 256.0f;

	if( boundingBox.max.x < mx )
		boundingBox.max.x = mx;

	if( boundingBox.max.z < my )
		boundingBox.max.z = my;

	if( boundingBox.max.y < mz )
		boundingBox.max.y = mz;

	return verticesCount - 1;
}

int Mesh::AddFace( int a, int b, int c, int materialID, Math::Vector2 texA, Math::Vector2 texB, Math::Vector2 texC )
{
	faces[facesCount].v[0] = a;
	faces[facesCount].v[1] = b;
	faces[facesCount].v[2] = c;
	faces[facesCount].v[3] = materialID;
	faces[facesCount].textureLink = nullptr;

	if( texA != Math::Vector2::Null || texB != Math::Vector2::Null || texC != Math::Vector2::Null )
		AddTextureCoord( facesCount, texA, texB, texC );

	facesCount++;

	return facesCount - 1;
}

int Mesh::AddTextureCoord( int faceIndex, Math::Vector2 texA, Math::Vector2 texB, Math::Vector2 texC )
{
	//UV Setting
	texturesCoord[texturesCount].u[0] = texA.u;
	texturesCoord[texturesCount].v[0] = texA.v;
	texturesCoord[texturesCount].u[1] = texB.u;
	texturesCoord[texturesCount].v[1] = texB.v;
	texturesCoord[texturesCount].u[2] = texC.u;
	texturesCoord[texturesCount].v[2] = texC.v;
	texturesCoord[texturesCount].texHandle = nullptr;
	texturesCoord[texturesCount].next = nullptr;

	//Associate with the specified face and check if it's the first texture coordinate
	if( !faces[faceIndex].textureLink )
		faces[faceIndex].textureLink = &texturesCoord[texturesCount];
	else
	{
		//If there are already connected coordinates
		IO::SMD::TextureLink* textureLink = faces[faceIndex].textureLink;

		//Loop through maximum textures
		for( int i = 0; i < 8; i++ )
		{
			if( !textureLink->next )
			{
				//Finally, find the connected coordinates and link
				textureLink->next = &texturesCoord[texturesCount];
				break;
			}
			else
				textureLink = textureLink->next;
		}
	}

	texturesCount++;

	return texturesCount - 1;
}

void Mesh::ApplyRotationTransform( Math::Matrix4& out, Math::Matrix4* previousRotation_, IO::SMD::KeyRotation* frameRotation_, int frame_, IO::SMD::FrameInfo* frameInfo )
{
	if( frameRotation_[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = frameRotation_[i].frame;
		currentFrame = frameRotation_[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	if( frameInfo && currentFrame > (int)frameInfo->endFrame )
		out = previousRotation_[i];

	out = Math::Quaternion().Slerp( Math::Quaternion( frameRotation_[i + 1].x, frameRotation_[i + 1].y, frameRotation_[i + 1].z, frameRotation_[i + 1].w ), alpha ).ToMatrix();
	out = previousRotation_[i]* out;
}

void Mesh::ApplyScalingTransform( Math::Matrix4& out, IO::SMD::KeyScale* frameScaling_, int frame_ )
{
	if( frameScaling[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = frameScaling[i].frame;
		currentFrame = frameScaling[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	out._11 = (frameScaling[i].x / 256.0f) + ( ( (frameScaling[i + 1].x / 256.0f) - (frameScaling[i].x / 256.0f) )* alpha );
	out._22 = (frameScaling[i].y / 256.0f) + ( ( (frameScaling[i + 1].y / 256.0f) - (frameScaling[i].y / 256.0f) )* alpha );
	out._33 = (frameScaling[i].z / 256.0f) + ( ( (frameScaling[i + 1].z / 256.0f) - (frameScaling[i].z / 256.0f) )* alpha );
}

void Mesh::ApplyTranslationTransform( Math::Matrix4& out, IO::SMD::KeyPosition* framePosition_, int frame_ )
{
	if( framePosition_[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = framePosition_[i].frame;
		currentFrame = framePosition_[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	out._41 = framePosition_[i].x + ( ( framePosition_[i + 1].x - framePosition_[i].x )* alpha );
	out._42 = framePosition_[i].y + ( ( framePosition_[i + 1].y - framePosition_[i].y )* alpha );
	out._43 = framePosition_[i].z + ( ( framePosition_[i + 1].z - framePosition_[i].z )* alpha );
}

int Mesh::FindAnimationPosition( int frame )
{
	if( framesInfoCount > 0 )
	{
		for( int i = 0; i < framesInfoCount; i++ )
		{
			if( i > _countof( framesInfoPosition ) )
				break;

			if( framesInfoPosition[i].keyFrameCount > 0 && framesInfoPosition[i].startFrame <= frame && framesInfoPosition[i].endFrame > frame )
				return framesInfoPosition[i].keyFrameStartIndex;
		}
	}

	return -1;
}

int Mesh::FindAnimationRotation( int frame )
{
	if( framesInfoCount > 0 )
	{
		for( int i = 0; i < framesInfoCount; i++ )
		{
			if( i > _countof( framesInfoRotation ) )
				break;

			if( framesInfoRotation[i].keyFrameCount > 0 && framesInfoRotation[i].startFrame <= frame && framesInfoRotation[i].endFrame > frame )
				return framesInfoRotation[i].keyFrameStartIndex;
		}
	}

	return -1;
}

int Mesh::FindAnimationScaling( int frame )
{
	if( framesInfoCount > 0 )
	{
		for( int i = 0; i < framesInfoCount; i++ )
		{
			if( i > _countof( framesInfoScaling ) )
				break;

			if( framesInfoScaling[i].keyFrameCount > 0 && framesInfoScaling[i].startFrame <= frame && framesInfoScaling[i].endFrame > frame )
				return framesInfoScaling[i].keyFrameStartIndex;
		}
	}

	return -1;
}

void Mesh::Animate( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo* frameInfo )
{
	auto PTDegreeToRadians = []( const int deg ) { return (float)deg* D3DX_PI / 2048.0f; };

	int rotationIndex = framesInfoCount ? FindAnimationRotation( frame_ ) : 0;
	int scalingIndex = framesInfoCount ? FindAnimationScaling( frame_ ) : 0;
	int positionIndex = framesInfoCount ? FindAnimationPosition( frame_ ) : 0;

	Math::Matrix4 result = Math::Matrix4::Identity;

	//Animation was found?
	if( (!framesInfoCount && (frameRotationCount > 0 || framePositionCount > 0 || frameScalingCount > 0)) || (framesInfoCount && (rotationIndex >= 0 || positionIndex >= 0 || scalingIndex > 0) ) )
	{
		bool hasRotationAnimation = frameRotationCount > 0  && (framesInfoCount ? rotationIndex >= 0 : frameRotation[frameRotationCount - 1].frame > frame_);
		bool hasScalingAnimation = frameScalingCount > 0  && (framesInfoCount ? scalingIndex >= 0 : frameScaling[frameScalingCount - 1].frame > frame_);
		bool hasPositionAnimation = framePositionCount > 0 && (framesInfoCount ? positionIndex >= 0 : framePosition[framePositionCount - 1].frame > frame_);

		//Rotation
		if( hasRotationAnimation )
			ApplyRotationTransform( result, &previousRotation[rotationIndex], &frameRotation[rotationIndex], frame_, frameInfo );
		else
			result = baseRotation;

		//Scaling
		if( hasScalingAnimation )
		{
			Math::Matrix4 scaling = Math::Matrix4::Identity;
			ApplyScalingTransform( scaling, &frameScaling[scalingIndex], frame_ );

			result = result* scaling;
		}

		//Position
		if( hasPositionAnimation )
			ApplyTranslationTransform( result, &framePosition[positionIndex], frame_ );
		else
		{
			result._41 = float( basePosition.x ) / 256.0f;
			result._42 = float( basePosition.y ) / 256.0f;
			result._43 = float( basePosition.z ) / 256.0f;
		}
	}
	else if( parent )
		result = baseFrame* parent->baseFrameInverse;
	else
		result = baseFrame;

	//Multiply by Parent Animation Matrix
	if( parent )
		resultAnimation = result* parent->resultAnimation;
	else
		memcpy( &resultAnimation, &result, sizeof( Math::Matrix4 ) );

	//Scaling Matrix
	static Math::Matrix4 scaling;
	static bool scaleModel = false;

	if( modelParent->scaling != Math::Vector3( 1.0f, 1.0f, 1.0f ) )
	{
		scaling.Scale( modelParent->scaling );
		scaleModel = true;
	}
	else
		scaleModel = false;

	//Rotate Final World Matrix
	if( rotation_ == Math::Vector3Int::Null )
	{
		world = resultAnimation;
		
		if( scaleModel )
			world = world* scaling;
	}
	else
	{
		Math::Matrix4 rotateX, rotateY, rotateZ;

		rotateX.RotateX( PTDegreeToRadians( rotation_.x ) );
		rotateY.RotateZ( PTDegreeToRadians( rotation_.y ) );
		rotateZ.RotateY( PTDegreeToRadians( rotation_.z ) );

		local = rotateZ* rotateX* rotateY;
		world = resultAnimation* local;

		if( scaleModel )
			world = world* scaling;
	}
}

void Mesh::SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ )
{
	if( position_ && rotation_ )
	{
		position.x = position_->x;
		position.y = position_->y;
		position.z = position_->z;

		rotation.x = rotation_->x;
		rotation.y = rotation_->y;
		rotation.z = rotation_->z;

		translation = Math::Matrix4::Identity;
		translation.Translate( position );
	}
}

void Mesh::UpdateBoundingVolumes()
{
	if( modelParent == nullptr )
		return;

	if( verticesCount <= 0 )
		return;

	bool first = true;

	//Building Bounding Box Volume
	for( int i = 0; i < verticesCount; i++ )
	{
		Math::Matrix4 matrix;

		//Skinned Mesh
		if( (modelParent->skeleton) && skinnedVerticesIndex.size() )
		{
			const auto& bones = modelParent->skeleton->orderedMeshes;

			if( bones.size() )
				if( skinnedVerticesIndex.size() > 0 )
					if( auto boneIndex = skinnedVerticesIndex[i]; i >= 0 && i < skinnedVerticesIndex[i] )
						if( boneIndex >= 0 && boneIndex < (int)bones.size() )
							matrix = bones[skinnedVerticesIndex[i]]->world;
		}
		else
			matrix = world;

		Math::Vector3 transformedVector = matrix.FlippedYZ()* Math::Vector3(vertices[i].x / 256.0f, vertices[i].y / 256.0f, vertices[i].z / 256.0f);

		//Update World Bounding Box
		if( first )
		{
			worldBoundingBox.min = transformedVector;
			worldBoundingBox.max = transformedVector;

			first = false;
		}
		else
		{
			if( transformedVector.x < worldBoundingBox.min.x )
				worldBoundingBox.min.x = transformedVector.x;
			if( transformedVector.y < worldBoundingBox.min.y )
				worldBoundingBox.min.y = transformedVector.y;
			if( transformedVector.z < worldBoundingBox.min.z )
				worldBoundingBox.min.z = transformedVector.z;

			if( transformedVector.x > worldBoundingBox.max.x )
				worldBoundingBox.max.x = transformedVector.x;
			if( transformedVector.y > worldBoundingBox.max.y )
				worldBoundingBox.max.y = transformedVector.y;
			if( transformedVector.z > worldBoundingBox.max.z )
				worldBoundingBox.max.z = transformedVector.z;
		}
	}

	//Update Bounding Sphere
	boundingSphere.radius = std::max( { worldBoundingBox.Size().x,  worldBoundingBox.Size().y, worldBoundingBox.Size().z } )* 0.5f;
	boundingSphere.center = worldBoundingBox.Center();
}

inline int Mesh::FindSimilarVertex( IO::SMD::PackedVertex& packed, std::map<IO::SMD::PackedVertex, unsigned int>& cache, unsigned short& out )
{
	auto it = cache.find( packed );

	//Not Found
	if( it == cache.end() )
		return false;
	else
	{
		out = it->second;
		return true;
	}

	return false;
}

MeshRenderResult Mesh::CanRender()
{
	MeshRenderResult ret = MeshRenderResult::Undefined;

	for( const auto& p : meshParts )
	{
		ret = p.second->CanRender();

		if( ret == MeshRenderResult::NotRender )
			break;
	}

	return ret;
}

inline void Mesh::Skinning()
{
	if( modelParent == nullptr )
		return;

	if( modelParent->skeleton == nullptr )
		return;

	if( vertexPositionBuffer && !vertexData.empty() )
	{
		float* vertexPositionData = (float*)vertexPositionBuffer->Lock();
		const auto& bones = modelParent->skeleton->orderedMeshes;

		if( vertexPositionData && bones.size() )
		{
			for( const auto& v : vertexData )
			{
				Math::Vector3 out = bones[v.second]->world.FlippedYZ()* v.first;

				*(vertexPositionData++) = out.x;
				*(vertexPositionData++) = out.y;
				*(vertexPositionData++) = out.z;
			}

			vertexPositionBuffer->Unlock();
		}
	}
}

void Mesh::Update( float timeElapsed )
{
}

int Mesh::Render()
{
	if( vertexPositionBuffer && vertexNormalBuffer && vertexColorBuffer && !textureCoordsBuffer.empty() && modelParent )
	{
		bool skinnedMesh = false;

		//Skinned Mesh
		if( modelParent->skeleton && skinnedVerticesIndex.size() )
			skinnedMesh = true;

		//Frustum Culling
		if( renderer->GetCamera()->Frustum().IsInside( worldBoundingBox.Transformed( translation ) ) == Intersection::Outside )
			return false;

		//Apply Camera Transformations
		renderer->ApplyTransformations();
		
		//Draw Bounding Box on Debug Mode
		if( renderer->IsDebugGeometry( DebugGeometry::DebugMesh ) )
			renderer->DrawDebugAABB( worldBoundingBox.Transformed( translation ) );

		//Set Vertex Position Buffer to Stream
		if( FAILED( device->SetStreamSource( 0, vertexPositionBuffer->Get(), 0, vertexPositionBuffer->ElementSize() ) ) )
			return false;

		//Set Vertex Normals Buffer to Stream
		if( FAILED( device->SetStreamSource( 1, vertexNormalBuffer->Get(), 0, vertexNormalBuffer->ElementSize() ) ) )
			return false;

		//Set Vertex Color Buffer to Stream
		if( FAILED( device->SetStreamSource( 2, vertexColorBuffer->Get(), 0, vertexColorBuffer->ElementSize() ) ) )
			return false;

		//Set Vertex Blend Indices Buffer to Stream
		if( skinnedMesh && vertexBlendIndicesBuffer )
			if( FAILED( device->SetStreamSource( 3, vertexBlendIndicesBuffer->Get(), 0, vertexBlendIndicesBuffer->ElementSize() ) ) )
				return false;

		//Set Texture Coordinates Buffer to Stream
		for( size_t i = 0; i < textureCoordsBuffer.size(); i++ )
			if( FAILED( device->SetStreamSource( skinnedMesh ? 4 + i: 3 + i, textureCoordsBuffer[i]->Get(), 0, textureCoordsBuffer[i]->ElementSize() ) ) )
				return false;

		//Scaling Matrix
		static Math::Matrix4 scalingMesh;
		static bool scaleMesh = false;

		if( modelParent->scaling != Math::Vector3( 1.0f, 1.0f, 1.0f ) )
		{
			scalingMesh.Scale( modelParent->scaling );
			scaleMesh = true;
		}
		else
			scaleMesh = false;

		//Draw Mesh Parts (per material)
		for( const auto& p : meshParts )
		{
			if( p.second )
			{
				//Push World Matrix
				renderer->PushWorldMatrix( skinnedMesh ? translation : world.FlippedYZ()* translation );

				//Push Scaling Matrix
				if( scaleMesh )
					renderer->PushWorldMatrix( scalingMesh );

				//Update Bones Transformations and set it to Texture Data
				if( skinnedMesh )
				{
					//Software Skinning
					if( graphics->useSoftwareSkinning )
						Skinning();
					//Hardware Skinning
					else if( (modelParent->skeleton) && modelParent->skeleton->bonesTexture )
					{
						//Update Bones Texture Fetch
						if( p.second->material->GetEffect() )
						{
							p.second->material->GetEffect()->SetTexture( "BonesMap", modelParent->skeleton->bonesTexture );
						}
					}
				}

				//Render It!
				p.second->Render( vertexPositionBuffer, skinnedMesh );

				//Pop Scaling Matrix
				if( scaleMesh )
					renderer->PopWorldMatrix();

				renderer->PopWorldMatrix();
			}
		}

		return true;
	}

	return false;
}

bool Mesh::Build( FILE* file, Model* skeleton, bool readVertexColor )
{
	if( file )
	{
		std::vector<IO::SMD::VertexColor> verticesColor;
		bool hasVertexColor = false;

		fread( &header, sizeof( texturesCoord ) + offsetof( Mesh, texturesCoord ) - offsetof( Mesh, header ), 1, file );
		fseek( file, 4 + sizeof( IO::SMD::Vertex ), SEEK_CUR );

		Math::Vector3 min, max;

		int i;
		fread( &i, sizeof( int ), 1, file );
		max.z = (float)i / 256.0f;

		fread( &i, sizeof( int ), 1, file );
		min.z = (float)i / 256.0f;

		fread( &i, sizeof( int ), 1, file );
		max.y = (float)i / 256.0f;

		fread( &i, sizeof( int ), 1, file );
		min.y = (float)i / 256.0f;

		fread( &i, sizeof( int ), 1, file );
		max.x = (float)i / 256.0f;

		fread( &i, sizeof( int ), 1, file );
		max.x = (float)i / 256.0f;

		//Create Bounding Box
		boundingBox = Math::BoundingBox( min, max );

		fseek( file, 16, SEEK_CUR );
		fread( &verticesCount, sizeof( texturesCount ) + offsetof( Mesh, texturesCount ) - offsetof( Mesh, verticesCount ), 1, file );
		fseek( file, 8, SEEK_CUR );
		fread( &position, sizeof( Math::Vector3 ), 1, file );
		fseek( file, 12, SEEK_CUR );
		fread( &rotation, sizeof( Math::Vector3Int ), 1, file );
		fseek( file, 32, SEEK_CUR );

		fread( name, 32, 1, file );
		fread( &parentName, 32, 1, file );

		fseek( file, 4, SEEK_CUR );

		auto ConvertMatrixToFloat = []( const int b[4][4] )
		{
			Math::Matrix4 m;

			for( int i = 0; i < 4; i++ )
				for( int j = 0; j < 4; j++ )
					m.m[i][j] = b[i][j] / 256.0f;

			return m;
		};

		int m[4][4] = { 0 };
		fread( &m, sizeof( m ), 1, file );
		baseFrame = ConvertMatrixToFloat( m );

		fread( &m, sizeof( m ), 1, file );
		baseFrameInverse = ConvertMatrixToFloat( m );

		fread( &resultAnimation, sizeof( m ), 1, file );

		fread( &m, sizeof( m ), 1, file );
		baseRotation = ConvertMatrixToFloat( m );

		fread( &m, sizeof( m ), 1, file );
		world = ConvertMatrixToFloat( m );

		fread( &m, sizeof( m ), 1, file );
		local = ConvertMatrixToFloat( m );

		//Fix Position
		position = Math::Vector3( baseFrame._41, baseFrame._42, baseFrame._43 );

		fread( &lastFrame, sizeof( int ), 1, file );
		fseek( file, 28, SEEK_CUR );
		fread( &basePosition, sizeof( framesInfoCount ) + offsetof( Mesh, framesInfoCount ) - offsetof( Mesh, basePosition ), 1, file );

		IO::SMD::TextureLink* psTextureTmp = texturesCoord;

		//Allocate Vertexs
		vertices = new IO::SMD::Vertex[verticesCount];
		fread( vertices, sizeof(IO::SMD::Vertex)* verticesCount, 1, file );

		//Allocate Faces
		faces = new IO::SMD::Face[facesCount];
		fread( faces, sizeof(IO::SMD::Face)* facesCount, 1, file );

		//Allocate Textures Link
		texturesCoord = new IO::SMD::TextureLink[texturesCount];
		fread( texturesCoord, sizeof(IO::SMD::TextureLink)* texturesCount, 1, file );

		//Allocate Animations Rotation, Position and Scale
		frameRotation = new IO::SMD::KeyRotation[frameRotationCount];
		fread( frameRotation, sizeof(IO::SMD::KeyRotation)* frameRotationCount, 1, file );

		framePosition = new IO::SMD::KeyPosition[framePositionCount];
		fread( framePosition, sizeof(IO::SMD::KeyPosition)* framePositionCount, 1, file );

		frameScaling = new IO::SMD::KeyScale[frameScalingCount];
		fread( frameScaling, sizeof(IO::SMD::KeyScale)* frameScalingCount, 1, file );

		//Allocate Previous Rotation
		previousRotation = new Math::Matrix4[frameRotationCount];
		fread( previousRotation, sizeof(Math::Matrix4)* frameRotationCount, 1, file );

		//Restore Texture Link
		int iSubTextureLinkID = texturesCoord - psTextureTmp;

		//Read Textures Link
		for( int i = 0; i < texturesCount; i++ )
		{
			if( texturesCoord[i].next )
			{
				iSubTextureLinkID = texturesCoord[i].next - psTextureTmp;
				texturesCoord[i].next = texturesCoord + iSubTextureLinkID;
			}
		}

		//Read Face Textures Link
		for( int i = 0; i < facesCount; i++ )
		{
			if( faces[i].textureLink )
			{
				iSubTextureLinkID = faces[i].textureLink - psTextureTmp;
				faces[i].textureLink = texturesCoord + iSubTextureLinkID;
			}
		}

		skinnedVerticesIndex.clear();
		skinnedVerticesIndex.reserve( verticesCount );

		//Skinned Object? Generate Bones List and Skinned Indices
		if( skeleton )
		{
			char* pszBuff = new char[verticesCount* 32];
			fread( pszBuff, verticesCount* 32, 1, file );

			for( int i = 0; i < verticesCount; i++ )
			{
				int boneIndex = skeleton->GetOrderedMeshIndex( pszBuff + i* 32 );

				if( boneIndex != -1 )
					skinnedVerticesIndex.push_back( boneIndex );
			}

			delete[] pszBuff;
		}

		//Read Vertices Colors
		if( readVertexColor )
		{
			//Check if have Vertices Colors
			fread( &hasVertexColor, sizeof( bool ), 1, file );

			if( hasVertexColor )
			{
				for( int i = 0; i < verticesCount; i++ )
				{
					float r, g, b;
					fread( &r, sizeof( float ), 1, file );
					fread( &g, sizeof( float ), 1, file );
					fread( &b, sizeof( float ), 1, file );

					verticesColor.push_back( IO::SMD::VertexColor{ r,g,b } );
				}
			}
		}

		//Do not build Mesh if it's a Skeleton Mesh
		if( facesCount > 0 && _strnicmp( name, "Bip01", 5 ) != 0 )
		{
			//Skinned Mesh Flag
			bool skinnedMesh = skeleton && skinnedVerticesIndex.size() > 0;

			//Create Texture to store Bones Matrices
			if( skinnedMesh && !graphics->useSoftwareSkinning )
				vertexBlendIndicesBuffer = graphics->CreateStaticVertexBuffer( sizeof( float ), facesCount* 3 );

			//Create Vertex Buffer and Texture Coordinates Buffer
			if( skinnedMesh && graphics->useSoftwareSkinning )
				vertexPositionBuffer = graphics->CreateDynamicVertexBuffer( sizeof( Math::Vector3 ), facesCount* 3 );
			else
				vertexPositionBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount* 3 );

			vertexNormalBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount* 3 );
			vertexColorBuffer = graphics->CreateStaticVertexBuffer( sizeof( D3DCOLOR ), facesCount* 3 );

			//Vertex Buffer and Texture Coordinates Buffer was created successfully?
			if( vertexPositionBuffer && vertexNormalBuffer && vertexColorBuffer )
			{
				float* vertexPositionData = (float*)vertexPositionBuffer->Lock();
				float* vertexNormalData = (float*)vertexNormalBuffer->Lock();
				float* vertexColorData = (float*)vertexColorBuffer->Lock();		
				float* textureCoordData[8] = { 0 };
				float* vertexBlendIndicesData = nullptr;

				//Skinned Mesh
				if( vertexBlendIndicesBuffer )
					vertexBlendIndicesData = (float*)vertexBlendIndicesBuffer->Lock();

				//Was locked successfully?
				if( vertexPositionData && vertexNormalData && vertexColorData )
				{
					unsigned int curVertexIndex = 0;
					std::map<IO::SMD::PackedVertex, unsigned int> verticesIndex;

					//Build Texture Coordinates Before proceed
					for( int i = 0; i < facesCount; i++ )
					{
						IO::SMD::Face* face = faces + i;

						//First Face?
						if( textureCoordsBuffer.empty() )
						{
							auto texCoord = face->textureLink;

							while( texCoord )
							{
								if( textureCoordsBuffer.size() >= 8 )
									break;

								auto textureCoordBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector2 ), facesCount* 3 );
								if( textureCoordBuffer )
								{
									textureCoordData[textureCoordsBuffer.size()] = (float*)textureCoordBuffer->Lock();
									textureCoordsBuffer.push_back( textureCoordBuffer );
								}

								texCoord = texCoord->next;
							}
						}
						else
							break;
					}

					//Loop through Mesh Faces
					for( int i = 0; i < facesCount; i++ )
					{
						IO::SMD::Face* face = faces + i;
						Material* material = (modelParent) && modelParent->materialCollection ? &modelParent->materialCollection->materials[face->v[3]] : nullptr;

						//Material not found or face without textures coordinates? Do nothing!
						if( !material || !face->textureLink )
							continue;

						MeshPart* meshPart = nullptr;

						//Find Mesh Part by current face Material
						if( meshParts.find( material ) != meshParts.end() )
							meshPart = meshParts[material];
						else
						{
							//Not Found? So create it and put on mesh parts vector
							meshPart = new MeshPart();
							meshPart->material = material;
							meshPart->mesh = this;
							meshParts[material] = meshPart;
						}
						
						//Loop through Face Vertices (A,B,C)
						for( int j = 0; j < 3; j++ )
						{
							//Temporary Variables used to find Vertex
							Math::Vector3 position( vertices[face->v[j]].x / 256.0f, vertices[face->v[j]].y / 256.0f, vertices[face->v[j]].z / 256.0f );
							Math::Vector3 normal( vertices[face->v[j]].nx / 256.0f, vertices[face->v[j]].ny / 256.0f, vertices[face->v[j]].nz / 256.0f );
							Math::Vector2 uv1( face->textureLink->u[j], face->textureLink->v[j] );
							D3DCOLOR vertexColor = hasVertexColor ? Math::Color( verticesColor[face->v[j]].r, verticesColor[face->v[j]].g, verticesColor[face->v[j]].b ).ToUInt() : -1;

							IO::SMD::PackedVertex packedVertex = { 0 };
							packedVertex.position = position;
							packedVertex.color = vertexColor;
							packedVertex.boneIndex = skinnedMesh ? skinnedVerticesIndex[face->v[j]] : -1;
							
							auto texCoord = face->textureLink;

							//Fill Texture Coordinates Buffer Data
							for( int k = 0; k < _countof( textureCoordData ); k++ )
							{
								if( texCoord == nullptr )
									break;

								if( textureCoordData[k] )
								{
									packedVertex.uv[k].u = texCoord->u[j];
									packedVertex.uv[k].v = texCoord->v[j];

									*(textureCoordData[k]++) = texCoord->u[j];
									*(textureCoordData[k]++) = texCoord->v[j];
								}

								texCoord = texCoord->next;
							}

							//Find Similar Vertex to Build Index Buffer
							unsigned short index = 0;
							bool found = FindSimilarVertex( packedVertex, verticesIndex, index );

							//Found? Push vertex index found, else, current vertex index
							if( found )
								meshPart->indices.push_back( index );
							else
								meshPart->indices.push_back( verticesIndex[packedVertex] = curVertexIndex );

							//Fill Vertex Buffer Data
							*(vertexPositionData++) = position.x;
							*(vertexPositionData++) = position.y;
							*(vertexPositionData++) = position.z;
							*(vertexNormalData++) = normal.x;
							*(vertexNormalData++) = normal.y;
							*(vertexNormalData++) = normal.z;
							*((DWORD*)(vertexColorData++)) = vertexColor;

							//Fill structure for Software Skinning
							if( graphics->useSoftwareSkinning )
								vertexData.push_back( std::make_pair( Math::Vector3( position.x, position.y, position.z ), skinnedVerticesIndex[face->v[j]] ) );

							//Skinned Mesh? So push the bone index
							if( vertexBlendIndicesData )
								*(vertexBlendIndicesData++) = (float)skinnedVerticesIndex[face->v[j]];

							curVertexIndex++;
						}	
					}

					vertexPositionBuffer->Unlock();
					vertexNormalBuffer->Unlock();
					vertexColorBuffer->Unlock();

					if( vertexBlendIndicesBuffer )
						vertexBlendIndicesBuffer->Unlock();

					for( auto& texCoordBuffer : textureCoordsBuffer )
						texCoordBuffer->Unlock();
				}
			}
		}

		//Mesh loaded
		loaded = true;

		return true;
	}

	return false;
}
}