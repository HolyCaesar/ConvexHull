#pragma once

#include "Header.h"
#include "GeoDefinition.h"

struct HalfedgeObject;
struct VertexObject;
struct FaceObject;

/*DCEL Structures*/
struct HalfedgeObject
{
	VertexObject*   origin;
	FaceObject*     attachedFace;
	HalfedgeObject* twins;
	HalfedgeObject* nextEdge;
	HalfedgeObject* preEdge;
};

struct VertexObject
{
	HalfedgeObject* leaving;
};

struct FaceObject
{
	HalfedgeObject* attachedEdge;
};

class DCEL
{
private:
	list<HalfedgeObject*>* m_HalfEdges;
	list<VertexObject*>*   m_Vertexs;
	list<FaceObject*>*     m_Faces;

public:
	DCEL();
	DCEL( const DCEL& other );
	~DCEL();

	void CreateDCEL( vector<TRIANGLE>* pTriangles, vector<VERTEX>* pVertex );

private:
	template<class T>
	void cleanList( list<T>* target );

	// Given a half edge, find its previous edge and next edge
	void findPreNextEdges( VertexObject* targetVertex1, VertexObject* targetVertex2, HalfedgeObject* preEdge, HalfedgeObject* nextEdge );

};