#include "StdAfx.h"
#include <vfw.h>
#include "image.h"
#include "appdefs.h"
//#include "resource.h"
#include <math.h>
//#include "CnvWork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EXCPT_INVALIDIMAGE 0

class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	hdd;
	


// tested
template <class COLORREPR> CImage<COLORREPR>::CImage<COLORREPR>(void)
{
	DeInit();
}

// not tested yet
template <class COLORREPR> CImage<COLORREPR>::CImage<COLORREPR>( HGLOBAL hDIB )
{
	DeInit();
	Attach(hDIB);
}

// not tested yet
template <class COLORREPR> CImage<COLORREPR>::CImage<COLORREPR>( short cx, short cy, int fsOptions )
{
	DeInit();
	Create(cx,cy,fsOptions);
}

// not tested yet
template <class COLORREPR> CImage<COLORREPR>::CImage<COLORREPR>( const CImage &Image )
{
	DeInit();
	Attach((CImageBase *)&Image);
}

// tested
#if defined _DEBUG && defined(__VT4__)
extern int g_bInsidePaint;
#endif

template <class COLORREPR> CImage<COLORREPR>::~CImage<COLORREPR>(void)
{
#if defined _DEBUG && defined(__VT4__)
	if (!g_bInsidePaint)
		g_bInsidePaint = FALSE;
#endif
	Free();
}

// tested for truecolor and grayscale images for 8-bit data
// Call stack:
//  CImage<unsigned char>::Create(short 380, short 50, int 1) line 84
//  MakePaintFromRGB(CImage<unsigned char> & {CImage<unsigned char>}, CImage<unsigned char> & {CImage<unsigned char>}, CRect {top=0 bottom=50 left=0 right=380}, int 1) line 191
//  CFrame::GetPaintImage(CImage<unsigned char> & {CImage<unsigned char>}, CRect {top=0 bottom=50 left=0 right=380}) line 976 + 52 bytes
//  CFrame::DoPaintImage(CDC & {hDC=0x000005ee attrib=0x000005ee}, CRect {top=0 bottom=195 left=0 right=380}) line 870
//  CFrame::DoPaint(CDC & {hDC=0x000005ee attrib=0x000005ee}, CRect {top=0 bottom=195 left=0 right=380}) line 832
template <class COLORREPR> void CImage<COLORREPR>::Create( short cx, short cy, int fsOptions )
{
	m_cx = cx;
	m_cy = cy;
	m_iCols = CalcColsNum(cx,fsOptions);
	m_iIncr = CalcInc(fsOptions);
	AllocBuffer( GetDataSize(m_iCols, m_cy, fsOptions), m_cy, fsOptions );
	AllocRows();
	if (!(fsOptions&NoInit))
	{
		InitHeader(fsOptions);
	}
	if (!(fsOptions&TrueColor)&&sizeof(COLORREPR)==1)
	{
		InitPaletteBits(m_pRGBQuad);
	}
}

// tested, see CColorConvert::DoAttach
template <class COLORREPR> void CImage<COLORREPR>::Attach8( CImage<BYTE> const &Image )
{
	CColorConvert<BYTE,COLORREPR>::DoAttach(*this,Image);
	if (!IsEmpty()&&!IsTrueColor()&&sizeof(COLORREPR)==1)
	{
		ReplacePalette(Image.GetRGBQuad());
//		InitPaletteBits(m_pRGBQuad);
	}
}

// tested, see CColorConvert::DoAttach
template <class COLORREPR> void CImage<COLORREPR>::Attach16( CImage<WORD> const &Image )
{
	CColorConvert<WORD,COLORREPR>::DoAttach(*this,Image);
	if (!IsEmpty()&&!IsTrueColor()&&sizeof(COLORREPR)==1)
	{
		InitPaletteBits(m_pRGBQuad);
	}
}

// This function may have used from CUndoSaveFunction::Do and
// ( for direct call ) CUndoSaveFunction::DoWork ( for to be called
// from CProcessor in foreground and background mode). In first
// mode any synchronization doesn't needed and parameters
// pDibArgument and pDibResult must be NULL.
template<class COLORREPR1, class COLORREPR2> void AttachImage(
	CImage<COLORREPR1> &imgDst, CImage<COLORREPR2> const &img, const CRect *pRect,
	CDIBArgument *pDibArgument, CDIBArgument *pDibResult)
{
	int fsOptions = img.IsTrueColor()?CImageBase::TrueColor:
		CImageBase::GrayScale;
	int x,y,nIncr,x0,cx,y0,cy,x1,y1,dx,dy;
	// y0   - top of rectangle in source image, which will be copied
	//        into destination image
	// cy   - vertical size of rectangle
	// y    - coordinate for source image
	// y-y0 - coordinate for destination image
	// For source image y will be between y0 and y0+cy,
	// for destination image y-y0 will be between 0 and cy.
	if (!pRect)
	{
		x0 = 0;
		y0 = 0;
		x1 = 0;
		y1 = 0;
		cy = dy = img.GetCY();
		cx = dx = img.GetCX();
	}
	else
	{
		x0 = max(min(pRect->left,img.GetCX()-1),0); // coordinates of the first point
		y0 = max(min(pRect->top,img.GetCY()-1),0);  // in the source image
		x1 = pRect->left; // coordinates of the first point in the created image
		y1 = pRect->top;
//		cx = min(pRect->right-pRect->left,img.GetCX()-x0); // size of the area to copy in
//		cy = min(pRect->bottom-pRect->top,img.GetCY()-y0); // the source image
		cx = min(pRect->right,img.GetCX()) - x0; // size of the area to copy in
		cy = min(pRect->bottom,img.GetCY()) - y0; // the source image
		dx = pRect->Width();
		dy = pRect->Height();
	}
	if (imgDst.IsEmpty()    ||
		imgDst.GetCX() !=cx ||
		imgDst.GetCY() !=cy ||
		imgDst.IsTrueColor() != img.IsTrueColor())
	{
		imgDst.Create(dx,dy,fsOptions);
	}
	y = y0 + cy - 1;

	nIncr = imgDst.GetIncr();
	size_t iLineWid = nIncr*sizeof(COLORREPR1)*(cx);

	for (; y >= y0; y--)
	{
//		if (pDibArgument) pDibArgument->WaitForReady(y);
		COLORREPR2 *pSrc = img[y];
		COLORREPR1 *pDst = imgDst[y-y1];
		if( sizeof(COLORREPR1) == sizeof(COLORREPR2) )
			memcpy( &pDst[x0*nIncr-x1*nIncr], &pSrc[x0*nIncr], iLineWid );
		else
			for (x = x0 * nIncr; x < ( x0 + cx ) * nIncr; x += nIncr)
			{
				int xd = x - x1 * nIncr;
				if (img.IsTrueColor())
				{
					pDst[xd]   = CImage<COLORREPR1>::Convert(pSrc[x]);
					pDst[xd+1] = CImage<COLORREPR1>::Convert(pSrc[x+1]);
					pDst[xd+2] = CImage<COLORREPR1>::Convert(pSrc[x+2]);
				}
				else
				{
					pDst[xd] = CImage<COLORREPR1>::Convert(pSrc[x]);
				}
			}
//		if (pDibResult) pDibResult->SetReady(y-y0);
	}
//	if (pDibResult) for (; y>=0; y--) pDibResult->SetReady(y);
}


template <class COLORREPR> void CImage<COLORREPR>::Attach8( CImage<BYTE> const &Image, CRect const &Rect )
{
	AttachImage(*this,Image,Rect==NORECT?NULL:&Rect,NULL,NULL);
	if (!IsEmpty()&&!IsTrueColor()&&sizeof(COLORREPR)==1)
	{
		ReplacePalette(Image.GetRGBQuad());
	}
}

template <class COLORREPR> void CImage<COLORREPR>::Attach16( CImage<WORD> const &Image, CRect const &Rect )
{
	AttachImage(*this,Image,Rect==NORECT?NULL:&Rect,NULL,NULL);
}

// tested, see AttachNoCopy
void CImage<BYTE>::Assign8( CImage<BYTE>& Image )
{
	AttachNoCopy(Image);
	Image.DeInit();
	m_fNotFree = FALSE;
}

void CImage<WORD>::Assign16( CImage<WORD>& Image )
{
	AttachNoCopy(Image);
	Image.DeInit();
	m_fNotFree = FALSE;
}

// tested, see Attach
void CImage<BYTE>::Assign16( CImage<WORD>& Image )
{
	Attach16(Image);
	Image.Free();
}

// tested, see Attach
void CImage<WORD>::Assign8( CImage<BYTE>& Image )
{
	Attach8(Image);
	Image.Free();
}

// tested for gray scale and true color images both 8- and 16-bit
// call stack:
//  CImage<unsigned char>::AttachNoCopy(const CImage<unsigned char> & {CImage<unsigned char>}) line 183
//  CImage<unsigned char>::operator=(CImage<unsigned char> & {CImage<unsigned char>}) line 162
//  CFrame::DoPaintImage(CDC & {hDC=0x000005fe attrib=0x000005fe}, CRect {top=0 bottom=182 left=0 right=476}) line 878
template <class COLORREPR> void CImage<COLORREPR>::AttachNoCopy( const CImage& Image )
{
	Free();
	m_cx = Image.m_cx;
	m_cy = Image.m_cy;
	m_iCols = Image.m_iCols;
	m_iIncr = Image.m_iIncr;
	m_lpbi = Image.m_lpbi;
	m_pRGBQuad = Image.m_pRGBQuad;
	m_pBits = Image.m_pBits;
	m_nAllocSize = Image.m_nAllocSize;
	m_Rows.RemoveAll();
	for (int i = 0; i< Image.m_Rows.GetSize(); i++)
	{
		COLORREPR *pColor = Image.m_Rows.GetAt(i);
		m_Rows.Add(pColor);
	}
	m_pPalette = Image.m_pPalette;
	m_fNotFree = TRUE;
}

// tested: tc8->gs8,  tc8->tc8,  gs8->gs8,  gs8->tc8
//         tc16->gs8, tc16->tc8, gs16->gs8, gs16->tc8
// all also when inserted image more than old
// call stack:
//  CImage<unsigned char>::Attach(void * 0x004310c8) line 195
//  CImgView::OnEditPaste() line 506
template <class COLORREPR> void CImage<COLORREPR>::Attach( HGLOBAL hDib )
{
	void *p  = GlobalLock(hDib);
	Attach(0,p);
	GlobalUnlock(hDib);
}

// Tested in truecolor and grayscale 8-bit
// Call stack:
//  CImage<unsigned char>::Attach(int 319240, void * 0x007b0000) line 174
//  CImage<unsigned char>::ReadBMP(CFile & {hFile=3 name="C:\bitmaps\Flora.bmp"}) line 649
//  CImage<unsigned char>::Read(char * 0x0065f4a8) line 669
//  CImgDoc::OnOpenDocument(char * 0x0065f4a8) line 104

template <class COLORREPR> void CImage<COLORREPR>::AttachNoCopy( LPBITMAPINFOHEADER lpbi, CRect *pRect /* = NULL */ )
{
	int nSize = CImage<COLORREPR>::GetDataSize( (short)lpbi->biWidth, 
		(short)lpbi->biHeight, CImageBase::IsTrueColor(lpbi->biBitCount)?TrueColor:GrayScale );
	AttachNoCopy( nSize, lpbi, pRect );
}

template <class COLORREPR> void CImage<COLORREPR>::AttachNoCopy( int nSize, void *pData, CRect *pRect /* = NULL */ )
{
	int n;
	TestHeader((BITMAPINFOHEADER *)pData);
	Free();
	m_lpbi  = (BITMAPINFOHEADER *)pData;
	if ( ( n = GetPaletteEntriesNum(m_lpbi) ) != 0)
	{
		m_pRGBQuad = (RGBQUAD *)(m_lpbi+1);
		m_pBits = (COLORREPR *)(m_pRGBQuad+n);
	}
	else
	{
		m_pRGBQuad = NULL;
		m_pBits = (COLORREPR *)(m_lpbi+1);
	}
	m_nAllocSize = nSize;
	ImportHeader(m_lpbi, TRUE);
	AllocRows();
	DoIsGSPalette();
	m_fNotFree = TRUE;
}

template <class COLORREPR> void CImage<COLORREPR>::Attach( int nSize,
	void *pData, CRect *pRect /* = NULL */, BOOL bAsIs /* = FALSE */ )
{
	BITMAPINFOHEADER *pHeader;
	RGBQUAD *pPalBits;
	BYTE *pDataBits;
	int fsOptions;
	pHeader  = (BITMAPINFOHEADER *)pData;
	pPalBits = (RGBQUAD *)(pHeader+1);
	pDataBits = (BYTE *)(pPalBits+GetPaletteEntriesNum(pHeader));

	int fsOldOptions = CImageBase::IsTrueColor(pHeader->biBitCount)?TrueColor:GrayScale;
	fsOptions = ImportHeader(pHeader, bAsIs);
	
	AllocBuffer( GetDataSize(m_iCols, m_cy, fsOptions), m_cy, fsOptions );
	AllocRows();
	InitHeader(fsOptions);
	if (m_pRGBQuad)
		memcpy(m_pRGBQuad, pPalBits, GetPaletteSize());
	DoIsGSPalette();
	if( fsOptions == fsOldOptions )
		memcpy(m_pBits,pDataBits,GetBitsSize());
	else
	{
		register short x, y;
		if( !(fsOptions&Color16Bit) && !(fsOldOptions&Color16Bit) )
		{
			C8Image imgOld;
			register BYTE *pNew;
			register BYTE *pOld;
			imgOld.AttachNoCopy( pHeader );
			if( (fsOldOptions&TrueColor) == GrayScale &&
				(fsOptions&TrueColor) == TrueColor)
			{
				for( y=0; y<m_cy; y++ )
				{
					pNew = (BYTE*)operator[]( y );
					pOld = imgOld[y];
					for( x=0; x<m_cx; x++ )
					{
						pNew[x*3] = pPalBits[pOld[x]].rgbBlue;
						pNew[x*3+1] = pPalBits[pOld[x]].rgbGreen;
						pNew[x*3+2] = pPalBits[pOld[x]].rgbRed;
					}
				}
			}
		}
		
	}
}


// tested for true color and gray scale 8- and 16-bit
template <class COLORREPR> HGLOBAL CImage<COLORREPR>::GetCopyHandle(int nPalEntries, const RGBQUAD *pPalette)
{
	long nSize = GetDataSize();
	HGLOBAL hDib = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,nSize);
	if (!hDib) AfxThrowMemoryException();
	void *p  = GlobalLock(hDib);
	memcpy(p,m_lpbi,nSize);
	return hDib;
}

// tested
template <class COLORREPR> void CImage<COLORREPR>::Free()
{
	DestroyPalette();
	DeallocBuffer();
	DeInit();
}

template <class COLORREPR> void CImage<COLORREPR>::CreateHisto(CHisto& Histo,
	CImageBase *pimgMask, CRect rc) const
{
	int i,j,nColorsNum;
	COLORREPR *pRow;
	BOOL fEmpty = !pimgMask||pimgMask->IsEmpty();
	CRect r(rc);
	C8Image *pmsk = (C8Image *)pimgMask;
	if (rc==NORECT)
	{
		r.right = m_cx;
		r.bottom = m_cy;
	}
	else
	{
		r.left  = max(0,rc.left);
		r.top   = max(0,rc.top);
		r.right = min(m_cx,rc.right);
		r.bottom= min(m_cy,rc.bottom);
	}
	nColorsNum = GetColorsNum();
	Histo.InitHisto(nColorsNum,IsTrueColor());
	for (i = r.top; i < r.bottom; i++)
	{
		pRow = (*this)[i] + r.left * GetIncr();
		for (j = r.left; j < r.right; j++)
		{
			if (fEmpty||(*pmsk)[i - r.top][j - r.left] == 255)
			{
				Histo.m_pData0[GetColor(*pRow)]++;
				if (IsTrueColor())
				{
					Histo.m_pData1[GetColor(pRow[1])]++;
					Histo.m_pData2[GetColor(pRow[2])]++;
					int t = SBright(GetColor(pRow[0]),
						GetColor(pRow[1]),GetColor(pRow[2]));
					if (t == 84)
						t = 84;
					Histo.m_pData[SBright(GetColor(pRow[0]),
						GetColor(pRow[1]),GetColor(pRow[2]))]++;
					pRow += GetIncr();
				}
				else pRow++;
			}
			else pRow += GetIncr();
		}
	}
	Histo.m_nMax0 = Histo.m_nMax1 = Histo.m_nMax2 =
		Histo.m_nMaxBright = 0;
	for (i = 0; i < nColorsNum; i++)
	{
		if (Histo.m_pData0[i] > Histo.m_nMax0)
			Histo.m_nMax0 = Histo.m_pData0[i];
		if (IsTrueColor())
		{
			if (Histo.m_pData1[i] > Histo.m_nMax1)
				Histo.m_nMax1 = Histo.m_pData1[i];
			if (Histo.m_pData2[i] > Histo.m_nMax2)
				Histo.m_nMax2 = Histo.m_pData2[i];
			int br = SBright(Histo.m_pData2[i], Histo.m_pData1[i],
				Histo.m_pData0[i]);
			if (br > Histo.m_nMaxBright)
				Histo.m_nMaxBright = br;
		}
	}
	Histo.CalcSum();
#if defined(_DEBUG)
	for (i = 0; i < nColorsNum; i++)
		if (Histo.m_pData[i]) break;
	if (i == nColorsNum)AfxDebugBreak();
#endif
}

// not tested yet
BOOL CImage<BYTE>::Draw( CDC *pDC, short x, short y )
{
	if( IsEmpty() )
		return FALSE;
	BOOL bReturn;
	CPalette *pOldPal = NULL, *pPal = GetPalette();
	if( pPal )
		pOldPal = pDC->SelectPalette( pPal, FALSE );
	pDC->RealizePalette();

	bReturn = SetDIBitsToDevice( pDC->m_hDC,    // hDC
								   x,           // DestX
								   y,           // DestY
								   m_cx,		// nDestWidth
								   m_cy,		// nDestHeight
								   0,           // SrcX
								   0,			// SrcY
								   0,           // nStartScan
								   m_cy,		// nNumScans
								   GetBitsPtr(),             // lpBits
								   (LPBITMAPINFO)m_lpbi,     // lpBitsInfo
								   DIB_RGB_COLORS);          // wUsage	
	if( pOldPal )
		 pDC->SelectPalette( pOldPal, TRUE );
	return bReturn;
}

// tested
BOOL CImage<BYTE>::Draw( CDC *pDC, CRect rcSrc, CRect rcDest, CPalette *pPal, BOOL bForseStretch )
{
	if( IsEmpty() )
		return FALSE;
	BOOL bReturn;

//	if(pPal) CopyPaletteData(pPal);

//	if(!pPal)
		pPal = GetPalette();
	CPalette *pOldPal = NULL;
	if( pPal )
		pOldPal = pDC->SelectPalette( pPal, TRUE );
	pDC->RealizePalette();

	if( rcSrc.Height()==rcDest.Height() &&
		rcSrc.Width()==rcDest.Width() 
		&& !bForseStretch )
		 bReturn = SetDIBitsToDevice(	//no zoom
				pDC->m_hDC,				// hDC
				max(rcDest.left, 0),    // DestX
				max(rcDest.top, 0),     // DestY
				min(m_cx, rcDest.Width()),		// nDestWidth
				min(m_cy, rcDest.Height()),		// nDestHeight
				rcSrc.left,				// SrcX
				m_cy- rcSrc.bottom,				// SrcY
				0,						// nStartScan
				m_cy,					// nNumScans
				GetBitsPtr(),           // lpBits
				(LPBITMAPINFO)m_lpbi,    // lpBitsInfo
				DIB_RGB_COLORS);        // wUsage	
	else
	{
		if (pDC->IsPrinting()||bForseStretch)
		{
			bReturn = StretchDIBits(	//zoom
					pDC->m_hDC,				// hDC
					rcDest.left,	    // DestX
					rcDest.top,      // DestY
					rcDest.Width(),		// nDestWidth
					rcDest.Height(),		// nDestHeight
					max(rcSrc.left, 0),				// SrcX
					max(m_cy-rcSrc.bottom, 0),	// SrcY
					min(rcSrc.Width(), m_cx),		// nSrcWidth
					min(rcSrc.Height(), m_cy),		// nSrcHeight
					GetBitsPtr(),           // lpBits
					(LPBITMAPINFO)m_lpbi,   // lpBitsInfo
					DIB_RGB_COLORS,		// wUsage	
					SRCCOPY);        
		}
		else
		{
			
			DrawDibDraw(hdd.hdd,
					pDC->m_hDC,				// hDC
					rcDest.left,	    // DestX
					rcDest.top,      // DestY
					rcDest.Width(),		// nDestWidth
					rcDest.Height(),		// nDestHeight
					(LPBITMAPINFOHEADER)m_lpbi,   // lpBitsInfo
					GetBitsPtr(),           // lpBits
					max(rcSrc.left, 0),				// SrcX
//is it true (m_cy-rcSrc.bottom)? possible, rcSrc.top
					//max(m_cy-rcSrc.bottom, 0),	// SrcY
					rcSrc.top,
					min(rcSrc.Width(), m_cx),		// nSrcWidth
					min(rcSrc.Height(), m_cy),		// nSrcHeight
					0); // Flags
		}
	}
	if( pOldPal )		  
		 pDC->SelectPalette( pOldPal, TRUE );
	DestroyPalette();
	return bReturn;
}

BOOL CImage<WORD>::Draw( CDC *pDC, short x, short y )
{
	return FALSE;
}

BOOL CImage<WORD>::Draw( CDC *pDC, CRect rcSrc, CRect rcDest, CPalette *pPal, BOOL bForseDraw )
{
	return FALSE;
}

// protected members - functions

// tested
template <class COLORREPR> void CImage<COLORREPR>::AllocBuffer( int nSize, unsigned nRows, int fsOptions )
{
	if (m_lpbi&&m_nAllocSize>=nSize)
	{
		memset(m_lpbi, 0, m_nAllocSize);
		return;
	}
	DeallocBuffer(); // m_lpbi, m_pRGBQuad and m_pBits will be garbage
	m_lpbi = (BITMAPINFOHEADER*)AllocMem(nSize);
	if (fsOptions&TrueColor||sizeof(COLORREPR)==2)
	{
		m_pRGBQuad = NULL;
		m_pBits = (COLORREPR *)(m_lpbi+1);
	}
	else
	{
		m_pRGBQuad = (RGBQUAD *)(m_lpbi+1);
		m_pBits = (COLORREPR *)(m_pRGBQuad+256);
	}
	DoIsGSPalette();
	m_fNotFree = FALSE;// memory allocated here and must be freed here
}

// tested
template <class COLORREPR> void CImage<COLORREPR>::AllocRows()
{
	COLORREPR *pRow = m_pBits;
	m_Rows.RemoveAll();
	m_Rows.Add(pRow);
	m_Rows.Add(pRow);
	for (int i=0; i<m_cy; i++)
	{
//		m_Rows.Add(pRow);
		m_Rows.InsertAt(0,pRow);
		pRow += m_iCols;
	}
	pRow -= m_iCols;
//	m_Rows.Add(pRow);
	m_Rows.InsertAt(0,pRow);
}

// tested
template <class COLORREPR> void CImage<COLORREPR>::DeallocBuffer()
{
	if (!m_fNotFree&&m_lpbi)
		FreeMem(m_lpbi);
	m_lpbi = NULL;
	m_pBits = NULL;
	m_pRGBQuad = NULL;
	m_nAllocSize = 0;
	m_Rows.RemoveAll();
}

// tested
template <class COLORREPR> void CImage<COLORREPR>::DeInit()
{
	m_cx = m_cy = 0;
	m_iCols = m_iIncr = 0;
	m_nAllocSize = 0;
	m_lpbi = NULL;
	m_pRGBQuad = NULL;
	m_pBits = NULL;
	if( m_Rows.GetSize() )
		m_Rows.RemoveAll();
	m_pPalette = NULL;
	m_fNotFree = FALSE;
	m_bGSPalette = TRUE;
}

// tested
template <class COLORREPR> int CImage<COLORREPR>::ImportHeader(BITMAPINFOHEADER *pHeader, BOOL bNotChangeSize)
{
	int fsOptions;
	m_cx = (short)pHeader->biWidth;
	m_cy = (short)pHeader->biHeight;
	if( IsTrueColor(pHeader->biBitCount) )
		fsOptions = TrueColor;
	else
		if( pHeader->biBitCount != 8 || IsGSPalette(pHeader) || bNotChangeSize )
			fsOptions = GrayScale;
		else
			fsOptions = TrueColor;
	m_iCols = CalcColsNum(m_cx,fsOptions);
	m_iIncr = CalcInc(fsOptions);
	return fsOptions;
//	memcpy(m_lpbi,pHeader,sizeof(BITMAPINFOHEADER));
}

// not tested yet
template <class COLORREPR> void CImage<COLORREPR>::ExportHeader(BITMAPINFOHEADER *pHeader) const
{
	pHeader->biSize = sizeof(*pHeader);
	pHeader->biWidth = m_cx;
	pHeader->biHeight = m_cy;
	pHeader->biPlanes = 1;
	pHeader->biBitCount = m_lpbi->biBitCount/sizeof(COLORREPR);
	pHeader->biCompression = BI_RGB;
	pHeader->biSizeImage = m_iCols*m_cy;
	pHeader->biXPelsPerMeter = 0;
	pHeader->biYPelsPerMeter = 0;
	pHeader->biClrUsed = IsTrueColor(pHeader->biBitCount)?0:256;
	pHeader->biClrImportant = 0;
}

// tested
template <class COLORREPR> void CImage<COLORREPR>::InitHeader(int fsOptions)
{
	m_lpbi->biSize = sizeof(BITMAPINFOHEADER);
	m_lpbi->biWidth = m_cx;
	m_lpbi->biHeight = m_cy;
	m_lpbi->biPlanes = 1;
	m_lpbi->biBitCount = (fsOptions&TrueColor?24:8)*sizeof(COLORREPR);
	m_lpbi->biCompression = BI_RGB;
	m_lpbi->biSizeImage = GetBitsSize();
	m_lpbi->biXPelsPerMeter = 0;
	m_lpbi->biYPelsPerMeter = 0;
	m_lpbi->biClrUsed = (fsOptions&TrueColor)||(sizeof(COLORREPR)==2)?0:256;
	m_lpbi->biClrImportant = 0;
}

//template <class COLORREPR> BOOL CImage<COLORREPR>::InitCnvData()
//{
//}

//template <class COLORREPR> void CImage<COLORREPR>::DeInitCnvData()
//{
//}

// tested
template <class COLORREPR> int CImage<COLORREPR>::GetPaletteEntriesNum( const BITMAPINFOHEADER *pBI )
{
	if (pBI->biBitCount==1)
	{
		return 2;
	}
	else if (pBI->biBitCount==4)
	{
		return 16;
	}
	else if (pBI->biBitCount==8)
	{
		if( pBI->biClrUsed )
			return pBI->biClrUsed;
		else
			return 256;
	}
	else if (pBI->biBitCount==16)
	{
		if (pBI->biCompression==BI_RGB)
			return 0;
		else if (pBI->biCompression==BI_BITFIELDS)
			return 3;
		else
			return 0;
	}
	else if (pBI->biBitCount==32)
	{
		if (pBI->biCompression==BI_RGB)
			return 0;
		else if (pBI->biCompression==BI_BITFIELDS)
			return 3;
		else
			return 0;
	}
	else return 0;
}

// not tested yet
template<class COLORREPR> CPalette *CImage<COLORREPR>::GetPalette()
{
	if( IsEmpty() )
		return NULL;
	if( IsTrueColor() )
		return NULL;
	if (!m_pPalette)
		m_pPalette = new CPalette;
	if( m_pPalette->m_hObject == NULL )
		CreatePalette();
	if( m_pPalette->m_hObject == NULL )
		return NULL;
	else
		return m_pPalette;
}

template<class COLORREPR> void CImage<COLORREPR>::InitPaletteBits(RGBQUAD *pPalBits) const
{
	if (IsTrueColor() || sizeof(COLORREPR)==2) return;
	for (int i=0; i<256; i++)
	{
		pPalBits[i].rgbRed = i;
		pPalBits[i].rgbGreen = i;
		pPalBits[i].rgbBlue = i;
		pPalBits[i].rgbReserved = 0;
	}
}

// not tested yet
template<class COLORREPR> void CImage<COLORREPR>::CreatePalette()
{
	if( IsEmpty() )
		return;
	TestHeader( m_lpbi );
	if( IsTrueColor() )
		return;
	LPBITMAPINFO lpbmi = (LPBITMAPINFO)m_lpbi;
	short n = GetPaletteEntriesNum( m_lpbi );
	if( n == 0 )
		return;
	LPLOGPALETTE lpPal = (LPLOGPALETTE)AllocMem( sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*n );
	if( !lpPal )
		return;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = n;
	for( register short i = 0; i < (int)n; i++)
	{
		lpPal->palPalEntry[i].peRed   = m_pRGBQuad[i].rgbRed;
		lpPal->palPalEntry[i].peGreen = m_pRGBQuad[i].rgbGreen;
		lpPal->palPalEntry[i].peBlue  = m_pRGBQuad[i].rgbBlue;
		lpPal->palPalEntry[i].peFlags = 0;
	}
	m_pPalette->CreatePalette( lpPal );
	FreeMem( lpPal );
}

// tested
template<class COLORREPR> void CImage<COLORREPR>::CopyPaletteData( CPalette *pPal )
{
	if (!m_pRGBQuad) return;
	ASSERT(IsGrayScale());
	int nEnts = pPal->GetEntryCount();
	LPPALETTEENTRY lpEntry;
	lpEntry = (LPPALETTEENTRY)AllocMem(nEnts*sizeof(PALETTEENTRY));
	pPal->GetPaletteEntries(0, nEnts, lpEntry);
	VERIFY(nEnts==GetPaletteEntriesNum());
	for (int i=0; i<nEnts; i++)
	{
		m_pRGBQuad[i].rgbRed = lpEntry[i].peRed;
		m_pRGBQuad[i].rgbGreen = lpEntry[i].peGreen;
		m_pRGBQuad[i].rgbBlue = lpEntry[i].peBlue;
		m_pRGBQuad[i].rgbReserved = 0;
	}
	FreeMem(lpEntry);
	DoIsGSPalette();
}

// tested when no palette
template<class COLORREPR> void CImage<COLORREPR>::DestroyPalette()
{
	if (m_pPalette)
	{
//		ASSERT(IsGrayScale());
		delete m_pPalette;
		m_pPalette = NULL;
	}
	//else ASSERT(IsTrueColor()||IsEmpty());
}

// tested
template<class COLORREPR> void CImage<COLORREPR>::TestHeader( const LPBITMAPINFOHEADER lpbi )
{
	if( lpbi == NULL )
		throw EXCPT_INVALIDIMAGE;
	if( lpbi->biSize != sizeof(BITMAPINFOHEADER) )
		throw EXCPT_INVALIDIMAGE;
	if( lpbi->biCompression != BI_RGB )
		throw EXCPT_INVALIDIMAGE;
	if( (lpbi->biBitCount != 8) && (lpbi->biBitCount != 24) &&
		(lpbi->biBitCount != 10) && (lpbi->biBitCount != 12) &&
		(lpbi->biBitCount != 16) && (lpbi->biBitCount != 30) &&
		(lpbi->biBitCount != 36) && (lpbi->biBitCount != 48) && (lpbi->biBitCount != 32) )
		throw EXCPT_INVALIDIMAGE;
}

void CImage<BYTE>::ConvertHeader()
{
	if (m_lpbi->biBitCount>=10&&m_lpbi->biBitCount<=16)
	{
		m_lpbi->biBitCount = 8;
		m_lpbi->biSizeImage >>= 1;
	}
	else if (m_lpbi->biBitCount>=30&&m_lpbi->biBitCount<=48)
	{
		m_lpbi->biBitCount = 24;
		m_lpbi->biSizeImage >>= 1;
	}
}

void CImage<WORD>::ConvertHeader()
{
	if (m_lpbi->biBitCount==8||m_lpbi->biBitCount==24)
	{
		m_lpbi->biBitCount <<= 1;
		m_lpbi->biSizeImage <<= 1;
		m_lpbi->biClrUsed = 0;
		m_lpbi->biClrImportant = 0;
	}
}


#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

#ifdef _MAC
#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))
void ByteSwapHeader(BITMAPFILEHEADER* bmiHeader);
void ByteSwapInfo(LPSTR lpHeader, BOOL fWin30Header);
#endif

// tested
template<class COLORREPR> void CImage<COLORREPR>::ReadBMP( CFile &file )
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;

	DeInit();

	if(file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		AfxThrowFileException( CFileException::generic );
#ifdef _MAC
	ByteSwapHeader(&bmfHeader);
#endif

	if (bmfHeader.bfType == 0)
		return;	//case if ni image stored;
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		throw EXCPT_INVALIDIMAGE;

	dwBitsSize = bmfHeader.bfSize - sizeof(BITMAPFILEHEADER);
	// Read data
	void *pData = AllocMem(dwBitsSize);
	if ( file.Read(pData, dwBitsSize) != dwBitsSize)
	{
		FreeMem(pData);
		AfxThrowFileException( CFileException::generic );
	}
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)pData) = SWAPLONG(*((LPDWORD)pData));
	// Now swap the rest of the structure
	ByteSwapInfo(pData, IS_WIN30_DIB(pData));
#endif
	Attach(dwBitsSize,pData);

}

// tested
template<class COLORREPR> void CImage<COLORREPR>::Read( LPCSTR szFileName )
{
	CString s = szFileName;
	s = s.Right(4);
	s.MakeUpper();
	if( s != ".BMP" && s != ".DIB" )
		throw EXCPT_INVALIDIMAGE;

	Free();
	CFile file;
	CFileException fe;
	if (!file.Open(szFileName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		fe.ReportError();
		return;
	}

	ReadBMP( file );
}

template<class COLORREPR> BYTE *CImage<COLORREPR>::m_pCnvBuffer = NULL;
template<class COLORREPR> double CImage<COLORREPR>::m_CnvKoef = 1.0;
template<class COLORREPR> BOOL CImage<COLORREPR>::m_fUseGC = FALSE;

template<class COLORREPR> void CImage<COLORREPR>::InitColorConv()
{
	ASSERT(m_pCnvBuffer == NULL);
	int nSize = sizeof(COLORREPR)==2?0x10000:0x100;
	m_pCnvBuffer = (BYTE *)AllocMem(nSize*sizeof(COLORREPR));
	CalcConvData();
}

#pragma optimize( "", off )

template<class COLORREPR> void CImage<COLORREPR>::CalcConvData()
{
	int nMax = sizeof(COLORREPR)==2?0xFFFF:0xFF;
	double CnvMul = nMax / pow((double)nMax, m_CnvKoef);

	if (m_fUseGC)
	{
		for (int i = 0; i <= nMax; i++)
		{
			COLORREPR b = (COLORREPR)(CnvMul * pow((double)i, m_CnvKoef));
			m_pCnvBuffer[i] = AsByteEx(b);
		}
	}
	else
	{
		for (int i = 0; i <= nMax; i++)
		{
			m_pCnvBuffer[i] = AsByteEx(i);
		}
		if (sizeof(COLORREPR) == 1)
			m_pCnvBuffer[255] = 255;
	}
}
#pragma optimize( "", on )

template<class COLORREPR> void CImage<COLORREPR>::DeInitColorConv()
{
	ASSERT(m_pCnvBuffer != NULL);
	FreeMem(m_pCnvBuffer);
	m_pCnvBuffer = NULL;
}


// tested
CImageBase *CImageBase::MakeFromFile( CFile &file )
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;


	/*
	 * Go read the DIB file header and check if it's valid.
	 */
	if(file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		AfxThrowFileException( CFileException::generic );

#ifdef _MAC
	ByteSwapHeader(&bmfHeader);
#endif
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		throw EXCPT_INVALIDIMAGE;

	dwBitsSize = bmfHeader.bfSize - sizeof(BITMAPFILEHEADER);

	// Read data
	void *pData = AllocMem(dwBitsSize);
	if ( file.Read(pData, dwBitsSize) != dwBitsSize)
	{
		FreeMem(pData);
		AfxThrowFileException( CFileException::generic );
	}
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)pData) = SWAPLONG(*((LPDWORD)pData));
	// Now swap the rest of the structure
	ByteSwapInfo(pData, IS_WIN30_DIB(pData));
#endif

	// Create image, depending from underlying type.
	CImageBase *pImageBase;
	if (((LPBITMAPINFOHEADER)pData)->biBitCount==8||((LPBITMAPINFOHEADER)pData)->biBitCount==24)
	{
		// 8-bit image
 		pImageBase = new C8Image;
		((C8Image *)pImageBase)->AttachNoCopy(dwBitsSize,pData);
		((C8Image *)pImageBase)->m_fNotFree = FALSE;
	}
	else if (Is16BitColor(((LPBITMAPINFOHEADER)pData)->biBitCount))		
	{
		// 16-bit image
		pImageBase = new C16Image;
		((C16Image *)pImageBase)->AttachNoCopy(dwBitsSize,pData);
		((C8Image *)pImageBase)->m_fNotFree = FALSE;
	}
	else throw EXCPT_INVALIDIMAGE;
	return pImageBase;
}

// tested
CImageBase *CImageBase::MakeFromFile( const char *pszFileName )
{
	CString s = pszFileName;
	s = s.Right(4);
	s.MakeUpper();
	if( s != ".BMP" && s != ".DIB" )
		throw EXCPT_INVALIDIMAGE;

	CFile file;
	CFileException fe;
	if (!file.Open(pszFileName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		fe.ReportError();
		return NULL;
	}

	return MakeFromFile( file );
}

CImageBase *CImageBase::CreateNew( unsigned iBitCount, int cx, int cy, COLORREF clrDef )
{
	int fsOptions = 0;
	if( IsTrueColor(iBitCount) )
		fsOptions |= TrueColor;
	if( Is16BitColor(iBitCount) )
		fsOptions |= Color16Bit;
	CImageBase *pimg;
	if( iBitCount == 8 || iBitCount == 24 )
		pimg = new C8Image( cx, cy, fsOptions );
	else
		pimg = new C16Image( cx, cy, fsOptions );
	if (clrDef) pimg->Fill(clrDef);
	return pimg;

}

CImageBase *CImageBase::MakeFromBits( LPBITMAPINFOHEADER lpbi, BOOL fCopy )
{
	if (lpbi->biBitCount==8||lpbi->biBitCount==24)
	{
		// 8-bit image
		C8Image *pImage;
		pImage = new C8Image;
		if (fCopy)
		{
			try
			{
				pImage->Attach(0,lpbi);
			} 
			catch(...)
			{
				safedelete( pImage );
//				AfxMessageBox( IDP_INVALID_IMAGE );
				AfxMessageBox( "Invalid image" );
				return NULL;
			}
		}
		else 
			pImage->AttachNoCopy(0,lpbi);
		return pImage;
	}
	else if (Is16BitColor(lpbi->biBitCount))
	{
		// 16-bit image
		C16Image *pImage;
		pImage = new C16Image;
		if (fCopy)
		{
			try
			{
				pImage->Attach(0,lpbi);
			} 
			catch(...)
			{
				safedelete( pImage );
//				AfxMessageBox( IDP_INVALID_IMAGE );
				AfxMessageBox( "Invalid image" );

				return NULL;
			}
		}
		else 
			pImage->AttachNoCopy(0,lpbi);
		return pImage;
	}
	else throw EXCPT_INVALIDIMAGE;
	ASSERT(FALSE);
	return NULL;
}

CImageBase *CImageBase::MakeFromBits( HGLOBAL h)
{
	void *p  = GlobalLock(h);
	CImageBase *pimg = MakeFromBits((LPBITMAPINFOHEADER)p,TRUE);
	GlobalUnlock(h);
	return pimg;
}


template<class COLORREPR> void CImage<COLORREPR>::WriteBMP( CFile &file )
{
	BITMAPFILEHEADER bmfHeader;

	// Fill bmfHeader by values

	DWORD dwBitsSize = 0;

	if( GetBI() )
	{
		dwBitsSize = GetDataSize();
		bmfHeader.bfType = DIB_HEADER_MARKER;
		bmfHeader.bfSize = sizeof(bmfHeader) + dwBitsSize;
		bmfHeader.bfReserved1 = 0;
		bmfHeader.bfReserved2 = 0;
		bmfHeader.bfOffBits   = sizeof(bmfHeader) + sizeof(BITMAPINFOHEADER) + GetPaletteSize();
	}
	else
	{
		ZeroMemory( &bmfHeader, sizeof( bmfHeader ) );
	}

#if defined(_MAC)
	ByteSwapHeader(&bmfHeader);
#endif

	// Write the header
	try {
		file.Write((LPSTR)&bmfHeader, sizeof(bmfHeader));
	}
	catch (CFileException e)
	{
		file.Close();
		CFile::Remove(file.GetFilePath());
		throw;
	}

	if( !GetBI() )
		return;

	// Write data
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)m_lpbi) = SWAPLONG(*((LPDWORD)m_lpbi));
	// Now swap the rest of the structure
	ByteSwapInfo(m_lpbi, IS_WIN30_DIB(m_lpbi));
#endif
	try {
		file.Write(m_lpbi,dwBitsSize);
	}
	catch (CFileException e)
	{
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)m_lpbi) = SWAPLONG(*((LPDWORD)m_lpbi));
	// Now swap the rest of the structure
	ByteSwapInfo(m_lpbi, IS_WIN30_DIB(m_lpbi));
#endif
	throw;
	}
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)m_lpbi) = SWAPLONG(*((LPDWORD)m_lpbi));
	// Now swap the rest of the structure
	ByteSwapInfo(m_lpbi, IS_WIN30_DIB(m_lpbi));
#endif
}


template<class COLORREPR> void CImage<COLORREPR>::Write( LPCSTR pszFileName )
{
	CString s = pszFileName;
	s = s.Right(4);
	s.MakeUpper();
	if( s != ".BMP" && s != ".DIB" )
		throw EXCPT_INVALIDIMAGE;

	CFile file;
	CFileException fe;
	if (!file.Open(pszFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &fe))
	{
		fe.ReportError();
		return;
	}

	WriteBMP( file );
}



#ifdef _MAC
void ByteSwapHeader(BITMAPFILEHEADER* bmfHeader)
{
	bmfHeader->bfType = SWAPWORD(bmfHeader->bfType);
	bmfHeader->bfSize = SWAPLONG(bmfHeader->bfSize);
	bmfHeader->bfOffBits = SWAPLONG(bmfHeader->bfOffBits);
}


void ByteSwapInfo(LPSTR lpHeader, BOOL fWin30Header)
{
	// Note this doesn't swap the bcSize/biSize field.  It assumes that the
	// size field was swapped during read or while setting the fWin30Header
	// flag.

	if (fWin30Header)
	{
		LPBITMAPINFOHEADER lpBMIH = &(LPBITMAPINFO(lpHeader)->bmiHeader);

		//lpBMIH->biSize = SWAPLONG(lpBMIH->biSize);
		lpBMIH->biWidth = SWAPLONG(lpBMIH->biWidth);
		lpBMIH->biHeight = SWAPLONG(lpBMIH->biHeight);
		lpBMIH->biPlanes = SWAPWORD(lpBMIH->biPlanes);
		lpBMIH->biBitCount = SWAPWORD(lpBMIH->biBitCount);
		lpBMIH->biCompression = SWAPLONG(lpBMIH->biCompression);
		lpBMIH->biSizeImage = SWAPLONG(lpBMIH->biSizeImage);
		lpBMIH->biXPelsPerMeter = SWAPLONG(lpBMIH->biXPelsPerMeter);
		lpBMIH->biYPelsPerMeter = SWAPLONG(lpBMIH->biYPelsPerMeter);
		lpBMIH->biClrUsed = SWAPLONG(lpBMIH->biClrUsed);
		lpBMIH->biClrImportant = SWAPLONG(lpBMIH->biClrImportant);
	}
	else
	{
		LPBITMAPCOREHEADER lpBMCH = &(LPBITMAPCOREINFO(lpHeader)->bmciHeader);

		lpBMCH->bcWidth = SWAPWORD(lpBMCH->bcWidth);
		lpBMCH->bcHeight = SWAPWORD(lpBMCH->bcHeight);
		lpBMCH->bcPlanes = SWAPWORD(lpBMCH->bcPlanes);
		lpBMCH->bcBitCount = SWAPWORD(lpBMCH->bcBitCount);
	}
}

#endif

template<class COLORREPR> void CImage<COLORREPR>::ReplacePalette( const RGBQUAD *pRGBQuads)
{
	int nEntries;
	if (!pRGBQuads) return;
	nEntries = GetPaletteEntriesNum();
	if(nEntries)
	{
		ASSERT(m_pRGBQuad);
		memcpy(m_pRGBQuad, pRGBQuads, nEntries*sizeof(RGBQUAD));
	}
	DoIsGSPalette();
}

BOOL CImageBase::IsGSPalette( LPBITMAPINFOHEADER lpbi )
{
	if( lpbi->biBitCount != 8 )
		return FALSE;
	if( lpbi->biClrUsed != 0 && lpbi->biClrUsed != 256 )
		return FALSE;
	RGBQUAD *pRGB = (RGBQUAD*)(lpbi+1);
	for( int i=0; i<256; i++ )
		if( pRGB[i].rgbRed != i || pRGB[i].rgbGreen != i || pRGB[i].rgbBlue != i )
			return FALSE;
	return TRUE;
}



void CImageBase::Serialize( CArchive& ar )
{
}

template<class COLORREPR> void CImage<COLORREPR>::
	Serialize( CArchive& ar )
{
	DWORD	dwBitsSize = 0;

	if( ar.IsStoring() )
	{
		if( IsEmpty() )
		{
			ar<<dwBitsSize;
		}
		else
		{
			dwBitsSize = GetDataSize();
			ar<<dwBitsSize;
			ar.Write( GetBI(), dwBitsSize );
		}
	}
	else
	{
		Free();
		ar>>dwBitsSize;

		if( dwBitsSize )
		{
			LPVOID p = AllocMem( dwBitsSize );
			ar.Read( p, dwBitsSize );
			AttachNoCopy( dwBitsSize, p );
			m_fNotFree = FALSE;
		}
	}
}

template<class COLORREPR> void CImage<COLORREPR>::DoIsGSPalette( )
{
	if( m_lpbi->biBitCount != 8 )
	{
		m_bGSPalette = TRUE;
		return;
	}
	if( m_lpbi->biClrUsed != 0 && m_lpbi->biClrUsed != 256 )
	{
		m_bGSPalette = FALSE;
		return;
	}
	int n = GetPaletteEntriesNum( m_lpbi );
	n = min(n,128);
	for( int i = 0; i < (int)n; i++)
	{
		if (m_pRGBQuad[i].rgbRed != m_pRGBQuad[i].rgbGreen ||
			m_pRGBQuad[i].rgbRed != m_pRGBQuad[i].rgbBlue)
		{
			m_bGSPalette = FALSE;
			return;
		}
	}
	m_bGSPalette = TRUE;
}

template<class COLORREPR> void CImage<COLORREPR>::Fill( COLORREF clr )
{
	if (IsGrayScale())
	{
		COLORREPR br = (COLORREPR)_Bright(GetRValue(clr), GetGValue(clr), GetBValue(clr));
		for (int y = 0; y < GetCY(); y++)
		{
			COLORREPR *p = operator[](y);
			for (int x = 0; x < GetCX(); x++)
			{
				*p++ = br;
			}
		}
	}
	else
	{
		COLORREPR r = (COLORREPR)GetRValue(clr);
		COLORREPR g = (COLORREPR)GetGValue(clr);
		COLORREPR b = (COLORREPR)GetBValue(clr);
		for (int y = 0; y < GetCY(); y++)
		{
			COLORREPR *p = operator[](y);
			for (int x = 0; x < GetCX(); x++)
			{
				*p++ = b;
				*p++ = g;
				*p++ = r;
			}
		}
	}
}

template<class COLORREPR> void CImage<COLORREPR>::AttachRGB555(LPBITMAPINFOHEADER lpbi)
{
	Create((short)lpbi->biWidth, (short)lpbi->biHeight, TrueColor);
	DWORD dwRowSrc = ((lpbi->biWidth*2+1)>>1)<<1;
	WORD *pSrc = ((WORD *)(lpbi+1))+(dwRowSrc*(lpbi->biHeight-1))/2;
	for (int y = 0; y < lpbi->biHeight; y++)
	{
		COLORREPR *pDst = (*this)[y];
		for (int x = 0; x < lpbi->biWidth; x++)
		{
			WORD w = pSrc[x];
			pDst[3*x+2] = Convert((BYTE)((w>>7)&0xFF));
			pDst[3*x+1] = Convert((BYTE)((w>>2)&0xFF));
			pDst[3*x+0] = Convert((BYTE)((w<<3)&0xFF));
		}
		pSrc -= dwRowSrc/2;
	}
}


#pragma warning( disable: 4660 )
template CImage<BYTE>;
template CImage<WORD>;
template void AttachImage(C8Image &imgDst, C8Image const &img, const CRect *pRect,
	CDIBArgument *pDibArgument, CDIBArgument *pDibResult);
template void AttachImage(C8Image &imgDst, C16Image const &img, const CRect *pRect,
	CDIBArgument *pDibArgument, CDIBArgument *pDibResult);
template void AttachImage(C16Image &imgDst, C8Image const &img, const CRect *pRect,
	CDIBArgument *pDibArgument, CDIBArgument *pDibResult);
template void AttachImage(C16Image &imgDst, C16Image const &img, const CRect *pRect,
	CDIBArgument *pDibArgument, CDIBArgument *pDibResult);
#pragma warning( default: 4660 )




#ifdef __VT4__
void DrawImage( CImageBase *pimg, 
			   CDC *pDC, 
			   CRect rcDib, 
			   CRect rcPaint, 
			   BOOL bForceStreatch )
{
	HDC hDC = ::CreateIC( "DISPLAY", 0, 0, 0 );
	BOOL bHiOrTrueColor = ::GetDeviceCaps( hDC, BITSPIXEL )>8;
	::DeleteDC( hDC );

	C8Image	*pimgPaint;
	C8Image	*pimgSrc;

	//work only with 8 or 24 bpp 
	if( pimg->Is16BitColor() )
		return;

	if( pimg->IsEmpty() )
		return;

	pimgSrc = ( C8Image * )pimg;

	CPalette *ppal;

	if( pimg->IsTrueColor() )
		ppal = g_PalWork.GetColorPal();
	else
		ppal = g_PalWork.GetGrayPal();

	if( !bHiOrTrueColor )
	{
		pimgPaint=  new C8Image;

		if( pimg->IsTrueColor() )
			MakePaint8FromRGB( *pimgPaint, pimg, rcDib, 1 );
		else
			MakePaintFromGS( *pimgPaint, pimg, rcDib, 1 );

		rcDib-=rcDib.TopLeft();
	}
	else
		pimgPaint = pimgSrc;


	pimgPaint->Draw( pDC, rcDib, rcPaint, 
		ppal, bForceStreatch );

	if( !bHiOrTrueColor )
		delete pimgPaint;
}

#endif
