// ActionLogWindow.cpp : implementation file
//

#include "stdafx.h"
#include "ActionMan.h"
#include "ActionLogWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionLogWindow

IMPLEMENT_DYNCREATE(CActionLogWindow, CListCtrl)

CActionLogWindow::CActionLogWindow()
{
	EnableAutomation();
	
	m_bInitialized  = false;

	m_sName = "ActionLogWindow";
	
	_OleLockApp( this );
}

CActionLogWindow::~CActionLogWindow()
{
	_OleUnlockApp( this );
}

void CActionLogWindow::OnFinalRelease()
{
	AddRef();
	if( m_bInitialized )
	{
		IUnknown *punkCtrl = GetOtherComponent( IID_INotifyController );
		if(punkCtrl)
		{
			UnRegister( punkCtrl );
			punkCtrl->Release();
		}
	}

	m_dwRef = 0;


	delete this;
}


BEGIN_MESSAGE_MAP(CActionLogWindow, CListCtrl)
	//{{AFX_MSG_MAP(CActionLogWindow)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CActionLogWindow, CListCtrl)
	INTERFACE_PART(CActionLogWindow, IID_IWindow, Wnd)
	INTERFACE_PART(CActionLogWindow, IID_IDockableWindow, Dock)
	INTERFACE_PART(CActionLogWindow, IID_IRootedObject, Rooted)
	INTERFACE_PART(CActionLogWindow, IID_IEventListener, EvList)
	INTERFACE_PART(CActionLogWindow, IID_INamedObject2, Name)
	INTERFACE_PART(CActionLogWindow, IID_IHelpInfo, Help)
END_INTERFACE_MAP()

// {51CE67B2-2220-11d3-A5D6-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionLogWindow, "ActionMan.ActionLogWindow", 
0x51ce67b2, 0x2220, 0x11d3, 0xa5, 0xd6, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);


/////////////////////////////////////////////////////////////////////////////
// CActionLogWindow message handlers
CWnd *CActionLogWindow::GetWindow()
{
	return this;
}

bool CActionLogWindow::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	if( !Create( WS_CHILD|WS_VISIBLE|LVS_REPORT, rc, pparent, nID ) )
		return false;

	SetWindowLong( *this, GWL_EXSTYLE, GetWindowLong( *this, GWL_EXSTYLE )|WS_EX_CLIENTEDGE );

	IUnknown *punkCtrl = GetOtherComponent( IID_INotifyController );
	ASSERT(punkCtrl);
							  
	Register( punkCtrl );

	punkCtrl->Release();

	m_bInitialized = true;

	return true;

}

void CActionLogWindow::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !GetSafeHwnd() )
		return;
	if( !strcmp( pszEvent, szEventAfterActionExecute ) )
	{
		long	lTime = 0;
		if( pdata )lTime = *(long*)pdata;

		CString	strTime, strParam, strAction;
		strTime.Format( "%d ms", lTime );

		BSTR	bstrParams = 0, bstrActionName = 0;
		IActionPtr	ptrA( punkHit );
		IUnknown	*punkAI = 0;

		ptrA->GetActionInfo( &punkAI );

		ASSERT(punkAI);
		IActionInfoPtr	ptrAI( punkAI );
		punkAI->Release();

		ptrA->StoreParameters( &bstrParams );
		ptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0 );

		if( bstrParams )strParam = bstrParams;
		if( bstrActionName )strAction = bstrActionName;

		::SysFreeString( bstrParams );
		::SysFreeString( bstrActionName );

		int idx = 
		InsertItem( GetItemCount(), strAction );
		SetItemText( idx, 1, strTime );
		SetItemText( idx, 2, strParam );
		
		
		EnsureVisible( idx, false );
	}
}


int CActionLogWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertColumn( 0, "Actions" );
	InsertColumn( 1, "Time" );
	InsertColumn( 2, "Params" );

	SetColumnWidth( 0, 200 );
	SetColumnWidth( 1, 100 );
	SetColumnWidth( 2, 200 );

	return 0;
}

void CActionLogWindow::PostNcDestroy() 
{
	//delete this;
}
