#include "DivideAndConquerFor3DCH.h"

int test = 0;

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

DCEL DivideAndConquerFor3DCH::BruceForceCH( vector<VERTEX>* pVertex )
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
				//face.P1.pointOne = (*pVertex)[ i ];
				//face.P2.pointTwo = (*pVertex)[ j ];
				//face.P3.pointThree = (*pVertex)[ k ];
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
		VERTEX pointOne = (*pVertex)[ triangle.p1.pointOneIndex ];
		VERTEX pointTwo = (*pVertex)[ triangle.p2.pointTwoIndex ];
		VERTEX pointThree = (*pVertex)[ triangle.p3.pointThreeIndex ];

		D3DXVECTOR3 edge1( pointTwo.x - pointOne.x, pointTwo.y - pointOne.y, pointTwo.z - pointOne.z );
		D3DXVECTOR3 edge2( pointThree.x - pointOne.x, pointThree.y - pointOne.y, pointThree.z - pointOne.z );
		// point3 - point1
		//D3DXVECTOR3 edge2( triangle.pointThree.x - triangle.pointOne.x, triangle.pointThree.y - triangle.pointOne.y, triangle.pointThree.z - triangle.pointOne.z );

		D3DXVECTOR3 triangleNormal;
		D3DXVec3Cross( &triangleNormal, &edge1, &edge2 );
		D3DXVECTOR3 rayStartPoint( ( pointOne.x + pointTwo.x + pointThree.x ) / 3.0f,
								   ( pointOne.y + pointTwo.y + pointThree.y ) / 3.0f,
								   ( pointOne.z + pointTwo.z + pointThree.z ) / 3.0f );

		Ray ray, invRay;
		D3DXVec3Normalize( &ray.direction, &triangleNormal );
		ray.position = rayStartPoint;
		invRay = ray;
		invRay.direction *= -1.0;

		bool rayIntersect = false;
		bool invRayIntersect = false;
		
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

	/*
	* TODO Test code, need to be delete
	*/
	cout << finalTriangleSet.size() << endl;
	for( int i = 0; i < finalTriangleSet.size(); i++ )
	{
		cout << "Triange whose points are " << finalTriangleSet[ i ].p1.pointOneIndex << " ," << finalTriangleSet[ i ].p2.pointTwoIndex << " and " << finalTriangleSet[ i ].p3.pointThreeIndex << endl;
	}

	// Use DCEL to represent the convex hull
	//m_pDCEL->createDCEL( &finalTriangleSet, pVertex );

	//list<FaceObject*>::iterator iter;
	//for( iter = m_pDCEL->m_Faces->begin(); iter != m_pDCEL->m_Faces->end(); iter++ )
	//{
	//	//m_pDCEL->test( *iter );
	//}

	DCEL dcel;
	dcel.createDCEL( &finalTriangleSet, pVertex );

	return dcel;
}

bool DivideAndConquerFor3DCH::RayTriangleIntersection( Ray r, TRIANGLE triangle, vector<VERTEX>* pVertex )
{
	VERTEX pointOne = (*pVertex)[ triangle.p1.pointOneIndex ];
	VERTEX pointTwo = (*pVertex)[ triangle.p2.pointTwoIndex ];
	VERTEX pointThree = (*pVertex)[ triangle.p3.pointThreeIndex ];

	D3DXVECTOR3 edge1( pointTwo.x - pointOne.x, pointTwo.y - pointOne.y, pointTwo.z - pointOne.z );
	D3DXVECTOR3 edge2( pointThree.x - pointOne.x, pointThree.y - pointOne.y, pointThree.z - pointOne.z );

	D3DXVECTOR3 triNormal;
	D3DXVec3Cross( &triNormal, &edge1, &edge2 );
	D3DXVec3Normalize( &triNormal, &triNormal );
	double denominator = D3DXVec3Dot( &triNormal, &r.direction );

	// Ray parallels to the plane
	if( fabs( denominator ) < 0.000001 )
	{
		return false;
	}

	double d = triNormal.x * pointOne.x + triNormal.y * pointOne.y + triNormal.z * pointOne.z;
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

	D3DXVECTOR3 tmpEdge( intersectPoint.x - pointOne.x, intersectPoint.y - pointOne.y, intersectPoint.z - pointOne.z );
	D3DXVECTOR3 tmpCrossRes;
	D3DXVec3Cross( &tmpCrossRes, &edge1, &tmpEdge );
	double alpha = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( alpha < 0.0f )
	{
		return false;
	}

	tmpEdge = D3DXVECTOR3( intersectPoint.x - pointTwo.x, intersectPoint.y - pointTwo.y, intersectPoint.z - pointTwo.z);
	D3DXVECTOR3 tmpEdge2( pointThree.x - pointTwo.x, pointThree.y - pointTwo.y, pointThree.z - pointTwo.z );
	D3DXVec3Cross( &tmpCrossRes, &tmpEdge2, &tmpEdge ); 
	double beta = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( beta < 0.0f ) 
	{
		return false;
	}

	tmpEdge = D3DXVECTOR3( intersectPoint.x - pointThree.x, intersectPoint.y - pointThree.y, intersectPoint.z - pointThree.z);
	tmpEdge2 = D3DXVECTOR3( pointOne.x - pointThree.x, pointOne.y - pointThree.y, pointOne.z - pointThree.z );
	D3DXVec3Cross( &tmpCrossRes, &tmpEdge2, &tmpEdge ); 
	double gamma = D3DXVec3Dot( &triNormal, &tmpCrossRes );
	if( gamma < 0.0f )
	{
		return false;
	}

	return true;
}

void DivideAndConquerFor3DCH::Calculate3DConvexHull( vector<VERTEX>* pVertex )
{
	
}

DCEL DivideAndConquerFor3DCH::DVCalculate3DConvexHull( vector<VERTEX>* pVertex, int startPoint, int endPoint )
{
	/*
	* Phase 1: Find two convex hulls
	*/
	if( endPoint - startPoint < 8 )
	{
		vector<VERTEX> tmpVertexSet;
		for( int i = startPoint; i < endPoint - startPoint + 1; i++ )
		{
			tmpVertexSet.push_back( (*pVertex)[ i ] );
		}
		return BruceForceCH( &tmpVertexSet );
	}

	int midPoint = ( endPoint - startPoint ) * 0.5;
	DCEL CH1 = DVCalculate3DConvexHull( pVertex, startPoint, midPoint );
	DCEL CH2 = DVCalculate3DConvexHull( pVertex, midPoint + 1, endPoint );

	/*
	* Phase 2: Merge two convex hulls
	*/
	// BruceForce method to find the tangent of two convex hulls. This make this algorithm no nlogn any more
	list<VertexObject*>::iterator CH1VexIter;
	list<VertexObject*>::iterator CH2VexIter;
	CH1VexIter = CH1.m_Vertexs->begin();
	CH2VexIter = CH2.m_Vertexs->begin();

	VERTEX* v1,* v2;
	VertexObject* v1AtTanVO = NULL, * v2AtTanVO = NULL;
	v1 = (*CH1VexIter++)->v;
	v2 = (*CH2VexIter++)->v;
	// Initialize the first tangent line
	D3DXVECTOR2 CH2DTangent( v1->x - v2->x, v1->y - v2->y );
	for( ; CH1VexIter != CH1.m_Vertexs->end(); CH1VexIter++ )
	{
		for( ; CH2VexIter != CH2.m_Vertexs->end(); CH2VexIter++ )
		{
			v1 = (*CH1VexIter)->v;
			v2 = (*CH2VexIter)->v;

			D3DXVECTOR2 tmpTangent( v1->x - v2->x, v1->y - v2->y );
			int cross2D = CH2DTangent.x * tmpTangent.y - CH2DTangent.y * tmpTangent.x;
			// TmpTangent lies on Ch2DTangent's right side
			if( cross2D > 0 )
			{
				CH2DTangent = tmpTangent;
				v1AtTanVO = *CH1VexIter;
				v2AtTanVO = *CH2VexIter;
			}
		}
	}

	// Find a plane that cross this tangent and parallel z axis
	D3DXVECTOR3 planV1( v1AtTanVO->v->x - v2AtTanVO->v->x, v1AtTanVO->v->y - v2AtTanVO->v->y, v1AtTanVO->v->z - v2AtTanVO->v->z );
	D3DXVECTOR3 planV2( 0.0f, 0.0f, -1.0f * v1AtTanVO->v->z );
	D3DXVECTOR3 planeNormal, tmpPlaneNormal;
	D3DXVec3Cross( &planeNormal, &planV1, &planV2 );
	
	// These two points are two ends of the tangent
	VertexObject* vE1Start = v1AtTanVO, * vE2Start = v2AtTanVO;
	vector<VertexObject*> CHCandidates;
	CHCandidates.push_back( v1AtTanVO );
	CHCandidates.push_back( v2AtTanVO );
    
	do{
		// Start warping this two convex hull
		double maxPlaneAngleE1 = 0.0f, maxPlaneAngleE2 = 0.0f;
		HalfedgeObject* initialHalfEdge, * currentHalfEdge;
		initialHalfEdge = v1AtTanVO->leaving;
		currentHalfEdge = initialHalfEdge->twins;
		VERTEX* testVertex = currentHalfEdge->origin->v;
		VertexObject* resultVertex1 = NULL, * resultVertex2 = NULL;
		while( currentHalfEdge->nextEdge != initialHalfEdge )
		{
			// Find the angle between two plane
			planV1 = D3DXVECTOR3( v1AtTanVO->v->x - testVertex->x, v1AtTanVO->v->y - testVertex->y, v1AtTanVO->v->z - testVertex->z );
			planV2 = D3DXVECTOR3( v2AtTanVO->v->x - v1AtTanVO->v->x, v2AtTanVO->v->y - v1AtTanVO->v->y, v2AtTanVO->v->z - v1AtTanVO->v->z );
			D3DXVec3Cross( &tmpPlaneNormal, &planV1, &planV2 );

			double tmpMaxPlaneAngle = D3DXVec3Dot( &planV1, &planV2 );
			if( tmpMaxPlaneAngle > maxPlaneAngleE1 )
			{
				maxPlaneAngleE1 = tmpMaxPlaneAngle;
				resultVertex1 = currentHalfEdge->origin;
			}

			currentHalfEdge = currentHalfEdge->nextEdge->twins;
			testVertex = currentHalfEdge->origin->v;
		}

		initialHalfEdge = v2AtTanVO->leaving;
		currentHalfEdge = initialHalfEdge->twins;
		testVertex = initialHalfEdge->twins->origin->v;
		while( currentHalfEdge->nextEdge != initialHalfEdge )
		{
			// Find the angle between two plane
			planV1 = D3DXVECTOR3( v2AtTanVO->v->x - testVertex->x, v2AtTanVO->v->y - testVertex->y, v2AtTanVO->v->z - testVertex->z );
			planV2 = D3DXVECTOR3( v1AtTanVO->v->x - v2AtTanVO->v->x, v1AtTanVO->v->y - v2AtTanVO->v->y, v1AtTanVO->v->z - v2AtTanVO->v->z );
			D3DXVec3Cross( &tmpPlaneNormal, &planV1, &planV2 );

			double tmpMaxPlaneAngle = D3DXVec3Dot( &planV1, &planV2 );
			if( tmpMaxPlaneAngle > maxPlaneAngleE2 )
			{
				maxPlaneAngleE2 = tmpMaxPlaneAngle;
				resultVertex2 = currentHalfEdge->origin;
			}

			currentHalfEdge = currentHalfEdge->nextEdge->twins;
			testVertex = currentHalfEdge->origin->v;
		}

		if( maxPlaneAngleE2 >= maxPlaneAngleE1 )
		{
			v2AtTanVO = resultVertex2;
			CHCandidates.push_back( resultVertex2 );
		}
		else if( maxPlaneAngleE2 < maxPlaneAngleE1 )
		{
			v1AtTanVO = resultVertex1;
			CHCandidates.push_back( resultVertex1 );
		}
	}while( vE1Start != v1AtTanVO && vE2Start != v2AtTanVO );

	// Change DCEL structure
	DCEL mergeDCEL;
	D3DXVECTOR3 posYVec( 0.0f, 1.0f, 0.0f );
	list<FaceObject*>::iterator faceIter;
	list<VertexObject*> toBeDeleteVertex;
	for( faceIter = CH1.m_Faces->begin(); faceIter != CH1.m_Faces->end(); faceIter++ )
	{
		VertexObject* v1, * v2, * v3;
		v1 = (*faceIter)->attachedEdge->origin;
		v2 = (*faceIter)->attachedEdge->nextEdge->origin;
		v3 = (*faceIter)->attachedEdge->nextEdge->nextEdge->origin;


		D3DXVECTOR3 curFaceNormal;
		D3DXVec3Cross( &curFaceNormal, &D3DXVECTOR3( v2->v->x - v1->v->x, v2->v->y - v1->v->y, v2->v->z - v1->v->z ), &D3DXVECTOR3( v3->v->x - v2->v->x, v3->v->y - v2->v->y, v3->v->z - v2->v->z ) );
		D3DXVec3Normalize( &curFaceNormal, &curFaceNormal );

		// Face that inside that cylinder
		if( D3DXVec3Dot( &posYVec, &curFaceNormal ) < 0 )
		{
			CH1.deleteFace( (*faceIter) );
		}
	}

	for( faceIter = CH2.m_Faces->begin(); faceIter != CH2.m_Faces->end(); faceIter++ )
	{
		VertexObject* v1, * v2, * v3;
		v1 = (*faceIter)->attachedEdge->origin;
		v2 = (*faceIter)->attachedEdge->nextEdge->origin;
		v3 = (*faceIter)->attachedEdge->nextEdge->nextEdge->origin;

		D3DXVECTOR3 curFaceNormal;
		D3DXVec3Cross( &curFaceNormal, &D3DXVECTOR3( v2->v->x - v1->v->x, v2->v->y - v1->v->y, v2->v->z - v1->v->z ), &D3DXVECTOR3( v3->v->x - v2->v->x, v3->v->y - v2->v->y, v3->v->z - v2->v->z ) );
		D3DXVec3Normalize( &curFaceNormal, &curFaceNormal );

		// Face that inside that cylinder
		if( D3DXVec3Dot( &posYVec, &curFaceNormal ) > 0 )
		{
			CH2.deleteFace( (*faceIter) );
		} 
	}

	return mergeDCEL;
}