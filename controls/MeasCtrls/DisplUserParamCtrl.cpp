// DisplUserParamCtrl.cpp : Implementation of the CDisplUserParamCtrl ActiveX Control class.
#include "stdafx.h"
#include "MeasCtrls.h"
#include "DisplUserParamCtrl.h"
#include "WorksModePropPage.h"
#include "works_modes.h"

#include <msstkppg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define ID_TIMER 234



IMPLEMENT_DYNCREATE(CDisplUserParamCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CDisplUserParamCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
//	ON_WM_KEYDOWN()
ON_WM_KEYDOWN()
ON_WM_GETDLGCODE()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CDisplUserParamCtrl, COleControl)
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FORECOLOR()
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "GetLastError", dispidGetLastError, GetLastError, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "Draw", dispidDraw, Draw, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "GetLastErrorCode", dispidGetLastErrorCode, GetLastErrorCode, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "Insert", dispidInsert, Insert,  VT_EMPTY, VTS_BSTR)
	DISP_PROPERTY_EX_ID(CDisplUserParamCtrl, "Expression", dispidExpression, GetExpression, SetExpression, VT_BSTR)
	DISP_PROPERTY_EX_ID(CDisplUserParamCtrl, "LeftMargin", dispidLeftMargin, GetLeftMargin, SetLeftMargin, VT_I4)
	DISP_PROPERTY_EX_ID(CDisplUserParamCtrl, "WorksMode", dispidWorksMode, GetWorksMode, SetWorksMode, VT_I4)
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "Refresh", dispidRefresh, Refresh, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CDisplUserParamCtrl, "Backspace", dispidDelete, DeleteActiveEntry, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CDisplUserParamCtrl, COleControl)
	EVENT_STOCK_KEYDOWN()
	EVENT_CUSTOM_ID("ContentChanged", eventidContentChanged, FireContentChanged, VTS_NONE)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CDisplUserParamCtrl, 3)
	PROPPAGEID(CWorksModePropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)
END_PROPPAGEIDS(CDisplUserParamCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDisplUserParamCtrl, "MEASCTRLS.DisplUserParamCtrl.1",
	0x9acf8f66, 0xb50f, 0x46f9, 0xaa, 0x45, 0xdb, 0x14, 0x27, 0x6e, 0xfa, 0xcc)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CDisplUserParamCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DMeasCtrls =
		{ 0xC3FAB755, 0xE612, 0x4640, { 0xA1, 0xD8, 0xA0, 0x3B, 0xDF, 0xFB, 0x6A, 0xA1 } };
const IID BASED_CODE IID_DMeasCtrlsEvents =
		{ 0xB592174D, 0xE82, 0x48DB, { 0xBB, 0xCC, 0xFD, 0x22, 0xC5, 0x49, 0x2F, 0x58 } };



// Control type information

static const DWORD BASED_CODE _dwMeasCtrlsOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDisplUserParamCtrl, IDS_MEASCTRLS, _dwMeasCtrlsOleMisc)



// CDisplUserParamCtrl::CDisplUserParamCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDisplUserParamCtrl

BOOL CDisplUserParamCtrl::CDisplUserParamCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_MEASCTRLS,
			IDB_MEASCTRLS,
			afxRegApartmentThreading,
			_dwMeasCtrlsOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CDisplUserParamCtrl::CDisplUserParamCtrl - Constructor

CDisplUserParamCtrl::CDisplUserParamCtrl()
{
	InitializeIIDs(&IID_DMeasCtrls, &IID_DMeasCtrlsEvents);
	//ICursorPosition* cp = get_cursor_interface();
	//if(cp)
	//{
	//	cp->ActivateCursor();
	//	cp->Release();

	//}
	ZeroMemory(&m_lf, sizeof(m_lf));
	
	m_sErrorDescription.Empty( );
	m_lLeftMargin = 0;
	InitDefault( );
	m_bRectsValid = FALSE;

	m_pExprRects = 0;
	m_pExprStr = 0;

	m_nRectCount = 0;
	m_nCursorPos = 0;

	m_SBVisible=false;
	width=old_width=0;
	m_SBPosition=0;
	m_SBMin=m_SBMax=0;
	m_nCursorPos = 0;
	m_bCursor = FALSE;


}



// CDisplUserParamCtrl::~CDisplUserParamCtrl - Destructor

CDisplUserParamCtrl::~CDisplUserParamCtrl()
{
	// TODO: Cleanup your control's instance data here.
	m_sErrorDescription.Empty( );
	if(m_pExprRects) delete[] m_pExprRects;
	if(m_pExprStr)
	{
		for(int i=0;i<m_nRectCount; i++)
		{
			delete[] m_pExprStr[i];
		}
		delete[] m_pExprStr; 
	}
	/*if(	m_iDrawPtr )
		m_iDrawPtr = 0;*/
}



// CDisplUserParamCtrl::OnDraw - Drawing function

void CDisplUserParamCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{


    if( ( GetSafeHwnd( ) != NULL ) /*&& (m_iDrawPtr != 0 )*/ )
	{
		CFont *pOldFont = 0;
		pOldFont = SelectStockFont( pdc );
		HBRUSH hbrush = 0;
		hbrush = ::CreateSolidBrush( TranslateColor( GetBackColor( ) ) );
		::FillRect( pdc->m_hDC, rcBounds, hbrush ); 
		::DeleteObject( hbrush );
		hbrush = 0;
		width=m_drawParam.GetWidth(pdc->m_hDC,!m_bRectsValid);
		width+=rcBounds.Width()/20;
		if(width>rcBounds.Width() && !m_SBVisible)
		{
			m_ScrollBar.ShowScrollBar(TRUE);
			m_SBVisible=true;
		}
		else
			if(width<=rcBounds.Width() && m_SBVisible)
			{
				m_ScrollBar.ShowScrollBar(FALSE);
				m_SBVisible=false;
				width=old_width=0;
				m_SBPosition=0;
				m_ScrollBar.SetScrollPos(m_SBMin);
			}

			if(width!=old_width && m_SBVisible)
			{

				m_ScrollBar.SetScrollRange(0,width/*-rcBounds.Width()*/);
				old_width=width;
				m_ScrollBar.GetScrollRange(&m_SBMin,&m_SBMax);

				SCROLLINFO si ={0};
				si.cbSize = sizeof(si);
				si.fMask = SIF_PAGE;
				float w = width;
				float th = m_SBMax - m_SBMin;

				si.nPage = th*rcBounds.Width()/w;
				m_ScrollBar.SetScrollInfo(&si, FALSE);
			}
		
        if( m_drawParam.Init( ) )
		{
			m_drawParam.SetBackColor( TranslateColor( GetBackColor( ) ) );
			m_drawParam.SetForeColor( TranslateColor( GetForeColor( ) ) );

			CRect rc;
			m_ScrollBar.GetClientRect(rc);
			int del;
			if(m_SBVisible)
				del=rc.Height();
			else
				del=0;
			RECT rtDraw = { rcBounds.left + m_lLeftMargin-m_SBPosition, rcBounds.top, 
							rcBounds.right-m_SBPosition, rcBounds.bottom-del };

			m_drawParam.DrawExpr( pdc->m_hDC, &rtDraw );
			m_sErrorDescription = m_drawParam.m_sErrorDescription;

			if(!m_bRectsValid)
			{
				ICursorPosition* cp = get_cursor_interface();
				cp->ActivateCursor();
				cp->GetCursorCoordinates(pdc->m_hDC,&m_rectCursor);

				if(IsRectEmpty(&m_rectCursor))
				{
					cp->MoveCursor(pdc->m_hDC, mdEnd, &m_rectCursor);
				}
				
				m_bRectsValid = TRUE;
				SetFocus();
				m_bFocused = TRUE;
				cp->Release();
			}


		}
			if( pOldFont )
			{
				pdc->SelectObject( pOldFont );
				pOldFont = 0;
			}
		}		
	else
	{
		pdc->Rectangle( rcBounds );
		pdc->MoveTo( rcBounds.left, rcBounds.top );
		pdc->LineTo( rcBounds.right, rcBounds.bottom );
		pdc->MoveTo( rcBounds.left, rcBounds.bottom );
		pdc->LineTo( rcBounds.right, rcBounds.top );
	}
}



// CDisplUserParamCtrl::DoPropExchange - Persistence support

void CDisplUserParamCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CDisplUserParamCtrl::OnResetState - Reset control to default state

void CDisplUserParamCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	InitDefault( );
}

BOOL CDisplUserParamCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CHILD| WS_VISIBLE |WS_BORDER;
	return COleControl::PreCreateWindow(cs);
}


void	CDisplUserParamCtrl::InitDefault()
{
	m_bRectsValid = FALSE;
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

	m_lLeftMargin = 5;

	//m_lWorksMode = MIN_WORKSMODE; 
	m_drawParam.SetWorksMode( MIN_WORKSMODE );
}


// CDisplUserParamCtrl message handlers

BSTR CDisplUserParamCtrl::GetLastError(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return m_drawParam.m_sErrorDescription.AllocSysString( );
}

void CDisplUserParamCtrl::Draw(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	InvalidateControl( );

	return;
}

BSTR CDisplUserParamCtrl::GetExpression(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//CString strResult;

	// TODO: Add your dispatch handler code here

	//return strResult.AllocSysString();
/*	BSTR bstr;
	if( m_iDrawPtr )
		m_iDrawPtr->GetString( &bstr );    
	else
		bstr = _bstr_t( "" ).copy();*/

	CString expr;
	m_drawParam.GetExpr( &expr );
  
	//return bstr;
	return expr.AllocSysString();
}

void CDisplUserParamCtrl::SetExpression(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	set_expression(CString(newVal));
	
	ICursorPosition* cp = get_cursor_interface();
	if(cp)
	{
		CDC* dc = GetDC();
		CFont* old = SelectStockFont(dc);
		m_cursor.Hide(this->m_hWnd,m_rectCursor);
		cp->MoveCursor(dc->m_hDC, mdEnd, &m_rectCursor);
		
		cp->Release();
		dc->SelectObject(old);
		ReleaseDC(dc);
	}
	adjust_scrollbar();

}

void CDisplUserParamCtrl::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int iVer = 1;
		ar << iVer;
		BSTR bstr_expr = GetExpression( );
		ar << CString( bstr_expr );
		if( bstr_expr )
		{
			::SysFreeString( bstr_expr );
			bstr_expr = 0;
		}
		ar << m_lLeftMargin;
		
		//ar << m_lWorksMode;
		long lWorksMode = m_drawParam.GetWorksMode( );
		ar << lWorksMode;
	}
	else
	{	// loading code
		int iVer = 0;
		ar >> iVer;
		CString sExpr;
		ar >> sExpr;
		ar >> m_lLeftMargin;
		//ar >> m_lWorksMode;
		long lWorksMode = 0;
		ar >> lWorksMode;
		m_drawParam.SetWorksMode( lWorksMode );
		SetExpression( sExpr );
	}

	COleControl::Serialize(ar);
}

LONG CDisplUserParamCtrl::GetLeftMargin(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lLeftMargin;
}

void CDisplUserParamCtrl::SetLeftMargin(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here
    m_lLeftMargin = newVal;
	SetModifiedFlag();
}

LONG CDisplUserParamCtrl::GetWorksMode(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//return m_lWorksMode;
	return m_drawParam.GetWorksMode( );
}

void CDisplUserParamCtrl::SetWorksMode(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lOldMode = m_drawParam.GetWorksMode( );
	if( IS_WORKSMODE( newVal ) && ( newVal != lOldMode ) )
	{
		//m_lWorksMode = newVal;
		m_drawParam.SetWorksMode( newVal );
		m_drawParam.ReInit( );
		SetModifiedFlag( );
	}
}

void CDisplUserParamCtrl::Refresh(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// vanek : refresh expression parameters infos - 29.03.2005
	m_drawParam.ReInit( );
	m_bRectsValid = FALSE;
    InvalidateControl( );
}

int CDisplUserParamCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(rc);
	VERIFY(m_ScrollBar.Create(
		SBS_HORZ | SBS_TOPALIGN | WS_CHILD, CRect(0, rc.bottom-15, rc.Width(), rc.bottom+1), this, 100));

	m_ScrollBar.ShowScrollBar(FALSE);

	m_timer = SetTimer(ID_TIMER, 500, 0);

	return 0;
}

void CDisplUserParamCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int old_sbPos = m_SBPosition;
	if(pScrollBar->GetSafeHwnd()==m_ScrollBar.GetSafeHwnd())
	{
		switch(nSBCode)
		{
			case SB_LINERIGHT:
				{
					m_SBPosition++;
					if(m_SBPosition>m_SBMax)
						m_SBPosition=m_SBMax;
					m_ScrollBar.SetScrollPos(m_SBPosition);
				}
				break;
			case SB_LINELEFT:
				{
					m_SBPosition--;
					if(m_SBPosition<m_SBMin)
						m_SBPosition=m_SBMin;
					m_ScrollBar.SetScrollPos(m_SBPosition);
				}
				break;
			case SB_THUMBPOSITION:case SB_THUMBTRACK:
				{
					m_SBPosition=nPos;
					m_ScrollBar.SetScrollPos(m_SBPosition);
				}
	
		}
		int offset = old_sbPos -m_SBPosition;
		

		OffsetRect(&m_rectCursor, offset, 0);
		Invalidate();
	}
	COleControl::OnHScroll(nSBCode, nPos, pScrollBar);
}
int CDisplUserParamCtrl::GetLastErrorCode(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_drawParam.m_nErrorCode;
}




ICursorPosition* CDisplUserParamCtrl::get_cursor_interface()
{
	return m_drawParam.GetCursorInterface();
}

void CDisplUserParamCtrl::Insert(LPCTSTR str)
{

	ICursorPosition* cp = get_cursor_interface();
	if(cp)
	{
		BSTR s1,s2;
		cp->GetDividedStrings(&s1,&s2);
		CString left(s1), right(s2);
		left+= CString(str) + right;

		m_bRectsValid =FALSE;
		set_expression(left);

		CDC* dc = GetDC();
		CFont* old = SelectStockFont(dc);
		m_cursor.Hide(m_hWnd, m_rectCursor);
		cp->MoveCursor(dc->m_hDC, mdRight, &m_rectCursor); 
		dc->SelectObject(old);


		SysFreeString(s1);
		SysFreeString(s2);

		if(!strcmp(str,"^2"))
			cp->MoveCursor(dc->m_hDC, mdRight, &m_rectCursor); 

		ReleaseDC(dc);
		cp->Release();
	}

	adjust_scrollbar();
}

void CDisplUserParamCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==ID_TIMER && m_bRectsValid && m_bFocused)
	{
		CDC* dc = GetDC();
		/*check_cursor_pos();*/
		RECT rect = m_rectCursor;
		rect.left = m_rectCursor.right;
		rect.right = m_rectCursor.right;
		if(m_bCursor) m_cursor.Draw(dc->m_hDC, rect);
		else m_cursor.Hide(m_hWnd, rect);
		ReleaseDC(dc);
		m_bCursor = !m_bCursor;
	}

	COleControl::OnTimer(nIDEvent);
}

void CBlinkingCursor::SetBkColor(COLORREF clr)
{
	m_clrBack = clr;
}
void CBlinkingCursor::SetForeColor(COLORREF clr)
{
	m_clrFore = clr;
}

void CBlinkingCursor::Draw(HDC dc, RECT rect)
{
	//Hide(dc,rect);
	CPen pen(PS_SOLID,1,m_clrFore);
	HGDIOBJ oldPen = SelectObject(dc,pen);
	int bkMode = SetBkMode(dc, TRANSPARENT);
	int rop2 = SetROP2(dc,R2_NOT);   
	::MoveToEx(dc, rect.right, rect.top,0);
	LineTo(dc, rect.right, rect.bottom);
	SelectObject(dc,oldPen);
	SetROP2(dc,rop2); 
		/*SetBkMode(dc, bkMode);*/
}

void  CBlinkingCursor::Hide(HWND hwnd, RECT rect)
{
	//HDC dc =GetDC(hwnd);
	/*FillRect(dc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));*/
	rect.left = rect.right-1;
	rect.right++;
	InvalidateRect(hwnd, &rect, FALSE);

}

void CDisplUserParamCtrl::OnDestroy()
{
	if(m_timer) KillTimer(m_timer);
	COleControl::OnDestroy();	
}

void CDisplUserParamCtrl::OnKillFocus(CWnd* pNewWnd)
{
	COleControl::OnKillFocus(pNewWnd);
	if(!m_bRectsValid) return;
	m_bFocused = FALSE;
	CDC* dc = GetDC();

	RECT rect = m_rectCursor;
	m_cursor.Hide(m_hWnd, rect);
	ReleaseDC(dc);
}

void CDisplUserParamCtrl::OnSetFocus(CWnd* pOldWnd)
{
	COleControl::OnSetFocus(pOldWnd);
	if(!m_bRectsValid) return;
	m_bFocused = TRUE;
	CDC* dc = GetDC();

	RECT rect = m_rectCursor;
	m_cursor.Draw(dc->m_hDC, rect);
	ReleaseDC(dc);
}

void CDisplUserParamCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	

	if(m_bRectsValid)
	{
		BOOL needRedraw = TRUE;
		int oldPos = m_nCursorPos;
		RECT oldRect = m_rectCursor;
		ICursorPosition* cp = get_cursor_interface();
		MovingDirection dir = mdNone;
		switch(nChar)
		{
		case VK_LEFT:
			dir = mdLeft;
			break;
		case VK_RIGHT:
			dir = mdRight;
			break;
		case VK_DELETE:
			{
				CDC* dc = GetDC();
				CFont *pOldFont = 0;
				pOldFont = SelectStockFont( dc );
				RECT old = m_rectCursor;
				cp->MoveCursor(dc->m_hDC, mdRight, &m_rectCursor);
				dc->SelectObject(pOldFont);
				ReleaseDC(dc);
				if(old.right == m_rectCursor.right) return;
				delete_entry();
			}
			break;
		case VK_BACK:
			delete_entry();
			break;
		case VK_END: 
			dir = mdEnd;
			break;
		case VK_HOME:
			dir = mdBegin;
			break;
		}
		CDC* dc = GetDC();
		CFont *pOldFont = 0;
		pOldFont = SelectStockFont( dc );

		if(dir!=mdNone) 
			cp->MoveCursor(dc->m_hDC, dir, &m_rectCursor);
		adjust_scrollbar();
		if(needRedraw)
		{
			m_cursor.Hide(m_hWnd, oldRect);
			m_cursor.Draw(dc->m_hDC, m_rectCursor);

		}
		dc->SelectObject(pOldFont);
		ReleaseDC(dc);
		cp->Release();
	}

	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CDisplUserParamCtrl::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;//order all keyboard input
}



// удаление элемента выражения 
void CDisplUserParamCtrl::delete_entry()
{

	ICursorPosition* cp = get_cursor_interface();
	BSTR bstr;
	cp->DeleteEntry(&bstr);
	set_expression(CString(bstr));

	SysFreeString(bstr);
	adjust_scrollbar();
}

void CDisplUserParamCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	

	COleControl::OnLButtonDown(nFlags, point);
	m_bFocused = TRUE;
	SetFocus();
}
#undef ID_TIMER
void CDisplUserParamCtrl::OnFontChanged()
{
	// TODO: Add your specialized code here and/or call the base class

	COleControl::OnFontChanged();
	LPFONTDISP fd = GetFont();
	IFontPtr font(fd);
	fd->Release();
	HFONT hf;
	font->get_hFont(&hf);
	::GetObject( hf, sizeof(m_lf), &m_lf );
}

void CDisplUserParamCtrl::set_expression(CString& str)
{
	CString stOldExpr( _T("") );
	m_drawParam.GetExpr( &stOldExpr ); 
	if( stOldExpr != str )
	{
		m_drawParam.SetExpr( &str );
		m_bRectsValid = FALSE;

		SetModifiedFlag();
		FireContentChanged();
		RedrawWindow();
		/*adjust_scrollbar();*/
	}
}

void CDisplUserParamCtrl::adjust_scrollbar(void)
{
	RECT cr;
	GetClientRect(&cr);



	DWORD wp; 
	if(cr.right< m_rectCursor.right)
		wp = MAKEWPARAM(SB_THUMBPOSITION, m_SBPosition + m_rectCursor.right - cr.right+10 );
	else if(m_rectCursor.right<=0) 
		wp = MAKEWPARAM(SB_THUMBPOSITION, m_SBPosition + m_rectCursor.right-10);
	else return;
	SendMessage(WM_HSCROLL, wp,(LPARAM)m_ScrollBar.m_hWnd);
}

void CDisplUserParamCtrl::DeleteActiveEntry()
{
	delete_entry();
}
