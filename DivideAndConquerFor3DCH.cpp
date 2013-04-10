#include "DivideAndConquerFor3DCH.h"

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
	vector<TRIANGLE> triangleSet;

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
				face.pointOne = (*pVertex)[ i ];
				face.pointTwo = (*pVertex)[ j ];
				face.pointThree = (*pVertex)[ k ];
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
		D3DXVECTOR3 edge1( triangle.pointTwo.x - triangle.pointOne.x, triangle.pointTwo.y - triangle.pointOne.y, triangle.pointTwo.z - triangle.pointOne.z );
		// point3 - point1
		D3DXVECTOR3 edge2( triangle.pointThree.x - triangle.pointOne.x, triangle.pointThree.y - triangle.pointOne.y, triangle.pointThree.z - triangle.pointOne.z );

		D3DXVECTOR3 triangleNormal;
		D3DXVec3Cross( &triangleNormal, &edge1, &edge2 );
		D3DXVECTOR3 rayStartPoint( ( triangle.pointOne.x + triangle.pointTwo.x + triangle.pointThree.x ) / 3.0f,
								   ( triangle.pointOne.y + triangle.pointTwo.y + triangle.pointThree.y ) / 3.0f,
								   ( triangle.pointOne.z + triangle.pointTwo.z + triangle.pointThree.z ) / 3.0f );

		Ray ray, invRay;
		D3DXVec3Normalize( &ray.direction, &triangleNormal );
		ray.position = rayStartPoint;
		invRay = ray;
		invRay.direction *= -1.0;

		for( int j = i + 1; j < triangleSet.size(); j++ )
		{
			bool rayIntersect = RayTriangleIntersection( ray, triangleSet[ j ] );
			bool invRayIntersect = RayTriangleIntersection( invRay, triangleSet[ j ] );

			// Remove the triangle that inside the convex hull
			if( rayIntersect == true && invRayIntersect == true )
			{
				triangleSet.erase( triangleSet.begin() + j );
			}
			// This is the face that contribute to the convex hull and find its vertices order
			else if( rayIntersect == false && invRayIntersect == true )
			{
				VERTEX tmpVer = triangleSet[ i ].pointTwo;
				triangleSet[ i ].pointTwo = triangleSet[ i ].pointThree;
				triangleSet[ i ].pointThree = tmpVer;
			}
			// The symetric case can be ignored.
		}
	}

	// Use DCEL to represent the convex hull

}

bool DivideAndConquerFor3DCH::RayTriangleIntersection( Ray r, TRIANGLE triangle )
{
	D3DXVECTOR3 edge1( triangle.pointTwo.x - triangle.pointOne.x, triangle.pointTwo.y - triangle.pointOne.y, triangle.pointTwo.z - triangle.pointOne.z );
	D3DXVECTOR3 edge2( triangle.pointThree.x - triangle.pointOne.x, triangle.pointThree.y - triangle.pointOne.y, triangle.pointThree.z - triangle.pointOne.z );

	D3DXVECTOR3 s1;
	D3DXVec3Cross( &s1, &r.direction, &edge2 );

	float divisor = D3DXVec3Dot( &s1, &edge1 );
	if( divisor == 0.0 )
	{
		return false;
	}

	float invDivisor = 1 / divisor;

	D3DXVECTOR3 distance( r.position.x - triangle.pointOne.x, r.position.y - triangle.pointOne.y, r.position.z - triangle.pointOne.z );
	float barycCoord_1 = D3DXVec3Dot( &distance, &s1 ) * invDivisor;
	if(	barycCoord_1 < 0.0 || barycCoord_1 > 1.0 )
	{
		return false;
	}

	D3DXVECTOR3 s2;
	D3DXVec3Cross( &s2, &distance, &edge1 );
	float barycCoord_2 = D3DXVec3Dot( &r.direction, &s2 ) * invDivisor;
	if( barycCoord_2 < 0.0 || ( barycCoord_1 + barycCoord_2 ) > 1.0 )
	{
		return false;
	}

	return true;
}