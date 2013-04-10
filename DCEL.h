#pragma once

#include "Header.h"
#include "GeoDefinition.h"

struct HalfedgeObject;
struct VertexObject;
struct FaceObject;

class DCEL
{
private:
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

private:
	vector<HalfedgeObject> m_HalfEdges;
	vector<VertexObject>   m_Vertexs;
	vector<FaceObject>     m_Faces;

public:
	DCEL();
	DCEL( const DCEL& other );
	~DCEL();

	void CreateDCEL( vector<TRIANGLE>* pTriangles );

};