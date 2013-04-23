//
//  PointGenerator.h
//  ConvexHull3D
//
//  Created by Yuan Li on 4/22/13.
//  Copyright (c) 2013 Yuan Li. All rights reserved.
//

#ifndef __ConvexHull3D__PointGenerator__
#define __ConvexHull3D__PointGenerator__

#include "Header.h"
#include "GeoDefinition.h"

class PointGenerator
{
private:
    struct cmp
    {
        bool operator() (const VERTEX &a, const VERTEX &b) const
        {
            if (a.x < b.x) return true;
            if (a.x > b.x) return false;
            if (a.y < b.y) return true;
            if (a.y > b.y) return false;
            return a.z < b.z;
        }
    };

public:
    static void seed()
    {
        srand((unsigned int)time(0));
    }
    inline static float nextRandom()
    {
        return (float)rand() / (float)RAND_MAX;
    }
    
    static vector<VERTEX> pointsInCube(int n = 10)
    {
        set<VERTEX, cmp> hash;
        vector<VERTEX> ans(n);
        for (vector<VERTEX>::iterator i=ans.begin(); i!=ans.end(); i++)
        {
            VERTEX v;
            do
            {
                v = VERTEX(nextRandom(), nextRandom(), nextRandom());
            }
            while(hash.find(v) != hash.end());
            
            *i = v;
            hash.insert(v);
        }
        return ans;
    }
    
    static vector<VERTEX> pointsOnSphere(int n = 10)
    {
        vector<VERTEX> ans(n);
        set<VERTEX, cmp> hash;
        for (vector<VERTEX>::iterator i=ans.begin(); i!=ans.end(); i++)
        {
            VERTEX v;
            do
            {
                v = VERTEX(nextRandom(), nextRandom(), nextRandom());
                float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                v.x /= length;
                v.y /= length;
                v.z /= length;
            }
            while(hash.find(v) != hash.end());
            
            *i = v;
            hash.insert(v);
        }
        return ans;
    }
    
    static vector<VERTEX> pointsInSphere(int n = 10)
    {
        vector<VERTEX> ans(n);
        set<VERTEX, cmp> hash;
        for (vector<VERTEX>::iterator i=ans.begin(); i!=ans.end(); i++)
        {
            VERTEX v;
            do
            {
                v = VERTEX(nextRandom(), nextRandom(), nextRandom());
                float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                v.x /= length;
                v.y /= length;
                v.z /= length;
                float scale = nextRandom();
                
                v.x *= scale;
                v.y *= scale;
                v.z *= scale;
            }
            while(hash.find(v) != hash.end());
            
            *i = v;
            hash.insert(v);
        }
        return ans;
    }
};


#endif /* defined(__ConvexHull3D__PointGenerator__) */
