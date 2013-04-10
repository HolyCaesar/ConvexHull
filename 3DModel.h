#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class Model3D
{
public:
	Model3D();
	Model3D( const Model3D& );
	~Model3D();

	bool Initialize( ID3D11Device* );
	void ShutDown();
	void RenderModel( ID3D11DeviceContext* );

	int GetIndexCount();

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

private:
	bool InitializeBuffers( ID3D11Device* );
	void ShutDownBuffers();
	void RenderBuffers( ID3D11DeviceContext* );

private:
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	int m_vertexCount;
	int m_indexCount;
};