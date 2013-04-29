//
//  IncrementalHull3DFast.h
//  ConvexHull3D
//
//  Created by Yuan Li on 4/23/13.
//  Copyright (c) 2013 Yuan Li. All rights reserved.
//

#ifndef ConvexHull3D_IncrementalHull3DFast_h
#define ConvexHull3D_IncrementalHull3DFast_h


#include "GeoDefinition.h"
#include "DCEL.h"
#include <set>
using namespace std;


typedef set<VertexObject*>* GV;
typedef set<FaceObject*>* GF;

class IncrementalHull3DFast
{
	vector<VERTEX> a;
	vector<VertexObject*> vObj;
	vector<HalfedgeObject*> newEdge;
	vector<int> outDegree;
    vector<int> next;
    vector<int> tmp;
    vector<FaceObject*> tmpFace;
    set<FaceObject*> faceToBeDel;
    vector<HalfedgeObject*> tmpHalfedge;
    int current;
    

    
    
	VECTOR findNormal(const VERTEX &a, const VERTEX &b, const VERTEX &c)
	{
		return VECTOR(a, b).cross(VECTOR(a, c)).normalize();
	}
    
    // Triangle pqr; s is the point outside
    // If s == -1, then the order of p, q, r should be correct.
	FaceObject* addTriangle(int p, int q, int r, int s = -1)
	{
		VECTOR normal = findNormal(a[p], a[q], a[r]);
		if (s != -1 && normal.dot(VECTOR(a[p], a[s])) > 0)
		{
			swap(p, q);
			normal = findNormal(a[p], a[q], a[r]);
			assert(normal.dot(VECTOR(a[p], a[s])) < 0);
		}
		
        
		HalfedgeObject* e1 = new HalfedgeObject;
		HalfedgeObject* e2 = new HalfedgeObject;
		HalfedgeObject* e3 = new HalfedgeObject;

		FaceObject* f = new FaceObject;
        f->data = new set<VertexObject*>;
		f->normal = normal;
		f->attachedEdge = e1;
        
		dcel.add(f);
		dcel.add(e1);
		dcel.add(e2);
		dcel.add(e3);
		
		e1->origin = vObj[p]; outDegree[p]++;
		e2->origin = vObj[q]; outDegree[q]++;
		e3->origin = vObj[r]; outDegree[r]++;
        
		vObj[p]->leaving = e1;
		vObj[q]->leaving = e2;
		vObj[r]->leaving = e3;
        
		e1->attachedFace = e2->attachedFace = e3->attachedFace = f;
		
		e1->nextEdge = e2; e2->nextEdge = e3; e3->nextEdge = e1;
		e1->preEdge = e3; e2->preEdge = e1; e3->preEdge = e2;
        
        return f;
	}
    
	void createTetrahedron(void)
	{
		if (a.size() < 4) { isDegenerate = true; return; }
        
		// step 1: find a segment
		isDegenerate = true;
		for (int i=1; i<(int)a.size(); i++)
			if (!(a[i] == a[0]))
			{
				swap(a[i], a[1]);
				isDegenerate = false;
				break;
			}
		if (isDegenerate) return;
        
		// step 2: find a triangle
		isDegenerate = true;
		for (int i=2; i<(int)a.size(); i++)
		{
			VECTOR p1(a[0], a[1]);
			VECTOR p2(a[0], a[i]);
			VECTOR res = p1.cross(p2);
			if (res.length() > EPS)
			{
				swap(a[i], a[2]);
				isDegenerate = false;
				break;
			}
		}
		if (isDegenerate) return;
        
		// step 3: find a tetrahedron
		isDegenerate = true;
		VECTOR normal = findNormal(a[0], a[1], a[2]);
		float d = normal.dot(VECTOR(a[0]));
		for (int i=3; i<(int)a.size(); i++)
		{
			float d_ = normal.dot(VECTOR(a[i]));
			if (fabs(d - d_) > EPS)
			{
				swap(a[i], a[3]);
				isDegenerate = false;
				break;
			}
		}
		if (isDegenerate) return;
        
		// Now a[0], a[1], a[2], a[3] form a tetrahedron
		for (int i=0; i<(int)a.size(); i++)
		{
			a[i].id = i;
			VertexObject *tmp = new VertexObject;
			tmp->leaving = NULL;
			tmp->v = &a[i];
			vObj.push_back(tmp);
            vObj[i]->data = new set<FaceObject*>;
		}
		
        dcel.add(vObj[0]);
        dcel.add(vObj[1]);
        dcel.add(vObj[2]);
        dcel.add(vObj[3]);
        
		addTriangle(0, 1, 2, 3);
		addTriangle(0, 1, 3, 2);
		addTriangle(0, 2, 3, 1);
		addTriangle(1, 2, 3, 0);
		
		// At last, fix twin pointers
		for (list<HalfedgeObject*>::iterator i=dcel.m_HalfEdges->begin(); i!=dcel.m_HalfEdges->end(); i++)
			for (list<HalfedgeObject*>::iterator j=dcel.m_HalfEdges->begin(); j!=dcel.m_HalfEdges->end(); j++)
			{
				HalfedgeObject* e1 = *i;
				HalfedgeObject* e2 = *j;
				if (e1->nextEdge->origin == e2->origin && e2->nextEdge->origin == e1->origin) // should use both!!
					e1->twins = e2;
			}
        
        // Build the graph
        for (int i=4; i<(int)a.size(); i++)
            for (list<FaceObject*>::iterator j=dcel.m_Faces->begin(); j!=dcel.m_Faces->end(); j++)
                if (isConflict(*j, &a[i])) {
                    GF gf = (GF)vObj[i]->data;
                    gf->insert(*j);
                    
                    GV gv = (GV)(*j)->data;
                    gv->insert(vObj[i]);
                }
	}
    
	bool isConflict(FaceObject *f, VERTEX *p)
	{
		return f->normal.dot(VECTOR(*(f->attachedEdge->origin->v), *p)) > EPS;
	}
    
    void printGraph(void) {
        for (int i=0; i<(int)a.size(); i++)
            cout << "vertex " << i << ", degree = " << ((GF)vObj[i]->data)->size() << endl;
        int cnt = 0;
        for (list<FaceObject*>::iterator i=dcel.m_Faces->begin(); i!=dcel.m_Faces->end(); i++)
        {
            cout << "face " << cnt << ", degree = " << ((GV)(*i)->data)->size() << endl;
            cnt++;
        }
    }
    
    inline void deleteFaceVertexInConflictGraph(FaceObject *f)
    {
        GV gv = (GV)f->data;
        for (set<VertexObject*>::iterator i=gv->begin(); i!=gv->end(); i++) {
            GF gf = (GF)(*i)->data;
            gf->erase(find(gf->begin(), gf->end(), f));
        }
        gv->clear();
    }
	void deleteFace(FaceObject *f, VERTEX *p)
	{
		HalfedgeObject* e = f->attachedEdge;
        bool onTheBoundary = false;
		
		for (int i=0; i<3; i++)
		{
			dcel.remove(e);
			if (!isConflict(e->twins->attachedFace, p))
			{
                current = e->origin->v->id;
                next[current] = e->twins->origin->v->id;
                tmpHalfedge[e->twins->origin->v->id] = e->twins;
                onTheBoundary = true;
			}
            
			outDegree[e->origin->v->id]--;
			if (outDegree[e->origin->v->id] == 0)
            {
                dcel.remove(e->origin);
            }
            
            e = e->nextEdge;
		}
        
        dcel.remove(f);
    }
    
    // f is the new face
    // f1 is neighbor invisible face
    // f2 is the visible face to be deleted
    inline void fixGraph(FaceObject *f, VertexObject *p, FaceObject *f1, FaceObject *f2) {
        GV _f = (GV)f->data;
        GV _f1 = (GV)f1->data;
        GV _f2 = (GV)f2->data;

        if (f1->normal == f->normal) {
            for (set<VertexObject*>::iterator i=_f1->begin(); i!=_f1->end(); i++) {
                _f->insert(*i);
                GF gf = (GF)(*i)->data;
                gf->insert(f);
            }
            return ;
        }

        for (set<VertexObject*>::iterator i=_f1->begin(); i!=_f1->end(); i++) {
            if (isConflict(f, (*i)->v)) {
                _f->insert(*i);
                GF gf = (GF)(*i)->data;
                gf->insert(f);
            }
        }

        for (set<VertexObject*>::iterator i=_f2->begin(); i!=_f2->end(); i++) {
            if (isConflict(f, (*i)->v)) {
                _f->insert(*i);
                GF gf = (GF)(*i)->data;
                gf->insert(f);
            }
        }
    }
    
    void fixTwin(int index, int len, VertexObject *p)
    {
        FaceObject* f1 = tmpFace[index];
        FaceObject* f2 = tmpFace[(index + 1) % len];
        HalfedgeObject* e1 = f1->attachedEdge;
        HalfedgeObject* e2 = f2->attachedEdge;
        
        do {
            HalfedgeObject* e = tmpHalfedge[e1->nextEdge->origin->v->id];
            if (e && e1->origin == e->nextEdge->origin && e1->nextEdge->origin == e->origin)
            {
                // fix the graph first 
                fixGraph(tmpFace[index], p, e->attachedFace, e->twins->attachedFace);
                
                e1->twins = e;
                e->twins = e1;
                tmpHalfedge[e->origin->v->id] = NULL;
            }
            e1 = e1->nextEdge;
        } while (e1 != f1->attachedEdge);
        
        do {
            do {
                if (e1->origin == e2->nextEdge->origin && e2->origin == e1->nextEdge->origin ) {
                    e1->twins = e2;
                    e2->twins = e1;
                }
                e2 = e2->nextEdge;
            } while (e2 != f2->attachedEdge);
            e1 = e1->nextEdge;
        } while (e1 != f1->attachedEdge);
    }
    
    
public:
    DCEL dcel;
	bool isDegenerate;
    
	IncrementalHull3DFast(const vector<VERTEX> &p)
	{
		a = p;
        outDegree = vector<int>(a.size());
        next = vector<int>(a.size());
        tmp = vector<int>(a.size());
        tmpFace = vector<FaceObject*>(a.size());
        tmpHalfedge = vector<HalfedgeObject*>(a.size());
        fill(tmpHalfedge.begin(), tmpHalfedge.end(), (HalfedgeObject*)NULL);
        fill(outDegree.begin(), outDegree.end(), 0);
        
		createTetrahedron();
        calcConvexHull();
	}
	void calcConvexHull(void)
	{
		if (isDegenerate) return;
        
		for (int i=4; i<(int)a.size(); i++)
		{
            GF gf = (GF)vObj[i]->data;
            for (set<FaceObject*>::iterator j=gf->begin(); j!=gf->end(); ) {
                if ((*j)->isDeleted) {
                    gf->erase(j++);
                }
                else j++;
            }
            
			bool isInside = gf->empty();


            if (isInside) continue;

            for (set<FaceObject*>::iterator j=gf->begin(); j!=gf->end(); j++)
            {
                deleteFace(*j, &a[i]);
            }

            int len = 1; tmp[0] = current;
            for (int j=next[current]; j!=current; j=next[j])
                tmp[len++] = j;
            
            int p = i;
            for (int j=0; j<len; j++)
            {
                int q = tmp[j];
                int r = tmp[(j+1) % len];
                
                tmpFace[j] = addTriangle(p, q, r);
            }
            
            for (int j=0; j<len; j++) {
                fixTwin(j, len, vObj[p]);
            }

            for (set<FaceObject*>::iterator j=gf->begin(); j!=gf->end(); ) {
                set<FaceObject*>::iterator _j = j++;
                deleteFaceVertexInConflictGraph(*_j);
            }

            dcel.add(vObj[p]);
        }
	}
    
	void test(void)
	{
		for (list<FaceObject*>::iterator i=dcel.m_Faces->begin(); i!=dcel.m_Faces->end(); i++)
		{
			cout << "Face: ";
			HalfedgeObject* j=(*i)->attachedEdge;
			do
			{
				cout << j->origin->v->id << ' ' ;
				j = j->nextEdge;
			}
			while(j != (*i)->attachedEdge);
            
			cout << endl;
		}
        
		for (list<HalfedgeObject*>::iterator i=dcel.m_HalfEdges->begin(); i!=dcel.m_HalfEdges->end(); i++)
		{
			HalfedgeObject* e = *i;
			cout << "Edge = " << e->origin->v->id << "->" << e->nextEdge->origin->v->id << ' '
                 <<  "next = " << e->nextEdge->origin->v->id << "->" << e->nextEdge->nextEdge->origin->v->id << ' '
                 << "twin = " << e->twins->origin->v->id << "->" << e->twins->nextEdge->origin->v->id << endl;
		}
        
		for (list<VertexObject*>::iterator i=dcel.m_Vertexs->begin(); i!=dcel.m_Vertexs->end(); i++)
		{
			cout << "Vertex " << (*i)->v->id << ": ";
			HalfedgeObject *j=(*i)->leaving;
			do
			{
				cout << j->twins->origin->v->id << ' ';
				j = j->twins->nextEdge;
			}
			while(j != (*i)->leaving);
			cout << endl;
		}
	}
};

#endif
