// SplitterCompare.cpp : implementation file
//

#include "stdafx.h"
#include "stdsplitter.h"
#include "SplitterCompare.h"
#include "SplitterWindow.h"
#include "dbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterCompare

CSplitterCompare::CSplitterCompare()
{
	_OleLockApp( this );

	m_nPrevPane0Width = 0;

	m_bDelimiter		= ::GetValueInt(GetAppUnknown(), "\\SplitterCompare", "Delimiter", 1) == 1;
	m_bHorizontal		= ::GetValueInt(GetAppUnknown(), "\\SplitterCompare", "Horizontal", 1) == 1;
	m_bShowSplitButton	= ( 1L == ::GetValueInt(GetAppUnknown(), "\\SplitterCompare", "ShowSplitButton", 1 ) );
	m_mode = dmNone;
	m_bMergeSmall = false;
	m_sName = "SplitterCompare";
}

CSplitterCompare::~CSplitterCompare()
{
	_OleUnlockApp( this );
}


void CSplitterCompare::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CSplitterStandard::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CSplitterCompare, CSplitterStandard)
	//{{AFX_MSG_MAP(CSplitterCompare)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSplitterCompare, CWnd)

// {E30040CA-92C6-4094-9105-A0D954A897FE}
GUARD_IMPLEMENT_OLECREATE(CSplitterCompare, "StdSplitter.SplitterCompare", 
0xe30040ca, 0x92c6, 0x4094, 0x91, 0x5, 0xa0, 0xd9, 0x54, 0xa8, 0x97, 0xfe);

BEGIN_INTERFACE_MAP(CSplitterCompare, CSplitterStandard)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterCompare message handlers

static bool s_bNeedPreventChangeScroll = false;

CSplitterCompare::XSplitterEntry *
CSplitterCompare::GetEntry( int nNo )
{
	if( nNo == 0 )
		return GetViewEntry( 0, 0 );
	if( !IsSplitted() )
		return 0;
	return GetViewEntry( m_bHorizontal?0:1, m_bHorizontal?1:0 );
}
bool CSplitterCompare::IsSplitted()
{
	return m_bHorizontal?
		(GetColsCount()>1):(GetRowsCount()>1);
}

void CSplitterCompare::LayoutWindow( XSplitterEntry *pe, CRect rc, HDWP &hdwp )
{
	bool bSplittedLeftPane = pe->nCol+pe->nRow == 0 && IsSplitted();
	bool bSplitted = IsSplitted();
	bool bAltPaint = false;

	if(bSplitted)
	{
		XSplitterEntry *pe0 = GetEntry( 0 );
		XSplitterEntry *pe1 = GetEntry( 1 );
		bAltPaint  = !(CheckInterface(pe0->punkView, IID_IBlankView) || CheckInterface(pe1->punkView, IID_IBlankView));
	}

			
	//s_bNeedPreventChangeScroll = bSplitted && ((m_mode & dmHorz)==dmHorz);
	//s_bNeedPreventChangeScroll = bSplitted;


	if(bAltPaint)
	{
		IUnknown* punkSM = ::GetSyncManager();
		ISyncManagerPtr sptrSM(punkSM);
		if(punkSM)
		{
			punkSM->Release();
			sptrSM->SetLockSyncMode(TRUE);
		}
	}

	int	cxScroll = ::GetSystemMetrics( SM_CXVSCROLL );
	int	cyScroll = ::GetSystemMetrics( SM_CYHSCROLL );
	int	cxButton = ::GetSystemMetrics( SM_CXHSCROLL );
	int	cyButton = ::GetSystemMetrics( SM_CYVSCROLL );

	CRect	rcWindow;
	HWND hwnd = ::GetWindowFromUnknown( pe->punkView )->GetSafeHwnd();
	hwnd = ::GetParent( hwnd );

	bool	bHide = rc.Width() < cxScroll ||
					rc.Height() < cyScroll;

	int	nShowCmd = bHide?SW_HIDE:SW_SHOW;

	pe->m_pscrollVert->ShowWindow( nShowCmd );
	pe->m_pscrollHorz->ShowWindow( nShowCmd );

	CWnd	*pwnd_split = m_bHorizontal?pe->m_pwnd1:pe->m_pwnd3;
	CWnd	*pwnd_merge = m_bHorizontal?pe->m_pwnd2:pe->m_pwnd4;


	if(bSplitted)
	{
		pwnd_merge->ShowWindow( m_bShowSplitButton ? nShowCmd : SW_HIDE );
		pwnd_split->ShowWindow( FALSE );
	}
	else
	{
		pwnd_split->ShowWindow( m_bShowSplitButton ? nShowCmd : SW_HIDE  );
		pwnd_merge->ShowWindow( FALSE );
	}
	
	
	pe->m_pwnd5->ShowWindow( nShowCmd );
	CWnd* pWnd = CWnd::FromHandle( hwnd );
	if(pWnd)
		pWnd->ShowWindow( nShowCmd );

	if( bHide )
		return;

//place the pane
	rcWindow = rc;
	CRect	rcButtonM = rc;
	CRect	rcButtonSH = rc;
	CRect	rcScrollH = rc;
	CRect	rcScrollV = rc;
	if( m_bHorizontal )
	{
		if(bSplittedLeftPane)
		{
			rcWindow.InflateRect( -cxScroll, 0, 0, -cyScroll);
			rcButtonM.right = cxScroll;
			rcButtonM.top = rcButtonM.bottom - cyScroll;
			rcButtonSH.left = rcButtonM.right;
			rcButtonSH.right = rcButtonSH.left + cxButton;
			rcButtonSH.top = rcButtonM.top;
			rcScrollH.left = m_bShowSplitButton ? rcButtonSH.right : rcButtonSH.left;
			rcScrollH.right -= SplitZoneWidth/2;
			rcScrollH.top = rc.bottom - cyScroll;
			rcScrollV.right = rcScrollV.left + cxScroll;
			rcScrollV.bottom = rcButtonM.top;
		}
		else
		{
			rcWindow.InflateRect( 0, 0, -cxScroll, -cyScroll);
			rcButtonM.left = rcButtonM.right - cxScroll;
			rcButtonM.top = rcButtonM.bottom - cyScroll;
			rcButtonSH.right = rcButtonM.left;
			rcButtonSH.left = rcButtonSH.right - cxButton;
			rcButtonSH.top = rcButtonM.top;
			if(bSplitted)rcScrollH.left += SplitZoneWidth/2;
			rcScrollH.right = m_bShowSplitButton ? rcButtonSH.left : rcButtonSH.right;
			rcScrollH.top = rc.bottom - cyScroll;
			rcScrollV.left = rcScrollV.right - cxScroll;
			rcScrollV.bottom = rcButtonM.top;
		}
	}
	else
	{
		if(bSplittedLeftPane)
		{
			rcWindow.InflateRect( 0, -cyScroll, -cxScroll, 0);

			rcButtonM.left = rcButtonM.right-cxScroll;
			rcButtonM.bottom = rcButtonM.top+cyScroll;
			rcButtonSH.top = rcButtonM.bottom;
			rcButtonSH.bottom = rcButtonSH.top + cyButton;
			rcButtonSH.left = rcButtonM.left;
			
			rcScrollH.right = rcButtonM.left;
			rcScrollH.bottom = cyScroll;
			
			rcScrollV.left = rcScrollV.right-cxScroll;
			rcScrollV.top = m_bShowSplitButton ? rcButtonSH.bottom : rcButtonSH.top;
			rcScrollV.bottom -= SplitZoneWidth/2;
		
		}
		else
		{
			rcWindow.InflateRect( 0, 0, -cyScroll, -cxScroll);

			rcButtonM.left = rcButtonM.right-cxScroll;
			rcButtonM.top = rcButtonM.bottom-cyScroll;

			rcButtonSH.bottom = rcButtonM.top;
			rcButtonSH.top = rcButtonSH.bottom - cyButton;
			rcButtonSH.left = rcButtonM.left;
			
			
			rcScrollH.right = rcButtonM.left;
			rcScrollH.top = rcScrollH.bottom-cyScroll;
			
			rcScrollV.left = rcScrollV.right - cxScroll;
			if(bSplitted)rcScrollV.top += SplitZoneWidth/2;
			rcScrollV.bottom = m_bShowSplitButton ? rcButtonSH.top : rcButtonSH.bottom;
			
		}
	}
	
	
	CRect	rcOldScrollVert;
	pe->m_pscrollVert->GetWindowRect( rcOldScrollVert );
	ScreenToClient( rcOldScrollVert );
	CRect	rcOldScrollHorz;
	pe->m_pscrollVert->GetWindowRect( rcOldScrollHorz );
	ScreenToClient( rcOldScrollHorz );

	DWORD	dwFlags = SWP_NOACTIVATE|SWP_NOZORDER/*|SWP_NOREDRAW*/;

	bool	bFirstlyMoveVertSB = rcOldScrollVert.CenterPoint().x > rcScrollV.CenterPoint().x;
	bool	bFirstlyMoveHorzSB = rcOldScrollHorz.CenterPoint().x > rcScrollH.CenterPoint().x;
	
	if( bFirstlyMoveVertSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollVert, HWND_NOTOPMOST, rcScrollV.left, rcScrollV.top, rcScrollV.Width(), rcScrollV.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd3, HWND_NOTOPMOST, rcButtonSV.left, rcButtonSV.top, rcButtonSV.Width(), rcButtonSV.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd4, HWND_NOTOPMOST, rcButtonNSV.left, rcButtonNSV.top, rcButtonNSV.Width(), rcButtonNSV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd5, HWND_NOTOPMOST, rcButtonM.left, rcButtonM.top, rcButtonM.Width(), rcButtonM.Height(), dwFlags );
	}

	if( bFirstlyMoveHorzSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollHorz, HWND_NOTOPMOST, rcScrollH.left, rcScrollH.top, rcScrollH.Width(), rcScrollH.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, bSplitted?*pwnd_merge:*pwnd_split, HWND_NOTOPMOST, rcButtonSH.left, rcButtonSH.top, rcButtonSH.Width(), rcButtonSH.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pwnd_merge, HWND_NOTOPMOST, rcButtonNSH.left, rcButtonNSH.top, rcButtonNSH.Width(), rcButtonNSH.Height(), dwFlags );
	}

	if(!bAltPaint)
		hdwp = ::DeferWindowPos( hdwp, hwnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), dwFlags );
	

	if( !bFirstlyMoveVertSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollVert, HWND_NOTOPMOST, rcScrollV.left, rcScrollV.top, rcScrollV.Width(), rcScrollV.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd3, HWND_NOTOPMOST, rcButtonSV.left, rcButtonSV.top, rcButtonSV.Width(), rcButtonSV.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd4, HWND_NOTOPMOST, rcButtonNSV.left, rcButtonNSV.top, rcButtonNSV.Width(), rcButtonNSV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd5, HWND_NOTOPMOST, rcButtonM.left, rcButtonM.top, rcButtonM.Width(), rcButtonM.Height(), dwFlags );
	}

	if( !bFirstlyMoveHorzSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollHorz, HWND_NOTOPMOST, rcScrollH.left, rcScrollH.top, rcScrollH.Width(), rcScrollH.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, bSplitted?*pwnd_merge:*pwnd_split, HWND_NOTOPMOST, rcButtonSH.left, rcButtonSH.top, rcButtonSH.Width(), rcButtonSH.Height(), dwFlags );
		//hdwp = ::DeferWindowPos( hdwp, *pwnd_merge, HWND_NOTOPMOST, rcButtonNSH.left, rcButtonNSH.top, rcButtonNSH.Width(), rcButtonNSH.Height(), dwFlags );
	}

	CRect	rcClient;
	GetClientRect( &rcClient );

	if( !bSplitted )
		pwnd_split->EnableWindow( rcClient.Width() > m_sizeMin.cx*2 );

	if(bAltPaint)
	{
		IScrollZoomSitePtr sptrSZS(pe->punkView);
		if(sptrSZS)
			sptrSZS->LockScroll(TRUE);

		CWnd* pwnd = ::GetWindowFromUnknown( pe->punkView )->GetParent();
		pwnd->MoveWindow(rcWindow, FALSE);
	}

	
}	

CRect CSplitterCompare::CalcWindowRect( XSplitterEntry *pe )
{
	if( !pe )
		return NORECT;

	int nRow = pe->nRow;
	int nCol = pe->nCol;

	CRect	rcClient;
	CRect	rcPane;

	GetClientRect( &rcClient );

	if( m_bHorizontal )
	{
		rcPane.left = int( nCol*m_fposX*rcClient.Width() );
		rcPane.right = int( (nCol == (GetColsCount()-1))?rcClient.Width():(m_fposX*rcClient.Width()) );
		rcPane.top = 0;
		rcPane.bottom = rcClient.Height();
	}
	else
	{
		rcPane.top = int( nRow*m_fposY*rcClient.Height() );
		rcPane.bottom = int( (nRow == (GetRowsCount()-1))?rcClient.Height():(m_fposY*rcClient.Height()) );
		rcPane.left = 0;
		rcPane.right = rcClient.Width();
	}

	if( IsSplitted() )
	{
		int	d = m_bDelimiter?-1:0;

		if( !m_bHorizontal )
		{
			if(nRow == 0)
				rcPane.InflateRect( -cxLeft, /*-cyTop*/0, m_bHorizontal?d:-cxRight, m_bHorizontal?-cyBottom:d );
			else
				rcPane.InflateRect( -cxLeft, -cyTop, m_bHorizontal?d:-cxRight, m_bHorizontal?-cyBottom:d );
		}
		else
		{
			if(nCol == 0)
				rcPane.InflateRect( -cxLeft, -cyTop, m_bHorizontal?d:-cxRight, m_bHorizontal?-cyBottom:d );
			else
				rcPane.InflateRect( m_bHorizontal?0:-cxLeft, m_bHorizontal?-cyTop:0, -cxRight, -cyBottom );
		}
	}
	else
		rcPane.InflateRect( -cxLeft, -cyTop, -cxRight, -cyBottom );

	return rcPane;
}

void CSplitterCompare::OnPaint() 
{
	//TRACE0("===========================SPLITTER PAINT\n");
	CPaintDC dc(this); // device context for painting

	CRect	rcClient = NORECT;
	GetClientRect( rcClient );


	dc.FillRect( rcClient, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );

	POSITION	pos = GetFirstViewEntryPosition();

	
	int nCol = 0;
	while( pos )
	{
		XSplitterEntry *pe = GetNextViewEntry( pos );
		

		CRect rcPane = CalcWindowRect( pe );

		if( IsSplitted() )
		{
			if(nCol == 0)
				rcPane.DeflateRect( -cxLeft, -cyTop, m_bHorizontal?-1:-cxRight, m_bHorizontal?-cyBottom:-1 );
			else
				rcPane.DeflateRect( m_bHorizontal?-1:-cxLeft, m_bHorizontal?-cyTop:-1, -cxRight, -cyBottom );
		}
		else
			rcPane.DeflateRect( -cxLeft, -cyTop, -cxRight, -cyBottom );


		COLORREF	crStart, crEnd;
		if( pe == m_pveActiveView )
		{
			crStart = RGB( 255, 0, 0 );
			crEnd = RGB( 255, 255, 0 );
			
			dc.FrameRect(&rcPane, &CBrush(crStart));
			rcPane.InflateRect( -1, -1 );
			dc.FrameRect(&rcPane, &CBrush(crEnd));

			//dc.Draw3dRect( rcPane, crStart, crEnd );	
		}
		nCol++;
	}

	if( IsSplitted() )
	{
		CRect rcPane = CalcWindowRect(GetEntry( 0 ));
		if(m_bDelimiter)
		{
			COLORREF clrSeparateLineColor = GetValueColor(GetAppUnknown(), "SplitterCompare", "SeparateLineColor", RGB(0,0,0));

			CPen pen(PS_SOLID, 1, clrSeparateLineColor);
			CPen* pOldPen = dc.SelectObject(&pen);

			dc.MoveTo(rcPane.right, rcPane.top);
			dc.LineTo(rcPane.right, rcPane.bottom);
			
			dc.SelectObject(pOldPen);
		}


		CRect	rcSplitZone = rcClient;

		if( m_bHorizontal )
		{
			rcSplitZone.bottom -= cyBottom;
			rcSplitZone.top = rcSplitZone.bottom - ::GetSystemMetrics( SM_CYHSCROLL );
			rcSplitZone.left = rcPane.right - SplitZoneWidth/2;
			rcSplitZone.right = rcPane.right + SplitZoneWidth/2 + 1;
		}
		else
		{
			rcSplitZone.right -= cxRight;
			rcSplitZone.left = rcSplitZone.right - ::GetSystemMetrics( SM_CXVSCROLL );
			rcSplitZone.top = rcPane.bottom - SplitZoneWidth/2;
			rcSplitZone.bottom = rcPane.bottom+ SplitZoneWidth/2 + 1;
		}

		dc.DrawFrameControl(&rcSplitZone, DFC_BUTTON, DFCS_BUTTONPUSH);

		rcSplitZone.DeflateRect(1,1,2,2);


		CBitmap Bmp;
		Bmp.LoadBitmap(MAKEINTRESOURCE(IDB_COMPARE_MOVER));
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap* pOldBmp = memDC.SelectObject(&Bmp);

		BITMAP bmp;
		Bmp.GetBitmap(&bmp);

		dc.StretchBlt(rcSplitZone.left, rcSplitZone.top, rcSplitZone.Width(), rcSplitZone.Height(), &memDC, 0, 0, bmp.bmWidth,  bmp.bmHeight, SRCCOPY);

		memDC.SelectObject(pOldBmp);
	}
}

CSplitterCompare::DragMode 	CSplitterCompare::HitTest( CPoint pt )
{
	m_mode = dmNone;

	if( GetEntriesCount() == 2 )
	{
		CRect	rcClient = NORECT;
		GetClientRect( rcClient );

		XSplitterEntry *pe1 = GetEntry( 0 );
		XSplitterEntry *pe2 = GetEntry( 1 );

		CRect	rc1 = CalcWindowRect( pe1 );
		CRect	rc2 = CalcWindowRect( pe2 );

		if( m_bHorizontal )
		{
			if(pt.y < rcClient.bottom - cyBottom &&  pt.y > rcClient.bottom - ::GetSystemMetrics( SM_CYHSCROLL ) - cyBottom)
			{
				if( rc1.right-SplitZoneWidth/2 < pt.x && rc2.left+SplitZoneWidth/2 > pt.x )
					m_mode = DragMode(m_mode|dmHorz);
			}
			else
			{
				if( rc1.right == pt.x )
					m_mode = DragMode(m_mode|dmHorz);
			}
		}
		else
		{
			if(pt.x < rcClient.right - cxRight &&  pt.x > rcClient.right - ::GetSystemMetrics( SM_CXVSCROLL ) - cxRight)
			{
				if( rc1.bottom-SplitZoneWidth/2 < pt.y && rc2.top+SplitZoneWidth/2 > pt.y )
					m_mode = DragMode(m_mode|dmVert);
			}
			else
			{
				if( rc1.bottom == pt.y )
					m_mode = DragMode(m_mode|dmVert);
			}
		}
	}
	return m_mode;
}

void CSplitterCompare::AfterRecalcLayout()
{
	if( IsSplitted() )
	{
		XSplitterEntry *pe0 = GetEntry( 0 );
		XSplitterEntry *pe1 = GetEntry( 1 );

		long nCol;
		CWnd	*pwnd = 0;

		CWnd	*pwnd0 = ::GetWindowFromUnknown( pe0->punkView )->GetParent();
		CRect	rcPane0 = CalcWindowRect( pe0 );

		CRect	rcSplitZone;
		GetClientRect( &rcSplitZone );

		if( !m_bHorizontal )
		{
			rcSplitZone.left = rcSplitZone.right - ::GetSystemMetrics( SM_CXVSCROLL )-cxRight;
			rcSplitZone.top = rcPane0.bottom - SplitZoneWidth/2-1;
			rcSplitZone.bottom = rcSplitZone.top+SplitZoneWidth+2;
		}

		InvalidateRect( rcSplitZone );


		if(CheckInterface(pe0->punkView, IID_IBlankView) || CheckInterface(pe1->punkView, IID_IBlankView))
			return;

		

		

		SCROLLINFO	si;
		ZeroMemory( &si, sizeof( si ) );
		si.cbSize = sizeof( si );
		si.fMask = SIF_ALL;

		CWnd	*pwnd_scroll = m_bHorizontal?pe1->m_pscrollHorz:pe1->m_pscrollVert;

		::GetScrollInfo( pe1->m_pscrollHorz->m_hWnd, SB_CTL, &si);

		if(m_nPrevPane0Width > 0)
		{
			if(pe0 && pe1 && pwnd_scroll)
			{
				long nScrollPos = ::GetScrollPos(pwnd_scroll->m_hWnd, SB_CTL);
  
				pwnd = ::GetWindowFromUnknown( pe1->punkView )->GetParent();

				long nSP = si.nPos+((m_bHorizontal?rcPane0.Width():rcPane0.Height()) - m_nPrevPane0Width)/*+(m_bDelimiter?1:0)*/;
				
				pwnd->SendMessage( m_bHorizontal?WM_HSCROLL:WM_VSCROLL, 
					MAKELONG(SB_THUMBPOSITION, nSP), (LPARAM)pwnd_scroll->m_hWnd );			

			}
		}

		for( nCol = 0; nCol < GetEntriesCount(); nCol++ )
		{
			XSplitterEntry	*pe = GetEntry( nCol );
			IScrollZoomSitePtr sptrSZS(pe->punkView);
			if(sptrSZS != 0)
				sptrSZS->UpdateScroll( SB_BOTH );

			pwnd = ::GetWindowFromUnknown( pe->punkView )->GetParent();
			pwnd->Invalidate();
			pwnd->UpdateWindow();
			
			if(sptrSZS != 0)
				sptrSZS->LockScroll(FALSE);
		}


		m_nPrevPane0Width = m_bHorizontal?rcPane0.Width():rcPane0.Height();

		IUnknown* punkSM = ::GetSyncManager();
		ISyncManagerPtr sptrSM(punkSM);
		if(punkSM)
		{
			punkSM->Release();
			sptrSM->SetLockSyncMode(FALSE);
		}

	}
	

	UpdateWindow();


}

void CSplitterCompare::AfterSplitCol()
{
	if( !IsSplitted() )
		return;

	XSplitterEntry	*pe0 = GetEntry( 0 );
	XSplitterEntry	*pe1 = GetEntry( 1 );

	IDataContext2Ptr sptrContext0(pe0->punkView);
	IDataContext2Ptr sptrContext1(pe1->punkView);

	
	long nCount = 0;
	sptrContext0->GetSelectedCount(_bstr_t(szArgumentTypeImage), &nCount);
	if(nCount > 1)
	{
		long nPos = 0;
		sptrContext0->GetFirstSelectedPos(_bstr_t(szArgumentTypeImage), &nPos);

		IUnknown *punkObject0 = 0, *punkObject1 = 0;

		if(nPos)
			sptrContext0->GetNextSelected(_bstr_t(szArgumentTypeImage), &nPos, &punkObject0);
			if(nPos)
				sptrContext0->GetNextSelected(_bstr_t(szArgumentTypeImage), &nPos, &punkObject1);
		

		if(punkObject0)
		{
			sptrContext0->SetActiveObject(_bstr_t(szArgumentTypeImage), punkObject0, aofActivateDepended);
			punkObject0->Release();
		}
		if(punkObject1)
		{
			sptrContext1->SetActiveObject(_bstr_t(szArgumentTypeImage), punkObject1, aofActivateDepended);
			punkObject1->Release();
		}
	}

	
	SCROLLINFO	si;
	ZeroMemory( &si, sizeof( si ) );
	si.cbSize = sizeof( si );
	si.fMask = SIF_ALL;

	CWnd	*pwnd_scroll = m_bHorizontal?pe1->m_pscrollHorz:pe1->m_pscrollVert;
	::GetScrollInfo( pwnd_scroll->m_hWnd, SB_CTL, &si);

	if(si.nPage > 0)
	{
		CWnd* pwnd = ::GetWindowFromUnknown( pe1->punkView )->GetParent();
		pwnd->SendMessage( m_bHorizontal?WM_HSCROLL:WM_VSCROLL, 
			MAKELONG(SB_RIGHT, 0), (LPARAM)pwnd_scroll->m_hWnd );			
	}
}


long CSplitterCompare::get_width()
{
	XSplitterEntry *pe1 = GetEntry( 0 );
	XSplitterEntry *pe2 = GetEntry( 1 );

	CRect	rc1 = CalcWindowRect( pe1 );
	CRect	rc2 = CalcWindowRect( pe2 );

	if( !m_bHorizontal )
		return rc1.Width();
	else
		return rc1.Width() + rc2.Width();
}

long CSplitterCompare::get_height()
{
	XSplitterEntry *pe1 = GetEntry( 0 );
	XSplitterEntry *pe2 = GetEntry( 1 );

	CRect	rc1 = CalcWindowRect( pe1 );
	CRect	rc2 = CalcWindowRect( pe2 );

	if( !m_bHorizontal )
		return rc1.Height() + rc2.Height();
	else
		return rc1.Height();
}

long CSplitterCompare::get_row_height(long lRow)
{
	XSplitterEntry *pe = 0;
	if( !m_bHorizontal )
		pe = GetEntry( lRow );
	else
		pe = GetEntry( 0 );

	CRect	rc = CalcWindowRect( pe );

	return rc.Height();
}

long CSplitterCompare::get_col_width(long lCol)
{
	XSplitterEntry *pe = 0;
	if( !m_bHorizontal )
		pe = GetEntry( 0 );
	else
		pe = GetEntry( lCol );

	CRect	rc = CalcWindowRect( pe );

	return rc.Width();
}

void CSplitterCompare::set_row_height(long lRow, long lHeight)
{
	if( !IsSplitted() )
		return;

	if( !m_bHorizontal )
	{
		if( lRow != GetRowsCount() - 1 )
		{
			CRect rc;
			GetClientRect( &rc );
			if(rc.Height()>0)
				m_fposY = ( lHeight + GetSystemMetrics(SM_CYVSCROLL) + GetSystemMetrics(SM_CYSIZEFRAME) )/ float( rc.Height() );
		}
		else
		{
			CRect rc;
			GetClientRect( &rc );
			if(rc.Height()>0)
				m_fposY = ( rc.Height() - lHeight - GetSystemMetrics(SM_CYVSCROLL) - GetSystemMetrics(SM_CYSIZEFRAME) )/ float( rc.Height() );
		}
	}

	DoRecalcLayout();
}

void CSplitterCompare::set_col_width(long lCol, long lWidth)
{
	if( !IsSplitted() )
		return;

	if( m_bHorizontal )
	{
		if( lCol != GetColsCount() - 1 )
		{
			CRect rc;
			GetClientRect( &rc );
			if(rc.Width()>0)
				m_fposX = ( lWidth + GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXSIZEFRAME) )/ float( rc.Width() );
		}
		else
		{
			CRect rc;
			GetClientRect( &rc );
			if(rc.Width()>0)
				m_fposX = ( rc.Width() - lWidth - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXSIZEFRAME) )/ float( rc.Width() );
		}
	}

	DoRecalcLayout();
}

bool CSplitterCompare::DoSplit(int nRowCount, int nColCount)
{
	if( nRowCount == nColCount && ( nColCount == 0 || nColCount == 2 ) )
		return false;

	return CSplitterStandard::DoSplit( nRowCount, nColCount );
}

// vanek -022.09.2003
bool CSplitterCompare::IsEnabledMenuButton( )
{
    return ( 1L == ::GetValueInt(GetAppUnknown(), "\\SplitterCompare", "EnableMenuButton", 1 ) );
}

