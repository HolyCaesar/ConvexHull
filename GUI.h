#pragma once

#include "Header.h"
#include "D3D.h"

class SystemClass;

class GUI
{
public:
	GUI( SystemClass* sysInstant );
	~GUI();

public:
	void InitializeGUI( int width, int height, ID3D11Device* device );

private:
	/* 
	* GUI stuff
	*/
	TwBar* m_pTwBar;
	int m_RNGDropDownIndex;
	int m_MethodDropDownIndex;
	int m_RNGNum;
	float m_Scale;
	bool m_animated;

	SystemClass* m_sysHandler;

	/*
	* AntTweek Call Back functions
	*/
private:
	static void TW_CALL SetDropDownListItem( const void *value, void * clientData );
	static void TW_CALL GetDropDownListItem( void *value, void * clientData );
	static void TW_CALL SetRNGNum( const void *value, void * clientData );
	static void TW_CALL GetRNGNum( void *value, void * clientData );
	static void TW_CALL SetComputationMethod( const void *value, void * clientData );
	static void TW_CALL GetComputationMethod( void *value, void * clientData );
	static void TW_CALL SetScale( const void *value, void * clientData );
	static void TW_CALL GetScale( void *value, void * clientData );
	static void TW_CALL SetAnimated( const void *value, void * clientData );
	static void TW_CALL GetAnimated( void *value, void * clientData );
};