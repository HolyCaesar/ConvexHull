#include "DCEL.h"

DCEL::DCEL()
{
	m_HalfEdges = new list<HalfedgeObject*>;
	m_Vertexs = new list<VertexObject*>;
	m_Faces = new list<FaceObject*>;
}

// When return an instance of this kind in a fucntion, this constructor will be invoked and therefore we can control this phase behavior by using this constructor
DCEL::DCEL( const DCEL& other )
{
	*this = other;
}

DCEL::~DCEL()
{
}

template<class T>
void DCEL::cleanList( list<T>* target )
{
	cout << target->size() << endl;

	list<T>::iterator iter;
	for( iter = target->begin(); iter != target->end(); iter++ )
	{
		delete *iter;
	}
}

void DCEL::clean()
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

void DCEL::createDCEL( vector<TRIANGLE>* pTriangles, vector<VERTEX>* pVertex )
{
	// Create vertexobject list for all vertices first
	for( int i = 0; i < pVertex->size(); i++ )
	{
		VertexObject* vo = new VertexObject;
		vo->v = &(*pVertex)[ i ];
		vo->leaving = NULL;
		add( vo );
	}

	// Buiuild halfedges that are belong to this current face
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
		
		// Create three interior halfedges, their twins will be assigned later
		HalfedgeObject *hEdge1 = new HalfedgeObject, *hEdge2 = new HalfedgeObject, *hEdge3 = new HalfedgeObject;
		add( hEdge1 );
		add( hEdge2 );
		add( hEdge3 );

		FaceObject* face = new FaceObject;
		face->attachedEdge = hEdge1;
		add( face );

		if( !pointOnePtr->leaving )
		{
			pointOnePtr->leaving = hEdge1;
		}
		hEdge1->origin = pointOnePtr;
		hEdge1->twins = NULL;
		hEdge1->attachedFace = face;
		hEdge1->nextEdge = hEdge2;
		hEdge1->preEdge  = hEdge3;

		if( !pointTwoPtr->leaving )
		{
			pointTwoPtr->leaving = hEdge2;
		}
		hEdge2->origin = pointTwoPtr;
		hEdge2->twins = NULL;
		hEdge2->attachedFace = face;
		hEdge2->nextEdge = hEdge3;
		hEdge2->preEdge  = hEdge1;

		if( !pointThreePtr->leaving )
		{
			pointThreePtr->leaving = hEdge3;
		}
		hEdge3->origin = pointThreePtr;
		hEdge3->twins = NULL;
		hEdge3->attachedFace = face;
		hEdge3->nextEdge = hEdge1;
		hEdge3->preEdge  = hEdge2;
	}

	// Find twin for each halfedge
	list<HalfedgeObject*>::iterator list_iter;
	list<HalfedgeObject*>::iterator tmp_list_iter;
	for( list_iter = m_HalfEdges->begin(); list_iter != m_HalfEdges->end(); list_iter++ )
	{
		if( (*list_iter)->twins == NULL )
		{
			for( tmp_list_iter = m_HalfEdges->begin(); tmp_list_iter != m_HalfEdges->end(); tmp_list_iter++ )
			{
				if( (*tmp_list_iter)->nextEdge->origin == (*list_iter)->origin )
				{
					(*tmp_list_iter)->twins = (*list_iter);
					(*list_iter)->twins = (*tmp_list_iter);
				}
			}
		}
	}
}

void DCEL::findPreNextEdges( VertexObject* refVertex, VertexObject* targetVertex1, VertexObject* targetVertex2, HalfedgeObject* preEdge, HalfedgeObject* nextEdge )
{
	// targetVertex1 uses for finding nextEdge, targetVertex2 uses for finding preEdge
	list<HalfedgeObject*>::iterator iter;
	for( iter = m_HalfEdges->begin(); iter != m_HalfEdges->end(); iter++ )
	{
		if( (*iter)->origin == targetVertex1 && (*iter)->twins->origin == targetVertex2 )
		{
			nextEdge = (*iter);
		}

		if( (*iter)->origin == targetVertex2 && (*iter)->twins->origin == refVertex )
		{
			preEdge = (*iter);
		}
	}
}

void DCEL::test( FaceObject* faceObject )
{
	HalfedgeObject* halfedgeObject = faceObject->attachedEdge;
	VertexObject* firstVertexObject = halfedgeObject->origin;
	VertexObject* secondVertexObject = halfedgeObject->nextEdge->origin;
	VertexObject* thirdVertexObject = halfedgeObject->nextEdge->nextEdge->origin;

	// Print out all vertices that belong to this face
	cout << "This face is consist of point " << findVertexID( firstVertexObject ) 
		 << " ," << findVertexID( secondVertexObject )
		 << " and " << findVertexID( thirdVertexObject ) << endl;
}

int DCEL::findVertexID( VertexObject* vertexObjcet )
{
	list<VertexObject*>::iterator iter;

	int count = 0;
	for( iter = m_Vertexs->begin(); iter != m_Vertexs->end(); iter++, count++ )
	{
		if( *iter == vertexObjcet )
		{
			return count;
		}
	}

	// If this vertex doesn't exist
	return -1;
}

void DCEL::deleteFace( FaceObject* faceObject )
{
	HalfedgeObject* faceAttachedEdge1 = faceObject->attachedEdge;
	HalfedgeObject* faceAttachedEdge2 = faceAttachedEdge1->nextEdge;
	HalfedgeObject* faceAttachedEdge3 = faceAttachedEdge2->nextEdge;

	faceAttachedEdge1->twins->twins = NULL;
	faceAttachedEdge2->twins->twins = NULL;
	faceAttachedEdge3->twins->twins = NULL;

	// Remove all edges that belong to this face
	remove( faceAttachedEdge1 );
	delete faceAttachedEdge1;
	remove( faceAttachedEdge2 );
	delete faceAttachedEdge2;
	remove( faceAttachedEdge3 );
	delete faceAttachedEdge3;
	
	// Remove this face
	remove( faceObject );
	delete faceObject;
}
