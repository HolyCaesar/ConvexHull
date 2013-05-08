#include "Graphics.h"
#include "Quaternion.h"

using namespace std;

Graphics::Graphics()
{
	m_pD3D = NULL;
	m_pModel = NULL;
	m_pShader = NULL;
	m_pCamera = NULL;
	m_pLightShader = NULL;
	m_pLight = NULL;
	m_pRenderTexture = NULL;
	m_pDebugWindow   = NULL;
	m_pTextureShader = NULL;

	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;

	m_scale     = 1.0f;
}

Graphics::Graphics( const Graphics& other )
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize( int screenWidth, int screenHeight, HWND hwnd )
{
	bool result;

	/*Create the Direct3D object.*/
	try{
		m_pD3D = new D3D;
		m_pModel = new Model3D;
		m_pShader = new Shader;
		m_pCamera = new Camera;
	}
	catch( bad_alloc& )
	{
		cout << "Error allocating memory." << endl;
	}

	/*Initialize the Direct3D object.*/
	if( !m_pD3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR ) )
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	/*Initialize the camera*/
	m_pCamera->setPosition( 0.0f, 0.0f, -10.0f );

	/*Initialize the model*/
	if( !( m_pModel->Initialize( m_pD3D->GetDevice() ) ) )
	{
		MessageBox(hwnd, L"Could not initialize Model", L"Error", MB_OK);
		return false;
	}

	/*Initialize the shader*/
	if( !m_pShader->Initialize( m_pD3D->GetDevice(), hwnd ) )
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_pLightShader = new LightShaderClass;
	if( !m_pLightShader )
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_pLightShader->Initialize( m_pD3D->GetDevice(), hwnd );
	if( !result )
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_pLight = new LightClass;
	if( !m_pLight )
	{
		return false;
	}

	// Initialize the light object.
	m_pLight->SetDiffuseColor( 0.1f, 0.5f, 0.8f, 1.0f );
	m_pLight->SetDirection( 0.0f, 0.0f, 1.0f );

	// Create the render to texture object.
	m_pRenderTexture = new RenderTextureClass;
	if( !m_pRenderTexture )
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_pRenderTexture->Initialize( m_pD3D->GetDevice(), screenWidth, screenHeight );
	if( !result )
	{
		return false;
	}

	// Create the debug window object.
	m_pDebugWindow = new DebugWindowClass;
	if( !m_pDebugWindow )
	{
		return false;
	}

	// Initialize the debug window object.
	result = m_pDebugWindow->Initialize( m_pD3D->GetDevice(), screenWidth, screenHeight, 200, 150 );
	if( !result )
	{
		MessageBox( hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK );
		return false;
	}

	// Create the texture shader object.
	m_pTextureShader = new TextureShaderClass;
	if( !m_pTextureShader )
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_pTextureShader->Initialize( m_pD3D->GetDevice(), hwnd );
	if( !result )
	{
		MessageBox( hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK );
		return false;
	}

	return true;
}

void Graphics::SetYawPitchRoll( float rx, float ry, float rz )
{
	m_xRotation += rx;
	m_yRotation += ry;
	m_zRotation += rz;
}

void Graphics::Shutdown()
{
	if( m_pD3D )
	{
		m_pD3D->Shutdown();
		delete m_pD3D;
		m_pD3D = NULL;
	}

	if( m_pShader )
	{
		m_pShader->Shutdown();
		delete m_pShader;
		m_pShader = NULL;
	}

	if( m_pModel )
	{
		m_pModel->ShutDown();
		delete m_pModel;
		m_pModel = NULL;
	}

	if( m_pCamera )
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if( m_pLightShader )
	{
		delete m_pLightShader;
		m_pLightShader = NULL;
	}

	if( m_pLight )
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	if( m_pRenderTexture )
	{
		delete m_pRenderTexture;
		m_pRenderTexture = NULL;
	}

	if( m_pDebugWindow )
	{
		delete m_pDebugWindow;
		m_pDebugWindow = NULL;
	}

	if( m_pTextureShader )
	{
		delete m_pTextureShader;
		m_pTextureShader = NULL;
	}
}

bool Graphics::Frame()
{
	D3DRender();
	return true;
}

bool Graphics::D3DRender()
{
	D3DXMATRIX viewMatrix, worldMatrix, orthoMatrix;

	D3DRenderToTexture();

	/*Clear the buffers to begin the scene.*/
	m_pD3D->BeginScene( 0.5f, 0.5f, 0.5f, 1.0f );

	D3DRenderToScene();
	m_pD3D->TurnZBufferOff();

	/*Get matrices*/
	m_pCamera->getViewMatrix( viewMatrix );
	m_pD3D->GetWorldMatrix( worldMatrix );
	m_pD3D->GetOrthoMatrix( orthoMatrix );

	m_pDebugWindow->Render( m_pD3D->GetDeviceContext(), 50, 50 );
	m_pTextureShader->Render( m_pD3D->GetDeviceContext(), m_pDebugWindow->GetIndexCount(), worldMatrix, viewMatrix, 
							  orthoMatrix, m_pRenderTexture->GetShaderResourceView() );

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_pD3D->TurnZBufferOn();

	/*Render GUI*/
	TwDraw();

	/*Present the rendered scene to the screen.*/
	m_pD3D->EndScene();
	return true;
}

void Graphics::SetModelData( DCEL* CHModel, vector<VERTEX>* animationSeq )
{
	m_pModel->UpdateModelData( m_pD3D->GetDevice(), CHModel, animationSeq );
}

void Graphics::Scale( float factor )
{
	m_scale = factor;
}

ID3D11Device* Graphics::GetD3DDevice()
{
	return m_pD3D->GetDevice();
};

bool Graphics::D3DRenderToTexture()
{
	bool result;

	// Set the render target to be the render to texture.
	m_pRenderTexture->SetRenderTarget( m_pD3D->GetDeviceContext(), m_pD3D->GetDepthStencilView() );

	// Clear the render to texture.
	m_pRenderTexture->ClearRenderTarget( m_pD3D->GetDeviceContext(), m_pD3D->GetDepthStencilView(), 0.1f, 0.3f, 0.7f, 1.0f );

	// Translate the camera to the back of the object
	m_pCamera->setPosition( 0.0f, 0.0f, 6.0f );
	m_pLight->SetDirection( 0.0f, 0.0f, -1.0f );
	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = D3DRenderToScene( false );
	if( !result )
	{
		return false;
	}
	m_pLight->SetDirection( 0.0f, 0.0f, 1.0f );
	// Tanslated the camera back
	m_pCamera->setPosition( 0.0f, 0.0f, -10.0f );

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_pD3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::D3DRenderToScene( bool animated )
{
	D3DXMATRIX viewMatrix, worldMatrix, projectionMatrix;
	/*Generate the view matrix based on the camera's position*/
	m_pCamera->render();

	/*Get matrices*/
	m_pCamera->getViewMatrix( viewMatrix );
	m_pD3D->GetWorldMatrix( worldMatrix );
	m_pD3D->GetProjectionMatrix( projectionMatrix );

	/*Create rotation matrix*/
	D3DXMATRIX rotationMatrix, scaleMatrix;
	D3DXMatrixRotationYawPitchRoll( &rotationMatrix, m_xRotation, m_yRotation, m_zRotation );
	D3DXMatrixScaling( &scaleMatrix, m_scale, m_scale, m_scale );
	D3DXMATRIX rotationTest = Quaternion::QuaternionToMatrix( Quaternion::g_SpongeRotation );
	//worldMatrix *= scaleMatrix * rotationMatrix;
	worldMatrix *= scaleMatrix * rotationTest;

	/*Put the model vertex and index buffers on the graphics pipeline to prepare for rendering*/
	m_pModel->RenderModel( m_pD3D->GetDeviceContext() );



	// TODO will be deleted. Tmp usage
	if( animated && m_pModel->m_isAnimated )
	{
		m_pShader->m_animated = m_pModel->m_isAnimated;
		m_pModel->m_isAnimated = false;
		m_pShader->m_animationStep = 0;
	}


	/*Render model using the color shader*/
	//m_pShader->Render( m_pD3D->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix );


	// Render the model using the light shader.
	bool result;
	result = m_pLightShader->Render( m_pD3D->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
		NULL, m_pLight->GetDirection(), m_pLight->GetDiffuseColor());
	if(!result)
	{
		return false;
	}
}