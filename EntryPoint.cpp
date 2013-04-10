#include "SystemClass.h"

// Test code
#include <vector>
struct point{
	float x;
	float y;
	float z;
};

vector<point> findConvexHull( vector<point>* pointSet, int divideLine );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	
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

	vector<point> pointSet;
	point p11, p12, p13, p14, p21, p22, p23, p24, p00;
	p00.x = 0.0f; p00.y = 0.0f; p00.z = 0.0f;

	p11.x = -0.5f; p11.y = 0.5f; p11.z = 0.5f;
	p12.x = 0.5f; p12.y = 0.5f; p12.z = 0.5f;
	p13.x = 0.5f; p13.y = 0.5f; p13.z = -0.5f;
	p14.x = -0.5f; p14.y = 0.5f; p14.z = -0.5f;

	p21.x = -0.5f; p21.y = -0.5f; p21.z = 0.5f;
	p22.x = 0.5f; p22.y = -0.5f; p22.z = 0.5f;
	p23.x = 0.5f; p23.y = -0.5f; p23.z = -0.5f;
	p24.x = -0.5f; p24.y = -0.5f; p24.z = -0.5f;

	pointSet.push_back( p11 );
	pointSet.push_back( p12 );
	pointSet.push_back( p13 );
	pointSet.push_back( p14 );
	pointSet.push_back( p00 );
	pointSet.push_back( p21 );
	pointSet.push_back( p22 );
	pointSet.push_back( p23 );
	pointSet.push_back( p24 );




	return 0;
}
