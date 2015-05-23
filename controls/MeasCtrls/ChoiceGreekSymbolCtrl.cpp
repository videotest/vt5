// ChoiceGreekSymbolCtrl.cpp : Implementation of the CChoiceGreekSymbolCtrl ActiveX Control class.

#include "stdafx.h"
#include "MeasCtrls.h"
#include "ChoiceGreekSymbolCtrl.h"
//#include "ChoiceGreekSymbolPropPage.h"

#include <comdef.h>
#include <msstkppg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CChoiceGreekSymbolCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CChoiceGreekSymbolCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_DRAWITEM_REFLECT()
    ON_WM_LBUTTONDOWN()
    ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CChoiceGreekSymbolCtrl, COleControl)
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FORECOLOR()
	DISP_STOCKPROP_FONT()
	DISP_PROPERTY_EX_ID(CChoiceGreekSymbolCtrl, "FirstChar", dispidFirstChar, GetFirstChar, SetFirstChar, VT_I4)
	DISP_PROPERTY_EX_ID(CChoiceGreekSymbolCtrl, "LastChar", dispidLastChar, GetLastChar, SetLastChar, VT_I4)
	DISP_PROPERTY_EX_ID(CChoiceGreekSymbolCtrl, "ActiveItem", dispidActiveItem, GetActiveItem, SetActiveItem, VT_I4)
	DISP_FUNCTION_ID(CChoiceGreekSymbolCtrl, "GetItemCount", dispidGetItemCount, GetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CChoiceGreekSymbolCtrl, "GetChar", dispidGetChar, GetChar, VT_I4, VTS_I4)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CChoiceGreekSymbolCtrl, COleControl)
	EVENT_CUSTOM_ID("OnActiveItemChange", eventidOnActiveItemChange, OnActiveItemChange, VTS_I4)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CChoiceGreekSymbolCtrl, 2)
	//PROPPAGEID(CChoiceGreekSymbolPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)
END_PROPPAGEIDS(CChoiceGreekSymbolCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CChoiceGreekSymbolCtrl, "CHOICEGREEKSYMBO.ChoiceGreekSymboCtrl.1",
	0x896fb8cb, 0xc056, 0x451f, 0x9d, 0xa6, 0x33, 0x71, 0x4b, 0xf9, 0x2b, 0x4d)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CChoiceGreekSymbolCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DChoiceGreekSymbol =
		{ 0xD1BF7023, 0x90E9, 0x4DD6, { 0x9F, 0x4A, 0x2D, 0x90, 0x3A, 0x6A, 0xDD, 0xF } };
const IID BASED_CODE IID_DChoiceGreekSymbolEvents =
		{ 0x5AEFB763, 0x3801, 0x4D40, { 0xB2, 0x60, 0x41, 0xC0, 0x2, 0x23, 0x2D, 0xF6 } };



// Control type information

static const DWORD BASED_CODE _dwChoiceGreekSymbolOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CChoiceGreekSymbolCtrl, IDS_CHOICEGREEKSYMBOL, _dwChoiceGreekSymbolOleMisc)



// CChoiceGreekSymbolCtrl::CChoiceGreekSymbolCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CChoiceGreekSymbolCtrl

BOOL CChoiceGreekSymbolCtrl::CChoiceGreekSymbolCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_CHOICEGREEKSYMBOL,
			IDB_CHOICEGREEKSYMBOL,
			afxRegApartmentThreading,
			_dwChoiceGreekSymbolOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CChoiceGreekSymbolCtrl::CChoiceGreekSymbolCtrl - Constructor

CChoiceGreekSymbolCtrl::CChoiceGreekSymbolCtrl()
{
	InitializeIIDs(&IID_DChoiceGreekSymbol, &IID_DChoiceGreekSymbolEvents);
	// TODO: Initialize your control's instance data here.
	m_lFirstChar = 0xBE;
	m_lLastChar = 0xFE;
	m_lActiveItem = -1;
	init_default( );
}



// CChoiceGreekSymbolCtrl::~CChoiceGreekSymbolCtrl - Destructor

CChoiceGreekSymbolCtrl::~CChoiceGreekSymbolCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CChoiceGreekSymbolCtrl::OnDraw - Drawing function

void CChoiceGreekSymbolCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
}



// CChoiceGreekSymbolCtrl::DoPropExchange - Persistence support

void CChoiceGreekSymbolCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CChoiceGreekSymbolCtrl::OnResetState - Reset control to default state

void CChoiceGreekSymbolCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	init_default( );
}



// CChoiceGreekSymbolCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CChoiceGreekSymbolCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("COMBOBOX");
	cs.style |= ( WS_CHILD| WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | CBS_OWNERDRAWFIXED );
	return COleControl::PreCreateWindow(cs);
}



// CChoiceGreekSymbolCtrl::IsSubclassedControl - This is a subclassed control

BOOL CChoiceGreekSymbolCtrl::IsSubclassedControl()
{
	return TRUE;
}


// CChoiceGreekSymbolCtrl::OnOcmCommand - Handle command messages

LRESULT CChoiceGreekSymbolCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.
	switch(wNotifyCode)
	{
	    case CBN_SELCHANGE: 
			{
				if( GetSafeHwnd( ) )
				{
					CComboBox *pCombo = (CComboBox *) this;
					m_lActiveItem = pCombo->GetCurSel( );
                    // fire event
					OnActiveItemChange( m_lActiveItem );    // if have smthg to select
				}
			}
			break;
		case CBN_DROPDOWN: 
			{
				CRect	rect, rcBounds;
				GetWindowRect( &rect );
				GetClientRect( &rcBounds );
				
				{
					int dy = 0;

					CComboBox *combo = (CComboBox *)this;

					int nSz = combo->GetCount();
					int nHeight = combo->GetItemHeight( nSz > 0 ? 0 : -1 );

					if( nSz > 10 )
						nSz = 10;

					dy = nHeight * nSz;
					dy += 2 * ::GetSystemMetrics( SM_CXEDGE );

					RECT rc_combo = rcBounds;
					ClientToScreen( &rc_combo );
					RECT rc_workarea = {0};
					::SystemParametersInfo( SPI_GETWORKAREA, 0, &rc_workarea, 0);
					dy = min( max( dy, nHeight ), rc_workarea.bottom - rc_combo.bottom );
                    rect.bottom += dy;
				}

				::SetWindowPos( GetSafeHwnd(), 0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER );
			}
			break;
		case CBN_CLOSEUP: 
			Refresh();
			break;

	}

	return 0;
}



// CChoiceGreekSymbolCtrl message handlers

LONG CChoiceGreekSymbolCtrl::GetFirstChar(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
    return m_lFirstChar;
}

void CChoiceGreekSymbolCtrl::SetFirstChar(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	m_lFirstChar = newVal;
	refresh_content( );

	SetModifiedFlag();
}

LONG CChoiceGreekSymbolCtrl::GetLastChar(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return m_lLastChar;
}

void CChoiceGreekSymbolCtrl::SetLastChar(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	m_lLastChar = newVal;
	refresh_content( );

	SetModifiedFlag();
}

LONG CChoiceGreekSymbolCtrl::GetActiveItem(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return m_lActiveItem;
}

void CChoiceGreekSymbolCtrl::SetActiveItem(LONG lIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
	if( GetSafeHwnd( ) )
	{
		CComboBox *pCombo = (CComboBox *) this;
		m_lActiveItem = pCombo->SetCurSel( lIdx );
	}
	else
		m_lActiveItem = -1;
	
	SetModifiedFlag();
}

LONG CChoiceGreekSymbolCtrl::GetItemCount(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	
	if( !GetSafeHwnd( ) )
		return 0;

	CComboBox *pCombo = (CComboBox *) this;
	return (long)pCombo->GetCount();
}

LONG CChoiceGreekSymbolCtrl::GetChar(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	TPOS lpos = 0;
	for( int nindex = nIdx; nindex >= 0; nindex -- )
		lpos = lpos ? m_symbols.next( lpos ) : m_symbols.head( );		
	
	if( lpos )
		return m_symbols.get( lpos );

	return 0;
}

void CChoiceGreekSymbolCtrl::OnFontChanged()
{
   
	COleControl::OnFontChanged();

	CFontHolder& font = InternalGetFont();

	if (font.m_pFont != NULL)
	{
		HFONT hFont = font.GetFontHandle();
		if( hFont )
		{
			CFont* pFont = CFont::FromHandle( hFont );
			if( pFont )
			{
				LOGFONT lf;
				::ZeroMemory( &lf, sizeof(LOGFONT) );
				pFont->GetLogFont( &lf );
				
				if( GetSafeHwnd() )
				{
					TEXTMETRIC tm;
					font.QueryTextMetrics( &tm );
					SendMessage( CB_SETITEMHEIGHT, 0, tm.tmHeight + tm.tmExternalLeading + 1 );
					SendMessage( CB_SETITEMHEIGHT, -1, tm.tmHeight + tm.tmExternalLeading + 1 );

					// [vanek] : т.к. количество поддерживаемых символов может отличаться  - 09.06.2004
					refresh_content( );
				}
			}
		}
	} 

	
}

int CChoiceGreekSymbolCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	refresh_content( );
	return 0;
}

void CChoiceGreekSymbolCtrl::DrawItem(  LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	CFontHolder& font = InternalGetFont();
    if ( !font.m_pFont )
		return;

	HFONT hFont = font.GetFontHandle();
	if( !hFont )
		return;
	
	LOGFONT lf = {0};
	if( !::GetObject( hFont, sizeof(LOGFONT), &lf ) )
		return;
	
	lf.lfCharSet = GREEK_CHARSET;
	HFONT hTempFont = ::CreateFontIndirect( &lf );
	if( !hTempFont )
		return;

	COLORREF	clrBack = 0, clrFore = 0;

	clrFore = SetTextColor(lpDrawItemStruct->hDC, 
                lpDrawItemStruct->itemState & ODS_SELECTED ? GetSysColor( COLOR_HIGHLIGHTTEXT ) :
				TranslateColor( GetForeColor( ) ) ); 
 
    clrBack = SetBkColor(lpDrawItemStruct->hDC, lpDrawItemStruct->itemState & ODS_SELECTED ?
                GetSysColor( COLOR_HIGHLIGHT ) : TranslateColor( GetBackColor() ) ); 

	hTempFont = (HFONT) ::SelectObject( lpDrawItemStruct->hDC, hTempFont );
	char str[2] = {0};
	
	// Get symbol
	str[0] = (int)(GetChar(lpDrawItemStruct->itemID));

	// Calculate the vertical and horizontal position. 
	TEXTMETRIC tm = {0};
    GetTextMetrics(lpDrawItemStruct->hDC, &tm); 
    int y = (lpDrawItemStruct->rcItem.bottom + lpDrawItemStruct->rcItem.top - tm.tmHeight) / 2,
		x = LOWORD(GetDialogBaseUnits()) / 4; 
 

	ExtTextOut( lpDrawItemStruct->hDC, 2 * x, y, ETO_CLIPPED | ETO_OPAQUE, &lpDrawItemStruct->rcItem,
				str, lstrlen( str ), 0 );

	::DeleteObject( ::SelectObject( lpDrawItemStruct->hDC, hTempFont ) );
	hTempFont = 0;

	::SetTextColor( lpDrawItemStruct->hDC, clrBack );
	::SetTextColor( lpDrawItemStruct->hDC, clrFore );

	// If the item has the focus, draw focus rectangle. 
     if (lpDrawItemStruct->itemState & ODS_FOCUS) 
       DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem); 
}

void CChoiceGreekSymbolCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	::SendMessage(GetSafeHwnd(),CB_SHOWDROPDOWN,(WPARAM)true,NULL);
}

HBRUSH CChoiceGreekSymbolCtrl::CtlColor ( CDC* pDC, UINT nCtlColor )
{
    return get_bk_brush( );
}

HBRUSH CChoiceGreekSymbolCtrl::OnCtlColor ( CDC* pDC, CWnd *pwnd, UINT nCtlColor )
{
    return get_bk_brush( );
}

void CChoiceGreekSymbolCtrl::OnBackColorChanged()
{
	// TODO: Add your specialized code here and/or call the base class
	COleControl::OnBackColorChanged();
	create_bk_brush( );
}


////////////// CChoiseGreekSymbol 
void	CChoiceGreekSymbolCtrl::refresh_content( )
{
    if( !GetSafeHwnd() )
		return;

	CComboBox *pCombo = (CComboBox*)this;

	if( pCombo->GetCount( ) > 0 )
		pCombo->ResetContent( );

	create_smb_list( );
	long lSize = m_symbols.count();
	if( lSize < 0 )
		lSize = -lSize;

	for(long l = 0; l < lSize; l++)
		pCombo->AddString( _T("") );

	pCombo->SetCurSel( m_lActiveItem );
}

void CChoiceGreekSymbolCtrl::Serialize(CArchive& ar)
{
	if( ar.IsStoring( ) )
	{	// storing
		int iVer = 1;
		ar << iVer;
		ar << m_lFirstChar;
		ar << m_lLastChar;
		ar << m_lActiveItem;
	}
	else
	{	// loading
		int iVer = 0;
		ar >> iVer;
		ar >> m_lFirstChar;
		ar >> m_lLastChar;
		ar >> m_lActiveItem;

		refresh_content( );
	}

	COleControl::Serialize(ar);
}

void	CChoiceGreekSymbolCtrl::init_default()
{
	// Font
	CFontHolder &font = InternalGetFont( );
	
	FONTDESC fd = {0};
	LOGFONT lf = {0};

	fd.cbSizeofstruct	= sizeof( fd );

	HFONT hFont = (HFONT) ::GetStockObject( ANSI_VAR_FONT );
    if( hFont && ::GetObject( hFont, sizeof( lf ), &lf ) )
	{
		CString strFontName = lf.lfFaceName;
        fd.lpstrName		= strFontName.AllocSysString( ); 			
		fd.cySize.Lo		= (unsigned long)(lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight )
								* 10000L * 72L / 96L;
		fd.cySize.Hi		= 0;
        fd.sWeight			= (SHORT)lf.lfWeight;
		fd.sCharset			= (SHORT)lf.lfCharSet;
		fd.fItalic			= (BOOL)lf.lfItalic;
		fd.fUnderline		= (BOOL)lf.lfUnderline;
		fd.fStrikethrough	= (BOOL)lf.lfStrikeOut;
    }
	else
	{
		fd.cbSizeofstruct = sizeof( FONTDESC );
		fd.lpstrName = OLESTR("Arial");
		fd.cySize.Lo = 10000;
		fd.cySize.Hi = 0;
		fd.sWeight = FW_THIN;
		fd.sCharset = DEFAULT_CHARSET;
		fd.fItalic	= FALSE;
		fd.fUnderline = FALSE;
		fd.fStrikethrough = FALSE; 
	}

    font.ReleaseFont( );
	font.InitializeFont( &fd );
}

void	CChoiceGreekSymbolCtrl::create_smb_list( )
{
	m_symbols.clear( );

	HDC hdc = 0;
	hdc = ::GetDC( GetSafeHwnd( ) );
	if( !hdc )
		return;

	// set font
	CFontHolder& font = InternalGetFont();
    if ( !font.m_pFont )
		return;

	HFONT hFont = font.GetFontHandle();
	if( !hFont )
		return;
	
	LOGFONT lf = {0};
	if( !::GetObject( hFont, sizeof(LOGFONT), &lf ) )
		return;
	
	lf.lfCharSet = GREEK_CHARSET;
	HFONT hTempFont = ::CreateFontIndirect( &lf );
	if( !hTempFont )
		return;

	hTempFont = (HFONT) ::SelectObject( hdc, hTempFont );


	bool basc = m_lFirstChar <= m_lLastChar;
	long	lcount = abs(m_lLastChar - m_lFirstChar) + 1;
	char	*psymbols = new char[ lcount ];
	
	// fill string
	for( long lsmb = m_lFirstChar, lidx = 0; lidx < lcount; lidx ++ , basc ? lsmb ++ : lsmb -- )
	    psymbols[ lidx ] = (int)lsmb;

	// get glypths
	WCHAR *pwglyphs = new WCHAR[ lcount ];
	::ZeroMemory( pwglyphs, sizeof(WCHAR) * lcount );
    
	GCP_RESULTSA	st_res = {0};
	st_res.lStructSize = sizeof( st_res );
	st_res.lpGlyphs = pwglyphs;
	st_res.nGlyphs = (UINT)lcount;


	if( 0 != ::GetCharacterPlacementA( hdc, psymbols, lcount, 0, &st_res, 0) )
	{
		for( long lidx = 0; lidx < lcount; lidx++ )
		{
		   if( pwglyphs[ lidx ] )
			   m_symbols.add_tail( psymbols[ lidx ] );
		}
	}

	if( pwglyphs )
		delete[] pwglyphs; pwglyphs = 0;
    
	if( psymbols )
		delete[] psymbols; psymbols = 0;

	::DeleteObject( ::SelectObject( hdc, hTempFont ) );
	hTempFont = 0;
}

void	CChoiceGreekSymbolCtrl::create_bk_brush( )
{
    if( m_brushBackground.GetSafeHandle() )    
		m_brushBackground.DeleteObject( );
    m_brushBackground.CreateSolidBrush( TranslateColor( GetBackColor() ) );            
}

HBRUSH	CChoiceGreekSymbolCtrl::get_bk_brush( )
{
    if( !m_brushBackground.GetSafeHandle() )
		create_bk_brush( );

	return m_brushBackground;
}
