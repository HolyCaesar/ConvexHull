#include "DivideAndConquerFor3DCH.h"

const float eps = 0.000001;

DivideAndConquerFor3DCH::DivideAndConquerFor3DCH()
{
	m_pDCEL = NULL;
	m_pDCEL = new DCEL;
}

DivideAndConquerFor3DCH::DivideAndConquerFor3DCH( const DivideAndConquerFor3DCH& other )
{
	m_pDCEL = NULL;
	m_pDCEL = new DCEL;
}

DivideAndConquerFor3DCH::~DivideAndConquerFor3DCH()
{
	if( !m_pDCEL )
	{
		delete m_pDCEL;
	}
}

DCEL DivideAndConquerFor3DCH::BruceForceCH( vector<VERTEX*>* pVertex, const unsigned int offset )
{
	vector<TRIANGLE> triangleSet, finalTriangleSet;

	// Generate all possible triangles
	int pointSetSize = pVertex->size();
	for( int i = 0; i < pointSetSize; i++ )
	{
		for( int j = i + 1; j < pointSetSize; j++ )
		{
			for( int k = j + 1; k < pointSetSize; k++ )
			{
				// Forming face
				TRIANGLE face;
				face.p1.pointOneIndex = i;
				face.p2.pointTwoIndex = j;
				face.p3.pointThreeIndex = k;
				triangleSet.push_back( face );
			}
		}
	}

	// Find the CH for this point set by using RayAndTriangleIntersection method
	for( int i = 0; i < triangleSet.size(); i++ )
	{
		// Create a ray from this surface
		TRIANGLE triangle = triangleSet[ i ];
		// Point2 - point1
		//D3DXVECTOR3 edge1( triangle.pointTwo.x - triangle.pointOne.x, triangle.pointTwo.y - triangle.pointOne.y, triangle.pointTwo.z - triangle.pointOne.z );
		VERTEX* pointOne = (*pVertex)[ triangle.p1.pointOneIndex ];
		VERTEX* pointTwo = (*pVertex)[ triangle.p2.pointTwoIndex ];
		VERTEX* pointThree = (*pVertex)[ triangle.p3.pointThreeIndex ];

		D3DXVECTOR3 edge1( pointTwo->x - pointOne->x, pointTwo->y - pointOne->y, pointTwo->z - pointOne->z );
		D3DXVECTOR3 edge2( pointThree->x - pointOne->x, pointThree->y - pointOne->y, pointThree->z - pointOne->z );
		// point3 - point1
		//D3DXVECTOR3 edge2( triangle.pointThree.x - triangle.pointOne.x, triangle.pointThree.y - triangle.pointOne.y, triangle.pointThree.z - triangle.pointOne.z );

		D3DXVECTOR3 triangleNormal;
		D3DXVec3Cross( &triangleNormal, &edge1, &edge2 );
		D3DXVECTOR3 rayStartPoint( ( pointOne->x + pointTwo->x + pointThree->x ) / 3.0f,
								   ( pointOne->y + pointTwo->y + pointThree->y ) / 3.0f,
								   ( pointOne->z + pointTwo->z + pointThree->z ) / 3.0f );

		Ray ray, invRay;
		D3DXVec3Normalize( &ray.direction, &triangleNormal );
		ray.position = rayStartPoint;
		invRay = ray;
		invRay.direction *= -1.0;


		bool rayIntersect = !isNormal(ray, triangleSet[i], pVertex);
		bool invRayIntersect = !isNormal(invRay, triangleSet[i], pVertex);
		
		/*
		for( int j = 0; j < triangleSet.size(); j++ )
		{
			if( j == i ) continue;

			rayIntersect = RayTriangleIntersection( ray, triangleSet[ j ], pVertex );

			if( rayIntersect )
			{
				break;
			}
		}

		for( int j = 0; j < triangleSet.size(); j++ )
		{
			if( j == i ) continue;

			test = i;
			invRayIntersect = RayTriangleIntersection( invRay, triangleSet[ j ], pVertex );

			if( invRayIntersect )
			{
				break;
			}
		}
		*/

		// This is the face that contribute to the convex hull and find its vertices order
		if( rayIntersect == false && invRayIntersect == true )
		{
			finalTriangleSet.push_back( triangleSet[ i ] );
		}
		else if( rayIntersect == true && invRayIntersect == false )
		{
			TRIANGLE tmpTri = triangleSet[ i ];
			int tmpVerIndex = tmpTri.p2.pointTwoIndex;
			tmpTri.p2.pointTwoIndex = tmpTri.p3.pointThreeIndex;
			tmpTri.p3.pointThreeIndex = tmpVerIndex;

			finalTriangleSet.push_back( tmpTri );
		}
	}

	DCEL dcel;
	dcel.createDCEL( &finalTriangleSet, pVertex, offset );

	return dcel;
}

bool DivideAndConquerFor3DCH::RayTriangleIntersection( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex )
{
	VERTEX* pointOne = (*pVertex)[ triangle.p1.pointOneIndex ];
	VERTEX* pointTwo = (*pVertex)[ triangle.p2.pointTwoIndex ];
	VERTEX* pointThree = (*pVertex)[ triangle.p3.pointThreeIndex ];

	D3DXVECTOR3 edge1( pointTwo->x - pointOne->x, pointTwo->y - pointOne->y, pointTwo->z - pointOne->z );
	D3DXVECTOR3 edge2( pointThree->x - pointOne->x, pointThree->y - pointOne->y, pointThree->z - pointOne->z );

	D3DXVECTOR3 triNormal;
	D3DXVec3Cross( &triNormal, &edge1, &edge2 );
	D3DXVec3Normalize( &triNormal, &triNormal );
	double denominator = D3DXVec3Dot( &triNormal, &r.direction );

	// Ray parallels to the plane
	if( fabs( denominator ) < 0.000001 )
	{
		return false;
	}

	double d = triNormal.x * pointOne->x + triNormal.y * pointOne->y + triNormal.z * pointOne->z;
	double t = ( d - D3DXVec3Dot( &triNormal, &r.position ) ) / denominator;

	// Trianle behine the ray
	if( t <= 0 )
	{
		return false;
	}

	D3DXVECTOR3 intersectPoint = r.position + t * r.direction;
	
	//D3DXVECTOR3 tmp;
	//D3DXVec3Cross( &tmp, &edge1, &edge2 );
	//double totalAmount = D3DXVec3Dot( &tmp, &triNormal );
	//double totalArea = D3DXVec3Length( &tmp ) * 0.5;

	//VERTEX tmpV = pointThree - pointTwo;
	//D3DXVec3Cross( &tmp, &D3DXVECTOR3( tmpV.x, tmpV.y, tmpV.z ), &D3DXVECTOR3( intersectPoint.x - pointTwo.x, intersectPoint.y - pointTwo.y, intersectPoint.z - pointTwo.z ) );
	//double alpha = D3DXVec3Length( &tmp ) * 0.5 / totalArea;
	//
	//tmpV = pointOne - pointThree;
	//D3DXVec3Cross( &tmp, &D3DXVECTOR3( tmpV.x, tmpV.y, tmpV.z ), &D3DXVECTOR3( intersectPoint.x - pointThree.x, intersectPoint.y - pointThree.y, intersectPoint.z - pointThree.z ) );
	//double beta = D3DXVec3Length( &tmp ) * 0.5 / totalArea;

	//tmpV = pointTwo - pointOne;
	//D3DXVec3Cross( &tmp, &D3DXVECTOR3( tmpV.x, tmpV.y, tmpV.z ), &D3DXVECTOR3( intersectPoint.x - pointOne.x, intersectPoint.y - pointOne.y, intersectPoint.z - pointOne.z ) );
	//double gamma = D3DXVec3Length( &tmp ) * 0.5 / totalArea;

//	if( alpha + beta + gamma > 1.00001 )
//	{
//		return false;
//	}

	D3DXVECTOR3 tmpEdge( intersectPoint.x - pointOne->x, intersectPoint.y - pointOne->y, intersectPoint.z - pointOne->z );
	D3DXVECTOR3 tmpCrossRes;
	D3DXVec3Cross( &tmpCrossRes, &edge1, &tmpEdge );
	double alpha = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( alpha < 0.0f )
	{
		return false;
	}

	tmpEdge = D3DXVECTOR3( intersectPoint.x - pointTwo->x, intersectPoint.y - pointTwo->y, intersectPoint.z - pointTwo->z);
	D3DXVECTOR3 tmpEdge2( pointThree->x - pointTwo->x, pointThree->y - pointTwo->y, pointThree->z - pointTwo->z );
	D3DXVec3Cross( &tmpCrossRes, &tmpEdge2, &tmpEdge ); 
	double beta = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( beta < 0.0f ) 
	{
		return false;
	}

	tmpEdge = D3DXVECTOR3( intersectPoint.x - pointThree->x, intersectPoint.y - pointThree->y, intersectPoint.z - pointThree->z );
	tmpEdge2 = D3DXVECTOR3( pointOne->x - pointThree->x, pointOne->y - pointThree->y, pointOne->z - pointThree->z );
	D3DXVec3Cross( &tmpCrossRes, &tmpEdge2, &tmpEdge ); 
	double gamma = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( gamma < 0.0f )
	{
		return false;
	}

	return true;
}

bool DivideAndConquerFor3DCH::isNormal( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex )
{
	for (int i=0; i<(int)pVertex ->size(); i++)
	{
		if (i == triangle.p1.pointOneIndex) continue;
		if (i == triangle.p2.pointTwoIndex) continue;
		if (i == triangle.p3.pointThreeIndex) continue;

		VERTEX* p = pVertex->operator[](i);

		D3DXVECTOR3 v(p->x - r.position.x, p->y - r.position.y, p->z - r.position.z);
		D3DXVec3Normalize(&v, &v);
		double dot = D3DXVec3Dot(&r.direction, &v);
		if (dot > 1E-5) return false;
	}
	return true;
}

DCEL DivideAndConquerFor3DCH::DVCalculate3DConvexHull( vector<VERTEX>* pVertex, const int startPoint, const int endPoint, const unsigned int offset )
{
	/*
	* Phase 1: Find two convex hulls
	*/
	if( endPoint - startPoint + 1 <= 10 )
	{
		vector<VERTEX*> tmpVertexSet;
		for( int i = startPoint; i < endPoint  + 1; i++ )
		{
			tmpVertexSet.push_back( &(*pVertex)[ i ] );
		}
		return BruceForceCH( &tmpVertexSet, offset );
	}

	int midPoint = ( endPoint + startPoint ) * 0.5;
	DCEL CH1 = DVCalculate3DConvexHull( pVertex, startPoint, midPoint, offset );
	DCEL CH2 = DVCalculate3DConvexHull( pVertex, midPoint + 1, endPoint, midPoint + 1 );
	
	assert(CH1.checkIterators());
	assert(CH2.checkIterators());

	CH1.unsetVisited();
	CH2.unsetVisited();

	/*
	* Phase 2: Merge two convex hulls
	*/
	VertexObject* v1AtTanVO = NULL, * v2AtTanVO = NULL;
	vector<VertexObject*> CH_Up, CH_Down;
	CH_Up = find2DConvexHull( CH1.m_Vertexs );
	CH_Down = find2DConvexHull( CH2.m_Vertexs );
	findTangentFor3DCHs( &CH_Up, &CH_Down, &v1AtTanVO, &v2AtTanVO );

	// Find a plane that cross this tangent and parallel z axis
	D3DXVECTOR3 planV1( v1AtTanVO->v->x - v2AtTanVO->v->x, v1AtTanVO->v->y - v2AtTanVO->v->y, v1AtTanVO->v->z - v2AtTanVO->v->z );
	D3DXVECTOR3 planV2( 0.0f, 0.0f, -1.0f * v1AtTanVO->v->z );
	D3DXVECTOR3 planeNormal, tmpPlaneNormal1, tmpPlaneNormal2, maxTmpPlaneNormal1, maxTmpPlaneNormal2;
	D3DXVec3Cross( &planeNormal, &planV1, &planV2 );
	D3DXVec3Normalize( &planeNormal, &planeNormal );

	// make sure planeNormal be the outside normal
	for (list<VertexObject*>::iterator it=CH1.m_Vertexs->begin(); it!=CH1.m_Vertexs->end(); it++)
	{
		D3DXVECTOR3 v((*it)->v->x - v1AtTanVO->v->x, (*it)->v->y - v1AtTanVO->v->y, (*it)->v->z - v1AtTanVO->v->z);
		double dot = D3DXVec3Dot(&v, &planeNormal);
		if (dot > 1E-5) 
		{
			planeNormal = planeNormal * -1.0f;
			break;
		}
	}
	for (list<VertexObject*>::iterator it=CH2.m_Vertexs->begin(); it!=CH2.m_Vertexs->end(); it++)
	{
		D3DXVECTOR3 v((*it)->v->x - v1AtTanVO->v->x, (*it)->v->y - v1AtTanVO->v->y, (*it)->v->z - v1AtTanVO->v->z);
		double dot = D3DXVec3Dot(&v, &planeNormal);
		if (dot > 1E-5) 
		{
			planeNormal = planeNormal * -1.0f;
			break;
		}
	}
	
	// test
	//for (list<VertexObject*>::iterator it=CH1.m_Vertexs->begin(); it!=CH1.m_Vertexs->end(); it++)
	//{
	//	D3DXVECTOR3 v((*it)->v->x - v1AtTanVO->v->x, (*it)->v->y - v1AtTanVO->v->y, (*it)->v->z - v1AtTanVO->v->z);
	//	double dot = D3DXVec3Dot(&v, &planeNormal);
	//	if (dot > 1E-5) 
	//	{
	//		cout << "RJB报告： 法线阳萎了" << endl;
	//		break;
	//	}
	//}
	//for (list<VertexObject*>::iterator it=CH2.m_Vertexs->begin(); it!=CH2.m_Vertexs->end(); it++)
	//{
	//	D3DXVECTOR3 v((*it)->v->x - v1AtTanVO->v->x, (*it)->v->y - v1AtTanVO->v->y, (*it)->v->z - v1AtTanVO->v->z);
	//	double dot = D3DXVec3Dot(&v, &planeNormal);
	//	if (dot > 1E-5) 
	//	{
	//		cout << "RJB报告： 法线阳萎了" << endl;
	//		break;
	//	}
	//}
	
	
	// These two points are two ends of the tangent
	VertexObject* vE1Start = v1AtTanVO, * vE2Start = v2AtTanVO;

	struct candidateList
	{
		VertexObject* candidatePoint;
		// 0, belong CH on the top; 1, belong CH at the bottom
		int           belonging;
		HalfedgeObject* candidateEdge;
		HalfedgeObject* candidateEdgeTwin;

		candidateList( VertexObject* cand, int bl, HalfedgeObject* candE )
		{
			candidatePoint = cand;
			belonging = bl;
			candidateEdge = candE;
		}

		candidateList()
		{
			candidatePoint = NULL;
			belonging = 1e20f;
			candidateEdge = NULL;
		}

		void operator =( const candidateList& other )
		{
			candidatePoint = other.candidatePoint;
			belonging      = other.belonging;
		}
	};
	vector<candidateList> CHCandidates;
	CHCandidates.push_back( candidateList( v1AtTanVO, 0, NULL ) );
	CHCandidates.push_back( candidateList( v2AtTanVO, 1, NULL ) );
   

	/*
	* Find triangulation T
	*/
	do{
		// Start warping this two convex hull
		double maxPlaneAngleE1 = -10000.0f, maxPlaneAngleE2 = -10000.0f;
		HalfedgeObject* currentHalfEdge = v1AtTanVO->leaving;
		VertexObject* testVertex = currentHalfEdge->twins->origin;
		VertexObject* resultVertex1 = NULL, * resultVertex2 = NULL;
		HalfedgeObject* resultEdge1 = NULL, * resultEdge2 = NULL;

		do
		{
			// Find the angle between two plane
			planV1 = D3DXVECTOR3( v1AtTanVO->v->x - testVertex->v->x, v1AtTanVO->v->y - testVertex->v->y, v1AtTanVO->v->z - testVertex->v->z );
			planV2 = D3DXVECTOR3( v2AtTanVO->v->x - v1AtTanVO->v->x, v2AtTanVO->v->y - v1AtTanVO->v->y, v2AtTanVO->v->z - v1AtTanVO->v->z );
			D3DXVec3Cross( &tmpPlaneNormal1, &planV2, &planV1 );
			D3DXVec3Normalize( &tmpPlaneNormal1, &tmpPlaneNormal1 );

			double tmpMaxPlaneAngle = D3DXVec3Dot( &planeNormal, &tmpPlaneNormal1 );
			if( tmpMaxPlaneAngle > maxPlaneAngleE1 )
			{
				maxPlaneAngleE1 = tmpMaxPlaneAngle;
				resultVertex1 = testVertex;
				maxTmpPlaneNormal1 = tmpPlaneNormal1;
				resultEdge1 = currentHalfEdge;
			}

			currentHalfEdge = currentHalfEdge->twins->nextEdge;
			testVertex = currentHalfEdge->twins->origin;
		}while( currentHalfEdge != v1AtTanVO->leaving );

		currentHalfEdge = v2AtTanVO->leaving;
		testVertex = currentHalfEdge->twins->origin;
		do
		{
			// Find the angle between two plane
			planV1 = D3DXVECTOR3( v2AtTanVO->v->x - testVertex->v->x, v2AtTanVO->v->y - testVertex->v->y, v2AtTanVO->v->z - testVertex->v->z );
			planV2 = D3DXVECTOR3( v1AtTanVO->v->x - v2AtTanVO->v->x, v1AtTanVO->v->y - v2AtTanVO->v->y, v1AtTanVO->v->z - v2AtTanVO->v->z );
			D3DXVec3Cross( &tmpPlaneNormal2, &planV1, &planV2 );
			D3DXVec3Normalize( &tmpPlaneNormal2, &tmpPlaneNormal2 );

			double tmpMaxPlaneAngle = D3DXVec3Dot( &planeNormal, &tmpPlaneNormal2 );
			if( tmpMaxPlaneAngle > maxPlaneAngleE2 )
			{
				maxPlaneAngleE2 = tmpMaxPlaneAngle;
				maxTmpPlaneNormal2 = tmpPlaneNormal2;
				resultVertex2 = testVertex;
				resultEdge2 = currentHalfEdge;
			}

			currentHalfEdge = currentHalfEdge->twins->nextEdge;
			testVertex = currentHalfEdge->twins->origin;
		}while( currentHalfEdge != v2AtTanVO->leaving );

		if( maxPlaneAngleE2 < maxPlaneAngleE1 )
		{
			v1AtTanVO = resultVertex1;
			v1AtTanVO->status = 1;
			planeNormal = maxTmpPlaneNormal1;
			CHCandidates.push_back( candidateList( resultVertex1, 0, resultEdge1 ) );

		}
		else
		{
			v2AtTanVO = resultVertex2;
			v2AtTanVO->status = 1;
			planeNormal = maxTmpPlaneNormal2;
			CHCandidates.push_back( candidateList( resultVertex2, 1, resultEdge2 ) );
		}
	}while( vE1Start != v1AtTanVO || vE2Start != v2AtTanVO );

	/*
	* Change DCEL structure, delete the face, halfedges and vertices inside the new convex hull
	*/
	for (int i=2; i<(int)CHCandidates.size(); i++)
	{
		CHCandidates[ i ].candidateEdge->visited = true;
	}

	for (int i=2; i<(int)CHCandidates.size(); i++)
	{
		if ( CHCandidates[ i ].belonging == 0)
			recursivelyDeleteFace( CHCandidates[ i ].candidateEdge->attachedFace, &CH1);
		else 
		{
			recursivelyDeleteFace( CHCandidates[ i ].candidateEdge->twins->attachedFace, &CH2);
		}
	}

	/*
	* Construct DCEL based on the CHCandidates
	*/
	
	list<FaceObject*>::iterator faceIter, tmpFaceIter;
	candidateList a = CHCandidates[ 0 ]; // tangent point at CH1
	candidateList b = CHCandidates[ 1 ]; // tangent point at CH2

	//int latestPos = 1;
	vector<FaceObject*> tmpFaces;
	for( int i = 2; i < CHCandidates.size(); i++ )
	{
		VertexObject* curPoint = CHCandidates[ i ].candidatePoint;
		HalfedgeObject *HEdge1 = new HalfedgeObject;
		HalfedgeObject *HEdge2 = new HalfedgeObject;
		HalfedgeObject *HEdge3 = new HalfedgeObject;

		if (CHCandidates[ i ].belonging == 0)
		{
			delete HEdge2;
			HEdge2 = CHCandidates[ i ].candidateEdge;
		}
		else
		{
			delete HEdge3;
			HEdge3 = CHCandidates[ i ].candidateEdge->twins;
		}

		FaceObject* face = new FaceObject;
		face->attachedEdge = HEdge1;
		tmpFaces.push_back( face );

		HEdge1->origin = b.candidatePoint;
		HEdge1->attachedFace = face;
		b.candidatePoint->leaving = HEdge1;
		HEdge1->nextEdge = HEdge2;
		HEdge1->preEdge  = HEdge3;

		HEdge2->origin = a.candidatePoint;
		HEdge2->attachedFace = face;
		a.candidatePoint->leaving = HEdge2;
		HEdge2->nextEdge = HEdge3;
		HEdge2->preEdge  = HEdge1;

		HEdge3->origin = curPoint;
		HEdge3->attachedFace = face;
		curPoint->leaving = HEdge3;
		HEdge3->nextEdge = HEdge1;
		HEdge3->preEdge  = HEdge2;

		if( CHCandidates[ i ].belonging == a.belonging )
		{
			HEdge2->twins = CHCandidates[ i ].candidateEdge->twins;
			CHCandidates[ i ].candidateEdge->twins->twins = HEdge2;
		}
		else
		{
			HEdge3->twins = CHCandidates[ i ].candidateEdge;
			CHCandidates[ i ].candidateEdge->twins = HEdge3;
		}

		int currentPos = CHCandidates[ i ].belonging;
		if( currentPos == 1 )
		{
			b = CHCandidates[ i ];
		}
		else
		{
			a = CHCandidates[ i ];
		}
	}


	for( int i = 0; i < tmpFaces.size(); i++ )
	{
		HalfedgeObject* preHEArray[ 3 ], * curHEArray[ 3];
		preHEArray[ 0 ] = tmpFaces[ ( i - 1 + tmpFaces.size() ) % tmpFaces.size() ]->attachedEdge;
		preHEArray[ 1 ] = tmpFaces[ ( i - 1 + tmpFaces.size() ) % tmpFaces.size() ]->attachedEdge->nextEdge;
		preHEArray[ 2 ] = tmpFaces[ ( i - 1 + tmpFaces.size() ) % tmpFaces.size() ]->attachedEdge->nextEdge->nextEdge;

		curHEArray[ 0 ] = tmpFaces[ i ]->attachedEdge;
		curHEArray[ 1 ] = tmpFaces[ i ]->attachedEdge->nextEdge;
		curHEArray[ 2 ] = tmpFaces[ i ]->attachedEdge->nextEdge->nextEdge;

		bool flag = true;
		for( int j = 0; j < 3 && flag; j++ )
		{
			for( int k = 0; k < 3; k++ )
			{
				if( preHEArray[ k ]->nextEdge->origin == curHEArray[ j ]->origin && curHEArray[ j ]->nextEdge->origin == preHEArray[ k ]->origin )
				{
					preHEArray[ k ]->twins = curHEArray[ j ];
					curHEArray[ j ]->twins = preHEArray[ k ];
					flag = false;
					break;
				}
			}
		}
	}

	/*
	* Change DCEL structure, delete the vertices
	* Delete points with status delete
	*/ 

	list<VertexObject*>::iterator vexIter, tmpVexIter;
	for( vexIter = CH1.m_Vertexs->begin(); vexIter != CH1.m_Vertexs->end();  )
	{
		if( (*vexIter)->status == 2 )
		{
			tmpVexIter = vexIter++;
			CH1.remove( (*tmpVexIter ) );
		}
		else
		{
			// Reset point status
			if( (*vexIter)->status == 1 )
			{
				(*vexIter)->status == 0;
			}
			vexIter++;
		}
	}
	for( vexIter = CH2.m_Vertexs->begin(); vexIter != CH2.m_Vertexs->end();  )
	{
		if( (*vexIter)->status == 2 )
		{
			tmpVexIter = vexIter++;
			CH2.remove( (*tmpVexIter) );
		}
		else
		{
			// Reset point status
			if( (*vexIter)->status == 1 )
			{
				(*vexIter)->status == 0;
			}
			vexIter++;
		}

	}

	for( int i = 0; i < tmpFaces.size(); i++ )
	{
		CH1.add( tmpFaces[ i ] );
		CH1.add( tmpFaces[ i ]->attachedEdge );
		CH1.add( tmpFaces[ i ]->attachedEdge->nextEdge );
		CH1.add( tmpFaces[ i ]->attachedEdge->nextEdge->nextEdge );
	}
	CH1.m_HalfEdges->splice( CH1.m_HalfEdges->end(), *CH2.m_HalfEdges );
	CH1.m_Faces->splice( CH1.m_Faces->end(), *CH2.m_Faces );
	CH1.m_Vertexs->splice( CH1.m_Vertexs->end(), *CH2.m_Vertexs );

	CH1.fixIterator();
	return CH1;
}

vector<VertexObject*> DivideAndConquerFor3DCH::find2DConvexHull( list<VertexObject*> *pointSet )
{
	vector<VertexObject*> upper;
	vector<VertexObject*> lower;

	list<VertexObject*>::iterator pointIter;
	D3DXVECTOR2 midLine( pointSet->back()->v->x - pointSet->front()->v->x, pointSet->back()->v->y - pointSet->front()->v->y );

	for( pointIter = pointSet->begin(); pointIter != pointSet->end(); pointIter++ )
	{
		D3DXVECTOR2 currentLine( (*pointIter)->v->x - pointSet->front()->v->x, (*pointIter)->v->y - pointSet->front()->v->y );
		float crossProduct = midLine.x * currentLine.y - midLine.y * currentLine.x;

		if( crossProduct > 0 || (*pointIter) == pointSet->front() || (*pointIter) == pointSet->back() )
		{
			upper.push_back( (*pointIter) );
		}
	}

	list<VertexObject*>::reverse_iterator re_pointIter;
	midLine = D3DXVECTOR2( pointSet->front()->v->x - pointSet->back()->v->x, pointSet->front()->v->y - pointSet->back()->v->y );
	for( re_pointIter = pointSet->rbegin(); re_pointIter != pointSet->rend(); re_pointIter++ )
	{
		D3DXVECTOR2 currentLine( (*re_pointIter)->v->x - pointSet->back()->v->x, (*re_pointIter)->v->y - pointSet->back()->v->y );
		float crossProduct = midLine.x * currentLine.y - midLine.y * currentLine.x;

		if( crossProduct > 0 || (*re_pointIter) == pointSet->front() || (*re_pointIter) == pointSet->back() )
		{
			lower.push_back( (*re_pointIter) );
		}
	}

	vector<VertexObject*> upperChain;
	vector<VertexObject*> lowerChain;
	
	upperChain.push_back( upper[ 0 ] );
	upperChain.push_back( upper[ 1 ] );
	for( int i = 2; i < upper.size(); i++ )
	{
		for( ; upperChain.size() >= 2 ; )
		{
			int upperChainSize = upperChain.size();
			D3DXVECTOR2 currentLine( upper[ i ]->v->x - upperChain[ upperChainSize - 1 ]->v->x, upper[ i ]->v->y - upperChain[ upperChainSize - 1 ]->v->y );
			D3DXVECTOR2 preLine( upperChain[ upperChainSize - 1 ]->v->x - upperChain[ upperChainSize - 2 ]->v->x, upperChain[ upperChainSize - 1 ]->v->y - upperChain[ upperChainSize - 2 ]->v->y );

			float crossProduct = preLine.x * currentLine.y - preLine.y * currentLine.x;
			if( crossProduct > 0 )
			{
				upperChain.pop_back();
			}
			else
			{
				break;
			}
		}

		upperChain.push_back( upper[ i ] );
	}

	lowerChain.push_back( lower[ 0 ] );
	lowerChain.push_back( lower[ 1 ] );
	for( int i = 2; i < lower.size(); i++ )
	{
		for( ; lowerChain.size() >= 2 ; )
		{
			int lowerChainSize = lowerChain.size();
			D3DXVECTOR2 currentLine( lower[ i ]->v->x - lowerChain[ lowerChainSize - 1 ]->v->x, lower[ i ]->v->y - lowerChain[ lowerChainSize - 1 ]->v->y );
			D3DXVECTOR2 preLine( lowerChain[ lowerChainSize - 1 ]->v->x - lowerChain[ lowerChainSize - 2 ]->v->x, lowerChain[ lowerChainSize - 1 ]->v->y - lowerChain[ lowerChainSize - 2 ]->v->y );

			float crossProduct = preLine.x * currentLine.y - preLine.y * currentLine.x;
			if( crossProduct > 0 )
			{
				lowerChain.pop_back();
			}
			else
			{
				break;
			}
		}

		lowerChain.push_back( lower[ i ] );
	}

	upperChain.pop_back();
	lowerChain.pop_back();

	for( int i = 0; i < lowerChain.size(); i++ )
	{
		upperChain.push_back( lowerChain[ i ] );
	}

	return upperChain;
}

void DivideAndConquerFor3DCH::findTangentFor3DCHs( vector<VertexObject*>* ch_one, vector<VertexObject*>* ch_two, VertexObject** ch1_cand, VertexObject** ch2_cand )
{
	double yRecord = 1000000000.0f;
	int    ch1Pointer = 0;
	int    ch2Pointer = 0;
	for( int i = 0; i < ch_one->size(); i++ )
	{
		if( (*ch_one)[ i ]->v->y < yRecord )
		{
			yRecord = (*ch_one)[ i ]->v->y;
			ch1Pointer = i;
		}
		else
		{
			break;
		}
	}

	// Start finding tangent
	bool keepDoing = true;
	do 
	{
		keepDoing = true;
		D3DXVECTOR2 currentTangent;
		D3DXVECTOR2 invCurrentTanget;
		D3DXVECTOR2 tmpLeft;
		D3DXVECTOR2 tmpRight;
		int prePointPtr = 0, nextPointPtr = 0;
		double crossProductOne = 0, crossProductTwo = 0;

		do{
			currentTangent = D3DXVECTOR2( (*ch_one)[ ch1Pointer ]->v->x - (*ch_two)[ ch2Pointer ]->v->x, (*ch_one)[ ch1Pointer ]->v->y - (*ch_two)[ ch2Pointer ]->v->y );

			prePointPtr = 0, nextPointPtr = 0;
			prePointPtr = ( ch1Pointer - 1  + ch_one->size() ) % ch_one->size();
			nextPointPtr = ( ch1Pointer + 1 ) % ch_one->size();

			tmpLeft = D3DXVECTOR2( (*ch_one)[ prePointPtr ]->v->x - (*ch_one)[ ch1Pointer ]->v->x, (*ch_one)[ prePointPtr ]->v->y - (*ch_one)[ ch1Pointer ]->v->y );
			tmpRight = D3DXVECTOR2( (*ch_one)[ nextPointPtr ]->v->x - (*ch_one)[ ch1Pointer ]->v->x, (*ch_one)[ nextPointPtr ]->v->y - (*ch_one)[ ch1Pointer ]->v->y );
			crossProductOne = currentTangent.x * tmpLeft.y - currentTangent.y * tmpLeft.x;
			crossProductTwo = currentTangent.x * tmpRight.y - currentTangent.y * tmpRight.x;

			if( crossProductOne * crossProductTwo < -eps|| ( crossProductOne < eps && crossProductTwo < eps ) )
			{
				ch1Pointer = ( ch1Pointer - 1 + ch_one->size() ) % ch_one->size();
				keepDoing = true;
			}
			else
			{
				keepDoing = false;
			}
		}while( keepDoing );

		do{
			currentTangent = D3DXVECTOR2( (*ch_one)[ ch1Pointer ]->v->x - (*ch_two)[ ch2Pointer ]->v->x, (*ch_one)[ ch1Pointer ]->v->y - (*ch_two)[ ch2Pointer ]->v->y );

			// For convex hull B
			prePointPtr = ( ch2Pointer - 1  + ch_two->size() ) % ch_two->size();
			nextPointPtr = ( ch2Pointer + 1 ) % ch_two->size();

			tmpLeft = D3DXVECTOR2( (*ch_two)[ prePointPtr ]->v->x - (*ch_two)[ ch2Pointer ]->v->x, (*ch_two)[ prePointPtr ]->v->y - (*ch_two)[ ch2Pointer ]->v->y );
			tmpRight = D3DXVECTOR2( (*ch_two)[ nextPointPtr ]->v->x - (*ch_two)[ ch2Pointer ]->v->x, (*ch_two)[ nextPointPtr ]->v->y - (*ch_two)[ ch2Pointer ]->v->y );
			crossProductOne = currentTangent.x * tmpLeft.y - currentTangent.y * tmpLeft.x;
			crossProductTwo = currentTangent.x * tmpRight.y - currentTangent.y * tmpRight.x;

			if( crossProductOne * crossProductTwo < -eps || ( crossProductOne < eps && crossProductTwo < eps ) )
			{
				ch2Pointer = ( ch2Pointer + 1 ) % ch_two->size();
				keepDoing = true;
			}
			else
			{
				keepDoing = false;
		}
		}while( keepDoing );

		currentTangent = D3DXVECTOR2( (*ch_one)[ ch1Pointer ]->v->x - (*ch_two)[ ch2Pointer ]->v->x, (*ch_one)[ ch1Pointer ]->v->y - (*ch_two)[ ch2Pointer ]->v->y );

		prePointPtr = 0, nextPointPtr = 0;
		prePointPtr = ( ch1Pointer - 1  + ch_one->size() ) % ch_one->size();
		nextPointPtr = ( ch1Pointer + 1 ) % ch_one->size();

		tmpLeft = D3DXVECTOR2( (*ch_one)[ prePointPtr ]->v->x - (*ch_one)[ ch1Pointer ]->v->x, (*ch_one)[ prePointPtr ]->v->y - (*ch_one)[ ch1Pointer ]->v->y );
		tmpRight = D3DXVECTOR2( (*ch_one)[ nextPointPtr ]->v->x - (*ch_one)[ ch1Pointer ]->v->x, (*ch_one)[ nextPointPtr ]->v->y - (*ch_one)[ ch1Pointer ]->v->y );
		crossProductOne = currentTangent.x * tmpLeft.y - currentTangent.y * tmpLeft.x;
		crossProductTwo = currentTangent.x * tmpRight.y - currentTangent.y * tmpRight.x;

		if( crossProductOne * crossProductTwo < -eps || ( crossProductOne < eps && crossProductTwo < eps ) )
		{
			ch1Pointer = ( ch1Pointer - 1 + ch_one->size() ) % ch_one->size();
			keepDoing = true;
		}
		else
		{
			keepDoing = false;
		}

	} while ( keepDoing );

	*ch1_cand = (*ch_one)[ ch1Pointer ];
	*ch2_cand = (*ch_two)[ ch2Pointer ];
}

void DivideAndConquerFor3DCH::recursivelyDeleteFace(FaceObject* face, DCEL *dcel)
{
	if ( face->isDeleted ) return;

	face->isDeleted = true;
	HalfedgeObject *e1 = face->attachedEdge;
	HalfedgeObject *e2 = e1->nextEdge;
	HalfedgeObject *e3 = e2->nextEdge;
	VertexObject *v1 = e1->origin;
	VertexObject *v2 = e2->origin;
	VertexObject *v3 = e3->origin;

	for ( int i = 0; i < 3; i++, e1 = e1->nextEdge )
	{
		//if (e1->origin->status != 1 || e1->twins->origin->status != 1)
		if (!e1->visited && !e1->twins->visited)
			recursivelyDeleteFace(e1->twins->attachedFace, dcel);
	}

	if( v1->status != 1 ) v1->status = 2;
	if( v2->status != 1 ) v2->status = 2;
	if( v3->status != 1 ) v3->status = 2;
			
	dcel->remove( e1 );
	dcel->remove( e2 );
	dcel->remove( e3 );
	dcel->remove(face);
}