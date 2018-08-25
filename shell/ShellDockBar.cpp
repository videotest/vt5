
// ShellDockBar.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "ShellDockBar.h"
#include "OutlookBar.h"
#include <afxtempl.h>
#include <math.h>
#include "mainfrm.h"

// [vanek] : register message WM_SETCANCLOSEWND - 22.10.2004
UINT WM_SETCANCLOSEWND = RegisterWindowMessage("SET_CAN_CLOSE_WINDOW");

//__declspec(dllimport) GLOBAL_DATA *pGlobalData;

IMPLEMENT_DYNAMIC(CShellDockBar, classDockBarBase);
/////////////////////////////////////////////////////////////////////////////
// CShellDockBar

#pragma optimize("", off)
CShellDockBar::CShellDockBar( IUnknown *punkBar )
{
	EnableAutomation();


	m_cxLeftBorder = m_cxRightBorder = 1;
	m_cxDefaultGap = 1;
	m_cyTopBorder = m_cyBottomBorder = 1;


	//m_cxEdge = 2;
    //m_cyGripper = 2;


	m_bStoreVisibleState = true;
	m_sptrDockClient = punkBar;
	m_sptrWindow = punkBar;
	m_sptrName = punkBar;

	m_hwndChild = 0;

	m_bChildFrameBar = FALSE;

	BOOL bFixed = false;
	if( m_sptrDockClient  )
		m_sptrDockClient->GetFixed(&bFixed);
	//if( bFixed )
	//{
	//	m_szMaxT = m_szMinT = m_szMin= CalcFrameSizeForFixedBar();
	//}
	//else
	//{
	//	m_szMinT = m_szMin= CSize(100, 100);
	//	m_szMaxT = CSize( 400, 400 );
	//}

	m_bcan_close = true;
	m_nEnableDock = -1;
}

void CShellDockBar::CommitSize( CSize size )
{
	if( IsFixed() )
		return;

	//if( IsHorzDocked() )
	//	m_szHorz = size;
	//else
	//	m_szVert = size;

}

CShellDockBar::~CShellDockBar()
{
}

void CShellDockBar::UpdateFrameTitle()
{
	CString strName = GetName( true );
	SetWindowText( strName );
	CWnd	*pwndParent = GetParent();

	if( pwndParent->GetSafeHwnd() )
	{
		DWORD	dwStyle = GetWindowLong( pwndParent->GetSafeHwnd(), GWL_STYLE );
		if( dwStyle & WS_POPUP )pwndParent->SetWindowText( strName );
	}
}

void CShellDockBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwStyle)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	UpdateFrameTitle();

	if( dwStyle & CBRS_FLOATING ) // AAM: было наоборот - поэтому, если выт€гивали бар, чтобы он плавал - он плавал с высотой, как у придоченного
		return;

	if( !m_pDockBar )
		return;
	if( m_pDockBar->FindBar(this) == -1 )
		return;
	
	//classBarArray arrSCBars;
	//GetRowSizingBars(arrSCBars);

	arrSCBars.Add( this );

	//BOOL bHorz = IsHorzDocked();
	//CSize sizeNew = (bHorz ? m_szHorz : m_szVert);
	
	{
		int	nNewSize = 0;
		//for (int i = 0; i < arrSCBars.GetSize(); i++)
		//{
		//	classDockBarBaseRoot* pBar = arrSCBars[i];
		//	(bHorz ? pBar->m_szHorz.cy : pBar->m_szVert.cx) =
  //              bHorz ? sizeNew.cy : sizeNew.cx;
		//	CSize	size = pBar->CalcDynamicLayout( bHorz ? sizeNew.cy:sizeNew.cx, bHorz ? LM_LENGTHY:0 );
		//	nNewSize = max( bHorz?size.cy:size.cx, nNewSize );
		//}
		//for ( i = 0; i < arrSCBars.GetSize(); i++)
		//{
		//	classDockBarBaseRoot * pBar = arrSCBars[i];
		//	(bHorz ? pBar->m_szHorz.cy : pBar->m_szVert.cx) = nNewSize;
		//}
	}

	
}


void CShellDockBar::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	classDockBarBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CShellDockBar, classDockBarBase)
	//{{AFX_MSG_MAP(CShellDockBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_NCRBUTTONUP()
	ON_REGISTERED_MESSAGE(WM_SETCANCLOSEWND,OnSetCanCloseWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CShellDockBar, classDockBarBase)
//{{AFX_DISPATCH_MAP(CShellDockBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IShellDockBar to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {265274C4-1EFD-11D3-A5D3-0000B493FF1B}
static const IID IID_IShellDockBar =
{ 0x265274c4, 0x1efd, 0x11d3, { 0xa5, 0xd3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CShellDockBar, classDockBarBase)
	INTERFACE_PART(CShellDockBar, IID_IShellDockBar, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellDockBar message handlers

class CDockBarAccessHelper : public CDockBar
{
public:
	void SetLayoutRect( CRect rect )
	{	m_rectLayout = rect;}
};
int CShellDockBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	if (classDockBarBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// SBT1348. Individual siges for each dockbar.
	_bstr_t bstrName;
	if (m_sptrName != 0)
		m_sptrName->GetName(bstrName.GetAddress());
	if (bstrName.length() > 0)
	{
		CString sSection(_T("\\MainFrame\\DockbarMinSizes"));
		CString s = GetValueString(::GetAppUnknown(), sSection, (LPCTSTR)bstrName, _T("100,100"));
		int cx,cy;
		if (!s.IsEmpty() && _stscanf(s, _T("%d,%d"), &cx, &cy) == 2)
			m_szMin = CSize(cx,cy);
	}

	m_dwSCBStyle = SCBS_SHOWEDGES;
	
	if( (m_sptrWindow != NULL) )
	{
		CRect	rc;
		
		GetClientRect( rc );

		m_sptrWindow->CreateWnd( GetSafeHwnd(), rc, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 999 );
		m_sptrWindow->GetHandle( (HANDLE*)&m_hwndChild );
	}

	//if( m_sptrDockClient != NULL )
	//	m_sptrDockClient->GetSize( &m_sizeDefault, false );

	m_sizeDefault= CalcFrameSizeForFixedBar();
	m_bAllowSizing = !IsFixed();

	m_dwStyle &= ~(CBRS_SIZE_DYNAMIC|CBRS_SIZE_FIXED);
/*	if( !m_bAllowSizing )
	{
		m_cyGripper = 0;
		m_dwStyle|=CBRS_SIZE_FIXED;
	}
	else
	{
		m_dwStyle|=CBRS_SIZE_DYNAMIC;
	}*/


	SetWindowPos( 0, 0, 0, m_sizeDefault.cx, m_sizeDefault.cy, SWP_NOZORDER|SWP_NOMOVE );

	return 0;
}

void CShellDockBar::InitDockSizes()
{
	if( m_pDockBar )
	{
		CDockBarAccessHelper * pDockBar = (CDockBarAccessHelper *) m_pDockBar;
		CSize	size = GetDefSize();
		//pDockBar->SetLayoutRect( CRect( CPoint( 0, 0 ), GetDefSize() ) );
	}
}


void CShellDockBar::OnSize(UINT nType, int cx, int cy) 
{
	classDockBarBase::OnSize(nType, cx, cy);
	
	if( m_hwndChild != NULL )
	{
		CRect	rc;
		
		GetClientRect( rc );

		if( m_hwndChild  )
		{
			if( IsFixed() )
			{
				CSize sizeFixed( 100, 100 );
				if( m_sptrDockClient!=0 )
					m_sptrDockClient->GetSize( &sizeFixed, FALSE );

				rc.right = max( rc.left+sizeFixed.cx, rc.right );
				rc.bottom = max( rc.top+sizeFixed.cy, rc.bottom );
			}

			CWnd	*pwnd = CWnd::FromHandle( m_hwndChild );
			if (pwnd)
			{

				pwnd->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);//|SWP_FRAMECHANGED|SWP_SHOWWINDOW);
//				pwnd->MoveWindow( rc );

			}
		}
	}
}

bool CShellDockBar::IsFixed()
{
	if( m_sptrDockClient == 0 )
		return false;
	BOOL	bFixed = false;
	m_sptrDockClient->GetFixed( &bFixed );
	return bFixed != 0;
}


DWORD	CShellDockBar::GetDefDockSide()
{
	DWORD	dwSide = AFX_IDW_DOCKBAR_BOTTOM;
	if( m_sptrDockClient != NULL )
		m_sptrDockClient->GetDockSide( &dwSide );

	return dwSide;
}

CSize	CShellDockBar::GetDefSize()
{
	DWORD	dwSide = AFX_IDW_DOCKBAR_BOTTOM;
	CSize size(100, 100);
	if( m_sptrDockClient != NULL )
	{
		m_sptrDockClient->GetDockSide( &dwSide );

		bool	bHorz = (dwSide == AFX_IDW_DOCKBAR_BOTTOM || 
										  dwSide == AFX_IDW_DOCKBAR_TOP);
		
		m_sptrDockClient->GetSize(&size,  !bHorz ); 

		/*if( bHorz )size.cx /= 4;
		else size.cy /= 4;*/
	}
	return size;
}


/* 
BOOL CShellDockBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	if (pNMHDR->hwndFrom == m_hwndChild)
	{	
		*pResult = ::SendMessage(m_hwndChild, WM_NOTIFY_CHILD, wParam, lParam);
		return 0;
	}
	
	return classDockBarBase::OnNotify(wParam, lParam, pResult);
}

*/

CSize CShellDockBar::CalcFrameSizeForFixedBar()
{
	if( m_sptrDockClient == 0 )
		return CSize( 100, 100 );
	CSize sizeFixed;
	m_sptrDockClient->GetSize( &sizeFixed, FALSE );

	CWnd	*pwndFrame = GetParentFrame();
	ASSERT( pwndFrame );

	int	cx = GetSystemMetrics( SM_CXBORDER )*2;
	int	cy = GetSystemMetrics( SM_CYBORDER )+GetSystemMetrics( SM_CYSMCAPTION );

	return ExpandSizes( sizeFixed );

	//AdjustWindowRectEx( &rect, /*WS_CAPTION*/WS_THICKFRAME, FALSE, WS_EX_PALETTEWINDOW);
	
	//sizeFixed.cx += 2;//rectWindow.Width()-rectClient.Width();
	//sizeFixed.cy += 2;//rectWindow.Height()-rectClient.Height();

}
CSize CShellDockBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	bool	bFixed = IsFixed();
	CSize	sizeFixed = CalcFrameSizeForFixedBar();

	bool	bNoDock = ((dwMode & LM_HORZDOCK) == 0 )&&((dwMode & LM_VERTDOCK) == 0 );

	CSize	size = classDockBarBase::CalcDynamicLayout( nLength, dwMode );
	 
	if( bFixed && bNoDock )
		return sizeFixed;

	/*if( !bNoDock )
	{
		if( dwMode & LM_HORZDOCK )
		{
			if( dwMode | LM_LENGTHY )
				return CSize( m_szHorz.cx, nLength );
			else
				return CSize( nLength, m_szHorz.cy );
		}
		if( dwMode & LM_VERTDOCK )
		{
			if( dwMode | LM_LENGTHY )
				return CSize( m_szVert.cx, nLength );
			else
				return CSize( nLength, m_szVert.cy );
		}
	}*/

	return size;
}

CSize CShellDockBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	//return classDockBarBase::CalcFixedLayout(bStretch, bHorz);

	int	nHeight = 0;
	bool	bFixed = IsFixed();

	classBarArray	arrSCBars;
	if( m_pDockBar )
		GetRowSizingBars( arrSCBars );
	else
		arrSCBars.Add( this );

	for( int i = 0; i < arrSCBars.GetSize(); i++ )
	{
		classDockBarBaseRoot	*pbar = arrSCBars[i];
		nHeight = max( bHorz ? pbar->m_szHorz.cy:pbar->m_szVert.cx, nHeight );
	}

	CSize	sizeFixed = CalcFrameSizeForFixedBar();

	if( !m_pDockContext->m_bDragging /*&& ! m_bTracking*/ )
	{
		if( bHorz )	sizeFixed.cy = nHeight;
		else	sizeFixed.cx = nHeight;
	}

	if( bFixed )
	{
		m_szHorz = m_szVert = sizeFixed;
		m_szFloat = sizeFixed;
		return sizeFixed;
	}

	/*if( bHorz )	m_szHorz.cy = nHeight;
	else	m_szVert.cx = nHeight;*/

	m_szHorz.cx = max( m_szHorz.cx, m_szMin.cx );
	m_szHorz.cy = max( m_szHorz.cy, m_szMin.cy );

	m_szVert.cx = max( m_szVert.cx, m_szMin.cx );
	m_szVert.cy = max( m_szVert.cy, m_szMin.cy );

	


	return bHorz?m_szHorz:m_szVert;
}

BOOL CShellDockBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !classDockBarBase::PreCreateWindow(cs) )return false;
	cs.style |= WS_BORDER;
	return true;
}

BOOL CShellDockBar::PreTranslateMessage(MSG* pMsg) 
{
	m_wndToolTip.RelayEvent (pMsg);
	HWND hWnd = pMsg->hwnd;

	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		while (m_hWnd && hWnd && hWnd != m_hWnd)
			hWnd = ::GetParent(hWnd);

		if (hWnd)
			return CControlBar::PreTranslateMessage(pMsg);
	}
	return false;
	//return CControlBar::PreTranslateMessage(pMsg);
}


void CShellDockBar::OnNcPaint() 
{
	classDockBarBase::OnNcPaint();
}

void CShellDockBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	classDockBarBase::OnNcCalcSize( bCalcValidRects, lpncsp );

	{ // !!!
		CString strName = ::GetObjectName(m_sptrName);
		int nEnableClose = GetValueInt( GetAppUnknown(), "\\MainFrame\\EnableDockCloseButton", strName, 1 );
		if(!nEnableClose)
		{
			for(int i=CDockablePane_BUTTONS_NUM-1; i>0; i--)
			{
				CRect rc = m_Buttons[i-1].GetRect();
				m_Buttons[i].Move(rc.TopLeft());
			}
			CRect rc = m_Buttons[0].GetRect();
			m_Buttons[0].Move(CPoint(-rc.left, -rc.right));
		}

		int nEnableMaximize = GetValueInt( GetAppUnknown(), "\\MainFrame", "EnableDockMaximizeButton", 1 );
		if(!nEnableMaximize && CDockablePane_BUTTONS_NUM>1)
		{
			for(int i=CDockablePane_BUTTONS_NUM-1; i>1; i--)
			{
				CRect rc = m_Buttons[i-1].GetRect();
				m_Buttons[i].Move(rc.TopLeft());
			}
			CRect rc = m_Buttons[1].GetRect();
			m_Buttons[1].Move(CPoint(-rc.left, -rc.right));
		}
	}
}

LRESULT CShellDockBar::OnNcHitTest(CPoint point) 
{
	return classDockBarBase::OnNcHitTest(point);
}

void CShellDockBar::OnCaptureChanged(CWnd *pWnd) 
{
	classDockBarBase::OnCaptureChanged(pWnd);
}

void CShellDockBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	classDockBarBase::OnLButtonUp(nFlags, point);
}

void CShellDockBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	classDockBarBase::OnMouseMove(nFlags, point);
}

void CShellDockBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (m_nEnableDock == -1)
	{
		CString strName = ::GetObjectName(m_sptrName);
		m_nEnableDock = GetValueInt( GetAppUnknown(), "\\MainFrame\\EnableDock", strName, 1 );
	}
	if (nHitTest == HTCAPTION && m_nEnableDock == 0)
		return;
	classDockBarBase::OnNcLButtonDown(nHitTest, point);
}

void CShellDockBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	if (m_nEnableDock == -1)
	{
		CString strName = ::GetObjectName(m_sptrName);
		m_nEnableDock = GetValueInt( GetAppUnknown(), "\\MainFrame\\EnableDock", strName, 1 );
	}
	if (m_nEnableDock == 0)
	{
        CWnd::OnNcLButtonDblClk(nHitTest, point);
		return;
	}
	classDockBarBase::OnNcLButtonDblClk(nHitTest, point);
}

void CShellDockBar::OnNcRButtonDown(UINT nHitTest, CPoint point) 
{
	classDockBarBase::OnNcRButtonDown(nHitTest, point);
}

void CShellDockBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	//paul 13.03.2003
	if( ( lpwndpos->flags & SWP_HIDEWINDOW ) && !m_bcan_close )
		lpwndpos->flags &= ~SWP_HIDEWINDOW;

	classDockBarBase::OnWindowPosChanging(lpwndpos);

	if( lpwndpos->flags & SWP_HIDEWINDOW && m_sptrDockClient != 0 )
	{
		m_sptrDockClient->OnHide();
		SendMessage( WM_CLOSE_DOCK_BAR, 0L, 0L );
		theApp.OnIdle( 0 );
	}
	else if( lpwndpos->flags & SWP_SHOWWINDOW  && m_sptrDockClient != 0 )
	{
		m_sptrDockClient->OnShow();
		theApp.OnIdle( 0 );
	}

	if( (lpwndpos->flags & (SWP_SHOWWINDOW)) && IsFloating() )
		// [vanek]   SBT:1196 - 28.10.2004
		SetCanClose( m_bcan_close ); // update close button
}

void CShellDockBar::OnPaint() 
{
	classDockBarBase::OnPaint();
}

BOOL CShellDockBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return classDockBarBase::OnSetCursor(pWnd, nHitTest, message);
}

void CShellDockBar::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	classDockBarBase::OnNcMouseMove(nHitTest, point);
}

void CShellDockBar::OnNcMButtonDblClk(UINT nHitTest, CPoint point) 
{
	classDockBarBase::OnNcMButtonDblClk(nHitTest, point);
}

void CShellDockBar::OnTimer(UINT_PTR nIDEvent) 
{
	classDockBarBase::OnTimer(nIDEvent);
}

void CShellDockBar::OnDestroy() 
{
	classDockBarBase::OnDestroy();
}

void CShellDockBar::OnNcRButtonUp(UINT nHitTest, CPoint point) 
{
	classDockBarBase::OnNcRButtonUp(nHitTest, point);
}

// [vanek] - 22.10.2004
LRESULT CShellDockBar::OnSetCanCloseWnd(WPARAM w, LPARAM l)
{
	SetCanClose( w != 0 );
    return 1;
}

CSize CShellDockBar::ExpandSizes( CSize sizeOrg )
{
	//return sizeOrg;
    // compute the the client area
    CRect rcClient( CPoint( 0, 0 ), sizeOrg );
    rcClient.InflateRect (m_nBorderSize, m_nBorderSize);

    m_dwSCBStyle &= ~SCBS_EDGEALL;

    switch(m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
        m_dwSCBStyle |= SCBS_EDGEBOTTOM;
        rcClient.InflateRect (m_cyGripper, 0, 0, 0);
        break;
    case AFX_IDW_DOCKBAR_BOTTOM:
        m_dwSCBStyle |= SCBS_EDGETOP;
        rcClient.InflateRect (m_cyGripper, 0, 0, 0);
        break;
    case AFX_IDW_DOCKBAR_LEFT:
        m_dwSCBStyle |= SCBS_EDGERIGHT;
        rcClient.InflateRect (0, m_cyGripper, 0, 0);
        break;
    case AFX_IDW_DOCKBAR_RIGHT:
        m_dwSCBStyle |= SCBS_EDGELEFT;
        rcClient.InflateRect (0, m_cyGripper, 0, 0);
        break;
    default:
        break;
    }

    if (!IsFloating() && m_pDockBar != NULL)
    {
        classBarArray arrSCBars;
        GetRowSizingBars(arrSCBars);

        for (int i = 0; i < arrSCBars.GetSize(); i++)
		{
            if (arrSCBars[i] == this)
            {
                //if (i > 0)
                    m_dwSCBStyle |= IsHorzDocked() ?
                        SCBS_EDGELEFT : SCBS_EDGETOP;
                //if (i < arrSCBars.GetSize() - 1)
                    m_dwSCBStyle |= IsHorzDocked() ?
                        SCBS_EDGERIGHT : SCBS_EDGEBOTTOM;
            }
		}
    }

    // make room for edges only if they will be painted
    if (m_dwSCBStyle & SCBS_SHOWEDGES)
	{
        rcClient.InflateRect (
            IsEdgeVisible (HTLEFT) ? m_cxEdge : 0,
            IsEdgeVisible (HTTOP) ? m_cxEdge : 0,
            IsEdgeVisible (HTRIGHT) ? m_cxEdge : 0,
            IsEdgeVisible (HTBOTTOM) ? m_cxEdge : 0);
	}

    return rcClient.Size();
}

void CShellDockBar::OnTrackUpdateSize(CPoint& point)
{
    ASSERT(!IsFloating());

    CSize szDelta = point - m_ptOld;

    if (szDelta == CSize(0, 0)) return; // no size change

    CSize sizeNew = m_szOld;
    switch (m_htEdge)
    {
    case HTLEFT:    sizeNew -= CSize(szDelta.cx, 0); break;
    case HTTOP:     sizeNew -= CSize(0, szDelta.cy); break;
    case HTRIGHT:   sizeNew += CSize(szDelta.cx, 0); break;
    case HTBOTTOM:  sizeNew += CSize(0, szDelta.cy); break;
    }

    // enforce the limits
    sizeNew.cx = max(m_szMinT.cx, min(m_szMaxT.cx, sizeNew.cx));
    sizeNew.cy = max(m_szMinT.cy, min(m_szMaxT.cy, sizeNew.cy));

    BOOL bHorz = IsHorzDocked();
    szDelta = sizeNew - (bHorz ? m_szHorz : m_szVert);
    
    OnTrackInvertTracker(); // erase tracker

    (bHorz ? m_szHorz : m_szVert) = sizeNew; // save the new size

    CSCBArray arrSCBars;
    GetRowSizingBars(arrSCBars);

//andy
	if (!IsSideTracking())
	{
		int	nNewSize = bHorz ? sizeNew.cy : sizeNew.cx;
		for (int i = 0; i < arrSCBars.GetSize(); i++)
		{
			CDockablePane* pBar = arrSCBars[i];

			if( !pBar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
				continue;
			if( !((CShellDockBar*) pBar )->IsFixed())
				continue;

			(bHorz ? pBar->m_szHorz.cy : pBar->m_szVert.cx) =
                bHorz ? sizeNew.cy : sizeNew.cx;

			//CSize	size = pBar->CalcDynamicLayout( bHorz ? sizeNew.cy:sizeNew.cx, (bHorz ? LM_LENGTHY:0)|(pBar->IsHorzDocked()?LM_HORZDOCK:LM_VERTDOCK) );
			CSize	size = pBar->CalcDynamicLayout( bHorz ? sizeNew.cy:sizeNew.cx, (bHorz ? LM_LENGTHY|LM_HORZ:0) );
			nNewSize = max( bHorz?size.cy:size.cx, nNewSize );
		}
		for ( i = 0; i < arrSCBars.GetSize(); i++)
		{
			CDockablePane * pBar = arrSCBars[i];
			(bHorz ? pBar->m_szHorz.cy : pBar->m_szVert.cx) = nNewSize;
		}
	}
	else
	{
		for (int i = 0; i < arrSCBars.GetSize(); i++)
		{
			CDockablePane* pBar = NULL;

			{   // adjust the neighbour's size too
				if (arrSCBars[i] != this) continue;
	//andy modification
				int	nNeightIdx = i +((m_htEdge == HTTOP || m_htEdge == HTLEFT) ? -1 : 1);
				if( nNeightIdx < 0 || nNeightIdx >=arrSCBars.GetSize() )
					continue;

				pBar = arrSCBars[i +
					((m_htEdge == HTTOP || m_htEdge == HTLEFT) ? -1 : 1)];
				ASSERT_VALID (pBar);
	//andy 
				(bHorz ? pBar->m_szHorz.cx : pBar->m_szVert.cy) -=
					bHorz ? szDelta.cx : szDelta.cy;
			}
		}
	}

    OnTrackInvertTracker(); // redraw tracker at new pos

}

void CShellDockBar::SetCanClose( bool bcan_close )
{
	m_bcan_close = bcan_close;

	// [vanek] : disable/enable close button  - 22.10.2004
	if( IsFloating() )
	{
		CWnd *pparent_wnd = 0;
		pparent_wnd = GetDockingFrame();

		if( pparent_wnd->IsKindOf( RUNTIME_CLASS(CVTMiniDockFrameWnd) ) )
		{
			HMENU hsysmenu = 0;
			hsysmenu = ::GetSystemMenu( pparent_wnd->GetSafeHwnd(), FALSE);
			if( hsysmenu )
			{
				DWORD dwflags = MF_BYCOMMAND;
				if( !m_bcan_close )
					dwflags |= MF_GRAYED;

				::EnableMenuItem( hsysmenu, SC_CLOSE, dwflags);             
			}
			return;
		}
	}
	
	SendMessage( WM_NCPAINT );

	//Invalidate();
}

bool CShellDockBar::GetCanClose( )
{
	if( !IsAllowMoving() )
		return false;

	return m_bcan_close;
}

void CShellDockBar::SetMiniFrameCaption( bool bMiniFrameCaption )
{
	m_bMiniFrameCaption = bMiniFrameCaption;
	if(!m_bMiniFrameCaption) m_cyGripper = 0;
	else m_cyGripper = max (12, GetGlobalData()->GetTextHeight ());
}

bool CShellDockBar::GetMiniFrameCaption( )
{
	return m_bMiniFrameCaption;
}

bool CShellDockBar::IsAllowMoving()
{
	static int nallow = -1;
	if( nallow == -1 )
		nallow = ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "AllowDockClose", 1L );
	return ( nallow == 1);
}

bool CShellDockBar::IsCloseButtonEnabled( )
{
    return GetCanClose( );
}																		


LRESULT CShellDockBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	//if( message == WM_SHOWWINDOW && wParam == FALSE && !m_bcan_close )
	//	return 1L;

	// [vanek] : даем возможность обновить состо€ние элементов управлени€, состо€ние которых зависит от внешних 
	// факторов, например, от  состо€ние какой-нибудь акции и т.д. - 28.10.2004
	if( message == WM_IDLEUPDATECMDUI && m_hwndChild )
		::SendMessage( m_hwndChild, WM_IDLEUPDATECMDUI, wParam, lParam);
	
	return CBCGDialogBar::WindowProc(message, wParam, lParam);
}

void CShellDockBar::DelayShow(BOOL bShow)
{
	if( !m_bcan_close && !bShow )
		return;

	__super::DelayShow(bShow);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CShellToolDockBar, CBCGToolDockBar)


void CShellToolDockBar::ResetArray()
{
	m_arrBars.SetSize( 0 );
	ValidateBarArray();

	// [vanek] : reset array of last visible dockbars - 15.10.2004
	ResetLastVisibleDockBars( );
}

void CShellToolDockBar::AddDockBar( CControlBar *pbar )
{
	ValidateBarArray();

	ASSERT(m_arrBars.GetSize() > 0 );

	DWORD	dwNewDockStyle = 0;

	UINT nID = GetDlgCtrlID();
	if( nID == AFX_IDW_DOCKBAR_RIGHT )		dwNewDockStyle = CBRS_ALIGN_RIGHT;
	else if( nID == AFX_IDW_DOCKBAR_LEFT )		dwNewDockStyle = CBRS_ALIGN_LEFT;
	else if( nID == AFX_IDW_DOCKBAR_TOP )		dwNewDockStyle = CBRS_ALIGN_TOP;
	else if( nID == AFX_IDW_DOCKBAR_BOTTOM )		dwNewDockStyle = CBRS_ALIGN_BOTTOM;

	pbar->m_dwStyle &= ~CBRS_ALIGN_ANY;
	pbar->m_dwStyle |= dwNewDockStyle;
	
	if (pbar->GetParent() != this)
	{
		pbar->SetParent(this);
	}

	if( pbar->m_pDockBar )
	{
		int nPos = pbar->m_pDockBar->FindBar( pbar, -1 );
		if( nPos != -1 )
			pbar->m_pDockBar->RemoveControlBar(pbar, -1);
	}

	pbar->m_pDockBar = this;

	ValidateBarArray();
	m_arrBars[m_arrBars.GetUpperBound()] = pbar;
	ValidateBarArray();

	// [vanek] : reset array of last visible dockbars - 15.10.2004
	ResetLastVisibleDockBars( );
}

CSize CShellToolDockBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	Invalidate();

	ValidateBarArray();

	CSize sizeFixed = CControlBar::CalcFixedLayout(bStretch, bHorz);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
		sizeMax = m_rectLayout.Size();
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame = GetParentFrame();
		pFrame->GetClientRect(&rectFrame);
		sizeMax = rectFrame.Size();
	}

	// prepare for layout
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos(m_arrBars.GetSize());
	int cxBorder = 2;//afxData.cxBorder2; 
	int cyBorder = 2;//afxData.cyBorder2;


	CTypedPtrArray<CObArray, CControlBar*> arrMenus;
	CTypedPtrArray<CObArray, CControlBar*> arrDockBars;
	CTypedPtrArray<CObArray, CControlBar*> arrOutlookBars;

	//1. remove menu bar and dock bars from array
	int	nPos = 0;
	for( int n = 0; n < m_arrBars.GetSize(); n++ )
	{
		CControlBar* pbar = GetDockedControlBar( n );
		if( !pbar )
			continue;
		if( pbar->IsKindOf( RUNTIME_CLASS( CBCGMenuBar ) ) )
		{
			ASSERT( GetDlgCtrlID() == AFX_IDW_DOCKBAR_TOP );
			arrMenus.Add( pbar );
			m_arrBars[n] = 0;
		}
		else if( pbar->IsKindOf( RUNTIME_CLASS( CDockablePane ) ) )
		{
			// A.M. BT4102
			bool bInserted = false;
			CRect rcNewBar;
			pbar->GetWindowRect(rcNewBar);
			for (int i = 0; i < arrDockBars.GetSize(); i++)
			{
				CControlBar *p = arrDockBars.GetAt(i);
				CRect rcBar;
				p->GetWindowRect(rcBar);
				if (bHorz ? rcNewBar.left < rcBar.left : rcNewBar.top < rcBar.top)
				{
					arrDockBars.InsertAt(i, pbar);
					bInserted = true;
					break;
				}
			}
			if (!bInserted)
				arrDockBars.Add( pbar );
			m_arrBars[n] = 0;
		}
		else if( pbar->IsKindOf( RUNTIME_CLASS( COutlookBar ) ) )
		{
			ASSERT( GetDlgCtrlID() == AFX_IDW_DOCKBAR_LEFT );
			arrOutlookBars.Add( pbar );
			m_arrBars[n] = 0;
		}
	}

	//2. clean the dummy row spaces
	bool	bLastEmpty = true;
	for( n = 0; n < m_arrBars.GetSize(); )
	{
		////void* pVoid = m_arrBars[n];
		void* pVoid = GetDockedControlBar( n );

		if( !pVoid )
		{
			if( bLastEmpty )
				m_arrBars.RemoveAt( n );
			else
			{
				bLastEmpty = true;
				n++;
			}
		}
		else
		{
			bLastEmpty = false;
			n++;
		}
	}
//insert menu bar at first position
	if( m_arrBars.GetSize() )
		ASSERT(m_arrBars[0]!=0 );
	m_arrBars.InsertAt( 0, (void*)0 );
	if( arrMenus.GetSize() )
	{
		ASSERT( arrMenus.GetSize() == 1 );
		m_arrBars.InsertAt( 0, arrMenus[0] );
		m_arrBars.InsertAt( 0, (void*)0 );
	}
	if( arrOutlookBars.GetSize() )
	{
		ASSERT( arrOutlookBars.GetSize() == 1 );
		m_arrBars.InsertAt( 0, arrOutlookBars[0] );
		m_arrBars.InsertAt( 0, (void*)0 );
	}
//insert dock bars at last position
	//ASSERT( m_arrBars[m_arrBars.GetSize()-1] == 0 );
	for( nPos = 0; nPos < arrDockBars.GetSize(); nPos++ )
		m_arrBars.Add( arrDockBars[nPos] );
	m_arrBars.Add( 0 );

	UINT 	nBarID = GetDlgCtrlID();

	
	CPoint	pointBottomRight = CPoint( 0, 0 );

	//3. loop for rows
	nPos = 0;
	while( nPos < m_arrBars.GetSize() )
	{
		CControlBar	*pbar = GetDockedControlBar( nPos );
		CTypedPtrList<CObList, CControlBar*>	arrBars;
		
		// [vanek] : массив показываемых докбаров (используетс€ при ресайзе докбаров, см. ниже) - 15.10.2004
		CTypedPtrArray<CObArray, CControlBar*> arrShowingDockBars;

		while( nPos < m_arrBars.GetSize() && !GetDockedControlBar( nPos ) )
			nPos++;
	//store the bars to the single row
		while( nPos < m_arrBars.GetSize() && (pbar=GetDockedControlBar( nPos ))!=0 )
		{
			if( pbar->IsVisible() )
			{
				arrBars.AddTail( pbar );

				// [vanek] : заполнение массива arrShowingDockBars - 15.10.2004
				bool bshowing_bar = true;
				for( int nidx = 0; nidx < m_arrLastVisibleDockBars.GetSize(); nidx ++ )
				{
					if( nPos == m_arrLastVisibleDockBars.GetAt( nidx ) )
					{	
                        bshowing_bar = false;                        						
						break;
					}
				}
				if( bshowing_bar )
					arrShowingDockBars.Add( pbar );
			}
			nPos++;
		}

	//process row
		POSITION posBar = arrBars.GetHeadPosition();
		int	nRowHeight = 0;
		while( posBar )
		{
			pbar = arrBars.GetNext( posBar );

			DWORD dwMode = 0;
				if ((pbar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
			(pbar->m_dwStyle & CBRS_FLOATING))
				dwMode |= LM_HORZ | LM_MRUWIDTH;
			else if (pbar->m_dwStyle & CBRS_ORIENT_HORZ)
				dwMode |= LM_HORZ | LM_HORZDOCK;
			else
				dwMode |=  LM_VERTDOCK;

			CSize sizeBar = pbar->CalcDynamicLayout(-1, dwMode);
	//special case for menu bar

			if( pbar->IsKindOf( RUNTIME_CLASS( CBCGMenuBar ) ) )
			{
				ASSERT( arrBars.GetCount() == 1 );

				if( bHorz )
					sizeBar.cx = sizeMax.cx;
				else
					sizeBar.cy = sizeMax.cy;

				CRect	rect( 0, 0, sizeBar.cx, sizeBar.cy );
				pbar->m_pDockContext->m_rectMRUDockPos = rect;
				//AfxRepositionWindow(&layout, pbar->m_hWnd, &rect);				
				pbar->MoveWindow( rect );
				pbar->Invalidate();

				nRowHeight = bHorz?sizeBar.cy:sizeBar.cx;
				break;
			}
			if( pbar->IsKindOf( RUNTIME_CLASS( COutlookBar ) ) )
			{
				ASSERT( arrBars.GetCount() == 1 );

				if( bHorz )
					sizeBar.cx = sizeMax.cx;
				else
					sizeBar.cy = sizeMax.cy;

				CRect	rect( 0, 0, sizeBar.cx, sizeBar.cy );
				pbar->m_pDockContext->m_rectMRUDockPos = rect;
				//AfxRepositionWindow(&layout, pbar->m_hWnd, &rect);				
				pbar->MoveWindow( rect );
				pbar->Invalidate();

				nRowHeight = bHorz?sizeBar.cy:sizeBar.cx;
				break;
			}
	//position other control bars
			nRowHeight = max( bHorz?sizeBar.cy:sizeBar.cx, nRowHeight );
			CRect	rect( pointBottomRight.x, pointBottomRight.y, pointBottomRight.x+sizeBar.cx, pointBottomRight.y+sizeBar.cy );
			pbar->m_pDockContext->m_rectMRUDockPos = rect;

			if( bHorz )pointBottomRight.x += rect.Width();
			else pointBottomRight.y += rect.Height();
		}

		// [vanek] SBT:1183 делаем докбары одной высоты(ширины), равной nRowHeight - 01.10.2004
		{
			POSITION	pos = arrBars.GetHeadPosition();
			while( pos )
			{
				CControlBar	*pbar = arrBars.GetNext( pos );
				CRect	rectBar = pbar->m_pDockContext->m_rectMRUDockPos;
				if( !bHorz )
					rectBar.right =  rectBar.left + nRowHeight;
				else
					rectBar.bottom =  rectBar.top + nRowHeight;
							
				pbar->m_pDockContext->m_rectMRUDockPos = rectBar;			
			}
		}

		//calc the size of fixed bars in the row
		int	nSizeTotal = 0;
		int	nSizeFixed = 0;
		int	nSizeMax = bHorz?sizeMax.cx:sizeMax.cy;
		int	nSizeFixedSide = 0;

		POSITION	pos = arrBars.GetHeadPosition();
		while( pos )
		{
			CControlBar	*pbar = arrBars.GetNext( pos );
			bool	bFixed = true;

			CRect	rectPos = pbar->m_pDockContext->m_rectMRUDockPos;

			if( pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
				bFixed = ((CShellDockBar*)pbar)->IsFixed();
			
			nSizeTotal+=bHorz?rectPos.Width():rectPos.Height();
			if( bFixed )
				nSizeFixed+=bHorz?rectPos.Width():rectPos.Height();

			nSizeFixedSide = max( nSizeFixedSide, bHorz?rectPos.Height():rectPos.Width() );
		}

		int	nDelta = nSizeMax-nSizeTotal;
        
		//expand the bars for fitting row
		pos = arrBars.GetTailPosition();
		while( pos && ((nDelta > 0)==(nSizeMax-nSizeTotal > 0)) )
		{
			POSITION	posRight = pos;
			CControlBar	*pbar = arrBars.GetPrev( pos );
			bool	bFixed = true;

			if( pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
				bFixed = ((CShellDockBar*)pbar)->IsFixed();

			if( bFixed )
				continue;


			// [vanek] : ресайзим докбары, которые уже были видны, а показываемые докбары не трогаем - 15.10.2004
			bool bshowing = false;
			for( int nidx = 0; nidx < arrShowingDockBars.GetSize(); nidx ++ )
			{
				if( bshowing = pbar == arrShowingDockBars.GetAt( nidx ) )
					break;
			}
			if( bshowing )
				continue;
			

			CShellDockBar	*pbarShell = (CShellDockBar*)pbar;
			CRect	rectBar = pbar->m_pDockContext->m_rectMRUDockPos;

			int	nMin = bHorz?pbarShell->GetMinSize().cx:pbarShell->GetMinSize().cy;
			int	nCurrent  = bHorz?rectBar.Width():rectBar.Height();
			int	nNewSize = max( nMin, nCurrent+nDelta );
			int	nChange = nNewSize - nCurrent;

			if( bHorz )
			{
				rectBar.right = rectBar.left + nNewSize;
			}
			else
			{
				rectBar.bottom= rectBar.top + nNewSize;
			}

			
			if( nBarID == AFX_IDW_DOCKBAR_LEFT )
				rectBar.left = rectBar.right-nSizeFixedSide;
			else if( nBarID == AFX_IDW_DOCKBAR_TOP )
				rectBar.top = rectBar.bottom-nSizeFixedSide;
			else if( nBarID == AFX_IDW_DOCKBAR_RIGHT )
				rectBar.right = rectBar.left+nSizeFixedSide;
			else if( nBarID == AFX_IDW_DOCKBAR_BOTTOM )
				rectBar.bottom = rectBar.top+nSizeFixedSide;

			pbar->m_pDockContext->m_rectMRUDockPos = rectBar;
			arrBars.GetNext( posRight );

			while( posRight )
			{
				CControlBar	*pbar = arrBars.GetNext( posRight );
				pbar->m_pDockContext->m_rectMRUDockPos.OffsetRect( bHorz?nChange:0, bHorz?0:nChange );
			}
			nDelta -= nChange;
		}

		// [vanek] : если не влезли - ресайзим все видимые докбары - 15.10.2004
		if( nDelta != 0 )
		{
			pos = arrBars.GetTailPosition();
			while( pos && ((nDelta > 0)==(nSizeMax-nSizeTotal > 0)) )
			{
				POSITION	posRight = pos;
				CControlBar	*pbar = arrBars.GetPrev( pos );
				bool	bFixed = true;

				if( pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
					bFixed = ((CShellDockBar*)pbar)->IsFixed();

				if( bFixed )
					continue;

                CShellDockBar	*pbarShell = (CShellDockBar*)pbar;
				CRect	rectBar = pbar->m_pDockContext->m_rectMRUDockPos;

				int	nMin = bHorz?pbarShell->GetMinSize().cx:pbarShell->GetMinSize().cy;
				int	nCurrent  = bHorz?rectBar.Width():rectBar.Height();
				int	nNewSize = max( nMin, nCurrent+nDelta );
				int	nChange = nNewSize - nCurrent;

				if( bHorz )
				{
					rectBar.right = rectBar.left + nNewSize;
				}
				else
				{
					rectBar.bottom= rectBar.top + nNewSize;
				}


				if( nBarID == AFX_IDW_DOCKBAR_LEFT )
					rectBar.left = rectBar.right-nSizeFixedSide;
				else if( nBarID == AFX_IDW_DOCKBAR_TOP )
					rectBar.top = rectBar.bottom-nSizeFixedSide;
				else if( nBarID == AFX_IDW_DOCKBAR_RIGHT )
					rectBar.right = rectBar.left+nSizeFixedSide;
				else if( nBarID == AFX_IDW_DOCKBAR_BOTTOM )
					rectBar.bottom = rectBar.top+nSizeFixedSide;

				pbar->m_pDockContext->m_rectMRUDockPos = rectBar;
				arrBars.GetNext( posRight );

				while( posRight )
				{
					CControlBar	*pbar = arrBars.GetNext( posRight );
					pbar->m_pDockContext->m_rectMRUDockPos.OffsetRect( bHorz?nChange:0, bHorz?0:nChange );
				}
				nDelta -= nChange;
			}
		}

		//pos windows
		pos = arrBars.GetHeadPosition();
		while( pos )
		{
			CControlBar	*pbar = arrBars.GetNext( pos );
			//AfxRepositionWindow(&layout, pbar->m_hWnd, &pbar->m_pDockContext->m_rectMRUDockPos );
			pbar->MoveWindow( pbar->m_pDockContext->m_rectMRUDockPos );
			pbar->Invalidate();

			bool	bFixed = true;

			if( pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
				bFixed = ((CShellDockBar*)pbar)->IsFixed();

			if( bFixed )
				continue;

			CRect	rect = pbar->m_pDockContext->m_rectMRUDockPos;
			((CShellDockBar*)pbar)->CommitSize( rect.Size() );
		}


		if( bHorz )
		{
			pointBottomRight.x = 0;
			pointBottomRight.y += nRowHeight;
		}
		else
		{
			pointBottomRight.x += nRowHeight;
			pointBottomRight.y = 0;
		}
	}


	if( bHorz )sizeMax.cy = pointBottomRight.y;
	else sizeMax.cx = pointBottomRight.x;

	// move and resize all the windows at once!
	if (!m_bLayoutQuery)
	{
		if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
			TRACE0("Warning: DeferWindowPos failed - low system resources.\n");
	}

	// [vanek] : обновл€ем список видимых докбаров - 15.10.2004
	{
		CControlBar	*pbar = 0;
		int npos = 0;
		m_arrLastVisibleDockBars.RemoveAll();
		while( npos < m_arrBars.GetSize() )
		{
			pbar = 0;
			pbar = GetDockedControlBar( npos );
			if( pbar && pbar->IsVisible() )
                m_arrLastVisibleDockBars.Add( npos );
			npos ++;
		}

	}

	return sizeMax;
}


POSITION	CShellToolDockBar::GetFirstDockBarPosition()
{
	for( int n = 0; n < m_arrBars.GetSize(); n++ )
	{
		CControlBar* pbar = GetDockedControlBar( n );
		if( !pbar )
			continue;
		if( pbar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
			return (POSITION)n;
	}
	return 0;
}

CShellDockBar	*CShellToolDockBar::GetNextDockBar( POSITION &rPos )
{
	int n = (int)rPos;
	CShellDockBar	*pbarSave = (CShellDockBar	*)m_arrBars[n++];
	ASSERT( pbarSave);
	ASSERT( pbarSave->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) );

	for( ; n < m_arrBars.GetSize(); n++ )
	{
		CControlBar* pbar = GetDockedControlBar( n );
		if( !pbar )
			continue;
		if( pbar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
		{
			rPos = (POSITION)n;
			return (CShellDockBar*)pbarSave;
		}
	}
	rPos = (POSITION)0;
	return pbarSave;
}

void CShellToolDockBar::ValidateBarArray()
{
	if( m_arrBars.GetSize() == 0 )
	{
		m_arrBars.Add( 0 );
		m_arrBars.Add( 0 );
	}

	if( m_arrBars[m_arrBars.GetUpperBound()] != 0 )
		m_arrBars.Add( 0 );

	if( m_arrBars[0] != 0 )
		m_arrBars.InsertAt( 0, 0, 1 );

}

CString	CShellDockBar::GetName( bool bUser )
{
	BSTR	bstrName;
	if( bUser )
		m_sptrName->GetUserName(&bstrName);
	else
		m_sptrName->GetName(&bstrName);
	CString	s;
	s = bstrName;
	::SysFreeString( bstrName );
	return s;
};

BOOL ProcessEraseBackground( HDC hdc, HWND hwnd );

BOOL CShellToolDockBar::OnEraseBkgnd(CDC* pDC) 
{
	return ::ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
}

BEGIN_MESSAGE_MAP(CShellToolDockBar, CBCGToolDockBar)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

