
#include "Camera.h"

Camera::Camera()
{
	mPositionX = 0.0f;
	mPositionY = 0.0f;
	mPositionZ = 0.0f;

	mRotationX = 0.0f;
	mRotationY = 0.0f;
	mRotationZ = 0.0f;
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}


void Camera::setPosition(float x, float y, float z)
{
	mPositionX = x;
	mPositionY = y;
	mPositionZ = z;
	return;
}


void Camera::setRotation(float x, float y, float z)
{
	mRotationX = x;
	mRotationY = y;
	mRotationZ = z;
	return;
}


D3DXVECTOR3 Camera::getPosition()
{
	return D3DXVECTOR3( mPositionX, mPositionY, mPositionZ );
}


D3DXVECTOR3 Camera::getRotation()
{
	return D3DXVECTOR3( mRotationX, mRotationY, mRotationZ );
}


void Camera::render()
{
    D3DXVECTOR3 up, position, lookAt;
    float yaw, pitch, roll;
    D3DXMATRIX rotationMatrix;

    /*Setup the vector that points upwards*/
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

	position.x = mPositionX;
	position.y = mPositionY;
	position.z = mPositionZ;

    /*Setup where the camera is looking at by default*/
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    /*Set the yaw( y axis ), pitch( x axis ), and roll( Z axis ) rotations in radians*/
    pitch = mRotationX * 0.0174532925f;
    yaw   = mRotationY * 0.0174532925f;
    roll  = mRotationZ * 0.0174532925f;

    /*Create the rotation matrix from the yaw, pitch and roll values*/
    D3DXMatrixRotationYawPitchRoll( &rotationMatrix, yaw, pitch, roll );

    /*Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin*/
    D3DXVec3TransformCoord( &lookAt, &lookAt, &rotationMatrix );
    D3DXVec3TransformCoord( &up, &up, &rotationMatrix );

    /*Translate the rotated camera position to the location of the viewer*/
    //lookAt = position + lookAt;

    /*Finally create the view matrix from the three updated vectors*/
    D3DXMatrixLookAtLH( &mViewMatrix, &position, &lookAt, &up );
}


void Camera::getViewMatrix( D3DXMATRIX& viewMatrix )
{
    viewMatrix = mViewMatrix;
}