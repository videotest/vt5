#include "stdafx.h"
#include "utils.h"
#include "window5.h"
#include "core5.h"
#include "mousebase.h"
#include "resource.h"
#include "statusutils.h"
#include "ScriptNotifyInt.h"
/////////////////////////////////////////////////////////////////////////////////////////
//mouse statics
bool CMouseImpl::Enable( bool bEnable )
{	
	bool bReturn = s_bEnabled; 
	CMouseImpl::s_bEnabled = bEnable; 
	return bReturn;
}

bool CMouseImpl::s_bEnabled = true;


/////////////////////////////////////////////////////////////////////////////////////////
//mouse implementation
CMouseImpl::CMouseImpl()
{
	m_iTrackDeltaPos = 5;
	m_iTrackVisibleRange = 2;
	m_pointCurrent = CPoint( 0, 0 );
	m_bCanTrackByRButton = false;

	m_bInTrackMode = false;
	m_bTestTrackMode = false;
	m_bAutoScrollExecuted = false;
	m_bForceEnabled = false;
	m_bTrackByRButton = false;
	m_bLockMovement = false;

	m_ptStartTrack = CPoint(-1,-1);

	m_rectLock = NORECT;
	{
		LOCAL_RESOURCE;
		m_hcurStop = AfxGetApp()->LoadCursor( IDC_STOP );
	}
}


CMouseImpl::~CMouseImpl()
{
	::DestroyCursor( m_hcurStop );
}


//central processing of all mouse events
bool CMouseImpl::_MouseEvent( MouseEvent me, CPoint point )
{
	if( me == meSetCursor )
	{
		::GetCursorPos( &point );
		CWnd	*pwnd = ::GetWindowFromUnknown( GetTarget() );
		if( pwnd ) 
			pwnd->ScreenToClient( &point );
	}

	
	m_pointCurrent = point;
	IUnknown	*punk = GetTarget();

	if( CheckInterface( punk, IID_IScrollZoomSite ) )
		point = ConvertToClient( punk, point );

//	::StatusSetPaneText( guidPaneMousePos, "[%d, %d]", point.x, point.y );

	switch( me )
	{
	case meSetCursor:
	{
		if( !m_bLockMovement || m_rectLock.PtInRect( point ) )
			return DoSetCursor( point );
		else
			::SetCursor( m_hcurStop );
		return true;
	}
	case meLDown:
		m_bTrackByRButton = false;
		{
			IScriptSitePtr	sptrSS(::GetAppUnknown());
			if (sptrSS != 0)
			{
				_variant_t vars[2];
				vars[0] = _variant_t((long)point.y);
				vars[1] = _variant_t((long)point.x);
				sptrSS->Invoke(_bstr_t("MouseImpl_OnLButtonDown"), vars, 2, 0, fwFormScript);
			}
		}
		if( _CheckCoords( point ) )
			return DoLButtonDown( point );
		else
			return false;
	case meLUp:
		return DoLButtonUp( point );
	case meLDblClick:
		if( _CheckCoords( point ) )
			return DoLButtonDblClick( point );
		else
			return false;
	case meRDown:
		m_bTrackByRButton = true;
		if( _CheckCoords( point ) )
			return DoRButtonDown( point );
		else
			return false;
	case meRUp:
		return DoRButtonUp( point );
	case meRDblClick:
		if( _CheckCoords( point ) )
			return DoRButtonDblClick( point );
		else
			return false;
	case meMove:
		return DoMouseMove( point );
	case meStartTrack:
		if( !_CheckCoords( point ) )
			return true;
		return DoStartTracking( point );
	case meTrack:
		if(m_bLockMovement)
		{
			point.x = max(m_rectLock.left, min(point.x, m_rectLock.right));
			point.y = max(m_rectLock.top , min(point.y, m_rectLock.bottom));
		}
		return DoTrack( point );
	case meFinishTrack:
		return DoFinishTracking( point );
	}
	return false;
}

BOOL CMouseImpl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( !s_bEnabled && !m_bForceEnabled )
		return false;

	bool	bReturn = false;

	
	
	CPoint	point( 0, 0 );

	if( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
	{
		CPoint	pointCursor;
		GetCursorPos( &pointCursor );

		CWnd	*pwnd = GetWindowFromUnknown( GetTarget() );
		if( pwnd )
		{
			pwnd->ScreenToClient( &pointCursor );
		}

		point = pointCursor;
	}
	

	switch( message )
	{
	case WM_KEYDOWN:
		bReturn = DoVirtKey( wParam );
	case WM_KEYUP:
		{
		CWnd	*pwnd = GetWindowFromUnknown( GetTarget() );
		if( pwnd )
		{
			CRect	rect;
			pwnd->GetWindowRect( &rect );
			POINT	pt;
			::GetCursorPos( &pt );

			if( rect.PtInRect( pt ) )
				_MouseEvent( meSetCursor, pt );
		}

		return false;
		}
	case WM_SETCURSOR:
		bReturn = _MouseEvent( (MouseEvent)message, point );
		break;
	case WM_RBUTTONDOWN:
		if(!m_bCanTrackByRButton)
		{
			bReturn = _MouseEvent( (MouseEvent)message, point );
			break;
		}
	case WM_LBUTTONDOWN:
		{
			::GetCursorPos( &m_ptPossibleStartTrackAbs );
			m_ptStartTrack = point;

			bReturn = _MouseEvent( (MouseEvent)message, point );

			if( bReturn )
				break;

			if( !GetTarget() )
				return bReturn;

			if( !m_bInTrackMode  )
			{
				CPoint	pt = ::ConvertToClient( GetTarget(), point );
				if( !CanStartTracking( pt ) )
					return false;
				else
					bReturn = true;

				m_bTestTrackMode = true;
			}

			break;
		}

	case WM_RBUTTONUP:
		if(!m_bCanTrackByRButton)
		{
			bReturn = _MouseEvent( (MouseEvent)message, point );
			break;
		}
	case WM_LBUTTONUP:
		m_bTestTrackMode = false;
		if( m_bInTrackMode )
		{
			bReturn = DoLeaveMode();
			bReturn = _MouseEvent( meFinishTrack, point );
		}
		m_bInTrackMode = false;
		bReturn = _MouseEvent( (MouseEvent)message, point );
		break;

	case WM_MOUSEMOVE:
		if( m_bTestTrackMode )
		{
			CPoint	ptTest;
			::GetCursorPos( &ptTest );

			if( ::abs( ptTest.x - m_ptPossibleStartTrackAbs.x ) > m_iTrackDeltaPos ||
				::abs( ptTest.y - m_ptPossibleStartTrackAbs.y ) > m_iTrackDeltaPos )
			{
			//start tracking

				bReturn = StartTracking( m_ptStartTrack );

				if( bReturn )
					return true;
			}
		}
		else
		if( m_bInTrackMode )
		{
			if( m_bAutoScrollExecuted )
				return true;
			CWnd *pwnd = GetWindowFromUnknown( GetTarget() );
			if( !pwnd )
				return true;

			CRect	rectClient;
			pwnd->GetClientRect( &rectClient );

			rectClient.InflateRect( -10, -10 );
			
			if( !rectClient.PtInRect( point ) )
			{
				sptrIScrollZoomSite	sptrS( GetTarget() );

				if( CheckInterface( GetTarget(), IID_IScrollZoomSite ) )
				{
//						m_bAutoScrollExecuted = true;
					CPoint	pointClient = ::ConvertToClient( sptrS, point );

					CRect	rect( pointClient.x - m_iTrackVisibleRange, pointClient.y - m_iTrackVisibleRange,
								  pointClient.x + m_iTrackVisibleRange+1, pointClient.y + m_iTrackVisibleRange+1 );

					CPoint	ptOldMousePos, ptNewMousePos;
					CPoint	ptOldScroll, ptNewScroll;

					::GetCursorPos( &ptOldMousePos );

					sptrS->GetScrollPos( &ptOldScroll );
					sptrS->EnsureVisible( rect );
					sptrS->GetScrollPos( &ptNewScroll );

					if( ptNewScroll != ptOldScroll )
					{
						ptNewMousePos.x = ptOldMousePos.x  - ptNewScroll.x + ptOldScroll.x;
						ptNewMousePos.y = ptOldMousePos.y  - ptNewScroll.y + ptOldScroll.y;
						::SetCursorPos( ptNewMousePos.x, ptNewMousePos.y );

						point.x = point.x - ptNewScroll.x + ptOldScroll.x;
						point.y = point.y - ptNewScroll.y + ptOldScroll.y;
					}
//						m_bAutoScrollExecuted = false;
				}
			}
			bReturn = _MouseEvent( meTrack, point );
		}
		bReturn = _MouseEvent( (MouseEvent)message, point );
		
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		m_ptStartTrack = point;
		bReturn = _MouseEvent( (MouseEvent)message, point );
		break;
	case WM_CHAR:
		bReturn = DoChar( wParam );
		break;
	}
	*pResult = bReturn;//false;

	return bReturn;
}

bool CMouseImpl::DoEnterMode(CPoint point)
{
	CWnd *pwnd = GetWindowFromUnknown( GetTarget() );
	if( pwnd )
		pwnd->SetCapture();

	return true;
}

bool CMouseImpl::DoLeaveMode()
{
	::ReleaseCapture();
	return true;
}

bool CMouseImpl::StartTracking( CPoint point )
{
	m_bTestTrackMode = false;

	CPoint	pointClient = ::ConvertToClient( GetTarget(), point );
	if( !_CheckCoords( pointClient ) )return false;


	if( m_bInTrackMode )
		return true;
	m_bInTrackMode = DoEnterMode(point);

	if( !m_bInTrackMode )
		return true;

	return _MouseEvent( (MouseEvent)meStartTrack, m_ptStartTrack );
}

void CMouseImpl::LockMovement( bool bLock, CRect rect )
{
	m_rectLock = rect;
	m_bLockMovement = bLock;
}

void CMouseImpl::_AjustCoords( CPoint &pt )
{
	if( m_bLockMovement )
	{
		pt.x = max( m_rectLock.left, min( m_rectLock.right-1, pt.x ) );
		pt.y = max( m_rectLock.top, min( m_rectLock.bottom-1, pt.y ) );
	}
}

bool CMouseImpl::_CheckCoords( CPoint &pt )
{
	if( !m_bLockMovement )return true;
	if( m_rectLock.PtInRect( pt ) )return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CDrawControllerBase implementation
BEGIN_INTERFACE_MAP(CDrawControllerBase, CCmdTarget)
	INTERFACE_PART(CDrawControllerBase, IID_IMouseDrawController, DrawCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDrawControllerBase, DrawCtrl);

CDrawControllerBase::CDrawControllerBase()
{
}
CDrawControllerBase::~CDrawControllerBase()
{
}

void CDrawControllerBase::Install()
{
	::InstallController( GetControllingUnknown() );
}

void CDrawControllerBase::Uninstall()
{
	::UnInstallController( GetControllingUnknown() );
}

void CDrawControllerBase::Redraw( bool bDraw )
{
	::RepaintMouseCtrl( 0, 0, bDraw );
}

HRESULT CDrawControllerBase::XDrawCtrl::DoDraw( HDC hDC, IUnknown *punkVS, POINT pointPos, BOOL bErase )
{
	_try_nested(CDrawControllerBase, DrawCtrl, DoDraw)
	{
		CDC	*pDC = CDC::FromHandle( hDC );
		CPoint	point = pointPos;

		pDC->SaveDC();
		pDC->SetROP2( R2_XORPEN );
		pDC->SelectStockObject( WHITE_PEN );
		pDC->SelectStockObject( NULL_BRUSH );

		pThis->DoDraw( *pDC, point, punkVS, bErase == TRUE );

		pDC->RestoreDC( -1 );

		return S_OK;
	}
	_catch_nested;
}

