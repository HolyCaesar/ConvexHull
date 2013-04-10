#ifndef CONSTANTS_H
#define CONSTANTS_H

#pragma comment(lib, "d3dx10.lib")

#include <D3D11.h>
#include <D3D10.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3DX10math.h>

/*D3DRenderWidght related constants*/
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR  = 0.1f;
const bool  VSYNC_ENABLED = true;
const bool  FULL_SCREEN = false;

/*D3D Constant buffers used by shaders*/
struct matrixBufferType
{
    D3DXMATRIX world;
    D3DXMATRIX view;
    D3DXMATRIX projection;
};

#endif // CONSTANTS_H