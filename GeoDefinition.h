#ifndef GEODEFINITION_H
#define GEODEFINITION_H

#include "Header.h"

#include <D3D11.h>
#include <D3D10.h>
#include <D3DX10.h>
#include <D3DX10math.h>

/*Primitive Oject*/
struct VERTEX
{
    float x, y, z;
    D3DXVECTOR4 color;

	VERTEX( float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}

	VERTEX()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	const VERTEX operator -( VERTEX p1 )
	{
		return VERTEX( this->x - p1.x, this->y - p1.y, this->z - p1.z );
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

	friend ostream & operator << (ostream &cout, TRIANGLE rjb) 
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

#endif // STRUCTURES_H