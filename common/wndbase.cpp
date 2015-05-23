#include "stdafx.h"
#include "wndbase.h"
#include "cmdtargex.h"
#include "object5.h"
#include "core5.h"
#include "AfxPriv.h"
#include "nameconsts.h"
#include "GuardInt.h"
#include "math.h"

#include "wndmisc5.h"


CWndClassHelper CWndClassHelper::helper;


CWindowImpl::CWindowImpl()
{
	m_dwFlags = wfZoomSupport;
}

CWindowImpl::~CWindowImpl()
{
}

IMPLEMENT_UNKNOWN_BASE(CWindowImpl, Wnd);

bool CWindowImpl::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	AFX_MANAGE_STATE(AfxGetModuleState());

	m_strClassName = ::AfxRegisterWndClass( CS_DBLCLKS|CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1) );
	BOOL bReturn = GetWindow()->Create( m_strClassName,
		m_strWindowTitle, 
		dwStyle, rc, 
		pparent, nID 
		/*pContext*/  );
		
	if( bReturn )
		return TRUE;

	char	sz[200];
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sz, 200, NULL );
	TRACE( "Window creating error %s\n", sz );

	return FALSE;
}


HRESULT CWindowImpl::XWnd::PreTranslateAccelerator( MSG *pmsg )
{
	_try_nested_base(CWindowImpl, Wnd, PreTranslateAccelerator)
	{
		bool bRet = false;
		CWnd * pWnd = pThis->GetWindow();
		HWND hOwnWnd = pWnd ? pWnd->m_hWnd : 0;

		if (hOwnWnd)
		{
			HWND hNext = ::GetFocus();
			while (hNext && hOwnWnd != hNext)
				hNext = ::GetParent(hNext);

			if (hNext)
				bRet = pThis->DoTranslateAccelerator(pmsg);
		}

		return bRet ? S_OK : S_FALSE;
	}
	_catch_nested;
}

HRESULT CWindowImpl::XWnd::CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyle, UINT nID )
{
	_try_nested_base(CWindowImpl, Wnd, CreateWnd)
	{
		CWnd	*pwndParent = CWnd::FromHandle( (HWND)hwndParent );

		pThis->DoCreate( dwStyle, rect, pwndParent, nID );
		
		return S_OK;
	}
	_catch_nested;
}
HRESULT CWindowImpl::XWnd::DestroyWindow()
{
	_try_nested_base(CWindowImpl, Wnd, DestroyWindow)
	{
		pThis->GetWindow()->DestroyWindow();
		return S_OK;
	}
	_catch_nested;
}
HRESULT CWindowImpl::XWnd::GetHandle( HANDLE *phWnd )
{
	METHOD_PROLOGUE_BASE(CWindowImpl, Wnd)
	*phWnd = pThis->GetWindow()->GetSafeHwnd();
	return S_OK;
}
HRESULT CWindowImpl::XWnd::HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand )
{
	_try_nested_base(CWindowImpl, Wnd, HelpHitTest)
	{
		return S_OK;
	}
	_catch_nested;
}

HRESULT CWindowImpl::XWnd::PreTranslateMsg( MSG *pmsg, BOOL *pbReturn )
{
	METHOD_PROLOGUE_BASE(CWindowImpl, Wnd)
	*pbReturn = pThis->GetWindow()->PreTranslateMessage( pmsg );
	return S_OK;
}

HRESULT CWindowImpl::XWnd::GetWindowFlags( DWORD *pdwFlags )
{
	_try_nested_base(CWindowImpl, Wnd, GetWindowFlags)
	{
		if (pdwFlags)
			*pdwFlags = pThis->m_dwFlags;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CWindowImpl::XWnd::SetWindowFlags( DWORD dwFlags )
{
	_try_nested_base(CWindowImpl, Wnd, SetWindowFlags)
	{
		pThis->m_dwFlags = dwFlags;
		return S_OK;
	}
	_catch_nested;
}


//CWindow2Impl
IMPLEMENT_UNKNOWN_BASE(CWindow2Impl, Wnd2);

CWindow2Impl::CWindow2Impl()
{
}

CWindow2Impl::~CWindow2Impl()
{
}

HRESULT CWindow2Impl::XWnd2::AttachMsgListener( IUnknown *punk )
{
	_try_nested_base(CWindow2Impl, Wnd2, AttachMsgListener)
	{
		pThis->_AttachListener( punk );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::DetachMsgListener( IUnknown *punk )
{
	_try_nested_base(CWindow2Impl, Wnd2, DetachMsgListener)
	{
		pThis->_DetachListener( punk );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID )
{
	_try_nested_base(CWindow2Impl, Wnd2, CreateWnd)
	{
		return pThis->m_xWnd.CreateWnd( hwndParent, rect, dwStyles, nID );
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::DestroyWindow()
{
	_try_nested_base(CWindow2Impl, Wnd2, DestroyWindow)
	{
		return pThis->m_xWnd.DestroyWindow();
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::GetHandle( HANDLE *phWnd )
{
	_try_nested_base(CWindow2Impl, Wnd2, GetHandle)
	{
		return pThis->m_xWnd.GetHandle( phWnd );
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand )
{
	_try_nested_base(CWindow2Impl, Wnd2, HelpHitTest)
	{
		return pThis->m_xWnd.HelpHitTest( pt, pbstrHelpFileName, pdwTopicID, pdwHelpCommand );
	}
	_catch_nested;
}


HRESULT CWindow2Impl::XWnd2::PreTranslateAccelerator( MSG *pmsg )
{
	_try_nested_base(CWindow2Impl, Wnd2, PreTranslateMsg)
	{
		return pThis->m_xWnd.PreTranslateAccelerator( pmsg );
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::PreTranslateMsg( MSG *pmsg, BOOL *pbReturn )
{
	_try_nested_base(CWindow2Impl, Wnd2, PreTranslateMsg)
	{
		return pThis->m_xWnd.PreTranslateMsg( pmsg, pbReturn );
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::AttachDrawing( IUnknown *punk )		//unknown should support IID_IDrawObject
{
	_try_nested_base(CWindow2Impl, Wnd2, AttachDrawing)
	{
		ASSERT( !pThis->m_ptrsDrawObjects.Find( punk ) );
		pThis->m_ptrsDrawObjects.AddTail( punk );

		return S_OK;
	}
	_catch_nested;
}
HRESULT CWindow2Impl::XWnd2::DetachDrawing( IUnknown *punk )
{
	_try_nested_base(CWindow2Impl, Wnd2, DetachDrawing)
	{
		POSITION	pos = pThis->m_ptrsDrawObjects.Find( punk );
		ASSERT( pos );
		pThis->m_ptrsDrawObjects.RemoveAt( pos );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::GetWindowFlags( DWORD *pdwFlags )
{
	_try_nested_base(CWindow2Impl, Wnd2, GetWindowFlags)
	{
		return pThis->m_xWnd.GetWindowFlags( pdwFlags );
	}
	_catch_nested;
}

HRESULT CWindow2Impl::XWnd2::SetWindowFlags( DWORD dwFlags )
{
	_try_nested_base(CWindow2Impl, Wnd2, SetWindowFlags)
	{
		return pThis->m_xWnd.SetWindowFlags( dwFlags );
	}
	_catch_nested;
}




void CWindow2Impl::_AttachListener( IUnknown *punkListener )
{
	ASSERT( !m_ptrs.Find( punkListener ) );
	m_ptrs.AddTail( punkListener );
}

void CWindow2Impl::_DetachListener( IUnknown *punkListener )
{
	POSITION	pos = m_ptrs.Find( punkListener );
	ASSERT( pos );
	m_ptrs.RemoveAt( pos );
}

bool CWindow2Impl::ProcessMessage( MSG *pmsg, LRESULT* pResult )
{
	POSITION	pos = m_ptrs.GetHeadPosition();

	while( pos )
	{
		IUnknown *punk = (IUnknown *)m_ptrs.GetNext( pos );

		if( !CheckInterface( punk, IID_IMsgListener ) )
			continue;
		sptrIMsgListener	sptrML( punk );

		if( sptrML->OnMessage( pmsg, pResult ) == S_FALSE )
			return true;
	}
	return false;	//continue dispatching
}

bool CWindow2Impl::ProcessDrawing( CDC &dc, CRect rcDraw, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	POSITION	pos = m_ptrsDrawObjects.GetHeadPosition();

	while( pos )
	{
		IUnknown *punk = (IUnknown *)m_ptrs.GetNext( pos );

		if( !CheckInterface( punk, IID_IDrawObject ) )
			continue;

		CRect rectObject;
		sptrIDrawObject	sptrDraw( punk ); 
		sptrDraw->SetTargetWindow( Unknown() );
		sptrDraw->GetRect( &rectObject );
		sptrDraw->SetTargetWindow( 0 );

		if( rcDraw.left > rectObject.right ||
			rcDraw.right < rectObject.left ||
			rcDraw.top > rectObject.bottom ||
			rcDraw.bottom < rectObject.top )
			continue;

		sptrDraw->Paint( dc, rcDraw, Unknown(), pbiCache, pdibCache );
	}
	return true;	//ok
}


/////////////////////////////////////////////////////////////////////////////////////////
//CMsgListenerImpl - implementation of IMsgListener
IMPLEMENT_UNKNOWN_BASE(CMsgListenerImpl, MsgList);

CMsgListenerImpl::CMsgListenerImpl()
{
}

CMsgListenerImpl::~CMsgListenerImpl()
{
}

BOOL CMsgListenerImpl::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	CCmdTarget * pcmd  = GetCmdTarget();
	if (pmsg && pcmd && pcmd->IsKindOf(RUNTIME_CLASS(CCmdTargetEx)))
	{
		CCmdTargetEx * pcmde = (CCmdTargetEx *)pcmd;
		if( pcmde && pcmde->OnWndMsg( pmsg->message, pmsg->wParam, pmsg->lParam, plResult ) )
			return true;
	}
	return 0;
}

HRESULT CMsgListenerImpl::XMsgList::OnMessage( MSG *pmsg, LRESULT *plReturn )
{
	METHOD_PROLOGUE_BASE(CMsgListenerImpl, MsgList)
	BOOL bReturn = pThis->OnListenMessage( pmsg, plReturn );
	return (HRESULT)bReturn;
}

/////////////////////////////////////////////////////////////////////////////////////////
//IDockableWindow
IMPLEMENT_UNKNOWN_BASE(CDockWindowImpl, Dock);

CDockWindowImpl::CDockWindowImpl()
{
	m_size = CSize( 100, 100 );
	m_bFixed = false;
	m_dwDockSide = AFX_IDW_DOCKBAR_BOTTOM;
}

bool CDockWindowImpl::OnChangeSize( CSize &sizeReq, bool bVert )
{
//	m_size = sizeReq;
	return true;
}

HRESULT CDockWindowImpl::XDock::GetSize( SIZE *psize, BOOL bVert )
{
	_try_nested_base(CDockWindowImpl, Dock, GetSize)
	{
		pThis->OnChangeSize( CSize(*psize), bVert==TRUE );
		*psize = pThis->m_size;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CDockWindowImpl::XDock::GetDockSide( DWORD	*pdwSite )
{
	_try_nested_base(CDockWindowImpl, Dock, GetDockSide)
	{
		*pdwSite = pThis->m_dwDockSide;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDockWindowImpl::XDock::GetFixed(BOOL *pbFixed)
{
	_try_nested_base(CDockWindowImpl, Dock, GetFixed)
	{
		if(pbFixed)
			*pbFixed = pThis->m_bFixed ? TRUE : FALSE;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDockWindowImpl::XDock::OnShow()
{
	_try_nested_base(CDockWindowImpl, Dock, OnShow)
	{
		pThis->OnShow();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDockWindowImpl::XDock::OnHide()
{
	_try_nested_base(CDockWindowImpl, Dock, OnHide)
	{
		pThis->OnHide();
		return S_OK;
	}
	_catch_nested;
}

//CScrollZoomSiteImpl
CScrollZoomSiteImpl::CScrollZoomSiteImpl()
{
	m_bAutoScrollMode = true;
	m_bFitDoc = false;
	m_size = CSize( 100, 100 );
	m_fZoom = 1;
	m_ptScroll = CPoint( 0, 0 );
	m_rcBounds = NORECT;
	m_bLock = false;
	m_bLockNotify = false;
}

IMPLEMENT_UNKNOWN_BASE(CScrollZoomSiteImpl, ScrollSite)

HRESULT CScrollZoomSiteImpl::XScrollSite::GetScrollBarCtrl( DWORD sbCode, HANDLE *phCtrl )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		*phCtrl = (HANDLE)pThis->GetScrollBarCtrl( sbCode )->GetSafeHwnd();
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetClientSize( SIZE *psize )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		*psize = pThis->_GetClientSize();
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetClientSize( SIZE sizeNew )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		pThis->_SetClientSize( sizeNew );
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetScrollPos( POINT *pptPos )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		*pptPos = pThis->_GetScrollPos();
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetScrollPos( POINT ptPos )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		pThis->_SetScrollPos(  ptPos );
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetVisibleRect( RECT *pRect )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		*pRect = pThis->_GetVisibleRect();
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::EnsureVisible( RECT rect )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		if( IsRectEmpty( &rect ) )
			return S_OK;
		if( pThis->_EnsureVisible( rect ) )
			return S_OK;
		else
			return E_FAIL;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetZoom( double *pfZoom )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)

	*pfZoom = pThis->_GetZoom();
	return S_OK;
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetZoom( double fZoom )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)

	pThis->_SetZoom( fZoom );
	return S_OK;
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetScrollOffsets( RECT rcOffsets )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	pThis->_SetBoundsRect( rcOffsets );
	return S_OK;
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetAutoScrollMode( BOOL bSet )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		if( bSet )
		{
			pThis->m_bAutoScrollMode = true;
			pThis->UpdateScroll( SB_BOTH );

		}
		else
			pThis->m_bAutoScrollMode = false;
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetAutoScrollMode( BOOL* pbSet )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		if( pbSet )
		{
			*pbSet = pThis->m_bAutoScrollMode;
		}
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::LockScroll( BOOL bLock )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		pThis->m_bLock = bLock == TRUE;
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::UpdateScroll( WORD sbCode )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		pThis->UpdateScroll( sbCode );
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::GetFitDoc( BOOL* pbFitDoc )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		if( pbFitDoc )
		{
			*pbFitDoc = pThis->m_bFitDoc;
		}
		return S_OK;
	}
}

HRESULT CScrollZoomSiteImpl::XScrollSite::SetFitDoc( BOOL bFitDoc )
{
	METHOD_PROLOGUE_BASE(CScrollZoomSiteImpl, ScrollSite)
	{
		pThis->m_bFitDoc = bFitDoc;
		pThis->UpdateScroll( SB_BOTH );
		return S_OK;
	}
}



//functions shold be called from window's message
void CScrollZoomSiteImpl::OnInitScrollSite()
{
	UpdateScroll( SB_BOTH );
}

void CScrollZoomSiteImpl::OnSize()
{
	if(m_bLock)
		return;

	CPoint	ptScroll = m_ptScroll;
	UpdateScroll( SB_BOTH );


	int	dx = m_ptScroll.x - ptScroll.x;
	int	dy = m_ptScroll.y - ptScroll.y;

	if( dx == 0 && dy == 0 )
		return;

	long	nZoom  = long(m_fZoom + .5);

		

	CRect	rcClient;
	GetWindow()->GetClientRect( rcClient );
	
	GetWindow()->ScrollWindowEx( -dx, -dy, rcClient, rcClient,NULL,NULL,
		SW_INVALIDATE|SW_SCROLLCHILDREN);

	CRect	rcInvalidate = rcClient;

	if( dx > 0 )
		rcInvalidate.left = rcInvalidate.right - dx;
	if( dx < 0 )
		rcInvalidate.right = rcInvalidate.left - dx;

	if( dx != 0 )
	{
		rcInvalidate.InflateRect( nZoom, 0 );
		GetWindow()->InvalidateRect( rcInvalidate );
	}

	if( dy > 0 )
		rcInvalidate.top = rcInvalidate.bottom - dy;
	if( dy < 0 )
		rcInvalidate.bottom = rcInvalidate.top - dy;

	if( dy != 0 )
	{
		rcInvalidate.InflateRect( nZoom, 0 );
		GetWindow()->InvalidateRect( rcInvalidate );
	}
	//GetWindow()->UpdateWindow();
}
						   
void CScrollZoomSiteImpl::OnScroll( UINT nSBCode, UINT nSBActionCode, UINT nPos )
{
	if( !m_bAutoScrollMode )
		return;

	HWND hscroll = GetScrollBarCtrl( nSBCode )->GetSafeHwnd();

	if( hscroll == 0 )
		return;

	SCROLLINFO	si;

	ZeroMemory( &si, sizeof( si ) );
	si.cbSize = sizeof( si );
	si.fMask = SIF_ALL;
	::SendMessage( hscroll, SBM_GETSCROLLINFO, 0, (LPARAM)&si ); 

	int	nMax = max( si.nMax-si.nPage, 0 );
	int	nOldPos = si.nPos;

	long nVal = ::GetValueInt( ::GetAppUnknown(), "\\Interface", "LineScrollSpeed", 1 );

	switch( nSBActionCode )
	{
	case SB_BOTTOM:
		si.nPos = nMax;
		break;
	case SB_TOP:
		si.nPos = si.nMin;
		break;
	case SB_LINEDOWN:
		si.nPos += nVal;
		break;
	case SB_LINEUP:
		si.nPos -= nVal;
		break;
	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = nPos;
		break;
	case SB_THUMBPOSITION:
		si.nPos = nPos;
		break;
	case SB_ENDSCROLL:
		return;
	}

	if(!m_bLock)
	{
		si.nPos = min( nMax, max( si.nMin, si.nPos ) );
		if( nOldPos == si.nPos )
			return;
	}

	CPoint	ptOldPos = m_ptScroll;
//scroll position ajust
	long	nZoom  = long(m_fZoom + .5);
	if( nSBCode == SB_HORZ )
		m_ptScroll.x = si.nPos;
	if( nSBCode == SB_VERT )
		m_ptScroll.y = si.nPos;

	if( m_ptScroll == ptOldPos )
		return;

	if(m_bLock)
		return;

	::SendMessage( hscroll, SBM_SETSCROLLINFO, true, (LPARAM)&si );

	CRect	rcClient = GetScrollRect();

	long	dx = m_ptScroll.x - ptOldPos.x;
	long	dy = m_ptScroll.y - ptOldPos.y;

	if(dx == 0 && dy == 0)
		return;

	CRect rcScrollable = rcClient;

	::FireEvent( Unknown(), "EventGetScrollableRect", Unknown(), 0, &rcScrollable, sizeof(rcScrollable) );
	// посылаем вьюхе rcScrolable, чтобы она его откорректировала.
	// все, что не влезает в rcScrolable, но входит в rcClient, должно быть перерисовано

	GetWindow()->ScrollWindowEx( -dx, -dy, rcScrollable, rcScrollable,NULL,NULL,
		SW_INVALIDATE|SW_SCROLLCHILDREN);

	if(dy<0) rcScrollable.top -= dy;
	if(dy>0) rcScrollable.bottom -= dy;
	if(dx<0) rcScrollable.left -= dx;
	if(dx>0) rcScrollable.right -= dx;
	//rcScrollable.OffsetRect( -dx, -dy);

	/*
	CRect	rcInvalidate = rcClient;
	CRect	rcInvalidate2 = rcClient;
	rcInvalidate2.top = rcInvalidate2.bottom - 30 - max(dy,0);

	if( dx > 0 )
		rcInvalidate.left = rcInvalidate.right - dx;
	if( dx < 0 )
		rcInvalidate.right = rcInvalidate.left - dx;

	if( dy > 0 )
		rcInvalidate.top = rcInvalidate.bottom - dy;
	if( dy < 0 )
		rcInvalidate.bottom = rcInvalidate.top - dy;

	if( rcInvalidate.Width() > 0 )
		rcInvalidate.InflateRect( nZoom, 0 );
	if( rcInvalidate.Height() > 0 )
		rcInvalidate.InflateRect( 0, nZoom );
	*/

	if( dx != 0 || dy != 0 )
		::FireEvent( GetAppUnknown(), szEventScrollPosition, Unknown() );

	/*
	GetWindow()->InvalidateRect( rcInvalidate );
	GetWindow()->InvalidateRect( rcInvalidate2 );
	//GetWindow()->Invalidate();
	*/

	CRect	rcInvalidate;

	rcInvalidate = rcClient;
	rcInvalidate.right = rcScrollable.left;
	if( rcInvalidate.Width() > 0 )
		rcInvalidate.InflateRect( nZoom, 0 );
	if( rcInvalidate.Height() > 0 )
		rcInvalidate.InflateRect( 0, nZoom );
	GetWindow()->InvalidateRect( rcInvalidate );

	rcInvalidate = rcClient;
	rcInvalidate.left = rcScrollable.right;
	if( rcInvalidate.Width() > 0 )
		rcInvalidate.InflateRect( nZoom, 0 );
	if( rcInvalidate.Height() > 0 )
		rcInvalidate.InflateRect( 0, nZoom );
	GetWindow()->InvalidateRect( rcInvalidate );

	rcInvalidate = rcClient;
	rcInvalidate.bottom = rcScrollable.top;
	if( rcInvalidate.Width() > 0 )
		rcInvalidate.InflateRect( nZoom, 0 );
	if( rcInvalidate.Height() > 0 )
		rcInvalidate.InflateRect( 0, nZoom );
	GetWindow()->InvalidateRect( rcInvalidate );

	rcInvalidate = rcClient;
	rcInvalidate.top = rcScrollable.bottom;
	if( rcInvalidate.Width() > 0 )
		rcInvalidate.InflateRect( nZoom, 0 );
	if( rcInvalidate.Height() > 0 )
		rcInvalidate.InflateRect( 0, nZoom );
	GetWindow()->InvalidateRect( rcInvalidate );

	GetWindow()->UpdateWindow();
	
	//UpdateScroll( nSBCode );
}

//zooming functions
void CScrollZoomSiteImpl::_SetZoom( double fZoom )
{
	if( !m_bAutoScrollMode )
		return;

	IScrollZoomViewPtr ptrSZView( Unknown() );
	if( ptrSZView )
	{
		double fCalcZoom = fZoom;
		BOOL bProcess = FALSE;
		ptrSZView->SetZoom( fZoom, &bProcess, &fCalcZoom );
		if( bProcess )
			fZoom = fCalcZoom;
	}

	
	//fZoom = int( fZoom*16+.5)/16;
	//fZoom = int( fZoom*256+.5 )/256.;
	
	fZoom = min( max( 1./16, fZoom ), 16 );
	ASSERT( fZoom > 0. );

	if( fZoom == m_fZoom )return;
	m_fZoom = fZoom;
	
	UpdateScroll( SB_BOTH );
	if( IsWindow( *GetWindow() ) )
		GetWindow()->Invalidate();
}

//scrolling
void CScrollZoomSiteImpl::_SetClientSize( SIZE size )
{
	if( !m_bAutoScrollMode )
		return;

	IScrollZoomViewPtr ptrSZView( Unknown() );
	if( ptrSZView )
	{
		CSize sizeCalc = size;
		BOOL bProcess = FALSE;
		ptrSZView->SetClientSize( size, &bProcess, &sizeCalc );
		if( bProcess )
		{
			size.cx = sizeCalc.cx;
			size.cy = sizeCalc.cy;
		}
	}

	
	m_size = size;
	UpdateScroll( SB_BOTH );
}

void CScrollZoomSiteImpl::_SetScrollPos( CPoint ptScroll )
{
	if( !m_bAutoScrollMode )
		return;

	IScrollZoomViewPtr ptrSZView( Unknown() );
	if( ptrSZView )
	{
		CPoint ptCalc = ptScroll;
		BOOL bProcess = FALSE;
		ptrSZView->SetScrollPos( ptScroll, &bProcess, &ptCalc );
		if( bProcess )
		{
			ptScroll.x = ptCalc.x;
			ptScroll.y = ptCalc.y;			
		}
	}

	
	if( m_ptScroll == ptScroll )
		return;

	bool	bLockNotify = m_bLockNotify;
	m_bLockNotify = true;

	m_ptScroll = ptScroll;
	UpdateScroll( SB_BOTH );

	m_bLockNotify = bLockNotify;

	if( !m_bLockNotify )::FireEvent( GetAppUnknown(), szEventScrollPosition, Unknown() );

	if( IsWindow( *GetWindow() ) )
		GetWindow()->Invalidate();

	
}

//visibility and bounds
bool CScrollZoomSiteImpl::_EnsureVisible( RECT rcNeedVisible )
{
	if( !m_bAutoScrollMode )
		return false;
	//ConvertToScrollSite( Unknown(), rc );

	CSize	sizeObject = m_size;

	if (rcNeedVisible.left < 0) rcNeedVisible.left = 0;
	if (rcNeedVisible.right > sizeObject.cx) rcNeedVisible.right = sizeObject.cx;
	if (rcNeedVisible.top < 0) rcNeedVisible.top = 0;
	if (rcNeedVisible.bottom > sizeObject.cy) rcNeedVisible.bottom = sizeObject.cy;

	CRect	rcClient;
	CPoint	ptScroll, ptScrollOld = m_ptScroll;

	ptScroll = ptScrollOld;

	CRect	rc;
	rc.left = int( rcNeedVisible.left*m_fZoom + .5 );
	rc.top = int( rcNeedVisible.top*m_fZoom + .5 );
	rc.right = int( rcNeedVisible.right*m_fZoom + .5 );
	rc.bottom = int( rcNeedVisible.bottom*m_fZoom + .5 );

	CRect	rcObject;
	rcObject.left = int( rcNeedVisible.left*m_fZoom + ptScroll.x + .5 );
	rcObject.top = int( rcNeedVisible.top*m_fZoom + ptScroll.y + .5 );
	rcObject.right = int( rcNeedVisible.right*m_fZoom + ptScroll.x + .5 );
	rcObject.bottom = int( rcNeedVisible.bottom*m_fZoom + ptScroll.y + .5 );

	GetWindow()->GetClientRect( rcClient );
		
	rcClient+=ptScroll;
	CPoint	ptMaxScroll;

	if( ( rcClient.left > rc.left ||
		rcClient.top > rc.top ||
		rcClient.right <= rc.right ||
		rcClient.bottom <= rc.bottom ) )
	{
//get the current scroll info
		HWND hscrollH = GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd();
		HWND hscrollV = GetScrollBarCtrl( SB_VERT )->GetSafeHwnd();

		//paul 8.2.2001
		if( hscrollH == NULL || hscrollV == NULL )
			return false;

		SCROLLINFO	sih, siv;

		ZeroMemory( &sih, sizeof( sih ) );
		ZeroMemory( &siv, sizeof( siv ) );

		sih.fMask = SIF_ALL;
		siv.fMask = SIF_ALL;

		::SendMessage( hscrollH, SBM_GETSCROLLINFO, 0, (LPARAM)&sih );
		::SendMessage( hscrollV, SBM_GETSCROLLINFO, 0, (LPARAM)&siv );

//get scroll range
		ptMaxScroll.x = max( sih.nMax-sih.nPage, 0 );
		ptMaxScroll.y = max( siv.nMax-siv.nPage, 0 );
//calc new scroll position
		if( rcClient.left > rc.left )ptScroll.x-=rcClient.left - rc.left;
		if( rcClient.right < rc.right )ptScroll.x-=rcClient.right - rc.right-1;
		if( rcClient.top > rc.top )ptScroll.y-=rcClient.top - rc.top;
		if( rcClient.bottom < rc.bottom )ptScroll.y-=rcClient.bottom - rc.bottom-1;

		if( rcClient.Width() < rc.Width()||rc.Width()==0 ) ptScroll.x = rcObject.CenterPoint().x-rcClient.CenterPoint().x;
		if( rcClient.Height() < rc.Height()||rc.Height()==0 ) ptScroll.y = rcObject.CenterPoint().y-rcClient.CenterPoint().y;

//ajust new scroll position
		m_ptScroll.x = max( 0, min( ptMaxScroll.x, ptScroll.x ) );
		m_ptScroll.y = max( 0, min( ptMaxScroll.y, ptScroll.y ) );

		sih.fMask = SIF_ALL|SIF_DISABLENOSCROLL;
		siv.fMask = SIF_ALL|SIF_DISABLENOSCROLL;

		sih.nPos = m_ptScroll.x;
		siv.nPos = m_ptScroll.y;

		::SendMessage( hscrollH, SBM_SETSCROLLINFO, true, (LPARAM)&sih );
		::SendMessage( hscrollV, SBM_SETSCROLLINFO, true, (LPARAM)&siv );

		int	dx = m_ptScroll.x - ptScrollOld.x;
		int	dy = m_ptScroll.y - ptScrollOld.y;

		GetWindow()->GetClientRect( &rcClient );

		CRect rcScrollable = rcClient;

		::FireEvent( Unknown(), "EventGetScrollableRect", Unknown(), 0, &rcScrollable, sizeof(rcScrollable) );
		// посылаем вьюхе rcScrolable, чтобы она его откорректировала.
		// все, что не влезает в rcScrolable, но входит в rcClient, должно быть перерисовано

		GetWindow()->ScrollWindowEx( -dx, -dy, rcScrollable, rcScrollable,NULL,NULL,
			SW_INVALIDATE|SW_SCROLLCHILDREN);

		if(dy>0) rcScrollable.top += dy;
		if(dy<0) rcScrollable.bottom += dy;
		if(dx>0) rcScrollable.left += dx;
		if(dx<0) rcScrollable.right += dx;
		//rcScrollable.OffsetRect( -dx, -dy);

		if( dx != 0 || dy != 0 )
			::FireEvent( GetAppUnknown(), szEventScrollPosition, Unknown() );

		CRect	rcInvalidate;

		long	nZoom  = long(m_fZoom + .5);
		rcInvalidate = rcClient;
		rcInvalidate.right = rcScrollable.left;
		if( rcInvalidate.Width() > 0 )
			rcInvalidate.InflateRect( nZoom, 0 );
		if( rcInvalidate.Height() > 0 )
			rcInvalidate.InflateRect( 0, nZoom );
		GetWindow()->InvalidateRect( rcInvalidate );

		rcInvalidate = rcClient;
		rcInvalidate.left = rcScrollable.right;
		if( rcInvalidate.Width() > 0 )
			rcInvalidate.InflateRect( nZoom, 0 );
		if( rcInvalidate.Height() > 0 )
			rcInvalidate.InflateRect( 0, nZoom );
		GetWindow()->InvalidateRect( rcInvalidate );

		rcInvalidate = rcClient;
		rcInvalidate.bottom = rcScrollable.top;
		if( rcInvalidate.Width() > 0 )
			rcInvalidate.InflateRect( nZoom, 0 );
		if( rcInvalidate.Height() > 0 )
			rcInvalidate.InflateRect( 0, nZoom );
		GetWindow()->InvalidateRect( rcInvalidate );

		rcInvalidate = rcClient;
		rcInvalidate.top = rcScrollable.bottom;
		if( rcInvalidate.Width() > 0 )
			rcInvalidate.InflateRect( nZoom, 0 );
		if( rcInvalidate.Height() > 0 )
			rcInvalidate.InflateRect( 0, nZoom );
		GetWindow()->InvalidateRect( rcInvalidate );

		return true;
	}
	return false;
}

CRect CScrollZoomSiteImpl::_GetVisibleRect()
{
	if( !m_bAutoScrollMode )
		return NORECT;
	CRect	rc = GetScrollRect();
	//ConvertToClient( Unknown(), rc );
	return rc;
}

void CScrollZoomSiteImpl::_SetBoundsRect( RECT rc )
{
	m_rcBounds = rc;
}
//private helpers
CRect CScrollZoomSiteImpl::GetScrollRect()
{
	CWnd	*pwnd = GetWindow();
	CRect	rc;
	pwnd->GetClientRect( rc );

	rc.InflateRect( -m_rcBounds.left, -m_rcBounds.top,
					-m_rcBounds.right, -m_rcBounds.bottom );

	return rc;
}

CSize CScrollZoomSiteImpl::GetClientSize()
{
	return CSize( int( m_size.cx * m_fZoom + .5 ),
		int( m_size.cy * m_fZoom + .5 ) );
}

void CScrollZoomSiteImpl::UpdateScroll( int sbCode )
{
	if( !m_bAutoScrollMode )
		return;

	// 27.01.2006 build 97
	// для фиксированного FitDocToScreen
	//int nFitDock = ::GetValueInt(::GetAppUnknown(),"\\Zoom","FitDocToScreen",0);
	if(m_bFitDoc)
	{ //set zoom
		static bool bLock=false; // защита от рекурсии - мало ли что
		if(!bLock)
		{
			bLock = true;

			IWindowPtr	sptrWnd(Unknown());
			HWND hWnd = GetWindow()->GetSafeHwnd();
			if(hWnd != 0)
			{
				CRect rcClient = NORECT;
				::GetClientRect(hWnd, &rcClient);

				CSize sizeClient = GetClientSize();

				double fZoom = 1;
				double fZoomX = rcClient.Width()/double(m_size.cx+5);
				double fZoomY = rcClient.Height()/double(m_size.cy+5);
				fZoom = fZoomX > fZoomY ? fZoomY : fZoomX;
				
				if( fabs( fZoom-m_fZoom ) > 1e-4 )
					_SetZoom(fZoom);
			}

			bLock=false;
		}
	}
	// закончили FitDock
	
	if( GetWindow()->GetSafeHwnd() == 0 )
		return;

	long	nZoom  = long(m_fZoom + .5);

	if( sbCode == SB_BOTH )
	{
		UpdateScroll( SB_VERT );
		UpdateScroll( SB_HORZ );
		return;
	}

	CRect	rcClient = GetScrollRect();
	CSize	sizeClient = GetClientSize();
	
	HWND hscroll = GetScrollBarCtrl( sbCode )->GetSafeHwnd();

	if( !hscroll )	return;
	SCROLLINFO	si;

	si.cbSize = sizeof( si );
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS|SIF_DISABLENOSCROLL;
	SendMessage( hscroll, SBM_GETSCROLLINFO, 0, (LPARAM)&si );
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS|SIF_DISABLENOSCROLL;
	si.nMin = 0;

	if( sbCode == SB_HORZ )
	{
		si.nMax = sizeClient.cx;
		si.nPos = m_ptScroll.x;
		si.nPage = rcClient.Width();
	}
	else
	{
		si.nMax = sizeClient.cy;
		si.nPos = m_ptScroll.y;
		si.nPage = rcClient.Height();
	}

	si.nPos = max( min( si.nMax-(int)si.nPage, si.nPos ), si.nMin );
	bool	bDisable = (si.nMax - int(si.nPage)) <= 0;

	if( bDisable )
	{
		si.nPos = 0;
		// [vanek & aam]: BT2000: 3353, 3368, 3369
		::SendMessage( hscroll, SBM_SETSCROLLINFO, false, (LPARAM)&si );
		::EnableWindow( hscroll, false );
		::SendMessage( hscroll, SBM_ENABLE_ARROWS, ESB_DISABLE_BOTH, 0 );
	}
	else
	{
		::EnableWindow( hscroll, true );
//		::SendMessage( hscroll, SBM_ENABLE_ARROWS, ESB_ENABLE_BOTH, 0 );
		::SendMessage( hscroll, SBM_SETSCROLLINFO, true, (LPARAM)&si );
	}

	if( sbCode == SB_HORZ )
		m_ptScroll.x = si.nPos;
	else
		m_ptScroll.y = si.nPos;

	if( !m_bLockNotify )
		::FireEvent( GetAppUnknown(), szEventScrollPosition, Unknown() );
}

UINT WM_NOTIFY_CHILD = RegisterWindowMessage("WM_NOTIFY_CHILD");



/////////////////////////////////////////////////////////////////////////////////////
//CStatusPaneBase

BEGIN_MESSAGE_MAP(CStatusPaneBase, CWnd)
	//{{AFX_MSG_MAP(CStatusPaneBase)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CStatusPaneBase, CWnd)
	INTERFACE_PART(CStatusPaneBase, IID_IWindow, Wnd)
	INTERFACE_PART(CStatusPaneBase, IID_IStatusPane, Pane)
	INTERFACE_PART(CStatusPaneBase, IID_INamedObject2, Name)
END_INTERFACE_MAP()

CStatusPaneBase::CStatusPaneBase()
{
	m_sName = "Base";
}


CStatusPaneImpl::CStatusPaneImpl()
{
	m_nWidth = 0;
	m_paneSide = psRight;
	IUnknown* punkStatus;
	sptrIApplication sptrApp(GetAppUnknown());
	sptrApp->GetStatusBar( &punkStatus );
	//m_sptrStatus = punkStatus;
	if( punkStatus )punkStatus->Release();
}

CStatusPaneImpl::~CStatusPaneImpl()
{
}

IMPLEMENT_UNKNOWN_BASE(CStatusPaneImpl, Pane)

HRESULT CStatusPaneImpl::XPane::GetWidth( int *pWidth )
{
	_try_nested_base(CStatusPaneImpl, Pane, GetWidth)
	{
		if(pWidth)
			*pWidth = pThis->m_nWidth;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CStatusPaneImpl::XPane::GetSide( DWORD *pdwSide )
{
	_try_nested_base(CStatusPaneImpl, Pane, GetSide)
	{
		if(pdwSide)
			*pdwSide = pThis->m_paneSide;
		return S_OK;
	}
	_catch_nested;
}


void CStatusPaneBase::OnFinalRelease()
{
	if( IsWindow(GetSafeHwnd()) )
		DestroyWindow();
	delete this;
}

//the base class for dialog pages

BEGIN_INTERFACE_MAP(CPropertyPageBase, CDialog)
	INTERFACE_PART(CPropertyPageBase, IID_IWindow, Wnd)
	INTERFACE_PART(CPropertyPageBase, IID_IOptionsPage, Page)
	INTERFACE_PART(CPropertyPageBase, IID_IPersist, Persist)
	INTERFACE_PART(CPropertyPageBase, IID_IHelpInfo, Help)
	INTERFACE_PART(CPropertyPageBase, IID_INamedObject2, Name)
	INTERFACE_AGGREGATE( CPropertyPageBase, m_punkNotifyCtrl )
END_INTERFACE_MAP()

BEGIN_MESSAGE_MAP(CPropertyPageBase, CDialog)
	//{{AFX_MSG_MAP(CPropertyPageBase)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

IMPLEMENT_UNKNOWN_BASE(CPropertyPageBase, Page);
IMPLEMENT_UNKNOWN_BASE(CPropertyPageBase, Persist);


HRESULT CPropertyPageBase::XPersist::GetClassID( CLSID *pClassID )
{
	_try_nested_base(CPropertyPageBase, Persist, GetClassID)
	{
		if (!pClassID)
			return E_POINTER;

		GuidKey guidInner;
		pThis->GetClassID(&guidInner);

		IVTApplicationPtr sptrA = GetAppUnknown(false);
		sptrA->GetEntry(1, (DWORD*)&guidInner, (DWORD*)pClassID, 0);

		return S_OK;
	}
	_catch_nested;
}


HRESULT CPropertyPageBase::XPage::LoadSettings()
{
	_try_nested_base(CPropertyPageBase, Page, LoadSettings)
	{
		pThis->DoLoadSettings();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPropertyPageBase::XPage::StoreSettings()
{
	_try_nested_base(CPropertyPageBase, Page, StoreSettings)
	{
		pThis->DoStoreSettings();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPropertyPageBase::XPage::GetSize( SIZE *psize )
{
	_try_nested_base(CPropertyPageBase, Page, GetSize)
	{
		CDialogTemplate dt;
		dt.Load( MAKEINTRESOURCE(pThis->m_nID) );
		dt.GetSizeInDialogUnits( psize );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CPropertyPageBase::XPage::GetCaption( BSTR *pbstrCaption )
{
	_try_nested_base(CPropertyPageBase, Page, GetCaption)
	{
		*pbstrCaption = pThis->m_sName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPropertyPageBase::XPage::OnSetActive()
{
	_try_nested_base(CPropertyPageBase, Page, GetCaption)
	{
		if (pThis->OnSetActive())
			return S_OK;
		else
			return E_FAIL;
	}
	_catch_nested;
}

CPropertyPageBase::CPropertyPageBase( UINT nID ) : CDialog( nID )
{
	EnableAggregation();

	m_nID = nID;

	INotifyControllerPtr	sptrNC;
	sptrNC.CreateInstance( szNotifyCtrlProgID );

	m_punkNotifyCtrl = sptrNC.Detach();

	m_bInitCalled = false;
}

CPropertyPageBase::~CPropertyPageBase()
{
	if( m_punkNotifyCtrl )
		m_punkNotifyCtrl->Release();
	m_punkNotifyCtrl = 0;
}

CWnd *CPropertyPageBase::GetWindow()
{
	return this;
}

bool CPropertyPageBase::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	return Create( m_nID, pparent ) != 0;
}

void CPropertyPageBase::UpdateAction()
{
	::FireEvent( m_punkNotifyCtrl, szEventChangeProperties );
}

BOOL CPropertyPageBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInitCalled = true;

	return true;
}

void CPropertyPageBase::OnFinalRelease()
{
	if(m_hWnd != 0)
		DestroyWindow();
	delete this;
}


void CPropertyPageBase::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_bInitCalled = false;
}
