#pragma once

#include <Windows.h>

#include "Header.h"
#include "Constants.h"
#include "D3D.h"
//#include "DXUT.h"
#include "3DModel.h"
#include "Camera.h"
#include "Shader.h"
#include "DCEL.h"

#pragma warning( once : 4005 )

class Graphics
{
public:
	Graphics();
	Graphics( const Graphics& );
	~Graphics();

	bool Initialize( int, int, HWND );
	void Shutdown();
	bool Frame();

	void SetYawPitchRoll( float rx, float ry, float rz );
	void SetModelData( DCEL* CHModel );

private:
	bool D3DRender();

private:
	D3D     *m_pD3D;
	Model3D *m_pModel;
	Shader  *m_pShader;
	Camera  *m_pCamera;

private:
	float   m_xRotation;
	float   m_yRotation;
	float   m_zRotation;
};