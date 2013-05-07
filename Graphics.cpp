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
}

bool Graphics::Frame()
{
	D3DRender();
	return true;
}

bool Graphics::D3DRender()
{
	D3DXMATRIX viewMatrix, worldMatrix, projectionMatrix;

	/*Clear the buffers to begin the scene.*/
	m_pD3D->BeginScene( 0.5f, 0.5f, 0.5f, 1.0f );

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
	if( m_pModel->m_isAnimated )
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

	/*Render GUI*/
	TwDraw();

	/*Present the rendered scene to the screen.*/
	m_pD3D->EndScene();
	return true;
}

void Graphics::SetModelData( DCEL* CHModel, vector<vector<VERTEX>>* animationSeq )
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