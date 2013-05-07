#include "GUI.h"
#include "SystemClass.h"
#include "Quaternion.h"

GUI::GUI( SystemClass* sysInstant )
{
	m_sysHandler = sysInstant;
	m_RNGNum = 5;
	m_RNGDropDownIndex = 0;
	m_MethodDropDownIndex = 0;
	m_DisplayDropDownIndex = 0;
	m_Scale = 1.0f;
	m_animated = false;
}

GUI::~GUI()
{

}

void GUI::InitializeGUI( int width, int height, ID3D11Device* device )
{
	/*Initialize the UI with AntTweek*/
	TwInit( TW_DIRECT3D11, device );
	TwWindowSize( width, height );
	m_pTwBar = TwNewBar( "ConvexHull Demo Panel" );

	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
	int barSize[ 2 ] = {300, 275};
	TwSetParam( m_pTwBar, NULL, "size", TW_PARAM_INT32, 2, barSize );

	TwEnumVal* dropdownlist = new TwEnumVal[4];
	dropdownlist[0].Value = 0; dropdownlist[0].Label = "PointsOnSphere";
	dropdownlist[1].Value = 1; dropdownlist[1].Label = "PointsInsideSphere";
	dropdownlist[2].Value = 2; dropdownlist[2].Label = "PointsOnCube";
	dropdownlist[3].Value = 3; dropdownlist[3].Label = "PointsOnUnitCube";
	m_RNGDropDownIndex = 0;

	TwEnumVal* dropdownlist2 = new TwEnumVal[2];
	dropdownlist2[0].Value = 0; dropdownlist2[0].Label = "Incremental Method";
	dropdownlist2[1].Value = 1; dropdownlist2[1].Label = "DivideAndConquer Method";
	m_MethodDropDownIndex = 0;

	TwEnumVal* dropdownlist3 = new TwEnumVal[2];
	dropdownlist3[0].Value = 0; dropdownlist3[0].Label = "Wired Frame Model";
	dropdownlist3[1].Value = 1; dropdownlist3[1].Label = "Fill Solid Model";
	m_DisplayDropDownIndex = 0;

	TwType addRNGDropDownList = TwDefineEnum( "rng_dropdowlist", dropdownlist, 4 );
	TwType addRNGDropDownList2 = TwDefineEnum( "method_dropdowlist", dropdownlist2, 2 );
	TwType addRNGDropDownList3 = TwDefineEnum( "displaymodel_dropdowlist", dropdownlist3, 2 );

	TwAddVarCB( m_pTwBar, "Random Number Generator", addRNGDropDownList, GUI::SetDropDownListItem, GUI::GetDropDownListItem, this, " group='GeneralParameters' keyIncr=Backspace keyDecr=SHIFT+Backspace help='Stop or change the rotation mode.' " );
	TwAddVarCB( m_pTwBar, "Methods for 3DCH", addRNGDropDownList2, GUI::SetComputationMethod, GUI::GetComputationMethod, this, " group='GeneralParameters' keyIncr=o keyDecr=p help='Stop or change the rotation mode.' " );
	TwAddVarCB( m_pTwBar, "Number of Random Value", TW_TYPE_UINT32, GUI::SetRNGNum, GUI::GetRNGNum, this, " group='GeneralParameters' min=5 max=2000 step=1 keyincr=+ keydecr=-" );
	TwAddVarCB( m_pTwBar, "Scale Image", TW_TYPE_FLOAT, GUI::SetScale, GUI::GetScale, this, " group='GraphicsParameters' min=0.05 max=20 step=0.05 keyincr=n keydecr=m" );
	TwAddVarCB( m_pTwBar, "Display Model", addRNGDropDownList3, GUI::SetDisplayModel, GUI::GetDisplayModel, this, " group='GraphicsParameters' keyIncr=c keyDecr=v help='Stop or change the rotation mode.' " );
	TwAddVarRW( m_pTwBar, "Rotation", TW_TYPE_QUAT4F, &Quaternion::g_SpongeRotation, "opened=true axisz=-z group=GraphicsParameters");
	TwAddVarCB( m_pTwBar, "Animation Enabled", TW_TYPE_BOOLCPP, GUI::SetAnimated, GUI::GetAnimated, this, "group=Animation key=u");
}

bool GUI::GetAnimatedInfo()
{
	return m_animated;
}

void TW_CALL GUI::SetDropDownListItem( const void *value, void * clientData )
{
	int index = *(int*)value;
	((GUI*)clientData)->m_RNGDropDownIndex = index;
	((GUI*)clientData)->m_sysHandler->SetRNGModel( index );
}
void TW_CALL GUI::GetDropDownListItem( void *value, void * clientData )
{
	*(int*)value = ((GUI*)clientData)->m_RNGDropDownIndex;
}

void TW_CALL GUI::SetRNGNum( const void *value, void * clientData )
{
	unsigned int number = *(unsigned int*)value;
	((GUI*)clientData)->m_RNGNum = number;
	((GUI*)clientData)->m_sysHandler->SetPointNum( number );
}
void TW_CALL GUI::GetRNGNum( void *value, void * clientData )
{
	*(int*)value = ((GUI*)clientData)->m_RNGNum;
}

void TW_CALL GUI::SetComputationMethod( const void *value, void * clientData )
{
	int model = *(int*)value;
	((GUI*)clientData)->m_MethodDropDownIndex = model;
	((GUI*)clientData)->m_sysHandler->Set3DCHAlgorithm( model );
}
void TW_CALL GUI::GetComputationMethod( void *value, void * clientData )
{
	*(int*)value = ((GUI*)clientData)->m_MethodDropDownIndex;
}

// Scale call back funciton
void TW_CALL GUI::SetScale( const void *value, void * clientData )
{
	float scale = *(float*)value;
	((GUI*)clientData)->m_Scale = scale;
	((GUI*)clientData)->m_sysHandler->SetScale( scale );
}
void TW_CALL GUI::GetScale( void *value, void * clientData )
{
	*(float*)value = ((GUI*)clientData)->m_Scale;
}

void TW_CALL GUI::SetAnimated( const void *value, void * clientData )
{
	((GUI*)clientData)->m_animated = *(bool*)value;
	//((GUI*)clientData)->m_sysHandler->SetScale( scale );
}
void TW_CALL GUI::GetAnimated( void *value, void * clientData )
{
	*static_cast<bool *>(value) = ((GUI*)clientData)->m_animated;
}

void TW_CALL GUI::SetDisplayModel( const void *value, void * clientData )
{
	int index = *(int*)value;
	((GUI*)clientData)->m_DisplayDropDownIndex = index;
	//((GUI*)clientData)->m_sysHandler->SetRNGModel( index );
}
void TW_CALL GUI::GetDisplayModel( void *value, void * clientData )
{
	*(int*)value = ((GUI*)clientData)->m_DisplayDropDownIndex;
}