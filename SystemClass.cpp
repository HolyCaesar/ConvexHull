#include "SystemClass.h"
using namespace std;

SystemClass::SystemClass()
{
	m_input = 0;
	m_graphicsEngine = 0;
	m_screenWidth = 0;
	m_screenHeight = 0;
	m_fullScreen = false;
}

SystemClass::SystemClass( const SystemClass& other )
{

}

SystemClass::~SystemClass()
{

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
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		/*If windows signals to end the application then exit out.*/
		if( msg.message == WM_QUIT )
		{
			done = true;
		}
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

bool SystemClass::Frame()
{
	bool result;

	/*Check if the user pressed escape and wants to exit the application.*/
	if( m_input->IsKeyDown( VK_ESCAPE ) )
	{
		return false;
	}

	if( m_input->IsKeyDown( 'W' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( 0.1, 0.0, 0.0 );
	}

	if( m_input->IsKeyDown( 'S' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( -0.1, 0.0, 0.0 );
	}

	if( m_input->IsKeyDown( 'A' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( 0.0, 0.1, 0.0 );
	}

	if( m_input->IsKeyDown( 'D' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( 0.0, -0.1, 0.0 );
	}

	if( m_input->IsKeyDown( 'Q' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( 0.0, 0.0, 0.1 );
	}

	if( m_input->IsKeyDown( 'E' ) )
	{
		m_graphicsEngine->SetYawPitchRoll( 0.0, 0.0, -0.1 );
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
