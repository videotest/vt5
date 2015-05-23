// DBTextCtl.cpp : Implementation of the CDBTextCtrl ActiveX Control class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBTextCtl.h"
#include "DBTextPpg.h"
#include "DBCommonPropPage.h"




IMPLEMENT_DYNCREATE(CDBTextCtrl, COleControl)

BEGIN_INTERFACE_MAP(CDBTextCtrl, COleControl)
	INTERFACE_PART(CDBTextCtrl, IID_IDBControl, DBControl)
	INTERFACE_PART(CDBTextCtrl, IID_IDBControl2, DBControl)	
	INTERFACE_PART(CDBTextCtrl, IID_IDBaseListener, DBaseListener)	
	INTERFACE_PART(CDBTextCtrl, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CDBTextCtrl, IID_IAXCtrlDataSite, AXCtrlDataSite)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDBTextCtrl, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CDBTextCtrl)

CORRECT_MFC_OnSetObjectRects(CDBTextCtrl);

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBTextCtrl, COleControl)
	//{{AFX_MSG_MAP(CDBTextCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CDBTextCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CDBTextCtrl)
	DISP_PROPERTY_EX(CDBTextCtrl, "AutoLabel", GetAutoLabel, SetAutoLabel, VT_BOOL)
	DISP_PROPERTY_EX(CDBTextCtrl, "EnableEnumeration", GetEnableEnumeration, SetEnableEnumeration, VT_BOOL)
	DISP_PROPERTY_EX(CDBTextCtrl, "TableName", GetTableName, SetTableName, VT_BSTR)
	DISP_PROPERTY_EX(CDBTextCtrl, "FieldName", GetFieldName, SetFieldName, VT_BSTR)
	DISP_PROPERTY_EX(CDBTextCtrl, "LabelPosition", GetLabelPosition, SetLabelPosition, VT_I2)
	DISP_PROPERTY_EX(CDBTextCtrl, "TextFont", GetTextFont, SetTextFont, VT_FONT)
	DISP_PROPERTY_EX(CDBTextCtrl, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_FUNCTION(CDBTextCtrl, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDBTextCtrl, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CDBTextCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CDBTextCtrl, COleControl)
	//{{AFX_EVENT_MAP(CDBTextCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CDBTextCtrl, 2)
//	PROPPAGEID(CDBTextPropPage::guid)
	PROPPAGEID(CDBCommonPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CDBTextCtrl)
*/
static CLSID pages[2];

LPCLSID CDBTextCtrl::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CDBCommonPropPage::guid;
	pages[cPropPages++]=CLSID_StockFontPage;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBTextCtrl, DBTextProgID,
	0x4a17bf90, 0xe003, 0x4c5f, 0xaa, 0x99, 0x25, 0x7b, 0x34, 0xd8, 0xdf, 0xf)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CDBTextCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

extern const IID BASED_CODE IID_DDBText =
		{ 0xef00f5fb, 0x9f2b, 0x4921, { 0x8b, 0x3c, 0x67, 0xa9, 0xd, 0xd6, 0x76, 0x94 } };
const IID BASED_CODE IID_DDBTextEvents =
		{ 0xc16587c7, 0x6aa5, 0x407f, { 0xa1, 0xd4, 0x29, 0xe6, 0x79, 0x43, 0xbd, 0x42 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwDBTextOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDBTextCtrl, IDS_DBTEXT, _dwDBTextOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::CDBTextCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBTextCtrl

BOOL CDBTextCtrl::CDBTextCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_DBTEXT,
			IDB_DBTEXT,
			afxRegApartmentThreading,
			_dwDBTextOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::CDBTextCtrl - Constructor

CDBTextCtrl::CDBTextCtrl() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DDBText, &IID_DDBTextEvents);
	CDBControlImpl::InitDefaults();	

	init_default_font( &m_fontDesc );

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	//added by akm 13_12_k5
	m_PropertyAXStatus=false;

}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::~CDBTextCtrl - Destructor

CDBTextCtrl::~CDBTextCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

/////////////////////////////////////////////////////////////////////////////
int CDBTextCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{							
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;	
	
	CRect rectClient;
	GetClientRect( &rectClient );
	
	RegisterDB();

	
	CRect rc = CRect(0,0,0,200);		
	m_ctrlEdit.Create( WS_CHILD | WS_BORDER | WS_TABSTOP | 
							ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE ,
					rc,  this, IDC_EDIT_CTRL);	
	m_ctrlPicker.Create( NULL, NULL, WS_CHILD  | WS_TABSTOP, rc, this, 102 );	

	m_ctrlEdit.SetFont( &m_Font );
	m_ctrlPicker.SetFont( &m_Font );

	//Resize();

	AnalizeInputCtrlAppearance();
	//GetValue( NULL );


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::Resize()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !::IsWindow( m_ctrlPicker.GetSafeHwnd() ) )
		return;


	if( !::IsWindow( m_ctrlEdit.GetSafeHwnd() ) )
		return;

	
	CRect rcClient;
	GetClientRect( &rcClient );
	rcClient.DeflateRect( 3, 3, 3, 3 );


	CSize sizeLeftOffset = CSize(0,0);
	CSize sizeRightOffset = CSize(0,0);

	if( m_bAutoLabel )
	{
		CClientDC dc( this );
		CString srtCaption = GetCaption();
		CRect rcText;
		CFont* pOldFont = dc.SelectObject( &m_Font );
		dc.DrawText( srtCaption, rcText, DT_CALCRECT );
		dc.SelectObject( pOldFont );

		if( m_nLabelPosition == 0 )
			sizeLeftOffset.cx = rcText.Width();
		else
			sizeLeftOffset.cy = rcText.Height();
	}
	
	//if( GetBlankViewMode() != bvmFilter )	
	{

		if( m_bEnableEnumeration )
		{
			sizeRightOffset = m_ctrlPicker.GetMinDimension();
		}

		CRect rcEdit, rcPicker;

		rcEdit = rcPicker = rcClient;

		rcEdit.left += sizeLeftOffset.cx;
		rcEdit.right -= sizeRightOffset.cx;

		rcEdit.top += sizeLeftOffset.cy;
		//rcEdit.bottom += sizeLeftOffset.cy;
		m_ctrlEdit.MoveWindow( &rcEdit );

		//determine window style if window height 
		HWND hwnd_edit = m_ctrlEdit.GetSafeHwnd();
		if( hwnd_edit )
		{
			RECT rc_visible = {0};
			::SendMessage( hwnd_edit, EM_GETRECT, 0, (LPARAM)&rc_visible );
			HFONT hfont = (HFONT)::SendMessage( hwnd_edit, WM_GETFONT, 0, 0 );
			if( hfont )
			{
				HDC hdc = ::GetDC( hwnd_edit );
				TEXTMETRIC tm = {0};
				HFONT hfont_old = (HFONT)::SelectObject( hdc, hfont );
				::GetTextMetrics( hdc, &tm );
				::SelectObject( hdc, hfont_old );
				::ReleaseDC( hwnd_edit, hdc );	hdc = 0;

				int nedit_height = rc_visible.bottom - rc_visible.top;
				//is 2 lines visible?
				LONG lstyle = ::GetWindowLong( hwnd_edit, GWL_STYLE );
				LONG lstyle_new = lstyle;
				if( nedit_height >= 2 * tm.tmHeight )
					lstyle_new &= ~ES_AUTOHSCROLL;
				else
					lstyle_new |= ES_AUTOHSCROLL;
				
				//Need recreate control cos SetWindowLong not work on the fly
				if( lstyle_new != lstyle )
				{
					m_ctrlEdit.DestroyWindow();
					m_ctrlEdit.Create( WS_BORDER|lstyle_new,	rcEdit,  this, IDC_EDIT_CTRL);	
					m_ctrlEdit.SetFont( &m_Font );
					//::SetWindowLong( hwnd_edit, GWL_STYLE, lstyle );
				}				
			}
		}


		rcPicker.left = rcPicker.right - sizeRightOffset.cy;
		m_ctrlPicker.MoveWindow( &rcPicker );

		
		CRect rcPopupInit;

		rcPopupInit.left   = rcClient.left;
		rcPopupInit.right  = rcClient.right;
		rcPopupInit.top    = rcEdit.bottom;
		rcPopupInit.bottom = rcPopupInit.top;

		ClientToScreen( &rcPopupInit );
		ClientToScreen( &rcPicker );


		m_ctrlPicker.SetPopupInitRect( rcPopupInit );
		m_ctrlPicker.SetPickerOldRect( rcPicker );

		m_ctrlEdit.ShowWindow(SW_SHOWNA);
		m_ctrlPicker.ShowWindow(SW_SHOWNA);
	}
}


void CDBTextCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);	
	Resize();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);		

	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetFocus();

	SetAciveDBaseField();
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	COleControl::OnRButtonDown(nFlags, point);

	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetFocus();

	SetAciveDBaseField();

}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);		
}


/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_BuildAppearanceAfterLoad( )
{
	//paul 13.02.2002
	//Resize();
	IDBControl_OnChangeBlankViewMode( GetBlankViewMode(), GetBlankViewMode() );
	IDBControl2_OnSetReadOnly( ); // [vanek] BT2000: 3510 - 26.01.2004
    /*	
	AnalizeInputCtrlAppearance();
	GetValue(NULL);
	*/

}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::OnDraw - Drawing function

void CDBTextCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rc = rcBounds;

	
	DrawBorder(pdc, rc, IsActiveDBaseField());
	/*if( IsActiveDBaseField() )
	{
		
		pdc->DrawEdge( &rc, EDGE_SUNKEN, BF_RECT );
	}
	else
	{
		pdc->DrawEdge( &rc, EDGE_RAISED, BF_RECT );
	}*/
	

	if( m_bAutoLabel )
	{
		COLORREF oldColor = pdc->SetBkColor( ::GetSysColor(COLOR_3DFACE) );
		CFont* pOldFont = pdc->SelectObject( &m_Font );
		CRect rc_text = rcBounds;
		rc_text.DeflateRect( 3, 3, 0, 0 );
		pdc->DrawText( GetCaption(), &rc_text, DT_LEFT );
		//pdc->TextOut( 3, 3 , GetCaption() );	
		pdc->SelectObject( pOldFont );
		pdc->SetBkColor( ::GetSysColor(oldColor) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::DoPropExchange - Persistence support

void CDBTextCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	{
		FONTDESC	fontDesc = m_fontDesc;
		CFontHolder fontHolder( &m_xFontNotification );
		fontHolder.ReleaseFont();

		fontHolder.InitializeFont( &m_fontDesc );	
		PX_Font(pPX, _T("TextFont"), fontHolder, &fontDesc);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::OnResetState - Reset control to default state

void CDBTextCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
	//CDBControlImpl::InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextCtrl::AboutBox - Display an "About" box to the user

void CDBTextCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_DBTEXT);
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::OnPropertyChange()
{
	SetModifiedFlag( TRUE );
	if( ::IsWindow( GetSafeHwnd() ) )
		InvalidateControl( );
}


/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetEnableWindow( bool bEnable )
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( bEnable == true );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::EnterViewMode( BlankViewMode newBlankMode )
{
	Resize();

	/*
	if( newBlankMode == bvmFilter )
	{
		m_ctrlFilterCombo.BuildAppearance( true );
	}
	*/

	AnalizeInputCtrlAppearance();

	//paul 13.02.2002 add test mode
	if( newBlankMode != bvmNoInfo )
	{
		GetValue( NULL, true );
		FireCtrlPropChange( GetControllingUnknown() );
	}

}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::Serialize(CArchive& ar)
{
	COleControl::Serialize(ar);	

	long nVersion = 2;
	if( ar.IsStoring() )
	{
		ar << nVersion;
	}
	else
	{
		ar >> nVersion;

	}

	CDBControlImpl::IDBControl_Serialize(ar);

	if( nVersion > 1 )
	{
		if( ar.IsStoring() )
			ar << m_fontDesc;
		else
		{
			ar >> m_fontDesc;

			m_fontHolder.ReleaseFont();
			m_fontHolder.InitializeFont( &m_fontDesc );	
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_OnSetTableName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_OnSetFieldName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_OnSetEnableEnumeration( )
{
	OnPropertyChange();
	//m_ctrlDropDownEdit.SetEnumeration( CDBControlImpl::m_bEnableEnumeration == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_OnSetAutoLabel( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
// [vanek] BT2000: 3510 - 26.01.2004
void CDBTextCtrl::IDBControl2_OnSetReadOnly( )
{
	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetReadOnly( IDBControl2_GetReadOnly( ) );

	if( m_ctrlPicker.GetSafeHwnd( ) )
		m_ctrlPicker.SetDisabled( IDBControl2_GetReadOnly( ) );

	OnPropertyChange( );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBTextCtrl::GetAutoLabel() 
{	
	return IDBControl_GetAutoLabel( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetAutoLabel(BOOL bNewValue) 
{
	IDBControl_SetAutoLabel( bNewValue );
	//m_ctrlDropDownEdit.SetAutoLabel( CDBControlImpl::m_bAutoLabel == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBTextCtrl::GetEnableEnumeration() 
{	
	return IDBControl_GetEnableEnumeration( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetEnableEnumeration(BOOL bNewValue) 
{	
	IDBControl_SetEnableEnumeration( bNewValue );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBTextCtrl::GetTableName() 
{
	CString strResult = IDBControl_GetTableName();	
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetTableName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetTableName( lpszNewValue );	
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBTextCtrl::GetFieldName() 
{
	CString strResult = IDBControl_GetFieldName( );
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetFieldName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetFieldName( lpszNewValue );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_GetValue( tagVARIANT *pvar )
{

}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_SetValue( const tagVARIANT var )
{

}

/////////////////////////////////////////////////////////////////////////////
short CDBTextCtrl::GetLabelPosition() 
{	
	return IDBControl_GetLabelPosition( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetLabelPosition(short nNewValue) 
{	
	IDBControl_SetLabelPosition( nNewValue );	
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_OnSetLabelPosition( )
{
	OnPropertyChange();
}


/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::IDBControl_ChangeValue( const tagVARIANT var )
{	
	if( GetBlankViewMode() != bvmPreview )
		return;

	sptrIDBControl spDBaseCtrl(GetControllingUnknown());
	if( spDBaseCtrl == NULL )
		return;

	IUnknown* punk = NULL;
	spDBaseCtrl->GetDBaseDocument( &punk );

	if( punk == NULL )
		return;

	sptrIDBaseDocument spIDBDoc(punk);
	punk->Release();

	
	if( spIDBDoc )
	{				
		CString strSection = SECTION_DBASEFIELDS;
		CString strKey;
		strKey.Format( "%s\\%s.%s",
						(LPCTSTR)SECTION_DBASEFIELDS, 
						(LPCTSTR)CDBControlImpl::IDBControl_GetTableName(),
						(LPCTSTR)CDBControlImpl::IDBControl_GetFieldName()
						);
		
		sptrINamedData spND( spIDBDoc );
		if( spND )
		{
			m_bCanRecieveNotify = false;
			//spND->SetupSection( strSection.AllocSysString() );
			spND->SetValue( _bstr_t( (LPCTSTR)strKey ), var );
			m_bCanRecieveNotify = true;
		}
	}
	
}

CString CDBTextCtrl::GetValue(sptrIDBaseDocument spDBaseDoc, bool bSetToEditCtrl)
{

	if( spDBaseDoc == NULL )
	{
		spDBaseDoc = m_spDBaseDocument;
		if( spDBaseDoc == NULL )
			return "";
	}

	CString strReturnValue;

	_variant_t var_t;
	CString strKey;
	strKey.Format( "%s\\%s.%s", (LPCTSTR)SECTION_DBASEFIELDS, 
		(LPCTSTR)IDBControl_GetTableName(), (LPCTSTR)IDBControl_GetFieldName() );

	sptrINamedData spND(spDBaseDoc);

	bool bSetValueToEditCtrl = false;

	if( GetBlankViewMode() != vmDesign )
	{
		if( spND != NULL )
		{
			//spND->SetupSection( _bstr_t(SECTION_DBASEFIELDS) );
			if( S_OK == spND->GetValue( _bstr_t(strKey), &var_t ) )
			{
				CString strText;
				if( var_t.vt ==  VT_BSTR )
					strText = var_t.bstrVal;

				strReturnValue = strText;

				if( bSetToEditCtrl )
				{
					if( m_ctrlEdit.GetSafeHwnd() )
					{
						m_ctrlEdit.CanSendMessage( false );
						m_ctrlEdit.SetWindowText( strText );
						m_ctrlEdit.CanSendMessage( true );
					}
				}
			}
		}		
	}
	

	return strReturnValue;
		
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	if( !m_bCanRecieveNotify )
		return;
	if( punkDBaseDocument == NULL )
		return;

	sptrIDBaseDocument spDBaseDoc( punkDBaseDocument );
	if( spDBaseDoc == NULL )
		return;

	CString strEvent = pszEvent;

	if( strEvent == szDBaseEventAfterQueryOpen ||
		strEvent == szDBaseEventAfterNavigation ||
		strEvent == szDBaseEventAfterInsertRecord ||
		strEvent == szDBaseEventAfterDeleteRecord
		)
	{		
		GetValue(spDBaseDoc);
		AnalizeInputCtrlAppearance();
		FireCtrlPropChange( GetControllingUnknown() );
	}

	if( strEvent == szDBaseEventAfterQueryOpen || 
		strEvent == szDBaseEventAfterQueryClose
		)
	{
		AnalizeInputCtrlAppearance();
	}

	if( !strcmp( pszEvent, szDBaseEventFieldChange ) )
	{	
		CString strTableName, strFieldName;	

		if( IDBControl_GetTableName() == bstrTableName &&
			IDBControl_GetFieldName() == bstrFieldName
			)				
		{
			CString strText = "";
			_variant_t var_t(var);
			if( var_t.vt == VT_BSTR )
			{										
				strText = var_t.bstrVal;				
			}

			if( m_ctrlEdit.GetSafeHwnd() )
			{
				m_ctrlEdit.CanSendMessage( false );
				m_ctrlEdit.SetWindowText( strText );					
				m_ctrlEdit.CanSendMessage( true );							
			}

			FireCtrlPropChange( GetControllingUnknown() );
		}
	}		

	/*
	if( !strcmp( pszEvent, szDBaseEventCaptionChange ) )
		ASSERT( false );
		*/

	if( !strcmp( pszEvent, szDBaseEventCaptionChange ) && 
		IDBControl_GetTableName() == bstrTableName &&
		IDBControl_GetFieldName() == bstrFieldName
		)				
	{
		Resize();
		Invalidate();
	}


}


BOOL CDBTextCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->FillRect(rcClient, &CBrush::CBrush( ::GetSysColor(COLOR_3DFACE)));	
	
	return TRUE;
}

LRESULT CDBTextCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_SET_CONTROL_TEXT ) 
	{		
		CString strText = (char*)lParam;								
		if( !m_ctrlEdit.IsSender() )
			if( m_ctrlEdit.GetSafeHwnd() )
				m_ctrlEdit.SetWindowText( strText );

		_variant_t var( strText );		

		IDBControl_ChangeValue( var );

		return TRUE;
	}
	else if( message == WM_SET_FOCUS_TO_CONTROL)
	{
		SetAciveDBaseField();
		return TRUE;
	}

	return COleControl::DefWindowProc(message, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CDBTextCtrl::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CDBTextCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBTextCtrl::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CDBTextCtrl, PrintCtrl)
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBTextCtrl::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CDBTextCtrl, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBTextCtrl::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CDBTextCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBTextCtrl::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CDBTextCtrl, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::DrawContext( HDC hDC, SIZE size )
{
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);

	//Now draw!

	CRect rcBounds = CRect( 0, 0, size.cx, size.cy );
	CRect rcCalc(0,0,0,0);
	CRect rcOut;		

	int nBoundsWidth  = rcBounds.Width();
	int nBoundsHeight = rcBounds.Height();

	LOGFONT lf;
	HFONT hFont = m_fontHolder.GetFontHandle( );
	CFont* pFont = CFont::FromHandle( hFont );

	pFont->GetLogFont( &lf );


	{
		double fAspect = 1.0 / 72.0 * 25.2 * 10;
	
		IFontPtr fontPtr( m_fontHolder.m_pFont );

		if( fontPtr != 0 )
		{
			CY		sizeCY;
			fontPtr->get_Size( &sizeCY );

			lf.lfHeight = (int)(((double)sizeCY.Lo)/(1000.0) * fAspect);
		}
	}	

	CFont fontToPrint;			

			fontToPrint.CreateFont( 
				lf.lfHeight, 0, 
				0, 0, 
				lf.lfWeight, lf.lfItalic, 
				lf.lfUnderline, lf.lfStrikeOut, 
				lf.lfCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,DEFAULT_PITCH,						
				lf.lfFaceName 
				);
	
	CFont* pOldFont = pdc->SelectObject( &fontToPrint );

	CString strValue = GetValue( NULL, false );


	CSize sizeLeftOffset(0,0);
	
	int nmode = pdc->SetBkMode( TRANSPARENT );

	if( m_bAutoLabel )
	{
		CString strCaption = GetCaption();		
		CRect rcCaption(0,0,0,0);

		pdc->DrawText( strCaption, &rcCaption , DT_CALCRECT );			
		rcCaption.NormalizeRect();

		if( m_nLabelPosition == 0 )
			sizeLeftOffset.cx = rcCaption.Width();
		else
			sizeLeftOffset.cy = rcCaption.Height();

		rcOut = CRect( 0, nBoundsHeight, rcCaption.Width(), nBoundsHeight - rcCaption.Height() );
		CorrectHimetricsRect( &rcOut, &rcBounds );
		pdc->DrawText( strCaption, &rcOut , DT_LEFT );			

	}



	BOOL bResult = pdc->DrawText( strValue, &rcCalc , DT_CALCRECT );			
	rcCalc.NormalizeRect();

	int nTextWidth  = rcCalc.Width();
	int nTextHeight = rcCalc.Height();

	int nX, nY;

	nY = nBoundsHeight-sizeLeftOffset.cy;
	nX = sizeLeftOffset.cx;

	rcOut = CRect( nX, nY, nX + rcBounds.Width() - sizeLeftOffset.cx, nY - nTextHeight );
	CorrectHimetricsRect( &rcOut, &rcBounds );
	rcOut.bottom = rcBounds.top;
	bResult = pdc->DrawText( strValue, &rcOut , DT_LEFT|DT_WORDBREAK );			

	pdc->SelectObject( pOldFont );
	pdc->SetBkMode( nmode );
}


//Call from EnterViewMode ,OnDBaseNotify, OnCreate

void CDBTextCtrl::AnalizeInputCtrlAppearance()
{

	if( !::IsWindow( m_ctrlEdit ) )
		return;

	BOOL bQueryOpen = FALSE;
	sptrISelectQuery spQuery = GetActiveQuery();	
	long nRecordCount = 0;

	if( spQuery )
	{
		spQuery->IsOpen( &bQueryOpen );
		spQuery->GetRecordCount( &nRecordCount );
	}
	

	BlankViewMode blankViewMode = GetBlankViewMode();

	if( blankViewMode == bvmPreview )
	{

		if( !m_ctrlEdit.IsWindowEnabled() )
			m_ctrlEdit.EnableWindow( TRUE );
	}
	
	
	if( blankViewMode != bvmPreview || !bQueryOpen || nRecordCount < 1 )
	{
		if( m_ctrlEdit.IsWindowEnabled() )
			m_ctrlEdit.EnableWindow( FALSE );

		m_ctrlEdit.CanSendMessage( false );
		m_ctrlEdit.SetWindowText( "" );
		m_ctrlEdit.CanSendMessage( true );
	}
	
}


/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::RefreshAppearance()
{
	Resize();
}

LPFONTDISP CDBTextCtrl::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.ReleaseFont( );
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);

	IFontPtr fontPtr = m_fontHolder.m_pFont;

	m_fontDesc.cbSizeofstruct = sizeof( FONTDESC );    
	fontPtr->get_Italic( &m_fontDesc.fItalic );
	fontPtr->get_Underline( &m_fontDesc.fUnderline );
	fontPtr->get_Strikethrough( &m_fontDesc.fStrikethrough );
	fontPtr->get_Charset( &m_fontDesc.sCharset );

	// [vanek] 14.01.2004 
	if( m_fontDesc.sCharset == ANSI_CHARSET )
		m_fontDesc.sCharset = DEFAULT_CHARSET;

	fontPtr->get_Weight( &m_fontDesc.sWeight );
	fontPtr->get_Size( &m_fontDesc.cySize );
	fontPtr->get_Name( &m_fontDesc.lpstrName );

	OnPropertyChange();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBTextCtrl::GetReadOnly()
{
	return IDBControl2_GetReadOnly( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBTextCtrl::SetReadOnly(BOOL bNewValue)
{
	IDBControl2_SetReadOnly( bNewValue );
}


void CDBTextCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( IDBControl2_GetReadOnly() && (int)g_DoNotActivateReadonly)
	{
		// To avoid SetFocus in COleControl::OnButtonDown (ctrlevent.cpp)
		HWND hWndSave = m_hWnd;
		ButtonDown(LEFT_BUTTON, nFlags, point);
		if (m_hWnd != hWndSave)
			return;
		Default();
	}
	else
		__super::OnLButtonDown(nFlags, point);
}

//added by akm 22_11_k5
BOOL CDBTextCtrl::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CDBTextCtrl::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}