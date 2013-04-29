#include "3DModel.h"
#include "GeoDefinition.h"

Model3D::Model3D( const Model3D& other )
{
}

Model3D::Model3D()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
}

Model3D::~Model3D()
{
}

bool Model3D::Initialize( ID3D11Device* device )
{
	return InitializeBuffers( device );
}

void Model3D::ShutDown()
{
	ShutDownBuffers();
}

void Model3D::RenderModel( ID3D11DeviceContext* deviceContext )
{
	RenderBuffers( deviceContext );
}

int Model3D::GetIndexCount()
{
	return m_indexCount;
}

bool Model3D::InitializeBuffers( ID3D11Device* device )
{
	VertexType* vertices;
	unsigned long* indices;
	
	m_vertexCount = 3;
	m_indexCount = 3;

	vertices = new VertexType[ m_vertexCount ];
	if( !vertices )
	{
		return false;
	}

	indices = new unsigned long[ m_indexCount ];
	if( !indices )
	{
		return false;
	}

	//TODO this part need modify so that we can read convex hull automatically
	/*Load the vertex array with data*/
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	/*Set up the description of the static vertex buffer*/
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	/*Give the subresource structure a pointer to the vertex data*/
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	/*Create vertex Buffer at device memory end*/
	if( FAILED( device->CreateBuffer( &vertexBufferDesc, &vertexData, &m_pVertexBuffer ) ) )
	{
		return false;
	}

	/*Set up the description of the static index buffer.*/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long ) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	/*Give the subresource structure a pointer to the index data*/
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	/*Create indices buffer at device memory end*/
	if( FAILED( device->CreateBuffer( &indexBufferDesc, &indexData, &m_pIndexBuffer ) ) )
	{
		return false;
	}

	/*Release temp buffers*/
	delete [] vertices;
	vertices = NULL;

	delete [] indices;
	indices = NULL;

	return true;
}

void Model3D::ShutDownBuffers()
{
	if( m_pIndexBuffer )
	{
		m_pIndexBuffer->Release();
	}

	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
	}
}

void Model3D::RenderBuffers( ID3D11DeviceContext* deviceContext )
{
	unsigned int stride;
	unsigned int offset;

	/*Set vertex buffer stride and offset*/
	stride = sizeof( VertexType );
	offset = 0;

	/*Set the vertex buffer to active in the input assembler so it can be rendered.*/
	deviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	deviceContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	/*Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.*/
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

bool Model3D::UpdateModelData( ID3D11Device* device, DCEL* CHModel )
{
	VertexType* vertices;
	unsigned long* indices;

	m_vertexCount = CHModel->m_Faces->size() * 3;
	m_indexCount  = CHModel->m_Faces->size() * 3;

	vertices = new VertexType[ m_vertexCount ];
	if( !vertices )
	{
		return false;
	}

	indices = new unsigned long[ m_indexCount ];
	if( !indices )
	{
		return false;
	}

	list<FaceObject*>::iterator facesIter;
	unsigned int counter = 0;
	for( facesIter = CHModel->m_Faces->begin(); facesIter != CHModel->m_Faces->end(); facesIter++ )
	{
		VERTEX* v1 = (*facesIter)->attachedEdge->origin->v;
		VERTEX* v2 = (*facesIter)->attachedEdge->nextEdge->origin->v;
		VERTEX* v3 = (*facesIter)->attachedEdge->nextEdge->nextEdge->origin->v;

		vertices[ counter ].position = D3DXVECTOR3( v1->x, v1->y, v1->z );
		vertices[ counter ].color = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f );
		indices[ counter ] = counter;

		vertices[ counter + 1 ].position = D3DXVECTOR3( v2->x, v2->y, v2->z );
		vertices[ counter + 1 ].color = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f );
		indices[ counter + 1 ] = counter + 1;

		vertices[ counter + 2 ].position = D3DXVECTOR3( v3->x, v3->y, v3->z );
		vertices[ counter + 2 ].color = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f );
		indices[ counter + 2 ] = counter + 2;

		counter += 3;
	}

	/*Set up the description of the static vertex buffer*/
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	/*Give the subresource structure a pointer to the vertex data*/
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	/*Create vertex Buffer at device memory end*/
	if( FAILED( device->CreateBuffer( &vertexBufferDesc, &vertexData, &m_pVertexBuffer ) ) )
	{
		return false;
	}

	/*Set up the description of the static index buffer.*/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long ) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	/*Give the subresource structure a pointer to the index data*/
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	/*Create indices buffer at device memory end*/
	if( FAILED( device->CreateBuffer( &indexBufferDesc, &indexData, &m_pIndexBuffer ) ) )
	{
		return false;
	}

	/*Release temp buffers*/
	delete [] vertices;
	vertices = NULL;

	delete [] indices;
	indices = NULL;

	return true;
}


	

