// DBDateTimeCtl.cpp : Implementation of the CDBDateTimeCtrl ActiveX Control class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBDateTimeCtl.h"
#include "DBDateTimePpg.h"
#include "DBCommonPropPage.h"

#include "EditCtrl.h"

#include "PrintConst.h"


IMPLEMENT_DYNCREATE(CDBDateTimeCtrl, COleControl)

BEGIN_INTERFACE_MAP(CDBDateTimeCtrl, COleControl)
	INTERFACE_PART(CDBDateTimeCtrl, IID_IDBControl, DBControl)	
	INTERFACE_PART(CDBDateTimeCtrl, IID_IDBControl2, DBControl)	
	INTERFACE_PART(CDBDateTimeCtrl, IID_IDBaseListener, DBaseListener)	
	INTERFACE_PART(CDBDateTimeCtrl, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CDBDateTimeCtrl, IID_IAXCtrlDataSite, AXCtrlDataSite)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDBDateTimeCtrl, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CDBDateTimeCtrl)


CORRECT_MFC_OnSetObjectRects(CDBDateTimeCtrl);

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBDateTimeCtrl, COleControl)
	//{{AFX_MSG_MAP(CDBDateTimeCtrl)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(DTN_DATETIMECHANGE, DATETIME_PICKER_ID, OnDatetimeChange)
	ON_NOTIFY(NM_SETFOCUS, DATETIME_PICKER_ID, OnSetfocusDatetimepicker)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CDBDateTimeCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CDBDateTimeCtrl)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "AutoLabel", GetAutoLabel, SetAutoLabel, VT_BOOL)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "EnableEnumeration", GetEnableEnumeration, SetEnableEnumeration, VT_BOOL)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "TableName", GetTableName, SetTableName, VT_BSTR)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "FieldName", GetFieldName, SetFieldName, VT_BSTR)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "LabelPosition", GetLabelPosition, SetLabelPosition, VT_I2)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "TextFont", GetTextFont, SetTextFont, VT_FONT)
	DISP_PROPERTY_EX(CDBDateTimeCtrl, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_FUNCTION(CDBDateTimeCtrl, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDBDateTimeCtrl, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CDBDateTimeCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CDBDateTimeCtrl, COleControl)
	//{{AFX_EVENT_MAP(CDBDateTimeCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CDBDateTimeCtrl, 2)
	//PROPPAGEID(CDBDateTimePropPage::guid)
	PROPPAGEID(CDBCommonPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CDBDateTimeCtrl)
*/
static CLSID pages[2];

LPCLSID CDBDateTimeCtrl::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CDBCommonPropPage::guid;
	pages[cPropPages++]=CLSID_StockFontPage;
	m_PropertyAXStatus=true;

	return pages;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBDateTimeCtrl, DBDateTimeProgID,
0x1ac3aeb0, 0x92ec, 0x4e43, 0x89, 0xd3, 0x86, 0x65, 0xed, 0x35, 0x18, 0x99)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CDBDateTimeCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

extern const IID BASED_CODE IID_DDBDateTime =
		{ 0x414f5379, 0xb140, 0x4586, { 0x87, 0xa5, 0x98, 0xd9, 0xb8, 0x2b, 0xe4, 0xce } };
const IID BASED_CODE IID_DDBDateTimeEvents =
		{ 0xf8205caf, 0x6501, 0x4e8f, { 0x9d, 0x6a, 0x4b, 0xfa, 0xec, 0xf1, 0x6e, 0xb } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwDBDateTimeOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDBDateTimeCtrl, IDS_DBDATETIME, _dwDBDateTimeOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimeCtrl::CDBDateTimeCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBDateTimeCtrl

BOOL CDBDateTimeCtrl::CDBDateTimeCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_DBDATETIME,
			IDB_DBDATETIME,
			afxRegApartmentThreading,
			_dwDBDateTimeOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimeCtrl::CDBDateTimeCtrl - Constructor

CDBDateTimeCtrl::CDBDateTimeCtrl() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DDBDateTime, &IID_DDBDateTimeEvents);
	CDBControlImpl::InitDefaults();	

	init_default_font( &m_fontDesc );

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	//added by akm 13_12_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimeCtrl::~CDBDateTimeCtrl - Destructor

CDBDateTimeCtrl::~CDBDateTimeCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

/////////////////////////////////////////////////////////////////////////////
int CDBDateTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectClient;
	GetClientRect( &rectClient );
	
	RegisterDB();


	CRect rc = CRect(0,0,0,200);		
	m_ctrlDateTimePicker.Create( DTS_SHORTDATEFORMAT|/*DTS_SHOWNONE|*/WS_CHILD|WS_VISIBLE, rc, this, DATETIME_PICKER_ID );
	//m_ctrlDateTimePicker.CreateControl( m_ctrlDateTimePicker.GetClsid(), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rc, this, 100 );
	m_ctrlPicker.Create( NULL, NULL, WS_CHILD | WS_VISIBLE| WS_TABSTOP, rc, this, 102 );
	
	//m_ctrlEdit.SetFont( &m_Font );
	m_ctrlPicker.SetFont( &m_Font );

	Resize();

	AnalizeInputCtrlAppearance();
	//GetValue( NULL );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::Resize()
{

	if( !GetSafeHwnd() ) 
		return;

	if( !::IsWindow( m_ctrlPicker.GetSafeHwnd() ) )
		return;


	if( !::IsWindow( m_ctrlDateTimePicker.GetSafeHwnd() ) )
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
		m_ctrlDateTimePicker.MoveWindow( &rcEdit );


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

		m_ctrlDateTimePicker.ShowWindow(SW_SHOWNA);
		m_ctrlPicker.ShowWindow(SW_SHOWNA);
		
	}
}


/////////////////////////////////////////////////////////////////////////////
LRESULT CDBDateTimeCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_SET_CONTROL_TEXT ) 
	{
		CString strText = (char*)lParam;		
		_variant_t var_t(strText);
		if( var_t.vt == VT_DATE )
		{
			try{
				if( m_ctrlDateTimePicker.GetSafeHwnd() )
				{
					m_ctrlDateTimePicker.SetTime( var_t );					
					m_ctrlDateTimePicker.Invalidate();
				}
			}
			catch(...){}
		}

		return TRUE;
	}
	
	
	return COleControl::DefWindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::OnSetfocusDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetAciveDBaseField();
	*pResult = 0;	
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);			
	if( m_ctrlDateTimePicker.GetSafeHwnd() )
		m_ctrlDateTimePicker.SetFocus();

	SetAciveDBaseField();
}

void CDBDateTimeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	COleControl::OnRButtonDown(nFlags, point);

	if( m_ctrlDateTimePicker.GetSafeHwnd() )
		m_ctrlDateTimePicker.SetFocus();

	SetAciveDBaseField();
}


/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);		
	Resize();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_BuildAppearanceAfterLoad( )
{
	//paul 13.02.2002
	//Resize();
	IDBControl_OnChangeBlankViewMode( GetBlankViewMode(), GetBlankViewMode() );
    IDBControl2_OnSetReadOnly( ); // [vanek] 26.01.2004
	//paul 13.02.2002
	//GetValue(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimeCtrl::OnDraw - Drawing function
void CDBDateTimeCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rc = rcBounds;
	
	DrawBorder(pdc, rc, IsActiveDBaseField());
/*	if( IsActiveDBaseField() )
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
// CDBDateTimeCtrl::DoPropExchange - Persistence support
void CDBDateTimeCtrl::DoPropExchange(CPropExchange* pPX)
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
// CDBDateTimeCtrl::OnResetState - Reset control to default state

void CDBDateTimeCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	//CDBControlImpl::InitDefaults();	
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimeCtrl::AboutBox - Display an "About" box to the user

void CDBDateTimeCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_DBDATETIME);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////

void CDBDateTimeCtrl::OnPropertyChange()
{
	SetModifiedFlag( TRUE );
	if( ::IsWindow( GetSafeHwnd() ) )
		InvalidateControl( );
}



/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::Serialize(CArchive& ar)
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
void CDBDateTimeCtrl::IDBControl_OnSetTableName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_OnSetFieldName(  )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_OnSetEnableEnumeration( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_OnSetAutoLabel( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl2_OnSetReadOnly( )
{
	if( m_ctrlPicker.GetSafeHwnd( ) )
		m_ctrlPicker.SetDisabled( IDBControl2_GetReadOnly( ) );

	if( m_ctrlDateTimePicker.GetSafeHwnd( ) )
		m_ctrlDateTimePicker.SetReadOnly( IDBControl2_GetReadOnly( ) );

    OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBDateTimeCtrl::GetAutoLabel() 
{	
	return IDBControl_GetAutoLabel( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetAutoLabel(BOOL bNewValue) 
{
	IDBControl_SetAutoLabel( bNewValue );	
	//m_ctrlDropDownEdit.SetAutoLabel( CDBControlImpl::m_bAutoLabel == TRUE );	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBDateTimeCtrl::GetEnableEnumeration() 
{	
	return IDBControl_GetEnableEnumeration( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetEnableEnumeration(BOOL bNewValue) 
{	
	IDBControl_SetEnableEnumeration( bNewValue );
	//m_ctrlDropDownEdit.SetEnumeration( CDBControlImpl::m_bEnableEnumeration == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBDateTimeCtrl::GetTableName() 
{
	CString strResult = IDBControl_GetTableName();	
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetTableName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetTableName( lpszNewValue );	
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBDateTimeCtrl::GetFieldName() 
{
	CString strResult = IDBControl_GetFieldName( );
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetFieldName(LPCTSTR lpszNewValue) 
{	
	IDBControl_SetFieldName( lpszNewValue );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_GetValue( tagVARIANT *pvar )
{

}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_SetValue( const tagVARIANT var )
{

}

/////////////////////////////////////////////////////////////////////////////
short CDBDateTimeCtrl::GetLabelPosition() 
{	
	return IDBControl_GetLabelPosition( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetLabelPosition(short nNewValue) 
{	
	IDBControl_SetLabelPosition( nNewValue );
	//m_ctrlDropDownEdit.SetLabelPosition( IDBControl_GetLabelPosition() );
}


/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::OnDatetimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	COleDateTime dtime;
	CTime timeTime;
	_variant_t var;
	
	if( m_ctrlDateTimePicker.GetSafeHwnd() )
	{
		if( m_ctrlDateTimePicker.GetTime( timeTime ) == GDT_VALID )
		{
			SYSTEMTIME	sysTime;

			timeTime.GetAsSystemTime( sysTime );
			dtime = sysTime;
			var = _variant_t( dtime, VT_DATE );
		}

		IDBControl_ChangeValue( var );
	}

}


/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::IDBControl_OnSetLabelPosition( )
{
	OnPropertyChange();
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetEnableWindow( bool bEnable )
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( bEnable == true );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::EnterViewMode( BlankViewMode newBlankMode )
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
void CDBDateTimeCtrl::IDBControl_ChangeValue( const tagVARIANT var )
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

	
	if( spIDBDoc)
	{
		_variant_t _var( var );
		try
		{			
			if( _var.vt != VT_EMPTY )
				_var.ChangeType( VT_DATE );

			CString strSection = SECTION_DBASEFIELDS;
			CString strKey;
			strKey.Format( "%s\\%s.%s", (LPCTSTR)SECTION_DBASEFIELDS, 
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


/////////////////////////////////////////////////////////////////////////////
CString CDBDateTimeCtrl::GetValue(sptrIDBaseDocument spDBaseDoc, bool bSetToEditCtrl)
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
				if( var_t.vt == VT_DATE )
				{
					if( bSetToEditCtrl )
					{
						if( m_ctrlDateTimePicker.GetSafeHwnd() )
						{
							m_ctrlDateTimePicker.SetTime( var_t );
							m_ctrlDateTimePicker.Invalidate();
						}
					}

					COleDateTime dt( var_t );										
					strReturnValue = dt.Format( VAR_DATEVALUEONLY );
				}
				else
				{
					if( bSetToEditCtrl )
					{
						if( m_ctrlDateTimePicker.GetSafeHwnd() )
						{
							m_ctrlDateTimePicker.SendMessage( DTM_SETSYSTEMTIME, GDT_NONE, 0 );
						}
					}
				}
			}

		}

	

	return strReturnValue;
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
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
		GetValue( spDBaseDoc );		
		FireCtrlPropChange( GetControllingUnknown() );
		AnalizeInputCtrlAppearance();
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
			GetValue( spDBaseDoc, true );
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


BOOL CDBDateTimeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->FillRect(rcClient, &CBrush::CBrush( ::GetSysColor(COLOR_3DFACE)));	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CDBDateTimeCtrl::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CDBDateTimeCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBDateTimeCtrl::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CDBDateTimeCtrl, PrintCtrl)
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBDateTimeCtrl::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CDBDateTimeCtrl, PrintCtrl)
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBDateTimeCtrl::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CDBDateTimeCtrl, PrintCtrl)	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBDateTimeCtrl::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CDBDateTimeCtrl, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::DrawContext( HDC hDC, SIZE size )
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
				1,
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
		pdc->DrawText( strCaption, &rcOut, DT_LEFT );			

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








BOOL CDBDateTimeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( wParam == DATETIME_PICKER_ID )
	{
		NMHDR* p = (NMHDR*)lParam;
		if( p )
		{
			if( p->code == NM_SETFOCUS || p->code == NM_KILLFOCUS )
			{
				Invalidate();
			}
		}
	}
	
	return COleControl::OnNotify(wParam, lParam, pResult);
}

void CDBDateTimeCtrl::AnalizeInputCtrlAppearance()
{

	if( !::IsWindow( m_ctrlDateTimePicker ) )
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

		if( !m_ctrlDateTimePicker.IsWindowEnabled() )
			m_ctrlDateTimePicker.EnableWindow( TRUE );
	}
	
	
	if( blankViewMode != bvmPreview || !bQueryOpen || nRecordCount < 1 )
	{
		if( m_ctrlDateTimePicker.IsWindowEnabled() )
			m_ctrlDateTimePicker.EnableWindow( FALSE );
	   /*
		m_ctrlDateTimePicker.CanSendMessage( false );
		m_ctrlDateTimePicker.SetWindowText( "" );
		m_ctrlDateTimePicker.CanSendMessage( true );
		*/
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::RefreshAppearance()
{
	Resize();
}

LPFONTDISP CDBDateTimeCtrl::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.ReleaseFont( );
	
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);	

	HFONT hFont = m_fontHolder.GetFontHandle( );
	CFont* pFont = CFont::FromHandle( hFont );
/*
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
BOOL CDBDateTimeCtrl::GetReadOnly()
{
	return IDBControl2_GetReadOnly( );
}

/////////////////////////////////////////////////////////////////////////////
void CDBDateTimeCtrl::SetReadOnly(BOOL bNewValue)
{
	IDBControl2_SetReadOnly( bNewValue );
}

//added by akm 22_11_k5
BOOL CDBDateTimeCtrl::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CDBDateTimeCtrl::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}