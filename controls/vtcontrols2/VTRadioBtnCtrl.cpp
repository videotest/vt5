// VTRadioBtnCtrl.cpp : Implementation of the CVTRadioBtnCtrl ActiveX Control class.

#include "stdafx.h"
//#include "VTRadioBtn.h"
#include <comdef.h>
#include "vtcontrols2.h"
#include "VTRadioBtnCtrl.h"
#include "VTRadioBtnPropPage.h"
#include <comutil.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTRadioBtnCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CVTRadioBtnCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
ON_WM_SIZE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CVTRadioBtnCtrl, COleControl)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "AddItem", dispidAddItem, AddItem, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "DeleteItem", dispidDeleteItem, DeleteItem, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "GetItemsCount", dispidGetItemsCount, GetItemsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "DeleteAll", dispidDeleteAll, DeleteAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "ReCreate", dispidReCreate, ReCreate, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "GetItem", dispidGetItem, GetItem, VT_BSTR, VTS_I4)
	DISP_PROPERTY_EX_ID(CVTRadioBtnCtrl, "ItemChecked", dispidItemChecked, GetItemChecked, SetItemChecked, VT_I4)
	DISP_PROPERTY_EX_ID(CVTRadioBtnCtrl, "Visible", dispidVisible, GetVisible, SetVisible, VT_BOOL)
	DISP_STOCKPROP_ENABLED()
	DISP_PROPERTY_EX_ID(CVTRadioBtnCtrl, "EvenlySpaced", dispidEvenlySpaced, GetEvenlySpaced, SetEvenlySpaced, VT_BOOL)
	DISP_PROPERTY_EX_ID(CVTRadioBtnCtrl, "EnableMask", dispidEnableMask, GetEnableMask, SetEnableMask, VT_I4)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "DisableRButton", dispidDisableRButton, DisableRButton, VT_BOOL, VTS_I4)
	DISP_FUNCTION_ID(CVTRadioBtnCtrl, "EnableRButton", dispidEnableRButton, EnableRButton, VT_BOOL, VTS_I4)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CVTRadioBtnCtrl, COleControl)
	EVENT_CUSTOM_ID("OnChangeCheck", eventidOnChangeCheck, OnChangeCheck, VTS_I4)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTRadioBtnCtrl, 2)
	PROPPAGEID(CVTRadioBtnPropPageTwo::guid)
	PROPPAGEID(CVTRadioBtnPropPage::guid)
END_PROPPAGEIDS(CVTRadioBtnCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTRadioBtnCtrl, "VTRADIOBTN.VTRadioBtnCtrl.1",
	0x565483d1, 0x812a, 0x43bb, 0xa2, 0x28, 0x86, 0x65, 0x1f, 0x54, 0x97, 0xbc)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTRadioBtnCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DVTRadioBtn =
		{ 0x5A81931C, 0xE10B, 0x4143, { 0xA9, 0x60, 0xD2, 0xB0, 0x66, 0xF4, 0xF5, 0xBE } };
const IID BASED_CODE IID_DVTRadioBtnEvents =
		{ 0xDE1D575C, 0x14DD, 0x4734, { 0xBA, 0xDE, 0xB8, 0x8B, 0x12, 0xA1, 0x5A, 0xC8 } };



// Control type information

static const DWORD BASED_CODE _dwVTRadioBtnOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTRadioBtnCtrl, IDS_VTRADIOBTN, _dwVTRadioBtnOleMisc)


// CVTRadioBtnCtrl::CVTRadioBtnCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTRadioBtnCtrl

BOOL CVTRadioBtnCtrl::CVTRadioBtnCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VTRADIOBTN,
			IDB_VTRADIOBTN,
			afxRegApartmentThreading,
			_dwVTRadioBtnOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CVTRadioBtnCtrl::CVTRadioBtnCtrl - Constructor

CVTRadioBtnCtrl::CVTRadioBtnCtrl()
{
	InitializeIIDs(&IID_DVTRadioBtn, &IID_DVTRadioBtnEvents);
	// TODO: Initialize your control's instance data here.
	m_lItemChecked = -1;
	m_haRBtns = 0;
	m_lRBCount = 0;
	m_bVisible = TRUE;
	m_lLayout = bltVertical;
	m_bEvenlySpaced = FALSE;
	m_lEnableMask = (DWORD)-1;
}



// CVTRadioBtnCtrl::~CVTRadioBtnCtrl - Destructor

CVTRadioBtnCtrl::~CVTRadioBtnCtrl()
{
	// TODO: Cleanup your control's instance data here.
	DeleteAll( );

	if( m_haRBtns )
	{
		delete[] m_haRBtns;
		m_haRBtns = 0;
		m_lRBCount = 0;
	}
}

int CVTRadioBtnCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	ReCreate( );
	return 0;
}


bool	CVTRadioBtnCtrl::CreateButtons( )
{
	if( !GetSafeHwnd() )
		return false;

	if( m_haRBtns )
		return false;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_VCENTER;
	RECT rtWRect = {0};
	HFONT hfont = 0;
	GetWindowRect( &rtWRect );
	CWnd *pParentWnd = GetParent( );
	if( pParentWnd )
		hfont = (HFONT)::SendMessage( pParentWnd->GetSafeHwnd( ), WM_GETFONT, 0, 0 );
	if( !hfont )
		hfont = (HFONT)::GetStockObject( ANSI_VAR_FONT );
	
	int iHeight =  15,
		iWidth = rtWRect.right - rtWRect.left,
		iY = 0;

	CClientDC dc(0);
	HFONT	hf_old = 0;
	hf_old = (HFONT)::SelectObject( dc, hfont );
	TEXTMETRIC tm = {0};
	if( ::GetTextMetrics( dc, &tm ) )
	    iHeight = max( iHeight, (tm.tmHeight + 2 * ::GetSystemMetrics( SM_CXEDGE ) ) );        
	

	long lItemsCount = m_saItems.GetCount( );

	if( lItemsCount < 0 )
		return false;

	m_lRBCount = lItemsCount;
	m_haRBtns = new HWND[ m_lRBCount ];

	for( long lItemIndex = 0; lItemIndex < lItemsCount; lItemIndex ++/*, iY += iHeight*/ )
	{
		RECT rctext = {0};
		m_haRBtns[ lItemIndex ] = CreateWindowEx(
									/*WS_EX_TRANSPARENT*/0, // extended style
									_T("BUTTON"),   // predefined class 
									//m_saItems.GetAt( lItemIndex ),       // button text 
									"",
									lItemIndex == 0 ? dwStyle | WS_GROUP : dwStyle,  // styles 
									// Size and position values are given explicitly, because 
									// the CW_USEDEFAULT constant gives zero values for buttons. 
									0,         // starting x position 
									iY,         // starting y position 
									iWidth,        // button width 
									iHeight,        // button height 
									GetSafeHwnd(),       // parent window 
									NULL,       // No menu 
									(HINSTANCE) GetWindowLong( GetSafeHwnd(), GWLP_HINSTANCE), 
									NULL);      // pointer not needed 
		if( hfont && m_haRBtns[ lItemIndex ] )
			::SendMessage( m_haRBtns[ lItemIndex ], WM_SETFONT, WPARAM(hfont), 0 );

		::SetWindowText(m_haRBtns[ lItemIndex ],m_saItems.GetAt( lItemIndex ));
	}

	
	CalcLayout( );

	SetButtonsOptions( boCheck | boEnable);
	SetVisible( m_bVisible );

	::SelectObject( dc, hf_old );
    
	return true;
}

bool	CVTRadioBtnCtrl::DeleteAllButtons( )
{
	if( m_haRBtns )
	{
		for( long index = 0; index < m_lRBCount; index ++ )
		{
			if( m_haRBtns[ index ] )
			{
				if ( !::DestroyWindow( m_haRBtns[ index ] ) )
					return false;
			}
			else
				return false;
		}
		delete[] m_haRBtns;
		m_haRBtns = 0;
		m_lRBCount = 0;
	}
	return true;
}

HWND	CVTRadioBtnCtrl::GetItemHwnd( long iItemIndex )
{
	if( ( iItemIndex < 0 ) || ( iItemIndex >= m_saItems.GetCount( ) ) )
		return 0;
	return m_haRBtns[ iItemIndex ];
}

long	CVTRadioBtnCtrl::GetItemIndex( HWND hwnd )
{
	if( m_haRBtns && m_lRBCount && hwnd )
	{
		for( long lIndex = 0; lIndex < m_lRBCount; lIndex ++ )
			if( m_haRBtns[ lIndex ] == hwnd )
				return lIndex;
	}
	return -1;
}

void	CVTRadioBtnCtrl::SetButtonsOptions( DWORD nFlags )
{
	if( !m_haRBtns || !m_lRBCount)
		return;

	for( long i = 0; i < m_lRBCount; i ++ )
	{
		HWND hwndButton = GetItemHwnd( i );
		if( !hwndButton )
			break;

		if( nFlags & boCheck )
			::SendMessage( hwndButton, BM_SETCHECK, ( i == (m_lItemChecked < m_lRBCount ? m_lItemChecked : -1) ? BST_CHECKED : BST_UNCHECKED ), 0 );

		if( nFlags & boVisible )
            ::ShowWindow( hwndButton, m_bVisible ? SW_SHOW : SW_HIDE );

		if( nFlags & boEnable )
		{
			BOOL bEnabledControl = ((1<<i)&m_lEnableMask)?TRUE:FALSE;
			::EnableWindow( hwndButton, GetEnabled( )&&bEnabledControl );
		}
	}
}

void	CVTRadioBtnCtrl::CalcLayout( )
{
	if( !GetSafeHwnd() || !m_haRBtns )
		return;

	RECT rc_client = {0};
	GetClientRect( &rc_client );

	// calc space between buttons
	int nspace = 0;
	if( m_bEvenlySpaced && m_lRBCount > 1 )
	{
		int nsum_length = 0;
		for( int nbtn = 0; nbtn < m_lRBCount; nbtn ++ )   		
		{
			RECT rc_wnd = {0};
			::GetWindowRect( m_haRBtns[ nbtn ], &rc_wnd );
			nsum_length += m_lLayout == bltVertical ? rc_wnd.bottom - rc_wnd.top : rc_wnd.right - rc_wnd.left;
		}
		
		nspace = (int)( ((m_lLayout == bltVertical ? rc_client.bottom - rc_client.top : 
					rc_client.right - rc_client.left) - nsum_length) / (m_lRBCount - 1) );
	}
    
	// move windows
	int	ncoord = 0;
	for( int nbtn = 0; nbtn < m_lRBCount; nbtn ++ )   		
	{
		RECT rc_wnd = {0};
		::GetWindowRect( m_haRBtns[ nbtn ], &rc_wnd );
		ScreenToClient( &rc_wnd );
		if( m_lLayout == bltVertical )
		{
			int nheight = 0;
			// смещаем
			nheight = rc_wnd.bottom - rc_wnd.top;
			rc_wnd.top = ncoord;
			rc_wnd.bottom = rc_wnd.top + nheight;
			// вписываем по ширине
			rc_wnd.right = rc_wnd.left + rc_client.right - rc_client.left;
		}
		else
        {
            int nwidth = 0;
			// пока только смещаем
			nwidth = rc_wnd.right - rc_wnd.left;
			rc_wnd.left = ncoord;
			rc_wnd.right = rc_wnd.left + nwidth;
		}

		::MoveWindow( m_haRBtns[ nbtn ], rc_wnd.left, rc_wnd.top, rc_wnd.right - rc_wnd.left, 
			rc_wnd.bottom - rc_wnd.top, TRUE );            		

		ncoord = (m_lLayout == bltVertical ? rc_wnd.bottom : rc_wnd.right) + nspace;
	}    
}

void	CVTRadioBtnCtrl::SetModifiedFlagAndRefresh( )
{
	SetModifiedFlag( );
	Refresh( );
}

// CVTRadioBtnCtrl::OnDraw - Drawing function

void CVTRadioBtnCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	if( !GetSafeHwnd( ) )
	{	// если окно есть, но не созданно
		pdc->Rectangle( rcBounds );
		pdc->MoveTo( rcBounds.left, rcBounds.top );
		pdc->LineTo( rcBounds.right, rcBounds.bottom );
		pdc->MoveTo( rcBounds.left, rcBounds.bottom );
		pdc->LineTo( rcBounds.right, rcBounds.top );
	}
	else
	   pdc->FillRect( rcBounds, &(CBrush( ::GetSysColor(COLOR_BTNFACE) )) );	
}



// CVTRadioBtnCtrl::DoPropExchange - Persistence support

void CVTRadioBtnCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CVTRadioBtnCtrl::OnResetState - Reset control to default state

void CVTRadioBtnCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

void	CVTRadioBtnCtrl::Serialize( CArchive &ar )
{
	if( ar.IsStoring( ) )
	{	// storing
		int iVer = 3; // 2 - add property "Visible"
					  // 3 - add property "EvenlySpaced" and save layout
		ar << iVer;
		long	lItemsCount = GetItemsCount( );
		ar << lItemsCount;
		for( long l = 0; l < lItemsCount; l++ )
		{
			ar << m_saItems.GetAt( l );
		}
		ar << m_lItemChecked;
		ar << m_bVisible;
		ar << m_lLayout;
		ar << m_bEvenlySpaced;
	}
	else
	{	// loading
		int iVer = 0;
		ar >> iVer;
		long	lItemsCount = 0;
		ar >> lItemsCount;

		m_saItems.RemoveAll( );
		for( long l = 0; l < lItemsCount; l++ )
		{
			CString str;
			ar >> str;
			m_saItems.Add( str );
		}
		ar >> m_lItemChecked;

		m_bVisible = TRUE;
		if( iVer > 1 )
			ar >> m_bVisible;

		if( iVer > 2 )
		{
			ar >> m_lLayout;
			ar >> m_bEvenlySpaced;
		}
	}

    COleControl::Serialize(ar);

	if( !ar.IsStoring( ) )
		ReCreate( );
}



// CVTRadioBtnCtrl message handlers

LONG CVTRadioBtnCtrl::AddItem(LPCTSTR NewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lRes =  m_saItems.Add( NewValue );
	SetModifiedFlagAndRefresh( );
	return lRes;
}

LONG CVTRadioBtnCtrl::DeleteItem(LONG lIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ( lIndex < 0 ) || ( lIndex >= m_saItems.GetCount( )  ) )
		return -1;
	m_saItems.RemoveAt( lIndex );

	long lRes = m_saItems.GetCount( );
	SetModifiedFlagAndRefresh( );
	return lRes;	// return number of items
}

LONG CVTRadioBtnCtrl::GetItemsCount(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return (long) m_saItems.GetCount( );
}

void CVTRadioBtnCtrl::DeleteAll(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_saItems.RemoveAll( );
	// m_lItemChecked = -1; - т.к. некорректно сбрасывать установленное извне свойство
	SetModifiedFlagAndRefresh( );
}

void CVTRadioBtnCtrl::ReCreate(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( DeleteAllButtons( ) )
		CreateButtons( );
}

BSTR CVTRadioBtnCtrl::GetItem(LONG lIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult("");
	if( ( lIndex >= 0 ) && ( lIndex < m_saItems.GetCount( ) ) )
		strResult = m_saItems.GetAt( lIndex );

	return strResult.AllocSysString();
}

LONG CVTRadioBtnCtrl::GetItemChecked(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return (long) m_lItemChecked;
}

void CVTRadioBtnCtrl::SetItemChecked(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lItemChecked = newVal < 0 ? -1 : newVal ;
    SetButtonsOptions( boCheck );
	SetModifiedFlag();
}

BOOL CVTRadioBtnCtrl::GetVisible(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_bVisible;
}

void CVTRadioBtnCtrl::SetVisible(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bVisible = newVal;
	SetModifiedFlag();
	ShowWindow( m_bVisible ? SW_SHOWNA : SW_HIDE);
}

BOOL CVTRadioBtnCtrl::GetEvenlySpaced(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_bEvenlySpaced;
}

void CVTRadioBtnCtrl::SetEvenlySpaced(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_bEvenlySpaced = newVal;
	// [vanek] SBT:1056 - 05.07.2004
	CalcLayout( );
	SetModifiedFlagAndRefresh( );
}


LONG CVTRadioBtnCtrl::GetEnableMask(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return (LONG)m_lEnableMask;
}

void CVTRadioBtnCtrl::SetEnableMask(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lEnableMask = newVal;
	SetModifiedFlag();
	SetButtonsOptions(boEnable);
}



void CVTRadioBtnCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);
    CalcLayout( );
}

BOOL CVTRadioBtnCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class 
	cs.style |= ( WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE );
	return COleControl::PreCreateWindow(cs);
}

BOOL CVTRadioBtnCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( wParam == BN_CLICKED )
	{
		long lIndex = GetItemIndex( (HWND) lParam );
		if( lIndex != -1 )
		{
			if ( m_lItemChecked != lIndex )
			{
				m_lItemChecked = lIndex;
				OnChangeCheck( m_lItemChecked );
				if (m_lEnableMask != (DWORD)-1)
					SetButtonsOptions( boCheck );
			}

		}
	}
	
	return COleControl::OnCommand(wParam, lParam);
}

void CVTRadioBtnCtrl::OnEnabledChanged()
{
	SetButtonsOptions( boEnable );
	COleControl::OnEnabledChanged();
}

BOOL CVTRadioBtnCtrl::DisableRButton(LONG lIndex)
{
	if( !m_haRBtns || !m_lRBCount)
		return FALSE;
	if(m_lRBCount<lIndex+1)
		return FALSE;
	
	HWND hwndButton = GetItemHwnd( lIndex );
	if( !hwndButton )
		return FALSE;

	::EnableWindow(hwndButton, false);

	return TRUE;
}

BOOL CVTRadioBtnCtrl::EnableRButton(LONG lIndex)
{
	if( !m_haRBtns || !m_lRBCount)
		return FALSE;
	if(m_lRBCount<lIndex+1)
		return FALSE;
	
	HWND hwndButton = GetItemHwnd( lIndex );
	if( !hwndButton )
		return FALSE;

	::EnableWindow(hwndButton, true);

	return TRUE;
}