// VTRadioBtnPropPage.cpp : Implementation of the CVTRadioBtnPropPage property page class.

#include "stdafx.h"
//#include "VTRadioBtn.h"
#include "VTRadioBtnPropPage.h"
#include "VTRadioBtnCtrl.h"
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTRadioBtnPropPage, COlePropertyPage)

// Message map
BEGIN_MESSAGE_MAP(CVTRadioBtnPropPage, COlePropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
END_MESSAGE_MAP()



// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(CVTRadioBtnPropPage, "VTRADIOBTN.VTRadioBtnPropPage.1",
	0xcbf250dd, 0xc9fc, 0x41ac, 0x85, 0xf5, 0xd3, 0x95, 0x35, 0x3f, 0x2d, 0xa7)


IDispatch*	CVTRadioBtnPropPage::GetRBInstance( )
{
	ULONG nObjects = 0;
	IDispatch** lpDisp = GetObjectArray( &nObjects );
	if( !nObjects )
		return 0;
    return lpDisp[0];	
}


BOOL CVTRadioBtnPropPage::OnInitDialog( )
{
	BOOL bRes = __super::OnInitDialog( );
	m_buttonUp.EnableWindow( FALSE );
	m_buttonDown.EnableWindow( FALSE );
	m_buttonDel.EnableWindow( FALSE );
	m_buttonUpdate.EnableWindow( FALSE );
	m_edit.SetFocus( );

	if( LoadFromControl( ) )
		m_bIsCreated = true;

	return bRes;
}


// CVTRadioBtnPropPage::CVTRadioBtnPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTRadioBtnPropPage

BOOL CVTRadioBtnPropPage::CVTRadioBtnPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTRADIOBTN_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CVTRadioBtnPropPage::CVTRadioBtnPropPage - Constructor

CVTRadioBtnPropPage::CVTRadioBtnPropPage() :
	COlePropertyPage(IDD, IDS_VTRADIOBTN_PPG_CAPTION)
{
	m_bIsCreated = false;
}



// CVTRadioBtnPropPage::DoDataExchange - Moves data between page and properties

void CVTRadioBtnPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON3, m_buttonUp);
	DDX_Control(pDX, IDC_BUTTON4, m_buttonDown);
	DDX_Control(pDX, IDC_BUTTON2, m_buttonDel);
	DDX_Control(pDX, IDC_BUTTON5, m_buttonUpdate);
	DDX_Control(pDX, IDC_BUTTON1, m_buttonAdd);
	
	if( m_bIsCreated && pDX->m_bSaveAndValidate )
	{
		IDispatch* pDisp = GetRBInstance( );
		if( pDisp )
		{
			// delete all
			HRESULT hRes = _com_dispatch_method( pDisp, CVTRadioBtnCtrl::dispidDeleteAll, 
				DISPATCH_METHOD, VT_EMPTY, NULL, NULL );
			if( hRes == S_OK )
			{
				// add items
				int iItemCount = m_list.GetCount( );
				for( int i = 0; i < iItemCount; i ++ )
				{
					CString str;
					long lRes = 0;
					m_list.GetText( i, str );
					BSTR bstr = str.AllocSysString( );
					hRes = _com_dispatch_method( pDisp, CVTRadioBtnCtrl::dispidAddItem,
						DISPATCH_METHOD, VT_I4, (void*)&lRes, L"\x0008", bstr );
					if( hRes != S_OK )
						break;
					SysFreeString( bstr );
				}
				if( hRes == S_OK )
					// ReCreate
					_com_dispatch_method( pDisp, CVTRadioBtnCtrl::dispidReCreate, DISPATCH_METHOD,
					VT_EMPTY, NULL, NULL );
	
			}
		}
	}
	
	DDP_PostProcessing(pDX);
}

bool	CVTRadioBtnPropPage::LoadFromControl( )
{
	m_list.ResetContent( );

	IDispatch *pDisp = GetRBInstance( );
	if( !pDisp )
		return false;

	long lItemsCount = 0;
	HRESULT hRes;
	hRes = _com_dispatch_method( pDisp, CVTRadioBtnCtrl::dispidGetItemsCount,
						DISPATCH_METHOD, VT_I4, (void*)&lItemsCount, NULL );
	if( hRes != S_OK )
		return false;

	BSTR bstr = 0;
	for( int i = 0; i < lItemsCount; i ++ )
	{
		// get string
		hRes = _com_dispatch_method( pDisp, CVTRadioBtnCtrl::dispidGetItem,
						DISPATCH_METHOD, VT_BSTR, (void*)&bstr, L"\x0003", (long) i );		
		if( hRes != S_OK )
			break;
		
		// add string to list
		m_list.AddString( (char *)_bstr_t( bstr ) );

		if( bstr )
		{
			::SysFreeString( bstr );
			bstr = 0;
		}
    }

	return true;
}

void CVTRadioBtnPropPage::MoveListString( int move )
{
	int iCurrIndex = m_list.GetCurSel( );
	if( ( iCurrIndex == LB_ERR ) || ( m_list.GetCount( ) <= 1 ) ) 
		return;
	if( ( ( move == MOVE_UP ) && !iCurrIndex ) || ( ( move != MOVE_UP ) && ( iCurrIndex == m_list.GetCount( ) - 1 ) ) )
		return;
	CString text;
	m_list.GetText( iCurrIndex, text );
	m_list.DeleteString( iCurrIndex );
	move == MOVE_UP ? iCurrIndex -- : iCurrIndex ++;
	m_list.SetCurSel( m_list.InsertString( iCurrIndex, text ) );
}

void CVTRadioBtnPropPage::UpdateEdit( )
{
	CString text("");
	int iSelItem = m_list.GetCurSel( );
	if( iSelItem !=  LB_ERR )
		m_list.GetText( iSelItem, text );
	m_edit.SetWindowText( text );
}

void	CVTRadioBtnPropPage::UpdateBtns( )
{
	int iSel = m_list.GetCurSel( );
	if( ( iSel == LB_ERR ) || ( m_list.GetCount( ) == 0 ) )
	{
		m_buttonDel.EnableWindow( FALSE );
		m_buttonUpdate.EnableWindow( FALSE );
	}
	else
	{
		m_buttonDel.EnableWindow( );
		m_buttonUpdate.EnableWindow( );
		UpdateEdit( );
	}
	UpdateUpDownBtns( iSel );
}

void	CVTRadioBtnPropPage::UpdateUpDownBtns( int iSelIndex )
{
	m_buttonUp.EnableWindow( ( iSelIndex != 0 ) && ( iSelIndex != LB_ERR ) );
	m_buttonDown.EnableWindow( ( iSelIndex != ( m_list.GetCount( ) - 1 ) ) && ( iSelIndex != LB_ERR ) ); 
}

// CVTRadioBtnPropPage message handlers

void CVTRadioBtnPropPage::OnLbnSelchangeList2()
{
	UpdateBtns( );
}

void CVTRadioBtnPropPage::OnBnClickedButton1()
{	// add 
	CString text;
	m_edit.GetWindowText( text );
	text = text.Trim( );
	if( text.IsEmpty( ) )
		return;
	int iFindIndex = m_list.FindString( 0, text );
	if( iFindIndex == LB_ERR )
        m_list.AddString( text );
	else
		m_list.SetCurSel( iFindIndex );
	UpdateEdit( );
	UpdateBtns( );
	m_edit.SetFocus( );
}

void CVTRadioBtnPropPage::OnBnClickedButton3()
{	// up
	MoveListString( MOVE_UP );
	UpdateBtns( );
	m_list.SetFocus( );
}

void CVTRadioBtnPropPage::OnBnClickedButton4()
{	// down
	MoveListString( MOVE_DOWN );
	UpdateBtns( );
	m_list.SetFocus( );
}

void CVTRadioBtnPropPage::OnBnClickedButton2()
{	// delete
	int iCurrIndexSel = m_list.GetCurSel( );
	if( iCurrIndexSel == LB_ERR  )
		return;
	if( m_list.DeleteString( iCurrIndexSel ) )
	{
		int iCount = m_list.GetCount( );
		iCurrIndexSel == iCount ? m_list.SetCurSel( iCount - 1 ) : m_list.SetCurSel( iCurrIndexSel );
	}
	UpdateEdit( );
	UpdateBtns( );
}

void CVTRadioBtnPropPage::OnBnClickedButton5()
{	// update
	CString text("");
	m_edit.GetWindowText( text );
	text = text.Trim( );
	int iItemSel = m_list.GetCurSel( );
	CString stSourceText("");
	m_list.GetText( iItemSel, stSourceText);
	if( text.IsEmpty( ) || text == stSourceText )
		return;
	m_list.DeleteString( iItemSel );
	m_list.InsertString( iItemSel, text );
	m_list.SetCurSel( iItemSel );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// CVTRadioBtnPropPageTwo
IMPLEMENT_DYNCREATE(CVTRadioBtnPropPageTwo, COlePropertyPage)

// Message map
BEGIN_MESSAGE_MAP(CVTRadioBtnPropPageTwo, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

// {AA024D62-E866-4828-BB05-558735A1497B}
IMPLEMENT_OLECREATE_EX(CVTRadioBtnPropPageTwo, "VTRADIOBTN.VTRadioBtnPropPageTwo.1",
	0xaa024d62, 0xe866, 0x4828, 0xbb, 0x5, 0x55, 0x87, 0x35, 0xa1, 0x49, 0x7b)


// CVTRadioBtnPropPage::CVTRadioBtnPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTRadioBtnPropPage

BOOL CVTRadioBtnPropPageTwo::CVTRadioBtnPropPageTwoFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTRADIOBTN_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CVTRadioBtnPropPageTwo::CVTRadioBtnPropPageTwo - Constructor

CVTRadioBtnPropPageTwo::CVTRadioBtnPropPageTwo() :
	COlePropertyPage(IDD, IDS_VTRADIOBTN_PPG_TWO_CAPTION)
{
	m_bEnable = FALSE;
	m_bVisible = FALSE;
	m_bEvenlySpaced = FALSE;
}



// CVTRadioBtnPropPage::DoDataExchange - Moves data between page and properties

void CVTRadioBtnPropPageTwo::DoDataExchange(CDataExchange* pDX)
{

	DDP_Check(pDX, IDC_VISIBLE, m_bVisible, _T("Visible") );
	DDX_Check(pDX, IDC_VISIBLE, m_bVisible);
	
	DDP_Check(pDX, IDC_ENABLE, m_bEnable, _T("Enabled") );
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);

	DDP_Check(pDX, IDC_EVENLYSPACED, m_bEvenlySpaced, _T("EvenlySpaced") );
	DDX_Check(pDX, IDC_EVENLYSPACED, m_bEvenlySpaced);

	DDP_PostProcessing(pDX);
}
