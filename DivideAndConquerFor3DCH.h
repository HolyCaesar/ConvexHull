#pragma once

#include "ConvexHullAlgorithm.h"

const float eps = 0.000001;

class DivideAndConquerFor3DCH : public ConvexHullAlgorithm
{
public:
	DivideAndConquerFor3DCH(bool generateAnimation = false);
	DivideAndConquerFor3DCH( const DivideAndConquerFor3DCH& other, bool generateAnimation = false );
	~DivideAndConquerFor3DCH();

public:
	DCEL BruceForceCH( vector<VERTEX*>* pVertex, const unsigned int offset );
	DCEL DVCalculate3DConvexHull( vector<VERTEX>* pVertex, const int startPoint, const int endPoint, const unsigned int offset );
	vector<VertexObject*> find2DConvexHull( list<VertexObject*> *pointSet );
	void findTangentFor3DCHs( vector<VertexObject*>* ch_one, vector<VertexObject*>* ch_two, VertexObject** ch1_cand, VertexObject** ch2_cand );
	vector<vector<VERTEX> > animation;

public:
	bool generateAnimation;

private:
	bool RayTriangleIntersection( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex );
	bool isNormal( Ray r, TRIANGLE triangle, const vector<VERTEX*>* pVertex );
	void recursivelyDeleteFace(FaceObject* face, DCEL *dcel);
	float sign( float a )
	{
		if (a > eps ) return 1;
		else if (a < -eps ) return -1;
		else return 0;
	}
	void addOneStep(FaceObject *face)
	{
		vector<VERTEX> res;
		if (animation.size() > 0) 
			res = animation[ animation.size() - 1 ];
		HalfedgeObject* e = face->attachedEdge;
		for (int i=0; i<3; i++, e = e->nextEdge)
			res.push_back( *e->origin->v );
		animation.push_back( res );		
	}
	void addOneStep(DCEL *dcel)
	{
		vector<VERTEX> res;
		for (list<FaceObject*>::iterator i=dcel->m_Faces->begin(); i!=dcel->m_Faces->end(); i++)
		{
			res.push_back( *(*i)->attachedEdge->origin->v) ;
			res.push_back( *(*i)->attachedEdge->nextEdge->origin->v );
			res.push_back( *(*i)->attachedEdge->nextEdge->nextEdge->origin->v );
		}
		animation.push_back( res );
	}
};
