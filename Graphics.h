#pragma once

#include <Windows.h>

#include "Header.h"
#include "Constants.h"
#include "D3D.h"
//#include "DXUT.h"
#include "3DModel.h"
#include "Camera.h"
#include "Shader.h"
#include "LightShader.h"
#include "Light.h"
#include "DCEL.h"
#include "DebugWindows.h"
#include "RenderTexture.h"
#include "TextureShader.h"

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
	void Scale( float factor );
	void SetModelData( DCEL* CHModel, vector<VERTEX>* animationSeq );
	ID3D11Device* GetD3DDevice();

private:
	bool D3DRender();
	bool D3DRenderToTexture();
	bool D3DRenderToScene( bool animated = true );

private:
	D3D     *m_pD3D;
	Model3D *m_pModel;
	Shader  *m_pShader;
	Camera  *m_pCamera;
	LightShaderClass *m_pLightShader;
	LightClass       *m_pLight;
	RenderTextureClass* m_pRenderTexture;
	DebugWindowClass* m_pDebugWindow;
	TextureShaderClass* m_pTextureShader;

private:
	float   m_xRotation;
	float   m_yRotation;
	float   m_zRotation;

	float   m_scale;
};