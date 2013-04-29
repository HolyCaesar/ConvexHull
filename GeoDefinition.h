#ifndef GEODEFINITION_H
#define GEODEFINITION_H

#include "Header.h"

#include <D3D11.h>
#include <D3D10.h>
#include <D3DX10.h>
#include <D3DX10math.h>

// Added by Yuan Li
#define EPS 1E-8


/*Primitive Oject*/
struct VERTEX
{
	// Added by Yuan Li
	int id;

    float x, y, z;
    D3DXVECTOR4 color;

	VERTEX( float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;

		color = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	VERTEX()
	{
		x = 0;
		y = 0;
		z = 0;

		color = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	void operator *=( float factors )
	{
		x *= factors;
		y *= factors;
		z *= factors;
	}

	const VERTEX operator -( VERTEX p1 )
	{
		return VERTEX( this->x - p1.x, this->y - p1.y, this->z - p1.z );
	}

	// Added by Yuan Li
	VERTEX(const VERTEX &p)
	{
		*this = p;
	}

	bool operator == (const VERTEX &p) const
	{
		return fabs(x - p.x) < EPS && fabs(y - p.y) < EPS && fabs(z - p.z) < EPS;
	}
    
	friend ostream& operator << (ostream &cout, VERTEX v)
	{
		cout << v.x << ' ' << v.y << ' ' << v.z;
		return cout;
    }
};

struct TRIANGLE
{
	union P1
	{
		VERTEX* pointOne;
		int    pointOneIndex;
	}p1;

	union P2
	{
		VERTEX* pointTwo;
		int    pointTwoIndex;
	}p2;

	union P3
	{
		VERTEX* pointThree;
		int    pointThreeIndex;
	}p3;

	friend ostream & operator << ( ostream &cout, TRIANGLE rjb ) 
	{
		cout << rjb.p1.pointOneIndex << ' ' << rjb.p2.pointTwoIndex << ' ' << rjb.p3.pointThreeIndex;
		return cout;
	}
};

struct Ray
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
};

struct VECTOR
{
	float x, y, z;
	VECTOR() { x = y = z = 0; }
	VECTOR(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	VECTOR(const VERTEX &p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}
	VECTOR(const VERTEX &a, const VERTEX &b)
	{
		*this = VECTOR(b.x - a.x, b.y - a.y, b.z - a.z);
	}
	VECTOR(const VECTOR &v) { *this = v; }
    
	float dot(const VECTOR &v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	VECTOR cross(const VECTOR &v)
	{
		return VECTOR(y * v.z - z * v.y, -(x * v.z - z * v.x), x * v.y - y * v.x);
	}
	float length(void)
	{
		return sqrt(x * x + y * y + z * z);
	}
    bool operator == (const VECTOR &p) const
	{
		return fabs(x - p.x) < EPS  && fabs(y - p.y) < EPS && fabs(z - p.z) < EPS;
	}
    VECTOR operator *(float f)
    {
        return VECTOR(x * f, y * f, z * f);
    }
    void operator *= (float f)
    {
        x *= f;
        y *= f;
        z *= f;
    }
	VECTOR normalize()
	{
		VECTOR ans = *this;
		float length = this->length();
		
//		assert(length > EPS);
        
		ans.x /= length;
		ans.y /= length;
		ans.z /= length;
		return ans;
	}
	friend ostream& operator << (ostream &cout, VECTOR v)
	{
		cout << v.x << ' ' << v.y << ' ' << v.z;
		return cout;
    }
};

#endif // STRUCTURES_H