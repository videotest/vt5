// DBNumberCtl.cpp : Implementation of the CDBNumberCtrl ActiveX Control class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBNumberCtl.h"
#include "DBNumberPpg.h"
#include "DBCommonPropPage.h"


#define IDC_EDIT_CTRL	3333 

IMPLEMENT_DYNCREATE(CDBNumberCtrl, COleControl)

BEGIN_INTERFACE_MAP(CDBNumberCtrl, COleControl)
	INTERFACE_PART(CDBNumberCtrl, IID_IDBControl, DBControl)
	INTERFACE_PART(CDBNumberCtrl, IID_IDBControl2, DBControl)	
	INTERFACE_PART(CDBNumberCtrl, IID_IDBaseListener, DBaseListener)	
	INTERFACE_PART(CDBNumberCtrl, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CDBNumberCtrl, IID_IAXCtrlDataSite, AXCtrlDataSite)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDBNumberCtrl, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CDBNumberCtrl)

CORRECT_MFC_OnSetObjectRects(CDBNumberCtrl);
/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBNumberCtrl, COleControl)
	//{{AFX_MSG_MAP(CDBNumberCtrl)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CDBNumberCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CDBNumberCtrl)
	DISP_PROPERTY_EX(CDBNumberCtrl, "TableName", GetTableName, SetTableName, VT_BSTR)
	DISP_PROPERTY_EX(CDBNumberCtrl, "FieldName", GetFieldName, SetFieldName, VT_BSTR)
	DISP_PROPERTY_EX(CDBNumberCtrl, "EnableEnumeration", GetEnableEnumeration, SetEnableEnumeration, VT_BOOL)
	DISP_PROPERTY_EX(CDBNumberCtrl, "AutoLabel", GetAutoLabel, SetAutoLabel, VT_BOOL)
	DISP_PROPERTY_EX(CDBNumberCtrl, "LabelPosition", GetLabelPosition, SetLabelPosition, VT_I2)
	DISP_PROPERTY_EX(CDBNumberCtrl, "TextFont", GetTextFont, SetTextFont, VT_FONT)
	DISP_PROPERTY_EX(CDBNumberCtrl, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_FUNCTION(CDBNumberCtrl, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDBNumberCtrl, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CDBNumberCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CDBNumberCtrl, COleControl)
	//{{AFX_EVENT_MAP(CDBNumberCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CDBNumberCtrl, 2)
	//PROPPAGEID(CDBNumberPropPage::guid)
	PROPPAGEID(CDBCommonPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CDBNumberCtrl)
*/
static CLSID pages[2];

LPCLSID CDBNumberCtrl::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CDBCommonPropPage::guid;
	pages[cPropPages++]=CLSID_StockFontPage;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBNumberCtrl, DBNumberProgID,
	0x2868adbf, 0xeae, 0x4038, 0xa2, 0x26, 0x82, 0x73, 0x32, 0xec, 0xa3, 0x26)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CDBNumberCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

extern const IID BASED_CODE IID_DDBNumber =
		{ 0x739709c4, 0xa075, 0x4520, { 0xb4, 0x3, 0xbf, 0xe5, 0xd8, 0xf1, 0x60, 0xd9 } };
const IID BASED_CODE IID_DDBNumberEvents =
		{ 0xd6dd5a3c, 0x473c, 0x482f, { 0xb1, 0xe1, 0x48, 0x89, 0x98, 0xca, 0x12, 0xad } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwDBNumberOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDBNumberCtrl, IDS_DBNUMBER, _dwDBNumberOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::CDBNumberCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBNumberCtrl

BOOL CDBNumberCtrl::CDBNumberCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_DBNUMBER,
			IDB_DBNUMBER,
			afxRegApartmentThreading,
			_dwDBNumberOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::CDBNumberCtrl - Constructor
CDBNumberCtrl::CDBNumberCtrl() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DDBNumber, &IID_DDBNumberEvents);

	CDBControlImpl::InitDefaults();	

	init_default_font( &m_fontDesc );

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	//added by akm 13_12_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::~CDBNumberCtrl - Destructor

CDBNumberCtrl::~CDBNumberCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

/////////////////////////////////////////////////////////////////////////////
int CDBNumberCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectClient;
	GetClientRect( &rectClient );
	

	RegisterDB();

	CRect rc = CRect(0,0,0,200);		

	m_ctrlEdit.Create(WS_CHILD | WS_BORDER| WS_TABSTOP | 
							WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL ,
					rc,  this, IDC_EDIT_CTRL );	
	m_ctrlPicker.Create( NULL, NULL, WS_CHILD | WS_VISIBLE| WS_TABSTOP, rc, this, 102 );	



	m_ctrlEdit.SetFont( &m_Font );
	m_ctrlPicker.SetFont( &m_Font );

	Resize();
	AnalizeInputCtrlAppearance();

	//GetValue( NULL );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::Resize()
{

	if( !GetSafeHwnd() ) 
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
	//if( 1 == 1 )
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

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetFocus();

	SetAciveDBaseField();
	
}

void CDBNumberCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{		
	COleControl::OnRButtonDown(nFlags, point);

	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetFocus();

	SetAciveDBaseField();

}


/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	Resize();
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_BuildAppearanceAfterLoad( )
{
	//paul 13.02.2002
	//Resize();
	IDBControl_OnChangeBlankViewMode( GetBlankViewMode(), GetBlankViewMode() );
	IDBControl2_OnSetReadOnly( ); // [vanek] BT2000: 3510 - 26.01.2004
	//paul 13.02.2002
	//GetValue(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::OnDraw - Drawing function

void CDBNumberCtrl::OnDraw(
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
// CDBNumberCtrl::DoPropExchange - Persistence support

void CDBNumberCtrl::DoPropExchange(CPropExchange* pPX)
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

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::OnResetState - Reset control to default state

void CDBNumberCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	//CDBControlImpl::InitDefaults();	
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl::AboutBox - Display an "About" box to the user

void CDBNumberCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_DBNUMBER);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::OnPropertyChange()
{
	SetModifiedFlag( TRUE );
	if( ::IsWindow( GetSafeHwnd() ) )
		InvalidateControl( );
}



/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::Serialize(CArchive& ar)
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
  /*
			HFONT hFont = m_fontHolder.GetFontHandle( );
			CFont* pFont = CFont::FromHandle( hFont );

			LOGFONT lf;
			pFont->GetLogFont( &lf );
			lf.lfCharSet = DEFAULT_CHARSET;

			m_Font.DeleteObject();		
			m_Font.CreateFontIndirect( &lf );
*/

		}
	}

}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_OnSetTableName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_OnSetFieldName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_OnSetEnableEnumeration( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_OnSetAutoLabel( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
// [vanek] BT2000: 3510 - 26.01.2004
void CDBNumberCtrl::IDBControl2_OnSetReadOnly( )
{
	if( m_ctrlEdit.GetSafeHwnd() )
		m_ctrlEdit.SetReadOnly( IDBControl2_GetReadOnly( ) );

	if( m_ctrlPicker.GetSafeHwnd( ) )
		m_ctrlPicker.SetDisabled( IDBControl2_GetReadOnly( ) );

	OnPropertyChange( );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBNumberCtrl::GetAutoLabel() 
{	
	return IDBControl_GetAutoLabel( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetAutoLabel(BOOL bNewValue) 
{
	IDBControl_SetAutoLabel( bNewValue );
	//m_ctrlDropDownEdit.SetAutoLabel( CDBControlImpl::m_bAutoLabel == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBNumberCtrl::GetEnableEnumeration() 
{	
	return IDBControl_GetEnableEnumeration( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetEnableEnumeration(BOOL bNewValue) 
{	
	IDBControl_SetEnableEnumeration( bNewValue );
	//m_ctrlDropDownEdit.SetEnumeration( CDBControlImpl::m_bEnableEnumeration == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBNumberCtrl::GetTableName() 
{
	CString strResult = IDBControl_GetTableName();	
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetTableName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetTableName( lpszNewValue );	
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBNumberCtrl::GetFieldName() 
{
	CString strResult = IDBControl_GetFieldName( );
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetFieldName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetFieldName( lpszNewValue );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_GetValue( tagVARIANT *pvar )
{

}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_SetValue( const tagVARIANT var )
{

}

/////////////////////////////////////////////////////////////////////////////
short CDBNumberCtrl::GetLabelPosition() 
{	
	return IDBControl_GetLabelPosition( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetLabelPosition(short nNewValue) 
{	
	IDBControl_SetLabelPosition( nNewValue );
	//m_ctrlDropDownEdit.SetLabelPosition( IDBControl_GetLabelPosition() );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_OnSetLabelPosition( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetEnableWindow( bool bEnable )
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( bEnable == true );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::EnterViewMode( BlankViewMode newBlankMode )
{
	Resize();

	AnalizeInputCtrlAppearance();

	//paul 13.02.2002 add test mode
	if( newBlankMode != bvmNoInfo )
	{
		GetValue( NULL, true );
		FireCtrlPropChange( GetControllingUnknown() );
	}
	/*
	if( newBlankMode == bvmFilter )
	{
		m_ctrlFilterCombo.BuildAppearance( true );
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::IDBControl_ChangeValue( const tagVARIANT var )
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
		_variant_t _var( var );
		try
		{			
			_var.ChangeType( VT_R8 );
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
		catch(...){} // do nothing
		
	}	
}

CString CDBNumberCtrl::GetValue(sptrIDBaseDocument spDBaseDoc, bool bSetToEditCtrl)
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

		if( spND != NULL )
		{
			//spND->SetupSection( _bstr_t(SECTION_DBASEFIELDS) );
			if( S_OK == spND->GetValue( _bstr_t(strKey), &var_t ) )
			{
				CString strText;
				if( var_t.vt == VT_R8 )
				{				
					try{
						var_t.ChangeType( VT_BSTR );
						strText = var_t.bstrVal;									
					}
					catch(...)
					{				
					}
				}				

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

	return strReturnValue;
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
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
			if( var_t.vt == VT_R8 )
			{
				try{
					var_t.ChangeType( VT_BSTR );
					strText = var_t.bstrVal;					
				}
				catch(...)
				{
					//ASSERT(FALSE);
				}
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


	if( !strcmp( pszEvent, szDBaseEventCaptionChange ) && 
		IDBControl_GetTableName() == bstrTableName &&
		IDBControl_GetFieldName() == bstrFieldName
		)				
	{
		Resize();
		Invalidate();
	}

	
}


BOOL CDBNumberCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->FillRect(rcClient, &CBrush::CBrush( ::GetSysColor(COLOR_3DFACE)));	
	return TRUE;
}

LRESULT CDBNumberCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_SET_CONTROL_TEXT ) 
	{
		CString strText = (char*)lParam;								
		if( !m_ctrlEdit.IsSender() )
		{
			if( m_ctrlEdit.GetSafeHwnd() )
				m_ctrlEdit.SetWindowText( strText );
		}

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
HRESULT CDBNumberCtrl::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CDBNumberCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBNumberCtrl::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CDBNumberCtrl, PrintCtrl)
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBNumberCtrl::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CDBNumberCtrl, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBNumberCtrl::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CDBNumberCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBNumberCtrl::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CDBNumberCtrl, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::DrawContext( HDC hDC, SIZE size )
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
//	m_Font.GetLogFont( &lf );
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

	
	rcOut = CRect( nX, nY, nX + nTextWidth, nY - nTextHeight );

	CorrectHimetricsRect( &rcOut, &rcBounds );
	bResult = pdc->DrawText( strValue, &rcOut , DT_LEFT );			
	
	pdc->SetBkMode( nmode );
	pdc->SelectObject( pOldFont );
}


/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::AnalizeInputCtrlAppearance()
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
void CDBNumberCtrl::RefreshAppearance()
{
	Resize();
}

LPFONTDISP CDBNumberCtrl::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.ReleaseFont( );
	
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);	
/*

	HFONT hFont = m_fontHolder.GetFontHandle( );
	CFont* pFont = CFont::FromHandle( hFont );

	LOGFONT lf;
	pFont->GetLogFont( &lf );
	lf.lfCharSet = DEFAULT_CHARSET;

	m_Font.DeleteObject();		
	m_Font.CreateFontIndirect( &lf );
*/
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
BOOL CDBNumberCtrl::GetReadOnly()
{
	return IDBControl2_GetReadOnly( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBNumberCtrl::SetReadOnly(BOOL bNewValue)
{
	IDBControl2_SetReadOnly( bNewValue );
}


BOOL CDBNumberCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if( LOWORD(wParam) == IDC_EDIT_CTRL && HIWORD(wParam) == EN_KILLFOCUS )
		GetValue( 0, true );

	return __super::OnCommand(wParam, lParam);
}

void CDBNumberCtrl::OnLButtonDown(UINT nFlags, CPoint point)
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
BOOL CDBNumberCtrl::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CDBNumberCtrl::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}