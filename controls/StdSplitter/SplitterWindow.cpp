// SplitterWindow.cpp : implementation file
//

#include "stdafx.h"
#include "StdSplitter.h"
#include "SplitterWindow.h"
#include "SplitterCompare.h"
#include "ScriptNotifyInt.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CSplitterStandard

IMPLEMENT_DYNCREATE(CSplitterStandard, CWnd)

// {A8902372-27AE-11d3-A5DC-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CSplitterStandard, "StdSplitter.SplitterWindow", 
0xa8902372, 0x27ae, 0x11d3, 0xa5, 0xdc, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);


CSplitterStandard::CSplitterStandard()
{
	_OleLockApp( this );

	EnableAggregation();
	EnableAutomation();

	m_fposX = 0;
	m_fposY = 0;
	m_dragMode = dmNone;

	m_sizeMin.cx = GetSystemMetrics( SM_CXVSCROLL )+GetSystemMetrics( SM_CXHSCROLL )*4;
	m_sizeMin.cy = GetSystemMetrics( SM_CYHSCROLL )+GetSystemMetrics( SM_CYVSCROLL )*4;

	m_hCursorVERT = AfxGetApp()->LoadCursor( IDC_VERT );
	m_hCursorHORZ = AfxGetApp()->LoadCursor( IDC_HORZ );
	m_hCursorBOTH = AfxGetApp()->LoadCursor( IDC_BOTH );
	m_bMergeSmall = true;
	m_sName = "SplitterStandard";

	// vanek - 23.09.2003
	m_nEnabledMenuButton = -1;	// no set
	m_bButCreate=false;
}

CSplitterStandard::~CSplitterStandard()
{
	::DestroyCursor( m_hCursorVERT ); 
	::DestroyCursor( m_hCursorHORZ ); 
	::DestroyCursor( m_hCursorBOTH ); 

	POSITION	pos = m_bitmaps.GetStartPosition();

	while( pos )
	{
		HBITMAP hBitmap;
		UINT	nID;

		m_bitmaps.GetNextAssoc( pos, nID, hBitmap );

		if( hBitmap )
			::DeleteObject( hBitmap );

	}

	_OleUnlockApp( this );
}


BEGIN_MESSAGE_MAP(CSplitterStandard, CSplitterBase)
	//{{AFX_MSG_MAP(CSplitterStandard)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(108, 128, OnButton)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CSplitterStandard, CSplitterBase)
END_INTERFACE_MAP()


bool CSplitterStandard::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	if( !CSplitterBase::DoCreate( dwStyle, rc, pparent, nID ) )
		return false;

	if( !CreateView( 0, 0 ) )
		return false;

	CWnd	*pwnd = GetWindowFromUnknown( GetViewEntry( 0, 0 )->punkView );
	if( pwnd->GetSafeHwnd() && pwnd->GetParent() )pwnd->GetParent()->SetFocus();

	//GetWindowFromUnknown( GetViewEntry( 0, 0 )->punkView )->GetParent()->SetFocus();
	return DoRecalcLayout();
}

bool CSplitterStandard::DoRecalcLayout()
{
	
	CRect	rcFirstPane;
	CRect	rcRightPane;
	CRect	rcBottomPane;

	int	nStartRow = 0, nFinishRow = GetRowsCount();
	int	nStartCol = 0, nFinishCol = GetColsCount();
	int	nColStep = 1, nRowStep = 1;

	XSplitterEntry	*pe = GetViewEntry( 0, 0 );

	if( !pe )
		return false;

	CRect	rc = CalcWindowRect( pe );

	if( rc.right > pe->m_rc.right )
	{
		nStartCol = GetColsCount()-1;
		nFinishCol = -1;
		nColStep = -1;
	}

	if( rc.bottom > pe->m_rc.bottom )
	{
		nStartRow = GetRowsCount()-1;
		nFinishRow = -1;
		nRowStep = -1;
	}

	HDWP hdwp = ::BeginDeferWindowPos( 40 );
	try
	{
	for( int nRow = nStartRow; nRow != nFinishRow; nRow+=nRowStep )
		for( int nCol = nStartCol; nCol != nFinishCol; nCol+=nColStep )
		{
			CRect	rcPane;

			XSplitterEntry	*pe = GetViewEntry( nRow, nCol );

			if( !pe )
			{
				::EndDeferWindowPos( hdwp );
				return false;
			}

			rcPane = CalcWindowRect( pe );

			if( nCol == 0 && nRow == 0 )
				rcFirstPane = rcPane;
			if( nCol == 1 && nRow == 0 )
				rcRightPane = rcPane;
			if( nCol == 0 && nRow == 1 )
				rcBottomPane = rcPane;
	
			pe->m_rc = rcPane;
			LayoutWindow( pe, rcPane, hdwp );
		}
	}
	catch(...)
	{
	}
	::EndDeferWindowPos( hdwp );

	//invalidate splitter
	CRect	rcClient;
	GetClientRect( rcClient );
//left rectangle
	rc = rcClient;
	rc.right = rcClient.left + cxLeft;
	InvalidateRect( rc );
//right rectangle
	rc.right = rcClient.right;
	rc.left = rcClient.right - cxRight;
	InvalidateRect( rc );
//top rectangle
	rc = rcClient;
	rc.bottom = rcClient.top + cyTop;
	InvalidateRect( rc );
//bottom rectangle
	rc.top = rcClient.bottom - cyBottom - ::GetSystemMetrics( SM_CYHSCROLL );
	rc.bottom = rcClient.bottom;
	InvalidateRect( rc );

	if( GetColsCount() > 1 )
	{
		rc = rcClient;
		rc.left = rcFirstPane.right;
		rc.right = rcRightPane.left;
		InvalidateRect( rc );
	}

	if( GetRowsCount() > 1 )
	{
		rc = rcClient;
		rc.top = rcFirstPane.bottom;
		rc.bottom = rcBottomPane.top;
		InvalidateRect( rc );
	}


	AfterRecalcLayout();	
	

	return true;
}

CRect CSplitterStandard::CalcWindowRect( XSplitterEntry *pe )
{
	if( !pe )
		return NORECT;

	int nRow = pe->nRow;
	int nCol = pe->nCol;

	CRect	rcClient;
	CRect	rcPane;

	GetClientRect( &rcClient );

	rcPane.left = int( nCol*m_fposX*rcClient.Width() );
	rcPane.right = int( (nCol == (GetColsCount()-1))?rcClient.Width():(m_fposX*rcClient.Width()) );
	rcPane.top = int( nRow*m_fposY*rcClient.Height() );
	rcPane.bottom = int( (nRow == (GetRowsCount()-1))?rcClient.Height():(m_fposY*rcClient.Height()) );

	rcPane.InflateRect( -cxLeft, -cyTop, -cxRight, -cyBottom );

	if( GetColsCount() > 1 )
		if( nCol  == 0 )
			rcPane.right -= cxSplitter;
		else
			rcPane.left += cxSplitter;

	if( GetRowsCount() > 1 )
		if( nRow  == 0 )
			rcPane.bottom -= cySplitter;
		else
			rcPane.top += cySplitter;

	return rcPane;
}

void CSplitterStandard::LayoutWindow( XSplitterEntry *pe, CRect rc, HDWP &hdwp )
{
	int	cxScroll = ::GetSystemMetrics( SM_CXVSCROLL );
	int	cyScroll = ::GetSystemMetrics( SM_CYHSCROLL );
	int	cxButton = ::GetSystemMetrics( SM_CXHSCROLL );
	int	cyButton = ::GetSystemMetrics( SM_CYVSCROLL );

	bool bSplittedCol = GetColsCount() == 2;
	bool bSplittedRow = GetRowsCount() == 2;

	CRect	rcWindow;
	HWND hwnd = ::GetWindowFromUnknown( pe->punkView )->GetSafeHwnd();
	hwnd = ::GetParent( hwnd );

	bool	bHide = rc.Width() < cxScroll ||
					rc.Height() < cyScroll;

	int	nShowCmd = bHide?SW_HIDE:SW_SHOW;

	pe->m_pscrollVert->ShowWindow( nShowCmd );
	pe->m_pscrollHorz->ShowWindow( nShowCmd );

	if(bSplittedCol)
	{
		pe->m_pwnd1->ShowWindow(SW_HIDE);
		pe->m_pwnd2->ShowWindow(nShowCmd);
	}
	else
	{
		pe->m_pwnd1->ShowWindow( nShowCmd );
		pe->m_pwnd2->ShowWindow(SW_HIDE);
	}

	if(bSplittedRow)
	{
		pe->m_pwnd3->ShowWindow(SW_HIDE);
		pe->m_pwnd4->ShowWindow(nShowCmd);
	}
	else
	{
		pe->m_pwnd3->ShowWindow(nShowCmd);
		pe->m_pwnd4->ShowWindow(SW_HIDE);
	}

	pe->m_pwnd5->ShowWindow( nShowCmd );

	if( hwnd )
		CWnd::FromHandle( hwnd )->ShowWindow( nShowCmd );

	if( bHide )
		return;

//place the pane
	rcWindow = rc;
	rcWindow.InflateRect( 0, 0, -cxScroll, -cyScroll );
//the "Menu" button
	CRect	rcButtonM = rc;
	rcButtonM.left = rcButtonM.right - cxScroll;
	rcButtonM.top = rcButtonM.bottom - cyScroll;
//the "SplitH" button
	CRect	rcButtonSH = rc;
	rcButtonSH.left = rcButtonM.left - cxButton;
	rcButtonSH.right = rcButtonM.left;
	rcButtonSH.top = rcButtonSH.bottom - cyScroll;
//the "SoSplitH" button
	CRect	rcButtonNSH = rc;
	rcButtonNSH.left = rcButtonM.left - cxButton;
	rcButtonNSH.top = rcButtonNSH.bottom - cyScroll;
	rcButtonNSH.right = rcButtonM.left;
//the horz scrollbar 
	CRect	rcScrollH = rc;
	rcScrollH.right = rcButtonSH.left;
	rcScrollH.top = rcButtonSH.bottom - cyScroll;
//the "SplitV" button
	CRect	rcButtonSV = rc;
	rcButtonSV.left = rcButtonSV.right - cxScroll;
	rcButtonSV.top = rcButtonM.top - cyButton;
	rcButtonSV.bottom = rcButtonM.top;
//the "NoSplitV" buttom
	CRect	rcButtonNSV = rc;
	rcButtonNSV.left = rcButtonNSV.right - cxScroll;
	rcButtonNSV.top = rcButtonM.top - cyButton;
	rcButtonNSV.bottom = rcButtonM.top;
//the vert scrollbar 
	CRect	rcScrollV = rc;
	rcScrollV.left = rcScrollV.right - cxScroll;
	//rcScrollV.top = rcButtonSV.bottom;
	rcScrollV.bottom = rcButtonSV.top;

	CRect	rcOldScrollVert;
	pe->m_pscrollVert->GetWindowRect( rcOldScrollVert );
	ScreenToClient( rcOldScrollVert );
	CRect	rcOldScrollHorz;
	pe->m_pscrollVert->GetWindowRect( rcOldScrollHorz );
	ScreenToClient( rcOldScrollHorz );

	DWORD	dwFlags = SWP_NOACTIVATE|SWP_NOZORDER;

	bool	bFirstlyMoveVertSB = rcOldScrollVert.CenterPoint().x > rcScrollV.CenterPoint().x;
	bool	bFirstlyMoveHorzSB = rcOldScrollHorz.CenterPoint().x > rcScrollH.CenterPoint().x;
	
	if( bFirstlyMoveVertSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollVert, HWND_NOTOPMOST, rcScrollV.left, rcScrollV.top, rcScrollV.Width(), rcScrollV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, (bSplittedRow ? *pe->m_pwnd4 : *pe->m_pwnd3), HWND_NOTOPMOST, rcButtonSV.left, rcButtonSV.top, rcButtonSV.Width(), rcButtonSV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd5, HWND_NOTOPMOST, rcButtonM.left, rcButtonM.top, rcButtonM.Width(), rcButtonM.Height(), dwFlags );
	}

	if( bFirstlyMoveHorzSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollHorz, HWND_NOTOPMOST, rcScrollH.left, rcScrollH.top, rcScrollH.Width(), rcScrollH.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, (bSplittedCol ? *pe->m_pwnd2 : *pe->m_pwnd1), HWND_NOTOPMOST, rcButtonSH.left, rcButtonSH.top, rcButtonSH.Width(), rcButtonSH.Height(), dwFlags );
	}

	hdwp = ::DeferWindowPos( hdwp, hwnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), dwFlags );

	if( !bFirstlyMoveVertSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollVert, HWND_NOTOPMOST, rcScrollV.left, rcScrollV.top, rcScrollV.Width(), rcScrollV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, (bSplittedRow ? *pe->m_pwnd4 : *pe->m_pwnd3), HWND_NOTOPMOST, rcButtonSV.left, rcButtonSV.top, rcButtonSV.Width(), rcButtonSV.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pwnd5, HWND_NOTOPMOST, rcButtonM.left, rcButtonM.top, rcButtonM.Width(), rcButtonM.Height(), dwFlags );
	}

	if( !bFirstlyMoveHorzSB )
	{
		hdwp = ::DeferWindowPos( hdwp, *pe->m_pscrollHorz, HWND_NOTOPMOST, rcScrollH.left, rcScrollH.top, rcScrollH.Width(), rcScrollH.Height(), dwFlags );
		hdwp = ::DeferWindowPos( hdwp, (bSplittedCol ? *pe->m_pwnd2 : *pe->m_pwnd1), HWND_NOTOPMOST, rcButtonSH.left, rcButtonSH.top, rcButtonSH.Width(), rcButtonSH.Height(), dwFlags );
	}

	CRect	rcClient;
	GetClientRect( &rcClient );

	/*if( GetColsCount() == 1 )
		pe->m_pwnd1->EnableWindow( rcClient.Width() > m_sizeMin.cx*2 );
	if( GetRowsCount() == 1 )
		pe->m_pwnd3->EnableWindow( rcClient.Height() > m_sizeMin.cy*2 );
	*/
		
		
		
		


	/*pe->m_pscrollHorz->MoveWindow( rcScrollH );
	pe->m_pscrollVert->MoveWindow( rcScrollV );
	pe->m_pwnd1->MoveWindow( rcButtonSH );
	pe->m_pwnd2->MoveWindow( rcButtonNSH );
	pe->m_pwnd3->MoveWindow( rcButtonSV );
	pe->m_pwnd4->MoveWindow( rcButtonNSV );
	pe->m_pwnd5->MoveWindow( rcButtonM );
	CWnd::FromHandle( hwnd )->MoveWindow( rcWindow );*/
}	
	
void CSplitterStandard::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

		CRect	rcClient;
	GetClientRect( rcClient );
	dc.FillRect( rcClient, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );

	POSITION	pos = GetFirstViewEntryPosition();

	while( pos )
	{
		XSplitterEntry *pe = GetNextViewEntry( pos );

		CRect rcPane = CalcWindowRect( pe );
		rcPane.InflateRect( 2, 2 );

		dc.DrawEdge( rcPane, BDR_SUNKENOUTER|BDR_SUNKENINNER, BF_RECT );

		COLORREF	crStart, crEnd;
		if( pe == m_pveActiveView )
		{
			crStart = RGB( 255, 0, 0 );
			crEnd = RGB( 255, 0, 0 );
			//dc.Draw3dRect( rcPane, crStart, crEnd );	
			rcPane.InflateRect( -1, -1 );
			dc.Draw3dRect( rcPane, crStart, crEnd );	
		}
	}

}

BOOL CSplitterStandard::OnEraseBkgnd(CDC* pDC) 
{
	return true;	
}

HBITMAP _MakeBitmapButton( CWnd *pwnd, UINT nID, CMap<UINT, UINT, HBITMAP, HBITMAP&>* pBitmaps )
{
	HBITMAP	hbm;

	bool bFoundExist = false;

	if( pBitmaps )
	{
		HBITMAP hBitmapExist = NULL;
		if( pBitmaps->Lookup( nID, hBitmapExist ) && hBitmapExist )
		{
			bFoundExist = true;
			hbm = hBitmapExist;
		}

	}

	if( !bFoundExist )
	{
		// vanek: replace flag LR_LOADMAP3DCOLORS with LR_LOADTRANSPARENT - 22.09.2003
		hbm = (HBITMAP)::LoadImage( 
			AfxGetResourceHandle(), MAKEINTRESOURCE(nID), 
			IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADTRANSPARENT); 
		
		if( pBitmaps )
		{
			pBitmaps->SetAt( nID, hbm);
		}

	}

	if( !hbm )
		return NULL;	

	((CButton*)pwnd)->SetBitmap( hbm );
	
	return hbm;
}

void CSplitterStandard::OnCreateView( XSplitterEntry *pe )
{
	pe->m_pscrollHorz = new CScrollBar();
	pe->m_pscrollVert = new CScrollBar();
	pe->m_pwnd1 = new CButton();
	pe->m_pwnd2 = new CButton();
	pe->m_pwnd3 = new CButton();
	pe->m_pwnd4 = new CButton();
	pe->m_pwnd5 = new CButton();

	CString	strScrollClass = GetValueString( GetAppUnknown(), "\\Interface", "ScrollClass", "SCROLLBAR" );
	pe->m_pscrollHorz->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_HORZ, NORECT, this, 100+pe->nRow*2+pe->nCol );
	pe->m_pscrollVert->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_VERT, NORECT, this, 104+pe->nRow*2+pe->nCol );

    ((CButton*)pe->m_pwnd1)->Create( "SplitH", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 108+pe->nRow*2+pe->nCol );
	((CButton*)pe->m_pwnd2)->Create( "NoSplitH", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 112+pe->nRow*2+pe->nCol );
	((CButton*)pe->m_pwnd3)->Create( "SplitV", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 114+pe->nRow*2+pe->nCol );
	((CButton*)pe->m_pwnd4)->Create( "NoSplitV", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 116+pe->nRow*2+pe->nCol );
	
	// vanek - 23.09.2003
	if( m_nEnabledMenuButton == -1 )
		m_nEnabledMenuButton = IsEnabledMenuButton();

	((CButton*)pe->m_pwnd5)->Create( "Menu", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP|
							(m_nEnabledMenuButton ? 0 : WS_DISABLED), NORECT, this, 118+pe->nRow*2+pe->nCol );

	pe->m_pscrollHorz->EnableScrollBar(ESB_DISABLE_BOTH);
	pe->m_pscrollVert->EnableScrollBar(ESB_DISABLE_BOTH);
	
	_ValidatePane( pe );
}

static void FireScriptEvent(LPCTSTR lpszEventName, int nParams, VARIANT *pvarParams)
{
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS == 0) return;
	_bstr_t bstrEvent("Splitter_");
	bstrEvent += lpszEventName;
	sptrSS->Invoke(bstrEvent, pvarParams, nParams, 0, fwFormScript);
	sptrSS->Invoke(bstrEvent, pvarParams, nParams, 0, fwAppScript);
}

void CSplitterStandard::OnButton( UINT nID )
{
	XSplitterEntry	*pe = GetViewEntry( nID );

	if( !pe )
		return;

	::GetWindowFromUnknown( pe->punkView )->GetParent()->SetFocus();

	if( nID == pe->m_pwnd1->GetDlgCtrlID() )
	{
		//Split horz
		SplitCol();
		_variant_t var((long)0);
		FireScriptEvent(_T("OnSplit"), 1, &var);
	}
	else
	if( nID == pe->m_pwnd2->GetDlgCtrlID() )
	{
		//no split horz
		MergeCol( 1-pe->nCol );
		_variant_t var((long)0);
		FireScriptEvent(_T("OnUnSplit"), 1, &var);
	}
	else
	if( nID == pe->m_pwnd3->GetDlgCtrlID() )
	{
		//Split vert
		SplitRow();
		_variant_t var((long)1);
		FireScriptEvent(_T("OnSplit"), 1, &var);
	}
	else
	if( nID == pe->m_pwnd4->GetDlgCtrlID() )
	{
		//no Split vert
		MergeRow( 1-pe->nRow );
		_variant_t var((long)1);
		FireScriptEvent(_T("OnUnSplit"), 1, &var);
	}
	else
	if( nID == pe->m_pwnd5->GetDlgCtrlID() )
	{
		//menu
		CRect	rcWindow;
		pe->m_pwnd5->GetWindowRect( rcWindow );

		CPoint	ptMenuPos;
		DWORD	dwFlags = 0;

		/*if( pe->nCol == 0 )
		{
			ptMenuPos.x = rcWindow.left;
			dwFlags = TPM_LEFTALIGN|TPM_BOTTOMALIGN;
		}
		else*/
		{
			ptMenuPos.x = rcWindow.right;
			dwFlags = TPM_RIGHTALIGN|TPM_BOTTOMALIGN;
		}

		ptMenuPos.y = rcWindow.top;

		ExecuteMenu( ptMenuPos, dwFlags );
	}
}

bool CSplitterStandard::MergeCol( int nColToDelete )
{
	if( GetColsCount() == 1 )
		return false;

	for( int nRow = 0; nRow < GetRowsCount(); nRow++ )
		DeleteView( nRow, nColToDelete );

	m_fposX = 0;

	_ValidateControls();

	return true;
}


bool CSplitterStandard::MergeRow( int nRowToDelete )
{
	if( GetRowsCount() == 1 )
		return false;

	for( int nCol = 0; nCol < GetColsCount(); nCol++ )
		DeleteView( nRowToDelete, nCol  );

	m_fposY = 0;

	_ValidateControls();
	
	return true;
}

bool CSplitterStandard::SplitCol()
{
	if( GetColsCount() == 2 )
		return false;

	m_fposX = .5;

	for( int nRow = 0; nRow < GetRowsCount(); nRow++ )
	{
		XSplitterEntry	*pe = GetViewEntry( 0, nRow );
		ASSERT( pe );
		CreateView( nRow, 1, pe->bstrViewRuntime );
	}

	_ValidateControls();

	AfterSplitCol();

	return true;
}

bool CSplitterStandard::SplitRow()
{
	if( GetRowsCount() == 2 )
		return false;	

	m_fposY = .5;

	for( int nCol = 0; nCol < GetColsCount(); nCol++ )
	{
		XSplitterEntry	*pe = GetViewEntry( nCol, 0 );
		ASSERT( pe );
		CreateView( 1, nCol, pe->bstrViewRuntime );
	}

	_ValidateControls();

	AfterSplitRow();

	return true;
}

void CSplitterStandard::_ValidateControls()
{
	POSITION	pos = GetFirstViewEntryPosition();

	if( m_pveActiveView )
	{
		_ValidatePane( m_pveActiveView );
		//GetWindowFromUnknown( m_pveActiveView->punkView )->GetParent()->SetFocus();	
		//GetWindowFromUnknown( m_pveActiveView->punkView )->SetFocus();	
	}

	while( pos )
	{
		XSplitterEntry *pe = GetNextViewEntry( pos );
		ASSERT( pe );
		if(!m_pveActiveView )
		{
			m_pveActiveView = pe;
			_ValidatePane( m_pveActiveView );
			//GetWindowFromUnknown( m_pveActiveView->punkView )->GetParent()->SetFocus();	
		}

	
		_ValidatePane( pe );
	}

	
	DoRecalcLayout();
}

void CSplitterStandard::_ValidatePane( XSplitterEntry *pe )
{
	if( !pe )
		return;

	pe->nCol = min( pe->nCol, GetColsCount()-1 );
	pe->nRow = min( pe->nRow, GetRowsCount()-1 );

	pe->m_pscrollHorz->SetDlgCtrlID( 100+pe->nRow*2+pe->nCol );
	pe->m_pscrollVert->SetDlgCtrlID( 104+pe->nRow*2+pe->nCol );
	pe->m_pwnd1->SetDlgCtrlID( 108+pe->nRow*2+pe->nCol );
	pe->m_pwnd2->SetDlgCtrlID( 112+pe->nRow*2+pe->nCol );
	pe->m_pwnd3->SetDlgCtrlID( 116+pe->nRow*2+pe->nCol );
	pe->m_pwnd4->SetDlgCtrlID( 120+pe->nRow*2+pe->nCol );
	pe->m_pwnd5->SetDlgCtrlID( 124+pe->nRow*2+pe->nCol );

	bool bActive = pe == m_pveActiveView;

	UINT	nSplitH =  bActive ? IDB_SPLITH:IDB_SPLITH_DK;
	UINT	nMergeH1 =  bActive ? IDB_MERGEH1:IDB_MERGEH1_DK;
	UINT	nMergeH2 =  bActive ? IDB_MERGEH2:IDB_MERGEH2_DK;
	UINT	nSplitV =  bActive ? IDB_SPLITV:IDB_SPLITV_DK;
	UINT	nMergeV1 =  bActive ? IDB_MERGEV1:IDB_MERGEV1_DK;
	UINT	nMergeV2 =  bActive ? IDB_MERGEV2:IDB_MERGEV2_DK;
	UINT	nMenu =  bActive ? IDB_MENU:IDB_MENU_DK;

	

	_MakeBitmapButton( pe->m_pwnd1, nSplitH, &m_bitmaps );
	_MakeBitmapButton( pe->m_pwnd2, nMergeH1, &m_bitmaps );
	_MakeBitmapButton( pe->m_pwnd3, nSplitV, &m_bitmaps );
	_MakeBitmapButton( pe->m_pwnd4, nMergeV1, &m_bitmaps );
	_MakeBitmapButton( pe->m_pwnd5, nMenu, &m_bitmaps );

	if(m_bButCreate)
	{
		pe->m_pwnd1->EnableWindow(FALSE);
		pe->m_pwnd2->EnableWindow(FALSE);
		pe->m_pwnd3->EnableWindow(FALSE);
		pe->m_pwnd4->EnableWindow(FALSE);
		pe->m_pwnd5->EnableWindow(FALSE);
	}
	else
	{
		pe->m_pwnd2->EnableWindow( GetColsCount() > 1 && (pe->m_wEnableMask&2) == 2 );
		pe->m_pwnd4->EnableWindow( GetRowsCount() > 1 && (pe->m_wEnableMask&8) == 8 );
		pe->m_pwnd1->EnableWindow( GetColsCount() == 1 && (pe->m_wEnableMask&1) == 1 );
		pe->m_pwnd3->EnableWindow( GetRowsCount() == 1 && (pe->m_wEnableMask&4) == 4 );
	}

}

void CSplitterStandard::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dragMode = HitTest( point );

	if( m_dragMode != dmNone )
	{
		SetCapture();
	}
	
	CSplitterBase::OnLButtonDown(nFlags, point);
}

void CSplitterStandard::OnLButtonUp(UINT nFlags, CPoint point) 
{
	DoCancelMode();

	CRect	rcClient;
	GetClientRect( rcClient );

	if( m_bMergeSmall )
	{
		int nRowCount = GetRowCount();
		int nColsCount = GetColsCount();
		if( m_fposX*rcClient.Width() < m_sizeMin.cx )
			MergeCol( 0 );
		if( (1-m_fposX)*rcClient.Width() < m_sizeMin.cx )
			MergeCol( 1 );
		if( m_fposY*rcClient.Height() < m_sizeMin.cy )
			MergeRow( 0 );
		if( (1-m_fposY)*rcClient.Height() < m_sizeMin.cy )
			MergeRow( 1 );
		bool bRowsChanged = nRowCount != GetRowCount();
		bool bColsChanged = nColsCount != GetColsCount();
		if (bRowsChanged || bColsChanged)
		{
			_variant_t var((long)(bRowsChanged?bColsChanged?2:1:0));
			FireScriptEvent(_T("OnUnSplit"), 1, &var);
		}
	}
		
	CSplitterBase::OnLButtonUp(nFlags, point);
}

void CSplitterStandard::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	DragMode	mode = HitTest( point );

	if( mode & dmHorz )
		MergeRow( 1 );
	if( mode & dmVert )
		MergeCol( 1 );
}
void CSplitterStandard::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_dragMode != dmNone )
	{
		CRect	rc;

		GetClientRect( rc );

		if( m_dragMode & dmHorz )m_fposX = ((double)point.x-rc.left)/rc.Width();
		if( m_dragMode & dmVert )m_fposY = ((double)point.y-rc.top)/rc.Height();

		if( m_bMergeSmall )
		{
			if( m_dragMode & dmHorz )
			{
				if( point.x - rc.left < m_sizeMin.cx )m_fposX = 0.001;
				if( rc.right - point.x < m_sizeMin.cx )m_fposX = 0.999;
			}

			if( m_dragMode & dmVert )
			{
				if( point.y - rc.top < m_sizeMin.cy )m_fposY = 0.001;
				if( rc.bottom - point.y < m_sizeMin.cy )m_fposY = 0.999;
			}
		}
		else
		{
			double	fMinCX = ((double)m_sizeMin.cx-rc.left)/rc.Width();
			double	fMinCY = ((double)m_sizeMin.cy-rc.top)/rc.Height();
			if( m_dragMode & dmHorz )m_fposX = max( fMinCX, min( 1-fMinCX, m_fposX ) );
			if( m_dragMode & dmVert )m_fposY = max( fMinCY, min( 1-fMinCY, m_fposY ) );
		}


		DoRecalcLayout();
	}								
	CSplitterBase::OnMouseMove(nFlags, point);
}

CSplitterStandard::DragMode 
	CSplitterStandard::HitTest( CPoint pt )
{
	DragMode	mode = dmNone;
	if( GetColsCount() == 2 )
	{
		XSplitterEntry *pe1 = GetViewEntry( 0, 0 );
		XSplitterEntry *pe2 = GetViewEntry( 0, 1 );

		CRect	rc1 = CalcWindowRect( pe1 );
		CRect	rc2 = CalcWindowRect( pe2 );

		if( rc1.right < pt.x && rc2.left > pt.x )
			mode = DragMode(mode|dmHorz);
	}

	if( GetRowsCount() == 2 )
	{
		XSplitterEntry *pe1 = GetViewEntry( 0, 0 );
		XSplitterEntry *pe2 = GetViewEntry( 1, 0 );

		CRect	rc1 = CalcWindowRect( pe1 );
		CRect	rc2 = CalcWindowRect( pe2 );

		if( rc1.bottom < pt.y && rc2.top > pt.y )
			mode = DragMode(mode|dmVert);
	}

	return mode;
}

BOOL CSplitterStandard::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint	pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );


	DragMode	mode = HitTest( pt );

	if( mode == dmVert )
		::SetCursor( m_hCursorVERT );
	else if( mode == dmHorz )
		::SetCursor( m_hCursorHORZ );
	else if( mode == dmBoth )
		::SetCursor( m_hCursorBOTH );
	else 
		return CSplitterBase::OnSetCursor(pWnd, nHitTest, message);
	return true;
}

void CSplitterStandard::OnCancelMode() 
{
	CSplitterBase::OnCancelMode();
	
	DoCancelMode();
}

bool CSplitterStandard::DoCancelMode()
{
	if( m_dragMode != dmNone )
	{
		ReleaseCapture();
		m_dragMode = dmNone;
	}
	return true;
}

// vanek - 22.09.2003
bool CSplitterStandard::IsEnabledMenuButton( )
{
	return ( 1L == ::GetValueInt(GetAppUnknown(), "\\SplitterWindow", "EnableMenuButton", 1 ) );
}

void CSplitterStandard::OnActivateView( XSplitterEntry *pe, XSplitterEntry *peOld )
{
	CRect	rcPane, rc;

	int	delta = 4;
	int	delta1 = 0;

	if( pe )
	{
		rcPane = CalcWindowRect( pe );
	//rect1	
		rc = rcPane;
		rc.right = rcPane.left+delta1;
		rc.left = rcPane.left-delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect2
		rc = rcPane;
		rc.left = rcPane.right-delta1;
		rc.right = rcPane.right+delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect3
		rc = rcPane;
		rc.top = rcPane.bottom-delta1;
		rc.bottom = rcPane.bottom+delta;
		InvalidateRect( rc );
	//rect4
		rc = rcPane;
		rc.top = rcPane.top-delta;
		rc.bottom = rcPane.top+delta1;
		InvalidateRect( rc );

		_ValidatePane( pe );
	}

	if( peOld )
	{
		rcPane = CalcWindowRect( peOld );
	//rect1	
		rc = rcPane;
		rc.right = rcPane.left+delta1;
		rc.left = rcPane.left-delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect2
		rc = rcPane;
		rc.left = rcPane.right-delta1;
		rc.right = rcPane.right+delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect3
		rc = rcPane;
		rc.top = rcPane.bottom-delta1;
		rc.bottom = rcPane.bottom+delta;
		InvalidateRect( rc );
	//rect4
		rc = rcPane;
		rc.top = rcPane.top-delta;
		rc.bottom = rcPane.top-delta1;
		InvalidateRect( rc );

		_ValidatePane( peOld );
	}

	/*POSITION pos = GetFirstViewEntryPosition();

	while( pos )
	{
		XSplitterEntry	*pse = GetNextViewEntry( pos );

		rcPane = CalcWindowRect( pse );
	//rect1	
		rc = rcPane;
		rc.right = rcPane.left+delta1;
		rc.left = rcPane.left-delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect2
		rc = rcPane;
		rc.left = rcPane.right-delta1;
		rc.right = rcPane.right+delta;
		rc.top -= delta;
		rc.bottom += delta;
		InvalidateRect( rc );
	//rect3
		rc = rcPane;
		rc.top = rcPane.bottom-delta1;
		rc.bottom = rcPane.bottom+delta;
		InvalidateRect( rc );
	//rect4
		rc = rcPane;
		rc.top = rcPane.top-delta;
		rc.bottom = rcPane.top-delta1;
		InvalidateRect( rc );

		_ValidatePane( pse );
	}*/


	//GetWindowFromUnknown( pe->punkView )->GetParent()->SetFocus();	

}

void CSplitterStandard::OnSize(UINT nType, int cx, int cy) 
{
	if(cx != 0 && cy != 0)
	{
		if( m_fposX > 0 )m_fposX = max( (double)m_sizeMin.cx/cx, min( 1-(double)m_sizeMin.cx/cx, m_fposX ) );
		if( m_fposY > 0 )m_fposY = max( (double)m_sizeMin.cy/cy, min( 1-(double)m_sizeMin.cy/cy, m_fposY ) );
	}
	
	CSplitterBase::OnSize(nType, cx, cy);
}

void CSplitterStandard::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = m_sizeMin.cx * GetColsCount();
	lpMMI->ptMinTrackSize.y = m_sizeMin.cy * GetRowsCount();
	
	CSplitterBase::OnGetMinMaxInfo(lpMMI);
}

void CSplitterStandard::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	//CSplitterBase::OnClose();
}


BEGIN_DISPATCH_MAP(CSplitterStandard, CSplitterBase)
	//{{AFX_DISPATCH_MAP(CSplitterStandard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		DISP_PROPERTY_EX(CSplitterStandard, "ActiveView", GetActiveView, SetActiveView, VT_DISPATCH)
		DISP_FUNCTION(CSplitterStandard, "GetViewType", GetViewTypeDisp, VT_BSTR, VTS_I4 VTS_I4)
		DISP_FUNCTION(CSplitterStandard, "SetViewType", SetViewTypeDisp, VT_EMPTY, VTS_I4 VTS_I4 VTS_BSTR)
		DISP_FUNCTION(CSplitterStandard, "ColsCount", GetColsCountDisp, VT_I4, VTS_NONE)
		DISP_FUNCTION(CSplitterStandard, "RowsCount", GetRowsCountDisp, VT_I4, VTS_NONE)
		DISP_FUNCTION(CSplitterStandard, "EnableButton", EnableButton, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
		DISP_FUNCTION(CSplitterStandard, "BlockButton", BlockButton, VT_EMPTY, VTS_BOOL)
		DISP_PROPERTY_EX(CSplitterStandard, "MergeSmall", GetMergeSmall, SetMergeSmall, VT_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

void CSplitterStandard::ErrorMsg(UINT nErrorID)
{
	CString strCaption;
	strCaption.LoadString(IDS_ERROR_CAPTION);

	CString strErr;
	strErr.LoadString(nErrorID);

	::MessageBox(NULL, strErr, strCaption, MB_OK|MB_ICONERROR|MB_APPLMODAL);
}

BSTR CSplitterStandard::GetViewTypeDisp(long nCol, long nRow)
{
	if(!(nCol >=0 && nCol < GetColsCount() && nRow >=0 && nRow < GetRowsCount()))
	{
		ErrorMsg(IDS_BAD_VIEW_COORD);
		return 0;
	}
	XSplitterEntry *pe = GetViewEntry( nRow, nCol );
	if( !pe )
	{
		ErrorMsg(IDS_BAD_VIEW_COORD);
		return 0;
	}

	CString strVType((char*)pe->bstrViewRuntime);

	return strVType.AllocSysString();
}

void CSplitterStandard::SetViewTypeDisp(long nCol, long nRow, LPCTSTR szType)
{
	if(!(nCol >=0 && nCol < GetColsCount() && nRow >=0 && nRow < GetRowsCount()))
	{
		ErrorMsg(IDS_BAD_VIEW_COORD);
		return;
	}
	ChangeViewType(nRow, nCol, _bstr_t(szType));
}

long CSplitterStandard::GetColsCountDisp()
{
	return GetColsCount();
}

long CSplitterStandard::GetRowsCountDisp()
{
	return GetRowsCount();
}

LPDISPATCH CSplitterStandard::GetActiveView()
{
	IDispatch* pdisp = 0;
	if(m_pveActiveView && m_pveActiveView->punkView)
	{
		m_pveActiveView->punkView->QueryInterface(IID_IDispatch, (void**)&pdisp);
	}
	return pdisp;
}

void CSplitterStandard::SetActiveView(LPDISPATCH pdispView)
{
	IUnknownPtr sptrV(pdispView);
	if(!m_listViews.Find(sptrV.GetInterfacePtr()))
	{
		IViewPtr sptrView(sptrV);
		IUnknown* punkDoc = 0;
		sptrView->GetDocument(&punkDoc);
		IDocumentSitePtr sptrDoc(punkDoc);
		if(punkDoc)
			punkDoc->Release();
		sptrDoc->SetActiveView(sptrView);
	}
	else
	{
		ErrorMsg(IDS_BAD_VIEW_TOACTIVATE);
	}
}

void CSplitterStandard::EnableButton(long lRow, long lCol, long lButton, long lEnable)
{
	if (lRow == -1)
	{
		EnableButton(0, lCol, lButton, lEnable);
		EnableButton(1, lCol, lButton, lEnable);
	}
	else if (lCol == -1)
	{
		EnableButton(lRow, 0, lButton, lEnable);
		EnableButton(lRow, 1, lButton, lEnable);
	}
	else
	{
		XSplitterEntry *pe = GetViewEntry(lRow, lCol);
		if (pe == NULL) return;
		WORD wLeaveMask = pe->m_wEnableMask & ~(WORD)lButton;
		WORD wEnableMask = (WORD)(lEnable&lButton);
		pe->m_wEnableMask = wLeaveMask|wEnableMask;
		if (lButton&1)
			pe->m_pwnd1->EnableWindow(pe->m_wEnableMask&1);
		if (lButton&2)
			pe->m_pwnd2->EnableWindow(pe->m_wEnableMask&2);
		if (lButton&4)
			pe->m_pwnd3->EnableWindow(pe->m_wEnableMask&4);
		if (lButton&8)
			pe->m_pwnd4->EnableWindow(pe->m_wEnableMask&8);
		if (lButton&16)
			pe->m_pwnd5->EnableWindow(pe->m_wEnableMask&16);
	}
}

void CSplitterStandard::BlockButton(bool b_BlockBut)
{
	m_bButCreate=b_BlockBut;
}

BOOL CSplitterStandard::GetMergeSmall()
{
	return m_bMergeSmall?TRUE:FALSE;
}

void CSplitterStandard::SetMergeSmall(BOOL bMergeSmall)
{
	m_bMergeSmall = bMergeSmall?true:false;
}


CRect CSplitterStandard::_CalcWindowRect2(XSplitterEntry *pe)
{
	CRect	rc = CalcWindowRect(pe);
	long nWidth = rc.Width() - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXSIZEFRAME)/2;
	long nHeight = rc.Height() - GetSystemMetrics(SM_CYHSCROLL) - GetSystemMetrics(SM_CYSIZEFRAME)/2;

	ASSERT(nWidth > 0);
	ASSERT(nHeight > 0);

	rc.right = rc.left + nWidth;
	rc.bottom = rc.top + nHeight;

	return rc;
}

void CSplitterStandard::GetPaneSize(XSplitterEntry	*pe, CSize& size)
{
	if(!pe)
		return;
	IScrollZoomSitePtr	sptrSite(pe->punkView);
	if(sptrSite == 0)
		return;

	BOOL bASM = FALSE;
	sptrSite->GetAutoScrollMode(&bASM);
	if(!bASM)
	{
		CRect	rc = _CalcWindowRect2(pe);
		size = CSize(rc.Width(), rc.Height());
		
	}
	else
	{
		double fZoom = 1;
		sptrSite->GetZoom(&fZoom);
		sptrSite->GetClientSize(&size);
		CWnd* pWnd = ::GetWindowFromUnknown(pe->punkView);
		long nDY = 0;
		if(pWnd)
		{
			CRect rcClient1 = NORECT;
			pWnd->GetClientRect(&rcClient1);
			pWnd = pWnd->GetParent();
			if(pWnd)
			{
				CRect rcClient2 = NORECT;
				pWnd->GetClientRect(&rcClient2);
				nDY = rcClient2.bottom - rcClient1.bottom;
			}
		}

		size.cx = (long)(size.cx*fZoom+.5);
		size.cy = (long)(size.cy*fZoom+.5);
		size.cy += nDY;
	}
}

void CSplitterStandard::OptimizeSplitter()
{
	if( CWnd::GetParent()->IsZoomed() )
		return;


	IUnknown	*punkMainWnd = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow );
	sptrIMainWindow	sptrMain( punkMainWnd );
	if( punkMainWnd )
		punkMainWnd->Release();

	
	HWND hWnd = 0;
	CRect rcClientFrame = NORECT;
	sptrMain->GetMDIClientWnd(&hWnd);

	if(hWnd)
	{
		::GetWindowRect(hWnd, &rcClientFrame);
	}

	CRect rcWnd = NORECT;
	CWnd::GetParent()->GetWindowRect(&rcWnd);
	CRect rcWndOrid = rcWnd;
	CRect rcClient = NORECT;
	GetClientRect(&rcClient);

	long nMaxWidth = max(0, rcClientFrame.right - rcWnd.right);
	long nMaxHeight = max(0, rcClientFrame.bottom - rcWnd.bottom);


	CWnd::GetParent()->GetParent()->ScreenToClient(&rcWnd);

	
	
	//ASSERT(nMaxWidth >= 0);
	//ASSERT(nMaxHeight >= 0);

	XSplitterEntry	*pe00 = GetViewEntry( 0, 0 );
	XSplitterEntry	*pe10 = GetViewEntry( 1, 0 );
	XSplitterEntry	*pe01 = GetViewEntry( 0, 1 );
	XSplitterEntry	*pe11 = GetViewEntry( 1, 1 );


	if(!pe00)
		return;

	if(!((pe00 && CheckInterface(pe00->punkView, IID_IImageView)) || 
	     (pe10 && CheckInterface(pe10->punkView, IID_IImageView)) ||
	     (pe01 && CheckInterface(pe01->punkView, IID_IImageView)) ||
	     (pe11 && CheckInterface(pe11->punkView, IID_IImageView))))
		return;
	


	long nDxResize = nMaxWidth;// - rcClient.Width();
	long nDyResize = nMaxHeight;// - rcClient.Height();

	CSize size00(0, 0);
	CSize size10(0, 0);
	CSize size01(0, 0);
	CSize size11(0, 0);

	GetPaneSize(pe00, size00);
	GetPaneSize(pe10, size10);
	GetPaneSize(pe01, size01);
	GetPaneSize(pe11, size11);

	long n0ColWidth = max(size00.cx, size10.cx);
	long n1ColWidth = max(size01.cx, size11.cx);
	
	long n0RowHeight = max(size00.cy, size01.cy);
	long n1RowHeight = max(size10.cy, size11.cy);

	CRect	rc00 = NORECT;
	if(pe00) rc00 = _CalcWindowRect2(pe00);
	CRect	rc10 = NORECT;
	if(pe10) rc10 = _CalcWindowRect2(pe10);
	CRect	rc01 = NORECT;
	if(pe01) rc01 = _CalcWindowRect2(pe01);
	CRect	rc11 = NORECT;
	if(pe11) rc11 = _CalcWindowRect2(pe11);

	long n0ColWidthOrig = max(rc00.Width(), rc10.Width());
	long n1ColWidthOrig = max(rc01.Width(), rc11.Width());
	
	long n0RowHeightOrig = max(rc00.Height(), rc01.Height());
	long n1RowHeightOrig = max(rc10.Height(), rc11.Height());


	long n0ColWidthTotal = n0ColWidthOrig;
	long n1ColWidthTotal = n1ColWidthOrig;
	long n0RowHeightTotal = n0RowHeightOrig;
	long n1RowHeightTotal = n1RowHeightOrig;

	if(n0ColWidth > n0ColWidthOrig)
	{
		if(n1ColWidthOrig>n1ColWidth)
			nDxResize += n1ColWidthOrig-n1ColWidth;
		if(n0ColWidth - n0ColWidthOrig >= nDxResize)
		{
			n0ColWidthTotal = n0ColWidthOrig + nDxResize;
			nDxResize = 0;
		}
		else
		{
			n0ColWidthTotal = n0ColWidth;
			nDxResize -= n0ColWidth - n0ColWidthOrig;
		}
	}
	else
	{
		nDxResize += n0ColWidthOrig - n0ColWidth;
		n0ColWidthTotal = n0ColWidth;
	}
	if(n1ColWidth > n1ColWidthOrig)
	{
		if(nDxResize > 0)
		{
			//long nCol0Dx = nDxResize - nMaxWidth;

			if(n1ColWidth - n1ColWidthOrig >= nDxResize)
				n1ColWidthTotal = n1ColWidthOrig + nDxResize;
			else
				n1ColWidthTotal = n1ColWidth;
		}
	}
	else
		n1ColWidthTotal = n1ColWidth;


	if(n0RowHeight > n0RowHeightOrig)
	{
		if(n1RowHeightOrig>n1RowHeight)
			nDyResize += n1RowHeightOrig-n1RowHeight;
		if(n0RowHeight - n0RowHeightOrig >= nDyResize)
		{
			n0RowHeightTotal = n0RowHeightOrig + nDyResize;
			nDyResize = 0;
		}
		else
		{
			n0RowHeightTotal = n0RowHeight;
			nDyResize -= n0RowHeight - n0RowHeightOrig;
		}
	}
	else
	{
		nDyResize += n0RowHeightOrig - n0RowHeight;
		n0RowHeightTotal = n0RowHeight;
	}
	if(n1RowHeight > n1RowHeightOrig)
	{
		if(nDyResize > 0)
		{
			if(n1RowHeight - n1RowHeightOrig >= nDyResize)
				n1RowHeightTotal = n1RowHeightOrig + nDyResize;
			else
				n1RowHeightTotal = n1RowHeight;
		}
	}
	else
		n1RowHeightTotal = n1RowHeight;

//n0ColWidthTotal = n0ColWidth;
//n1ColWidthTotal = n1ColWidth;
//n0RowHeightTotal = n0RowHeight;
//n1RowHeightTotal = n1RowHeight;


	n0ColWidthTotal += GetSystemMetrics(SM_CXVSCROLL) + 2*GetSystemMetrics(SM_CXSIZEFRAME);
	if(n1ColWidthTotal > 0)
		n1ColWidthTotal += GetSystemMetrics(SM_CXVSCROLL) + 2*GetSystemMetrics(SM_CXSIZEFRAME);

	n0RowHeightTotal += GetSystemMetrics(SM_CYHSCROLL) + 2*GetSystemMetrics(SM_CYSIZEFRAME);
	if(n1RowHeightTotal > 0)
		n1RowHeightTotal += GetSystemMetrics(SM_CYHSCROLL) + 2*GetSystemMetrics(SM_CYSIZEFRAME);
	
	CRect rcScreen = NORECT;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	double fOrigposX = m_fposX;
	double fOrigposY = m_fposY;

  	if(m_fposX > 0)
	{
		//m_fposX = n0ColWidthTotal/(double)min(n0ColWidthTotal + n1ColWidthTotal+2, rcScreen.Width());
	}

	if(m_fposY > 0)
	{
		//m_fposY = n0RowHeightTotal/(double)min(n0RowHeightTotal + n1RowHeightTotal+2, rcScreen.Height());
	}

	long nMagickWidth =  3 + (IsKindOf(RUNTIME_CLASS(CSplitterCompare)) ? (GetColsCount() == 2 ? 5 : 0) : 0);
	rcWnd.right = rcWnd.left + n0ColWidthTotal + n1ColWidthTotal + (rcWnd.Width() - rcClient.Width()) - nMagickWidth;
	rcWnd.bottom = rcWnd.top + n0RowHeightTotal + n1RowHeightTotal + (rcWnd.Height() - rcClient.Height()) - 3;

	CWnd::GetParent()->MoveWindow(rcWnd);

	if(rcWndOrid.Width() == rcWnd.Width() && rcWndOrid.Height() == rcWnd.Height())
	{
		if(!(fOrigposX == m_fposX && fOrigposY == m_fposY))
			SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rcWnd.Width(), rcWnd.Height()));
	}

 }

bool CSplitterStandard::DoSplit( int nRowCount, int nColCount )
{
	if( nRowCount != 1 && nRowCount != 2 )return false;
	if( nColCount != 1 && nColCount != 2 )return false;
	if( GetRowsCount() == 1 && nRowCount == 2 )
		SplitRow();
	int nActiveRow = m_pveActiveView->nRow;
	int nActiveCol = m_pveActiveView->nCol;
	if( GetRowsCount() == 2 && nRowCount == 1 )
		//vanek - 16.10.2003
		MergeRow( 1 - nActiveRow );

	if( GetColsCount() == 1 && nColCount == 2 )
		SplitCol();
	if( GetColsCount() == 2 && nColCount == 1 )
		//vanek - 16.10.2003
		MergeCol( 1 - nActiveCol );

	if( m_pveActiveView )
	{
		CWnd	*pwnd = GetWindowFromUnknown( m_pveActiveView->punkView );
		if( pwnd )
		{
			CWnd *pwndParent = pwnd->GetParent();
			CWnd *pwndFocus = CWnd::GetFocus();
			if( pwndParent && pwndParent != pwndFocus ) pwndParent->SendMessage(WM_SETFOCUS);
		}		
	}


	return true;
}

void CSplitterStandard::AfterRecalcLayout()
{
	// Redraw during splitter drag, but doesn't redraw during windows switching,
	// see also SBT1680
	if( m_dragMode != dmNone )
		UpdateWindow();
}
