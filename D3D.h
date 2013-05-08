#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <D3D11.h>
#include <D3D10.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3DX10math.h>
#include <DXGI.h>

#include "Header.h"
#include "Constants.h"

class D3D
{
public:
	D3D();
	D3D( const D3D& );
	~D3D();

	bool Initialize( int, int, bool, HWND, bool, float, float );
	void Shutdown();
	
	void BeginScene( float, float, float, float );
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix( D3DXMATRIX& );
	void GetWorldMatrix( D3DXMATRIX& );
	void GetOrthoMatrix( D3DXMATRIX& );

	void GetVideoCardInfo( char*, int& );

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

	void TurnZBufferOn();
	void TurnZBufferOff();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[ 128 ];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11RasterizerState* m_rasterState;


	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
};