// SplitterTabbed.cpp : implementation file
//

#include "stdafx.h"
#include "StdSplitter.h"
#include "SplitterTabbed.h"
#include "SplitterWindow.h"
#include "TabBeam.h"
#include "utils.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterTabbed

IMPLEMENT_DYNCREATE(CSplitterTabbed, CWnd)

CSplitterTabbed::CSplitterTabbed()
{
	_OleLockApp( this );

	m_bFirstView = true;
	m_lLeftHorzScroll = 0;
	m_lTopHorzScroll = 0;
	m_activeView = 0;
	m_dragMode = dmNone;
	m_numViews = 0;
	m_maxNumView = -1;
	m_bScrollLeft = false;
	m_bScrollRight = false;
	m_pTabber = NULL;

	m_bRegisterNotify = false;
	
	m_fposX = 0;
	m_fposY = 0;

	m_sizeMin.cx = GetSystemMetrics( SM_CXVSCROLL )+GetSystemMetrics( SM_CXHSCROLL )*8;
	m_sizeMin.cy = GetSystemMetrics( SM_CYHSCROLL )+GetSystemMetrics( SM_CYVSCROLL )*4;

	m_hCursorHORZ = AfxGetApp()->LoadCursor( IDC_HORZ );
	m_hBitmapMenu = NULL;
	m_sName = "SplitterTabbed";
	m_lShowTabControl = ::GetValueInt( GetAppUnknown(), "\\Layout", "ShowTabsOnTabbedSplitter", 1 );

	// venek - 23.09.2003
	m_nEnabledMenuButton = -1;
}

CSplitterTabbed::~CSplitterTabbed()
{
	::DestroyCursor( m_hCursorHORZ );

	if( m_hBitmapMenu )
		::DeleteObject( m_hBitmapMenu );

	_OleUnlockApp( this );

	if (m_pscrollHorz)
	{
		m_pscrollHorz->DestroyWindow();
		delete m_pscrollHorz;
	}
	if (m_pscrollVert)
	{
		m_pscrollVert->DestroyWindow();
		delete m_pscrollVert;
	}
	//if (m_pbuttLeft)
	//{
	//	m_pbuttLeft->DestroyWindow();
	//	delete m_pbuttLeft;
	//}
	//if (m_pbuttRight)
	//{
	//	m_pbuttRight->DestroyWindow();
	//	delete m_pbuttRight;
	//}
	if (m_pscrollTabs)
	{
		m_pscrollTabs->DestroyWindow();
		delete m_pscrollTabs;
	}
	if (m_pbuttMenu)
	{
		m_pbuttMenu->DestroyWindow();
		delete m_pbuttMenu;
	}
	if (m_pTabber)
	{
		//m_pTabber->DestroyWindow();
		delete m_pTabber;
	}

	

}

BEGIN_MESSAGE_MAP(CSplitterTabbed, CSplitterBase)
	//{{AFX_MSG_MAP(CSplitterTabbed)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(100, 128, OnButton)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CSplitterTabbed, CSplitterBase)
	INTERFACE_PART(CSplitterTabbed, IID_ISplitterTabbed, Splitter)
	INTERFACE_PART(CSplitterTabbed, IID_IEventListener, EvList)
END_INTERFACE_MAP()

GUARD_IMPLEMENT_OLECREATE(CSplitterTabbed, "StdSplitter.SplitterTabbed",
0xec201e5, 0x5a2d, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

/////////////////////////////////////////////////////////////////////////////
// ISplitterTabbed interface implement
IMPLEMENT_UNKNOWN(CSplitterTabbed, Splitter)

HRESULT CSplitterTabbed::XSplitter::AddView(BSTR bstrName)
{
	_try_nested(CSplitterTabbed, Splitter, AddView)
	{
		CString strName(bstrName);
		pThis->AddView(strName);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterTabbed::XSplitter::RemoveActiveView()
{
	_try_nested(CSplitterTabbed, Splitter, RemoveActiveView)
	{
		pThis->RemoveView();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterTabbed::XSplitter::RenameView(BSTR bstrNewName)
{
	_try_nested(CSplitterTabbed, Splitter, RenameView)
	{
		CString strName(bstrNewName);
		pThis->RenameView(strName);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterTabbed::XSplitter::SaveState(IUnknown* punkND)
{
	_try_nested(CSplitterTabbed, Splitter, SaveState)
	{
		pThis->SaveViewsState(punkND);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterTabbed::XSplitter::LoadState(IUnknown* punkND)
{
	_try_nested(CSplitterTabbed, Splitter, LoadState)
	{
		pThis->LoadViewsState(punkND);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterTabbed::XSplitter::GetActiveViewName(BSTR* pbstrName)
{
	_try_nested(CSplitterTabbed, Splitter, GetActiveViewName)
	{
		CString strName;
		pThis->GetViewName(strName);
		if(pbstrName)
			*pbstrName = strName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
// CSplitterTabbed message handlers

bool CSplitterTabbed::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	if( !CSplitterBase::DoCreate( dwStyle, rc, pparent, nID ) )
		return false;
	if( !CreateView( 0, 0 ) )
		return false;
	DoRecalcLayout();


	IViewPtr	sptrView(GetViewEntry(0, 0)->punkView);
	if(sptrView != 0)
	{
		IUnknown* punkDoc = 0;
		sptrView->GetDocument(&punkDoc);
		if(punkDoc)
		{
			m_sptrDoc = punkDoc;
			Register(punkDoc);
			//Register(::GetAppUnknown());
			m_bRegisterNotify = true;
			punkDoc->Release();
		}
		
	}

	return true;


	/*if (!LoadViewsState())
	{
		//create from registrator
		CString strCompName;
		strCompName = m_regViews.GetComponentName(0);
		BSTR bstr = GetViewEntry( 0, 0 )->bstrViewRuntime;
		CString strDocType(bstr);
		::SysFreeString(bstr);
		if (strDocType != strCompName)
		{
			ChangeViewType(0, 0, _bstr_t(strCompName));
			DoRecalcLayout();
		}
		for (int i = 1; i < m_regViews.GetRegistredComponentCount(); i++)
		{
			strCompName = m_regViews.GetComponentName(i);
			AddView( "", false, _bstr_t(strCompName));
		}

		//GetWindowFromUnknown( GetViewEntry( 0, 0 )->punkView )->GetParent()->SetFocus();
	}
	*/
	
	//return DoRecalcLayout();
}


void CSplitterTabbed::OnCreateView(XSplitterEntry *pe)
{
	pe->m_bOwnScrollBars = false;
	if (!m_bFirstView)
	{
		//GetWindowFromUnknown( GetViewEntry( 0, m_activeView )->punkView )->GetParent()->ShowWindow(SW_HIDE);
		//GetParent()->
		pe->m_pscrollVert = m_pscrollVert;
		pe->m_pscrollHorz = m_pscrollHorz;
		return;
	}
	m_bFirstView = false;
	m_numViews = 1;
	m_maxNumView = 0;

	
	CRect rc;
	GetClientRect(rc);
	m_lLeftHorzScroll = 2*rc.Width()/3;
	m_pscrollHorz = new CScrollBar();
	m_pscrollVert = new CScrollBar();

	//m_pbuttLeft = new CButton();
	//m_pbuttRight = new CButton();
	m_pscrollTabs = new CScrollBar();
	m_pbuttMenu = new CButton();
	m_pTabber = new CTabBeam();
	
	CString	strScrollClass = GetValueString( GetAppUnknown(), "\\Interface", "ScrollClass", "SCROLLBAR" );
	m_pscrollHorz->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_HORZ, NORECT, this, 100 );
	m_pscrollVert->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_VERT, NORECT, this, 104 );
	m_pscrollTabs->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_HORZ, NORECT, this, 106 );
	//((CButton*)m_pbuttLeft)->Create( "MoveLeft", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 108 );
	//((CButton*)m_pbuttRight)->Create( "MoveRight", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 112 );
    ((CButton*)m_pbuttMenu)->Create( "Menu", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_BITMAP, NORECT, this, 118 );
	((CWnd*)m_pTabber)->Create(NULL, "Tabs", WS_CHILD|WS_VISIBLE, NORECT, this, 114 );
	//((CButton*)m_pTabber)->Create( "Tabs", WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, NORECT, this, 114 );

	pe->m_pscrollVert = m_pscrollVert;
	pe->m_pscrollHorz = m_pscrollHorz;
	
	((CTabBeam*)m_pTabber)->AddTab("View1");

	//m_pscrollHorz->EnableScrollBar(ESB_DISABLE_BOTH);
	//m_pscrollVert->EnableScrollBar(ESB_DISABLE_BOTH);
	
	//_ValidatePane( pe );
	//m_pbuttLeft->EnableWindow( false );
	//m_pbuttRight->EnableWindow( false );
	m_pscrollTabs->EnableScrollBar(ESB_DISABLE_BOTH);

    // vanek - 23.09.2003
	if( m_nEnabledMenuButton == -1 )
		m_nEnabledMenuButton = IsEnabledMenuButton();
	m_pbuttMenu->EnableWindow( m_nEnabledMenuButton );
	m_hBitmapMenu = _MakeBitmapButton(m_pbuttMenu, IDB_MENU);


}

void CSplitterTabbed::OnActivateView(XSplitterEntry *pe, XSplitterEntry *peOld)
{
	DoRecalcLayout();

}

bool CSplitterTabbed::DoRecalcLayout()
{
	CRect	rcFirstPane;
	CRect	rcRightPane;
	CRect	rcBottomPane;

	CRect	rc;
	GetClientRect(rc);

	
	//XSplitterEntry *pe = GetViewEntry(0, m_activeView);

	//if ( !pe )
	//	return false;
	if ( !m_pveActiveView )
		return false;
	
	HDWP hdwp = ::BeginDeferWindowPos( 40 );
	_LayoutWindow(m_pveActiveView, rc, hdwp ); 
	::EndDeferWindowPos( hdwp );

//	SetActiveView(m_activeView);
	Invalidate();
	return true;
}

void CSplitterTabbed::_LayoutWindow(XSplitterEntry *pe, CRect rc, HDWP &hdwp)
{

	if (rc.left < 0 || rc.right < 0 || rc.top < 0 || rc.bottom < 0)
		return;
	int	cxScroll = ::GetSystemMetrics( SM_CXVSCROLL );
	int	cyScroll = ::GetSystemMetrics( SM_CYHSCROLL );
	int	cxButton = ::GetSystemMetrics( SM_CXHSCROLL );
	int	cyButton = ::GetSystemMetrics( SM_CYVSCROLL );

	HWND hwnd = ::GetWindowFromUnknown( pe->punkView )->GetSafeHwnd();
	hwnd = ::GetParent( hwnd );

	/*bool	bHide = rc.Width() < cxScroll ||
				rc.Height() < cyScroll;

	int	nShowCmd = bHide?SW_HIDE:SW_SHOW;
	
	m_pscrollVert->ShowWindow( nShowCmd );
	m_pscrollHorz->ShowWindow( nShowCmd );
	m_pbuttLeft->ShowWindow( nShowCmd );
	m_pbuttRight->ShowWindow( nShowCmd );
	m_pbuttMenu->ShowWindow( nShowCmd );
	m_pTabber->ShowWindow( nShowCmd );

	//CWnd::FromHandle( hwnd )->ShowWindow( nShowCmd );
	if( bHide )
		return;*/
	
	
//place the pane
	CRect	rcWindow;
	rcWindow = rc;
	rcWindow.InflateRect( 0, 0, -cxScroll, -cyScroll );

//the "Menu" button
	CRect	rcButtonM = rc;
	rcButtonM.left = rcButtonM.right - cxScroll;
	rcButtonM.top = rc.bottom - cyScroll;
/*
//the "MoveLeft" button
	CRect	rcButtonL = rc;
	rcButtonL.right = rcButtonL.left + cxButton;
	rcButtonL.top = rc.bottom - cyScroll;

//the "MoveRight" button
	CRect	rcButtonR = rc;
	rcButtonR.left = rcButtonL.right;
	rcButtonR.top = rc.bottom - cyScroll;
	rcButtonR.right = rcButtonR.left  + cxButton;
*/
// m_pscrollTabs
	long	nLeftHorzScroll = m_lLeftHorzScroll;
	CRect	rcScrollT = rc;

	if( m_lShowTabControl )
	{
		rcScrollT.right = rcScrollT.left + cxButton*2;
		long tmp = nLeftHorzScroll;
		nLeftHorzScroll = min(nLeftHorzScroll, rc.right - cxButton*4);
		if (nLeftHorzScroll < 0)
			nLeftHorzScroll = tmp;
	}
	else
	{
		rcScrollT.right = rcScrollT.left;
		nLeftHorzScroll = 0;
	}
	rcScrollT.top = rc.bottom - cyScroll;


//the horz scrollbar 
	CRect	rcScrollH = rc;
	rcScrollH.left = nLeftHorzScroll;
	rcScrollH.right = rcButtonM.left;
	rcScrollH.top = rc.bottom - cyScroll;
	m_lTopHorzScroll = rcScrollH.top;

//the vert scrollbar 
	CRect	rcScrollV = rc;
	rcScrollV.left = rc.right - cxScroll;
	rcScrollV.bottom = rcButtonM.top;

//Tabber
	CRect	rcTabber = rc;
	rcTabber.left = rcScrollT.right;
	rcTabber.top = rc.bottom - cyScroll;
	rcTabber.right = nLeftHorzScroll-ResizerWidth;
	
	DWORD	dwFlags = SWP_NOACTIVATE|SWP_NOZORDER;

	hdwp = ::DeferWindowPos( hdwp, *m_pscrollVert, HWND_NOTOPMOST, rcScrollV.left, rcScrollV.top, rcScrollV.Width(), rcScrollV.Height(), dwFlags );
	hdwp = ::DeferWindowPos( hdwp, *m_pbuttMenu, HWND_NOTOPMOST, rcButtonM.left, rcButtonM.top, rcButtonM.Width(), rcButtonM.Height(), dwFlags );
	hdwp = ::DeferWindowPos( hdwp, *m_pTabber, HWND_NOTOPMOST, rcTabber.left, rcTabber.top, rcTabber.Width(), rcTabber.Height(), dwFlags );
	hdwp = ::DeferWindowPos( hdwp, *m_pscrollHorz, HWND_NOTOPMOST, rcScrollH.left, rcScrollH.top, rcScrollH.Width(), rcScrollH.Height(), dwFlags );
	hdwp = ::DeferWindowPos( hdwp, *m_pscrollTabs, HWND_NOTOPMOST, rcScrollT.left, rcScrollT.top, rcScrollT.Width(), rcScrollT.Height(), dwFlags );
	//hdwp = ::DeferWindowPos( hdwp, *m_pbuttLeft, HWND_NOTOPMOST, rcButtonL.left, rcButtonL.top, rcButtonL.Width(), rcButtonL.Height(), dwFlags );
	//hdwp = ::DeferWindowPos( hdwp, *m_pbuttRight, HWND_NOTOPMOST, rcButtonR.left, rcButtonR.top, rcButtonR.Width(), rcButtonR.Height(), dwFlags );
		
	hdwp = ::DeferWindowPos( hdwp, hwnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), dwFlags );

	//CRect	rcClient;
	//GetClientRect( &rcClient );

}

void CSplitterTabbed::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect	rcClient;
	GetClientRect( rcClient );
	//dc.FillRect( rcClient, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
	rcClient.right = m_lLeftHorzScroll;
	rcClient.left = rcClient.right - ResizerWidth;
	rcClient.top = rcClient.bottom - ::GetSystemMetrics( SM_CYHSCROLL );
	rcClient.InflateRect(0, 0, 0, 1);
	dc.FillRect( rcClient, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
	dc.DrawEdge(&rcClient, EDGE_RAISED, BF_RECT);

	
	// TODO: Add your message handler code here
	
	// Do not call CSplitterBase::OnPaint() for painting messages
}

BOOL CSplitterTabbed::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return true;
}



void CSplitterTabbed::OnButton( UINT nID )
{
	if (nID == m_pbuttMenu->GetDlgCtrlID())
	{
		//Menu
		/*short s = GetKeyState(VK_CONTROL);
		if ( s < 0)
			RemoveView(m_activeView);
		else
			AddView();*/

		CRect	rcWindow;
		m_pbuttMenu->GetWindowRect( rcWindow );

		CPoint	ptMenuPos;
		DWORD	dwFlags = 0;

		ptMenuPos.x = rcWindow.right;
		dwFlags = TPM_RIGHTALIGN|TPM_BOTTOMALIGN;
		
		ptMenuPos.y = rcWindow.top;

		ExecuteMenu( ptMenuPos, dwFlags );
	}
}

void CSplitterTabbed::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_dragMode == dmHorz)
	{
		long oldLeft = m_lLeftHorzScroll;
		CRect	 rc;
		GetClientRect(&rc);
		rc.left = point.x;
		rc.left = max(::GetSystemMetrics( SM_CXHSCROLL )*3, rc.left);
		rc.right -= ::GetSystemMetrics( SM_CXHSCROLL );
		if (rc.left > rc.right-::GetSystemMetrics( SM_CXHSCROLL )*3)
			rc.left = rc.right-::GetSystemMetrics( SM_CXHSCROLL )*3;
		rc.top = rc.bottom - ::GetSystemMetrics( SM_CYVSCROLL );
		m_lLeftHorzScroll = rc.left;
		//XSplitterEntry *pe = GetViewEntry(0, 0);
		//ASSERT ( pe );
		((CWnd*)m_pscrollHorz)->SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
		GetClientRect(&rc);
		rc.left = ::GetSystemMetrics( SM_CXHSCROLL )*2;
		rc.right = m_lLeftHorzScroll-ResizerWidth;
		rc.top = rc.bottom - ::GetSystemMetrics( SM_CYVSCROLL );
		//((CWnd*)m_pTabber)->SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
		((CTabBeam*)m_pTabber)->m_partialRedraw = true;
		((CWnd*)m_pTabber)->MoveWindow(&rc, false);
		((CTabBeam*)m_pTabber)->Invalidate();
		Invalidate();
		UpdateWindow();
	}
	else
		CSplitterBase::OnMouseMove(nFlags, point);
}

CSplitterTabbed::DragMode CSplitterTabbed::_HitTest(CPoint pt)
{
	if ((pt.x > m_lLeftHorzScroll - ResizerWidth)&&(pt.x < m_lLeftHorzScroll)&&(pt.y > m_lTopHorzScroll)&&(pt.y < m_lTopHorzScroll + ::GetSystemMetrics( SM_CYVSCROLL )))
		return dmHorz;
	else
		return dmNone;
}

void CSplitterTabbed::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_dragMode = _HitTest( point );
	
	if( m_dragMode != dmNone )
	{
		SetCapture();
	}	

	CSplitterBase::OnLButtonDown(nFlags, point);
}

BOOL CSplitterTabbed::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint	pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );

	DragMode	mode = _HitTest( pt );

	if( mode == dmHorz )
	{
		if( m_hCursorHORZ )
			::SetCursor( m_hCursorHORZ );		
	}
	else 
		return CSplitterBase::OnSetCursor(pWnd, nHitTest, message);
	return true;
}

void CSplitterTabbed::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_dragMode != dmNone )
	{
		ReleaseCapture();
		m_dragMode = dmNone;
		((CTabBeam*)m_pTabber)->m_partialRedraw = false;
	}
	CSplitterBase::OnLButtonUp(nFlags, point);
}

void CSplitterTabbed::AddView(CString strLabel, bool setActive, BSTR bstrViewType)
{
	m_numViews++;
	m_maxNumView++;
	if (strLabel == "")
		strLabel.Format("View %d", m_maxNumView+1);
	CreateView(0, m_maxNumView, bstrViewType);//, strLabel.AllocSysString());
	((CTabBeam*)m_pTabber)->AddTab(strLabel, setActive);
	/*if (!setActive)
	{
		m_activeView = m_pTabber->GetIndexByTab(m_pTabber->m_nActiveTab
			);
		XSplitterEntry *pe = GetViewEntry(0, m_activeView);
			ASSERT ( pe );
		m_pveActiveView = pe;
	}*/
	//SetActiveView(m_maxNumView);
	DoRecalcLayout();
	SetButtons(m_pTabber->NeedScrollLeft() ? 1 : 0, -1);
	SetButtons(-1, m_pTabber->NeedScrollRight() ? 1 : 0);
}

void CSplitterTabbed::SetActiveView(short newActive)
{
	if (newActive < 0 || newActive > m_maxNumView)
		return;

	if(m_activeView == newActive)return;

	XSplitterEntry *pe = GetViewEntry(0, m_activeView);

	if (pe && pe->punkView)
	{
		pe->m_pscrollVert = NULL;
		pe->m_pscrollHorz = NULL;

		//::GetWindowFromUnknown( pe->punkView )->GetParent()->SendMessage(WM_SIZE);
		//::GetWindowFromUnknown( pe->punkView )->SendMessage(WM_SIZE);
		
		::GetWindowFromUnknown( pe->punkView )->ShowWindow(SW_HIDE);
		::GetWindowFromUnknown( pe->punkView )->GetParent()->ShowWindow(SW_HIDE);
		
	}
	m_activeView = newActive;
	pe = GetViewEntry(0, m_activeView);
	ASSERT ( pe );
	m_pveActiveView = pe;

	
	if (pe->punkView)
	{
		pe->m_pscrollVert = m_pscrollVert;
		pe->m_pscrollHorz = m_pscrollHorz;
		::GetWindowFromUnknown( pe->punkView )->GetParent()->SendMessage(WM_SETFOCUS);
		::GetWindowFromUnknown( pe->punkView )->SendMessage(WM_SIZE);
		::GetWindowFromUnknown( pe->punkView )->GetParent()->SendMessage(WM_SIZE);
		::GetWindowFromUnknown( pe->punkView )->GetParent()->ShowWindow(SW_SHOW);
		::GetWindowFromUnknown( pe->punkView )->ShowWindow(SW_SHOW);

		DoRecalcLayout();
	}
	
}



void CSplitterTabbed::SetButtons(short left, short right)
{
	if (left == 0)
	{
		//m_pbuttLeft->EnableWindow(false);
		m_bScrollLeft = false;
	}else if (left == 1)
	{
		//_MakeBitmapButton(m_pbuttLeft, IDB_MERGEH1_DK);
		//m_pbuttLeft->EnableWindow(true);
		m_bScrollLeft = true;
	}
	if (right == 0)
	{
		m_bScrollRight = false;
		//m_pbuttRight->EnableWindow(false);
	}else if (right == 1)
	{
		m_bScrollRight = true;
		//_MakeBitmapButton(m_pbuttRight, IDB_MERGEH2_DK);
		//m_pbuttRight->EnableWindow(true);
	}

	m_pscrollTabs->EnableScrollBar( ESB_ENABLE_BOTH );
	if (!m_bScrollRight)
		m_pscrollTabs->EnableScrollBar(ESB_DISABLE_RTDN);
	if (!m_bScrollLeft)
		m_pscrollTabs->EnableScrollBar(ESB_DISABLE_LTUP);
}

void CSplitterTabbed::RemoveView()
{
	RemoveView(m_activeView);
}

void CSplitterTabbed::RemoveView(short Number, bool setActive)
{
	if (Number > m_maxNumView || Number < 0 || m_numViews == 1)
		return;
	//SetActiveView(min(m_numViews-1, Number));
	m_numViews--;
//	if (Number == m_maxNumView)
//	{
//		m_maxNumView--;
//	}
	DeleteView(0, Number);
	//XSplitterEntry *pe = GetViewEntry(0, Number);
	((CTabBeam*)m_pTabber)->DelTab(Number, setActive);
	SetButtons(m_pTabber->NeedScrollLeft() ? 1 : 0, -1);
	SetButtons(-1, m_pTabber->NeedScrollRight() ? 1 : 0);
}

void CSplitterTabbed::SaveViewsState(IUnknown* punkF)
{

	IUnknown* punk; 
	if (punkF)
		punk = punkF;
	else
		punk = GetAppUnknown();
	if( !CheckInterface( punk, IID_INamedData ))
		return;

	CString pszSection = "Splitter_"+m_strDocTemplateName;
	_variant_t	var;
	XSplitterEntry *pe; 
	CString str1, str2;
	CPoint pointScroll;

	var = (const char* )"SplitterTabbed";
	::SetValue(punk, pszSection, "Signature", var);
	var = (long)m_numViews;
	::SetValue(punk, pszSection, "NumViews",  var);

	var = (long)m_pTabber->m_nActiveTab;
	::SetValue(punk, pszSection, "ActiveView",  var);

	for (int i = 0; i < m_numViews; i++)
	{
		str1.Format("View%dName", i);
		m_pTabber->GetTabName(m_pTabber->GetIndexByTab(i), str2);
		var = str2;
		::SetValue(punk, pszSection, str1,  var);

		str1.Format("View%dType", i);
		pe = GetViewEntry(0, m_pTabber->GetIndexByTab(i));
		var = pe->bstrViewRuntime.copy();
		::SetValue(punk, pszSection, str1,  var);

		str1.Format("View%dHorzPos", i);
		sptrIScrollZoomSite sptr(pe->punkView);
		sptr->GetScrollPos(&pointScroll);
		var = (long)pointScroll.x;
		::SetValue(punk, pszSection, str1,  var);

		str1.Format("View%dVertPos", i);
		var = (long)pointScroll.y;
		::SetValue(punk, pszSection, str1,  var);
	}
	var = (long)m_pTabber->m_lLeftVisiblePos;
	::SetValue(punk, pszSection, "LeftVisiblePos",  var);
	var = (long)m_pTabber->m_nCurLeftVisTab;
	::SetValue(punk, pszSection, "CurLeftVisTab",  var);
	var = (long)m_lLeftHorzScroll;
	::SetValue(punk, pszSection, "LeftHorzScroll",  var);
}

void CSplitterTabbed::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = m_sizeMin.cx * 1;
	lpMMI->ptMinTrackSize.y = m_sizeMin.cy * 1;
	
	CSplitterBase::OnGetMinMaxInfo(lpMMI);
}

void CSplitterTabbed::OnSize(UINT nType, int cx, int cy) 
{
	if( m_fposX > 0 )m_fposX = max( (double)m_sizeMin.cx/cx, min( 1-(double)m_sizeMin.cx/cx, m_fposX ) );
	if( m_fposY > 0 )m_fposY = max( (double)m_sizeMin.cy/cy, min( 1-(double)m_sizeMin.cy/cy, m_fposY ) );

	CSplitterBase::OnSize(nType, cx, cy);
}

void CSplitterTabbed::RenameView(CString strNewName)
{
	RenameView(m_activeView, strNewName);	
}


void CSplitterTabbed::GetViewName(CString &strName)
{
	m_pTabber->GetTabName(m_activeView, strName);
}

bool CSplitterTabbed::LoadViewsState(IUnknown* punkF)
{
	IUnknown* punk; 
	if (punkF)
		punk = punkF;
	else
		punk = GetAppUnknown();
	if( !CheckInterface( punk, IID_INamedData ))
		return false;
	CString pszSection = "Splitter_"+m_strDocTemplateName;
	int i;
	long numViews;
	_variant_t	var;
	XSplitterEntry *pe; 
	CString str1, str2;
	CPoint pointScroll;

	CString strSignature;
	strSignature = ::GetValueString(punk, pszSection, "Signature");
	if (strSignature != "SplitterTabbed")
	{
		//AfxMessageBox(IDS_NO_LOAD_DATA);
		return false;
	}
	if (m_pTabber == NULL)
		return false;
	m_pTabber->m_lLeftVisiblePos = ::GetValueInt(punk, pszSection, "LeftVisiblePos", _variant_t((long)0));
	m_pTabber->m_nCurLeftVisTab = ::GetValueInt(punk, pszSection, "CurLeftVisTab", _variant_t((long)0));
	m_lLeftHorzScroll = ::GetValueInt(punk, pszSection, "LeftHorzScroll", _variant_t((long)-1));
	if (m_lLeftHorzScroll < 0)
		m_lLeftHorzScroll = ::GetSystemMetrics(SM_CYHSCROLL)*3;

	CRect	 rc;
	GetClientRect(&rc);
	rc.left = m_lLeftHorzScroll;
	rc.right -= ::GetSystemMetrics( SM_CXHSCROLL );
	rc.top = rc.bottom - ::GetSystemMetrics( SM_CYHSCROLL );
	((CWnd*)m_pscrollHorz)->SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
	GetClientRect(&rc);
	rc.left = ::GetSystemMetrics( SM_CXHSCROLL )*2;
	rc.right = m_lLeftHorzScroll-ResizerWidth;
	rc.top = rc.bottom - ::GetSystemMetrics( SM_CYVSCROLL );
	((CWnd*)m_pTabber)->MoveWindow(&rc, true);



	numViews = ::GetValueInt(punk, pszSection, "NumViews", _variant_t((long)1));
	
	if (m_numViews < numViews)
	{
		long tmp = m_numViews;
		for (i = tmp; i <= numViews-1; i++)
		{
			AddView("", false);
		}
	}
	else if (m_numViews > numViews)
	{
		long tmp = m_numViews-1;
		for (i = tmp; i >= numViews; i--)
		{
			RemoveView(m_pTabber->GetIndexByTab(i), false);
		}
	}

	m_pscrollHorz->EnableWindow(FALSE);
	m_pscrollVert->EnableWindow(FALSE);
	for (i = 0; i < numViews; i++)
	{
		str1.Format("View%dName", i);
		str2 = ::GetValueString(punk, pszSection, str1);
		RenameView(m_pTabber->GetIndexByTab(i), str2);
		
		str1.Format("View%dType", i);
		pe = GetViewEntry(0, m_pTabber->GetIndexByTab(i));
		var = ::GetValue(punk, pszSection, str1, var);
		if (_bstr_t(var.bstrVal, true) != _bstr_t(pe->bstrViewRuntime, true))
		{
			ChangeViewType(0, m_pTabber->GetIndexByTab(i), var.bstrVal);
			pe = GetViewEntry(0, m_pTabber->GetIndexByTab(i));
		}
		
		str1.Format("View%dHorzPos", i);
		sptrIScrollZoomSite sptr(pe->punkView);
		pointScroll.x = ::GetValueInt(punk, pszSection, str1);

		str1.Format("View%dVertPos", i);
		pointScroll.y = ::GetValueInt(punk, pszSection, str1);

		sptr->SetScrollPos(pointScroll);
	}
	m_pscrollHorz->EnableWindow();
	m_pscrollVert->EnableWindow();


	m_pTabber->SetActiveTab(::GetValueInt(punk, pszSection, "ActiveView", _variant_t((long)0)));

	return true;
}

void CSplitterTabbed::RenameView(short idx, CString strNewName)
{
	if (strNewName.IsEmpty())
	{
		strNewName.Format("View%d", idx+1);
	}
	m_pTabber->RenameTab(idx, strNewName);
}


void CSplitterTabbed::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pscrollTabs->GetDlgCtrlID() == pScrollBar->GetDlgCtrlID())
	{
		if (nSBCode == SB_LINELEFT)
		{
			// [vanek]: prevent scroll if first tab is visible - 06.04.2004
			if( m_pTabber->NeedScrollLeft() )
			{
				//MoveLeft
				m_pTabber->ScrollLeft();
				SetButtons(m_pTabber->NeedScrollLeft() ? 1 : 0, -1);
				SetButtons(-1, m_pTabber->NeedScrollRight() ? 1 : 0);
			}
		}
		else if(nSBCode == SB_LINERIGHT)
		{	
			// [vanek]: prevent scroll if last tab is visible - 06.04.2004
			if( m_pTabber->NeedScrollRight() )
			{
				//MoveRight
				m_pTabber->ScrollRight();
				SetButtons(m_pTabber->NeedScrollLeft() ? 1 : 0, -1);
				SetButtons(-1, m_pTabber->NeedScrollRight() ? 1 : 0);
			}
		}
		// [vanek]: invaidate buttons - 06.04.2004
		else if(nSBCode == SB_ENDSCROLL )
			m_pscrollTabs->Invalidate();
	}
	else if(m_pscrollHorz->GetDlgCtrlID() == pScrollBar->GetDlgCtrlID())
	{
		SetFocus();
		CSplitterBase::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	Invalidate();
}

void CSplitterTabbed::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_pscrollVert->GetDlgCtrlID() == pScrollBar->GetDlgCtrlID())
	{
		SetFocus();
		CSplitterBase::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}


void CSplitterTabbed::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//SaveViewsState();

	//CSplitterBase::OnClose();
}



void CSplitterTabbed::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	
	if(::GetKeyState(VK_CONTROL) < 0)
	{
		short nTab = m_pTabber->GetActiveTab();

		if(nChar == 33) //PageUp
				nTab -= 1;
		else if(nChar == 34) //PageDown
			nTab += 1;

		if(nTab != m_pTabber->GetActiveTab())
			m_pTabber->SetActiveTab(nTab);
	}
	CSplitterBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSplitterTabbed::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	return;
	_try(CSplitterTabbed, OnNotify)
	{
		if( strcmp(pszEvent, szEventCreateTypedObject ) == 0)
		{
			sptrINamedDataObject sptrObj(punkFrom);	
			
			if(sptrObj == 0)
				return;

			POSITION	pos = GetFirstViewEntryPosition();
			XSplitterEntry* pEntry = 0;
			bool bNeedNewView = true;
			long nCurView = -1;
			CString strObjType = ::GetObjectKind(sptrObj);
			sptrIView sptrView; 
			while(pos)
			{
				pEntry = GetNextViewEntry(pos);
				nCurView++;
				DWORD dwMatch = mvNone;

				sptrView = pEntry->punkView;

				if(sptrView == 0)
					continue;

				CString strViewType = ::GetObjectKind(sptrView);

				sptrView->GetMatchType(_bstr_t(strObjType), &dwMatch);
				if(dwMatch & mvFull || dwMatch & mvPartial)
				{
					int nTypeCount = 0;
					sptrIDataContext2 sptrContext(sptrView);

					if(sptrContext == 0)
						continue;

					/*sptrContext->GetObjectTypeCount(&nTypeCount);

					bNeedNewView = false;
					for(int i = 0; i < nTypeCount; i++)
					{
						BSTR bstrType = 0;
						sptrContext->GetObjectType(i, &bstrType);
						CString strType(bstrType);
						::SysFreeString(bstrType);
						if(strType == strObjType)
						{
							bNeedNewView = true;
							break;
						}
					}*/
					IUnknown* punkActiveObj = GetActiveObjectFromContextEx(sptrView);
					if(punkActiveObj)
					{
						bNeedNewView = ::GetObjectKey(punkActiveObj) != ::GetObjectKey(sptrObj);

						punkActiveObj->Release();
					}
					else
					{
						bNeedNewView = false;
					}
							
					if(!bNeedNewView)
					{
						//activate current view & setup context
						m_pTabber->SetActiveTab(nCurView);

						sptrContext->SetActiveObject( 0, sptrObj, aofActivateDepended);
						break;
					}
				}
			}
			if(bNeedNewView)
			{
				//add view to splitter & select into view's context the new object
				DWORD dwMatch = mvNone;
				CString strViewType = ::FindMostMatchView(strObjType, dwMatch);
				if(dwMatch != mvNone)
				{
					//TRACE("!!!!!!!!!Want add %d type object to %s view\n", strObjType, strViewType);
					AddView("", true, _bstr_t(strViewType));
					if(m_pveActiveView)
					{
						sptrIDataContext2 sptrContext(m_pveActiveView->punkView);
						sptrContext->SetActiveObject( 0, sptrObj, aofActivateDepended);
					}
				}
			}
		}
		
	}
	_catch;
}

void CSplitterTabbed::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_bRegisterNotify)
	{
		if(m_sptrDoc != 0)
		{
			UnRegister(m_sptrDoc);
			//UnRegister(::GetAppUnknown());
			m_bRegisterNotify = false;
			m_sptrDoc = 0;
		}
	}
	
	//CSplitterBase::OnFinalRelease();
	delete this;
}

void CSplitterTabbed::OptimizeSplitter()
{
	if( CWnd::GetParent()->IsZoomed() )
		return;

	if( !m_lShowTabControl )
		return;

	IScrollZoomSitePtr	sptrSite(m_pveActiveView->punkView);
	if(sptrSite == 0)
		return;

	long nDX = GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXSIZEFRAME)/2;
	long nDY = GetSystemMetrics(SM_CYHSCROLL) + GetSystemMetrics(SM_CYSIZEFRAME)/2;

	CSize size(0, 0);
	BOOL bASM = FALSE;
	sptrSite->GetAutoScrollMode(&bASM);
	if(!bASM)
	{
		return;
	}
	else
	{
		double fZoom = 1;
		sptrSite->GetZoom(&fZoom);
	
		sptrSite->GetClientSize(&size);

		size.cx *= fZoom;
		size.cy *= fZoom;

		size.cx += nDX;
		size.cy += nDY;
	}
		
	IUnknown	*punkMainWnd = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow );
	sptrIMainWindow	sptrMain( punkMainWnd );
	if( punkMainWnd )
		punkMainWnd->Release();
	
	HWND hWnd = 0;
	CRect rcClientFrame = NORECT;
	sptrMain->GetMDIClientWnd(&hWnd);
	if(hWnd)
		::GetWindowRect(hWnd, &rcClientFrame);

	CRect rcWnd = NORECT;
	CWnd::GetParent()->GetWindowRect(&rcWnd);
	CRect rcWndOrid = rcWnd;
	CRect rcClient = NORECT;
	GetClientRect(&rcClient);

	long nMaxWidth = max(0, rcClientFrame.right - rcWnd.right);
	long nMaxHeight = max(0, rcClientFrame.bottom - rcWnd.bottom);

	CWnd::GetParent()->GetParent()->ScreenToClient(&rcWnd);

	long nNewWidth = rcClient.Width();
	long nNewHeight = rcClient.Height();


	if(size.cx > nNewWidth)
	{
		if(size.cx - nNewWidth >= nMaxWidth)
			nNewWidth += nMaxWidth;
		else
			nNewWidth = size.cx;
	}
	else
		nNewWidth = size.cx;

	if(size.cy > nNewHeight)
	{
		if(size.cy - nNewHeight >= nMaxHeight)
			nNewHeight += nMaxHeight;
		else
			nNewHeight = size.cy;
	}
	else
		nNewHeight = size.cy;


	//nNewWidth += GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXSIZEFRAME);
	//nNewHeight += GetSystemMetrics(SM_CYHSCROLL) + GetSystemMetrics(SM_CYSIZEFRAME); 

	rcWnd.right = rcWnd.left + nNewWidth + rcWnd.Width() - rcClient.Width();
	rcWnd.bottom = rcWnd.top + nNewHeight + rcWnd.Height() - rcClient.Height();
	
	CWnd::GetParent()->MoveWindow(rcWnd);

}

// vanek -022.09.2003
bool CSplitterTabbed::IsEnabledMenuButton( )
{
    return ( 1L == ::GetValueInt(GetAppUnknown(), "\\SplitterTabbed", "EnableMenuButton", 1 ) );
}
