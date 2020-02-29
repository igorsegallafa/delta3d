#include "PrecompiledHeader.h"
#include "Frustum.h"

namespace Delta3D::Math
{

void Frustum::Define( float fov, float zoom, float aspectRatio, float nearClip, float farClip )
{
	nearDistance = nearClip;
	farDistance = farClip;

	//Compute Width and Height of near and far planes (scaled)
	float tang = tanf(fov* 0.5f);

	nearHeight = nearDistance* tang* zoom;
	nearWidth = nearHeight* aspectRatio;

	farHeight = farDistance* tang* zoom;
	farWidth = farHeight* aspectRatio;
}

void Frustum::Transform( const Vector3& eye, const Vector3& lookAt, const Vector3& up )
{
	//Compute Z axis of Camera
	Vector3 Z = lookAt - eye;
	Z.Normalize();

	//Compute X axis of Camera
	Vector3 X = up.CrossProduct( Z );
	X.Normalize();

	//Compute Y axis of Camera
	Vector3 Y = Z.CrossProduct( X );

	//Compute centers of near and far plane
	Vector3 nc = eye + ( Z* nearDistance );
	Vector3 fc = eye + ( Z* farDistance );

	//Compute the 4 corners of the frustum on the near plane
	vertices[0] = nc + Y* nearHeight - X* nearWidth;
	vertices[1] = nc + Y* nearHeight + X* nearWidth;
	vertices[2] = nc - Y* nearHeight - X* nearWidth;
	vertices[3] = nc - Y* nearHeight + X* nearWidth;

	//Compute the 4 corners of the frustum on the far plane
	vertices[4] = fc + Y* farHeight - X* farWidth;
	vertices[5] = fc + Y* farHeight + X* farWidth;
	vertices[6] = fc - Y* farHeight - X* farWidth;
	vertices[7] = fc - Y* farHeight + X* farWidth;

	UpdatePlanes();
}

void Frustum::UpdatePlanes()
{
	//Build Planes from Triangles facing inward (triangles are clockwise)
	p[(int)FrustumPlane::Near] = Plane( vertices[0], vertices[1], vertices[3] );
	p[(int)FrustumPlane::Left] = Plane( vertices[5], vertices[4], vertices[6] );
	p[(int)FrustumPlane::Right] = Plane( vertices[0], vertices[2], vertices[6] );
	p[(int)FrustumPlane::UP] = Plane( vertices[3], vertices[1], vertices[7] );
	p[(int)FrustumPlane::Down] = Plane( vertices[1], vertices[0], vertices[4] );
	p[(int)FrustumPlane::Far] = Plane( vertices[2], vertices[3], vertices[7] );
}

}