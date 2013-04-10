#include "DCEL.h"

DCEL::DCEL()
{
}

DCEL::DCEL( const DCEL& other )
{
}

DCEL::~DCEL()
{
}

void DCEL::CreateDCEL( vector<TRIANGLE>* pTriangles )
{
	for( int i = 0; i < pTriangles->size(); i++ )
	{
		TRIANGLE triangle = (*pTriangles)[ i ];

		
	}
}
