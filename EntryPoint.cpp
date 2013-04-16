#include "SystemClass.h"

#include "DCEL.h"
#include "DivideAndConquerFor3DCH.h"
// Test code
#include <vector>
struct point{
	float x;
	float y;
	float z;
};

vector<point> findConvexHull( vector<point>* pointSet, int divideLine );

void UseConsole()
{
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	UseConsole();

	//// Create the system object.
	//System = new SystemClass;
	//if(!System)
	//{
	//	return 0;
	//}

	//// Initialize and run the system object.
	//result = System->Initialize();
	//if( result )
	//{
	//	System->Run();
	//}

	//// Shutdown and release the system object.
	//System->Shutdown();
	//delete System;
	//System = 0;

	vector<VERTEX> pointSet;
	VERTEX p11, p12, p13, p14, p21, p22, p23, p24, p00;
	//p00.x = 0.0f; p00.y = 0.0f; p00.z = 0.0f;

	//p11.x = -0.5f; p11.y = 0.5f; p11.z = 0.5f;
	//p11.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p12.x = 0.5f; p12.y = 0.5f; p12.z = 0.5f;
	//p12.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p13.x = 0.5f; p13.y = 0.5f; p13.z = -0.5f;
	//p13.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p14.x = -0.5f; p14.y = 0.5f; p14.z = -0.5f;
	//p14.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );

	//p21.x = -0.5f; p21.y = -0.5f; p21.z = 0.5f;
	//p21.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p22.x = 0.5f; p22.y = -0.5f; p22.z = 0.5f;
	//p22.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p23.x = 0.5f; p23.y = -0.5f; p23.z = -0.5f;
	//p23.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	//p24.x = -0.5f; p24.y = -0.5f; p24.z = -0.5f;
	//p24.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );

	p00.x = 0.0f; p00.y = 0.0f; p00.z = 0.0f;

	p11.x = 0.0f; p11.y = 1.0f; p11.z = 0.0f;
	p11.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	p12.x = 1.0f; p12.y = 0.0f; p12.z = 0.0f;
	p12.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	p13.x = 0.0f; p13.y = 0.0f; p13.z = 1.0f;
	p13.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	p14.x = 0.5f; p14.y = 0.5f; p14.z = 0.5f;
	p14.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );

	p21.x = 0.5f; p21.y = 0.2f; p21.z = 0.3f;
	p21.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	p22.x = 0.3f; p22.y = 0.1f; p22.z = 0.1f;
	p22.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );
	p23.x = 0.2f; p23.y = 0.2f; p23.z = 0.6f;
	p23.color = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0 );

	pointSet.push_back( p11 );
	pointSet.push_back( p12 );
	pointSet.push_back( p13 );
	pointSet.push_back( p14 );
	pointSet.push_back( p00 );
	pointSet.push_back( p21 );
	pointSet.push_back( p22 );
	pointSet.push_back( p23 );
	//pointSet.push_back( p24 );

	DivideAndConquerFor3DCH test;
	test.BruceForceCH( &pointSet );
	
	int a;
	cin >> a;
	


	return 0;
}
