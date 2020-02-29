#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "MeshPart.h"
#include "Mesh.h"

namespace Delta3D::Graphics
{

Renderer::Renderer( Graphics* graphics_ ) : 
	worldTime( 0.0f ), 
	graphics( graphics_ ), 
	sphereMesh( nullptr ), 
	worldMatrixd3dStack( nullptr ), 
	debugGeometry( DebugGeometry::DebugNone ), 
	fogEnd( 1500.0f ), 
	maxLights( 64 ), 
	applyDistortionFlag( false ),
	renderReflectionMap( false ), 
	reflectionCamera( nullptr ), 
	reflectionRenderTarget( nullptr )
{
	reflectionCamera = new Camera();

	D3DXCreateMatrixStack( 0, &worldMatrixd3dStack );
	worldMatrixd3dStack->Push();
	worldMatrixd3dStack->LoadIdentity();

	lights.reserve( 64 );
}

Renderer::~Renderer()
{
	if( reflectionCamera )
	{
		delete reflectionCamera;
		reflectionCamera = nullptr;
	}

	if( worldMatrixd3dStack )
	{
		worldMatrixd3dStack->Release();
		worldMatrixd3dStack = nullptr;
	}

	if( sphereMesh )
	{
		sphereMesh->Release();
		sphereMesh = nullptr;
	}
}

bool Renderer::Begin()
{
	//Begin Scene
	if( !graphics->Begin() )
		return false;

	//Clear Scene
	graphics->Clear();

	//Push Identity Matrix for World Transform
	PushWorldMatrix( Math::Matrix4::Identity );

	//Fire Rendering Event
	FireEvent( RendererEvents::BeginRendering );

	return true;
}

void Renderer::RenderReflectionMap()
{
	if( reflectionRenderTarget == nullptr )
		reflectionRenderTarget = graphics->GetRenderTargetFactory()->Create();

	if( reflectionCamera && reflectionRenderTarget )
	{
		Vector3 reflCameraPosition = GetCamera()->Eye();
		reflCameraPosition.y = -reflCameraPosition.y + 100.f* 2;

		Vector3 reflTargetPos = GetCamera()->LookAt();
		reflTargetPos.y = -reflTargetPos.y + 100.f* 2;

		reflectionCamera->SetProjection( GetCamera()->Fov(), GetCamera()->AspectRatio(), GetCamera()->NearClip(), GetCamera()->FarClip() );
		reflectionCamera->SetPosition( reflCameraPosition, reflTargetPos );

		PushCamera( reflectionCamera );

		if( graphics->SetRenderTarget( reflectionRenderTarget ) )
		{
			graphics->Clear();
			isRenderingReflectionMap = true;
			Render();
			isRenderingReflectionMap = false;
			graphics->UnsetRenderTarget();
		}

		PopCamera();
	}
}

void Renderer::Render()
{
	ApplyTransformations();

	//Draw Camera Frustum
	if( IsDebugGeometry( DebugGeometry::DebugCameraFrustum ) )
		DrawDebugFrustum( GetCamera()->Frustum() );

	//Fire Rendering Event
	FireEvent( RendererEvents::Rendering3D );
}

void Renderer::RenderParticles()
{
	//Apply Effect Transformations
	graphics->GetEffectRenderer()->SetProjectionMatrix( *(Effekseer::Matrix44*)&GetCamera()->Projection() );
	graphics->GetEffectRenderer()->SetCameraMatrix( *(Effekseer::Matrix44*)&GetCamera()->View() );

	//Draw Effects
	if( graphics->GetEffectRenderer()->BeginRendering() )
	{
		graphics->GetEffectManager()->Draw();
		graphics->GetEffectRenderer()->EndRendering();
	}
}

void Renderer::End()
{
	PopWorldMatrix();

	lights.clear();
	postRenderMeshes.clear();

	FireEvent( RendererEvents::EndRendering );

	graphics->End();
}

void Renderer::Run()
{
	if( Begin() )
	{
		//Render reflection Map?
		if( renderReflectionMap )
			RenderReflectionMap();

		//Render 3D
		Render();

		//Post Render Meshes
		for( const auto& mesh : postRenderMeshes )
			if( mesh )
				mesh->Render();

		//Fire Rendering 2D Event
		FireEvent( RendererEvents::Rendering2D );

		End();
	}
}

void Renderer::Update( float elapsedTime )
{
	//Update Effect Manager
	graphics->effectManager->Update( elapsedTime / 10.0f );

	//Update World Time
	worldTime += elapsedTime / 1000.0f;

	if( worldTime >= 100.0f )
		worldTime = 0.0f;
}

void Renderer::PushLight( const Light& light )
{
	if( lights.size() < maxLights )
		lights.push_back( RenderLight{ light.position, light.range, light.color } );
}

void Renderer::PushLight( const RenderLight& light )
{
	if( lights.size() < maxLights )
		lights.push_back( light );
}

void Renderer::PushWorldMatrix( const Math::Matrix4& world )
{
	//D3DX Matrix Stack
	{
		worldMatrixd3dStack->Push();
		worldMatrixd3dStack->MultMatrixLocal( &world.Get() );
	}

	//Set new World Matrix to Device
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, worldMatrixd3dStack->GetTop() );
}

void Renderer::PopWorldMatrix()
{
	//D3DX Matrix Stack
	worldMatrixd3dStack->Pop();

	//Set new World Matrix to Device
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, worldMatrixd3dStack->GetTop() );
}

const D3DXMATRIX& Renderer::WorldMatrix() const
{
	return *worldMatrixd3dStack->GetTop();
}

bool Renderer::Prepare( std::shared_ptr<Shader> effect )
{
	if( effect )
	{
		//Prepare Lights
		if( !lights.empty() )
			effect->SetData( "Lights", (const char*)&lights[0], lights.size()* sizeof( RenderLight ) );
		
		effect->SetInt( "NumActiveLights", lights.size() );
		effect->SetBool( "ApplyDistortionFlag", applyDistortionFlag );
		effect->SetFloat( "WorldTime", worldTime );
		effect->SetFloat( "FogEnd", fogEnd );
		effect->SetMatrix( "World", *worldMatrixd3dStack->GetTop() );
		effect->SetMatrix( "View", GetCamera()->View().Get() );
		effect->SetMatrix( "Projection", GetCamera()->Projection().Get() );
		effect->SetFloatArray( "CameraEye", GetCamera()->Eye().Get(), 3 );
		effect->SetFloatArray( "CameraLookAt", GetCamera()->LookAt().Get(), 3 );
		effect->SetFloatArray( "CameraUp", GetCamera()->Up().Get(), 3 );
		effect->SetFloatArray( "CameraDirection", GetCamera()->Direction().Get(), 3 );
		effect->SetFloatArray( "AmbientColor", &ambientColor.r, 4 );

		if( renderReflectionMap && reflectionCamera && reflectionRenderTarget )
		{
			effect->SetMatrix( "ReflectionView", reflectionCamera->View().Get() );
			effect->SetTexture( "ReflectionTexture", reflectionRenderTarget->Texture() );
		}

		return true;
	}

	return false;
}

void Renderer::PushCamera( Camera* camera_ )
{
	cameraStack.push( camera_ );
}

void Renderer::PopCamera()
{
	cameraStack.pop();
}

void Renderer::ApplyTransformations()
{
	graphics->GetDevice()->SetViewport( &viewport.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_VIEW, &GetCamera()->View().Get() );
	graphics->GetDevice()->SetTransform( D3DTS_PROJECTION, &GetCamera()->Projection().Get() );
}

void Renderer::ResetTransformations()
{
	graphics->GetDevice()->SetViewport( &viewport.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_VIEW, &Matrix4::Identity.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, &Matrix4::Identity.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_PROJECTION, &GetCamera()->Projection().Get() );
}

void Renderer::DrawDebugFrustum( const Math::Frustum& frustum )
{
	//Near Plane
	{
		Math::Quad q;
		q.p1 = frustum.vertices[0];
		q.p2 = frustum.vertices[2];
		q.p3 = frustum.vertices[3];
		q.p4 = frustum.vertices[1];
		DrawDebugQuad( q );
	}

	//Far Plane
	{
		Math::Quad q;
		q.p1 = frustum.vertices[4];
		q.p2 = frustum.vertices[6];
		q.p3 = frustum.vertices[7];
		q.p4 = frustum.vertices[5];
		DrawDebugQuad( q );
	}

	//Attach Corners
	DrawDebugLine( frustum.vertices[0], frustum.vertices[4] );
	DrawDebugLine( frustum.vertices[2], frustum.vertices[6] );
	DrawDebugLine( frustum.vertices[3], frustum.vertices[7] );
	DrawDebugLine( frustum.vertices[1], frustum.vertices[5] );
}

void Renderer::DrawDebugAABB( const Math::BoundingBox& box )
{
	Vector3 corners[8];
	box.ComputeCorners( corners );

	PushWorldMatrix( Math::Matrix4::Identity );

	DrawDebugQuad( Quad( corners[0], corners[4], corners[7], corners[3] ) ); //Left
	DrawDebugQuad( Quad( corners[5], corners[1], corners[2], corners[6] ) ); //Right
	DrawDebugQuad( Quad( corners[7], corners[6], corners[2], corners[3] ) ); //Bottom
	DrawDebugQuad( Quad( corners[0], corners[1], corners[5], corners[4] ) ); //Top
	DrawDebugQuad( Quad( corners[4], corners[5], corners[6], corners[7] ) ); //Front
	DrawDebugQuad( Quad( corners[1], corners[0], corners[3], corners[2] ) ); //Back

	PopWorldMatrix();
}

void Renderer::DrawDebugSphere( const Math::Sphere& sphere )
{
	if( !sphereMesh )
		D3DXCreateSphere( graphics->GetDevice(), 1.0f, 15, 15, &sphereMesh, nullptr );

	//Apply Transformations
	ApplyTransformations();

	//Matrix Transformed
	Math::Matrix4 translation;
	translation.Translate( sphere.center );

	Math::Matrix4 scaling;
	scaling.Scale( sphere.radius );

	//Push World Matrix
	PushWorldMatrix( scaling* translation );

	//Draw Sphere
	graphics->SetFillMode( FillMode::Wireframe );
	sphereMesh->DrawSubset( 0 );
	graphics->SetFillMode( FillMode::Solid );

	//Reset Scene Transformations and restore World Matrix
	ResetTransformations();
	PopWorldMatrix();
}

void Renderer::DrawDebugQuad( const Math::Quad& quad )
{
	DrawDebugLine( quad.p1, quad.p2 );
	DrawDebugLine( quad.p2, quad.p3 );
	DrawDebugLine( quad.p3, quad.p4 );
	DrawDebugLine( quad.p4, quad.p1 );
}

void Renderer::DrawDebugLine( const Math::Vector3& point1, const Math::Vector3& point2 )
{
	struct Vertex { Vector3 pos; int color; };

	Vertex vertex[2];
	vertex[0] = { Vector3( point1.x, point1.y, point1.z ), -1 };
	vertex[1] = { Vector3( point2.x, point2.y, point2.z ), -1 };

	graphics->GetDevice()->SetTexture( 0, nullptr );
	graphics->GetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	graphics->GetDevice()->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, vertex, sizeof( Vertex ) );
}

}