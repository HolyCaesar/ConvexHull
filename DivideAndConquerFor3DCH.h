#pragma once

#include "ConvexHullAlgorithm.h"

const float eps = 0.000001;

class DivideAndConquerFor3DCH : public ConvexHullAlgorithm
{
public:
	DivideAndConquerFor3DCH();
	DivideAndConquerFor3DCH( const DivideAndConquerFor3DCH& other );
	~DivideAndConquerFor3DCH();

public:
	DCEL BruceForceCH( vector<VERTEX*>* pVertex, const unsigned int offset );
	DCEL DVCalculate3DConvexHull( vector<VERTEX>* pVertex, const int startPoint, const int endPoint, const unsigned int offset );
	vector<VertexObject*> find2DConvexHull( list<VertexObject*> *pointSet );
	void findTangentFor3DCHs( vector<VertexObject*>* ch_one, vector<VertexObject*>* ch_two, VertexObject** ch1_cand, VertexObject** ch2_cand );
private:
	bool RayTriangleIntersection( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex );
	bool isNormal( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex );
	void recursivelyDeleteFace(FaceObject* face, DCEL *dcel);
	float sign(float a)
	{
		if (a > eps) return 1;
		else if (a < -eps) return -1;
		else return 0;
	}
};
