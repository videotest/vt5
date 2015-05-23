#include "StdAfx.h"
#include <math.h>
#include "ImageUtil.h"
#include "misc_templ.h"
#include "misc_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0
bool AttachDIBBits(IImage3 *pimg, BITMAPINFOHEADER *pbi, byte *pbyteDIBBits)
{
	ASSERT( pbi );

	int	cx = pbi->biWidth;
	int	cy = pbi->biHeight;
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
	/*else if( nBitCount == 15 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 16 )
		cx4 = (cx*2+3)/4*4;*/
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else
		return false;

	
	long	nPalSize = (nBitCount <= 8) ? 1<<nBitCount : 0;
	
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
		ASSERT( pbytePalette );
		bool	bGrayScale = true;

		pbyteRed = new byte[nPalSize];
		pbyteGreen = new byte[nPalSize];
		pbyteBlue = new byte[nPalSize];

		for( long nEntry = 0; nEntry < nPalSize; nEntry++ )
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
//create a new image
	_bstr_t strCC = bColor?"RGB":"Gray";

	if (pimg->CreateImage( cx, cy, strCC,-1 ) != S_OK)
		return false;
//find the color convertor
	IUnknown	*punkCC = 0;
	pimg->GetColorConvertor( &punkCC );
	sptrIColorConvertor5	sptrC( punkCC );

	if( !punkCC )
		return false;
	punkCC->Release();
//alloc the buffer

	int	nColorsCount = ::GetImagePaneCount( pimg );
	color	**ppcolorBuf = new color*[nColorsCount];

	long	nLineLength = bColor?((cx*3+3)/4*4):((cx+3)/4*4);
	long	nColorCol, nCol, nRow;
//convert image rows
	for( nRow = 0; nRow < cy; nRow++ )
	{
		bool	bFreeFlag = false;

		byte	*pRaw = pbyteDIBData+nRow*cx4;
		byte	*pRGB = 0;
//convert row from raw to RGB DIB
		switch( nBitCount )
		{
		case 1:		//1 bit per pixel image
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			if( bColor )
			{
				pRGB = new byte[nLineLength];
				bFreeFlag = true;

				ASSERT(pbyteRed);
				ASSERT(pbyteGreen);
				ASSERT(pbyteBlue);

				for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
				{
					byte	byteVal = pRaw[nCol];

					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
			}
			else
				pRGB = pRaw;

			break;
		case 16:
			ASSERT( bColor );
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				WORD	wVal = ((WORD*)pRaw)[nCol];

				byte	r = (wVal & 0xF800)>>6;
				byte	g = (wVal & 0x0760)>>3;
				byte	b = (wVal & 0x001F)<<3;

				pRGB[nColorCol+2] = b;
				pRGB[nColorCol+1] = g;
				pRGB[nColorCol+0] = r;
			}

		case 24:		//24 bit per pixel image
			ASSERT(bColor);
			pRGB = pRaw;
			break;
		}
		
		
		for( long nColor = 0; nColor < nColorsCount; nColor++ )
		{
			 pimg->GetRow( cy-nRow-1, nColor, &ppcolorBuf[nColor] );
		}

		if( sptrC->ConvertDIBToImageEx( pRGB, ppcolorBuf, cx, bColor, nColorsCount ) != S_OK )
			return false;

		if( bFreeFlag )
			delete pRGB;
	}

	if( ppcolorBuf )delete ppcolorBuf;
	if( pbyteRed )delete pbyteRed;
	if( pbyteGreen )delete pbyteGreen;
	if( pbyteBlue )delete pbyteBlue;

	return true;
}
#endif

bool AttachDIBBits(IImage3 *pimg, BITMAPINFOHEADER *pbi, byte *pbyteDIBBits)
{
	ASSERT( pbi );
	
	int	cx = pbi->biWidth;
	int	cy = pbi->biHeight;    
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
	else if( nBitCount == 10 || nBitCount == 12 || nBitCount == 16 ) // 16-bit grayscale
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else if( nBitCount == 30 || nBitCount == 36 || nBitCount == 48 )
		cx4 = (cx*6+3)/4*4;
	else
		return false;

	
	long	nPalSize = (nBitCount <= 8) ? 1<<nBitCount : 0;
	
	byte	*pbytePalette = nPalSize?((byte*)pbi)+pbi->biSize:0;
	byte	*pbyteDIBData = ((byte*)pbi)+pbi->biSize+nPalSize*4;

	if( pbyteDIBBits )
		pbyteDIBData = pbyteDIBBits;

	byte	*pbyteRed = 0;
	byte	*pbyteGreen = 0;
	byte	*pbyteBlue = 0;
//is color image
	bool	bColor = nBitCount>16;
//if image has palette, check it is gray scale
	if( nPalSize > 0 )
	{
		ASSERT( pbytePalette );
		bool	bGrayScale = true;

		pbyteRed = new byte[nPalSize];
		pbyteGreen = new byte[nPalSize];
		pbyteBlue = new byte[nPalSize];

		for( long nEntry = 0; nEntry < nPalSize; nEntry++ )
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
//create a new image
	_bstr_t strCC = bColor?"RGB":"Gray";

	if (pimg->CreateImage( cx, cy, strCC, -1 ) != S_OK)
		return false;
//find the color convertor
	IUnknown	*punkCC = 0;
	pimg->GetColorConvertor( &punkCC );
	sptrIColorConvertor5	sptrC( punkCC );

	if( !punkCC )
		return false;
	punkCC->Release();
//alloc the buffer

	int	nColorsCount = ::GetImagePaneCount( pimg );
	color	**ppcolorBuf = new color*[nColorsCount];

	long	nLineLength = bColor?((cx*3+3)/4*4):((cx+3)/4*4);
	long	nColorCol, nCol, nRow;
	int nShift;
//convert image rows
	for( nRow = 0; nRow < cy; nRow++ )
	{
		bool	bFreeFlag = false;
		color	*pRow,*pRowR,*pRowG,*pRowB; 
		bool    bConverted = false;
		byte	*pRaw = pbyteDIBData+nRow*cx4;
		byte	*pRGB = 0;
//convert row from raw to RGB DIB
		switch( nBitCount )
		{
		case 1:		//1 bit per pixel image
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

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
			if( bColor )
			{
				pRGB = new byte[nLineLength];
				bFreeFlag = true;

				ASSERT(pbyteRed);
				ASSERT(pbyteGreen);
				ASSERT(pbyteBlue);

				for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
				{
					byte	byteVal = pRaw[nCol];

					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
			}
			else
				pRGB = pRaw;

			break;
		case 10:
		case 12:
		case 16:
			ASSERT( !bColor );
			nShift = nBitCount==10?6:nBitCount==12?4:0;
			pimg->GetRow( cy-nRow-1, 0, &pRow );
			for (nCol = 0; nCol < cx; nCol++)
				pRow[nCol] = ((WORD*)pRaw)[nCol]<<nShift;
			bConverted = true;
			break;

		case 24:		//24 bit per pixel image
			ASSERT(bColor);
			pimg->GetRow( cy-nRow-1, 0, &pRowR );
			pimg->GetRow( cy-nRow-1, 1, &pRowG );
			pimg->GetRow( cy-nRow-1, 2, &pRowB );
			for (nCol = 0; nCol < cx; nCol++)
			{
				pRowB[nCol] = ((WORD)pRaw[3*nCol])<<8;
				pRowG[nCol] = ((WORD)pRaw[3*nCol+1])<<8;
				pRowR[nCol] = ((WORD)pRaw[3*nCol+2])<<8;
			}
			bConverted = true;
			break;

		case 30:
		case 36:
		case 48:
			nShift = nBitCount==30?6:nBitCount==36?4:0;
			ASSERT( bColor );
			pimg->GetRow( cy-nRow-1, 0, &pRowR );
			pimg->GetRow( cy-nRow-1, 1, &pRowG );
			pimg->GetRow( cy-nRow-1, 2, &pRowB );
			for (nCol = 0; nCol < cx; nCol++)
			{
				pRowR[nCol] = ((WORD*)pRaw)[3*nCol+2]<<nShift;
				pRowG[nCol] = ((WORD*)pRaw)[3*nCol+1]<<nShift;
				pRowB[nCol] = ((WORD*)pRaw)[3*nCol]<<nShift;
			}
			bConverted = true;
			break;

		}

		if (!bConverted && sptrC != 0)
		{
			for( long nColor = 0; nColor < nColorsCount; nColor++ )
			{
				 pimg->GetRow( cy-nRow-1, nColor, &ppcolorBuf[nColor] );
			}
			if( sptrC->ConvertDIBToImageEx( pRGB, ppcolorBuf, cx, bColor, nColorsCount ) != S_OK )
				return false;
		}
		if( bFreeFlag )
			delete pRGB;
	}

	if( ppcolorBuf )delete ppcolorBuf;
	if( pbyteRed )delete pbyteRed;
	if( pbyteGreen )delete pbyteGreen;
	if( pbyteBlue )delete pbyteBlue;

	return true;
}


void AttachPlanes16ToRGB(IImage3 *pimg, int cx, int cy, LPWORD pSrcR, LPWORD pSrcG, LPWORD pSrcB,
	bool bHMirror, bool bVMirror, CConv16To16 *pConv)
{
	if (pimg->CreateImage(cx, cy, _bstr_t("RGB"),-1 ) != S_OK)
		return;
	if (pSrcB != NULL && pSrcG != NULL && pSrcR != NULL)
    {
		long x=0,y=0;
		int nOrigOffset = bVMirror==false?0:(cy-1)*cx;
		long nRowOffs = bVMirror==false?cx:-cx;
		unsigned short* pB = pSrcB+nOrigOffset;
		unsigned short* pG = pSrcG+nOrigOffset;
		unsigned short* pR = pSrcR+nOrigOffset;
		for(int y=0; y < (int)cy; y++)
		{
			color *pDstR,*pDstG,*pDstB;
			pimg->GetRow(y, 0, &pDstR);
			pimg->GetRow(y, 1, &pDstG);
			pimg->GetRow(y, 2, &pDstB);
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < (int)cx; x++,x1+=xOffs)
			{
				pDstB[x] = (*pConv)[pB[x1]&pConv->m_wMask];
				pDstG[x] = (*pConv)[pG[x1]&pConv->m_wMask];
				pDstR[x] = (*pConv)[pR[x1]&pConv->m_wMask];
			}
			pB += nRowOffs;
			pG += nRowOffs;
			pR += nRowOffs;
		}
	}
}

void AttachPlanes16ToGray(IImage3 *pimg, int cx, int cy, LPWORD pSrcR, LPWORD pSrcG, LPWORD pSrcB,
	bool bHMirror, bool bVMirror, CConv16To16 *pConv)
{
	if (pimg->CreateImage(cx, cy, _bstr_t("Gray"),-1 ) != S_OK)
		return;
	if (pSrcB != NULL && pSrcG != NULL && pSrcR != NULL)
    {
		long x=0,y=0;
		int nOrigOffset = bVMirror==false?0:(cy-1)*cx;
		long nRowOffs = bVMirror==false?cx:-cx;
		unsigned short* pB = pSrcB+nOrigOffset;
		unsigned short* pG = pSrcG+nOrigOffset;
		unsigned short* pR = pSrcR+nOrigOffset;
		for(int y=0; y < (int)cy; y++)
		{
			color *pDstG;
			pimg->GetRow(y, 0, &pDstG);
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < (int)cx; x++,x1+=xOffs)
			{
				color b = (*pConv)[pB[x1]&pConv->m_wMask];
				color g = (*pConv)[pG[x1]&pConv->m_wMask];
				color r = (*pConv)[pR[x1]&pConv->m_wMask];
				color br = _Bright(b, g, r);
				pDstG[x] = (color)(min(max(0,br),0xFFFF));
			}
			pB += nRowOffs;
			pG += nRowOffs;
			pR += nRowOffs;
		}
	}
}

void AttachPlane16ToGray(IImage3 *pimg, int cx, int cy, LPWORD pSrcG,
	bool bHMirror, bool bVMirror, CConv16To16 *pConv)
{
	if (pimg->CreateImage(cx, cy, _bstr_t("Gray"),-1 ) != S_OK)
		return;
	if (pSrcG != NULL)
    {
		long x=0,y=0;
		int nOrigOffset = bVMirror==false?0:(cy-1)*cx;
		long nRowOffs = bVMirror==false?cx:-cx;
		unsigned short* pG = pSrcG+nOrigOffset;
		for(int y=0; y < (int)cy; y++)
		{
			color *pDstG;
			pimg->GetRow(y, 0, &pDstG);
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < (int)cx; x++,x1+=xOffs)
				pDstG[x] = (*pConv)[pG[x1]&pConv->m_wMask];
//				pDstG[x] = (pG[x1] << nShift);
			pG += nRowOffs;
		}
	}
}

void ConvertPlanes16ToDIB24(LPBITMAPINFOHEADER lpbi, LPWORD pSrcR, LPWORD pSrcG, LPWORD pSrcB,
	bool bHMirror, bool bVMirror, CConv16To8 *pConv)
{
	if (pSrcB != NULL && pSrcG != NULL && pSrcR != NULL)
    {
		long cx = lpbi->biWidth, cy = lpbi->biHeight;
		long x=0,y=0;
		int nOrigOffset = bVMirror?0:(cy-1)*cx;
		long nRowOffs = bVMirror?cx:-cx;
		unsigned short* pB = pSrcB+nOrigOffset;
		unsigned short* pG = pSrcG+nOrigOffset;
		unsigned short* pR = pSrcR+nOrigOffset;
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);;
		for(int y=0; y < cy; y++)
		{
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < cx; x++,x1+=xOffs)
			{
				lpDst[3*x+0] = (*pConv)[pB[x1]&pConv->m_wMask];
				lpDst[3*x+1] = (*pConv)[pG[x1]&pConv->m_wMask];
				lpDst[3*x+2] = (*pConv)[pR[x1]&pConv->m_wMask];
			}
			pB += nRowOffs;
			pG += nRowOffs;
			pR += nRowOffs;
			lpDst += dwRow;
		}
	}
}

void ConvertPlanes16ToDIB8(LPBITMAPINFOHEADER lpbi, LPWORD pSrcR, LPWORD pSrcG, LPWORD pSrcB,
	bool bHMirror, bool bVMirror, CConv16To8 *pConv)
{
	if (pSrcB != NULL && pSrcG != NULL && pSrcR != NULL)
    {
		long cx = lpbi->biWidth, cy = lpbi->biHeight;
		long x=0,y=0;
		int nOrigOffset = bVMirror?0:(cy-1)*cx;
		long nRowOffs = bVMirror?cx:-cx;
		unsigned short* pB = pSrcB+nOrigOffset;
		unsigned short* pG = pSrcG+nOrigOffset;
		unsigned short* pR = pSrcR+nOrigOffset;
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);
		for(int y=0; y < cy; y++)
		{
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < cx; x++,x1+=xOffs)
			{
				BYTE b = (*pConv)[pB[x1]&pConv->m_wMask];
				BYTE r = (*pConv)[pR[x1]&pConv->m_wMask];
				BYTE g = (*pConv)[pG[x1]&pConv->m_wMask];
				int br = _Bright(b, g, r);
				lpDst[x] = (BYTE)(min(max(0,br),255));
			}
			pB += nRowOffs;
			pG += nRowOffs;
			pR += nRowOffs;
			lpDst += dwRow;
		}
	}
}


void ConvertPlane16ToDIB8(LPBITMAPINFOHEADER lpbi, LPWORD pSrcG, bool bHMirror,
	bool bVMirror, CConv16To8 *pConv)
{
	if (pSrcG != NULL)
    {
		long cx = lpbi->biWidth, cy = lpbi->biHeight;
		long x=0,y=0;
		int nOrigOffset = bVMirror?0:(cy-1)*cx;
		long nRowOffs = bVMirror?cx:-cx;
		unsigned short* pG = pSrcG+nOrigOffset;
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);
		for(int y=0; y < cy; y++)
		{
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < cx; x++,x1+=xOffs)
//				lpDst[x] = (BYTE)(pG[x1]>>nShift);
				lpDst[x] = (*pConv)[pG[x1]&pConv->m_wMask];
			pG += nRowOffs;
			lpDst += dwRow;
		}
	}
}

//sergey 06.04.06
    void ConvertPlane16ToDIB8_Mutech(LPBITMAPINFOHEADER lpbi, LPBYTE pSrcG, bool bHMirror,
	bool bVMirror, CConv16To8 *pConv)
{
	
		if (pSrcG != NULL)
    {
		long cx = lpbi->biWidth, cy = lpbi->biHeight;
		long x=0,y=0;
		int nOrigOffset = bVMirror?0:(cy-1)*cx;
		long nRowOffs = bVMirror?cx:-cx;
		LPBYTE pG = pSrcG+nOrigOffset;
		//unsigned short* pG = pSrcG+nOrigOffset;
		//LPBYTE source=(LPBYTE)pSrcG+nOrigOffset;
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);
		for(int y=0; y < cy; y++)
		{
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for(int x=0; x < cx; x++,x1+=xOffs)
//				lpDst[x] = (BYTE)(pG[x1]>>nShift);
                lpDst[x] = (*pConv)[pG[x1]&pConv->m_wMask];
                //lpDst[x] = (*pConv)[source[x1]&pConv->m_wMask];
				pG += nRowOffs;
			//source += nRowOffs;
			lpDst += dwRow;
		}
	}

}


void DisplayImageDataDIB_Mutech(LPBITMAPINFOHEADER lpbi, LPBYTE pSrcG, bool bHMirror,bool bVMirror, CConv16To8 *pConv,LPBYTE dest)
{
	LPBYTE sourceTemp,sourceTemp1,sourceTemp2;
	LPBYTE destTemp,destTemp1,destTemp2,temp,tempDisplay;
	
	if (pSrcG != NULL)
    {
		long cx = lpbi->biWidth, cy = lpbi->biHeight;
	LONG length = lpbi->biWidth;// * m_pParams->pixel_size;				// length of one line in terms of bytes
	LONG lastLine;
	/*if (m_pParams->pixel_format == RGB_888)  // the size of one pixel in destination buffer is 3
		lastLine = length * (m_pParams->frame_height - 1);
	else  */  // the size of each pixel in destination buffer is 1
		lastLine = length * (lpbi->biHeight - 1);	// the last line of image in destination buffer
	//LPBYTE dest = (LPBYTE) m_lpDIBImageData + lastLine;
	//LPBYTE dest =(LPBYTE)m_Dib.GetData()+ lastLine;
		
	int nOrigOffset = bVMirror?0:lastLine;
	long nRowOffs = bVMirror?-length:length;
	unsigned short* pG =(LPWORD) pSrcG+nOrigOffset;
		//dest=dest+lastLine;
		dest=dest+nOrigOffset;
		destTemp=dest;
	    LPBYTE source=sourceTemp=pSrcG;
	int i, j;
	
		//for (i = 0; i < frame_height; i ++)
		/*	for (i = 0; i < lpbi->biHeight; i ++)
		{
			memcpy(destTemp1, sourceTemp, length);
			sourceTemp += length;
			destTemp1 -= length;
		}*/
		
	        
		for (i = 0; i < lpbi->biHeight; i ++)
		{
			int x1 = bHMirror?0:cx-1;
			int xOffs = bHMirror?1:-1;
			for (j = 0; j < lpbi->biWidth; j ++,x1+=xOffs)		
			//dest[j]=(*pConv)[source[j]&pConv->m_wMask];

			dest[j]=(*pConv)[source[x1]&pConv->m_wMask];

			
			//sourceTemp += nRowOffs;
			//destTemp -= length;
			sourceTemp += length;
			destTemp -= nRowOffs;
			//memcpy(dest, source, length);
			source = sourceTemp;
			dest=destTemp;
		
		
	}
	}
	
}
//end

void ConvertDIB24ToDIB8(LPBITMAPINFOHEADER lpbiDst, LPBITMAPINFOHEADER lpbiSrc, int nPlane)
{
	LPBYTE pSrc = (LPBYTE)::DIBData(lpbiSrc);
	DWORD dwSrcRow = ::DIBRowSize(lpbiSrc);
	LPBYTE pDst = (LPBYTE)::DIBData(lpbiDst);
	DWORD dwDstRow = ::DIBRowSize(lpbiDst);
	int dx = lpbiSrc->biWidth;
	int dy = abs(lpbiSrc->biHeight);
	for (int y = 0; y < dy; y++)
	{
		if (nPlane >= 0 && nPlane < 3)
		{
			for (int x = 0; x < dx; x++)
				pDst[x] = pSrc[3*x+nPlane];
		}
		else
		{
			for (int x = 0; x < dx; x++)
				pDst[x] = Bright(pSrc[3*x+0], pSrc[3*x+1], pSrc[3*x+2]);
		}
		pSrc += dwSrcRow;
		pDst += dwDstRow;
	}
}


template<class ColorRepr> void _ApplyCnvBufferBCG(ColorRepr *pCnvBuff, int nBrightness, int nContrast, double dGamma, ColorRepr MaxColor, int nMaxColorSrc)
{
	double a = 1.+3*nContrast/255.;//pow(MaxColor,nContrast/100.);
	int b = nBrightness*MaxColor/255;
	dGamma = dGamma==0.?1.:(dGamma!=1.)?::tan((dGamma+1.)/4.*PI):1000.;
	double	g = max(0.001, dGamma);
	double CnvMul = MaxColor/pow(MaxColor, 1/g);
	for (int i = 0; i <= nMaxColorSrc; i++)
	{
//		double d = a*double(pCnvBuff[i])+b;
		double d = a*double(i)*double(MaxColor+1)/double(nMaxColorSrc+1)+b;
		if (d < 0) d = 0;
		if (d > MaxColor) d = MaxColor;
		d = CnvMul*pow(d,1/g);
		pCnvBuff[i] = (ColorRepr)(d);
	}
}

void CConv16To8::Init(int bpp, int nBrightness, int nContrast, double dGamma)
{
	bpp = bpp>16?bpp/3:bpp;
	int nMaxColor = (1<<bpp)-1;
	int nShift = bpp-8;
	alloc(nMaxColor+1);
	for (int i = 0; i <= nMaxColor; i++)
	{
		WORD w = (WORD)i;
		ptr[i] = w>>nShift;
	}
	m_wMask = (WORD)nMaxColor;
	_ApplyCnvBufferBCG<BYTE>(ptr, nBrightness, nContrast, dGamma, 255, nMaxColor);
}

void CConv16To16::Init(int bpp, int nBrightness, int nContrast, double dGamma)
{
	bpp = bpp>16?bpp/3:bpp;
	int nMaxColor = (1<<bpp)-1;
	int nShift = 16-bpp;
	alloc(nMaxColor+1);
	for (int i = 0; i <= nMaxColor; i++)
	{
		WORD w = (WORD)i;
		ptr[i] = 0xFFFF*i/nMaxColor;
	}
	m_wMask = (WORD)nMaxColor;
	_ApplyCnvBufferBCG<WORD>(ptr, nBrightness, nContrast, dGamma, 0xFFFF, nMaxColor);
}


void MakeDummyImage(IUnknown *punkImg, int cx, int cy)
{
	IImage3Ptr sptrImage(punkImg);
	if (sptrImage != 0)
		sptrImage->CreateImage(cx, cy, _bstr_t("RGB"),-1 );
}

void CalAvrBrightness(LPBITMAPINFOHEADER lpbi, double *pdBriRed, double *pdBriGreen, double *pdBriBlue)
{
	double r = 0., g = 0., b = 0.;
	int n = 0;
	if (lpbi->biBitCount == 24)
	{
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);;
		for(int y=0; y < lpbi->biHeight; y++)
		{
			for(int x=0; x < lpbi->biWidth; x++)
			{
				b += double(lpDst[3*x+0]);
				g += double(lpDst[3*x+1]);
				r += double(lpDst[3*x+2]);
				n++;
			}
			lpDst += dwRow;
		}
	}
	if (pdBriRed)
		*pdBriRed = r/n;
	if (pdBriGreen)
		*pdBriGreen = g/n;
	if (pdBriBlue)
		*pdBriBlue = b/n;
}

void CalAvrBrightnessFields(LPBITMAPINFOHEADER lpbi, double *pdBriRed1, double *pdBriRed2,
	double *pdBriGreen1, double *pdBriGreen2, double *pdBriBlue1, double *pdBriBlue2)
{
	double r1 = 0., r2 = 0., g1 = 0., g2 = 0., b1 = 0., b2 = 0.;
	int n1 = 0, n2 = 0;
	if (lpbi->biBitCount == 24)
	{
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);;
		for(int y=0; y < lpbi->biHeight; y+=2)
		{
			for(int x=0; x < lpbi->biWidth; x++)
			{
				b1 += double(lpDst[3*x+0]);
				g1 += double(lpDst[3*x+1]);
				r1 += double(lpDst[3*x+2]);
				n1++;
			}
			lpDst += dwRow;
			for(x=0; x < lpbi->biWidth; x++)
			{
				b2 += double(lpDst[3*x+0]);
				g2 += double(lpDst[3*x+1]);
				r2 += double(lpDst[3*x+2]);
				n2++;
			}
			lpDst += dwRow;
		}
	}
	else if (lpbi->biBitCount == 8)
	{
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);;
		for(int y=0; y < lpbi->biHeight; y+=2)
		{
			for(int x=0; x < lpbi->biWidth; x++)
			{
				r1 += double(lpDst[x]);
				n1++;
			}
			lpDst += dwRow;
			for(x=0; x < lpbi->biWidth; x++)
			{
				r2 += double(lpDst[x]);
				n2++;
			}
			lpDst += dwRow;
		}
	}
	if (pdBriRed1)
		*pdBriRed1 = r1/n1;
	if (pdBriRed2)
		*pdBriRed2 = r2/n2;
	if (lpbi->biBitCount == 24)
	{
		if (pdBriGreen1)
			*pdBriGreen1 = g1/n1;
		if (pdBriGreen2)
			*pdBriGreen2 = g2/n2;
		if (pdBriBlue1)
			*pdBriBlue1 = b1/n1;
		if (pdBriBlue2)
			*pdBriBlue2 = b2/n2;
	}
}

int CalAvrBrightnessProc(LPBITMAPINFOHEADER lpbi, int nProcBright, int nProcDark)
{
	int Histo[256];
	memset(Histo, 0, sizeof(Histo));
	if (lpbi->biBitCount == 8)
	{
		LPBYTE lpDst = (LPBYTE)DIBData(lpbi);
		DWORD dwRow = ::DIBRowSize(lpbi);;
		for(int y=0; y < lpbi->biHeight; y++)
		{
			for(int x=0; x < lpbi->biWidth; x++)
				Histo[lpDst[x]]++;
			lpDst += dwRow;
		}
	}
	int nPoints = lpbi->biWidth*lpbi->biHeight;
	int n1 = 0, nSum = 0;
	int nPoints1 = nPoints*nProcDark/100;
	while (nSum+Histo[n1] < nPoints1)
		nSum += Histo[n1++];
	int n2 = 255, nSum2 = 0;
	int nPoints2 = nPoints*nProcBright/100;
	while (nSum2+Histo[n2] < nPoints2)
		nSum2 += Histo[n2--];
	double dSum = 0;
	int nPts = 0;
	for (int i = n1; i <= n2; i++)
	{
		dSum += i*Histo[i];
		nPts += Histo[i];
	}
	return (int)(dSum/nPts);
}


static void _DrawFrameOnImage24(LPBITMAPINFOHEADER lpbi, CRect rc, BYTE byR,
	BYTE byG, BYTE byB)
{
	LPBYTE lpData = (LPBYTE)DIBData(lpbi);
	DWORD dwRow = DIBRowSize(lpbi);//(lpbi->biWidth*3+3)/4*4;
	LPBYTE lp = lpData+dwRow*(lpbi->biHeight-1-rc.top);
	for (int x = rc.left; x < rc.right; x++)
	{
		lp[3*x] = byB;
		lp[3*x+1] = byG;
		lp[3*x+2] = byR;
	}
	lp = lpData+dwRow*(lpbi->biHeight-1-(rc.bottom-1));
	for (int x = rc.left; x < rc.right; x++)
	{
		lp[3*x] = byB;
		lp[3*x+1] = byG;
		lp[3*x+2] = byR;
	}
	for (int y = rc.top; y < rc.bottom; y++)
	{
		lp = lpData+dwRow*(lpbi->biHeight-1-y);
		lp[3*rc.left] = byB;
		lp[3*rc.left+1] = byG;
		lp[3*rc.left+2] = byR;
		lp[3*(rc.right-1)] = byB;
		lp[3*(rc.right-1)+1] = byG;
		lp[3*(rc.right-1)+2] = byR;
	}
}

static void _DrawFrameOnImage8(LPBITMAPINFOHEADER lpbi, CRect rc, BYTE by)
{
	LPBYTE lpData = (LPBYTE)DIBData(lpbi);
	DWORD dwRow = DIBRowSize(lpbi);
	LPBYTE lp = lpData+dwRow*(lpbi->biHeight-1-rc.top);
	for (int x = rc.left; x < rc.right; x++)
		lp[x] = by;
	lp = lpData+dwRow*(lpbi->biHeight-1-(rc.bottom-1));
	for (int x = rc.left; x < rc.right; x++)
		lp[x] = by;
	for (int y = rc.top; y < rc.bottom; y++)
	{
		lp = lpData+dwRow*(lpbi->biHeight-1-y);
		lp[rc.left] = by;
		lp[rc.right-1] = by;
	}
}


void MapRectOnDIB(LPBITMAPINFOHEADER lpbi, CRect rc, COLORREF clr)
{
	if (lpbi->biBitCount == 24)
		_DrawFrameOnImage24(lpbi, rc, GetRValue(clr), GetGValue(clr), GetBValue(clr));
	else if (lpbi->biBitCount == 8)
		_DrawFrameOnImage8(lpbi, rc, GetRValue(clr));
}

void FlipDIB(LPBITMAPINFOHEADER lpbi, LPBYTE lpData, bool bHorz, bool bVert)
{
	if (lpData == NULL)
		lpData = (LPBYTE)DIBData(lpbi);
	DWORD dwRowSize = DIBRowSize(lpbi);
	if (bVert)
	{
		_ptr_t2<BYTE> buff(dwRowSize);
		LPBYTE pBuff = (LPBYTE)buff;
		for (int y = 0; y < lpbi->biHeight/2; y++)
		{
			int y1 = lpbi->biHeight-1-y;
			LPBYTE lpRow1 = lpData+y*dwRowSize;
			LPBYTE lpRow2 = lpData+y1*dwRowSize;
			memcpy((LPBYTE)buff,lpRow1,dwRowSize);
			memcpy(lpRow1,lpRow2,dwRowSize);
			memcpy(lpRow2,(LPBYTE)buff,dwRowSize);
		}
	}
	if (bHorz)
	{
		int bpp = lpbi->biBitCount>24?6:lpbi->biBitCount>8&&lpbi->biBitCount<16?2:lpbi->biBitCount/8;
		int w1 = lpbi->biWidth-1;
		if (bpp == 1)
		{
			for (int y = 0; y < lpbi->biHeight; y++)
			{
				LPBYTE lpRow = lpData+y*dwRowSize;
				for (int x = 0; x < lpbi->biWidth/2; x++)
				{
					BYTE by = lpRow[x];
					lpRow[x] = lpRow[w1-x];
					lpRow[w1-x] = by;
				}
			}
		}
		else
		{
			_ptr_t2<BYTE> buff(bpp);
			LPBYTE pBuff = (LPBYTE)buff;
			for (int y = 0; y < lpbi->biHeight; y++)
			{
				LPBYTE lpRow = lpData+y*dwRowSize;
				LPBYTE lp1 = lpRow;
				LPBYTE lp2 = lpRow+w1*bpp;
				for (int x = 0; x < lpbi->biWidth/2; x++)
				{
					memcpy((LPBYTE)buff, lp1, bpp);
					memcpy(lp1,lp2,bpp);
					memcpy(lp2,(LPBYTE)buff,bpp);
					lp1 += bpp;
					lp2 -= bpp;
				}
			}
		}
	}
}



