#pragma once

#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Frustum.h"

namespace Delta3D::Graphics
{
class Camera
{
public:
	//! Default Constructor for Camera.
	Camera() : 
		maxPitch( D3DXToRadian( 89.0f ) ), 
		translation( Math::Vector3::Null ), 
		maxVelocity( 1.0f ), 
		look( 0.0f, 0.0f, 1.0f ), 
		fov( 45.0f ), 
		aspectRatio( 0.0f ), 
		nearClip( 0.1f ), 
		farClip( 1000.0f ), 
		up( 0.0f, 1.0f, 0.0f ), 
		yaw( 0.f ), 
		pitch( 0.f )
	{
	}

	//! Set Camera Position.
	void SetPosition( const Math::Vector3& eye_, const Math::Vector3& lookAt_, const Math::Vector3& up_ = Math::Vector3( 0.f, 1.f, 0.f ), bool forceUpdateView = true );

	//! Set Camera Eye.
	void SetEye( const Math::Vector3& eye_ ) { eye = eye_; Update(); }

	//! Set Camera View.
	void SetView( Math::Matrix4 view_ ) { view = view_; }

	//! Set Camera Look At.
	void SetLookAt( const Math::Vector3& lookAt_ ) { lookAt = lookAt_; look = ( lookAt - eye ).Normalize(); Update(); }

	//! Set Camera Projection
	void SetProjection( float fov_, float aspectRatio_, float nearClip_ = 0.01f, float farClip_ = 1000.0f ) { fov = fov_; aspectRatio = aspectRatio_; nearClip = nearClip_; farClip = farClip_; UpdateProjection(); }

	//! Move Forward of Camera.
	void MoveForward( float units );

	//! Set Velocity.
	void SetVelocity( const Math::Vector3& velocity_ ) { velocity = velocity_; }

	//! Strafe Camera.
	void Strafe( float units );

	//! Move Up of Camera.
	void MoveUp( float units );

	//! Rotate Yaw of Camera.
	void Yaw( float radians );

	//! Rotate Pitch of Camera.
	void Pitch( float radians );

	//! Rotate Roll of Camera.
	void Roll( float radians );

	//! Update Camera.
	void Update();

	//! Translate Camera.
	void Translate( const Math::Vector3& translation_ ) { translation = translation_; }

	//! Camera Getters.
	Math::Vector3 Eye() const { return eye + translation; }
	Math::Vector3 LookAt() const { return lookAt + translation; }
	const Math::Vector3& Up() const { return up; }

	//! Get Camera Direction.
	Math::Vector3 Direction() const { return ( LookAt() - Eye() ).Normalize(); }

	//! Get and Setter Camera View Matrix.
	const Math::Matrix4& View() const { return view; }

	//! Get Camera Projection Matrix.
	const Math::Matrix4& Projection() const { return projection; }

	//! Get Camera View and Projection combined Matrix.
	const Math::Matrix4& ViewProjection() { return View() * Projection(); }

	//! Get Camera Frustum.
	const Math::Frustum& Frustum() const{ return frustum; }

	//! Camera Projection Getters.
	const float Fov() const { return fov; }
	const float AspectRatio() const { return aspectRatio; }
	const float NearClip() const { return nearClip; }
	const float FarClip() const { return farClip; }
private:
	//! Update Camera Projection Matrix.
	void UpdateProjection();

	//! Update Camera View Matrix.
	void UpdateView();
private:
	Math::Vector3 eye;	//!< Eye Position
	Math::Vector3 lookAt;	//!< Look At Position
	Math::Vector3 up;	//!< Up Vector
	Math::Vector3 right;	//!< Right Vector
	Math::Vector3 look;	//!< Look Vector
	Math::Vector3 velocity;	//!< Velocity Vector
	Math::Vector3 translation;	//!< Camera Translation

	float fov;	//!< Camera FOV
	float aspectRatio;	//!< Camera Aspect Ratio
	float nearClip;	//!< Near Clip
	float farClip;	//!< Far Clip

	float yaw;	//!< Yaw
	float pitch;	//!< Pitch
	float maxPitch;	//!< Max Pitch
	float maxVelocity;	//!< Max Velocity

	Math::Matrix4 view;	//!< Camera View Matrix
	Math::Matrix4 projection;	//!< Camera Projection Matrix
	Math::Frustum frustum;	//!< Camera Frustum
};
}