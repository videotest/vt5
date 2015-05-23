// ActionInteractiveScroll.cpp: implementation of the CActionInteractiveScroll class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionInteractiveScroll.h"
#include "resource.h"
#include "actions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CActionInteractiveScroll, CCmdTargetEx);

// {70AB0294-8980-4ee3-B317-2EC63413B5D3}
GUARD_IMPLEMENT_OLECREATE( CActionInteractiveScroll, "Zoom.InteractiveScroll", 
0x70ab0294, 0x8980, 0x4ee3, 0xb3, 0x17, 0x2e, 0xc6, 0x34, 0x13, 0xb5, 0xd3);

// {AED2C712-062C-44ab-93EE-BDF534A30333}
static const GUID guidInteractiveScroll = 
{ 0xaed2c712, 0x62c, 0x44ab, { 0x93, 0xee, 0xbd, 0xf5, 0x34, 0xa3, 0x3, 0x33 } };

ACTION_INFO_FULL(CActionInteractiveScroll, ID_ACTION_INTERACTIVE_SCROLL, -1, -1, guidInteractiveScroll);
ACTION_TARGET(CActionInteractiveScroll, szTargetViewSite);

#define MOUSE_SECTION				"IntelliMouse"

//mouse wheel
#define MOUSE_WHEEL_ENABLE			"Wheel enable"
#define MOUSE_WHEEL_HORZ_KEY		"Wheel horz scroll key"
#define MOUSE_WHEEL_SCROLL_STEP		"Wheel step"
#define MOUSE_WHEEL_INVERT_HORZ		"Invert wheel horz direction"
#define MOUSE_WHEEL_INVERT_VERT		"Invert wheel vert direction"

//mouse scroll
#define MOUSE_SCROLL_ENABLE			"Scroll enable"
#define MOUSE_SCROLL_BTN			"Scroll key"

#define MOUSE_SCROLL_INVERT_HORZ	"Invert scroll horz direction"
#define MOUSE_SCROLL_INVERT_VERT	"Invert scroll invert vert direction"


CActionInteractiveScroll::CActionInteractiveScroll()
{
	m_bInvertScrollVertDirection = m_bInvertScrollHorzDirection = m_bMPressed = false;
	
	CWinApp *pApp = AfxGetApp();
	if( pApp )
	{
		m_hcurCatch = pApp->LoadCursor( IDC_CURSOR_CATCH );
		m_hcurFree  = pApp->LoadCursor( IDC_CURSOR_FREE );
		m_hcurOld = ::GetCursor();
	}
}

CActionInteractiveScroll::~CActionInteractiveScroll()
{
	if( m_hcurCatch )
		::DestroyCursor( m_hcurCatch );
	if( m_hcurFree )
		::DestroyCursor( m_hcurFree );
}

bool CActionInteractiveScroll::DoLButtonDown( CPoint pt )
{

	m_ptStart = _convert_point( pt );
	m_bMPressed = true;

	CWnd *pWnd = ::GetWindowFromUnknown( m_punkTarget );

	if( pWnd )
		pWnd->SetCapture();

	m_hcurActive = m_hcurCatch;
	::SetCursor( m_hcurActive );
	return true;
}

bool CActionInteractiveScroll::DoMouseMove( CPoint pt )
{
	if( m_bMPressed )
	{
		pt = _convert_point( pt );

		CPoint ptDelta = m_ptStart - pt;

		if( m_bInvertScrollHorzDirection )
			ptDelta.x = -ptDelta.x;

		if( m_bInvertScrollVertDirection )
			ptDelta.y = -ptDelta.y;

		_scroll_delta( ptDelta );

		m_ptStart = pt;
	}
	else
		m_hcurActive = m_hcurFree;
	return true;
}

bool CActionInteractiveScroll::DoRButtonDown( CPoint pt )
{
	_activate_object( pt );
	return false;
}

CPoint CActionInteractiveScroll::_convert_point( CPoint pt )
{
	pt = ::ConvertToWindow( m_punkTarget, pt );
	return pt;
}

bool CActionInteractiveScroll::DoLButtonUp( CPoint pt )
{
	{
		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( pWnd )
		{
			CRect rc;
			pWnd->GetWindowRect( &rc );

			CPoint pt;

			GetCursorPos( &pt );

			if( !rc.PtInRect( pt ) )
				m_hcurActive = m_hcurOld;
			else
				m_hcurActive = m_hcurFree;
		}
	}

	::SetCursor( m_hcurActive );


	m_bMPressed = false;
	ReleaseCapture();
	return false;
}

void CActionInteractiveScroll::_scroll_delta( CPoint ptDelta )
{
	IScrollZoomSitePtr ptrZ = m_punkTarget;
	if( ptrZ == 0 )
		return;

	SIZE size;
	CPoint ptScrll;
	double fZoom = 0;

	ptrZ->GetClientSize( &size );
	ptrZ->GetScrollPos( &ptScrll );
 	ptrZ->GetZoom( &fZoom );


//	RECT rc;

//	ptrZ->GetVisibleRect( &rc );

	size.cx = long( double( size.cx ) * fZoom );// - ( rc.right - rc.left );
	size.cy = long( double( size.cy ) * fZoom );// - ( rc.bottom - rc.top );

	ptScrll += ptDelta;

	if( ptScrll.x >= size.cx )
		ptScrll.x = size.cx-1;

	if( ptScrll.y >= size.cy ) 
		ptScrll.y = size.cy-1;

	if( ptScrll.x < 0 ) 
		ptScrll.x = 0;

	if( ptScrll.y < 0 ) 
		ptScrll.y = 0;

/*
	m_ptStart = pt + ptDelta;

	if( ptScrll.y + ptDelta.y < 0 || ptScrll.y + ptDelta.y >= size.cy ) 
		m_ptStart.y = pt.y;
	if( ptScrll.x + ptDelta.x < 0 || ptScrll.x + ptDelta.x >= size.cx ) 
		m_ptStart.x = pt.x;
*/
	ptrZ->SetScrollPos( ptScrll );
}

bool CActionInteractiveScroll::Initialize()
{
	long lVal = 0;

	lVal = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_INVERT_HORZ, 0 );
	m_bInvertScrollHorzDirection = ( lVal == 1L ? true : false );

	lVal = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_INVERT_VERT, 0 );
	m_bInvertScrollVertDirection = ( lVal == 1L ? true : false );

	m_hcurActive = m_hcurFree;

	return CInteractiveActionBase::Initialize();

}

void CActionInteractiveScroll::_activate_object( CPoint pt )
{
	IUnknown *ptrDocList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	bool bFounded = false;

	if( !ptrDocList )
		return;

	INamedDataObject2Ptr	ptrListObject = ptrDocList;
	ptrDocList->Release();

	if( ptrListObject == 0 )
		return;


	POSITION posStart = 0, lPrevPos = 0;
	
	ptrListObject->GetFirstChildPosition( &posStart );

	while( posStart )
	{
		IUnknown *punkObject = 0;
		lPrevPos = posStart;
		ptrListObject->GetNextChild( &posStart, &punkObject );

		if( punkObject )
		{
			IMeasureObjectPtr pObject = punkObject;
			punkObject->Release();

			if( pObject == 0 )
				continue;
		
			IUnknown *pImage = 0;
			pObject->GetImage( &pImage );

			if( !pImage )
				continue;

			IImage3Ptr ptrImage = pImage;
			pImage->Release();

			if( ptrImage == 0 )
				continue;

			RECT rcImage;
			_get_img_rect( pImage , rcImage );

			if( ::PtInRect( &rcImage, pt ) )
			{
				BYTE *pByte = 0;
				ptrImage->GetRowMask( pt.y - rcImage.top, &pByte );

				if( pByte[pt.x - rcImage.left] )
				{
					ptrListObject->SetActiveChild( lPrevPos );
					return;
				}
			}
		}
	}
}

void CActionInteractiveScroll::_get_img_rect(IUnknown *pImage,RECT &rc)
{
	IImage3Ptr ptrImage = pImage;

	if( ptrImage == 0 )
		return;

	POINT ptImgOffset;
	ptrImage->GetOffset( &ptImgOffset );

	int w = 0,h = 0;
	ptrImage->GetSize( &w,&h );

	rc.left = ptImgOffset.x;
	rc.top = ptImgOffset.y;

	rc.right = rc.left + w;
	rc.bottom = rc.top + h;
}

bool CActionInteractiveScroll::IsAvaible()
{
	if( !m_punkTarget ) 
		return false;
	
	CZoomUndoable zoom; 

	if( !zoom.IsAvaible( m_punkTarget ) )
		return false;
	
	CWnd *pWnd = ::GetWindowFromUnknown( m_punkTarget );
	
	if( pWnd )
	{
		IScrollZoomSitePtr zoom = m_punkTarget;

		HANDLE hHorz = 0 , hVert = 0;

		zoom->GetScrollBarCtrl( SB_HORZ, &hHorz );
		zoom->GetScrollBarCtrl( SB_VERT, &hVert );

		if( hHorz || hVert )
		{
			CWnd *pHorz = CWnd::FromHandle( (HWND)hHorz );
			CWnd *pVert = CWnd::FromHandle( (HWND)hVert );

			if( pHorz || pVert )
				return ( (pHorz && pHorz->IsWindowEnabled() ) || (pVert && pVert->IsWindowEnabled() ) );
		}
	}	
	return false;
}

void CActionInteractiveScroll::Finalize()
{
	if( m_bMPressed )
	{
		m_hcurActive = m_hcurFree;
		::SetCursor( m_hcurActive );

		m_bMPressed = false;
		ReleaseCapture();
	}
	else
	{
		m_hcurActive = m_hcurOld;
		::SetCursor( m_hcurActive );
	}

	CInteractiveActionBase::Finalize();
}

void CActionInteractiveScroll::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CInteractiveActionBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if( !strcmp(pszEvent, szAppActivateView) || !strcmp(pszEvent, szEventScrollPosition) )
	{
		if( !IsAvaible() )
			Terminate();

	}
}