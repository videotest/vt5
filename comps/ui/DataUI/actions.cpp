#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "data_context_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
//
//	Class CActionChooseObject
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionChooseObject, CCmdTargetEx);

// {CA09CA1A-932D-461c-901F-7367F2CF4BCC}
static const GUID guid_choose_object = 
{ 0xca09ca1a, 0x932d, 0x461c, { 0x90, 0x1f, 0x73, 0x67, 0xf2, 0xcf, 0x4b, 0xcc } };

// {72FFC801-D5C3-410a-BAEC-F40955635786}
GUARD_IMPLEMENT_OLECREATE( CActionChooseObject, "DataUI.ChooseObject", 
0x72ffc801, 0xd5c3, 0x410a, 0xba, 0xec, 0xf4, 0x9, 0x55, 0x63, 0x57, 0x86);

ACTION_INFO_FULL(CActionChooseObject, IDS_ACTION_CHOOSE_OBJECT, -1, -1, guid_choose_object);

ACTION_TARGET(CActionChooseObject, "anydoc");

//////////////////////////////////////////////////////////////////////
ACTION_UI(CActionChooseObject, CObjectChooser)

//////////////////////////////////////////////////////////////////////
//CActionChooseObject implementation
CActionChooseObject::CActionChooseObject()
{
}

CActionChooseObject::~CActionChooseObject()
{
}

bool CActionChooseObject::Invoke()
{
	return true;
}

//extended UI
bool CActionChooseObject::IsAvaible()
{
	IDataContextCtrlPtr	ptr_dcc = GetDataContextCtrl();
	BOOL bavailable = FALSE;
	
	if( ptr_dcc )
		ptr_dcc->IsAvailable( &bavailable );

	return ( bavailable == TRUE );
}

bool CActionChooseObject::IsChecked()
{
	return false;
}



//////////////////////////////////////////////////////////////////////
//
//	Class CObjectChooser
//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CObjectChooser, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CObjectChooser, Combo)

BEGIN_INTERFACE_MAP(CObjectChooser, CCmdTargetEx)
	INTERFACE_PART(CObjectChooser, IID_IUIComboBox, Combo)
END_INTERFACE_MAP()

CObjectChooser::CObjectChooser()
{		
	m_hwnd_combo = 0;
}

//////////////////////////////////////////////////////////////////////
HRESULT CObjectChooser::XCombo::OnInitList( HWND hWnd )
{
	METHOD_PROLOGUE_EX(CObjectChooser, Combo)

	pThis->m_hwnd_combo = hWnd;

	IDataContextCtrlPtr	ptr_dcc = GetDataContextCtrl();
	if( ptr_dcc )
		ptr_dcc->AddComboBox( hWnd );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CObjectChooser::XCombo::OnSelChange()
{
	METHOD_PROLOGUE_EX(CObjectChooser, Combo)

	IDataContextCtrlPtr	ptr_dcc = GetDataContextCtrl();
	if( ptr_dcc )
		ptr_dcc->OnSelChange( pThis->m_hwnd_combo );
		
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CObjectChooser::XCombo::OnDetachWindow()
{
	METHOD_PROLOGUE_EX(CObjectChooser, Combo)

	IDataContextCtrlPtr	ptr_dcc = GetDataContextCtrl();
	if( ptr_dcc )
		ptr_dcc->RemoveComboBox( pThis->m_hwnd_combo );

	return S_OK;
}
