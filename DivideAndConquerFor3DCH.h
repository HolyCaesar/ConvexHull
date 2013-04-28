#pragma once

#include "ConvexHullAlgorithm.h"

class DivideAndConquerFor3DCH : public ConvexHullAlgorithm
{
public:
	DivideAndConquerFor3DCH();
	DivideAndConquerFor3DCH( const DivideAndConquerFor3DCH& other );
	~DivideAndConquerFor3DCH();

public:
	DCEL BruceForceCH( vector<VERTEX>* pVertex );
	bool RayTriangleIntersection( Ray r, TRIANGLE triangle, vector<VERTEX>* pVertex );
	void Calculate3DConvexHull( vector<VERTEX>* pVertex );

private:
	DCEL DVCalculate3DConvexHull( vector<VERTEX>* pVertex, int startPoint, int endPoint );
};
