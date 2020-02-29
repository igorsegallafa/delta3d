#include "PrecompiledHeader.h"
#include "Camera.h"

namespace Delta3D::Graphics
{
void Camera::SetPosition( const Math::Vector3& eye_, const Math::Vector3& lookAt_, const Math::Vector3& up_, bool forceUpdateView )
{
	eye = eye_; 
	lookAt = lookAt_;
	up = up_;

	if( forceUpdateView )
	{
		UpdateView();
	}
	else
	{
		look = (lookAt - eye).Normalize(); 
		Update();
	}
}

void Camera::MoveForward( float units )
{
	velocity = velocity + (look* units);
}

void Camera::Strafe( float units )
{
	velocity = velocity + (right* units);
}

void Camera::MoveUp( float units )
{
	velocity.y += units;
}

void Camera::Yaw( float radians )
{
	if( radians == 0.0f )
		return;

	Math::Matrix4 rotation;
	rotation.RotateAxis( up, radians );
	right.Transform( right, rotation );
	look.Transform( look, rotation );
}

void Camera::Pitch( float radians )
{
	if( radians == 0.0f )
        return;
 
    pitch -= radians;

    if ( pitch > maxPitch )
        radians += pitch - maxPitch;
    else if ( pitch < -maxPitch )
        radians += pitch + maxPitch;
 
	Math::Matrix4 rotation;
	rotation.RotateAxis( right, radians );

	up.Transform( up, rotation );
	look.Transform( look, rotation );
}

void Camera::Roll( float radians )
{
	if( radians == 0.0f )
		return;

	Math::Matrix4 rotation;
	rotation.RotateAxis( look, radians );
	right.Transform( right, rotation );
	up.Transform( up, rotation );
}

void Camera::Update()
{
	if( velocity.Length() > maxVelocity )
		velocity = velocity.Normalize()* maxVelocity;

	eye = eye + velocity;

	velocity = Math::Vector3::Null;
	lookAt = eye + look;

	//Update Camera View Matrix
	UpdateView();

	//Set the camera axes from the view matrix
	right.x = view._11;
	right.y = view._21;
	right.z = view._31;
	up.x = view._12;
	up.y = view._22;
	up.z = view._32;
	look.x = view._13;
	look.y = view._23;
	look.z = view._33;

	//Retrieve new yaw and pitch from Camera
	float lookLengthOnXZ = sqrtf( look.z* look.z + look.x* look.x );
	pitch = atan2f( look.y, lookLengthOnXZ );
	yaw = atan2f( look.x, look.z );
}

void Camera::UpdateProjection()
{
	float h = 1.0f / tanf( fov* 0.5f );
	float w = h / aspectRatio;

	projection.m[0][0] = w;
	projection.m[1][1] = h;
	projection.m[2][2] = farClip / ( farClip - nearClip );
	projection.m[2][3] = 1.0f;
	projection.m[3][2] = -nearClip* farClip / ( farClip - nearClip );

	frustum.Define( fov, 1.0f, aspectRatio, nearClip, farClip );
}

void Camera::UpdateView()
{
	//Update View Matrix
	D3DXMATRIX m;
	D3DXMatrixLookAtLH( &m, &(eye + translation).Get(), &(lookAt + translation).Get(), &up.Get() );
	memcpy_s( &view, sizeof( Math::Matrix4 ), &m, sizeof( D3DXMATRIX ) );
	
	//Update Camera Frustum
	frustum.Transform( eye + translation, lookAt + translation, up );
}

}