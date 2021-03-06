#include "Shader.h"

Shader::Shader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_animated = false;
	m_animationStep = 0;
}

Shader::Shader( const Shader& other )
{
}

Shader::~Shader()
{
}

bool Shader::Initialize( ID3D11Device* device, HWND hwnd )
{
	return InitializeShader( device, hwnd, L"VertexShader.hlsl", L"PixelShader.hlsl" );
}

void Shader::Shutdown()
{
	ShutdownShader();
}

bool Shader::Render( ID3D11DeviceContext* deviceContext,
					 int indexCount,
					 D3DXMATRIX worldMatrix,
					 D3DXMATRIX viewMatrix,
					 D3DXMATRIX projectionMatrix )
{
	if( !SetShaderParameters( deviceContext, worldMatrix, viewMatrix, projectionMatrix ) )
	{
		return false;
	}

	/*Render the prepared buffers with the shader*/
	RenderShader( deviceContext, indexCount );

	return true;
}

bool Shader::InitializeShader( ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName )
{
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	/*Compile the vertex  shader code*/
	if( FAILED( D3DX11CompileFromFile( vsFileName, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL ) ) )
	{
		if( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, vsFileName );
		}
		else
		{
			MessageBox( hwnd, vsFileName, L"Missing Shader File", MB_OK );
		}

		return false;
	}

	/*Compile the pixel shader code*/
	if( FAILED( D3DX11CompileFromFile( psFileName, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL ) ) )
	{
		if( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, vsFileName );
		}
		else
		{
			MessageBox( hwnd, vsFileName, L"Missing Shader File", MB_OK );
		}

		return false;
	}

	/*Create the vertex shader from the buffer*/
	if( FAILED( device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader ) ) )
	{
		return false;
	}

	/*Create the pixel shader from the buffer*/
	if( FAILED( device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader ) ) )
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[ 2 ];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*Get a count of the elements in the layout*/
	numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[ 0 ] );
	/*Create the vertex input layout*/
	if( FAILED( device->CreateInputLayout( polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout ) ) )
	{
		return false;
	}

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	/*Setup the description of the dynamic matrix constant buffer that is in the vertex shader*/
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if( FAILED( device->CreateBuffer( &matrixBufferDesc, NULL, &m_matrixBuffer ) ) )
	{
		return false;
	}

	return true;
}

void Shader::ShutdownShader()
{
	/*Release the matrix constant buffer.*/
	if( m_matrixBuffer )
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = NULL;
	}

	/*Release the layout.*/
	if( m_layout )
	{
		m_layout->Release();
		m_layout = NULL;
	}

	/*Release the pixel shader.*/
	if( m_pixelShader )
	{
		m_pixelShader->Release();
		m_pixelShader = NULL;
	}

	/*Release the vertex shader.*/
	if( m_vertexShader )
	{
		m_vertexShader->Release();
		m_vertexShader = NULL;
	}
}

void Shader::OutputShaderErrorMessage( ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename )
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	/*Get a pointer to the error message text buffer.*/
	compileErrors = (char*)( errorMessage->GetBufferPointer() );

	/*Get the length of the message.*/
	bufferSize = errorMessage->GetBufferSize();

	/*Open a file to write the error message to.*/
	fout.open( "shader-error.txt" );

	/*Write out the error message.*/
	for( i = 0; i < bufferSize; i++ )
	{
		fout << compileErrors[ i ];
	}

	/*Close the file.*/
	fout.close();

	/*Release the error message.*/
	errorMessage->Release();
	errorMessage = NULL;

	/*Pop a message up on the screen to notify the user to check the text file for compile errors.*/
	MessageBox( hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK );
}

bool Shader::SetShaderParameters( ID3D11DeviceContext* deviceContext, 
								  D3DXMATRIX worldMatrix, 
								  D3DXMATRIX viewMatrix, 
								  D3DXMATRIX projectionMatrix )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	/*Transpose the matrices to prepare them for the shader*/
	D3DXMatrixTranspose( &worldMatrix, &worldMatrix );
	D3DXMatrixTranspose( &viewMatrix, &viewMatrix );
	D3DXMatrixTranspose( &projectionMatrix, &projectionMatrix );

	/*Lock the constant buffer so it can be written to.*/
	if( deviceContext->Map( m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) )
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	/*Copy the matrices into the constant buffer*/
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	/*Unlock the constant buffer*/
	deviceContext->Unmap( m_matrixBuffer, 0 );

	/*set the position of the constant buffer in the vertex shader*/
	bufferNumber = 0;

	/*Finally set the constant buffer in the vertex shader with the updated values*/
	deviceContext->VSSetConstantBuffers( bufferNumber, 1, &m_matrixBuffer );

	return true;
}

using namespace std;
#include <iostream>
void Shader::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount )
{
	/*Set the vertex input layout.*/
	deviceContext->IASetInputLayout( m_layout) ;

	/*Set the vertex and pixel shaders that will be used to render this triangle.*/
	deviceContext->VSSetShader( m_vertexShader, NULL, 0 );
	deviceContext->PSSetShader( m_pixelShader, NULL, 0 );

	/*Render the triangle.*/
	//deviceContext->DrawIndexed( indexCount, 0, 0 );
	if( m_animated )
	{
		deviceContext->DrawIndexed( m_animationStep, 0, 0 );
		m_animationStep += 3;

		if( m_animationStep == indexCount )
		{
			m_animated = false;
		}
	}
	else
	{
		deviceContext->DrawIndexed( indexCount, 0, 0 );
	}

	return;
}