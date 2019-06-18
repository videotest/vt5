// UserParamsListBoxCtrl.cpp : Implementation of the CUserParamsListBoxCtrl ActiveX Control class.

#include "stdafx.h"
#include <math.h>
#include "MeasCtrls.h"
#include "UserParamsListBoxCtrl.h"
#include "WorksModePropPage.h"

#define	LeftMarginExpr	5

#include <msstkppg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CUserParamsListBoxCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CUserParamsListBoxCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	//ON_WM_DRAWITEM()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CUserParamsListBoxCtrl, COleControl)
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_FORECOLOR()
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "GetItemCount", dispidGetItemCount, GetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "GetParamSection", dispidGetParamSection, GetParamSection, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "AddParam", dispidAddParam, AddParam, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "RemoveItem", dispidRemoveItem, RemoveItem, VT_BOOL, VTS_I4)
	DISP_PROPERTY_EX_ID(CUserParamsListBoxCtrl, "ActiveItem", dispidActiveItem, GetActiveItem, SetActiveItem, VT_I4)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "Refresh", dispidRefresh, Refresh, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "SetItemData", dispidSetItemData, SetItemData, VT_BOOL, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CUserParamsListBoxCtrl, "GetItemData", dispidGetItemData, GetItemData, VT_I4, VTS_I4)
	DISP_PROPERTY_EX_ID(CUserParamsListBoxCtrl, "FirstColumnsWidth", dispidFirstColumnsWidth, GetFirstColumnsWidth, SetFirstColumnsWidth, VT_I4)
	DISP_PROPERTY_EX_ID(CUserParamsListBoxCtrl, "WorksMode", dispidWorksMode, GetWorksMode, SetWorksMode, VT_I4)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CUserParamsListBoxCtrl, COleControl)
	EVENT_CUSTOM_ID("OnActiveItemChange", eventidOnActiveItemChange, OnActiveItemChange, VTS_I4)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CUserParamsListBoxCtrl, 3)
	PROPPAGEID(CWorksModePropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)
END_PROPPAGEIDS(CUserParamsListBoxCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CUserParamsListBoxCtrl, "USERPARAMSLISTBO.UserParamsListBoCtrl.1",
	0xc58b9605, 0x705f, 0x411c, 0x81, 0x8d, 0xf4, 0x1e, 0xf1, 0xa9, 0xe0, 0xa0)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CUserParamsListBoxCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DUserParamsListBox =
		{ 0x1D715E46, 0x1BC4, 0x4DCC, { 0xB7, 0x30, 0x5D, 0xD4, 0xFA, 0x89, 0xA5, 0xDC } };
const IID BASED_CODE IID_DUserParamsListBoxEvents =
		{ 0xFD25AC5A, 0xAF2F, 0x43A0, { 0xA5, 0x23, 0x5F, 0x3, 0xCD, 0x68, 0x4F, 0x6B } };



// Control type information

static const DWORD BASED_CODE _dwUserParamsListBoxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CUserParamsListBoxCtrl, IDS_USERPARAMSLISTBOX, _dwUserParamsListBoxOleMisc)



// CUserParamsListBoxCtrl::CUserParamsListBoxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CUserParamsListBoxCtrl

BOOL CUserParamsListBoxCtrl::CUserParamsListBoxCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_USERPARAMSLISTBOX,
			IDB_USERPARAMSLISTBOX,
			afxRegApartmentThreading,
			_dwUserParamsListBoxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CUserParamsListBoxCtrl::CUserParamsListBoxCtrl - Constructor

CUserParamsListBoxCtrl::CUserParamsListBoxCtrl()
{
	InitializeIIDs(&IID_DUserParamsListBox, &IID_DUserParamsListBoxEvents);
	// TODO: Initialize your control's instance data here.
	InitDefault();

	m_brBkgnd = 0;
}



// CUserParamsListBoxCtrl::~CUserParamsListBoxCtrl - Destructor

CUserParamsListBoxCtrl::~CUserParamsListBoxCtrl()
{
	// TODO: Cleanup your control's instance data here.
	if( m_brBkgnd )
	{
		::DeleteObject( m_brBkgnd );
		m_brBkgnd = 0;
	}

	Clear( );
}



// CUserParamsListBoxCtrl::OnDraw - Drawing function

void CUserParamsListBoxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
}



// CUserParamsListBoxCtrl::DoPropExchange - Persistence support

void CUserParamsListBoxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CUserParamsListBoxCtrl::OnResetState - Reset control to default state

void CUserParamsListBoxCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	InitDefault();
}



// CUserParamsListBoxCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CUserParamsListBoxCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("LISTBOX");
	cs.style |= ( WS_CHILD| WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER | LBS_OWNERDRAWVARIABLE /*LBS_OWNERDRAWFIXED*/ /*| LBS_NOINTEGRALHEIGHT*/ | LBS_NOTIFY | LBS_DISABLENOSCROLL );
	return COleControl::PreCreateWindow(cs);
}



// CUserParamsListBoxCtrl::IsSubclassedControl - This is a subclassed control

BOOL CUserParamsListBoxCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CUserParamsListBoxCtrl::OnOcmCommand - Handle command messages

LRESULT CUserParamsListBoxCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	if( wNotifyCode == LBN_SELCHANGE )
	{
		if( GetSafeHwnd( ) )
		{
			CListBox *pList = (CListBox *) this;
			m_lActiveItem = pList->GetCurSel( );
            // fire event
			OnActiveItemChange( m_lActiveItem );    // if have smthg to select
		} 
	}

	return 0;
}



// CUserParamsListBoxCtrl message handlers

LONG CUserParamsListBoxCtrl::GetItemCount(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( GetSafeHwnd( ) )
	{
		CListBox *pList = (CListBox *) this;
		return pList->GetCount( );
	}
	return 0;
}

BSTR CUserParamsListBoxCtrl::GetParamSection(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	long lPos = m_ParamList[ nIdx ];
	CDrawParam *pParam = lPos ? m_ParamList.get( lPos ) : 0;
	if( pParam )
		pParam->GetSection( &strResult );

	return strResult.AllocSysString();
}

LONG CUserParamsListBoxCtrl::AddParam(LPCTSTR lpcstrSection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !GetSafeHwnd( ) || !lpcstrSection )
		return -1;
	
	long lIdx = -1;
	if( SearchItemListBySection( lpcstrSection, &lIdx  ) )
		return lIdx;

	CDrawParam *pParam = new CDrawParam;
	pParam->SetSection( &CString(lpcstrSection) );
	pParam->SetWorksMode( m_lWorksMode );
	if( !pParam->Init( &m_mapKeyToSection ) )
	{
		delete pParam;	pParam = 0;			
		return -1;
	}
	   		
	m_ParamList.add_tail( pParam );
	CListBox *pList = (CListBox*) this;
	lIdx = pList->AddString( _T("") );
	SetItemsHeights( );
	SetHorzExtent( );

	// [vanek] - 19.03.2005
	UpdateScrollBars();

	return lIdx;
}

VARIANT_BOOL CUserParamsListBoxCtrl::RemoveItem(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lPos = m_ParamList[ nIdx ];
	if( GetSafeHwnd( ) && lPos )
	{
		CListBox *pList = (CListBox*) this;
		if( LB_ERR != pList->DeleteString( nIdx ) )
			m_ParamList.remove( lPos );
	}
	
	return VARIANT_FALSE;
}

LONG CUserParamsListBoxCtrl::GetActiveItem(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lActiveItem;
}

void CUserParamsListBoxCtrl::SetActiveItem(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( GetSafeHwnd( ) )
	{
		CListBox *pList = (CListBox *) this;
		m_lActiveItem = pList->SetCurSel( newVal );
	}
	else
		m_lActiveItem = -1;

	SetModifiedFlag();
}

void CUserParamsListBoxCtrl::Refresh(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//Clear( );
	//LoadParams( );
	ReLoadList( );
	SetActiveItem( m_lActiveItem );
}

VARIANT_BOOL CUserParamsListBoxCtrl::SetItemData(LONG nIdx, LONG lData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !GetSafeHwnd( ) )
        return VARIANT_FALSE;

	CListBox *pList = (CListBox*) this;
	return (LB_ERR != pList->SetItemData( nIdx, lData ) ) ? VARIANT_TRUE : VARIANT_FALSE;
}

LONG CUserParamsListBoxCtrl::GetItemData(LONG nIdx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !GetSafeHwnd( ) )
		return 0;

	CListBox *pList = (CListBox*) this;
	long lRes = pList->GetItemData( nIdx );
	return (lRes != LB_ERR) ? lRes : 0;
}

long	CUserParamsListBoxCtrl::LoadParams( )
{
	m_ParamList.clear( );

	// vanek - 28.03.2005
	m_mapKeyToSection.RemoveAll( );
    
	BSTR	*pbstrSections = 0;
	long	*plKeys = 0;

	long lSize = 0;
	lSize = get_sections_by_worksmode( m_lWorksMode, &pbstrSections, &plKeys );
	if( lSize > 0 )
	{
		for( long l = 0; l < lSize; l++ )
		{
			CDrawParam *pcParam = new CDrawParam;
            pcParam->SetSection( &CString( *(pbstrSections + l) ) );
			pcParam->SetWorksMode( m_lWorksMode );
			m_mapKeyToSection.SetAt( *(plKeys + l), CString( *(pbstrSections + l) ) );
			if( pcParam->Init( &m_mapKeyToSection ) )
				m_ParamList.add_tail( pcParam );
			else
			{
				m_mapKeyToSection.RemoveKey( *(plKeys + l) );
				delete pcParam;
				pcParam = 0;
			}
		}

		for( long l = 0; l < lSize; l++ )
		{
			if( *(pbstrSections + l) )
			{
				::SysFreeString( *(pbstrSections + l) );
				*(pbstrSections + l) = 0;
			}
		}
	}

	if( pbstrSections )
	{
		delete[] pbstrSections;
		pbstrSections = 0;
	}

	if( plKeys )
	{
		delete[] plKeys;
		plKeys = 0;
	}

	return m_ParamList.count( );
}

long	CUserParamsListBoxCtrl::GetMaxSecondColumnWidth( )
{
	long lMaxWidth = 0;
	CDC *pdc = 0;
	//CFont *pOldFont = 0;
	HFONT hFont = 0;
	CListBox *plist = 0;
	int	iOldMode = 0;


	if( GetSafeHwnd( ) )
	{
		plist = (CListBox*) this;
		pdc = plist->GetDC( );
		//pOldFont = SelectStockFont( pdc );
		hFont = InternalGetFont( ).GetFontHandle( );
		if( hFont )
			hFont = (HFONT) ::SelectObject( pdc->m_hDC, hFont );
		if( pdc )
			iOldMode = ::SetMapMode( pdc->m_hDC, MM_TEXT );
	}

	

	for( long lPos = m_ParamList.head( ); lPos; lPos = m_ParamList.next( lPos ) )
	{
		long lWidth = m_ParamList.get( lPos )->GetWidth( pdc ? pdc->m_hDC : 0, TRUE );
		if( lWidth > lMaxWidth )
			lMaxWidth = lWidth;
	}

	if( pdc && plist) 
	{
		::SetMapMode( pdc->m_hDC, iOldMode );
		iOldMode = 0;
		//pdc->SelectObject( pOldFont );
		//pOldFont = 0;
		if( hFont )
			::SelectObject( pdc->m_hDC, hFont );
	    hFont = 0;
		plist->ReleaseDC( pdc );
		pdc = 0;
	}

	return lMaxWidth;
}

BOOL	CUserParamsListBoxCtrl::SetHorzExtent( )
{
	if( !GetSafeHwnd( ) )
		return FALSE;

	CListBox *plist = (CListBox*) this;
	plist->SendMessage( LB_SETHORIZONTALEXTENT, (WPARAM)( m_lFirstColumnsWidth + LeftMarginExpr + GetMaxSecondColumnWidth( ) ), (LPARAM) 0 );
    return TRUE;
}

BOOL	CUserParamsListBoxCtrl::SetItemsHeights( )
{
	if( !GetSafeHwnd( ) )
		return FALSE;

	CListBox *pList = (CListBox*) this;

	CDC *pdc = pList->GetDC( );
	HFONT hFont = InternalGetFont( ).GetFontHandle( );
	if( hFont )
		hFont = (HFONT) ::SelectObject( pdc->m_hDC, hFont );

	CListBox *plist = (CListBox*) this;
	BOOL bRes = TRUE;

	int iOldMode = ::SetMapMode( pdc->m_hDC, MM_TEXT );
	for( long l = 0; l < m_ParamList.count( ); l++ )
	{
		long lHeight = 0;
		CDrawParam *pParam = 0;
		pParam = m_ParamList.get(m_ParamList[ l ]);
		if( pParam )
		{
			CString str_name;
			pParam->GetUserName( &str_name );

            // [vanek] SBT:351 measure string name - 02.06.2004			
			Graphics graphics( pdc->m_hDC );
			PointF point_origin( 0.f, 0.f);
			RectF boundRect;
			
			LOGFONT lf = {0};
			::GetObject( hFont, sizeof(lf), &lf );
			INT nFontStyle = FontStyleRegular;
			if ( lf.lfWidth == FW_BOLD ) nFontStyle |= FontStyleBold;
			if ( lf.lfItalic ) nFontStyle |= FontStyleItalic;
			if ( lf.lfUnderline ) nFontStyle |= FontStyleUnderline;
			if ( lf.lfStrikeOut ) nFontStyle |= FontStyleStrikeout;
			Gdiplus::Font font( _bstr_t(lf.lfFaceName), (REAL)(abs(lf.lfHeight) * 72.f / pdc->GetDeviceCaps(LOGPIXELSY)), 
				nFontStyle, UnitPoint );
			graphics.MeasureString( _bstr_t( str_name ), str_name.GetLength(), &font, point_origin, &boundRect );
					
			lHeight = (long)max( pParam->GetHeight( pdc->m_hDC, TRUE ) + 2, ceil(boundRect.Height) );
		}

		lHeight = plist->SendMessage( LB_SETITEMHEIGHT, (WPARAM) l, (LPARAM) lHeight );
		if( lHeight == LB_ERR  )
		{
			bRes = FALSE;
			break;
		}  
	}
	::SetMapMode( pdc->m_hDC, iOldMode );
	iOldMode = 0;

	if( hFont )
		::SelectObject( pdc->m_hDC, hFont );
	
	hFont = 0;
	pList->ReleaseDC( pdc );
	pdc = 0;
	return bRes;	
}


void CUserParamsListBoxCtrl::DrawItem(  LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !GetSafeHwnd( ) )
		return;

    switch (lpDrawItemStruct->itemAction) 
    { 
		//case ODA_FOCUS: 
        case ODA_SELECT: 
        case ODA_DRAWENTIRE:
			{
				CDrawParam *pParam = 0;
				pParam = m_ParamList.get( m_ParamList[ lpDrawItemStruct->itemID ]);
				if( !pParam )
					break;
				
				COLORREF	clrBack = 0, clrFore = 0, clrLine = 0;

				// [vanek] BT2000:4230 - 02.01.2005
				CListBox *plist = (CListBox*) this;
				CRect rcItem;
				int nx_offset = 0, 
					ny_offset = 0;
				plist->GetItemRect( lpDrawItemStruct->itemID, &rcItem );
				nx_offset = rcItem.left;
				ny_offset = rcItem.top;
				rcItem.OffsetRect( -nx_offset, -ny_offset );

				CDC cMemDC;
				CBitmap cMemBmp;
				cMemDC.CreateCompatibleDC( CDC::FromHandle(lpDrawItemStruct->hDC) );
				cMemBmp.CreateCompatibleBitmap( CDC::FromHandle(lpDrawItemStruct->hDC), rcItem.Width(), rcItem.Height());
				CBitmap* pOldBmp = cMemDC.SelectObject(&cMemBmp);

				clrFore = cMemDC.SetTextColor( lpDrawItemStruct->itemState & ODS_SELECTED ?
					GetSysColor( COLOR_HIGHLIGHTTEXT ) : TranslateColor( GetForeColor() ));

				COLORREF clBack = lpDrawItemStruct->itemState & ODS_SELECTED ?
					GetSysColor( COLOR_HIGHLIGHT ) : TranslateColor( GetBackColor() );
				clrBack = cMemDC.SetBkColor( clBack );

                HBRUSH hbrush = 0;
				hbrush = ::CreateSolidBrush( clBack );
				cMemDC.FillRect( &rcItem, CBrush::FromHandle(hbrush) ); 
				if( hbrush )
				{
					::DeleteObject( hbrush );
					hbrush = 0; 
				}
				
				int iMode = cMemDC.SetBkMode( TRANSPARENT );

				CFont *pOldFont = 0;
				HRGN hRgn = 0;

				hRgn = ::CreateRectRgn( rcItem.left,rcItem.top, rcItem.right, rcItem.bottom );
				
				pOldFont = SelectStockFont( &cMemDC ); 

				// Calculate the vertical and horizontal position. 
				TEXTMETRIC tm = {0};
				cMemDC.GetTextMetrics( &tm ); 
				int y = (rcItem.bottom + rcItem.top - tm.tmHeight) / 2,
					x = LOWORD(GetDialogBaseUnits()) / 4; 
			 
				RECT	rcFirstColumn = {	rcItem.left,
											rcItem.top,
											rcItem.left + m_lFirstColumnsWidth,
											rcItem.bottom },
						rcSecondColumn = {	rcItem.left + m_lFirstColumnsWidth + LeftMarginExpr,
											rcItem.top,
											rcItem.right,		///а может и надо getMaxwidth
											rcItem.bottom};	

				CString value;
                pParam->GetUserName( &value );
    				
				Graphics graphics( cMemDC.GetSafeHdc() );	  
				LOGFONT lf = {0};
				::GetObject( ::GetCurrentObject( cMemDC.GetSafeHdc(), OBJ_FONT ), sizeof(lf), &lf );
				INT nFontStyle = FontStyleRegular;
				if ( lf.lfWidth == FW_BOLD ) nFontStyle |= FontStyleBold;
				if ( lf.lfItalic ) nFontStyle |= FontStyleItalic;
				if ( lf.lfUnderline ) nFontStyle |= FontStyleUnderline;
				if ( lf.lfStrikeOut ) nFontStyle |= FontStyleStrikeout;
				Gdiplus::Font font( _bstr_t(lf.lfFaceName), (REAL)(abs(lf.lfHeight) * 72.f / ::GetDeviceCaps(lpDrawItemStruct->hDC, LOGPIXELSY)), nFontStyle, UnitPoint );
				StringFormat format;
				format.SetAlignment(StringAlignmentCenter);
				// [vanek] BT2000:4229 - 02.01.2005
				format.SetTrimming( StringTrimmingEllipsisWord );
				format.SetLineAlignment(StringAlignmentCenter);
				format.SetFormatFlags( StringFormatFlagsNoWrap );
				RectF layoutRect( (REAL)(rcFirstColumn.left), (REAL)(rcFirstColumn.top), (REAL)(rcFirstColumn.right - rcFirstColumn.left), 
					(REAL)(rcFirstColumn.bottom - rcFirstColumn.top) );
				Color gdipColor;
				gdipColor.SetFromCOLORREF( cMemDC.GetTextColor( ) );
				SolidBrush brush( gdipColor );
				graphics.DrawString( _bstr_t( value ), value.GetLength(), &font, layoutRect, &format, &brush );
				
				HPEN hPen = ::CreatePen( PS_SOLID, 0, cMemDC.GetTextColor( ) );
				hPen = (HPEN) cMemDC.SelectObject( hPen );
				cMemDC.MoveTo( rcItem.left + m_lFirstColumnsWidth, rcItem.top );
				cMemDC.LineTo( rcItem.left + m_lFirstColumnsWidth, rcItem.bottom );
				if( hPen )
				{
					DeleteObject( cMemDC.SelectObject( hPen ) );
					hPen = 0;	
				}

				pParam->DrawExpr( cMemDC.GetSafeHdc(), &rcSecondColumn );

				if( hRgn )
				{
					::DeleteObject( hRgn );
					hRgn = 0;
				}

				cMemDC.SelectObject( pOldFont );
				pOldFont = 0;

				cMemDC.SetBkMode( iMode );

				cMemDC.SetTextColor( clrFore );
				cMemDC.SetTextColor( clrBack );

				pParam = 0;

				rcItem.OffsetRect( nx_offset, ny_offset );
                ::BitBlt( lpDrawItemStruct->hDC, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), cMemDC.m_hDC, 0, 0, SRCCOPY);
				cMemDC.SelectObject(pOldBmp);
			}
			break;
	}
}

HBRUSH CUserParamsListBoxCtrl::CtlColor ( CDC* pDC, UINT nCtlColor )
{
	COLORREF	clrBack = TranslateColor( GetBackColor( ) ),
				clrFore = TranslateColor( GetForeColor( ) );
	SetBkColor( pDC->m_hDC, clrBack );	
	SetTextColor( pDC->m_hDC, clrFore ); 
	if( m_brBkgnd )
	{
		::DeleteObject( m_brBkgnd );
		m_brBkgnd = 0;
	}
	m_brBkgnd = ::CreateSolidBrush( clrBack );
	return m_brBkgnd; 
} 

void	CUserParamsListBoxCtrl::InitDefault()
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

	m_lFirstColumnsWidth = 10;
	m_lActiveItem = -1;
	m_lWorksMode = MIN_WORKSMODE;
}

/*void	CUserParamsListBoxCtrl::SetWorksModes( )
{
	for( long lPos = m_ParamList.head( ); lPos; lPos = m_ParamList.next( lPos ) )
		m_ParamList.get( lPos )->SetWorksMode( m_lWorksMode );

}  */

void	CUserParamsListBoxCtrl::Clear( )
{
	if( GetSafeHwnd( ) )
	{
		CListBox *pList = (CListBox*) this;
		pList->ResetContent( );
	}
	
	m_mapKeyToSection.RemoveAll( );
	m_ParamList.clear( );
}

void	CUserParamsListBoxCtrl::ReLoadList( )
{
    if( GetSafeHwnd( ) )
	{
		CListBox *plist = (CListBox*) this;
		long lCount = LoadParams();
		plist->ResetContent( );
		for( long l = 0; l < lCount; l++ )
			plist->AddString( _T("") );
		
		SetItemsHeights( );
		SetHorzExtent( );

		// [vanek] - 19.03.2005
		UpdateScrollBars();
		
		
		__super::Refresh();
	}
}

BOOL	CUserParamsListBoxCtrl::SearchItemListBySection( CString strSection, long *plIdx )
{
	if( !plIdx )
		FALSE;

    (*plIdx) = 0;
	for( long lPos = m_ParamList.head( ); lPos; lPos = m_ParamList.next( lPos ) )
	{
		CDrawParam *pParam = 0;
		pParam = m_ParamList.get( lPos );
		if( pParam )
		{
			CString strValue;
			pParam->GetSection( &strValue );
			if( strValue == strSection )
				return TRUE;	
			(*plIdx) ++;
		}
	}

	return FALSE;
}


void CUserParamsListBoxCtrl::OnFontChanged()
{
	// TODO: Add your specialized code here and/or call the base class
    
	COleControl::OnFontChanged();

	SetItemsHeights( );
	SetHorzExtent( );

	// [vanek] - 19.03.2005
	UpdateScrollBars();

	__super::Refresh();
}

int CUserParamsListBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
    ReLoadList( );
	return 0;
}

void CUserParamsListBoxCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	SetHorzExtent( );
    __super::Refresh();
}

/*BOOL CUserParamsListBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	COLORREF	clrBack = 0, clrFore = 0;

	clrBack = SetBkColor(pDC->m_hDC, TranslateColor( GetBackColor() ) );	

	return COleControl::OnEraseBkgnd(pDC);

	SetBkColor(pDC->m_hDC, clrBack );	
}  */

void CUserParamsListBoxCtrl::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int iVer = 1;
		ar << iVer;
		ar << m_lFirstColumnsWidth;
		ar << m_lActiveItem;
		ar << m_lWorksMode;
	}
	else
	{	// loading code
		int iVer = 0;
		ar >> iVer;
		ar >> m_lFirstColumnsWidth;
		ar >> m_lActiveItem;
		ar >> m_lWorksMode;

		ReLoadList( );
	}

	COleControl::Serialize( ar );
}

LONG CUserParamsListBoxCtrl::GetFirstColumnsWidth(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lFirstColumnsWidth;
}

void CUserParamsListBoxCtrl::SetFirstColumnsWidth(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lFirstColumnsWidth = newVal;
    SetHorzExtent( );

	// [vanek] - 19.03.2005
	UpdateScrollBars();

	SetModifiedFlag();
}

LONG CUserParamsListBoxCtrl::GetWorksMode(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lWorksMode;
}

void CUserParamsListBoxCtrl::SetWorksMode(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_lWorksMode != newVal )
	{
		m_lWorksMode = newVal;
		
		// [vanek] SBT:1087 - 23.07.2004
		Refresh( );
		SetModifiedFlag();		
	}
}

void CUserParamsListBoxCtrl::UpdateScrollBars()
{
	if( !GetSafeHwnd( ) )
		return;
	
	CListBox *plist = (CListBox*) this;
	CRect rc;
	plist->GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	plist->SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
}