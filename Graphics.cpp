#include "Graphics.h"

using namespace std;

Graphics::Graphics()
{
	m_pD3D = NULL;
	m_pModel = NULL;
	m_pShader = NULL;
	m_pCamera = NULL;

	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;
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
	D3DXMatrixScaling( &scaleMatrix, 2.0f, 2.0f, 2.0f );
	worldMatrix *= scaleMatrix * rotationMatrix;

	/*Put the model vertex and index buffers on the graphics pipeline to prepare for rendering*/
	m_pModel->RenderModel( m_pD3D->GetDeviceContext() );

	/*Render model using the color shader*/
	m_pShader->Render( m_pD3D->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix );

	/*Present the rendered scene to the screen.*/
	m_pD3D->EndScene();
	return true;
}

void Graphics::SetModelData( DCEL* CHModel )
{
	m_pModel->UpdateModelData( m_pD3D->GetDevice(), CHModel );
}
