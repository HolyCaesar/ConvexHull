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
	VERTEX pointOne;
	VERTEX pointTwo;
	VERTEX pointThree;
};

struct Ray
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
};

#endif // STRUCTURES_H