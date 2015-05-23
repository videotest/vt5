#include "stdafx.h"
#include "impl_mouse.h"
#include "com_main.h"
#include "misc_utils.h"
#include "math.h"


/////////////////////////////////////////////////////////////////////////////////////////
//CMouseImpl
CMouseImpl::CMouseImpl()
{
	m_bTracking = false;
	m_bLeftButton = true;
	m_bTestTracking = false;
	m_pointTestTracking.x = -1;
	m_pointTestTracking.y = -1;
}

CMouseImpl::~CMouseImpl()
{
}

void CMouseImpl::InitMouse( IUnknown *punkSite )
{
	m_ptrSite = punkSite;
}

void CMouseImpl::DeInitMouse()
{
	m_ptrSite = 0;
}


LRESULT	CMouseImpl::DoMouseMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	bool	bResult = false;

	_point	point;

	if( nMsg >= WM_MOUSEFIRST && nMsg <= WM_MOUSELAST )
	{
		point.from_lparam( lParam );
		m_pointLocal = ::_client( point, m_ptrSite );
	}

	

	if( nMsg == WM_MOUSEMOVE )
	{
		if( m_bTestTracking )
		{
			if( CheckTracking() )
				bResult = StartTracking( m_pointStartTracking );
		}
		else if( m_bTracking )
		{
			bResult = DoTrack( m_pointLocal );
			EnsureVisible( m_pointLocal );
		}
		else bResult = DoMouseMove( m_pointLocal );
	}
	else if( nMsg == WM_LBUTTONDOWN )
	{
		::GetCursorPos( &m_pointTestTracking );
		m_bLeftButton = true;
		m_bTestTracking = true;
		m_pointStartTracking = m_pointLocal;
		bResult = DoLButtonDown( m_pointLocal );

	}
	else if( nMsg == WM_LBUTTONUP )
	{
		m_bTestTracking = false;
		if( m_bTracking && m_bLeftButton )
		{
			EndTracking( false );
			bResult = true;
		}
		else
			bResult = DoLButtonUp( m_pointLocal );
	}
	else if( nMsg == WM_RBUTTONDOWN )
	{
		::GetCursorPos( &m_pointTestTracking );
		m_bTestTracking = true;
		m_bLeftButton = false;
		m_pointStartTracking = m_pointLocal;
		bResult = DoRButtonDown( m_pointLocal );
	}
	else if( nMsg == WM_RBUTTONUP )
	{
		m_bTestTracking = false;
		if( m_bTracking && !m_bLeftButton )
		{
			EndTracking( true );
			bResult = true;
		}
		else
			bResult = DoRButtonUp( m_pointLocal );
	}
	else if( nMsg == WM_LBUTTONDBLCLK )
	{
		bResult = DoLButtonDblClick( m_pointLocal );
	}
	else if( nMsg == WM_RBUTTONDBLCLK )
	{
		bResult = DoRButtonDblClick( m_pointLocal );
	}
	else if( nMsg == WM_CHAR )
	{
		bResult = DoChar( wParam, true );
	}
	else if( nMsg == WM_KEYDOWN )
	{
		bResult = DoChar( wParam, false );
	}

	return bResult;
}

bool CMouseImpl::CheckTracking()
{
	_point	point;
	::GetCursorPos( &point );
	
	if( abs( m_pointTestTracking.y - point.y ) < 5 &&
		abs( m_pointTestTracking.x - point.x ) < 5 )
		return false;
	return true;
}


bool CMouseImpl::StartTracking( _point point )
{
	m_bTracking = true;
	m_bTestTracking = false;

	HWND	hwnd;
	IWindowPtr	ptrWindow( m_ptrSite );
	ptrWindow->GetHandle( (HANDLE*)&hwnd );
	::SetCapture( hwnd );

	DoStartTracking( point );
	return true;
}

bool CMouseImpl::EndTracking( bool bTerminate )
{
	DoFinishTracking( m_pointLocal );

	::ReleaseCapture();
	m_bTracking = false;
	return true;
}

void CMouseImpl::EnsureVisible( _point point )
{
	
}