#include "stdafx.h"
#include "fftdraw.h"
#include "math.h"

IMPLEMENT_DYNCREATE(CFFTDrawController, CCmdTargetEx);

// {ED918358-8BFC-4055-8A9B-AFC7BDBEAE80}
GUARD_IMPLEMENT_OLECREATE(CFFTDrawController, "fft.DrawController", 
0xed918358, 0x8bfc, 0x4055, 0x8a, 0x9b, 0xaf, 0xc7, 0xbd, 0xbe, 0xae, 0x80);

BEGIN_INTERFACE_MAP(CFFTDrawController, CCmdTargetEx)
	INTERFACE_PART(CFFTDrawController, IID_IFFTDrawController, FftDraw)
	INTERFACE_PART(CFFTDrawController, IID_IDrawObject, Draw)
	INTERFACE_PART(CFFTDrawController, IID_IMsgListener, MsgList)
	INTERFACE_PART(CFFTDrawController, IID_IEventListener, EvList)
	
END_INTERFACE_MAP()

BEGIN_MESSAGE_MAP(CFFTDrawController, CCmdTargetEx)
	//{{AFX_MSG_MAP(CFFTDrawController)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


IMPLEMENT_UNKNOWN(CFFTDrawController, FftDraw)
IMPLEMENT_UNKNOWN(CFFTDrawController, MsgList)


CFFTDrawController::CFFTDrawController()
{
	m_punkDoc = 0;
	m_pointFFTPos = CPoint( 0, 0 );
	m_sizeFFT = CSize( 0, 0 );

	Register( GetAppUnknown() );	
	m_hwndLastSource = 0;
}

CFFTDrawController::~CFFTDrawController()
{
}

void CFFTDrawController::OnFinalRelease()
{
	m_xFftDraw.DeInit();
	UnRegister();

	delete this;
}

HRESULT CFFTDrawController::XMsgList::OnMessage( MSG *pmsg, LRESULT *plReturn )
{

	METHOD_PROLOGUE_EX(CFFTDrawController, MsgList);

	if( pmsg->message == WM_MOUSEMOVE )
	{
		CPoint	point;
		POINTSTOPOINT( point, pmsg->lParam );
		pThis->DoMouseMove( point, pmsg->hwnd );


	}
	if( pmsg->message == WM_TIMER &&
		pmsg->hwnd == pThis->m_hwndLastSource )
	{
		CPoint	point;
		::GetCursorPos( &point );

		HWND hwnd = ::WindowFromPoint( point );
		if( hwnd != pThis->m_hwndLastSource )
		{
			if( pThis->m_pointFFTPos != CPoint( 0, 0 ) )
			{
				pThis->_RedrawAll();
				pThis->m_pointFFTPos = CPoint( 0, 0 );
				pThis->_RedrawAll();
			}
		}

	}

	return S_OK;
}


HRESULT CFFTDrawController::XFftDraw::Init( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CFFTDrawController, FftDraw);

	pThis->m_punkDoc = punkDoc;

	IDocumentSitePtr	ptrDoc( punkDoc);

	TPOS	lPos = 0;
	ptrDoc->GetFirstViewPosition( &lPos );

	while( lPos )
	{
		IUnknown	*punkView = 0;
		ptrDoc->GetNextView( &punkView, &lPos );

		IWindow2Ptr	ptrW( punkView );
		punkView->Release();

		pThis->_AddView( punkView );
	
	}

	return S_OK;
}

HRESULT CFFTDrawController::XFftDraw::DeInit()
{
	METHOD_PROLOGUE_EX(CFFTDrawController, FftDraw);

	IDocumentSitePtr	ptrDoc( pThis->m_punkDoc );

	POSITION	pos = pThis->m_views.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punkView = pThis->m_views.GetNext( pos );

		IWindow2Ptr	ptrW( punkView );
		ptrW->DetachMsgListener( pThis->GetControllingUnknown() );
		ptrW->DetachDrawing( pThis->GetControllingUnknown() );
	}

	pThis->m_views.RemoveAll();

	return S_OK;
}


void CFFTDrawController::_AddView( IUnknown *punkView )
{
	IViewPtr	ptrVw( punkView );
	IUnknown	*punkDoc = 0;
	ptrVw->GetDocument( &punkDoc );
	IUnknownPtr	ptrDoc = punkDoc;
	if( !punkDoc )
		return;

	punkDoc->Release();

	if( GetObjectKey( punkDoc ) != GetObjectKey( m_punkDoc ) )
		return;
	
	if( !CheckInterface( punkView, IID_IImageView ) )
		return;

	m_views.AddTail( punkView );

	IWindow2Ptr	ptrW( punkView );
	ptrW->AttachMsgListener( GetControllingUnknown() );
	ptrW->AttachDrawing( GetControllingUnknown() );

	CWnd	*pwnd = ::GetWindowFromUnknown( ptrW );
	pwnd->Invalidate();
}

void CFFTDrawController::_RemoveView( IUnknown *punkView )
{
	POSITION	pos = m_views.Find( punkView );
	if( !pos )return;

	m_views.RemoveAt( pos );

	IWindow2Ptr	ptrW( punkView );
	ptrW->DetachMsgListener( GetControllingUnknown() );
	ptrW->DetachDrawing( GetControllingUnknown() );

	CWnd	*pwnd = ::GetWindowFromUnknown( ptrW );
	pwnd->Invalidate();
}

void CFFTDrawController::DoDraw( CDC &dc )
{
	if( !m_punkDrawTarget )
		return;

	IUnknown	*punkImage = GetActiveParentObjectFromContext( m_punkDrawTarget, szTypeImage );
	if( !punkImage )return;
	CImageWrp	image( punkImage, false );

	if( image.GetColorConvertor().CompareNoCase( "Gray"  ))return;

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();

	int		r2Old;	
	CPen	*pOldPen;
	CPen	pen( PS_SOLID, 0, ::GetValueColor( GetAppUnknown(), "Colors", "FFTLines", RGB( 255, 255, 255 ) ) );

	r2Old = dc.SetROP2(R2_XORPEN);
	pOldPen = ( CPen* )dc.SelectObject( &pen );

	double	fZoom = ::GetZoom( m_punkDrawTarget );
	CPoint	ptScroll = ::GetScrollPos( m_punkDrawTarget );

	CRect	rect = image.GetRect();
	CPoint	pointCenter = rect.CenterPoint();

	double	dx = 0;
	double	dy = 0;

	if( m_pointFFTPos.x == 0 &&
		m_pointFFTPos.y == 0 )
		
	{
		dc.SelectObject(pOldPen);
		dc.SetROP2(r2Old);

		return;
	}

	if( m_pointFFTPos.x != 0 )
		dx = 1./m_pointFFTPos.x * m_sizeFFT.cx;
	if( m_pointFFTPos.y != 0 )
		dy = 1./m_pointFFTPos.y * m_sizeFFT.cy;

	CPoint	point;
	CPoint	pt1, pt2, pt3, pt4;
	CPoint	point1, point2;
	

	bool	bHorz = ::fabs( dx ) > fabs( dy );
	int	nStep = 0;
	CPoint	pointOffest = pointCenter;

	double	a = dy, b = dx, c;

	while( true )
	{
		if( bHorz )
			pointOffest.x = int( pointCenter.x+dx*nStep );
		else
			pointOffest.y = int( pointCenter.y+dy*nStep );

		c = -a*pointOffest.x - b*pointOffest.y;

		if( a == 0 )
		{
			point1.x = pointOffest.x;
			point1.y = rect.top;
			point2.x = pointOffest.x;
			point2.y = rect.bottom-1;
		}
		else if( b == 0 )
		{
			point1.x = rect.left;
			point1.y = pointOffest.y;
			point2.x = rect.right-1;
			point2.y = pointOffest.y;
		}
		else
		{
			pt1.x = rect.left;
			pt1.y = -(pt1.x*a+c)/b;
			pt2.y = rect.top;
			pt2.x = -(pt2.y*b+c)/a;
			pt3.y = rect.bottom-1;
			pt3.x = -(pt3.y*b+c)/a;
			pt4.x = rect.right-1;
			pt4.y = -(pt4.x*a+c)/b;

			TRACE( "(%d %d)(%d %d)(%d %d)(%d %d)\n", pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y );
			

			if( bHorz )
			{
				if( pt2.x > pt3.x )
				{
					CPoint	pt = pt2;
					pt2 = pt3;
					pt3 = pt;
				}
				if( pt1.x > pt2.x )point1 = pt1;else point1 = pt2;
				if( pt3.x < pt4.x )point2 = pt3;else point2 = pt4;
			}
			else
			{
				if( pt1.y > pt4.y )
				{
					CPoint	pt = pt1;
					pt1 = pt4;
					pt4 = pt;
				}

				if( pt1.y > pt2.y )point1 = pt1;else point1 = pt2;
				if( pt3.y < pt4.y )point2 = pt3;else point2 = pt4;
			}
			TRACE( "(%d %d)(%d %d)\n", point1.x, point1.y, point2.x, point2.y );
		}

		if( !rect.PtInRect( point1 ) && !rect.PtInRect( point2 ) )
			break;

		CPoint	point3( rect.right - point1.x-1, rect.bottom - point1.y-1 ), point4( rect.right - point2.x-1, rect.bottom - point2.y-1 );

		point1.x = _AjustValue( fZoom, point1.x * fZoom - ptScroll.x );
		point1.y = _AjustValue( fZoom, point1.y * fZoom - ptScroll.y );
		point2.x = _AjustValue( fZoom, point2.x * fZoom - ptScroll.x );
		point2.y = _AjustValue( fZoom, point2.y * fZoom - ptScroll.y );

		point3.x = _AjustValue( fZoom, point3.x * fZoom - ptScroll.x );
		point3.y = _AjustValue( fZoom, point3.y * fZoom - ptScroll.y );
		point4.x = _AjustValue( fZoom, point4.x * fZoom - ptScroll.x );
		point4.y = _AjustValue( fZoom, point4.y * fZoom - ptScroll.y );

		dc.MoveTo( point1 );
		dc.LineTo( point2 );

		if( nStep )
		{
			dc.MoveTo( point3 );
			dc.LineTo( point4 );
		}

		nStep++;
	}

	/*if (m_pointFFTPos.y == 0)
	{
		if (m_pointFFTPos.x != 0)
		{
			int n1 = -m_sizeFFT.cx/2;
			int n2 = m_sizeFFT.cx/2;

			for (int i = n1; i <= n2; i++)
			{
				CPoint pt1(i*m_sizeFFT.cx/m_pointFFTPos.x,0);
				CPoint pt2(i*m_sizeFFT.cx/m_pointFFTPos.x,cy);

				pt1.x = _AjustValue( fZoom, pt1.x * fZoom - ptScroll.x );
				pt1.y = _AjustValue( fZoom, pt1.y * fZoom - ptScroll.y );
				pt2.x = _AjustValue( fZoom, pt2.x * fZoom - ptScroll.x );
				pt2.y = _AjustValue( fZoom, pt2.y * fZoom - ptScroll.y );

				dc.MoveTo( pt1 );
				dc.LineTo( pt2 );
			}
		}
	}
	else
	{
		if( m_pointFFTPos.y != 0 )
		{
			int n1 = -m_sizeFFT.cy/2;
			int n2 = m_sizeFFT.cy/2;

			for (int i = n1; i <= n2; i++)
			{
				CPoint pt1(0,i*m_sizeFFT.cy/m_pointFFTPos.y);
				CPoint pt2(cx,-m_pointFFTPos.x*cx/m_pointFFTPos.y+i*m_sizeFFT.cy/m_pointFFTPos.y);

				pt1.x = _AjustValue( fZoom, pt1.x * fZoom - ptScroll.x );
				pt1.y = _AjustValue( fZoom, pt1.y * fZoom - ptScroll.y );
				pt2.x = _AjustValue( fZoom, pt2.x * fZoom - ptScroll.x );
				pt2.y = _AjustValue( fZoom, pt2.y * fZoom - ptScroll.y );

				dc.MoveTo( pt1 );
				dc.LineTo( pt2 );
			}
		}
	}*/
	dc.SelectObject(pOldPen);
	dc.SetROP2(r2Old);

	dc.SetBoundsRect( 0, DCB_RESET );
}

void CFFTDrawController::_RedrawAll()
{
	POSITION	pos = m_views.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punkView = m_views.GetNext( pos );
		m_punkDrawTarget = punkView;
		CWnd	*pwnd = ::GetWindowFromUnknown( punkView );

		CDC	*pdc = pwnd->GetDC();
		_Draw( pdc );
		pwnd->ReleaseDC( pdc );
	}
}

IUnknown*	CFFTDrawController::_GetViewFromUnknown( HWND hWnd )
{
	POSITION	pos = m_views.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punkView = m_views.GetNext( pos );
		CWnd	*pwnd = ::GetWindowFromUnknown( punkView );

		if( pwnd->GetSafeHwnd() == hWnd )
			return punkView;
	}
	return 0;
}

bool CFFTDrawController::DoMouseMove( CPoint pt, HWND hwnd )
{
	IUnknown	*punkView = _GetViewFromUnknown( hwnd );
	if( !punkView )return false;
	pt = ::ConvertToClient( punkView, pt );
	IUnknown	*punkImage = GetActiveParentObjectFromContext( punkView, szTypeImage );
	if( !punkImage )return false;
	CImageWrp	image( punkImage, false );

	m_hwndLastSource = hwnd;


	CPoint	point = CPoint( 0, 0 );
	CSize	size = CSize( 0, 0 );

	if( image.GetColorConvertor() == "Complex" )
	{
		CRect	rect = image.GetRect();

		if( rect.PtInRect( pt ) )
		{
			pt -= rect.CenterPoint();
			point = pt;
			size = rect.Size();
		}
	}


	if( point == m_pointFFTPos &&
		size == m_sizeFFT )
		return false;

	_RedrawAll();

	m_sizeFFT = size;
	m_pointFFTPos = point;


	_RedrawAll();


	return true;
}


void CFFTDrawController::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{				
	if( !strcmp( pszEvent, szAppCreateView ) )
	{
		_AddView( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		_RemoveView( punkFrom );
	}

}
