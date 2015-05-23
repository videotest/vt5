#include "stdafx.h"
#include "contourapi.h"

#include "imagebase.h"
#include "math.h"
#include "binary.h"
#include "core5.h"
#include "docview5.h"
#include "nameconsts.h"
#include "ImagePane.h"




int nInfo = 0;
int nColors = 0;


const int BodyMask = 1<<7;

FillInfo* FillInfo::Create()
{
	FillInfo *pInfo = new FillInfo;
	//ZeroMemory( pInfo, sizeof( pInfo ) );
	pInfo->nX		= 0;
	pInfo->nCount	= 0;
	pInfo->pColors	= 0;
	pInfo->bFill	= 0;
	pInfo->pNext	= 0;
	return pInfo;
}

void FillInfo::CreateColors(int nSize)
{
	if( !nSize )
		return;
	ASSERT( nSize > 0 );
	ASSERT( !pColors );

	pColors = new color[nSize];
}


void FillInfo::Destroy()
{
	KillColors();

	delete this;
}

void FillInfo::KillColors()
{
	if(pColors)
	{
		delete pColors;
	}

	pColors = 0;
}

//construct WalkFillInfo, copy image mask to the info
CWalkFillInfo::CWalkFillInfo( IUnknown *punkImage )
{
	m_pbmp_old = 0;

	CImageWrp	image( punkImage );

	CPoint	ptOffset = image.GetOffset();

	m_rect.left = ptOffset.x;
	m_rect.top = ptOffset.y;
	m_rect.right = ptOffset.x+image.GetWidth();
	m_rect.bottom = ptOffset.y+image.GetHeight();

	InitDIBSection();

	int	cx = m_rect.Width();
	int	cy = m_rect.Height();
//"expand" image by 1 pixel with every side
	for( int i = 0; i < cy; i++ )
	{
		//memcpy( m_pprows[i+1]+1, image.GetRowMask( i ), cx );
		byte *pSrc = image.GetRowMask( i );
		byte *pDst = m_pprows[i+1]+1;
		for(int j=0; j<cx; j++) pDst[j] = (pSrc[j] & 128) ? m_byteRawBody : 0 ;
		m_pprows[i+1][0] = 0;
		m_pprows[i+1][cx+1] = 0;
	}

	memset( m_pprows[0], 0, cx+2 );
	memset( m_pprows[cy+1], 0, cx+2 );

}

//construct WalkFillInfo sized by specified rectangle, init by zero
CWalkFillInfo::CWalkFillInfo( const CRect rect )
{
	m_pbmp_old = 0;

	m_rect = rect;

	InitDIBSection();

	int	cx = m_rect.Width();
	int	cy = m_rect.Height();


//"expand" image by 1 pixel with every side
	for( int i = 0; i < cy+2; i++ )
		memset( m_pprows[i], 0, cx+2 );
}

//destruct the fill info
CWalkFillInfo::~CWalkFillInfo()
{
	if( m_pbmp_old )
		m_dc.SelectObject( m_pbmp_old );

	delete m_pprows;

	for( int i = 0; i < GetContoursCount(); i++ )
		ContourDelete( GetContour( i ) );
	m_contours.RemoveAll();
}

//empty the contour list
void CWalkFillInfo::ClearContours()
{
	for( int i = 0; i < GetContoursCount(); i++ )
		ContourDelete( GetContour( i ) );
	m_contours.RemoveAll();
}

//walk the contours
bool CWalkFillInfo::InitContours(bool bOnlyOne, CPoint pointStart, CPoint *ppointFound, CRect rectBounds, IUnknown* pimgBase,  IUnknown** ppimgOut, bool bBaseImageIsObject)
{
/*#ifdef _DEBUG
	{
		for( int j = m_rect.top-1; j < m_rect.bottom+1; j++ )
			for( int i = m_rect.left-1; i < m_rect.right+1; i++ )
			{
				byte	pixel = GetPixel( i, j );
				ASSERT(  pixel == 0 ||
						pixel == 0xFF );

				if( j == m_rect.top-1 ||
					j == m_rect.bottom ||
					i == m_rect.left-1 ||
					i == m_rect.right )
					ASSERT( pixel == 0 );

			}


	}
#endif //_DEBUG*/

	if( rectBounds == NORECT )
		rectBounds = m_rect;

	if( pointStart.x == -1 )
		pointStart = rectBounds.TopLeft();

	bool	bFound = false;

	for( int j = pointStart.y; j < rectBounds.bottom; j++ )
		for( int i = rectBounds.left; i < rectBounds.right; i++ )
			if( GetPixel( i, j ) == m_byteRawBody )
			{
				Contour	*p = ::ContourWalk( *this, i, j, 6 );

				if( !p )
					continue;

				bFound = true;

				if( ppointFound )
				{
					ppointFound->x = i;
					ppointFound->y = j;
				}

				p->lFlags = cfExternal;

				AddContour( p );

				CRect	rect = ::ContourCalcRect( p );

				::ContourExternalMark( *this, p, m_byteExternalMark, m_byteContourMark );

				//rectObject = CalcContoursRect();

				bool bPresentImg = false;
				if(ppimgOut && pimgBase)
				{
					CImageWrp imgBase(pimgBase);
					CImageWrp imgOut(imgBase.CreateVImage( rect ));
					*ppimgOut = (IUnknown*)imgOut;
					imgOut.InitRowMasks();
					bPresentImg = true;


					//TEMP!!  TODO: add flags to InitRowMasks() func
					{
					long nRowCount = imgOut.GetHeight();
					long nRowWidth = imgOut.GetWidth();
					for(long nRow = 0; nRow < nRowCount; nRow++)
						ZeroMemory(imgOut.GetRowMask(nRow), nRowWidth);
					}

					
				}
				
				::ContourFillAreaWithHoles( *this, m_byteContourMark, m_byteExternalMark, m_byteFillBody, m_byteInternalMark, (bBaseImageIsObject ? rectBounds : rect), (bPresentImg?*ppimgOut:(bBaseImageIsObject?pimgBase:0)) );

				//kill external mark
				::ContourExternalMark( *this, p, 0, 0 );


				if( bOnlyOne )
					return true;
			}
	return bFound;
	
}

//create a dib section for drawinf inside wfi bytes
void CWalkFillInfo::InitDIBSection()
{
	m_byteFillBody = 0xFE;		//body
	m_byteRawBody = 0xFF;		//body
	m_byteContourMark = 0xFD;	//body
	m_byteExternalMark = 0x01;	//meat edge
	m_byteInternalMark = 0x02;  //hole edge
	
	DWORD	dwMemSize = sizeof(BITMAPINFOHEADER)+1024;
	byte	*pbyteInfo = new byte[dwMemSize];
	BITMAPINFOHEADER	*pbih = (BITMAPINFOHEADER	*)pbyteInfo;

	ZeroMemory( pbih, sizeof( BITMAPINFOHEADER ) );
	pbih->biSize = sizeof( BITMAPINFOHEADER );
	pbih->biWidth = m_rect.Width()+2;
	pbih->biHeight = m_rect.Height()+2;
	pbih->biPlanes = 1;
	pbih->biBitCount = 8;

	BITMAPINFO	*pbi = (BITMAPINFO	*)pbih;

	for( int i = 0; i < 256; i++ )
	{
		pbi->bmiColors[i].rgbBlue = i;
		pbi->bmiColors[i].rgbGreen = i;
		pbi->bmiColors[i].rgbRed = i;
		pbi->bmiColors[i].rgbReserved = 0;
	}

	m_dibs.SetBitmap( (BITMAPINFO*)pbi, 0 );

	CClientDC	dc( 0 );

	m_dc.CreateCompatibleDC( &dc );
	m_pbmp_old = m_dc.SelectObject( CBitmap::FromHandle( m_dibs.GetSafeHandle() ) );
	m_dc.SetWindowOrg( m_rect.left-1, m_rect.top-1 );
	m_dc.SelectStockObject( WHITE_PEN );
	m_dc.SelectStockObject( WHITE_BRUSH );
	m_dc.SetPolyFillMode( WINDING );


	delete pbyteInfo;

	m_pprows = new byte*[m_rect.Height()+2];

	int	nRowSize = (m_rect.Width()+5)/4*4;


	byte *pbyte = (byte*)m_dibs.GetDIBits();
	for( i = 0; i < m_rect.Height()+2; i++ )
		m_pprows[i] = pbyte+((m_rect.Height()+1-i)*nRowSize);
}

CRect CWalkFillInfo::CalcNoEmptyRect()
{
	int	x, y;
	CRect	rect = CRect( 0, 0, 0, 0 );
	bool	bInit = false;

	for( y = m_rect.top; y < m_rect.bottom; y++ )
	{
		byte	*prow = GetRowMask( y-m_rect.top );

		for( x = m_rect.left; x < m_rect.right; x++ )
		{
			if( *prow )
			{
				if( !bInit )
				{
					rect.left = x;
					rect.right = x+1;
					rect.top = y;
					rect.bottom = y+1;
					bInit = true;
				}
				else
				{
					rect.left = min( x, rect.left );
					rect.right = max( x+1, rect.right );
					rect.top = min( y, rect.top );
					rect.bottom = max( y+1, rect.bottom );
				}
			}
			prow++;
		}
	}
	return rect;
}


//empty(zero) the area
void CWalkFillInfo::ClearMasks()
{
	//memset(m_pprows, 0, (m_rect.Width()+1)*(m_rect.Height()+1));
	int	nRowSize = (m_rect.Width()+5)/4*4;
	for(int i = 0; i < m_rect.Height()+2; i++ )
		memset(m_pprows[i], 0, nRowSize);
	//ASSERT(FALSE);//Not implementerd yet!
}

void CWalkFillInfo::DetachContours()
{
	m_contours.RemoveAll();
}

int CWalkFillInfo::GetContoursCount()
{
	return m_contours.GetSize();
}
Contour	*CWalkFillInfo::GetContour( int nPos )
{
	return (Contour	*)m_contours.GetAt( nPos );
}

//get the bounds contour rect
CRect CWalkFillInfo::CalcContoursRect()
{
	int	nContoursCount = GetContoursCount(), nContour;
	CRect	rectTotal = NORECT;

	for( nContour = 0; nContour < nContoursCount; nContour++ )
	{
		Contour	*pContour = GetContour( nContour );
		CRect	rect = ::ContourCalcRect( pContour );

		if( rectTotal == NORECT )
			rectTotal = rect;
		else
		{
			rectTotal.left = min( rectTotal.left, rect.left );
			rectTotal.top = min( rectTotal.top, rect.top );
			rectTotal.right = max( rectTotal.right, rect.right );
			rectTotal.bottom = max( rectTotal.bottom, rect.bottom );
		}
	}

	return rectTotal;
}


void CWalkFillInfo::InitFillContours()
{
	for( int j = m_rect.top; j < m_rect.bottom; j++ )
		for( int i = m_rect.left; i < m_rect.right; i++ )
			if( GetPixel( i, j ) == m_byteRawBody )
			{
				//walk the contour
				Contour	*p = ::ContourWalk( *this, i, j, 6 );
				//nark the image

				if( !p )
					continue;

				p->lFlags = cfExternal;

				::ContourExternalMark( *this, p, m_byteExternalMark, m_byteContourMark );
				//get contour rect
				CRect	rect = ::ContourCalcRect( p );
				//fill it
				::ContourFillArea( *this, m_byteRawBody, m_byteExternalMark, m_byteFillBody, rect );
				::ContourDelete( p );
			}
}

void CWalkFillInfo::AddContour( Contour *p )
{
	m_contours.Add( p );
}

void CWalkFillInfo::AttachMasksToImage( IUnknown *punkImage )
{
	CImageWrp	image( punkImage );

	image.InitRowMasks();

	CRect	rectImage = image.GetRect();
	CRect	rectCommon;

	rectCommon.left = max( m_rect.left, rectImage.left );
	rectCommon.top = max( m_rect.top, rectImage.top );
	rectCommon.right = min( m_rect.right, rectImage.right );
	rectCommon.bottom = min( m_rect.bottom, rectImage.bottom );

	int	i, j;

	for( j = rectCommon.top; j < rectCommon.bottom; j++ )
	{
		byte	*pbyte = image.GetRowMask( j-rectImage.top );
		
		for( i = rectCommon.left; i < rectCommon.right; i++ )
		{
			if( GetPixel( i, j ) & BodyMask )
				pbyte[i-rectImage.left] = 0xFF;
			else
				pbyte[i-rectImage.left] = 0;
		}
	}
}


void CWalkFillInfo::AddImageMasks( IUnknown *punkImage )
{
	CImageWrp	image( punkImage );

	CRect	rectImage = image.GetRect();
	CRect	rectCommon;

	rectCommon.left = max( m_rect.left, rectImage.left );
	rectCommon.top = max( m_rect.top, rectImage.top );
	rectCommon.right = min( m_rect.right, rectImage.right );
	rectCommon.bottom = min( m_rect.bottom, rectImage.bottom );

	int	i, j;

	for( j = rectCommon.top; j < rectCommon.bottom; j++ )
	{
		byte	*pbyte = image.GetRowMask( j-rectImage.top );
		
		for( i = rectCommon.left; i < rectCommon.right; i++ )
		{
			if( pbyte[i-rectImage.left]  )
				SetPixel( i, j, m_byteRawBody );
		}
	}
}

void CWalkFillInfo::AttachFillInfoToImage( CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo,
	IUnknown* punkImage,  CRect rcDraw, color* pColors, color* pBackColors, IUnknown *punkView)
{
	color** pColor = 0;
	//_try(CWalkFillInfo, AttachFillInfoToImage)
	{
		if(rcDraw == NORECT || pColors == 0 || !punkImage)
			return;
		//save new data in m_ptrFillInfo
		CImageWrp image(punkImage);
		int nPanes = image.GetColorsCount();
		bool bIsSeq = false;
		int i, j, k;
		FillInfo* pInfo = 0;
		FillInfo* pInfoNext = 0;
		FillInfo* pInfoOrig = 0;
		FillInfo* pInfoCur = 0;
		pColor = new color*[nPanes];
		rcDraw.NormalizeRect();
		CImagePaneCache ImgPanes(punkImage, true, punkView);

		for(j = max(rcDraw.top, m_rect.top); j <= min(rcDraw.bottom, m_rect.bottom-1); j++)
		{	
			for(k = 0; k < nPanes; k++)
			{
				pColor[k] = image.GetRow(j, k);	//image row
			}

			byte	*pbyteMask = image.GetRowMask( j );

			pInfo = 0;
			bool bFillSeq = false;
			bIsSeq = false;
			byte pixel;
			for(i = max(rcDraw.left, m_rect.left); i <= min(rcDraw.right, m_rect.right-1); i++)
			{
				pixel = GetPixel(i, j );
				if(pixel & BodyMask && pbyteMask[i] != 0)
				{
					if((pixel == m_byteFillBody) != bFillSeq)
					{
						if(bIsSeq)
						{		
							//pInfo->pColors = new color[nPanes*pInfo->nCount];
							pInfo->CreateColors(nPanes*pInfo->nCount);

							for (int n = 0; n < pInfo->nCount; n++)
							for(k = 0; k < nPanes; k++)
							{
								pInfo->pColors[k+nPanes*n] = pColor[k][pInfo->nX+n];	//image row
								if (ImgPanes.IsPaneEnable(k))
									pColor[k][pInfo->nX+n] = bFillSeq ? pBackColors[k] : pColors[k];
							
							}
						}
						bIsSeq = false;
						bFillSeq = pixel == m_byteFillBody;
					}
					if(!bIsSeq)
					{
						pInfoOrig = pptrFillInfo->GetAt(j);
						if(pInfoOrig->nCount == 0)
						{
							pInfo = pInfoOrig;
						}
						else
						{
							//pInfo = new FillInfo;
							pInfo = FillInfo::Create();

							pInfo->pNext = pInfoOrig;
							pInfo->pColors = 0;
							pptrFillInfo->SetAt(j, pInfo);
						}

						pInfo->nX = i;
						pInfo->nCount = 1;
						pInfo->bFill = bFillSeq;
						
						bIsSeq = true;
					}
					else
					{
						pInfo->nCount += 1;
					}
				}
				else
				{
					if(bIsSeq)
					{		
						//pInfo->pColors = new color[nPanes*pInfo->nCount];
						pInfo->CreateColors(nPanes*pInfo->nCount);
						for (int n = 0; n < pInfo->nCount; n++)
						for(k = 0; k < nPanes; k++)
						{
							pInfo->pColors[k+nPanes*n] = pColor[k][pInfo->nX+n];	//image row
							if (ImgPanes.IsPaneEnable(k))
								pColor[k][pInfo->nX+n] = bFillSeq ? pBackColors[k] : pColors[k];
						}
					}
					bIsSeq = false;
				}
			}
			if(bIsSeq)
			{
				//pInfo->pColors = new color[nPanes*pInfo->nCount];
				pInfo->CreateColors(nPanes*pInfo->nCount);
				for (int n = 0; n < pInfo->nCount; n++)
				for(k = 0; k < nPanes; k++)
				{
					pInfo->pColors[k+nPanes*n] = pColor[k][pInfo->nX+n];	//image row
					if (ImgPanes.IsPaneEnable(k))
						pColor[k][pInfo->nX+n] = bFillSeq ? pBackColors[k] : pColors[k];
				}
			}
		}
		delete pColor;

		return;
	}
	//_catch;

	if(pColor)delete pColor;
}

void CWalkFillInfo::AttachFillInfoToImageBin( CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo,
	IUnknown* punkImage,  CRect rcDraw, color* pColors, color* pBackColors, IUnknown *punkView)
{
	color** pColor = 0;
	{
		if(rcDraw == NORECT || pColors == 0 || !punkImage)
			return;
		//save new data in m_ptrFillInfo
		CBinImageWrp binary(punkImage);

		int nBinRowSize = 0;
		
		bool bIsSeq = false;
		int i, j;
		FillInfo* pInfo = 0;
		FillInfo* pInfoNext = 0;
		FillInfo* pInfoOrig = 0;
		FillInfo* pInfoCur = 0;
		rcDraw.NormalizeRect();

		byte* pbin = 0;

		IApplicationPtr sptrApp(GetAppUnknown());
		IUnknown* pDoc = 0;
		sptrApp->GetActiveDocument(&pDoc);
		IUnknown* punkBaseImage = 0;
		if(pDoc)
		{
			IDocumentSitePtr sptrDoc(pDoc);
			IUnknown* punkView  = 0;
			sptrDoc->GetActiveView(&punkView);
			if(punkView)
			{
				punkBaseImage = GetActiveObjectFromContext(punkView, _bstr_t(szArgumentTypeImage));
				punkView->Release();
			}
			pDoc->Release();
		}

		CImageWrp img(punkBaseImage);
		if(punkBaseImage)
			punkBaseImage->Release();

		CPoint ptOffset(0,0);
		binary->GetOffset(&ptOffset);
		CRect rcImg = img.GetRect();
		if(img) 
		{
			rcImg.DeflateRect(0,0,1,1);
			rcImg.OffsetRect(-ptOffset);
			// [Max]Draw trace outside soucre image rect if binary image has moved 
			rcDraw.IntersectRect(&rcDraw, &rcImg);
			// monster: вернул строчку выше на место - без нее вылет [BT4416]
			if (rcDraw == NORECT)
				return;
		}
		
		byte	*pbyteMask = 0;	

		long nTop    = max(rcDraw.top, m_rect.top);
		long nBottom = min(rcDraw.bottom, m_rect.bottom-1);
		long nLeft   = max(rcDraw.left, m_rect.left);
		long nRight  = min(rcDraw.right, m_rect.right-1);

		for(j = nTop; j <= nBottom; j++)
		{	
			binary->GetRow(&pbin, j, FALSE);
			pInfo = 0;
			bIsSeq = false;
			byte pixel;

			if(img != 0)
				pbyteMask = img.GetRowMask(j - rcImg.top);

			for(i = nLeft; i <= nRight; i++)
			{
				pixel = GetPixel(i, j);
				if((pixel & BodyMask) && pbyteMask && (pbyteMask[i-rcImg.left] != 0))
				{
					if(!bIsSeq)
					{
						pInfoOrig = pptrFillInfo->GetAt(j);
						if(pInfoOrig->nCount == 0)
						{
							pInfo = pInfoOrig;
						}
						else
						{
							pInfo = FillInfo::Create();
							pInfo->pNext = pInfoOrig;
							pInfo->pColors = 0;
							pptrFillInfo->SetAt(j, pInfo);
						}

						pInfo->nX = i;
						pInfo->nCount = 1;
						pInfo->bFill = false;
						bIsSeq = true;
					}
					else
					{
						pInfo->nCount += 1;
					}
				}
				else
				{
					if(bIsSeq)
					{
						bIsSeq = false;
						/*long nColorSize = sizeof(color)*8;
						long nAllocSize = pInfo->nCount/nColorSize+(pInfo->nCount%nColorSize==0 ? 0 : 1);
						pInfo->CreateColors(nAllocSize);
						ZeroMemory(pInfo->pColors, nAllocSize*sizeof(color));
						long nX = pInfo->nX;
						for (int n = 0; n < pInfo->nCount; n++, nX++)
						{
							if((pbin[nX/8] & (0x80>>(nX%8))) != 0)
							{
								pInfo->pColors[n/nColorSize] |= 0x8000>>( n % nColorSize );
								if(pColors[0] == 0)
									pbin[nX/8] &= (~(0x80>>( nX % 8 )));
							}
							else
							{
								if(pColors[0] == 1)
									pbin[nX/8] |= 0x80>>(nX%8);
							}
						}*/

						pInfo->CreateColors(pInfo->nCount);
						long nX = pInfo->nX;
						for (int n = 0; n < pInfo->nCount; n++, nX++)
						{
							pInfo->pColors[n] = (byte)pbin[nX];
							pbin[nX] = (byte)pColors[0];
						}
					}
				}
			}
			if(bIsSeq)
			{
				/*long nColorSize = sizeof(color)*8;
				long nAllocSize = pInfo->nCount/nColorSize+(pInfo->nCount%nColorSize==0 ? 0 : 1);
				pInfo->CreateColors(nAllocSize);
				ZeroMemory(pInfo->pColors, nAllocSize*sizeof(color));
				long nX = pInfo->nX;
				for (int n = 0; n < pInfo->nCount; n++, nX++)
				{
					if((pbin[nX/8] & (0x80>>(nX%8))) != 0)
					{
						pInfo->pColors[n/nColorSize] |= 0x8000>>( n % nColorSize );
						if(pColors[0] == 0)
							pbin[nX/8] &= (~(0x80>>( nX % 8 )));
					}
					else
					{
						if(pColors[0] == 1)
							pbin[nX/8] |= 0x80>>(nX%8);
					}
				}*/
        pInfo->CreateColors(pInfo->nCount);
        long nX = pInfo->nX;
				for (int n = 0; n < pInfo->nCount; n++, nX++)
				{
          pInfo->pColors[n] = pbin[nX];
          pbin[nX] = (byte)pColors[0];
				}
			}
		}
	}
}


bool CWalkFillInfo::InitFillInfo(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo)
{
	_try(CWalkFillInfo, InitFillInfo)
	{
		if (m_rect == NORECT)
			return false;
		pptrFillInfo->SetSize(m_rect.Height());
		FillInfo* pInfo = 0;
		for(int i = 0; i < m_rect.Height(); i++)
		{
			//pInfo = new FillInfo;
			pInfo = FillInfo::Create();
			pInfo->pNext = 0;
			pInfo->nX = 0;
			pInfo->nCount = 0;
			pInfo->pColors = 0;
			pptrFillInfo->SetAt(i, pInfo);
		}
		pInfo = 0;

		return true;
	}
	_catch;
		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//contour api
//mark contour to walkinfo in specified byte
//draw a contour
std_dll void ContourDraw( CDC &dc, Contour *pc, IUnknown *punkVS, DWORD dwFlags )
{
	double fZoom = 1;
	CPoint pointScroll = CPoint( 0, 0 );

	if( punkVS )
	{
		fZoom = GetZoom( punkVS );
		pointScroll = GetScrollPos( punkVS );
	}
	::ContourDraw( dc, pc, fZoom, pointScroll, dwFlags );

}
std_dll void ContourDraw( CDC &dc, Contour *pc, double fZoom, CPoint pointScroll, DWORD dwFlags )
{
	if( !pc )
		return;

	if( pc->nContourSize == 0 )
		return;

	long	n = 0;
	long	nContourSize = pc->nContourSize;

	int		nSize = nContourSize;
	if ((dwFlags & cdfSpline) == cdfSpline)
	{
		nSize = 20 * (dwFlags & cdfClosed == cdfClosed ? nContourSize : nContourSize + 1);
	}
	POINT	*ppts = new POINT[ nSize+1 ];


	{
		int	nPtsCount = 0;

		if( (dwFlags & cdfSpline ) == 0 )
		{
			for( ; n < nContourSize; n++ )
			{
				ppts[nPtsCount].x = int((pc->ppoints[n].x+.5)*fZoom)-pointScroll.x;
				ppts[nPtsCount].y = int((pc->ppoints[n].y+.5)*fZoom)-pointScroll.y;
				nPtsCount++;
			}
			if( (dwFlags & cdfClosed ) != 0 )
			{
				ppts[nPtsCount].x = int((pc->ppoints[0].x+.5)*fZoom)-pointScroll.x;
				ppts[nPtsCount].y = int((pc->ppoints[0].y+.5)*fZoom)-pointScroll.y;
				nPtsCount++;
			}
		}
		else
		{
			if( nContourSize == 1 )
			{
				delete ppts;
				return;
			}

			bool	bCycle = dwFlags & cdfClosed == cdfClosed;
			int		nMaxIndex = bCycle?nContourSize:nContourSize+1;

			for( ; n < nMaxIndex; n++ )
			{
				//add spline points
				int	idx1 = n-2;
				int	idx2 = n-1;
				int	idx3 = n;
				int	idx4 = n+1;

				if( (dwFlags & cdfClosed) == 0 )
				{
					if( idx1 < 0 )idx1 = 0;
					if( idx2 < 0 )idx2 = 0;
					if( idx3 >= nContourSize )idx3 = nContourSize-1;
					if( idx4 >= nContourSize )idx4 = nContourSize-1;
				}
				else
				{
					if( nContourSize > 2 )
					{
						if( idx1 < 0 )idx1 += nContourSize;
						if( idx2 < 0 )idx2 += nContourSize;
						if( idx3 >= nContourSize )idx3 -= nContourSize;
						if( idx4 >= nContourSize )idx4 -= nContourSize;
					}
					else
					{
						idx1 = 0;
						idx2 = 0;
						idx3 = 1;
						idx4 = 1;
					}
				}

				ASSERT( idx1 >= 0 );ASSERT( idx1 < nContourSize );
				ASSERT( idx3 >= 0 );ASSERT( idx3 < nContourSize );
				ASSERT( idx4 >= 0 );ASSERT( idx4 < nContourSize );
				double	a0, a1, a2, a3;
				double	b0, b1, b2, b3;
				double	xa, xb, xc, xd;
				double	ya, yb, yc, yd;

				xa = pc->ppoints[idx1].x;
				xb = pc->ppoints[idx2].x;
				xc = pc->ppoints[idx3].x;
				xd = pc->ppoints[idx4].x;

				ya = pc->ppoints[idx1].y;
				yb = pc->ppoints[idx2].y;
				yc = pc->ppoints[idx3].y;
				yd = pc->ppoints[idx4].y;

				a3 = (-xa+3*(xb-xc)+xd)/6.0;
				a2 = (xa-2*xb+xc)/2.0;
				a1 =(xc-xa)/2.0;
				a0 = (xa+4*xb+xc)/6.0;

				b3 = (-ya+3*(yb-yc)+yd)/6.0;
				b2 = (ya-2*yb+yc)/2.0;
				b1 =(yc-ya)/2.0;
				b0 = (ya+4*yb+yc)/6.0;


				for(double t = 0; t< 1.0; t+=0.05)
				{
					ppts[nPtsCount].x = int((((a3*t+a2)*t+a1)*t+a0)*fZoom+.5)-pointScroll.x;
					ppts[nPtsCount].y = int((((b3*t+b2)*t+b1)*t+b0)*fZoom+.5)-pointScroll.y;
					nPtsCount++;
				}
			}
		}

		{
			if( dwFlags & cdfFill )
				dc.Polygon( ppts, nPtsCount );
			else
				dc.Polyline( ppts, nPtsCount );
		}
	}

	delete [] ppts;

	if( dwFlags & cdfPoint )
	{
		for( int nPoint = 0; nPoint < pc->nContourSize; nPoint++ )
		{
			CPoint	point;
			point.x = int(pc->ppoints[nPoint].x*fZoom+.5)+pointScroll.x;
			point.y = int(pc->ppoints[nPoint].y*fZoom+.5)+pointScroll.y;

			CRect	rect;
			rect.left = point.x - 5;
			rect.right = point.x + 5;
			rect.top = point.y - 5;
			rect.bottom = point.y + 5;

			dc.SelectStockObject( WHITE_BRUSH );
			dc.SelectStockObject( BLACK_PEN );

			dc.Rectangle( rect );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//CWalkFillInfo helpers
const int directX[8] = {0, 1, 1, 1, 0, -1, -1, -1 };
const int directY[8] = {-1, -1, 0, 1, 1, 1, 0, -1 };

//add a point to the contour
std_dll void ContourAddPoint( Contour *pcntr, int x, int y, bool bAddSamePoints)
{
	if(pcntr->nContourSize > 0)
	{
		if(pcntr->ppoints[pcntr->nContourSize-1].x == x &&
		   pcntr->ppoints[pcntr->nContourSize-1].y == y && !bAddSamePoints)
		   return;
	}
	const int iContourAligment=1000;
	if( !pcntr->nAllocatedSize )
	{
		ASSERT( !pcntr->ppoints );

		pcntr->ppoints = new ContourPoint[iContourAligment];
		pcntr->nAllocatedSize = iContourAligment;
		
	}

	if( pcntr->nContourSize == pcntr->nAllocatedSize )
	{
		ContourPoint	*p = pcntr->ppoints;
		pcntr->ppoints = new ContourPoint[pcntr->nAllocatedSize+iContourAligment];
		memcpy( pcntr->ppoints, p, pcntr->nAllocatedSize*sizeof( ContourPoint ) );
		pcntr->nAllocatedSize+=iContourAligment;
		delete p;
	}

	pcntr->ppoints[pcntr->nContourSize].x = x;
	pcntr->ppoints[pcntr->nContourSize].y = y;
	pcntr->nContourSize++;
}

//kill last point entry
std_dll void ContourKillLastPoint( Contour *pcntr)
{
	pcntr->nContourSize--;
}

//set point
std_dll void ContourSetPoint( Contour *pcntr, int nNumberPoint, int x, int y)
{
	try
	{
		pcntr->ppoints[nNumberPoint].x = x;
		pcntr->ppoints[nNumberPoint].y = y;
	}
	catch(...)
	{
		ASSERT(FALSE);
	}
}

//get point
std_dll void ContourGetPoint( Contour *pcntr, int nNumberPoint, int& x, int& y)
{
	try
	{
		x = pcntr->ppoints[nNumberPoint].x;
		y = pcntr->ppoints[nNumberPoint].y;
	}
	catch(...)
	{
		ASSERT(FALSE);
	}
}

//create an empty contour
std_dll Contour *ContourCreate()
{
	Contour *pcntr = new Contour;

	pcntr->nContourSize = 0;
	pcntr->lFlags = 0;
	pcntr->lReserved = 0;
	pcntr->nAllocatedSize = 0;
	pcntr->ppoints = 0;

	return pcntr;
}

//create the copy of contour
std_dll Contour *ContourCopy(Contour *pContourSrc)
{
	Contour *pcntr = new Contour;

	pcntr->lFlags = pContourSrc->lFlags;
	pcntr->lReserved = pContourSrc->lReserved;
	pcntr->nAllocatedSize = pContourSrc->nContourSize;
	pcntr->nContourSize = pContourSrc->nContourSize;
	pcntr->ppoints = new ContourPoint[pcntr->nAllocatedSize];
	memcpy( pcntr->ppoints, pContourSrc->ppoints, pcntr->nAllocatedSize*sizeof( ContourPoint ) );

	return pcntr;
}


//delete a contour
std_dll void ContourDelete( Contour *p )
{
	if( p )delete p->ppoints;
	delete p;
}

//make contour empty
std_dll void ContourEmpty(Contour *p)
{
	delete p->ppoints;
	p->nAllocatedSize = p->nContourSize = 0;
	p->ppoints = NULL;
}

//make contour compact
std_dll void ContourCompact(Contour *pc)
{
	if (pc->nContourSize < pc->nAllocatedSize)
	{
		ContourPoint *p = new ContourPoint[pc->nContourSize];
		memcpy( p, pc->ppoints, pc->nContourSize*sizeof( ContourPoint ) );
		delete pc->ppoints;
		pc->ppoints = p;
		pc->nAllocatedSize = pc->nContourSize;
	}
}


//walk a contour
//Функция Walk. обойти контур
std_dll Contour *ContourWalk( CWalkFillInfo &info, int iStartX, int iStartY, int iStartDir /*=-1*/)
{
	int		i, x = iStartX, y = iStartY;

	if( info.GetPixel( x, y ) == 0 )
		return 0;

//if it is single point, return false
	for( i = 0; i< 8; i++ )
		if( info.GetPixel( x+directX[i], y+directY[i] ) & BodyMask )
			break;

	if( i == 8 )
		return 0;
//check the start direction. Get it, if not specifie
	if( iStartDir != -1 )
	{

		if( info.GetPixel( x+directX[iStartDir], y+directY[iStartDir] ) & BodyMask )
			return 0;	
		i = iStartDir;
	}
	else 
	{
		for(i = 0; i<8; i++)
			if( info.GetPixel( x+directX[i], y+directY[i] ) & BodyMask )
				break;
		iStartDir = i;
	}
//add the start point to the contour
	Contour	*pcntr = ContourCreate();
	ContourAddPoint( pcntr, x, y );

	while( true )
	{
//find the next "body" point
		for(;;i=(i==7)?0:(i+1))
			if( info.GetPixel( x+directX[i], y+directY[i] ) & BodyMask )
				break;
		
		x+=directX[i];
		y+=directY[i];
		i=i>2?(i-3):(i+5);
//we has walked to the first point
		if( iStartX == x && iStartY == y )
			break;
//add the next
		ContourAddPoint( pcntr, x, y );
#ifdef _DEBUG
//assert the contour is not very large - possible bug
		ASSERT(pcntr->nContourSize < 100000);
		if(pcntr->nContourSize > 100000)break;
#endif	//_DEBUG	
	}
//check if image cross the start point more that one time
	for(;;i=(i==7)?0:(i+1))
	{
		if( info.GetPixel( x+directX[i], y+directY[i] ) & BodyMask )
			break;
	}

	x+=directX[i];
	y+=directY[i];
	if( x != pcntr->ppoints[1].x || y != pcntr->ppoints[1].y )
	{
//add the start point second time and add the next point
		ContourAddPoint( pcntr, iStartX, iStartY );
		ContourAddPoint( pcntr, x, y );
//get the next direction
		i=i>2?(i-3):(i+5);

		while( true )
		{
//find the next "body" point
			for(;;i=(i==7)?0:(i+1))
				if( info.GetPixel( x+directX[i], y+directY[i] ) & BodyMask )
					break;
							
			x+=directX[i];
			y+=directY[i];
			i=i>2?(i-3):(i+5);
//we has walked to the first point
			if( iStartX == x && iStartY == y )
				break;
//add the next
			ContourAddPoint( pcntr, x, y );
#ifdef _DEBUG
//assert the contour is not very large - possible bug
			ASSERT(pcntr->nContourSize < 100000);
			if(pcntr->nContourSize > 100000)break;
#endif	//_DEBUG	
		}
	}

	return pcntr;
}

//offset a contour
std_dll void ContourOffset( Contour *p, CPoint pointOffset )
{
	for( int i = 0; i < p->nContourSize; i++ )
	{
		p->ppoints[i].x += (short)pointOffset.x;
		p->ppoints[i].y += (short)pointOffset.y;
	}
}

std_dll void ContourMark( CWalkFillInfo &info, Contour *pcntr, byte byteMark )
{
	for( int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;
		info.SetPixel( x, y, byteMark );
	}
}

//mark external area of contour in specified byte
std_dll void ContourExternalMark( CWalkFillInfo &info, Contour *pcntr, byte byteMark, byte byteMarkContour )
{
	if( pcntr->nContourSize == 0 )
		return;

	int	nDir, nNextDir = ContourGetDirection( pcntr, pcntr->nContourSize-1 );

	for( int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		nDir = nNextDir;
		nNextDir = ContourGetDirection( pcntr, nPoint );

		ASSERT( nDir != -1 );
		ASSERT( nNextDir != -1 );

		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;

		for( nDir=(nDir < 3)?nDir+5:nDir-3;
				nDir != nNextDir; 
				nDir=((nDir==7)?0:nDir+1) )
			info.SetPixel( x+directX[nDir], y+directY[nDir], byteMark );

		if(byteMarkContour)
			info.SetPixel( x, y, byteMarkContour );
	}

}

//fills the single area in wfi
std_dll Contour *ContourFillAreas( CWalkFillInfo &info )
{
	info.InitFillContours();
	return 0;
}

//get the direction
std_dll int ContourGetDirection( Contour *pcntr, int idx )
{
	int	idxNext = (idx==pcntr->nContourSize-1)?0:idx+1;

	int	dx = pcntr->ppoints[idxNext].x-pcntr->ppoints[idx].x;
	int	dy = pcntr->ppoints[idxNext].y-pcntr->ppoints[idx].y;

	for( int i = 0; i < 8; i++ )
		if( dx == directX[i] && dy == directY[i] )
			return i;
	return -1;
}

std_dll void ContourFillArea( CWalkFillInfo &info, byte byteRawBody, byte byteExtMark, byte byteFill, CRect rect )
{
	if( rect == NORECT )
		rect = info.GetRect();

	int	x, y;

	byte	bytePrev, byteCur;
	bool	bFill = false;

	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		byteCur = 0x00;
		for( x = rect.left-1; x < rect.right+1; x++ )
		{
			bytePrev = byteCur;
			byteCur = info.GetPixel( x, y );

			if( bytePrev == byteExtMark && byteCur == byteRawBody )
			{
				ASSERT( !bFill );
				bFill = true;
			}
			if( bytePrev == byteRawBody && byteCur == byteExtMark )
			{
				ASSERT( bFill );
				bFill = false;
			}

			if( bFill )
				info.SetPixel( x, y, byteFill );
		}

		ASSERT( !bFill );
	}
}

std_dll void ContourFillAreaWithHoles( CWalkFillInfo &info, byte byteRawBody, byte byteExtMark, byte byteFill, byte byteIntMark, CRect rect, IUnknown* punkimage )
{
	if( rect == NORECT )
		rect = info.GetRect();

	int	x, y;

	byte	bytePrev, byteCur;
	bool	bFill = false;
	bool	bFillHole = false;

	CImageWrp image(punkimage);
	CPoint	point( 0, 0 );
	CSize	sizeImg;
	if( image != 0 )
	{
		point = image.GetOffset();
		image->GetSize((int*)&sizeImg.cx, (int*)&sizeImg.cy);
	}

	
	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		byte	*pmask = 0;

		if(punkimage) 
		{
			//if(y >= rect.top && y < rect.bottom)
			//	pmask = image.GetRowMask( y-rect.top );
			if(y >= point.y && y < point.y+sizeImg.cy )
				pmask = image.GetRowMask( y-point.y );
			else
				pmask = 0;
		}

		byteCur = 0x00;
		for( x = rect.left-1; x < rect.right+1; x++)
		{
			bytePrev = byteCur;
			byteCur = info.GetPixel( x, y );

			if( (bytePrev == byteExtMark  && byteCur == byteRawBody)/*||(bytePrev == byteExtMark  && byteCur == byteFill)*/ || (bytePrev == byteIntMark && byteCur == byteRawBody) )
			{
				ASSERT( !bFill );
				bFill = true;
			}
			if( (bytePrev == byteRawBody && byteCur == byteExtMark) /*|| (byteCur == byteExtMark && bytePrev == byteFill)*/ || (byteCur == byteIntMark && bytePrev == byteRawBody))
			{
				ASSERT( bFill );
				bFill = false;
			}

			if(bytePrev == byteRawBody && byteCur == byteIntMark)
			{
				ASSERT(!bFillHole);
				bFillHole = true;

			}

			if(bytePrev == byteIntMark && byteCur == byteRawBody)
			{
				bFillHole = false;
			}
				
						
			if( bFill && (bytePrev & BodyMask) == BodyMask
				&& byteCur == 0 )
			{
				Contour *p = ::ContourWalk( info, x-1, y, 2 );

				if( !p )
					continue;

				p->lFlags = cfInternal;

				::ContourExternalMark( info, p, byteIntMark, byteRawBody);
				//::ContourExternalMark( info, p, byteExtMark, byteRawBody );
				bFill = false;

				byteCur = info.GetPixel( x, y );

				// 26.09.2005 build 90
				// BT 4456 - первая горизонтальная линия в дырке не учитывалась в Inner Area
				bytePrev = info.GetPixel( x-1, y );

				// 10.08.2005 build 88
				// BT 4456 - первая горизонтальная линия в дырке не учитывалась в Inner Area
				if(bytePrev == byteRawBody && byteCur == byteIntMark)
				{
					ASSERT(!bFillHole);
					bFillHole = true;
				}

				ASSERT( byteCur == byteIntMark );
				
				info.AddContour( p );
			}

									
			if( bFill )
				info.SetPixel( x, y, byteFill );


			//if(pmask && x >= rect.left && x < rect.right)
			if(pmask && x >= point.x && x < point.x+sizeImg.cx )
			{
				//[AY]
				//*pmask = 0x00;
				if(bFillHole)
					*pmask = 0x01;
				if(bFill)
					*pmask = 0xFF;
				pmask++;
			}
		}

		ASSERT( !bFill );
		ASSERT(!bFillHole);
	}

	for( int c = 0; c < info.GetContoursCount(); c++ )
		::ContourMark( info, info.GetContour( c ), byteFill );
}



//calc the contour sizes
std_dll CRect ContourCalcRect( Contour *pcntr )
{
	CRect	rect;
	bool	bFound = false;

	for( int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;

		if( !bFound )
		{
			bFound = true;
			rect.left = x;
			rect.right = x+1;
			rect.top = y;
			rect.bottom = y+1;
		}
		else
		{
			rect.left = min( x, rect.left );
			rect.right = max( x+1, rect.right );
			rect.top = min( y, rect.top );
			rect.bottom = max( y+1, rect.bottom );
		}
	}

	return rect;
}

void __stdcall LineDDAProcContour( int x, int y, LPARAM lData )
{
	Contour	*pcntr = (Contour*)lData;

	if( pcntr->nContourSize > 0 )
		if( pcntr->ppoints[pcntr->nContourSize-1].x == x &&
			pcntr->ppoints[pcntr->nContourSize-1].y == y )
			return;

	::ContourAddPoint( pcntr, x, y );
}
 
//fill the contour holes
std_dll Contour *ContourFillHoles( Contour *pcntr )
{
	if( !pcntr || !pcntr->nContourSize )
		return 0;
	int	x = pcntr->ppoints[pcntr->nContourSize-1].x;
	int	y = pcntr->ppoints[pcntr->nContourSize-1].y;

	Contour *pcntrNew = ContourCreate();
	pcntrNew->lFlags = pcntr->lFlags;

	for( int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		int	nOldX = x;
		int	nOldY = y;
		x = pcntr->ppoints[nPoint].x;
		y = pcntr->ppoints[nPoint].y;

		::LineDDA( nOldX, nOldY, x, y, LineDDAProcContour, (long)pcntrNew ); 
	}

	return pcntrNew;
}

std_dll void ContourApplySmoothFilter( Contour *pcntr, DWORD dwNum, double *x, double *y )
{
	int	iSize = (int)pcntr->nContourSize;
	int	iNum  = (int)dwNum;


	if(iSize <= 0)
	{
		*x = -1;
		*y = -1;
		return;
	}

	if( iSize < 3 )
	{
		*x = pcntr->ppoints[iNum].x;
		*y = pcntr->ppoints[iNum].y;
		return;
	}
	int	dw1, dw2, dw3, dw4, dw5;

	//if( m_bCycled )
	{
		dw1 = (iNum>1)?iNum-2:iSize+iNum-2;
		dw2 = (iNum>0)?iNum-1:iSize+iNum-1;
		dw3 = iNum;
		dw4 = (iNum<(iSize-1))?iNum+1:iNum-iSize+2;
		dw5 = (iNum<(iSize-2))?iNum+2:iNum-iSize+3;
	}
	/*else
	{
		dw1 = max( iNum-2, 0 );
		dw2 = max( iNum-1, 0 );
		dw3 = iNum;
		dw4 = min( iNum+1, iSize-1 );
		dw5 = min( iNum+2, iSize-1 );
	}*/

	*x = (pcntr->ppoints[dw1].x+2*pcntr->ppoints[dw2].x+3*pcntr->ppoints[dw3].x+2*pcntr->ppoints[dw4].x+pcntr->ppoints[dw5].x)/9.;
	*y = (pcntr->ppoints[dw1].y+2*pcntr->ppoints[dw2].y+3*pcntr->ppoints[dw3].y+2*pcntr->ppoints[dw4].y+pcntr->ppoints[dw5].y)/9.;
}

#pragma optimize("", off)
double ContourMeasurePerimeter(Contour *p, bool bUseSmooth, double fXCalibr, double fYCalibr)	
{
	double fP = 0;

	if(!p || p->nContourSize <= 0) //empty
		return fP;

	if(p->nContourSize <  3)
	{
		fP += 1;
	}

	double	x1, y1;	//next point
	double	x2, y2;	//current point

	double	dist;	 

	if( bUseSmooth )
	{
		ContourApplySmoothFilter(p, 0, &x2, &y2);
		ContourApplySmoothFilter(p, 1, &x1, &y1);
	}
	else
	{
		x2 = p->ppoints[0].x;
		y2 = p->ppoints[0].y;
		x1 = p->ppoints[1].x;
		y1 = p->ppoints[1].y;
	}

	double dx = (x2-x1)*fXCalibr;
	double dy = (y2-y1)*fYCalibr;
	dist=sqrt( dx*dx+dy*dy );

	fP+=dist;

	for( DWORD dw = 2; dw< (DWORD)p->nContourSize; dw ++ )
	{
		x2 = x1;
		y2 = y1;

		if(bUseSmooth)
		{
			ContourApplySmoothFilter(p, dw, &x1, &y1);
		}
		else
		{
			x1 = p->ppoints[dw].x;
			y1 = p->ppoints[dw].y;
		}
			
		double dx = (x2-x1)*fXCalibr;
		double dy = (y2-y1)*fYCalibr;
		dist=sqrt( dx*dx+dy*dy );

		fP+=dist;
	}

	return fP;
}
#pragma optimize("", on)