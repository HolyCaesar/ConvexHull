
#pragma once

#define WIN32_LEAN_AND_MEAN

// TODO if we are going to use qt, then we don't need this at all
#include <Windows.h>

#include "Header.h"
#include "Input.h"
#include "Graphics.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass( const SystemClass& );
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler( HWND, UINT, WPARAM, LPARAM );

private:
	bool Frame();
	void InitializeWindows();
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	int m_screenWidth;
	int m_screenHeight;

	bool m_fullScreen;

	Input* m_input;
	Graphics* m_graphicsEngine;
};

static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
/*Only one system instance exists during the lifetime of the program*/
static SystemClass* ApplicationHandle = 0;

