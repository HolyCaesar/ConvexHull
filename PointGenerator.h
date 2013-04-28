//
//  PointGenerator.h
//  ConvexHull3D
//
//  Created by Yuan Li on 4/22/13.
//  Copyright (c) 2013 Yuan Li. All rights reserved.
//

#ifndef __ConvexHull3D__PointGenerator__
#define __ConvexHull3D__PointGenerator__

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <set>
using namespace std;

#include "GeoDefinition.h"

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
        
class PointGenerator
{
public:
    static void seed()
    {
        srand((unsigned int)time(0));
    }
            
            
    // Will a return a floating point in the form 0.XXXXX
    inline static float nextRandom()
    {
        static char s[11];
        static float ans;
        sprintf(s, "0.%05d", rand() % 100000);
        sscanf(s, "%f", &ans);
        
        return ans;
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
                v.x /= length; if (rand() & 1) v.x *= -1;
                v.y /= length; if (rand() & 1) v.y *= -1;
                v.z /= length; if (rand() & 1) v.z *= -1;
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
                
                v.x *= scale; if (rand() & 1) v.x *= -1;
                v.y *= scale; if (rand() & 1) v.y *= -1;
                v.z *= scale; if (rand() & 1) v.z *= -1;
            }
            while(hash.find(v) != hash.end());
            
            *i = v;
            hash.insert(v);
        }
        return ans;
    }
    
    static vector<VERTEX> unitCube(void) {
        vector<VERTEX> ans;
        ans.push_back(VERTEX(0, 0, 0));
        ans.push_back(VERTEX(1, 0, 0));
        ans.push_back(VERTEX(0, 1, 0));
        ans.push_back(VERTEX(1, 1, 0));
        ans.push_back(VERTEX(0, 0, 1));
        ans.push_back(VERTEX(1, 0, 1));
        ans.push_back(VERTEX(0, 1, 1));
        ans.push_back(VERTEX(1, 1, 1));
        return ans;
    }
    
    static vector<VERTEX> tetrahedron(void) {
        vector<VERTEX> ans;
        ans.push_back(VERTEX(0, 0, 0));
        ans.push_back(VERTEX(1, 0, 0));
        ans.push_back(VERTEX(0, 1, 0));
        ans.push_back(VERTEX(0, 0, 1));
//                ans.push_back(VERTEX(-.5, -.5, -.5));
        return ans;
    }
};


#endif /* defined(__ConvexHull3D__PointGenerator__) */
