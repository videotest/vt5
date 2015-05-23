#include "stdafx.h"
#include "com_main.h"
#include "win_main.h"
#include "nameconsts.h"


//window funcs

LRESULT __stdcall WorkWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	CMessageFilterImpl	*pFilter = (CMessageFilterImpl*)hWnd;
	LRESULT lres = pFilter->DoMessage(nMessage, wParam, lParam);
	return lres;
}

LRESULT __stdcall InitWindowProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	if( nMessage == WM_NCCREATE )
	{
		CREATESTRUCT	*pcs = (CREATESTRUCT*)lParam;
		CWinImpl		*pwnd = (CWinImpl*)pcs->lpCreateParams;
		pwnd->Subclass( hWnd );
		return pwnd->DoMessage( nMessage, wParam, lParam );
	}
	return 0;
}


INT_PTR __stdcall InitDlgProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	if( nMessage == WM_INITDIALOG )
	{
		CWinImpl		*pwnd = (CWinImpl*)lParam;
		pwnd->Subclass( hWnd );
		return pwnd->DoMessage( nMessage, wParam, lParam );
	}
	return 0;
}


//implementation base functionality of window
CWinImpl::CWinImpl()
{
	m_hwnd = 0;
	m_dwWindowFlags = 0;
	m_pDefHandler = 0;
	strcpy( m_szClassName, szDefClassName );
}

CWinImpl::~CWinImpl()
{
	if( ::IsWindow( m_hwnd ) && m_pDefHandler )
		::SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_pDefHandler );
}

IUnknown	*CWinImpl::DoGetWinInterface( const IID &iid )
{
	if( iid == IID_IWindow2 )return (IWindow2*)this;
	else if( iid == IID_IWindow )return (IWindow*)this;
	else return 0;
}

void CWinImpl::Subclass( HWND hwnd )
{
	m_hwnd = hwnd;

	char	szClassName[64];
	::GetClassName( m_hwnd, szClassName, 64 );
	
	m_thunk.Init( (DWORD_PTR)WorkWindowProc, (IMessageFilter*)this );
	m_pDefHandler = (WNDPROC)::GetWindowLongPtr( m_hwnd , GWLP_WNDPROC );

	CWndClass	*pclass = App::instance()->FindWindowClass( szClassName );
	
	if( pclass && pclass->lpfnWndProc == m_pDefHandler )
		m_pDefHandler = DefWindowProc;

	::SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_thunk.GetCodeAddress() );
}

void CWinImpl::Detach()
{
	if( m_hwnd )
		::SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_pDefHandler );
	m_pDefHandler = 0;
	m_hwnd = 0;
}

LRESULT	CWinImpl::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes = CMessageFilterImpl::DoMessage( nMsg, wParam, lParam );
	if( lRes != 0 )return lRes;

	switch( nMsg )
	{
	case WM_CREATE:			return DoCreate( (CREATESTRUCT*)lParam );
	case WM_PAINT:			DoPaint();return 1;
	case WM_COMMAND:		DoCommand( wParam );return 1;
	case WM_NOTIFY:			DoNotify( wParam, (NMHDR*)lParam );
	case WM_GETINTERFACE:	return (LRESULT)this;
	}

/*	if( nMsg == WM_CREATE ){return DoCreate( );}else 
	if( nMsg == WM_PAINT ){	DoPaint(); return true;}else
	if( nMsg == WM_ERASEBACKRGND ){	return DoEraseBackgrnd(); }else
	if( nMsg == WM_ERASEBACKRGND ){	DoCommand(); }else*/



	if( m_pDefHandler )lRes = CallWindowProc( m_pDefHandler, m_hwnd, nMsg, wParam, lParam );
	if( lRes != 0 )return lRes;
	if( nMsg == WM_NCCREATE )return  1;
	if( nMsg == WM_NCDESTROY )
	{
		m_hwnd = 0;
		m_pDefHandler = 0;
	}

	return 0;
}

HRESULT	CWinImpl::CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID )
{
	m_hwnd = ::CreateWindow( m_szClassName, 
						0/*m_szTitle*/,
						dwStyles,
						rect.left,
						rect.top,
						rect.right-rect.left,
						rect.bottom-rect.top,
						(HWND)hwndParent,
						(HMENU)nID,
						App::instance()->m_hInstance,
						this );

	if( m_hwnd == 0 )
	{
		App::instance()->handle_error();
		return E_FAIL;
	}
	return S_OK;
}
HRESULT	CWinImpl::DestroyWindow()
{
	::DestroyWindow( m_hwnd );
	return S_OK;
}
HRESULT	CWinImpl::GetHandle( HANDLE *phwnd )
{
	*phwnd = m_hwnd;
	return S_OK;
}

HRESULT	CWinImpl::HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand )
{
	return S_OK;
}

HRESULT	CWinImpl::PreTranslateMsg( MSG *pmsg, BOOL *pbReturn )
{
	TPOS pos = m_ptrsMsgListeners.head();
	while( pos )
	{
		IMsgListenerPtr ptrML( m_ptrsMsgListeners.get(pos) );
		pos = m_ptrsMsgListeners.next(pos);
		if(ptrML==0) continue;
		LONG_PTR lRes = 0;
		if( ptrML->OnMessage( pmsg, &lRes ) == S_FALSE )
		{
			*pbReturn = true;
			break;
		}
	}
	return S_OK;
}

HRESULT	CWinImpl::GetWindowFlags( DWORD *pdwFlags )
{
	*pdwFlags = m_dwWindowFlags;
	return S_OK;
}
HRESULT	CWinImpl::SetWindowFlags( DWORD dwFlags )
{
	m_dwWindowFlags = dwFlags;
	return S_OK;
}
HRESULT	CWinImpl::PreTranslateAccelerator( MSG *pmsg )
{
	//paul not S_OK default S_FALSE;
	return S_FALSE;
}
//IWindow2
HRESULT	CWinImpl::AttachMsgListener( IUnknown *punk )
{
	IUnknownPtr ptr(punk);
	dbg_assert( !m_ptrsMsgListeners.find( ptr ) );
	m_ptrsMsgListeners.add_tail( punk );
    return S_OK;
}
HRESULT	CWinImpl::DetachMsgListener( IUnknown *punk )
{
	IUnknownPtr ptr(punk);
	TPOS pos = m_ptrsMsgListeners.find(ptr);
	dbg_assert( pos!=0 );
	if(pos) m_ptrsMsgListeners.remove(pos);
    return S_OK;
}
HRESULT	CWinImpl::AttachDrawing( IUnknown *punk )
{
	return E_NOTIMPL;
}
HRESULT	CWinImpl::DetachDrawing( IUnknown *punk )
{
	return E_NOTIMPL;
}



///////////////////////////////////////////////////////////////////////////////
//window class implementation
CWndClass::CWndClass( const char *pszName, WNDPROC pproc )
{
	ZeroMemory( (WNDCLASSEX*)this, sizeof( WNDCLASSEX ) );

	strcpy( m_szClassName, pszName );

	cbSize = sizeof( WNDCLASSEX );
    style = CS_OWNDC|CS_OWNDC|CS_DBLCLKS;
    lpfnWndProc = pproc?pproc:InitWindowProc;
    cbClsExtra = 0;
    cbWndExtra = 0;
    hInstance = 0;
    hIcon = 0;
	hCursor = ::LoadCursor( hInstance, MAKEINTRESOURCE(IDC_ARROW) );
    hbrBackground = 0;
    lpszMenuName = 0;
    lpszClassName = m_szClassName;
    hIconSm = 0;

	m_bRegistred = false;
	m_pnext = 0;
}

bool CWndClass::Register()
{
	if( m_bRegistred )return true;

	hInstance = App::handle();

	m_bRegistred = ::RegisterClassEx( this ) != 0;
	if( !m_bRegistred )
		App::instance()->handle_error();

	return m_bRegistred;
}

