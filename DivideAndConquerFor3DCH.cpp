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

void DivideAndConquerFor3DCH::BruceForceCH( vector<VERTEX>* pVertex )
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
	m_pDCEL->CreateDCEL( &finalTriangleSet, pVertex );

	list<FaceObject*>::iterator iter;
	for( iter = m_pDCEL->m_Faces->begin(); iter != m_pDCEL->m_Faces->end(); iter++ )
	{
		m_pDCEL->test( *iter );
	}
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