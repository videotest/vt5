// SimpleDib.cpp : implementation file
//

#include "stdafx.h"
#include "dib.h"
#include "dbasedoc.h"

#include "thumbnail.h"
#include "StreamEx.h"


/////////////////////////////////////////////////////////////////////////////
// CDibWrapper

CDibWrapper::CDibWrapper()
{
	m_pDIB = NULL;	
}

/////////////////////////////////////////////////////////////////////////////
CDibWrapper::~CDibWrapper()
{
	DeInit();
}

/////////////////////////////////////////////////////////////////////////////
void CDibWrapper::DeInit()
{
	if( m_pDIB )
	{
		::ThumbnailFree( m_pDIB );
		m_pDIB = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::IsWasCreated()
{
	return ( m_pDIB != NULL ? true : false );
}


/////////////////////////////////////////////////////////////////////////////
CSize CDibWrapper::GetDIBSize()
{
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)m_pDIB;

	if( m_pDIB == NULL )
		return CSize(0,0);

	return CSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight );
}


long nVer = 1;

//2 times serialize version cos export from common.dll

/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::Store( IStream* pStream )
{
	CStreamEx ar( pStream, false );
	ar << nVer;

	DWORD dwSize = ::ThumbnailGetSize( m_pDIB );

	ar << nVer;
	ar << dwSize;
	
	if( dwSize < sizeof(BITMAPINFOHEADER) + ((BYTE)sizeof(TumbnailInfo)) )
		return false;

	ar.Write( m_pDIB, dwSize );

	return true;

}

bool CDibWrapper::Load( IStream* pStream )
{
	CStreamEx ar( pStream, true );
	DeInit();
	ar >> nVer;

	DWORD dwSize = -1;
	
	long nVersion;

	ar >> nVersion;
	ar >> dwSize;
	if( dwSize < 1 )
		return true;//no data - empty

	m_pDIB = new BYTE[dwSize];
	ar.Read( m_pDIB, dwSize );
	
	return true;

}
/*
/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::Serialize( CArchive& ar )
{
	try
	{
		long nVer = 1;

		if( ar.IsLoading() )
		{
			DeInit();
			ar >> nVer;
			::ThumbnailLoad( ar, &m_pDIB );
		}
		else
		{
			ar << nVer;
			::ThumbnailStore( ar, m_pDIB );
			
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}
*/

/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::Draw( CDC* pDC, CRect rcTarget, bool bDrawBorderAroundDib )
{

	if( m_pDIB == NULL ) 
		return false;

	CRect rcDibOutput;


	CSize sizeOutput = ::ThumbnailGetRatioSize( 
							CSize( rcTarget.Width(), rcTarget.Height() ), 
							GetDIBSize()
									);

	rcDibOutput.left	= rcTarget.left + rcTarget.Width()  / 2 - sizeOutput.cx / 2;
	rcDibOutput.top		= rcTarget.top  + rcTarget.Height() / 2 - sizeOutput.cy / 2;

	rcDibOutput.right	= rcDibOutput.left + sizeOutput.cx;
	rcDibOutput.bottom	= rcDibOutput.top  + sizeOutput.cy;

	if( bDrawBorderAroundDib )
	{
		CRect rcBorderAroundPicture = rcDibOutput;
		rcBorderAroundPicture.InflateRect( 1, 1, 1, 1 );
		pDC->DrawEdge( &rcBorderAroundPicture, EDGE_ETCHED, BF_RECT );
	}


	::ThumbnailDraw( m_pDIB, pDC, rcTarget, 0 );


  
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::CreateFromDataObject( IUnknown* punkDataObject, CSize sizeDIB )
{

	DeInit();

	if( punkDataObject == NULL )
		return false;

	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return false;

	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( punkDataObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return false;

	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return false;


	short nSupported;
	spRenderObject->GenerateThumbnail( punkDataObject, 24, 0, 0, 
				sizeDIB, 
				&nSupported, &m_pDIB );

	if( !m_pDIB )
		return false;

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CDibWrapper::NeedReCreate( IUnknown* punkDataObject, CSize sizeDIB )
{

	if( punkDataObject == NULL ) 
	{
		if( m_pDIB == NULL )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if( m_pDIB == NULL )
		{
			return true;
		}
		else
		{
			BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)m_pDIB;
			if( pBitmapInfoHeader->biWidth  == sizeDIB.cx &&
				pBitmapInfoHeader->biHeight == sizeDIB.cy
				)
			{
				//Need verify for context in additional
				return false;
			}
			else
			{
				return true;
			}
		}
	}


	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CSimpleDib


/*
CSimpleDib::CSimpleDib()
{
	m_pDIB = NULL;
	m_nDibSize = 0;
	m_bWasCreated = false;

}

CSize CSimpleDib::GetDIBSize()
{
	return CSize(
		m_BitmapInfo.bmiHeader.biWidth,
		m_BitmapInfo.bmiHeader.biHeight
		);	
}


CSimpleDib::~CSimpleDib()
{
	Free();
}

void CSimpleDib::Free()
{
	if( m_pDIB )
	{
		delete[] m_pDIB;		
		m_pDIB = NULL;
	}

	ZeroMemory( &m_BitmapInfo, sizeof( m_BitmapInfo ) );	

	m_nDibSize = 0;
}

DWORD CSimpleDib::GetSerializeLength()
{
	return (
		sizeof(long) +
		sizeof(m_BitmapInfo) +
		sizeof(m_nDibSize) + 
		m_nDibSize + 
		sizeof(BOOL)
		);
}

bool CSimpleDib::Serialize( CArchive& ar )
{
	try
	{
		long nVer = 2;

		if( ar.IsLoading() )
		{
			Free();
			ar >> nVer;
			ar.Read( &m_BitmapInfo, sizeof(m_BitmapInfo) );
			ar.Read( &m_nDibSize, sizeof(m_nDibSize) );
			
			m_pDIB = new BYTE[m_nDibSize];
			ar.Read( m_pDIB, m_nDibSize );	
			if( nVer >= 2)
			{
				BOOL bWasCreated;
				ar >> bWasCreated;
				m_bWasCreated = ( bWasCreated == TRUE );
			}

		}
		else
		{
			ar << nVer;
			ar.Write( &m_BitmapInfo, sizeof(m_BitmapInfo) );
			ar.Write( &m_nDibSize, sizeof(m_nDibSize) );
			ar.Write( m_pDIB, m_nDibSize );	
			BOOL bWasCreated = ( m_bWasCreated == true );
			ar<<bWasCreated;
		}
	}
	catch(...)
	{
		return false;
	}

	return true;

}


bool CSimpleDib::Draw( CDC* pDC, CRect rcOutput, bool bDrawBorderAroundDib )
{

	if( m_pDIB == NULL ) 
		return false;

	CRect rcDibOutput;


	CSize sizeOutput = ::GetRatioSize( 
							CSize( rcOutput.Width(), rcOutput.Height() ), 
							CSize( m_BitmapInfo.bmiHeader.biWidth, 
								   m_BitmapInfo.bmiHeader.biHeight)
									);

	rcDibOutput.left	= rcOutput.left + rcOutput.Width()  / 2 - sizeOutput.cx / 2;
	rcDibOutput.top		= rcOutput.top  + rcOutput.Height() / 2 - sizeOutput.cy / 2;

	rcDibOutput.right	= rcDibOutput.left + sizeOutput.cx;
	rcDibOutput.bottom	= rcDibOutput.top  + sizeOutput.cy;

	if( bDrawBorderAroundDib )
	{
		CRect rcBorderAroundPicture = rcDibOutput;
		rcBorderAroundPicture.InflateRect( 1, 1, 1, 1 );
		pDC->DrawEdge( &rcBorderAroundPicture, EDGE_ETCHED, BF_RECT );
	}


	 ::StretchDIBits(pDC->m_hDC,            // hDC
							   rcDibOutput.left,               // DestX
							   rcDibOutput.top,                // DestY
							   rcDibOutput.Width(),          // nDestWidth
							   rcDibOutput.Height(),         // nDestHeight
							   0,              // SrcX
							   0,               // SrcY
							   m_BitmapInfo.bmiHeader.biWidth,         // wSrcWidth
							   m_BitmapInfo.bmiHeader.biHeight,        // wSrcHeight
							   m_pDIB,                      // lpBits
							   &m_BitmapInfo,                       // lpBitsInfo
							   DIB_RGB_COLORS,               // wUsage
							   SRCCOPY);                     // dwROP
			


  
	return true;
}



bool CSimpleDib::CreateFromDC( CDC* pDC, CRect rcSource, int nDibWidth, int nDibHeight )
{
	Free();

	CDC			dcMemory;
	dcMemory.CreateCompatibleDC( pDC );
//init the DIB hearer		

	m_BitmapInfo.bmiHeader.biBitCount = 16;
	m_BitmapInfo.bmiHeader.biHeight = nDibHeight;
	m_BitmapInfo.bmiHeader.biWidth = nDibWidth;
	m_BitmapInfo.bmiHeader.biSize = sizeof( m_BitmapInfo.bmiHeader );
	m_BitmapInfo.bmiHeader.biPlanes = 1;

	
	if( m_BitmapInfo.bmiHeader.biHeight == 0 ||m_BitmapInfo.bmiHeader.biWidth == 0 )
		return false;

	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( pDC->m_hDC, (BITMAPINFO*)&m_BitmapInfo.bmiHeader, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );
	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );


	VERIFY( dcMemory.StretchBlt(0, 0, nDibWidth, nDibHeight, pDC, 
		rcSource.left, rcSource.top,
		rcSource.Width(), rcSource.Height(),
		SRCCOPY) ); 

	//dcMemory.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY); 
					 
	m_nDibSize = nDibHeight*nDibWidth*2;
	m_pDIB = new BYTE[m_nDibSize];
	memcpy( m_pDIB, pdibBits, m_nDibSize );

	dcMemory.SelectObject( poldBmp );	

	::DeleteObject(hDIBSection);

	m_bWasCreated = true;

	return true;
}

*/


