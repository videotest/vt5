#include "stdafx.h"

#include "AviObj.h"
#include "AviBase.h"

#include "misc_utils.h"

#include "resource.h"


//////////////////////////////////////////////////////////////////////
bool CAviObj::_CreateSrcImage( BYTE* bHeaderBuf, long lHeaderBufSize, BYTE* pbyteDIBBits, long lDibBitsBufSize )
{

	BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)bHeaderBuf;
	if( !pbi )
		return false;
	
	//pbi->biCompression = BI_RGB;

	if( m_ptrCC == 0 )//not init yet
		_InitImage( pbi->biWidth, pbi->biHeight );

	int cx = m_nImageWidth;
	int cy = m_nImageHeight;

	if( !m_pcolors || !m_pcolors_src || 
		!m_ppRows || !m_ppRowMasks ||  
		!m_pbytes || m_ptrCC == 0 
		|| cx < 1 || cy < 1 )
		return false;

	int	cx4;
	
	long	nBitCount = pbi->biBitCount;
//calc line length
	if( nBitCount == 1 )
		cx4 = ((cx+7)/8+3)/4*4;
	else if( nBitCount == 2 )
		cx4 = ((cx+3)/4+3)/4*4;
	else if( nBitCount == 4 )
		cx4 = ((cx+1)/2+3)/4*4;
	else if( nBitCount == 8 )
		cx4 = (cx+3)/4*4;
	else if( nBitCount == 15 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 16 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else if( nBitCount == 32 )
		cx4 = cx*4;
	else
		return false;

	
	long	nPalSize = 0;
	if( nBitCount <= 8 )
	{
		if( pbi->biClrUsed != 0 )
			nPalSize = pbi->biClrUsed * 4;
		else
			nPalSize = 1<<nBitCount;


	}
	//(nBitCount <= 8) ? 4 * ( lBufSize - sizeof(BITMAPINFOHEADER) )/*1<<nBitCount*/ : 0;
	
	byte	*pbytePalette = nPalSize?((byte*)pbi)+pbi->biSize:0;
	byte	*pbyteDIBData = ((byte*)pbi)+pbi->biSize+nPalSize*4;
	

	if( pbyteDIBBits )
		pbyteDIBData = pbyteDIBBits;

	byte	*pbyteRed = 0;
	byte	*pbyteGreen = 0;
	byte	*pbyteBlue = 0;
//is color image
	bool	bColor = true;
//if image has palette, check it is gray scale
	if( nPalSize > 0 )
	{
		assert( pbytePalette != 0 );
		bool	bGrayScale = true;

		pbyteRed = new byte[nPalSize];
		pbyteGreen = new byte[nPalSize];
		pbyteBlue = new byte[nPalSize];

		for( long nEntry = 0; nEntry < nPalSize/4; nEntry++ )
		{
			long	nPalOffset = nEntry*4;
			
			if( pbytePalette[nPalOffset] != pbytePalette[nPalOffset+1] ||
				pbytePalette[nPalOffset] != pbytePalette[nPalOffset+2] )
				bGrayScale = false;
			pbyteRed[nEntry] = pbytePalette[nPalOffset];
			pbyteGreen[nEntry] = pbytePalette[nPalOffset+1];
			pbyteBlue[nEntry] = pbytePalette[nPalOffset+2];			
			
		}

		if( bGrayScale )
			bColor = false;
	}

//alloc the buffer

	long	nColorsCount = m_iPanesCount;
	//color	**ppcolorBuf = new color*[nColorsCount];

	long	nLineLength = (cx*3+3)/4*4;//;bColor?((cx*3+3)/4*4):((cx+3)/4*4);
	BYTE*	pbuf = new byte[nLineLength];
	if( !pbuf )
		return false;
	::ZeroMemory( pbuf, sizeof(nLineLength) );

	long	nColorCol, nCol, nrow_bmp;
//convert image rows
	byte	*pRGB = 0;

	//nDoubleBufStage
	//m_bEvenThenOdd

	int nLineToDup = m_bEvenThenOdd ? 1-m_nDoubleBufStage : m_nDoubleBufStage;

	int nimg_y = 0;

	for( int y = 0; y < cy; y ++ )
	{
		int nrow_copy_from = y;
		if( m_bEnableDoubleBuffering )
		{
			nrow_copy_from = ( (nrow_copy_from - nLineToDup) & 0xFFFFFFFE ) + nLineToDup;
			if( nrow_copy_from < 0 )	nrow_copy_from += 2;
		}

		nrow_bmp = cy-nrow_copy_from-1;

		byte	*pRaw = pbyteDIBData+nrow_bmp*cx4;

		//convert row from raw to RGB DIB
		switch( nBitCount )
		{
		case 1:		//1 bit per pixel image
			pRGB = pbuf;

			assert( pbyteRed != 0 );
			assert( pbyteGreen != 0 );
			assert( pbyteBlue != 0 );

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 8;
				long	nOffset = nCol % 8;
				long	nOffset1 = 7-nOffset;

				byte	byteVal = (pRaw[nByte] & (128>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 2:		//2 bit per pixel image
			pRGB = pbuf;

			assert( pbyteRed != 0 );
			assert( pbyteGreen != 0 );
			assert( pbyteBlue != 0 );

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 4; 
				long	nOffset = nCol % 4;
				long	nOffset1 = 4-nOffset;

				byte	byteVal = (pRaw[nByte] & (192>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 4:		//4 bit per pixel image
			pRGB = pbuf;

			assert( pbyteRed != 0 );
			assert( pbyteGreen != 0 );
			assert( pbyteBlue != 0 );

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 2; 
				long	nOffset = 4*(nCol % 2);
				long	nOffset1 = 4-nOffset;

				byte	byteVal = (pRaw[nByte] & (240>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 8:		//8 bit per pixel image
			//if( bColor )
			{
				pRGB = pbuf;

				assert( pbyteRed != 0 );
				assert( pbyteGreen != 0 );
				assert( pbyteBlue != 0 );

				for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
				{
					byte	byteVal = pRaw[nCol];

					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
			}
			/*else
			{
				pRGB = pRaw;
			}
			*/

			break;
		case 16:			
			assert( bColor );
			pRGB = pbuf;

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				/*
				When the biCompression member is BI_BITFIELDS, 
				the system supports only the following 16bpp color masks: 
				A 5-5-5 16-bit image, 
				where the blue mask is 0x001F, the green mask is 0x03E0, and the red mask is 0x7C00; 
				and a 5-6-5 16-bit image, 
				where the blue mask is 0x001F, the green mask is 0x07E0, and the red mask is 0xF800.
				*/

				/*
				WORD	wVal = ((WORD*)pRaw)[nCol];

				byte	r = (wVal & 0xF800)>>6;
				byte	g = (wVal & 0x0760)>>3;
				byte	b = (wVal & 0x001F)<<3;

				pRGB[nColorCol+2] = b;
				pRGB[nColorCol+1] = g;
				pRGB[nColorCol+0] = r;
				*/
				//Paul change format 3.02.2003
				WORD wVal = ((WORD*)pRaw)[nCol];
				byte	r = ( BYTE ) ( ( ( wVal >> 10 ) & 0x1f ) << 3 );
				byte	g = ( BYTE ) ( ( ( wVal >> 5 ) & 0x1f ) << 3 );
				byte	b = ( BYTE ) ( ( wVal & 0x1f ) << 3 );

				pRGB[nColorCol+0] = b;
				pRGB[nColorCol+1] = g;
				pRGB[nColorCol+2] = r;
			}
			
			break;			

		case 24:		//24 bit per pixel image
			assert(bColor);
			pRGB = pRaw;
			break;


		case 32:
			assert( bColor );
			pRGB = pbuf;

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				WORD wVal = ((WORD*)pRaw)[nCol];
				byte	r = ( BYTE ) ( ( ( wVal >> 10 ) & 0x1f ) << 3 );
				byte	g = ( BYTE ) ( ( ( wVal >> 5 ) & 0x1f ) << 3 );
				byte	b = ( BYTE ) ( ( wVal & 0x1f ) << 3 );

				pRGB[nColorCol+0] = pRaw[nCol*4+0];
				pRGB[nColorCol+1] = pRaw[nCol*4+1];
				pRGB[nColorCol+2] = pRaw[nCol*4+2];
			}
			
			break;			
		}
		
		nimg_y = y;

		if( nimg_y >= 0 && nimg_y < cy )
			copy_from_dib_to_img( pRGB, cx, nimg_y );

		/*
			for( int nColor=0;nColor<m_iPanesCount;nColor++ )
				GetRow( cy-nRow-1, nColor, &(ppcolorBuf[nColor])  );						

			m_ptrCC->ConvertDIBToImage( pRGB, ppcolorBuf, cx, bColor );		
		*/
	}

	delete 	pbuf;	pbuf = 0;

	//if( ppcolorBuf )delete ppcolorBuf;
	if( pbyteRed )delete pbyteRed;
	if( pbyteGreen )delete pbyteGreen;
	if( pbyteBlue )delete pbyteBlue;

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAviObj::copy_from_dib_to_img( BYTE* prgb_color, int cx, int nimg_row )
{
	color* pcolor = m_pcolors + cx * nimg_row * m_iPanesCount;
	for( int nCol = 0; nCol < cx; nCol++ )
	{			
		color* p = pcolor + nCol;

		p[cx+cx] = ( (*(prgb_color++)) * 256 );
		p[cx] = ( (*(prgb_color++)) * 256 );
		(*p) = ( (*(prgb_color++)) * 256 );// = r;			
		p += cx*3;
	}
}


//////////////////////////////////////////////////////////////////////
bool CAviObj::_InitImage( int cx, int cy )
{
	_DestroyImage();

	m_ptrCC = GetCCByName( m_bstrCCName );

	if( m_ptrCC == 0 )
		return false;

	int nPaneCount = 0;
	m_ptrCC->GetColorPanesDefCount( &nPaneCount );
	if( nPaneCount < 1 )
		return false;

	m_iPanesCount = nPaneCount;	

	m_nImageWidth	= cx;
	m_nImageHeight	= cy;
	
	m_pcolors = new color[cx*cy*nPaneCount];
	if( !m_pcolors )
		return false;

	m_pcolors_src = new color[cx*cy*nPaneCount];
	if( !m_pcolors_src )
		return false;

	m_ppRows = new color*[cy*nPaneCount];
	if( !m_ppRows )
		return false;

	for( int i = 0; i < cy*nPaneCount; i++ )
		m_ppRows[i] = m_pcolors+cx*i;

	m_ppRowMasks = new byte*[ cy ];
	if( !m_ppRowMasks )
		return false;
	
	m_pbytes = new byte[cx*cy];
	if( !m_pbytes )
		return false;

	memset( m_pbytes, 255, cx*cy );

	for( i = 0; i < cy; i++ )
		m_ppRowMasks[i] = m_pbytes+cx*i;


	return true;
}

//////////////////////////////////////////////////////////////////////
void CAviObj::_DestroyImage()
{
	if( m_ppRows )
		delete [] m_ppRows;	m_ppRows = 0;

	if( m_ppRowMasks )
		delete [] m_ppRowMasks;	m_ppRowMasks = 0;
	
	if( m_pcolors )
		delete [] m_pcolors;	m_pcolors = 0;
	
	if( m_pcolors_src )
		delete []m_pcolors_src;	m_pcolors_src = 0;
	
	if( m_pbytes )
		delete [] m_pbytes;		m_pbytes = 0;


	m_nImageWidth = m_nImageHeight = 0;	

	m_ptrCC = 0;

	m_iPanesCount = 0;
}

//IImage
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::CreateImage( int cx, int cy, BSTR bstrCCNamed, int nPaneNum )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::FreeImage()
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetColorConvertor( IUnknown **ppunk )
{
	if( m_ptrCC )
	{
		m_ptrCC->AddRef();
		*ppunk = m_ptrCC;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetRow( int nRow, int nPane, color **ppcolor )
{
	*ppcolor = 0;

	if( !m_ppRows )
		return S_FALSE;

	nPane = max(0, min(nPane, m_iPanesCount-1));
	*ppcolor = m_ppRows[ nRow*m_iPanesCount+nPane ];

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetSize( int *pnCX, int *pnCY )
{
	*pnCX = m_nImageWidth;
	*pnCY = m_nImageHeight;

	return S_OK;
}

//IImage2
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::CreateVirtual( RECT rect )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::InitRowMasks()
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetRowMask( int nRow, BYTE **ppByte )
{
	*ppByte = 0;

	if( m_ppRowMasks )
	{
		*ppByte = m_ppRowMasks[nRow];
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetOffset( POINT *pt )
{
	pt->x = pt->y = 0;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::SetOffset( POINT pt, BOOL bMoveImage )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetImageFlags( DWORD *pdwFlags )
{
	*pdwFlags = 0;
	return S_OK;
}

//IImage3
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::InitContours()
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::FreeContours()
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetContoursCount( int *piCount )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetContour( int nPos, Contour **ppContour )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::SetContour( int nPos, Contour *pContour )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::AddContour( Contour *pContour )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::AddPane(int nPane )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::RemovePane( int nPane )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetPanesCount(int *nSz  )
{
	if( !nSz )	return E_POINTER;

	if( !m_pcolors )	return S_FALSE;

	*nSz = 3;

	return S_OK;
}

