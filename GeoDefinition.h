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
};

struct Ray
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
};

#endif // STRUCTURES_H