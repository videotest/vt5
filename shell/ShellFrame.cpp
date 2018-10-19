// ChildFrm.cpp : implementation of the CShellFrame class
//

#include "stdafx.h"
#include "shell.h"
#include "ShellFrame.h"

#include "shelldoc.h"
#include "shellview.h"
#include "docs.h"

#include "targetman.h"
#include "mainfrm.h"
#include "misc.h"
#include "shelltoolbar.h"

#include <afxpriv.h>
//#include "afximpl.h"

#include "CommandManager.h"



CBCGToolbarButton *_create_button_from_string( CStringArray &strings, int &ipos, bool bMenuBar );




//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
/////////////////////////////////////////////////////////////////////////////
// CCoolBar

IMPLEMENT_DYNAMIC(CChildFrameBar, CShellDockBar)
BEGIN_MESSAGE_MAP(CChildFrameBar, CShellDockBar)
	//{{AFX_MSG_MAP(CCoolBar)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_WM_MOVE()	
	ON_WM_PARENTNOTIFY()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_CLOSE_DOCK_BAR, OnCloseDockBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CChildFrameBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CShellDockBar::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

void CChildFrameBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle)
{
	if( ((dwNewStyle ^ dwOldStyle) & WS_VISIBLE)==WS_VISIBLE )
	{
		//TRACE( "WS_VISIBLE\n" );
		OnShowWindow( ( dwNewStyle & WS_VISIBLE )== WS_VISIBLE, 0 );
	}
	CShellDockBar::OnBarStyleChange(dwOldStyle, dwNewStyle);
}

void CChildFrameBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CShellDockBar::OnShowWindow(bShow, nStatus);	

	CWnd* pChildWnd = NULL;
	pChildWnd = GetWindow( GW_CHILD );	

	CShellFrame* pChildFrame = (CShellFrame*)pChildWnd;

	if( !pChildFrame )return;
	CView *pView = pChildFrame->GetActiveView();

	if( !pView )return;

	if( bShow ) 
	{
		pView->SetFocus();
		::FireEvent( ::GetAppUnknown(), szAppActivateView, pView->GetControllingUnknown(), pView->GetDocument()->GetControllingUnknown() );
		
	}
	else
	{
		if( !pChildFrame->ActivateLasMDIChild( true ) )
			::FireEvent( ::GetAppUnknown(), szAppActivateView, 0 );
	}
	pView->SendMessage( WM_SHOWWINDOW, bShow );
	AfxGetMainWnd()->PostMessage( WM_KICKIDLE );
}

BOOL CChildFrameBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_WINDOWPOSCHANGED || message == WM_PAINT )
	{
		BOOL	bVisible = IsWindowVisible();
	
		if( bVisible != m_bOldVisible )
			OnShowWindow( bVisible, 0 );

		m_bOldVisible = bVisible;
	}


	return CShellDockBar::OnWndMsg(message, wParam, lParam, pResult);


}


LRESULT CChildFrameBar::OnCloseDockBar(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
		
	CShellFrame* pMDIChildToActivate = NULL;

	CWnd* pChildWnd = pChildWnd = GetWindow( GW_CHILD );	
	if( pChildWnd && pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)) )
	{
		CShellFrame* pChildFrame = (CShellFrame*)pChildWnd;
		CView *pView = pChildFrame->GetActiveView();
		if( pView ) 
		{
			sptrIView spView( pView->GetControllingUnknown() );		
			if( spView )
			{
				spView->OnActivateView( false, NULL );
			}
		}

		if( !pChildFrame->ActivateLasMDIChild( true ) )
			::FireEvent( ::GetAppUnknown(), szAppActivateView, 0 );

	}
	return 1L;

}

void CChildFrameBar::StoreDockPosition()
{
	CWnd* pChildWnd = pChildWnd = GetWindow( GW_CHILD );
	if( !pChildWnd )return;

	CShellFrame* pFrame = (CShellFrame*)pChildWnd;
	CView *pView = pFrame->GetActiveView();

	if( !pView ) return;
	CDocument	*pdoc= pView->GetDocument();
	CDocTemplate	*ptempl = pdoc->GetDocTemplate();


	//if this document is first, save position
	POSITION	pos = ptempl->GetFirstDocPosition();
	ASSERT(pos);
	CDocument	*p = ptempl->GetNextDoc( pos );
	if( p != pdoc ) return;

	CString	strDocTemplate;
	ptempl->GetDocString( strDocTemplate, CDocTemplate::docName );

	strDocTemplate = "\\"+strDocTemplate;

	CShellToolDockBar	*pleft,*ptop, *pright, *pbottom;
	CMainFrame	*pmain = (CMainFrame	*)AfxGetMainWnd();

	pleft = pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT );
	ptop = pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP );
	pright = pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT );
	pbottom = pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM );


	CRect	rectPos = CRect( 0, 0, 100, 100 );
	int		nDockSite = 0;
	int		nVisible = IsWindowVisible();

	if( IsFloating() )
	{
		GetParentFrame()->GetWindowRect( &rectPos );
	}
	else
	{
		if( pleft->FindBar( this ) != -1 )nDockSite = AFX_IDW_DOCKBAR_LEFT;
		if( ptop->FindBar( this ) != -1 )nDockSite = AFX_IDW_DOCKBAR_TOP;
		if( pright->FindBar( this ) != -1 )nDockSite = AFX_IDW_DOCKBAR_RIGHT;
		if( pbottom->FindBar( this ) != -1 )nDockSite = AFX_IDW_DOCKBAR_BOTTOM;
		rectPos = m_pDockContext->m_rectMRUDockPos;
	}

	CString	str1, str2;
	str1.Format( "%d %d %d %d %d %d", 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );
	str2.Format( "(%d %d) (%d %d) (%d %d)", m_szVert.cx, m_szVert.cy, m_szHorz.cx, m_szHorz.cy, m_szFloat.cx, m_szFloat.cy );


	SetValue( GetAppUnknown(), strDocTemplate, "DockState1", str1 );
	SetValue( GetAppUnknown(), strDocTemplate, "DockState2", str2 );
}

void CChildFrameBar::OnMove(int x, int y)
{
	CShellDockBar::OnMove( x, y );
	StoreDockPosition();
}

void CChildFrameBar::OnSize(UINT nType, int cx, int cy) 
{
	CShellDockBar::OnSize(nType, cx, cy);
	CWnd* pChildWnd = NULL;
	
	
	pChildWnd = GetWindow( GW_CHILD );	
	if( !pChildWnd || !pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)) )
		return;
	CRect rcClient;
	GetClientRect( &rcClient );	

	pChildWnd->MoveWindow( 								
								rcClient.left, rcClient.top,
								rcClient.Width(), rcClient.Height(), 
								TRUE
								);	
	StoreDockPosition();

}



void CChildFrameBar::OnParentNotify(UINT message, LPARAM lParam) 
{
	CWnd* pChildWnd = NULL;	
	
	pChildWnd = GetWindow( GW_CHILD );	
	if( pChildWnd && pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)))
	{
		pChildWnd->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, 0L );
	}

	CShellDockBar::OnParentNotify(message, lParam);	
}

BOOL CChildFrameBar::PreTranslateMessage(MSG* pMsg) 
{	


	if( 			
		pMsg->message == WM_KEYDOWN ||  
		pMsg->message == WM_SYSKEYDOWN ||  
		pMsg->message == WM_LBUTTONDOWN ||  
		pMsg->message == WM_RBUTTONDOWN ||  
		pMsg->message == WM_MBUTTONDOWN ||  
		pMsg->message == WM_NCLBUTTONDOWN ||  
		pMsg->message == WM_NCRBUTTONDOWN ||  
		pMsg->message == WM_NCMBUTTONDOWN ||
		pMsg->message == WM_NCACTIVATE
		)
	{						

		HWND hWndDockBar;

		hWndDockBar = GetSafeHwnd();
		
		if( pMsg->hwnd == hWndDockBar )
		{
			CWnd* pChildWnd = NULL;	
			
			pChildWnd = GetWindow( GW_CHILD );	
			if( pChildWnd && pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)))
			{
				pChildWnd->SendMessage( WM_NCACTIVATE, (WPARAM)TRUE, 0L );
			}
		}
			
	}

	
	UINT message = pMsg->message;
	
	
	//if( pMsg->message == WM_CHAR || pMsg->message == WM_KEYDOWN )

	if( pMsg->message == WM_CHAR || pMsg->message == WM_KEYDOWN )/*||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
		)	*/
	{
		//Press enter key

		CWnd* pChildWnd = NULL;	
		
		pChildWnd = GetWindow( GW_CHILD );	
		if( pChildWnd && pChildWnd->IsKindOf( RUNTIME_CLASS(CShellFrame)))
		{

			CShellView* pView = NULL;
			pView = (CShellView*)((CShellFrame*)pChildWnd)->GetActiveView( );
		 
			//pView
			if( pView )
			{				
				IUnknown* punkView = NULL;
				punkView = pView->GetViewUnknown();
				if( punkView )
				{
					CWnd* wndView = NULL;
					wndView = GetWindowFromUnknown(punkView);
					if( wndView )
					{						
						wndView->SendMessage( message, pMsg->wParam, pMsg->lParam );
						if( pMsg->wParam == VK_RETURN )
							wndView->SendMessage( WM_CHAR, pMsg->wParam, pMsg->lParam );

					}
					punkView->Release();

				}
			}
		}
	
	}	





	return CShellDockBar::PreTranslateMessage(pMsg);
	
}

CMiniDockFrameWnd* CChildFrameBar::GetMiniDockWnd()
{
	CDockBar* pDockBar = m_pDockBar;

	if(pDockBar == NULL) return NULL;

	if(!pDockBar->m_bFloating) return NULL;


	CMiniDockFrameWnd* pDockFrame = NULL;

	pDockFrame = (CMiniDockFrameWnd*)pDockBar->GetParent();
					
	return pDockFrame;

}

CChildFrameBar::CChildFrameBar( IUnknown *punkBar ) : CShellDockBar( punkBar )
{	
	m_bChildFrameBar = TRUE;	
	m_bOldVisible = false;
}

void CChildFrameBar::SetInterface( IUnknown *punkBar )
{
	m_sptrDockClient = punkBar;
	m_sptrWindow = punkBar;
	m_sptrName = punkBar;
}

void CChildFrameBar::SetName( CString strName )
{
	m_strName = strName;
}

LRESULT CChildFrameBar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	if( message == WM_SETTEXT )
	{
		SetName( (char*)lParam );
	}
	/*
	else
	if( message == WM_GETTEXT )
	{
		ASSERT( false );
	}
	*/
	//m_strName = strName;
	return CShellDockBar::DefWindowProc( message, wParam, lParam );
}


//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CShellFrame

IMPLEMENT_DYNCREATE(CShellFrame, CMDIChildWnd)


//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
//	ON_WM_CREATE()
//	ON_WM_WINDOWPOSCHANGING()		
//	ON_WM_MOUSEACTIVATE()
//	ON_WM_ACTIVATE()
//	ON_WM_NCACTIVATE()
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

BEGIN_MESSAGE_MAP(CShellFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CShellFrame)
	ON_WM_CLOSE()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()		
	ON_WM_MOUSEACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PARENTNOTIFY()
	ON_WM_SYSCOMMAND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CShellFrame, CMDIChildWnd)
	INTERFACE_PART(CShellFrame, IID_IFrameWindow, Frm)
	INTERFACE_PART(CShellFrame, IID_IFrameWindow2, Frm)
	INTERFACE_PART(CShellFrame, IID_INumeredObject, Num)
	INTERFACE_PART(CShellFrame, IID_IWindow, Wnd)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CShellFrame, Frm);

LPUNKNOWN CShellFrame::GetInterfaceHook(const void*piidv)
{
	static bool bInsideHook = false;

	if( bInsideHook )
		return false;

	IID *piid = (IID *)piidv;

	if( *piid == IID_IUnknown )
		return 0;
	bInsideHook  = true;

	if( CheckInterface( m_sptrSplitter, *piid ) )
	{
		IUnknown	*punkInterface = 0;
		m_sptrSplitter->QueryInterface( *piid, (void**)&punkInterface );

		ASSERT(punkInterface);
		punkInterface->Release();

		bInsideHook  = false;

		return punkInterface;
	}
	bInsideHook  = false;

	return 0;
}

HRESULT CShellFrame::XFrm::CreateView( UINT nID, BSTR bstrProgID, IUnknown **ppunkView )
{
	_try_nested(CShellFrame, Frm, CreateView)
	{
		//sptrIView	sptrV;

		CShellView* pView = new CShellView( bstrProgID );

		//sptrV.CreateInstance( bstrProgID, pView->GetControllingUnknown() );
		
		*ppunkView = pView->GetControllingUnknown();

		pView->Init( pThis, pThis->m_pDocument, /**ppunkView*/ 0 );

		CCreateContext	context;
		context.m_pNewViewClass = RUNTIME_CLASS( CShellView );
		context.m_pCurrentDoc = pThis->m_pDocument;
		context.m_pNewDocTemplate = pThis->m_pDocument->GetDocTemplate();
		context.m_pLastView = 0;
		context.m_pCurrentFrame = pThis;

		HWND	hwnd = 0;

		sptrIWindow	sptrWindow( pThis->m_sptrSplitter );

		if( sptrWindow )
			sptrWindow->GetHandle( (HANDLE*)&hwnd );

		CWnd	*pwndParent = CWnd::FromHandle( hwnd );

		//To correct RepositionBars, then MFC think that our splitter is a view
		//pwndParent->SetDlgCtrlID( AFX_IDW_PANE_FIRST );	


		// views are always created with a border!
		if( !pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), pwndParent, nID, &context) )
		{
			TRACE0("Warning: could not create view for frame.\n");
			return E_FAIL;
		}

		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::DestroyView( IUnknown *punkView )
{
	_try_nested(CShellFrame, Frm, DestroyView)
	{
		POSITION	pos = pThis->m_pDocument->GetFirstViewPosition();

		while( pos )
		{
			CShellView	*pview = (CShellView	*)pThis->m_pDocument->GetNextView( pos );
			IUnknown *punk = pview->GetViewUnknown();

			if( ::GetWindowFromUnknown( punk ) == ::GetWindowFromUnknown( punkView ) )
			{
				ULONG ul=punk->Release(); 
				pview->DestroyWindow();
				//pview->Release();
				return S_OK;
			}
			else
				punk->Release();
		}
		return E_INVALIDARG;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::GetActiveView( IUnknown **ppunkView )
{
	_try_nested(CShellFrame, Frm, GetActiveView)
	{
		*ppunkView = 0;

		CView	*pview = pThis->GetActiveView();

		if( !pview )
			return S_OK;

		ASSERT_KINDOF(CShellView, pview);

		*ppunkView = ((CShellView*)pview)->GetViewUnknown();
		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::GetDocument( IUnknown **ppunkDoc )
{
	_try_nested(CShellFrame, Frm, GetDocument)
	{
		*ppunkDoc = pThis->m_pDocument->GetDocumentInterface();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::GetSplitter( IUnknown **ppunkSplitter )
{
	_try_nested(CShellFrame, Frm, GetDocument)
	{
		*ppunkSplitter = pThis->m_sptrSplitter;

		if( *ppunkSplitter )
			(*ppunkSplitter)->AddRef();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::RecalcLayout()
{
	_try_nested(CShellFrame, Frm, RecalcLayout)
	{
		pThis->RecalcLayout();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::Maximize()
{
	_try_nested(CShellFrame, Frm, Maximize)
	{
		pThis->MDIMaximize();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::Restore()
{
	_try_nested(CShellFrame, Frm, Restore)
	{
		pThis->MDIRestore();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellFrame::XFrm::GetLockedFlag(BOOL *pbLocked)
{
	_try_nested(CShellFrame, Frm, GetLockedFlag)
	{
		*pbLocked = pThis->m_bLockedViews;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellFrame::XFrm::SetLockedFlag(BOOL bLocked)
{
	_try_nested(CShellFrame, Frm, SetLockedFlag)
	{
		pThis->m_bLockedViews = bLocked;
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
// CShellFrame construction/destruction

CShellFrame::CShellFrame()
{
	EnableAggregation();
	EnableAutomation();

	m_sizeClient = CSize( 0, 0 );
	
	CMainFrame	*pfrm = (CMainFrame	*)AfxGetMainWnd();
	pfrm->RegisterMDIChild( this );
	m_pDocument = 0;
	m_bInitialized = false;
	m_bAlwaysZoomed = false;

	g_TargetManager.RegisterTarget( GetControllingUnknown(), szTargetFrame );

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	m_frameType = ftMDI;
	m_pFrameDockBar = NULL;

	m_bInMDIActivateState = false;

	m_bDestroyInProgress = false;
	m_bCreateComplete = false;

	m_bToolbarCreateProcess = false;


//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

	m_bLockedViews = FALSE;
}

CShellFrame::~CShellFrame()
{	
	DestroyToolBarList();

	CMainFrame	*pfrm = (CMainFrame	*)AfxGetMainWnd();

	pfrm->UnRegisterMDIChild( this );

	if( m_pDocument )
		m_pDocument->RemoveFrame( this );

	g_TargetManager.UnRegisterTarget( GetControllingUnknown() );

	
	ActivateLasMDIChild();


}

bool CShellFrame::ActivateLasMDIChild( bool bActivateOtherType /* false*/ )
{
	bool bFoundAtLeastOneMDIChildWnd = false;

	CMainFrame	*pfrm = (CMainFrame	*)AfxGetMainWnd();
	POSITION	pos = pfrm->GetFirstMDIChildPosition();

	

	if( !bActivateOtherType )
	{
		while( pos )
		{
			CMDIChildWnd	*pMDIChildWnd = pfrm->GetNextMDIChild( pos );
			if( pMDIChildWnd && pMDIChildWnd->IsKindOf( RUNTIME_CLASS( CShellFrame ) ) &&
				((CShellFrame*)pMDIChildWnd)->GetFrameType() == ftMDI
				)
				bFoundAtLeastOneMDIChildWnd = true;
		}

		if( !bFoundAtLeastOneMDIChildWnd )
			::FireEvent( ::GetAppUnknown(), szAppActivateView );
	}
	else
	{
		
		CShellFrame* pFrame = NULL;
		CShellFrame* pFrameFound = NULL;

		pos = pfrm->GetFirstChildFramePos();
		while( pos )
		{
			pFrame = pfrm->GetNextChildFrame( pos );
			if( pFrame != this && 
				( pFrame->GetFrameType() == ftMDI || pFrame->GetFrameType() == ftPDI ) 
				)
				pFrameFound = pFrame;
		}

		if( pFrameFound )
		{
			pfrm->SetNewActiveChild( NULL );
			pFrameFound->SetActive( TRUE );

			/*
			pfrm->SetNewActiveChild( pFrameFound );
			CView *pView = pFrameFound->GetActiveView();

			if( pView )
			{
				pView->SetFocus();
				::FireEvent( ::GetAppUnknown(), szAppActivateView, pView->GetControllingUnknown(), pView->GetDocument()->GetControllingUnknown() );

			}
			*/
			
		}
		return pFrameFound != NULL;
			
	}

	return bFoundAtLeastOneMDIChildWnd;
}

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
void CShellFrame::NotifyParent( CShellFrame* pNewChild )
{
	CMainFrame* pMainWnd = NULL;
	pMainWnd = GetMainFrame();

	if( pMainWnd )
	{
		pMainWnd->SetNewActiveChild( pNewChild );

		if( pNewChild )
		{
			if( pNewChild )
			{
				OnUpdateFrameMenu( TRUE, this, NULL );
				OnUpdateFrameTitle( TRUE );
			}
			else
			{
				OnUpdateFrameMenu( FALSE, this, NULL );
				OnUpdateFrameTitle( TRUE );
			}

			pMainWnd->DrawMenuBar();

		}
	}
}


CMainFrame* CShellFrame::GetMainFrame()
{
	CMainFrame* pMainFrame = NULL;
	
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	
	return pMainFrame;

}


BOOL CShellFrame::CreatePDI(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext) 
{	
	ASSERT( (m_frameType == ftPDI) || (m_frameType == ftDDI) );

	if (pParentWnd == NULL)
	{
		CWnd* pMainWnd = AfxGetThread()->m_pMainWnd;
		ASSERT(pMainWnd != NULL);
		ASSERT_KINDOF(CMDIFrameWnd, pMainWnd);
		pParentWnd = (CWnd*)pMainWnd;
	}
	
	BOOL b_result = CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, NULL, 0, pContext);

	// if available - get MDI child menus from doc template
	/*
	ASSERT(m_hMenuShared == NULL);      // only do once
	CMultiDocTemplate* pTemplate;
	if (pContext != NULL &&
		(pTemplate = (CMultiDocTemplate*)pContext->m_pNewDocTemplate) != NULL)
	{
		ASSERT_KINDOF(CMultiDocTemplate, pTemplate);
		// get shared menu from doc template
		m_hMenuShared = pTemplate->m_hMenuShared;
		m_hAccelTable = pTemplate->m_hAccelTable;
	}
	else
	{
		TRACE0("Warning: no shared menu/acceltable for MDI Child window.\n");
			// if this happens, programmer must load these manually
	}
	*/

	CenterWindow();

	return b_result;

}

int CShellFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	CMainFrame* pMainFrm = NULL;
	pMainFrm = GetMainFrame();
	ASSERT( pMainFrm  != NULL ); 

	pMainFrm->AddChild( this );
	

	CCreateContext* pContext = 0;
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;	
		pContext = (CCreateContext*)lpCreateStruct->lpCreateParams;
	}
	else
	{
		if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
			return -1;			
		MDICREATESTRUCT* lpmcs;
		lpmcs = (MDICREATESTRUCT*)lpCreateStruct->lpCreateParams;
		pContext = (CCreateContext*)lpmcs->lParam;
	}

	
	if( m_frameType == ftDDI )
	{			
		CMainFrame* pMainFrm = NULL;
		pMainFrm = GetMainFrame();
		ASSERT( pMainFrm  != NULL ); 

		m_pFrameDockBar = new CChildFrameBar( NULL );

		m_pFrameDockBar->SetInterface( NULL );

		pMainFrm->AddDockChildFrame( (void*)m_pFrameDockBar );

		if (!m_pFrameDockBar->Create(AfxGetMainWnd(), IDD_DUMMY, WS_CHILD|WS_VISIBLE, AFX_IDW_CONTROLBAR_FIRST + 32))
		//if (!m_pFrameDockBar->Create( "qqq", AfxGetMainWnd(), CSize( 100, 100 ), true, AFX_IDW_CONTROLBAR_FIRST + 32 ) )
		{
			TRACE0("Failed to create ControlBar\n");
			return -1;      // fail to create
		}
		
		m_pFrameDockBar->SetBarStyleEx( m_pFrameDockBar->GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );

		m_pFrameDockBar->EnableDocking( CBRS_ALIGN_ANY );
		//pMainFrm->DockControlBar( m_pFrameDockBar, AFX_IDW_DOCKBAR_LEFT );
		
		m_pFrameDockBar->SetOwner( GetMainFrame() );

		DWORD dwStyle	= WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE;
		DWORD dwExStyle	= 0;

		ModifyStyle( GetStyle(), dwStyle );
		ModifyStyleEx( GetExStyle(), dwExStyle );					
		
		SetParent( m_pFrameDockBar );
		CString	str;
		GetWindowText( str );
		m_pFrameDockBar->SetWindowText( str );

		//dock here
		CString	str1 = GetValueString( GetAppUnknown(), m_strDocTemplateName, "DockState1", 0 );
		CString	str2 = GetValueString( GetAppUnknown(), m_strDocTemplateName, "DockState2", 0 );

		CRect	rectPos;

		bool	bFound = true;
		int		nDockSite, nVisible;

		CSize	sizeVert, sizeHorz, sizeFloat;

		if( sscanf( str1, "%d%d%d%d%d%d", &rectPos.left, &rectPos.top, &rectPos.right, &rectPos.bottom, &nDockSite, &nVisible )!= 6 )
			bFound = false;
		if( sscanf( str2, "(%d %d) (%d %d) (%d %d)", &sizeVert.cx, &sizeVert.cy, &sizeHorz.cx, &sizeHorz.cy, 
			&sizeFloat.cx, &sizeFloat.cy ) != 6 )
			bFound = false;

		CShellDockBar	*pbar = m_pFrameDockBar;

		if( bFound )
		{
			sizeVert.cx = max( sizeVert.cx, 100 );
			sizeVert.cy = max( sizeVert.cy, 100 );

			sizeHorz.cx = max( sizeHorz.cx, 100 );
			sizeHorz.cy = max( sizeHorz.cy, 100 );

			sizeFloat.cx = max( sizeFloat.cx, 100 );
			sizeFloat.cy = max( sizeFloat.cy, 100 );

			rectPos.right =  max( rectPos.right, rectPos.left+100 );
			rectPos.bottom =  max( rectPos.bottom, rectPos.top+100 );

			CRect	rectDesktop;
			GetDesktopWindow()->GetClientRect( &rectDesktop );

			pbar->m_szVert = sizeVert;
			pbar->m_szHorz = sizeHorz;
			pbar->m_szFloat = sizeFloat;

			if( !nDockSite )
			{
				//pmain->ClientToScreen( rectPos );
				rectPos.right = rectPos.left+pbar->m_szFloat.cx;
				rectPos.bottom = rectPos.top+pbar->m_szFloat.cy;

				if( rectPos.left < rectDesktop.left )
					rectPos.OffsetRect( rectDesktop.left-rectPos.left, 0 );
				if( rectPos.right > rectDesktop.right )
					rectPos.OffsetRect( rectDesktop.right-rectPos.right, 0 );
				if( rectPos.top < rectDesktop.top )
					rectPos.OffsetRect( 0, rectDesktop.top-rectPos.top );
				if( rectPos.bottom > rectDesktop.bottom )
					rectPos.OffsetRect( 0, rectDesktop.bottom-rectPos.bottom );

				pMainFrm->FloatControlBar( pbar, rectPos.TopLeft() );
				pbar->GetParentFrame()->MoveWindow( rectPos );
			}
			else
			{
				if( nDockSite == AFX_IDW_DOCKBAR_LEFT ||
					nDockSite == AFX_IDW_DOCKBAR_RIGHT )
				{
					rectPos.right = rectPos.left+pbar->m_szHorz.cx;
					rectPos.bottom = rectPos.top+pbar->m_szHorz.cy;
				}
				else
				{
					rectPos.right = rectPos.left+pbar->m_szVert.cx;
					rectPos.bottom = rectPos.top+pbar->m_szVert.cy;
				}

				pbar->m_pDockContext->m_rectMRUDockPos = rectPos;
				CShellToolDockBar	*pshellDB = pMainFrm->GetDockSite( nDockSite );
				pshellDB->AddDockBar( pbar );
			}

			pMainFrm->ShowControlBar( pbar, nVisible, true );
			pbar->SendMessage( WM_SHOWWINDOW, nVisible );

			pbar->m_szVert = sizeVert;
			pbar->m_szHorz = sizeHorz;
			pbar->m_szFloat = sizeFloat;

		}
		else
			pMainFrm->DockControlBar( m_pFrameDockBar, AFX_IDW_DOCKBAR_LEFT );



	}

	EnableDocking(CBRS_ALIGN_TOP);
	// A.M 11.03.2003. Когда работает OnNewDocument, активное окно должно быть установлено,
	// нужно для создания новой базы.
	SetActive(TRUE);


	m_bCreateComplete = true;
	
	
	if (!OnCreateClient(lpCreateStruct, pContext))
		return -1;

	// [max] & [vanek] 23.01.2004: обновление кнопок вьюхи(minimize,maximize, close) на менюбаре, т.к. при создании 
	// докбаров вьюхи (EnableDocking - см. выше) сама вьюха еще не создана и соответсвенно не активна, поэтому 
	// вся инфа берется из текущей активной вьюхи. Данная хрень проявляется, когда у текущей активной вьюхи кнопок нет,
    // т.е. в настройках соответствующего этой вьюхе документа параметр Flags = 8 (SDI), и у создаваемой вьюхи у Flags
	// 3-й бит = 0, при этом получается, что активизится вьюха, максимизиться, а кнопок на меню баре нет. 
    OnUpdateFrameMenu( FALSE, 0, NULL );
	CMainFrame* pMainWnd = NULL;
	pMainWnd = GetMainFrame();
	if( pMainWnd )
		GetMainFrame()->DrawMenuBar();
	

	g_CmdManager.GatherShellFrameToolbarsInfo( this );
	g_CmdManager.CreateShellFrameToolbars( this );


	//_RecalcLayoyt( true );

	return 0;
}



void CShellFrame::ActivateFrame(int nCmdShow) 
{	
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		CFrameWnd::ActivateFrame(nCmdShow);
	else
		CMDIChildWnd::ActivateFrame(nCmdShow);	
}

LRESULT CShellFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		return CFrameWnd::DefWindowProc(message, wParam, lParam);
	else
		return CMDIChildWnd::DefWindowProc(message, wParam, lParam);
}

CMDIFrameWnd* CShellFrame::GetMDIFrame()
{
	
	return GetMainFrame();
	/*
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{
		if(!IsWindow(m_hWnd)) return NULL;

		CWnd* pOwner = NULL;

		pOwner = GetOwner();
		if(!pOwner) return NULL;
		
		return (CMDIFrameWnd*)pOwner;
	}
	else
		return CMDIChildWnd::GetMDIFrame();	
		*/

}

CWnd* CShellFrame::GetMessageBar()
{
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		return CFrameWnd::GetMessageBar();
	else
		return CMDIChildWnd::GetMessageBar();
	
}

void CShellFrame::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		CFrameWnd::OnWindowPosChanging(lpWndPos);
	else
	{
		CMainFrame	*pMainFrame = (CMainFrame*)AfxGetMainWnd();
		//pMainFrame->UpdateClientStyle();

		if( pMainFrame->IsXPBarAvailable() )
			CFrameWnd::OnWindowPosChanging(lpWndPos);
		else
			CMDIChildWnd::OnWindowPosChanging(lpWndPos);

	
	}
	
}


void CShellFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	//if ((GetStyle() & FWS_ADDTOTITLE) == 0)
	//	return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (!m_strUserTitle.IsEmpty())
			lstrcpy(szText, m_strUserTitle);
		else if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, pDocument->GetTitle());
		if (m_nWindow > 0)
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);


		//DDI set title
		if( GetFrameType() == ftDDI )
		{
			if( m_pFrameDockBar != NULL )
			{
				if( ::IsWindow( m_pFrameDockBar->GetSafeHwnd() ) )
				{
					m_pFrameDockBar->SetWindowText( szText );
					m_pFrameDockBar->SetName( szText );
				}
			}
		}


		// set title if changed, but don't remove completely
		AfxSetWindowText(m_hWnd, szText);
	}
}


void CShellFrame::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd,
	HMENU hMenuAlt)
{

	////////////////////////////
	CMDIFrameWnd* pFrame = GetMDIFrame();

	if (hMenuAlt == NULL && bActivate)
	{
		// attempt to get default menu from document
		CDocument* pDoc = GetActiveDocument();
		if (pDoc != NULL)
			hMenuAlt = pDoc->GetDefaultMenu();
	}

	// use default menu stored in frame if none from document
	if (hMenuAlt == NULL)
		hMenuAlt = m_hMenuShared;

	if (hMenuAlt != NULL && bActivate)
	{
		ASSERT(pActivateWnd == this);

		// activating child, set parent menu
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)hMenuAlt, (LPARAM)pFrame->GetWindowMenuPopup(hMenuAlt));
	}
	else if (hMenuAlt != NULL && !bActivate && pActivateWnd == NULL)
	{
		// destroying last child
		HMENU hMenuLast = NULL;
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)pFrame->m_hMenuDefault, (LPARAM)hMenuLast);
	}
	else
	{
		// refresh MDI Window menu (even if non-shared menu)
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	}

	return;
}


BOOL CShellFrame::DestroyWindow() 
{	

	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if( pMainFrame )
	{
		ASSERT_KINDOF( CMainFrame, pMainFrame );

		if( pMainFrame->IsSDIMode() )
			return FALSE;
	}

	
	if( m_pFrameDockBar&&m_pFrameDockBar->GetSafeHwnd() )
	{
		SetParent( pMainFrame );
		m_pFrameDockBar->DestroyWindow();		

		pMainFrame->RemoveDockChildFrame( m_pFrameDockBar );		
		
		delete m_pFrameDockBar;
		m_pFrameDockBar = 0;

	}

	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{
		return CFrameWnd::DestroyWindow();
	}
	else
	{
		return CMDIChildWnd::DestroyWindow();
	}

}


int CShellFrame::OnMouseActivate(CWnd* pActivateWnd, UINT nHitTest, UINT message)
{

	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		return CFrameWnd::OnMouseActivate(pActivateWnd, nHitTest, message);
	
	return CMDIChildWnd::OnMouseActivate( pActivateWnd, nHitTest, message );
}



void CShellFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{

	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{	
		CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	}
	else	
		CMDIChildWnd::OnActivate( nState, pWndOther, bMinimized );

}


BOOL CShellFrame::PreTranslateMessage(MSG* pMsg) 
{	
	if( pMsg->message >= WM_KEYFIRST &&
		pMsg->message <= WM_KEYLAST )
	{
		CView		*pview = GetActiveView();
		if( !pview )return false;
		ASSERT( pview->IsKindOf( RUNTIME_CLASS( CShellView ) ) );
		CShellView	*pshellView = (CShellView*)pview;
		IUnknown	*punkVW = pshellView->GetViewUnknown();
		IWindowPtr	ptrWindow( punkVW );
		punkVW->Release();

		if( ptrWindow->PreTranslateAccelerator( pMsg ) == S_OK )
			return true;
	}


	BOOL bResult;

	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		bResult = CFrameWnd::PreTranslateMessage(pMsg);
	else
		bResult = CMDIChildWnd::PreTranslateMessage(pMsg);


/*	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		// use document specific accelerator table over m_hAccelTable
		HACCEL hAccel = GetDefaultAccelerator();
		if( hAccel != NULL )
		   if( ::TranslateAccelerator( AfxGetMainWnd(), hAccel, pMsg ) )
			   return 0;

	}*/
		 
	if(
		pMsg->message == WM_SYSKEYDOWN ||  
		pMsg->message == WM_LBUTTONDOWN ||  
		pMsg->message == WM_RBUTTONDOWN ||  
		pMsg->message == WM_MBUTTONDOWN ||  
		pMsg->message == WM_NCLBUTTONDOWN ||  
		pMsg->message == WM_NCRBUTTONDOWN ||  
		pMsg->message == WM_NCMBUTTONDOWN 
		)
	{			
		HWND hWnd = 0;
		hWnd = GetSafeHwnd();

		if( pMsg->hwnd == hWnd )
		{
			SetActive( TRUE );
			DefWindowProc( pMsg->message, pMsg->wParam, pMsg->lParam );			
			return TRUE;
			
		}

	}


  
	return bResult;
  
}

class _CCallCounterImpl
{
public:
	int *m_pnCounter;
	_CCallCounterImpl(int *pnCounter)
	{
		m_pnCounter = pnCounter;
		(*m_pnCounter)++;
	}
	~_CCallCounterImpl()
	{
		(*m_pnCounter)--;
	}
};

BOOL CShellFrame::SetActive( BOOL bActive )
{	
	static int s_nCallCounter = 0;
	if (s_nCallCounter > 0) return FALSE;
	_CCallCounterImpl CCnt(&s_nCallCounter);
	if( m_bDestroyInProgress )
		return FALSE;

	if( bActive && m_frameType == ftDDI && !IsWindowVisible() )
	{		
		return FALSE;
	}


	if( !bActive )
	{
		return FALSE;
	}

	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CShellFrame* pOldFrame = NULL;
	if( pMainFrame )
		pOldFrame = pMainFrame->GetActiveChildFrame();	

	
	if( pOldFrame && ( pOldFrame != this ) )
	{		
		CShellView* pOldView = (CShellView*)pOldFrame->GetActiveView();
		if( pOldView )
		{
			bool bVal = pOldFrame->IsInMDIActivateState();
			pOldFrame->SetInMDIActivateState( true );
			pOldView->OnActivateView(FALSE, pOldView, pOldView );				
			pOldFrame->SetInMDIActivateState( bVal );
			
		}
	}

	//Force CView::SetFocus
	CShellView* pView = NULL;
	pView = (CShellView*)GetActiveView( );


	if( pView )
	{	
		if( (GetFrameType() == ftDDI) &&
			(IsFloatingDDIFrame() )
			) 
		{
			
			SetActiveView( pView, FALSE );
			pView->SetFocus( );
		}		
	}



	if( (GetMainFrame()->GetActiveChildFrame() == NULL) ||
		(GetMainFrame()->GetActiveChildFrame() != this)
		)
	{		

		NotifyParent( this );		

		//Force CView::OnActivateView
		CShellView* pView = NULL;
		pView = (CShellView*)GetActiveView( );

		
		if( pView )
		{	
			if( (GetFrameType() == ftDDI) &&
				(IsFloatingDDIFrame() )
				) 
			{
				pView->SetFocus( );
			}

			m_bInMDIActivateState = true;			
			/*
			if( pOldView )
				pOldView->OnActivateView(FALSE, pOldView, pView );
				*/
			//Reactivate view
			pView->OnActivateView(TRUE, pView, GetActiveView() );				
			m_bInMDIActivateState = false;
			//SetActiveView( pView, TRUE );
		}
		
		
		if( bActive )
			g_TargetManager.ActivateTarget( GetControllingUnknown() );


		//get the action manager
		IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
		sptrIActionManager	sptrAM( punkAM );
		punkAM->Release();

		DWORD	dwExecuteFlags = 0;
		sptrAM->GetCurrentExecuteFlags( &dwExecuteFlags );

		
		if( bActive && !(dwExecuteFlags&aefRunning) &&
			!(dwExecuteFlags&aefScriptRunning))
		{
			CString	strText;

			GetWindowText( strText );

			CString	strParam;

			strParam = CString( "\"" )+strText+CString( "\"" );
			::ExecuteAction( "WindowActivate", strParam );
		}

		if( bActive && pOldFrame != this )
		{
			CShellDocManager *pman = theApp.GetDocManager();
			
			CDocument	*pdoc = pman->GetDispatchImpl()->GetActiveDocument();

			if( pdoc )

			{
				static int call_counter = 0;

				if( !call_counter )
				{
					call_counter++;
					pman->GetDispatchImpl()->FireOnActivateDocument( pdoc->GetIDispatch( false ) );
					call_counter--;
				}

			}
		}		



		return TRUE;
	}
	
	
	return FALSE;

}

void CShellFrame::OnParentNotify(UINT message, LPARAM lParam) 
{
	CMDIChildWnd::OnParentNotify(message, lParam);
	// A.M. 11.03.2003 Если из скрипта из OnActivateDocument вызывается ToolsLoadState
	// (как в эксперте), то изнутри ToolsLoadState при удалении докбаров CShellFrame
	// приходит CShellFrame::OnParentNotify, из него вызывается SetActive, а из него снова
	// OnActivateDocument в скрипте и ToolsLoadState, в результате все падает (докбар в
	// момент вызова уже уничтожена, но в списках присутствует).
//	SetActive( TRUE );
}



BOOL CShellFrame::OnNcActivate(BOOL bActive) 
{
	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CWnd* pActiveWnd = NULL;
	if( pMainFrame )
		pActiveWnd = pMainFrame->_GetActiveFrame();
		
	if( pMainFrame )
	{
		ASSERT_KINDOF( CMainFrame, pMainFrame );

		if( pMainFrame->IsSDIMode() )
		{
			if( pActiveWnd != (CWnd*)this )
				return FALSE;
		}
	}

	if( bActive )
		SetActive( bActive );

	return CMDIChildWnd::OnNcActivate(bActive);
}



BOOL CShellFrame::IsFloatingDDIFrame()
{
	if( GetFrameType() == ftDDI )
	{
		if( m_pFrameDockBar != NULL )
		{
			if( m_pFrameDockBar->GetMiniDockWnd() != NULL )
				return TRUE;
		}
	}
	return FALSE;
}
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************



//register view
void CShellFrame::AddView( CShellView *pview )
{
	ASSERT( !m_listViews.Find( pview ) );
	m_listViews.AddHead( pview );
}

//unregister view
void CShellFrame::RemoveView( CShellView *pview )
{
	POSITION	pos = m_listViews.Find( pview );

	ASSERT( pos );
	m_listViews.RemoveAt( pos );
}

//access to the views list
POSITION CShellFrame::GetFirstViewPosition()
{
	POSITION	pos = m_listViews.GetHeadPosition();

	return pos;
}
//return view and get next poss
CShellView *CShellFrame::GetNextView( POSITION &rPos )
{
	return (CShellView*)m_listViews.GetNext( rPos );
}


BOOL CShellFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_strClassName = ::AfxRegisterWndClass( CS_DBLCLKS|CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1) );
	cs.lpszClass = m_strClassName;

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |	
	static int x = 0;
	static int y = 0;


	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{
		x += 10;
		y += 10;

		cs.style |= WS_POPUP;	
		cs.style &= ~WS_CHILD;	
		cs.cx = cs.cy = 300;
		cs.x = x;
		cs.y = y;

		CMainFrame* pMainFrame = NULL;
		pMainFrame = GetMainFrame();

		if( pMainFrame )
			cs.hwndParent = pMainFrame->GetSafeHwnd();

		return CFrameWnd::PreCreateWindow(cs);
	}
	else
	{
		DWORD	dwDocFlags = ::GetValueInt( GetAppUnknown(), m_strDocTemplateName, "Flags", 0 );
		if( dwDocFlags & dtfSDIWindow )
		{
			cs.style = WS_CHILD;
			m_bAlwaysZoomed = true;
		}
		return CMDIChildWnd::PreCreateWindow(cs);
	}
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CShellFrame diagnostics

#ifdef _DEBUG
void CShellFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CShellFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShellFrame message handlers

BOOL CShellFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	



	ASSERT(m_hMenuShared == NULL);      // only do once

	m_nIDHelp = -1;	// no ID for help context 

	// parent must be MDI Frame (or NULL for default)
	ASSERT(pParentWnd == NULL || pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	// will be a child of MDIClient
	ASSERT(!(dwDefaultStyle & WS_POPUP));
	dwDefaultStyle |= WS_CHILD;

	// if available - get MDI child menus from doc template
	ASSERT(m_hMenuShared == NULL);      // only do once
	
	ASSERT( pContext != NULL && pContext->m_pNewDocTemplate != NULL );
	ASSERT_KINDOF(CShellDocTemplate, pContext->m_pNewDocTemplate);

	CShellDocTemplate	*pTemplate = 
		(CShellDocTemplate*)pContext->m_pNewDocTemplate;

	// get shared menu from doc template
	m_hMenuShared = pTemplate->m_hMenuShared;
	m_hAccelTable = pTemplate->m_hAccelTable;

	pTemplate->GetDocString( m_strDocTemplateName, CDocTemplate::docName );

	ASSERT(m_hWnd == NULL);

	//Default doc style
	m_frameType = ftMDI;

	CString strFrameType = 
		::GetValueString( ((CShellApp*)AfxGetApp())->GetControllingUnknown(), m_strDocTemplateName, "FrameType", "MDI");

	if( strFrameType == "MDI" )
		m_frameType = ftMDI;

	if( strFrameType == "PDI" )
		m_frameType = ftPDI;
	
	if( strFrameType == "DDI" )
		m_frameType = ftDDI;


	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
	{

		if (!CreatePDI(GetIconWndClass(dwDefaultStyle, 1001),
				m_strDocTemplateName, dwDefaultStyle, rectDefault,
				(CWnd*)pParentWnd, pContext))
		{
			return FALSE;   // will self destruct on failure normally
		}

	}
	else
	{
		if( !Create( GetIconWndClass(dwDefaultStyle, 1001),
						m_strDocTemplateName, dwDefaultStyle, rectDefault,
						(CMDIFrameWnd*)pParentWnd, pContext ) )
		{
			return FALSE;   // will self destruct on failure normally
		}
		DWORD	dwDocFlags = ::GetValueInt( GetAppUnknown(), m_strDocTemplateName, "Flags", 0 );
		if( dwDocFlags & dtfSDIWindow )ShowWindow( SW_MAXIMIZE );
	}


	m_bToolbarCreateProcess = true;

	_RecalcLayoyt( true );

	m_bToolbarCreateProcess = false;

		
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************
	
	
		/*
		
		ASSERT(m_hMenuShared == NULL);      // only do once

		m_nIDHelp = -1;	// no ID for help context 

		// parent must be MDI Frame (or NULL for default)
		ASSERT(pParentWnd == NULL || pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
		// will be a child of MDIClient
		ASSERT(!(dwDefaultStyle & WS_POPUP));
		dwDefaultStyle |= WS_CHILD;

		// if available - get MDI child menus from doc template
		ASSERT(m_hMenuShared == NULL);      // only do once
		
		ASSERT( pContext != NULL && pContext->m_pNewDocTemplate != NULL );
		ASSERT_KINDOF(CShellDocTemplate, pContext->m_pNewDocTemplate);

		CShellDocTemplate	*pTemplate = 
			(CShellDocTemplate*)pContext->m_pNewDocTemplate;

		// get shared menu from doc template
		m_hMenuShared = pTemplate->m_hMenuShared;
		m_hAccelTable = pTemplate->m_hAccelTable;

		CString	strTitle;
		pTemplate->GetDocString( strTitle, CDocTemplate::docName );

		ASSERT(m_hWnd == NULL);



		if( !Create( GetIconWndClass(dwDefaultStyle, 1001),
						strTitle, dwDefaultStyle, rectDefault,
						(CMDIFrameWnd*)pParentWnd, pContext ) )
		{
			return FALSE;   // will self destruct on failure normally
		}
		*/
	

	//register target 
	g_TargetManager.RegisterTarget( GetControllingUnknown(), "anywindow\nmdiwindow" );	
	
	// it worked !
	return TRUE;
}

BOOL CShellFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if( !m_bCreateComplete )return true;
	//m_rectBorder.top = 20;
	ASSERT(m_hWnd != NULL);
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pContext != NULL);
	ASSERT(pContext->m_pCurrentDoc != 0 );

	ASSERT_KINDOF(CShellDoc, pContext->m_pCurrentDoc);
	ASSERT_KINDOF(CShellDocTemplate, pContext->m_pNewDocTemplate);

	CShellDocTemplate	*pdocTemplate = (CShellDocTemplate *)pContext->m_pNewDocTemplate;
	CShellDoc			*pdoc = (CShellDoc *)pContext->m_pCurrentDoc;

	m_pDocument = pdoc;

	ASSERT( m_pDocument );
	m_pDocument->AddFrame( this );


	CString	strName, str;

	{
		const char szSplitterProgID[] = "StdSplitter.SplitterWindow";
		pContext->m_pNewDocTemplate->GetDocString( strName, CDocTemplate::docName );
		str = ::GetValueString( GetAppUnknown(), strName, "Splitter", szSplitterProgID );
	}

	if( m_sptrSplitter.CreateInstance( str ) != S_OK )
		return FALSE;

	//attach parent window
	IRootedObjectPtr	sptrRooted( m_sptrSplitter );
	sptrRooted->AttachParent( GetControllingUnknown() );

	sptrIWindow	sptrW( m_sptrSplitter );

	CRect	rc;
	GetClientRect( rc );
	sptrW->CreateWnd( GetSafeHwnd(), rc, WS_CHILD|WS_VISIBLE, 1002 );

	m_bInitialized = true;

	return TRUE;
}

void CShellFrame::OnClose() 
{
	CDocument* pDocument = GetActiveDocument();

/*	if( pDocument )
	{
		CShellDocManager *pman = theApp.GetDocManager();

		VARIANT varRes;
		varRes.vt = VT_BOOL;
		varRes.boolVal = true;

		if( !pman )
			return;
		pman->GetDispatchImpl()->FireOnCloseDocument( pDocument->GetIDispatch( false ), &varRes );

		if( !varRes.boolVal )
			return;
	}
*/
	// A.M. SBT1417
	if( m_frameType != ftPDI && m_frameType != ftDDI )
	{
		DWORD	dwDocFlags = ::GetValueInt( GetAppUnknown(), m_strDocTemplateName, "Flags", 0 );
		if( dwDocFlags & dtfSDIWindow )
			return;
	}
	if (pDocument != NULL && !pDocument->CanCloseFrame(this))
	{
		// document can't close right now -- don't close it
		return;
	}

	// detect the case that this is the last frame on the document and
	// shut down with OnCloseDocument instead.
	if (pDocument != NULL && pDocument->m_bAutoDelete)
	{
		BOOL bOtherFrame = FALSE;
		POSITION pos = pDocument->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView->GetParentFrame() != this)
			{
				bOtherFrame = TRUE;
				break;
			}
			else
			{
/*				m_XFrm::DestroyView( pView->GetC )
				pView*/
			}
		}

		GetSplitterWnd()->SendMessage(WM_CLOSE);

		if (!bOtherFrame)
		{
			::ExecuteAction( "FileClose" );
			return;
		}
		else
		{
			::ExecuteAction( "WindowClose" );
//			DestroyWindow();
		}
	}
}


void CShellFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	if( pActivateWnd != pDeactivateWnd )
		m_bInMDIActivateState = true;

//???
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	CMainFrame	*pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->UpdateClientStyle();

	m_bInMDIActivateState = false;


}

void CShellFrame::OnFinalRelease() 
{
	CMDIChildWnd::OnFinalRelease();
}

void CShellFrame::PostNcDestroy() 
{
	delete this;
}

CWnd *CShellFrame::GetSplitterWnd()
{
	if( !m_bInitialized )
		return 0;

	sptrIWindow	sptr( m_sptrSplitter );

	HWND	hwnd = 0;

	sptr->GetHandle( (HANDLE*)&hwnd );
	return CWnd::FromHandle( hwnd );
}

void CShellFrame::OnSize(UINT nType, int cx, int cy) 
{
//							MDI/PDI/DDI		support.
	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		CFrameWnd::OnSize(nType, cx, cy);	
	else
		CMDIChildWnd::OnSize(nType, cx, cy);
	DoRecalcLayout(nType,cx,cy);
}

static bool b_inside_resize = false;
void CShellFrame::DoRecalcLayout(UINT nType, int cx, int cy) 
{
	b_inside_resize = true;
	int nToolBarHeight = 0;
	bool bHaveToolbar = m_ptrToolBarList.GetCount() > 0;
	if( bHaveToolbar )
	{
		POSITION pos = GetFirstToolBarPos();
		CShellFrameToolBar* ptb = GetNextToolBar( pos );
		if( ptb && ptb->GetSafeHwnd() )
		{
			if( !((CMainFrame*)AfxGetMainWnd())->IsInOleEditMode() || m_bToolbarCreateProcess )
			{
				nToolBarHeight = ptb->CalcFixedLayout( TRUE, TRUE ).cy;
			}
			else
				nToolBarHeight = 0;

			bool bVis = ( 0 != ( ptb->GetStyle() & WS_VISIBLE ) );
			if( !m_bToolbarCreateProcess && !bVis )
				nToolBarHeight = 0;				
		}
		
		cy -= nToolBarHeight;
	}
	
	
	CWnd	*pwnd = GetSplitterWnd();

	if( !pwnd )
	{
		b_inside_resize = false;
		return;
	}

	CRect	rc;

	GetClientRect( rc );
	if( bHaveToolbar )
	{
		POSITION pos = GetFirstToolBarPos();
		CShellFrameToolBar* ptb = GetNextToolBar( pos );
		if( ptb && ptb->GetSafeHwnd() )
		{						
			ptb->MoveWindow( 0, 0, cx, nToolBarHeight );
		}
	}

	rc.top += nToolBarHeight;	

	pwnd->MoveWindow( rc );

	DWORD	dwRedrawFlags = RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN/*|RDW_UPDATENOW*/;
	if( m_sizeClient.cx < cx )
		RedrawWindow( CRect( m_sizeClient.cx, 0, cx, cy), 0, dwRedrawFlags );
	if( m_sizeClient.cy < cy )
		RedrawWindow( CRect( 0, m_sizeClient.cy, cx, cy ), 0, dwRedrawFlags );
	
	m_sizeClient = CSize( cx, cy );	

	b_inside_resize = false;
}


void CShellFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	/*CRect	rcClient;
	GetClientRect( rcClient );

	dc.FillRect( rcClient, &CBrush( RGB( 0, 255, 0 ) ) );
	dc.Ellipse( rcClient );	
	dc.Ellipse( rcClient );	
	dc.DrawText( "no splitter, sorry...", rcClient, DT_CENTER|DT_VCENTER|DT_SINGLELINE );*/
	// Do not call CMDIChildWnd::OnPaint() for painting messages
}

CDocument* CShellFrame::GetActiveDocument()
{
	return m_pDocument;
}

BOOL CShellFrame::OnEraseBkgnd(CDC* pDC) 
{
	//return CMDIChildWnd::OnEraseBkgnd(pDC);
	return false;
}

void CShellFrame::OnDestroy()
{
	m_bDestroyInProgress = true;

	g_CmdManager.UpdateShellFrameToolbarInfo( this );

	DestroyAllToolBars();
	//CMDIChildWnd::OnDestroy();

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	

	CMainFrame* pMainFrm = NULL;
	pMainFrm = GetMainFrame();
	ASSERT( pMainFrm  != NULL ); 

	pMainFrm->RemoveChild( this );

	NotifyParent( NULL );	

	{//if DocTempl->OpenDocumentFile() == 0
		if( !pMainFrm->GetFirstChildFramePos() )
		{
			CMDIFrameWnd* pFrame = GetMDIFrame();
			if( pFrame )
			{
				::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
				(WPARAM)pFrame->m_hMenuDefault, (LPARAM)0 );
			}
		}
	}


	if( m_frameType == ftDDI )
		pMainFrm->RecalcLayout( );


	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )	
		CFrameWnd::OnDestroy();	
	else
	{
		CMainFrame	*pMainFrame = (CMainFrame*)AfxGetMainWnd();

		if( pMainFrame->IsXPBarAvailable() )
			CFrameWnd::OnDestroy();
		else
			CMDIChildWnd::OnDestroy();
	}
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************
	g_TargetManager.UnRegisterTarget( GetControllingUnknown() );	
}

void CShellFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CPoint	ptTrackSize = lpMMI->ptMinTrackSize;

	CWnd	*pwndSplitter = GetSplitterWnd();
	if( pwndSplitter )						  
	{
		pwndSplitter->SendMessage(WM_GETMINMAXINFO, 0, (LPARAM)lpMMI);
		//add header and bordes
		CRect	rcWindow;
		GetWindowRect( &rcWindow );
		CRect	rcClient;
		pwndSplitter->GetWindowRect( &rcClient );

		lpMMI->ptMinTrackSize.x = rcWindow.Width()-rcClient.Width();

// [Max] Соглсен. Не круто, но стойко работает. 
// При ресторе окна из всернутого состояния чуть прячется горизонтальный скроллер теперь све ок. 
		lpMMI->ptMinTrackSize.y = rcWindow.Height()-rcClient.Height() - 16; 

		ptTrackSize = lpMMI->ptMinTrackSize;
	} 
	CMDIChildWnd::OnGetMinMaxInfo( lpMMI );

	lpMMI->ptMinTrackSize = ptTrackSize;

}


void CShellFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if( pMainFrame )
	{
		ASSERT_KINDOF( CMainFrame, pMainFrame );

		if( pMainFrame->IsSDIMode() )
		{
			if (
				nID == SC_CLOSE || 
				nID == SC_MINIMIZE ||
				nID == SC_NEXTWINDOW ||
				nID == SC_PREVWINDOW ||
				nID == SC_SIZE ||
				nID == SC_MOVE ||
				nID == SC_RESTORE 
				)
			{
				return;
			}
		}
	}

	
	
	CMDIChildWnd::OnSysCommand(nID, lParam);
}

BEGIN_DISPATCH_MAP(CShellFrame, CMDIChildWnd)
	//{{AFX_DISPATCH_MAP(CShellFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		DISP_FUNCTION(CShellFrame, "GetSplitter", GetSplitter, VT_DISPATCH, VTS_NONE)
		DISP_PROPERTY_NOTIFY(CShellFrame, "Title", m_strUserTitle, OnUserTitleChanged, VT_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

LPDISPATCH CShellFrame::GetSplitter()
{
	IDispatch* pdisp = 0;
	if(m_sptrSplitter != 0)
	{
		m_sptrSplitter->QueryInterface(IID_IDispatch, (void**)&pdisp);
		//_ReportCounter( pdisp );
	}
	return pdisp;
}

void CShellFrame::OnUserTitleChanged()
{
	OnUpdateFrameTitle(TRUE);
}


void CShellFrame::OnSetFocus(CWnd* pOldWnd) 
{
  CMDIChildWnd::OnSetFocus(pOldWnd);
	
	CWnd	*pwnd = ::GetWindowFromUnknown( m_sptrSplitter );
	if( pwnd )pwnd->SetFocus();

  AfxGetApp()->OnIdle(0);
}

BOOL CShellFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{	
	if( GetToolBarCount() )
	{
		if( nID >= ID_CMD_POPUP_MIN && nID <= ID_CMD_POPUP_MAX )
			return true;

		if( g_CmdManager.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
		{
			return true;
		}
	}

	if( (m_frameType == ftPDI) || (m_frameType == ftDDI) )
		return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	
		

	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Shell Tool bar functions
//
/////////////////////////////////////////////////////////////////////////////

extern const char *szEndmarker;

CShellFrameToolBar* CShellFrame::CreateToolBar( CToolBarInfo* pti )
{
	if( !pti )
		return NULL;

	//paul 29.03.2002
	CString strName;// = pti->GetName();
	

	CDocument* pDoc = GetActiveDocument();
	if( pDoc )
	{
		strName = pDoc->GetTitle();
		IDocumentSitePtr ptrDS( pDoc->GetControllingUnknown() );

		if( ptrDS )
		{		
			IUnknown* punkDT = 0;
			ptrDS->GetDocumentTemplate( &punkDT );
			if( punkDT )
			{				
				IDocTemplatePtr ptrDT( punkDT );
				punkDT->Release();

				if( ptrDT )
				{				
					BSTR bstr = 0;			 
					ptrDT->GetDocTemplString( CDocTemplate::fileNewName, &bstr );
					
					strName += " ";
					strName += CString( bstr );
					::SysFreeString( bstr );
				}
			}
		}
	}


	CShellFrameToolBar* pBar = new CShellFrameToolBar();
	if( !pBar )
		return NULL;

	DWORD dwCtrlStyle = TBSTYLE_FLAT;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_ORIENT_HORZ;
	
	
	VERIFY( pBar->CreateEx( this, dwCtrlStyle, CBRS_TOOLTIPS|WS_CHILD|WS_VISIBLE, NORECT, AFX_IDW_CONTROLBAR_FIRST + 32) );		
	pBar->SetWindowText( (LPCSTR)strName );



	
	DWORD dwTBStyle = pBar->GetBarStyle();
	dwTBStyle |= CBRS_ORIENT_HORZ;
	dwTBStyle &= ~CBRS_GRIPPER;
	

	pBar->SetBarStyleEx(  dwTBStyle );	

	{
#if _MSC_VER >= 1300		
			CToolTipCtrl* pToolTip = 0;
			//17.12.2002 paul
			//pToolTip = AfxGetModuleThreadState()->m_pToolTip;
			//AfxGetModuleThreadState()->m_pToolTip = 0;
#else
			CToolTipCtrl* pToolTip = AfxGetThreadState()->m_pToolTip;
			AfxGetThreadState()->m_pToolTip = 0;
#endif
		if( pToolTip )
		{
			pToolTip->DestroyWindow();
			delete pToolTip;
			pToolTip = 0;
		}
		//pToolTip->SendMessage(TTM_ACTIVATE, TRUE);

	}

	
	
	/*
	pBar->EnableDocking( CBRS_ALIGN_TOP );			
	DockControlBar( pBar, pti->GetDocPos(), &pti->GetRect() );	
	*/	

	m_ptrToolBarList.AddTail( pBar );

	if( pti->m_arBtnString.GetSize() > 0 )
	{
		
		int idx = 0;
		while( true )
		{
			
			if( idx >= pti->m_arBtnString.GetSize() )
				break;

			CString	str = pti->m_arBtnString[idx];

			if( str.IsEmpty() || str[0] == '\'' )
				continue;

			if( str == szEndmarker )
			{
				break;
			}			
		
			CBCGToolbarButton	*pbtn = _create_button_from_string( pti->m_arBtnString, idx, false );
			if( pbtn )
			{
				pBar->InsertButton( *pbtn );
				delete pbtn;	pbtn = 0;

			}
		}
	}

	//RecalcLayout();
	{
		
		m_bToolbarCreateProcess = true;

		_RecalcLayoyt( true );

		m_bToolbarCreateProcess = false;
		
	}


	return pBar;
}

/////////////////////////////////////////////////////////////////////////////
void CShellFrame::_RecalcLayoyt( bool bOnlyIfToolbarExist )
{
	if( bOnlyIfToolbarExist && m_ptrToolBarList.GetCount() <= 0 )
		return;
		
	CRect rect;
	if( IsWindow( m_hWnd ) )
	{
		GetClientRect( &rect );
		DoRecalcLayout( 0, rect.Width(), rect.Height() );
	}

}

/////////////////////////////////////////////////////////////////////////////
int CShellFrame::GetToolBarCount()
{
	return (int)m_ptrToolBarList.GetCount();
}

/////////////////////////////////////////////////////////////////////////////
POSITION CShellFrame::GetFirstToolBarPos()
{
	return m_ptrToolBarList.GetHeadPosition();		
}

/////////////////////////////////////////////////////////////////////////////
CShellFrameToolBar* CShellFrame::GetNextToolBar( POSITION& pos )
{
	return (CShellFrameToolBar*)m_ptrToolBarList.GetNext( pos );
}

/////////////////////////////////////////////////////////////////////////////
void CShellFrame::DestroyAllToolBars()
{
	POSITION pos = m_ptrToolBarList.GetHeadPosition();
	while( pos )
	{
		
		POSITION posSave = pos;
		CShellFrameToolBar* pBar = (CShellFrameToolBar*)m_ptrToolBarList.GetNext( pos );
		pBar->DestroyWindow();
		delete pBar;   pBar = 0;
	}

	m_ptrToolBarList.RemoveAll();
}

/*
/////////////////////////////////////////////////////////////////////////////
void CShellFrame::RemoveToolBar(LPCTSTR szToolBarName)
{
	POSITION pos2Del = 0;
	CShellFrameToolBar* pBar2Del = 0;

	POSITION pos = m_ptrToolBarList.GetHeadPosition();
	while( pos )
	{
		
		POSITION posSave = pos;
		CShellFrameToolBar* pBar = (CShellFrameToolBar*)m_ptrToolBarList.GetNext( pos );
		CString strTitle;
		pBar->GetWindowText( strTitle );
		if( strTitle == szToolBarName )
		{
			pBar2Del = pBar;
			pos2Del = posSave;
			pos = 0;
		}		
	}

	if( pos2Del )
	{
		if( pBar2Del )
		{
			pBar2Del->DestroyWindow();
			delete pBar2Del;
			pBar2Del = 0;
		}

		m_ptrToolBarList.RemoveAt( pos2Del );

		RecalcLayout();
		g_CmdManager.UpdateShellFrameToolbarInfo( this );
	}

	return;
}

/////////////////////////////////////////////////////////////////////////////
void CShellFrame::AddToolBar(LPCTSTR szToolBarName)
{
	CToolBarInfo ti( szToolBarName, NORECT, AFX_IDW_DOCKBAR_TOP );	
	CreateToolBar( &ti );
	RecalcLayout();
	g_CmdManager.UpdateShellFrameToolbarInfo( this );

}
*/


/////////////////////////////////////////////////////////////////////////////
void CShellFrame::DestroyToolBarList()
{
	POSITION pos = m_ptrToolBarList.GetHeadPosition();
	while( pos )
	{
		delete (CShellFrameToolBar*)m_ptrToolBarList.GetNext( pos );
	}

	m_ptrToolBarList.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
void CShellFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	static CBCGToolbarButton button;
	
	button.m_nID = nID;
	
	CMainFrame	*pfrm = (CMainFrame	*)AfxGetMainWnd();	
	pfrm->GetToolbarButtonToolTipText( &button, rMessage );
}

LRESULT CShellFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CMDIChildWnd::WindowProc(message, wParam, lParam);
}

LRESULT CShellFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	UINT nIDLast = m_nIDLastMessage;
	m_nFlags &= ~WF_NOPOPMSG;

	CWnd* pMessageBar = GetMessageBar();
	if (pMessageBar != NULL)
	{
		LPCTSTR lpsz = NULL;
		CString strMessage;

		// set the message bar text
		if (lParam != 0)
		{
			ASSERT(wParam == 0);    // can't have both an ID and a string
			lpsz = (LPCTSTR)lParam; // set an explicit string
		}
		else if (wParam != 0)
		{
			// map SC_CLOSE to PREVIEW_CLOSE when in print preview mode
			if (wParam == AFX_IDS_SCCLOSE && m_lpfnCloseProc != NULL)
				wParam = AFX_IDS_PREVIEW_CLOSE;

			CFrameWnd* pFrameWnd = pMessageBar->GetParentFrame();
			// get message associated with the ID indicated by wParam
			if( pFrameWnd )
				pFrameWnd->GetMessageString((UINT)wParam, strMessage);
			else
				GetMessageString((UINT)wParam, strMessage);

			lpsz = strMessage;
		}
		pMessageBar->SetWindowText(lpsz);

		// update owner of the bar in terms of last message selected
//		CFrameWnd* pFrameWnd = pMessageBar->GetParentFrame();
//		if (pFrameWnd != NULL)
//		{
//			pFrameWnd->m_nIDLastMessage = (UINT)wParam;
//			pFrameWnd->m_nIDTracking = (UINT)wParam;
//		}
	}

	m_nIDLastMessage = (UINT)wParam;    // new ID (or 0)
	m_nIDTracking = (UINT)wParam;       // so F1 on toolbar buttons work
	return nIDLast;
}


bool CShellFrame::TryActivateView()
{
	CShellView* pview = (CShellView*)GetActiveView();
	if( !pview )
		return false;

	pview->OnActivateView( TRUE, pview, 0 );	

	return true;
}

