#ifndef CAMERA_H
#define CAMERA_H

#include "Header.h"
#include <D3DX10math.h>

class Camera
{
public:
    Camera( const Camera& );
    Camera();
    ~Camera();

    void setPosition( float x, float y, float z );
    void setRotation( float x, float y, float z );

    D3DXVECTOR3 getPosition();
    D3DXVECTOR3 getRotation();

    void render();
    void getViewMatrix( D3DXMATRIX& );

private:
    float mPositionX, mPositionY, mPositionZ;
    float mRotationX, mRotationY, mRotationZ;
    D3DXMATRIX mViewMatrix;
};

#endif // CAMERA_H