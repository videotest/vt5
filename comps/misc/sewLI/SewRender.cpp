#include "stdafx.h"
#include "SewRender.h"

#include "resource.h"
//#include "constant.h"
#include "misc_utils.h"
#include "atltypes.h"
#include "iSewLI.h"
#include <math.h>

//#include "oledata.h"


SIZE GetRenderSize( SIZE sizeThumb, SIZE sizeImage );

//////////////////////////////////////////////////////////////////////
CSewRender::CSewRender()
{

}

//////////////////////////////////////////////////////////////////////
CSewRender::~CSewRender()
{

}


//////////////////////////////////////////////////////////////////////
IUnknown* CSewRender::DoGetInterface( const IID &iid )
{
	if( iid == IID_IRenderObject ) return (IRenderObject*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewRender::Support( IUnknown* punkDataObject, short* pnRenderSupport )
{

	*pnRenderSupport = ::CheckInterface(punkDataObject,IID_ISewImageList)?rsFull:rsNone;
	return S_OK;
}



HRESULT CSewRender::GenerateThumbnail( 
		/*[in]*/	IUnknown* punkDataObject,
		/*[in]*/	int	nBitsCount,
		/*[in]*/	DWORD dwFlag,
		/*[in]*/	LPARAM lParam,
		/*[in]*/	SIZE sizeThumbnail, 
		/*[out]*/	short*  pnRenderSupport,
		/*[out]*/	BYTE** ppbi,
		/*[in]*/	IUnknown* punkBag
		)
{
	IThumbnailManagerPtr ptrMan( ::GetAppUnknown() );
	if( ptrMan == 0 )
		return E_FAIL;
	ISewImageListPtr sptrSLI( punkDataObject );
	if( sptrSLI == NULL )
		return E_FAIL;
	*pnRenderSupport = (short)rsFull;
	//Get total image.
	IUnknownPtr punkTImg;
	if (sptrSLI != 0)
		sptrSLI->GetTotalImage(&punkTImg);
	IImage2Ptr imgTotal(punkTImg);
	IUnknownPtr punkCC;
	if (imgTotal != 0)
		imgTotal->GetColorConvertor(&punkCC);
	sptrIColorConvertorEx sptrCCEx(punkCC);
	//Calculate drawing area.
	if( sizeThumbnail.cx < 1 || sizeThumbnail.cy < 1 )
		return S_FALSE;
	CSize sizeRender(32,32);
	double dZoom = 1.;
	if( imgTotal != 0 )
	{
		int cx,cy;
		imgTotal->GetSize(&cx, &cy);
		double dZoomX = double(sizeThumbnail.cx)/double(cx);
		double dZoomY = double(sizeThumbnail.cy)/double(cy);
		dZoom = min(dZoomX,dZoomY);
		sizeRender = CSize((int)floor(cx*dZoom),(int)floor(cy*dZoom));
	}
	//Allocate bitmap for thumbnail
	BITMAPINFOHEADER bmih;
	::ZeroMemory( &bmih, sizeof( bmih )  );
	bmih.biBitCount = 24;
	bmih.biHeight = sizeRender.cy;
	bmih.biWidth = sizeRender.cx;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;
	DWORD dwSize = 0;
	if( S_OK != ptrMan->ThumbnailGetSize( (BYTE*)&bmih, &dwSize ) )
		return E_FAIL;
	if( dwSize <= 0 )
		return E_FAIL;
	if( S_OK != ptrMan->ThumbnailAllocate( ppbi, sizeRender ) )
		return E_FAIL;
	//Prepare DIBSection and DC for drawing.
	byte	*pdibBits = 0;
	HDC hDCScreen = ::CreateDC( "DISPLAY", NULL, NULL, NULL );
	if( !hDCScreen )
	{
		ptrMan->ThumbnailFree( *ppbi );
		*ppbi = NULL;
		return E_FAIL;
	}
	HDC hDCMemory = 0;
	hDCMemory = ::CreateCompatibleDC( hDCScreen );
	::DeleteDC( hDCScreen );	hDCScreen = 0;
	if( !hDCMemory )
	{
		ptrMan->ThumbnailFree( *ppbi );
		*ppbi = NULL;
		return E_FAIL;
	}
	HBITMAP	hDIBSection = ::CreateDIBSection( 
						hDCMemory, 
						(BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	if( !hDIBSection )
	{
		ptrMan->ThumbnailFree( *ppbi );
		*ppbi = NULL;

		::DeleteDC( hDCMemory ); hDCMemory = 0;
		return E_FAIL;
	}
	HBITMAP hBmpOld = (HBITMAP)::SelectObject( hDCMemory, hDIBSection );
	CRect rcTarget;
	::SetRect( &rcTarget, 0, 0, sizeRender.cx, sizeRender.cy );
	TumbnailFlags tf = tfVarStretch;
	DWORD dwIcon = FALSE;
	// Draw
	if (imgTotal == 0 || sptrCCEx == 0)
	{
//		_brush brush(::CreateSolidBrush( RGB(255,255,255)));
//		::FillRect(hDCMemory, &rcTarget, brush);
		if( dwFlag == RF_NODEFINE )
		{			
			_brush brush( ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) ) );
			::FillRect( hDCMemory, &rcTarget, brush );
		}
		else if( dwFlag == RF_BACKGROUND )
		{
			_brush brush( ::CreateSolidBrush( (COLORREF)lParam ) );
			::FillRect( hDCMemory, &rcTarget, brush );
		}
		tf = tfCanStretch2_1;
		dwIcon = TRUE;
		HICON hIcon = ::LoadIcon( App::handle(), MAKEINTRESOURCE(IDI_SEWLI) );			
		if( hIcon == NULL )
			return S_FALSE;
		::DrawIcon( hDCMemory, 0, 0, hIcon );
		::DestroyIcon( hIcon );
	}
	else
	{
		sptrCCEx->ConvertImageToDIBRect(&bmih, pdibBits, rcTarget.TopLeft(),
			imgTotal, rcTarget, CPoint(0,0), dZoom, CPoint(0,0), lParam,
			0, NULL);
	}
//	Draw( hDCMemory, rcTarget, dwFlag, lParam, false, punkDataObject, 
//								tf, dwIcon );
	BYTE* pSource = pdibBits;
	BYTE* pTarget = (*ppbi) + ((BYTE)sizeof(BITMAPINFOHEADER)) +
									((BYTE)sizeof(TumbnailInfo));
	//Copy BITMAPINFOHEADER
	memcpy( *ppbi, &bmih, sizeof(BITMAPINFOHEADER) );
	//Copy TumbnailInfo
	TumbnailInfo tumbnailInfo;
	::ZeroMemory( &tumbnailInfo, sizeof( tumbnailInfo ) );
	tumbnailInfo.dwIcon = dwIcon;
	tumbnailInfo.dwFlags = tf;
	memcpy( (*ppbi+((BYTE)sizeof(BITMAPINFOHEADER))), &tumbnailInfo, sizeof(tumbnailInfo) );
	//Copy DIB bits
	memcpy( pTarget, pSource, dwSize - sizeof(BITMAPINFOHEADER) - ((BYTE)sizeof(TumbnailInfo)) );
	::SelectObject( hDCMemory, hBmpOld );
	::DeleteDC( hDCMemory ); hDCMemory = 0;
	::DeleteObject(hDIBSection);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CSewRender::Draw( HDC hdc, RECT rcTarget, DWORD dwFlag, LPARAM lParam, 
					  bool bDrawIcon, IUnknown* punkDataObject,
						TumbnailFlags& tf, DWORD& dwIcon
					  )
{
	if( dwFlag == RF_NODEFINE )
	{			
		_brush brush( ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) ) );
		::FillRect( hdc, &rcTarget, brush );
	}
	else
	if( dwFlag == RF_BACKGROUND )
	{
		_brush brush( ::CreateSolidBrush( (COLORREF)lParam ) );
		::FillRect( hdc, &rcTarget, brush );
	}
	ISewImageListPtr sptrSLI( punkDataObject );
	IUnknownPtr punkTImg;
	if (sptrSLI != 0)
		sptrSLI->GetTotalImage(&punkTImg);
	IImagePtr imgTotal(punkTImg);
	if( bDrawIcon || imgTotal == 0 )
	{
		tf = tfCanStretch2_1;
		dwIcon = TRUE;
		HICON hIcon = ::LoadIcon( App::handle(), MAKEINTRESOURCE(IDI_SEWLI) );			
		if( hIcon == NULL )
			return;
		::DrawIcon( hdc, 0, 0, hIcon );
		::DestroyIcon( hIcon );
	}
	else
	{
//		_brush brush( ::CreateSolidBrush( RGB(128,255,128) ) );
//		::FillRect( hdc, &rcTarget, brush );

		

	}
}

