// OleView.cpp: implementation of the COleView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OleView.h"
#include "constant.h"
#include "misc_utils.h"
#include "nameconsts.h"
#include "core5.h"
#include "com_main.h"
#include "resource.h"

#include "MenuRegistrator.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COleView::COleView()
{	
	m_bstrName		= "Ole View";

	char sz_buf[256];
	sz_buf[0] = 0;

	::LoadString( App::handle(), IDS_OLE_VIEW_NAME, sz_buf, sizeof(sz_buf) );
	
	m_bstrUserName	= sz_buf;

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;
}

//////////////////////////////////////////////////////////////////////
COleView::~COleView()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnPaint()
{
	PAINTSTRUCT	ps;	

	RECT	rectPaint;
	::GetUpdateRect( m_hwnd, &rectPaint, false );

	RECT	rectClient;
	::GetClientRect( m_hwnd, &rectClient );
	
	if( rectPaint.right - rectPaint.left <= 0 )
	{
		::BeginPaint( m_hwnd, &ps );	
		::EndPaint( m_hwnd, &ps );
		return 1;
	}

	if( rectPaint.bottom - rectPaint.top <= 0 )
	{
		::BeginPaint( m_hwnd, &ps );	
		::EndPaint( m_hwnd, &ps );
		return 1;
	}

	HDC hdcPaint = ::BeginPaint( m_hwnd, &ps );	


	BOOL bWasCreated = FALSE;
	if( m_ptrActiveObject )
	{
		m_ptrActiveObject->IsObjectCreated( &bWasCreated );
		
		BOOL bCanProcessDrawing = FALSE;
		m_ptrActiveObject->CanProcessDrawig( m_hwnd, &bCanProcessDrawing );
		if( bCanProcessDrawing )
		{
			IScrollZoomSitePtr pSite( Unknown() );
			
			
			double fZoom = 1.0;
			pSite->GetZoom( &fZoom );

			POINT ptScroll;
			if( pSite )
			{
				pSite->GetZoom( &fZoom );
				pSite->GetScrollPos( &ptScroll );
			}
			
			_brush brush( ::CreateSolidBrush( RGB(255,255,255) ) );
			::FillRect( hdcPaint, &rectClient, brush );


			SIZE sizeClip;
			sizeClip.cx = long( double(rectClient.right - rectClient.left ) / fZoom );
			sizeClip.cy = long( double(rectClient.bottom - rectClient.top ) / fZoom );

			ptScroll.x	= long( double(ptScroll.x) / fZoom );;
			ptScroll.y	= long( double(ptScroll.y) / fZoom );;

			m_ptrActiveObject->DrawMeta( DVASPECT_CONTENT, m_hwnd, hdcPaint, rectClient, ptScroll, sizeClip );
		}

		//::IntersectClipRect( hdcPaint, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
	}				

	if( !bWasCreated )
	{
		_brush brush( ::CreateSolidBrush( ::GetSysColor( COLOR_3DFACE ) ) );
		::FillRect( hdcPaint, &rectPaint, brush );
	}

	::EndPaint( m_hwnd, &ps );
	

	return 1;
}

//////////////////////////////////////////////////////////////////////
LRESULT	COleView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	switch( nMsg )
	{
		case WM_PAINT:			return OnPaint();				
		case WM_ERASEBKGND:		return OnEraseBackground( (HDC)wParam );
		case WM_SIZE:			return OnSize( (int)LOWORD(lParam), (int)HIWORD(lParam) );
		case WM_DESTROY:		return OnDestroy();
		case WM_LBUTTONDBLCLK:	return OnLButtonDblClick( (int)LOWORD(lParam), (int)HIWORD(lParam) );
		case WM_CONTEXTMENU:	return OnContextMenu( HWND(wParam), (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam) );
		case WM_SETFOCUS:		return OnSetFocus( HWND(wParam) );
	}

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetHwnd( HWND* phwnd )
{
	*phwnd = hwnd();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void COleView::ReAttachObject( bool bForceReattach )
{
	IUnknown	*punkOleObject = 0;	
	IDataContext3Ptr	ptrC( Unknown() );
	if( ptrC == 0 )
		return;

	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkOleObject );	
	IOleObjectDataPtr ptr = punkOleObject;
	
	if( ptr == 0 )
	{
		SIZE size;
		size.cx = size.cy = 0;
		SetClientSize( size ); 
	}

	if( punkOleObject != 0 )
		punkOleObject->Release();


	if( ::GetKey( m_ptrActiveObject ) != ::GetKey( ptr ) || bForceReattach )
	{
		if( m_ptrActiveObject != NULL )
		{				
			m_ptrActiveObject->Deactivate( m_hwnd );				
		}

		m_ptrActiveObject = ptr;
		
		if( m_ptrActiveObject )
		{
			SIZE size;	
			::ZeroMemory( &size, sizeof(SIZE) );

			m_ptrActiveObject->GetScrollSize( &size, m_hwnd );
			
			if( size.cx < 1 )	size.cx = 100;
			if( size.cy < 1 )	size.cy = 100;

			SetClientSize( size );
			
			/*
			long lAutoActivate = ::GetValueInt( ::GetAppUnknown(), "OleClient", "Auto Activate", 0L );
			if( lAutoActivate == 1 )
			{
				m_ptrActiveObject->Activate( m_hwnd );
				return;
			}
			*/
			
		}		
	}

	if( ::IsWindow( m_hwnd ) )
	{
		InvalidateRect( m_hwnd, 0, 0 );
		UpdateWindow( m_hwnd );
	}

}

//////////////////////////////////////////////////////////////////////
void COleView::DoAttachObjects()
{	 
	ReAttachObject( false );	
}

//////////////////////////////////////////////////////////////////////
void COleView::SetClientSize( SIZE size )
{
	if( !::IsWindow( m_hwnd ) )
		return;


	IScrollZoomSitePtr pSite( Unknown() );
	if( pSite != 0 && size.cx >= 0 && size.cy >= 0 ) 
	{
		SIZE sizeOld;
		pSite->GetClientSize( &sizeOld );

		if( sizeOld.cx != size.cx || sizeOld.cy != size.cy )
			pSite->SetClientSize( size );
	}

}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
{
	ShowPropPage( bActivate == TRUE );
	
	if( m_ptrActiveObject )
	{			
		BOOL bObjectCreated = FALSE;
		m_ptrActiveObject->IsObjectCreated( &bObjectCreated );					
		if( bObjectCreated )
		{
			BOOL bResult = FALSE;
			if( bActivate )
			{
				m_ptrActiveObject->ActivateUI( m_hwnd );
			}
			else
			{
				m_ptrActiveObject->DeactivateUI( m_hwnd );
			}
		}
	}

	return S_OK;	
}


//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnLButtonDblClick( int x, int y )
{
	if( m_ptrActiveObject )
	{
		short nState;
		m_ptrActiveObject->GetState( &nState );
		OleItemState is = (OleItemState)nState;
		if( is == iaLoaded || is == iaFullDeactivate )
		{
			m_ptrActiveObject->Activate( m_hwnd );
			return 1L;
		}

	}

	return 0; 

}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnDestroy( )
{
	if( m_ptrActiveObject )
	{		
		m_ptrActiveObject->Deactivate( m_hwnd );				
	}

	return 0;
}
		
//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	CWinViewBase::GetObjectFlags( bstrObjectType, pdwMatch );

	_bstr_t	bstr( bstrObjectType );
	if( !strcmp( bstr, szTypeOleItem ) )
		*pdwMatch = mvFull;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnEraseBackground( HDC hDC )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnSize( int cx, int cy )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

	if( m_ptrActiveObject )
	{
		m_ptrActiveObject->OnSize( cx, cy );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnContextMenu( HWND hWnd, int x, int y )
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	//ScreenToClient( m_hwnd, &pt );

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return 0;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == NULL )
		return 0;
	
	CMenuRegistrator rcm;	

	_bstr_t	bstrMenuName = rcm.GetMenu( szOleViewNameMenu, 0 );
	ptrCM->ExecuteContextMenu2( bstrMenuName, pt, 0 );	


	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT COleView::OnSetFocus( HWND hWndOld )
{
	if( m_ptrActiveObject )
	{
		short nState = 0;
		m_ptrActiveObject->GetState( &nState );
		if( iaFullActivate == nState )
		{
			m_ptrActiveObject->OnOwnerActivate( );
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
void COleView::OnNotify( const char *pszEvent, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize )
{
	CWinViewBase::OnNotify( pszEvent, pHint, pFrom, pdata, cbSize );

	
	if( !strcmp( pszEvent, szOleItemStateChange ) )
	{
		if( m_punkDocument != 0 && m_ptrActiveObject != 0 && 
				::GetKey( m_ptrActiveObject ) == ::GetKey( pHint ) )
		{
			SIZE size;
			m_ptrActiveObject->GetScrollSize( &size, m_hwnd );
			SetClientSize( size );
		}
	}
	else if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		if( ::GetKey( m_ptrActiveObject ) == ::GetKey( pFrom ) )
		{
			ReAttachObject( true );
		}
	}


}


//////////////////////////////////////////////////////////////////////
IUnknown* COleView::DoGetInterface( const IID &iid )
{
	if( iid == IID_IOleView ) return (IOleView*)this;
	else if( iid == IID_IPrintView ) return (IPrintView*)this;
	else if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;	
	else 
		return CWinViewBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	*pbContinue		= FALSE;
	*pnReturnWidth	= 20;
	*pnNewUserPosX	= 20;

	IScrollZoomSitePtr pSite( Unknown() );
	if( pSite == 0 )
		return E_FAIL;
		
	double fZoom = 1.0;
	pSite->GetZoom( &fZoom );


	if( m_ptrActiveObject )
	{
		BOOL bWasCreated = FALSE;
		m_ptrActiveObject->IsObjectCreated( &bWasCreated );
		if( !bWasCreated )
		{	
			return S_OK;
		}

		SIZE size;
		m_ptrActiveObject->GetScrollSize( &size, NULL );

		size.cx = long( double(size.cx) * fZoom );
		size.cy = long( double(size.cy) * fZoom );

		int nWidth = size.cx;
		if( nWidth > nUserPosX + nMaxWidth )
		{
			(*pnReturnWidth) = nMaxWidth;
			(*pnNewUserPosX) += (*pnReturnWidth);
			*pbContinue = TRUE;
		}
		else
		{
			(*pnReturnWidth) = nWidth - nUserPosX;
			(*pnNewUserPosX) = nWidth;
		}
	}
	else
		return S_FALSE;


	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	*pbContinue		= FALSE;
	*pnReturnHeight	= 20;
	*pnNewUserPosY	= 20;

	IScrollZoomSitePtr pSite( Unknown() );
	if( pSite == 0 )
		return E_FAIL;
		
	double fZoom = 1.0;
	pSite->GetZoom( &fZoom );


	if( m_ptrActiveObject )
	{
		BOOL bWasCreated = FALSE;
		m_ptrActiveObject->IsObjectCreated( &bWasCreated );
		if( !bWasCreated )
			return S_OK;
		
		SIZE size;
		m_ptrActiveObject->GetScrollSize( &size, NULL );

		size.cx = long( double(size.cx) * fZoom );
		size.cy = long( double(size.cy) * fZoom );


		int nHeight = size.cy;
		if( nHeight > nUserPosY + nMaxHeight )
		{
			(*pnReturnHeight) = nMaxHeight;
			(*pnNewUserPosY) += (*pnReturnHeight);
			*pbContinue = TRUE;
		}
		else
		{
			(*pnReturnHeight) = nHeight - nUserPosY;
			(*pnNewUserPosY) = nHeight;
		}
	}
	else
		return S_FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::Print( HDC hdc, RECT rcTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	if( m_ptrActiveObject )
	{
		BOOL bWasCreated = FALSE;
		m_ptrActiveObject->IsObjectCreated( &bWasCreated );
		if( !bWasCreated )
			return S_OK;

		IScrollZoomSitePtr pSite( Unknown() );
		if( pSite == 0 )
			return E_FAIL;
			
		double fZoom = 1.0;
		pSite->GetZoom( &fZoom );


		
		POINT ptScroll;
		ptScroll.x = long( double(nUserPosX) / fZoom );
		ptScroll.y = long( double(nUserPosY) / fZoom );


		SIZE sizeClip;

		sizeClip.cx = long( double(nUserPosDX) / fZoom );
		sizeClip.cy = long( double(nUserPosDY) / fZoom );

		
		/*
		SIZE sizeClip;
		sizeClip.cx = nUserPosDX;
		sizeClip.cy = nUserPosDY;
		*/

		return m_ptrActiveObject->DrawMeta( DVASPECT_CONTENT, NULL, hdc, rcTarget, ptScroll, sizeClip );
		
	}
	else
		return S_FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetClassID(CLSID *pClassID )
{
	memcpy( pClassID, &clsidOleObjectView, sizeof(CLSID) );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetFirstVisibleObjectPosition( long *plpos )
{
	*plpos = 0;

	if( m_ptrActiveObject != 0 )
		*plpos = 1;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT COleView::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{	
	long lPos		= *plPos;
	*ppunkObject	= 0;

	*plPos			= 0;

	if( lPos == 1 )
	{
		if( m_ptrActiveObject )
		{
			m_ptrActiveObject->AddRef();
			*ppunkObject = m_ptrActiveObject;
			return S_OK;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void COleView::ShowPropPage( bool bShow )
{
	IPropertySheetPtr ptrSheet;
	IOptionsPagePtr ptrPage;
	int nPage = -1;
	
	{
		IPropertySheet	*pSheet = ::FindPropertySheet();
		if( !pSheet )
			return;

		ptrSheet = pSheet;
		pSheet->Release();	pSheet = 0;
		

		IOptionsPage	*pPage = ::FindPage( ptrSheet, clsidOlePropPage, &nPage );
		if( !pPage )
			return;

		ptrPage = pPage;
		pPage->Release();	pPage = 0;
	}


	
	if( nPage == -1 )
		return;

	if( bShow )
	{
		ptrSheet->IncludePage( nPage );
	}
	else
	{
		ptrSheet->ExcludePage( nPage );
	}
		
}
