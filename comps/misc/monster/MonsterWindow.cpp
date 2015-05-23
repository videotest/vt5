// MonsterWindow.cpp : implementation file
//

#include "stdafx.h"
#include "monster.h"
#include "MonsterWindow.h"


void GiveFocusToApp()
{
	HWND	hwndMainWindow;
	sptrIApplication	sptrA( GetAppUnknown() );

	sptrA->GetMainWindowHandle( &hwndMainWindow );
	::SetFocus( hwndMainWindow );
}

/////////////////////////////////////////////////////////////////////////////
// CMonsterWindow

IMPLEMENT_DYNCREATE(CMonsterWindow, CWnd)

CMonsterWindow	*CMonsterWindow::s_pMonsterWindow = 0;

CMonsterWindow::CMonsterWindow()
{
	EnableAutomation();

	s_pMonsterWindow = this;
	m_pMessageWindow = 0;

	Register();

	m_bCanDestroy = false;
	
	_OleLockApp( this );
}

CMonsterWindow::~CMonsterWindow()
{
	s_pMonsterWindow = 0;

	UnRegister();
	
	_OleUnlockApp( this );
}

void CMonsterWindow::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	m_bCanDestroy = true;


	POSITION pos = m_listShedule.GetHeadPosition();

	while( pos )
		delete m_listShedule.GetNext( pos );
	m_listShedule.RemoveAll();

	DestroyWindow();

	delete this;
}


BEGIN_MESSAGE_MAP(CMonsterWindow, CTransparentWnd)
	//{{AFX_MSG_MAP(CMonsterWindow)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CMonsterWindow, CTransparentWnd)
	//{{AFX_DISPATCH_MAP(CMonsterWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMonsterWindow to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {223D1DC1-70AD-11D3-A65B-0090275995FE}
static const IID IID_IMonsterWindow =
{ 0x223d1dc1, 0x70ad, 0x11d3, { 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

BEGIN_INTERFACE_MAP(CMonsterWindow, CTransparentWnd)
	INTERFACE_PART(CMonsterWindow, IID_IMonsterWindow, Dispatch)
	INTERFACE_PART(CMonsterWindow, IID_IRootedObject, Rooted)
	INTERFACE_PART(CMonsterWindow, IID_IEventListener, EvList)
END_INTERFACE_MAP()

// {665A1FC1-70AE-11d3-A65B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMonsterWindow, "monster.MonsterWindow", 
0x665a1fc1, 0x70ae, 0x11d3, 0xa6, 0x5b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

/////////////////////////////////////////////////////////////////////////////
// CMonsterWindow message handlers

int CMonsterWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTransparentWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_nTimerID = SetTimer( m_nTimerMsgID=777, 100, 0 );

	m_dwLastOperationTickCount = GetTickCount();
	m_dwMonsterActionInterval = ::GetValueInt( GetAppUnknown(), _T("Monster"), _T("ActionInterval"), 5 )*1000;
	m_bAutomaticMode = ::GetValueInt( GetAppUnknown(), _T("Monster"), _T("Automatic"), 1 )!=0;

	CRect	rect;
	GetWindowRect( &rect );

	int	xPos = ::GetValueInt( GetAppUnknown(), _T("Monster"), _T("XPos"), rect.left );
	int	yPos = ::GetValueInt( GetAppUnknown(), _T("Monster"), _T("YPos"), rect.top );
	
	MoveWindow( xPos, yPos, rect.Width(), rect.Height() );

	return 0;
}

static CPoint g_pt_mouse = CPoint( 0, 0 );

void CMonsterWindow::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == m_nTimerMsgID )
	{
		bool	bRandom = rand() <RAND_MAX/10000;

		if( bRandom )
		{
			SheduleOperation( 1000, stHid );
			SheduleOperation( 5000, stNormal );
		}

		DWORD	dwCurrentTickCount = ::GetTickCount();

		if( m_listShedule.GetHeadPosition() )
		{
			Shedule	*ps = m_listShedule.GetHead();

			if( ps->m_dwTickCount < dwCurrentTickCount )
				PreformOperation();
		}


		if( m_bAutomaticMode )
		{
			if( dwCurrentTickCount-m_dwLastOperationTickCount > m_dwMonsterActionInterval )
			{
				bool	bHorz = ::rand()*2/RAND_MAX != 0;

				
				CRect	rcDesktop;
				CRect	rcWindow;

				CWnd	*pwndDesktop = CWnd::GetDesktopWindow();

				
				GetWindowRect( &rcWindow );
				CSize	size = rcWindow.Size();

				CPoint	ptStart = rcWindow.TopLeft();

				pwndDesktop->GetWindowRect( rcDesktop );
				rcDesktop.InflateRect( -size.cx, -size.cy );

				CPoint	ptNewOffset;

				if( bHorz )
				{
					ptNewOffset.x = ::rand()*rcDesktop.Width()/RAND_MAX+rcDesktop.left-size.cx/2;
					ptNewOffset.y = ::rand()*size.cy/RAND_MAX+rcDesktop.top-size.cy/2;
				}
				else
				{
					ptNewOffset.x = ::rand()*size.cx/RAND_MAX+rcDesktop.left-size.cx/2;
					ptNewOffset.y = ::rand()*rcDesktop.Height()/RAND_MAX+rcDesktop.top-size.cy/2;
				}

				m_dwLastOperationTickCount = dwCurrentTickCount;

				MoveWindow( ptNewOffset.x, ptNewOffset.y, size.cx, size.cy );
			}
		}

		//paul 30.05.2002
		if( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) == 0 )
		{
			CPoint pt_cursor( 0, 0 );
			GetCursorPos( &pt_cursor );

			if( g_pt_mouse == CPoint( 0, 0 ) )
				g_pt_mouse = pt_cursor;

			CRect rc_wnd = NORECT;
			GetWindowRect( &rc_wnd );

			int ndx, ndy;

			if( rc_wnd.PtInRect( pt_cursor ) )
			{
				ndx = pt_cursor.x - g_pt_mouse.x;
				ndy = pt_cursor.y - g_pt_mouse.y;

				rc_wnd += CPoint( ndx, ndy );
				MoveWindow( &rc_wnd );
				
			}

			g_pt_mouse = pt_cursor;
		}
	}
	
	CTransparentWnd::OnTimer(nIDEvent);
}

void CMonsterWindow::OnDestroy() 
{
	CTransparentWnd::OnDestroy();
	
	KillTimer( m_nTimerID );	
}

void CMonsterWindow::OnChangeParent()
{
	CreateTransparent( _T("VideoTest Daemon"), NORECT, IDB_MONSTER_FUN );

	if( ::GetValueInt( GetAppUnknown(), _T("Monster"), _T("Show"), 1 ) )
	{
		SheduleOperation( 1000, stNormal );
		SheduleOperation( 1500, stFun );
		SheduleOperation( 2000, stNormal );
	}
}

void CMonsterWindow::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szBeforeAppTerminate ) )
	{
//store position

		if( IsWindow( GetSafeHwnd() ) )
		{
			CRect	rect;
			GetWindowRect( &rect );

			::SetValue( GetAppUnknown(), _T("Monster"), _T("XPos"), rect.left );
			::SetValue( GetAppUnknown(), _T("Monster"), _T("YPos"), rect.top );
		}
	}
}


void CMonsterWindow::OnMove(int x, int y) 
{
	CTransparentWnd::OnMove(x, y);
	
	if( m_pMessageWindow )
		m_pMessageWindow->PlaceWindow();
}

void CMonsterWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_pMessageWindow )
		m_pMessageWindow->DestroyWindow();
	else
		ExecuteAction( _T("MonsterDisplayMessage") );
}

void CMonsterWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	GetWindowRect( &rect );

	CPoint	ptFrom = rect.TopLeft();

	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));

	GetWindowRect( &rect );
	CPoint	ptTo = rect.TopLeft();

	if( ptFrom != ptTo )
	{
		CString	str;
		str.Format( "%d, %d", ptTo.x, ptTo.y );
		ExecuteAction( "MonsterMove", str );
	}

	GiveFocusToApp();
	
	CTransparentWnd::OnLButtonDown(nFlags, point);
}

void CMonsterWindow::PostNcDestroy() 
{}

void CMonsterWindow::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus( pOldWnd );
}


//monster shedule
void CMonsterWindow::SheduleOperation( DWORD dwMSecs, ShowType ShowType )
{
	Shedule	*p = new Shedule;
	p->m_dwTickCount = GetTickCount()+dwMSecs;
	p->m_ShowType = ShowType;
	m_listShedule.AddTail( p );
}

void CMonsterWindow::PreformOperation()
{
	Shedule *p = m_listShedule.GetHead();
	if( p->m_ShowType == stInvisible )
	{
		ShowWindow( SW_HIDE );
		SetupRegion( GetWindowDC(), stNormal );
	}
	else
	{
		ShowWindow( SW_SHOW );
		SetupRegion( GetWindowDC(), p->m_ShowType );
	}

	m_listShedule.RemoveAt( m_listShedule.GetHeadPosition() );
	delete p;
}
BOOL CMonsterWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEALL ) );
	return true;
}

void CMonsterWindow::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	if( m_bCanDestroy )
		CTransparentWnd::OnClose();
}
