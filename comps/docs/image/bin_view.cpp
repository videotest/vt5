#include "stdafx.h"
#include "bin_view.h"
#include "image_app.h"
#include "data5.h"
#include "math.h"
#include "misc_templ.h"
#include "nameconsts.h"


void DrawBinaryRect( IBinaryImage *pbin, BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, RECT rectDest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwBackColor )
{
	if( !pbi || pbin==0 )return;

	int		cxImg, cyImg;
	pbin->GetSizes( &cxImg, &cyImg );

	POINT	pointImageOffest;
	pointImageOffest.x = pointImageOffest.y = 0;

	
//	if( dwFlags & cidrNoOffset )
//		pointImageOffest = CPoint( 0, 0 );

	int		x, y, xImg, yImg, yBmp;
	byte	fillR = GetRValue(dwFillColor);
	byte	fillG = GetGValue(dwFillColor);
	byte	fillB = GetBValue(dwFillColor);
	byte	backR = GetRValue(dwBackColor);
	byte	backG = GetGValue(dwBackColor);
	byte	backB = GetBValue(dwBackColor);
	byte	*p, *pmask;
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;
	int	nZoom = int( fZoom*256 );
	int	xBmpStart = max( int(ceil( pointImageOffest.x*nZoom/256.-ptScroll.x)), rectDest.left );
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*nZoom/256.-ptScroll.x)), rectDest.right );
	int	yBmpStart = max( int(ceil( pointImageOffest.y*nZoom/256.-ptScroll.y)), rectDest.top );
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*nZoom/256.-ptScroll.y)), rectDest.bottom );
	int	yImgOld = -1;
																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		/*get the pointer to the BGR struct*/																
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;
		/*get the y coord of source image*/																	
		yImg = ((y+ptScroll.y)<<8)/nZoom;
		/*offset coordinates to image*/																		
		yImg-=pointImageOffest.y;
		if( yImgOld != yImg )
		{
			pbin->GetRow(&pmask, yImg, FALSE);
			
			yImgOld = yImg;
		}
		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			/*get the x coord of source image*/
			xImg = ((x+ptScroll.x)<<8)/nZoom;
			/*offset coordinates to image*/
			xImg-=pointImageOffest.x;
			if( (pmask[xImg/8] & (0x80>>(xImg%8)))==0 )
			{
				*p = backB; p++;
				*p = backG; p++;
				*p = backR; p++;
			}
			else
			{
				*p = fillB; p++;
				*p = fillG; p++;
				*p = fillR; p++;
			}
		}
	}
}

inline int Width( const RECT &rect )
{	return rect.right-rect.left;}
inline int Height( const RECT &rect )
{	return rect.bottom-rect.top;}


LRESULT CBinaryView::OnPaint()
{
	PAINTSTRUCT	ps;

	RECT	rectPaint;
	::GetUpdateRect( m_hwnd, &rectPaint, false );
	int	cx = Width( rectPaint );
	int	cy = Height( rectPaint );
	if(  cx == 0 || cy == 0  )return 0;

	HDC hdcPaint = ::BeginPaint( m_hwnd, &ps );

	RECT	rect;
	_brush brush( ::CreateSolidBrush( ::GetSysColor( COLOR_3DFACE ) ) );
	
	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof(bmih ) );
	bmih.biSize = sizeof( bmih );
	bmih.biWidth = cx;
	bmih.biHeight = cy;
	bmih.biBitCount = 24;
	bmih.biPlanes = 1;

	_ptr_t2<byte>	pdibBits( (cx*3+3)/4*4*cy );
	pdibBits.zero();

	POINT	pointBmpOffset;
	pointBmpOffset.x = rectPaint.left;
	pointBmpOffset.y = rectPaint.top;

	if( pdibBits )
	{
		IScrollZoomSitePtr	ptrSite( Unknown() );

		POINT	pointScroll;
		double	fZoom = 1;
		
		ptrSite->GetZoom( &fZoom );
		ptrSite->GetScrollPos( &pointScroll );

		DrawBinaryRect( m_ptrBinary, &bmih, pdibBits, pointBmpOffset, rectPaint, fZoom, pointScroll, RGB( 255, 0, 0 ), RGB( 0, 0, 255 ) );

		::SetDIBitsToDevice( hdcPaint, rectPaint.left, rectPaint.top, Width( rectPaint ), Height( rectPaint ),
			0, 0, 0, Height( rectPaint ), pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );


	}
	else
	{
		::GetClientRect( m_hwnd, &rect );
		::FillRect( hdcPaint, &rect, brush );
	}

	::EndPaint( m_hwnd, &ps );

	return 1;
}

LRESULT CBinaryView::OnLButtonDown( WPARAM, LPARAM )
{
	MessageBox( m_hwnd, "wqwwqwd", "bin", MB_OK );
	return 1;
}

LRESULT	CBinaryView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	switch( nMsg )
	{
		case WM_PAINT:			return OnPaint();
		case WM_LBUTTONDOWN:	return OnLButtonDown( wParam, lParam );
		case WM_ERASEBKGND:		return OnEraseBackground( (HDC)wParam );
	}
	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

void CBinaryView::DoAttachObjects()
{
	m_listObjects.deinit();

	IUnknown	*punkBinary = 0;
	_bstr_t		bstrBinary( szTypeBinaryImage );
	IDataContext3Ptr	ptrC( Unknown() );
	ptrC->GetActiveObject( bstrBinary, &punkBinary );

	m_ptrBinary = punkBinary;

	if( punkBinary )m_listObjects.insert( punkBinary );

	InvalidateRect( m_hwnd, 0, 0 );


}

HRESULT CBinaryView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	CWinViewBase::GetObjectFlags( bstrObjectType, pdwMatch );

	_bstr_t	bstr( bstrObjectType );
	if( !strcmp( bstr, szTypeBinaryImage ) )
		*pdwMatch = mvFull;

	return S_OK;
}

LRESULT CBinaryView::OnEraseBackground( HDC hDC )
{
	SIZE	size;

	IScrollZoomSitePtr	ptrSite( Unknown() );
	ptrSite->GetClientSize( &size );
	double	fZoom;
	POINT	pointScroll;

	ptrSite->GetScrollPos( &pointScroll );
	ptrSite->GetZoom( &fZoom );

	size.cx = int( size.cx*fZoom-pointScroll.x +.5 );
	size.cy = int( size.cy*fZoom-pointScroll.y +.5 );

	RECT	rectClient;
	GetClientRect( m_hwnd, &rectClient );

	RECT	rect1, rect2;
	rect1.left = size.cx;
	rect1.right = rectClient.right;
	rect1.top = 0;
	rect1.bottom = rectClient.bottom;

	rect2.left = 0;
	rect2.right = rectClient.right;
	rect2.top = size.cy;
	rect2.bottom = rectClient.bottom;

	//::FillRect( hdcPaint, &rect, brush );
	//::FillRect

	return 1;
}