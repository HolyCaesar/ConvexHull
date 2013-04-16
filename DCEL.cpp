#include "DCEL.h"

DCEL::DCEL()
{
	m_HalfEdges = new list<HalfedgeObject*>;
	m_Vertexs = new list<VertexObject*>;
	m_Faces = new list<FaceObject*>;
}

DCEL::DCEL( const DCEL& other )
{
}

DCEL::~DCEL()
{
	cleanList( m_HalfEdges );
	m_HalfEdges->clear();
	delete m_HalfEdges;

	cleanList( m_Vertexs );
	m_Vertexs->clear();
	delete m_Vertexs;

	cleanList( m_Faces );
	m_Faces->clear();
	delete m_Faces;
}

template<class T>
void DCEL::cleanList( list<T>* target )
{
	list<T>::iterator iter;

	for( iter = target->begin(); iter != target->end(); iter++ )
	{
		delete *iter;
	}
}

void DCEL::CreateDCEL( vector<TRIANGLE>* pTriangles, vector<VERTEX>* pVertex )
{
	// Create vertexobject list for all vertices first
	for( int i = 0; i < pVertex->size(); i++ )
	{
		VertexObject* vo = new VertexObject;
		vo->leaving = NULL;
		m_Vertexs->push_back( vo );
	}

	for( int i = 0; i < pTriangles->size(); i++ )
	{
		TRIANGLE triangle = (*pTriangles)[ i ];
		int pointOneIndex = triangle.p1.pointOneIndex;
		int pointTwoIndex = triangle.p2.pointTwoIndex;
		int pointThreeIndex = triangle.p3.pointThreeIndex;
		VertexObject* pointOnePtr = NULL;
		VertexObject* pointTwoPtr = NULL;
		VertexObject* pointThreePtr = NULL;

		list<VertexObject*>::iterator vlistIter;
		int counter = 0;
		// Find those points that form the triangle in VertexObject list
		for( vlistIter = m_Vertexs->begin(); vlistIter != m_Vertexs->end(); vlistIter++, counter++ )
		{
			if( counter == pointOneIndex )
			{
				pointOnePtr = *vlistIter;
			}
			else if( counter == pointTwoIndex )
			{
				pointTwoPtr = *vlistIter;
			}
			else if( counter == pointThreeIndex )
			{
				pointThreePtr = *vlistIter;
			}
		}
		
		HalfedgeObject *hEdge11 = NULL, *hEdge12 = NULL, *hEdge21 = NULL, *hEdge22 = NULL, *hEdge31 = NULL, *hEdge32 = NULL;
		FaceObject* face = new FaceObject;
		face->attachedEdge = NULL;
		m_Faces->push_back( face );

		if( !pointOnePtr->leaving )
		{
			// hEdge11 is the leaving edge of this point
			hEdge11 = new HalfedgeObject;
			// hEdge12 is the other half edge of this point
			hEdge12 = new HalfedgeObject;
			m_HalfEdges->push_back( hEdge11 );
			m_HalfEdges->push_back( hEdge12 );

			face->attachedEdge = hEdge11;
			// TODO add attachedEdge here
			hEdge11->origin = pointOnePtr;
			hEdge11->twins = hEdge21;
			hEdge11->attachedFace = face;

			hEdge12->twins = hEdge11;
			hEdge12->origin = pointTwoPtr;
			hEdge12->attachedFace = face;
		}

		if( !pointTwoPtr->leaving )
		{
			// hEdge21 is the leaving edge of this point
			hEdge21 = new HalfedgeObject;
			// hEdge22 is the other half edge of this point
			hEdge22 = new HalfedgeObject;
			m_HalfEdges->push_back( hEdge21 );
			m_HalfEdges->push_back( hEdge22 );

			if( !face->attachedEdge )
			{
				face->attachedEdge = hEdge21;
			}
			hEdge21->origin = pointTwoPtr;
			hEdge21->attachedFace = face;
			hEdge21->twins = hEdge22;
			
			hEdge22->twins = hEdge21;
			hEdge22->origin = pointThreePtr;
			hEdge22->attachedFace = face;
		}

		if( !pointThreePtr->leaving )
		{
			// hEdge21 is the leaving edge of this point
			hEdge31 = new HalfedgeObject;
			// hEdge22 is the other half edge of this point
			hEdge32 = new HalfedgeObject;
			m_HalfEdges->push_back( hEdge31 );
			m_HalfEdges->push_back( hEdge32 );

			if( !face->attachedEdge )
			{
				face->attachedEdge = hEdge31;
			}
			hEdge31->origin = pointThreePtr;
			hEdge31->twins = hEdge32;
			hEdge31->attachedFace = face;

			hEdge32->twins = hEdge31;
			hEdge32->origin = pointOnePtr;
			hEdge32->attachedFace = face;
		}

		findPreNextEdges( pointTwoPtr, pointThreePtr, hEdge11->preEdge, hEdge11->nextEdge );
		findPreNextEdges( pointOnePtr, pointThreePtr, hEdge12->preEdge, hEdge12->nextEdge );

		findPreNextEdges( pointThreePtr, pointOnePtr, hEdge21->preEdge, hEdge21->nextEdge );
		findPreNextEdges( pointTwoPtr, pointOnePtr, hEdge22->preEdge, hEdge22->nextEdge );

		findPreNextEdges( pointOnePtr, pointTwoPtr, hEdge31->preEdge, hEdge31->nextEdge );
		findPreNextEdges( pointThreePtr, pointTwoPtr, hEdge32->preEdge, hEdge32->nextEdge );
	}
}

void DCEL::findPreNextEdges( VertexObject* targetVertex1, VertexObject* targetVertex2, HalfedgeObject* preEdge, HalfedgeObject* nextEdge )
{
	// targetVertex1 uses for finding nextEdge, targetVertex2 uses for finding preEdge
	list<HalfedgeObject*>::iterator iter;
	for( iter = m_HalfEdges->begin(); iter != m_HalfEdges->end(); iter++ )
	{
		if( (*iter)->origin == targetVertex1 )
		{
			nextEdge = (*iter);
		}

		if( (*iter)->origin == targetVertex2 )
		{
			preEdge = (*iter);
		}
	}
}