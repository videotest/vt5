// mouse.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mouse.h"
#include "Utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


class CMouseController : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CMouseController);
	GUARD_DECLARE_OLECREATE(CMouseController);
public:
	struct ViewInfo
	{
		bool		bCtrlShown;	
		CPoint		point;
		IUnknown	*punkView;
		POSITION	pos;
		HWND		hwnd;

		CPtrArray	hilighted_objects;
	};

	DECLARE_INTERFACE_MAP();
public:
	CMouseController();
	~CMouseController();

	BEGIN_INTERFACE_PART(Mouse, IMouseController)
		com_call RegisterMouseCtrlTarget( IUnknown *punkView );
		com_call UnRegisterMouseCtrlTarget( IUnknown *punkView );
		com_call DispatchMouseToCtrl( IUnknown *punkView, POINT point );
		com_call RepaintMouseCtrl( IUnknown *punkView, HDC hDC, BOOL bDraw );
		com_call CheckMouseInWindow( IUnknown *punkView );
		com_call InstallController( IUnknown *punkCtrl );
		com_call UnInstallController( IUnknown *punkCtrl );
	END_INTERFACE_PART(Mouse)
protected:
	POSITION	GetFirstViewPosition();
	ViewInfo	*GetNextView( POSITION &pos );
	ViewInfo	*GetView( IUnknown *punk );
	bool	IsAvaible();

	void _RegisterMouseCtrlTarget( IUnknown *punkView );
	void _UnRegisterMouseCtrlTarget( IUnknown *punkView );
	void _DispatchMouseToCtrl( IUnknown *punkView, POINT point );
	void _RepaintMouseCtrl( IUnknown *punkView, HDC hDC, long lDraw );
	void _CheckMouseInWindow( IUnknown *punkView );
	void _InstallController( IUnknown *punkCtrl );
	void _UnInstallController( IUnknown *punkCtrl );
	void _ReDrawAll( bool bDraw );
	void _ReDrawView( ViewInfo *pinfo, long lDraw, HDC hDC =0 );
protected:
	CPtrList					m_listViews;
	sptrIMouseDrawController	m_sptrCtrl;
};

////////////////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CMouseController, CCmdTargetEx)
	INTERFACE_PART(CMouseController, IID_IMouseController, Mouse)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CMouseController, Mouse);
IMPLEMENT_DYNCREATE(CMouseController, CCmdTargetEx);

// {C508FBE2-A88F-11d3-A6B8-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMouseController, "Mouse.MouseController", 
0xc508fbe2, 0xa88f, 0x11d3, 0xa6, 0xb8, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

CMouseController::CMouseController()
{
	_OleLockApp( this );
	EnableAggregation();
}

CMouseController::~CMouseController()
{
	POSITION	pos = GetFirstViewPosition();

	while( pos )
		delete GetNextView( pos );

	_OleUnlockApp( this );
}

POSITION CMouseController::GetFirstViewPosition()
{
	return m_listViews.GetHeadPosition();
}

CMouseController::ViewInfo *CMouseController::GetNextView( POSITION &pos )
{
	return (ViewInfo *)m_listViews.GetNext( pos );
}

CMouseController::ViewInfo	*CMouseController::GetView( IUnknown *punk )
{
	POSITION	pos = GetFirstViewPosition();

	while( pos )
	{
		ViewInfo	*pi = GetNextView( pos );

		if( pi->punkView == punk  )
			return pi;
	}
	return 0;
}

bool CMouseController::IsAvaible()
{
	return m_sptrCtrl != 0;
}


////////////////////////////////////////////////////////////////////////////////////////
HRESULT CMouseController::XMouse::RegisterMouseCtrlTarget( IUnknown *punkView )
{
	_try_nested(CMouseController, Mouse, RegisterMouseCtrlTarget)
	{
		pThis->_RegisterMouseCtrlTarget( punkView );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::UnRegisterMouseCtrlTarget( IUnknown *punkView )
{
	_try_nested(CMouseController, Mouse, UnRegisterMouseCtrlTarget)
	{
		pThis->_UnRegisterMouseCtrlTarget( punkView );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::DispatchMouseToCtrl( IUnknown *punkView, POINT point )
{
	_try_nested(CMouseController, Mouse, DispatchMouseToCtrl)
	{
		pThis->_DispatchMouseToCtrl( punkView, point );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::RepaintMouseCtrl( IUnknown *punkView, HDC hDC, BOOL lDraw )
{
	_try_nested(CMouseController, Mouse, RepaintMouseCtrl)
	{
		pThis->_RepaintMouseCtrl( punkView, hDC, lDraw );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::CheckMouseInWindow( IUnknown *punkView )
{
	_try_nested(CMouseController, Mouse, CheckMouseInWindow)
	{
		pThis->_CheckMouseInWindow( punkView );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::InstallController( IUnknown *punkCtrl )
{
	_try_nested(CMouseController, Mouse, InstallController )
	{
		pThis->_InstallController( punkCtrl );
		return S_OK;
	}
	_catch_nested;
}
HRESULT CMouseController::XMouse::UnInstallController( IUnknown *punkCtrl )
{
	_try_nested(CMouseController, Mouse, UnInstallController )
	{
		pThis->_UnInstallController( punkCtrl );
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////////////////

void CMouseController::_ReDrawView( ViewInfo *pinfo, long lDraw, HDC hDC )
{
	CWnd	*pwnd = GetWindowFromUnknown( pinfo->punkView );
	if( !pwnd->GetSafeHwnd() )
		return;
	bool	bOwnDC = true;

	if( !hDC )
	{
		ASSERT(pwnd->GetSafeHwnd());

		hDC = ::GetDC( pwnd->GetSafeHwnd() );
		bOwnDC = false;
	}

	if( m_sptrCtrl != 0 )
		m_sptrCtrl->DoDraw( hDC, pinfo->punkView, pinfo->point, lDraw );

	if( !bOwnDC )
	{
		::ReleaseDC( pwnd->GetSafeHwnd(), hDC );
	}

}

void CMouseController::_ReDrawAll( bool bDraw )
{
	POSITION	pos = GetFirstViewPosition();

	while( pos )
	{
		ViewInfo	*pinfo = GetNextView( pos );
		if( pinfo->bCtrlShown )
			_ReDrawView( pinfo, bDraw );
	}
}

void CMouseController::_RegisterMouseCtrlTarget( IUnknown *punkView )
{
	ViewInfo *pinfo = GetView( punkView );

		//check allredy registered
		if( pinfo )
		return;	

	pinfo = new ViewInfo;
	pinfo->bCtrlShown = false;
	pinfo->point = CPoint( -1, -1 );
	pinfo->punkView = punkView;
	pinfo->pos = m_listViews.AddTail( pinfo );

	IWindowPtr	ptrWindow( punkView );
	ptrWindow->GetHandle( (void**)&pinfo->hwnd );


	_CheckMouseInWindow( punkView );
}


void CMouseController::_UnRegisterMouseCtrlTarget( IUnknown *punkView )
{
	ViewInfo *pinfo = GetView( punkView );
	ASSERT( pinfo );

	_ReDrawView( pinfo, false );

	m_listViews.RemoveAt( pinfo->pos );
	delete pinfo;
}

void CMouseController::_DispatchMouseToCtrl( IUnknown *punkView, POINT point )
{
	ViewInfo *pinfo = GetView( punkView );
	ASSERT( pinfo );

	if( !pinfo->bCtrlShown )
	{
		pinfo->bCtrlShown = 1;
	}
	else
	{
		_ReDrawView( pinfo, false );
	}
	pinfo->point = point;

	_ReDrawView( pinfo, true );
}

void CMouseController::_RepaintMouseCtrl( IUnknown *punkView, HDC hDC, long lDraw )
{
	if( punkView )
	{
		ViewInfo *pinfo = GetView( punkView );
		ASSERT( pinfo );

		if( pinfo->bCtrlShown )
			_ReDrawView( pinfo, lDraw, hDC );
	}
	else
	{
		_ReDrawAll( lDraw != 0 );
	}
}

void CMouseController::_CheckMouseInWindow( IUnknown *punkView )
{
	ViewInfo *pinfo = GetView( punkView );

	if( !pinfo || !pinfo->bCtrlShown )
		return;
	CWnd	*pwnd = ::GetWindowFromUnknown( punkView );
	CPoint	pointCursor;
	::GetCursorPos( &pointCursor );

	if( pwnd->GetSafeHwnd() != ::WindowFromPoint( pointCursor ) )
	{
		_ReDrawView( pinfo, false );
		pinfo->bCtrlShown = false;
	}
}

void CMouseController::_InstallController( IUnknown *punkCtrl )
{
	_ReDrawAll( false );
	m_sptrCtrl = punkCtrl;
	_ReDrawAll( true );
}
void CMouseController::_UnInstallController( IUnknown *punkCtrl )
{
	_ReDrawAll( false );
	m_sptrCtrl = 0;
	_ReDrawAll( true );
}
/////////////////////////////////////////////////////////////////////////////////////////







BEGIN_MESSAGE_MAP(CMouseApp, CWinApp)
	//{{AFX_MSG_MAP(CMouseApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMouseApp construction

CMouseApp::CMouseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMouseApp object

CMouseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMouseApp initialization

BOOL CMouseApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	VERIFY(LoadVTClass(this) == true);
	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CCompRegistrator	r( szPluginApplicationAggr );
	r.RegisterComponent( "Mouse.MouseController" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	r( szPluginApplicationAggr );
	r.UnRegisterComponent( "Mouse.MouseController" );

	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}
