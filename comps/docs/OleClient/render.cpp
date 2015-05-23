#include "stdafx.h"
#include "render.h"

#include "resource.h"
#include "constant.h"
#include "misc_utils.h"

#include "oledata.h"


SIZE GetRenderSize( SIZE sizeThumb, SIZE sizeImage );

//////////////////////////////////////////////////////////////////////
COleRender::COleRender()
{

}

//////////////////////////////////////////////////////////////////////
COleRender::~COleRender()
{

}


//////////////////////////////////////////////////////////////////////
IUnknown* COleRender::DoGetInterface( const IID &iid )
{
	if( iid == IID_IRenderObject ) return (IRenderObject*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT COleRender::Support( IUnknown* punkDataObject, short* pnRenderSupport )
{

	*pnRenderSupport = rsNone;
	

	IOleObjectUIPtr ptr( punkDataObject );
	if( ptr == NULL )
		return S_FALSE;

	*pnRenderSupport = (short)rsFull;

	return S_OK;

}



HRESULT COleRender::GenerateThumbnail( 
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

	IOleObjectUIPtr ptr( punkDataObject );
	if( ptr == NULL )
		return E_FAIL;

	*pnRenderSupport = (short)rsFull;

	if( sizeThumbnail.cx < 1 || sizeThumbnail.cy < 1 )
		return S_FALSE;

	BOOL bWasCreated = FALSE;
	ptr->IsObjectCreated( &bWasCreated );

	SIZE sizeRender;

	if( bWasCreated )
	{
		SIZE sizeScroll;
		ptr->GetScrollSize( &sizeScroll, NULL );
		if( sizeScroll.cx <= 0 || sizeScroll.cy <= 0 )
		{
				sizeRender.cx = 32;
				sizeRender.cy = 32;
		}
		else
		{
			sizeRender = GetRenderSize( sizeThumbnail, sizeScroll );
			
			if( sizeRender.cx < 35 && sizeRender.cy < 35 )
			{
				sizeRender.cx = 32;
				sizeRender.cy = 32;
			}
			
		}

	}

	if( !bWasCreated )
	{
		sizeRender.cx = 32;
		sizeRender.cy = 32;
	}

	if( sizeRender.cx <= 0 || sizeRender.cy <= 0 )
		return E_FAIL;
	

	

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

	RECT rcTarget;
	::SetRect( &rcTarget, 0, 0, sizeRender.cx, sizeRender.cy );


	TumbnailFlags tf = tfVarStretch;
	DWORD dwIcon = FALSE;

	Draw( hDCMemory, rcTarget, dwFlag, lParam, bWasCreated == false, punkDataObject, 
								tf, dwIcon );


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
void COleRender::Draw( HDC hdc, RECT rcTarget, DWORD dwFlag, LPARAM lParam, 
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


	if( bDrawIcon )
	{
		tf = tfCanStretch2_1;
		dwIcon = TRUE;


		HICON hIcon = ::LoadIcon( App::handle(), MAKEINTRESOURCE(IDI_OLEITEMIMAGE) );			
		if( hIcon == NULL )
			return;

		::DrawIcon( hdc, 0, 0, hIcon );
		
		::DestroyIcon( hIcon );
	}
	else
	{
		IOleObjectUIPtr ptr( punkDataObject );
		if( ptr == NULL )
			return;

		SIZE sizeScroll;
		ptr->GetScrollSize( &sizeScroll, NULL );

		POINT ptOffset;
		ptOffset.x = 0;
		ptOffset.y = 0;

		int nDrawAspect = DVASPECT_CONTENT;
		if( rcTarget.right < 35 && rcTarget.bottom < 35 )  
			nDrawAspect = DVASPECT_ICON;		

		if( nDrawAspect == DVASPECT_ICON )
		{
			tf = tfCanStretch2_1;
			dwIcon = TRUE;
		}
		else
		{
			tf = tfVarStretch;
			dwIcon = FALSE;
		}
		

		ptr->DrawMeta( nDrawAspect, NULL, hdc, rcTarget, ptOffset, sizeScroll );

		
		

	}
}

/////////////////////////////////////////////////////////////////////////////
SIZE GetRenderSize( SIZE sizeThumb, SIZE sizeImage )
{
	double fThumbWidth		= (double)sizeThumb.cx;
	double fThumbHeight	= (double)sizeThumb.cy;
	double fThumbZoom		= fThumbWidth / fThumbHeight;

	double fImageWidth	= (double)sizeImage.cx;
	double fImageHeight	= (double)sizeImage.cy;	
	double fImageZoom	= fImageWidth / fImageHeight;


	double fZoom = min( fThumbWidth / fImageWidth, fThumbHeight / fImageHeight );

	double fWidth, fHeight;

	fWidth = fThumbWidth;
	fHeight = fThumbHeight;
	
	if( fWidth / fImageZoom > fThumbHeight )
	{		
		fWidth = fThumbHeight * fImageZoom;
		fHeight = fThumbHeight;
	}

	if( fHeight * fImageZoom > fThumbWidth )
	{
		fHeight = fThumbWidth / fImageZoom;
		fWidth = fThumbWidth;
	}

	SIZE size;
	size.cx = int( fWidth );
	size.cy = int(fHeight );
	
	return size;
}
