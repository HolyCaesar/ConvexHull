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

	// Added by Yuan Li, for deletion
	list<HalfedgeObject*>::iterator it;
    bool visited;
};

struct VertexObject
{
	HalfedgeObject* leaving;
	// 0 unvisited, 1 remain, 2 deleted
	int  status;

	// Added by Yuan Li
	VERTEX *v;
	list<VertexObject*>::iterator it;
	void* data;
    bool visited;
};

struct FaceObject
{
	HalfedgeObject* attachedEdge;

	// Added by Yuan Li
	VECTOR normal;
	list<FaceObject*>::iterator it;
	void* data;
    bool isDeleted;
    FaceObject() { isDeleted = false; }
};

class DCEL
{
public:
	list<HalfedgeObject*>* m_HalfEdges;
	list<VertexObject*>*   m_Vertexs;
	list<FaceObject*>*     m_Faces;

public:
	DCEL();
	DCEL( const DCEL& other );
	~DCEL();

	void unsetVisited(void)
	{
		for (list<HalfedgeObject*>::iterator i=m_HalfEdges->begin(); i!=m_HalfEdges->end(); i++)
			(*i)->visited = false;
		for (list<FaceObject*>::iterator i=m_Faces->begin(); i!=m_Faces->end(); i++)
			(*i)->isDeleted = false;
		for (list<VertexObject*>::iterator i=m_Vertexs->begin(); i!=m_Vertexs->end(); i++)
			(*i)->visited = false;
	}
	void createDCEL( vector<TRIANGLE>* pTriangles, vector<VERTEX*>* pVertex, const unsigned int offset );
	void clean();

	// Added by Yuan Li
	inline void add(HalfedgeObject* e)
    {
        m_HalfEdges->push_back(e);
        e->it = m_HalfEdges->end();
		e->it--; 
    }

    inline void add(VertexObject* v)
    {
        m_Vertexs->push_back(v);
        v->it = m_Vertexs->end();
		v->it--;
	}

    inline void add(FaceObject* f)
    {
        m_Faces->push_back(f);
        f->it = m_Faces->end();
		f->it--;
    }

    inline void remove(HalfedgeObject* e)
    {
        m_HalfEdges->erase(e->it);
    }

    inline void remove(VertexObject* v)
    {
        m_Vertexs->erase(v->it);
    }

    inline void remove(FaceObject* f)
    {
        m_Faces->erase(f->it);
    }

    friend ostream& operator << (ostream& cout, DCEL dcel)
    {
        cout << "Vertices: ";
        for (list<VertexObject*>::iterator i=dcel.m_Vertexs->begin(); i!=dcel.m_Vertexs->end(); i++)
        cout << (*i)->v->id << ' ';
        cout << endl;
        
        cout << "Edges:\n";
        for (list<HalfedgeObject*>::iterator i=dcel.m_HalfEdges->begin(); i!=dcel.m_HalfEdges->end(); i++)
        {
            HalfedgeObject* e = *i;
            cout << e->origin->v->id << "->" << e->twins->origin->v->id << endl;
        }
        cout << endl;
        
        cout << "Faces:\n";
        for (list<FaceObject*>::iterator i=dcel.m_Faces->begin(); i!=dcel.m_Faces->end(); i++)
        {
            HalfedgeObject* e = (*i)->attachedEdge;
            cout << e->origin->v->id << ' '; e = e->nextEdge;
            cout << e->origin->v->id << ' '; e = e->nextEdge;
            cout << e->origin->v->id << ' '; e = e->nextEdge;
            cout << endl;
        }
        
        cout << "# of vertices = " << dcel.m_Vertexs->size() << endl;
        cout << "# of edges = " << dcel.m_HalfEdges->size() << endl;
        cout << "# of faces = " << dcel.m_Faces->size();
        return cout;
    }

public:
	void deleteFace( FaceObject* faceObject );
	void deleteEdge( HalfedgeObject* edgeObject );
private:
	template<class T>
	void cleanList( list<T>* target );

	// Given a half edge, find its previous edge and next edge
	void findPreNextEdges( VertexObject* refVertex, VertexObject* targetVertex1, VertexObject* targetVertex2, HalfedgeObject* preEdge, HalfedgeObject* nextEdge );

};
