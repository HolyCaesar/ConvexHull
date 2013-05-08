#include "SystemClass.h"
#include "DCEL.h"
using namespace std;

#include <algorithm>

bool cmp(const VERTEX &a, const VERTEX &b)
{
	return a.y > b.y;
}

SystemClass::SystemClass()
{
	m_input = 0;
	m_gui   = 0;
	m_graphicsEngine = 0;
	m_screenWidth = 0;
	m_screenHeight = 0;
	m_fullScreen = false;
	m_chAlgorithm = 0;
	m_updateModelFlag = false;

	pointGeneratorMode = 1;
	pointNumber = 5;
	//PointGenerator::seed();
	m_divideandconquerMethod = 0;
	m_testPointSet = PointGenerator::pointsOnSphere( 10 );
	m_incrementalMethod = 0;
}

SystemClass::SystemClass( const SystemClass& other )
{

}

SystemClass::~SystemClass()
{
	delete m_incrementalMethod;
}

void SystemClass::Set3DCHAlgorithm( int algorithm )
{
	m_chAlgorithm = algorithm;
	m_updateModelFlag = true;
}

void SystemClass::SetPointNum( unsigned int num )
{
	pointNumber = num;
	m_updateModelFlag = true;
}

void SystemClass::SetRNGModel( unsigned int model )
{
	pointGeneratorMode = model;
	m_updateModelFlag = true;
}

void SystemClass::SetScale( float factor )
{
	m_graphicsEngine->Scale( factor );
}

bool SystemClass::Initialize()
{
	bool result;

	/*Initialize the windows api*/
	InitializeWindows();

	/*Create the input object.  This object will be used to handle reading the keyboard input from the user.*/
	m_input = new Input;
	if( !m_input )
	{
		return false;
	}

	/*Initialize the input object.*/
	m_input->Initialize();

	/*Create the graphics object.  This object will handle rendering all the graphics for this application.*/
	m_graphicsEngine = new Graphics;
	if( !m_graphicsEngine )
	{
		return false;
	}

	/*Initialize the graphics object.*/
	result = m_graphicsEngine->Initialize( m_screenWidth, m_screenHeight, m_hwnd );
	if(!result)
	{
		return false;
	}

	/*Initialize the GUI*/
	m_gui = new GUI( this );
	if( !m_gui )
	{
		return false;
	}
	m_gui->InitializeGUI( m_screenWidth, m_screenHeight, m_graphicsEngine->GetD3DDevice() );

	m_divideandconquerMethod = new DivideAndConquerFor3DCH( m_gui->GetAnimatedInfo() );
	if( !m_divideandconquerMethod )
	{
		return false;
	}

	m_incrementalMethod = new IncrementalHull3DFast( m_testPointSet, m_gui->GetAnimatedInfo() );
	if( !m_incrementalMethod )
	{
		return false;
	}
	
	m_graphicsEngine->SetModelData( &(m_incrementalMethod->dcel), NULL );

	return true;
}

void SystemClass::Shutdown()
{
	/*Release the graphics object.*/
	if( m_graphicsEngine )
	{
		m_graphicsEngine->Shutdown();
		delete m_graphicsEngine;
		m_graphicsEngine = NULL;
	}

	/*Release the input object.*/
	if( m_input )
	{
		delete m_input;
		m_input = NULL;
	}

	/*Shutdown the window.*/
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	/*Initialize the message structure.*/
	ZeroMemory( &msg, sizeof( MSG ) );
	
	/*/Loop until there is a quit message from the window or the user.*/
	done = false;
	while( !done )
	{
		/*Handle the windows messages.*/
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if(msg.message == WM_QUIT) { done = true; }
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);
		}
		/*If windows signals to end the application then exit out.*/
		//if( msg.message == WM_QUIT )
		//{
		//	done = true;
		//}
		else
		{
			/*Otherwise do the frame processing.*/
			result = Frame();
			
			if( !result )
			{
				done = true;
			}
		}
	}
}

void SystemClass::ComputeCH()
{

}

bool SystemClass::Frame()
{
	bool result;

	/*Check if the user pressed escape and wants to exit the application.*/
	if( m_input->IsKeyDown( VK_ESCAPE ) )
	{
		return false;
	}

	if( m_updateModelFlag )
	{
		// Generate the point set
		//cout << "RNG model " << pointGeneratorMode << endl;
		//cout << "point NUum " << pointNumber << endl;

		switch( pointGeneratorMode )
		{
		case 0:
			m_testPointSet = PointGenerator::pointsInSphere( pointNumber );
			break;
		case 1:
			cout << pointNumber << endl;
			m_testPointSet = PointGenerator::pointsOnSphere( pointNumber );
			break;
		case 2:
			m_testPointSet = PointGenerator::pointsInCube( pointNumber );
			break;
		case 3:
			m_testPointSet = PointGenerator::unitCube();
		default:
			break;
		}

		// Chose different algorithm
		switch ( m_chAlgorithm )
		{
		case 0:
			{
				if( m_gui->GetAnimatedInfo() )
				{
					if( !m_incrementalMethod )
					{
						m_incrementalMethod = new IncrementalHull3DFast( m_testPointSet, m_gui->GetAnimatedInfo() );
					}

					m_gui->SetMaxStep( m_incrementalMethod->animation.size() - 1 );
					m_graphicsEngine->SetModelData( NULL, &( m_incrementalMethod->animation[ m_gui->GetAnimationStep() ] ) );
				}
				else
				{
					if( m_incrementalMethod )
					{
						delete m_incrementalMethod;
					}
					m_incrementalMethod = new IncrementalHull3DFast( m_testPointSet, m_gui->GetAnimatedInfo() );

					m_graphicsEngine->SetModelData( &( m_incrementalMethod->dcel ), NULL );
				}
				break;
			}
		case 1:
			{
				sort( m_testPointSet.begin(), m_testPointSet.end(), cmp );

				if( m_gui->GetAnimatedInfo() )
				{
					if ( !m_divideandconquerMethod->generateAnimation )
					{
						m_divideandconquerMethod->generateAnimation = true;
						m_divideandconquerMethod->animation.clear();
						m_divideandconquerMethod->DVCalculate3DConvexHull( &m_testPointSet, 0, m_testPointSet.size() - 1, 0 );
					}
					m_gui->SetMaxStep( m_divideandconquerMethod->animation.size() - 1 );
					m_graphicsEngine->SetModelData( NULL, &( m_divideandconquerMethod->animation[ m_gui->GetAnimationStep() ] ) );
				}
				else
				{
					m_divideandconquerMethod->animation.clear();
					m_divideandconquerMethod->generateAnimation = m_gui->GetAnimatedInfo();
					m_graphicsEngine->SetModelData( &( m_divideandconquerMethod->DVCalculate3DConvexHull( &m_testPointSet, 0, m_testPointSet.size() - 1, 0 ) ), NULL );
				}
			}
			break;
		default:
			break;
		}

		m_updateModelFlag = false;
	}

	/*Do the frame processing for the graphics object.*/
	try{
		result = m_graphicsEngine->Frame();
	}catch( bool param )
	{
		cout << "Fail to render the scene!" << endl;
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam )
{
	if( TwEventWin( m_hwnd, umsg, wparam, lparam ) ) {
		return 0;
	}

	switch( umsg )
	{
		/*Check if a key has been pressed on the keyboard.*/
		case WM_KEYDOWN:
		{
			/*If a key is pressed send it to the input object so it can record that state.*/
			m_input->KeyDown( ( unsigned int )wparam );
			return 0;
		}

		/*Check if a key has been released on the keyboard.*/
		case WM_KEYUP:
		{
			/*If a key is released then send it to the input object so it can unset the state for that key.*/
			m_input->KeyUp( ( unsigned int )wparam );
			return 0;
		}

		/*Any other messages send to the default message handler as our application won't make use of them.*/
		default:
		{
			return DefWindowProc( hwnd, umsg, wparam, lparam );
		}
	}
}

void SystemClass::InitializeWindows()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	/*Get an external pointer to this object.*/
	ApplicationHandle = this;

	/*Get the instance of this application.*/
	m_hinstance = GetModuleHandle( NULL );

	/*Give the application a name.*/
	m_applicationName = L"ConvexHull";

	/*Setup the windows class with default settings.*/
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	/*Register the window class.*/
	RegisterClassEx( &wc );

	/*Determine the resolution of the clients desktop screen.*/
	m_screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	/*Setup the screen settings depending on whether it is running in full screen or in windowed mode.*/
	if( m_fullScreen )
	{
		/*If full screen set the screen to maximum size of the users desktop and 32bit.*/
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)m_screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)m_screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		/*Change the display settings to full screen.*/
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		/*Set the position of the window to the top left corner.*/
		posX = posY = 0;
	}
	else
	{
		/*If windowed then set it to 800x600 resolution.*/
		m_screenWidth  = 800;
		m_screenHeight = 600;

		/*Place the window in the middle of the screen.*/
		posX = ( GetSystemMetrics(SM_CXSCREEN) - m_screenWidth )  / 2;
		posY = ( GetSystemMetrics(SM_CYSCREEN) - m_screenHeight ) / 2;
	}

	/*Create the window with the screen settings and get the handle to it.*/
	m_hwnd = CreateWindowEx( WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
							 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							 posX, posY, m_screenWidth, m_screenHeight, NULL, NULL, m_hinstance, NULL );

	/*Bring the window up on the screen and set it as main focus.*/
	ShowWindow( m_hwnd, SW_SHOW );
	SetForegroundWindow( m_hwnd );
	SetFocus( m_hwnd );

	/*Hide the mouse cursor.*/
	ShowCursor( true );
}

void SystemClass::ShutdownWindows()
{
	/*Show the mouse cursor.*/
	ShowCursor( true );

	/*Fix the display settings if leaving full screen mode.*/
	if( m_fullScreen )
	{
		ChangeDisplaySettings( NULL, 0 );
	}

	/*Remove the window.*/
	DestroyWindow( m_hwnd );
	m_hwnd = NULL;

	/*Remove the application instance.*/
	UnregisterClass( m_applicationName, m_hinstance );
	m_hinstance = NULL;

	/*Release the pointer to this class.*/
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam )
{
	switch( umessage )
	{
		/*Check if the window is being destroyed.*/
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}

		/*Check if the window is being closed.*/
		case WM_CLOSE:
		{
			PostQuitMessage( 0 );		
			return 0;
		}

		/*All other messages pass to the message handler in the system class.*/
		default:
		{
			return ApplicationHandle->MessageHandler( hwnd, umessage, wparam, lparam );
		}
	}
}

void SystemClass::SetUpdateModelFlag( bool flag )
{
	m_updateModelFlag = flag;
}

void SystemClass::cleanIncrementalMethod(void)
{
	if ( m_incrementalMethod )
	{
		delete m_incrementalMethod;
		m_incrementalMethod = NULL;
	}
}

void SystemClass::cleanDivideAndConquerMethod(void)
{
	m_divideandconquerMethod->generateAnimation = false;
}
