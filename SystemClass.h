
#pragma once

#define WIN32_LEAN_AND_MEAN

// TODO if we are going to use qt, then we don't need this at all
#include <Windows.h>

#include "Header.h"
#include "Input.h"
#include "Graphics.h"
#include "PointGenerator.h"
#include "IncrementalHull3DFast.h"
#include "DivideAndConquerFor3DCH.h"
#include "GUI.h"

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

public:
	void Set3DCHAlgorithm( int algorithm );
	void SetRNGModel( unsigned int model );
	void SetPointNum( unsigned int num );
	void SetScale( float factor );

private:
	bool Frame();
	void ComputeCH();
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
	
	GUI* m_gui;
private:
	vector<VERTEX> m_testPointSet;
	IncrementalHull3DFast* m_incrementalMethod;
	DivideAndConquerFor3DCH *m_divideandconquerMethod;

	/*
	* 0--DivideAndConquer;
	* 1--Incremental;
	*/
	int m_chAlgorithm;
	bool m_updateModelFlag;
	/*
	* 1. pointGeneratorMode = 0 means Insphere mode
	* 2. pointGeneratorMode = 1 means Onsphere mode
	* 3. pointGeneratorMode = 2 means Incude mode
	* 4. pointGeneratorMode = 3 means Unitcube mode
	*/
	unsigned int pointGeneratorMode;
	unsigned int pointNumber;

private:

};

static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
/*Only one system instance exists during the lifetime of the program*/
static SystemClass* ApplicationHandle = 0;

