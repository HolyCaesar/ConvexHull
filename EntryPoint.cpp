#include "SystemClass.h"

#include "DCEL.h"
#include "DivideAndConquerFor3DCH.h"
#include "PointGenerator.h"

// test code
#include <vector>
#include <list>
#include <algorithm>
#include "GeoDefinition.h"

using namespace std;

void UseConsole()
{
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);

	// introduction
	cout << "/******************************************************************/" << endl;
	cout << "/********  Welcome to our 3D Convex Hull Computation demo  ********/" << endl;
	cout << "/******************************************************************/" << endl << endl;
	cout << "1. Press W or S can rotate the object around x axis" << endl;
	cout << "2. Press A or D can rotate the object around y axis" << endl;
	cout << "3. Press Q or E can rotate the object around z axis" << endl;
	cout << "4. Press Z can zoom in the object" << endl;
	cout << "5. Press X can zoom out the object" << endl;
	cout << "6. Press 1 to change point set generator to InSphere. Point Set S is consist of points inside a sphere" << endl;
	cout << "7. Press 2 to change point set generator to OnSphere. Point Set S is consist of points on a sphere's surface" << endl;
	cout << "8. press 3 to change point set generator to InCube. Point Set S is consist of points inside a cube" << endl;
	cout << "9. press 4 to change point set generator to UnitCube. Point Set S is consist of 8 points that form a cube" << endl;
	cout << "10. press N to change point set generator to UnitCube. Add 10 points to current point set" << endl;
	cout << "11. press M to change point set generator to UnitCube. Subtract 10 points from current point set" << endl;
	cout << "12. press ESC to exit" << endl;
}

bool cmp(const VERTEX &a, const VERTEX &b)
{
	return a.y > b.y;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	UseConsole();

	// Create the system object.
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

	vector<VERTEX> pointSet = PointGenerator::pointsOnSphere( 100 );

	IncrementalHull3DFast rjb(pointSet);
	cout << rjb.dcel << endl;



	sort( pointSet.begin(), pointSet.end(), cmp );
	//for (int i=0; i<pointSet.size(); i++)
	//	cout << pointSet[i] << endl;
	//char jb; cin >> jb;

	DivideAndConquerFor3DCH dc;
	DCEL dcel = dc.DVCalculate3DConvexHull( &pointSet, 0, pointSet.size() - 1, 0 );
	cout << dcel << endl;

	int a;
	cin >> a;

	return 0;
}
