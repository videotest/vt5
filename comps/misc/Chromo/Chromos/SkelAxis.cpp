#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


bool ClearEnds(LPBITMAPINFOHEADER lpbiSrc)
{
	int nChanges = 0;
	LPBYTE lpSrc = ((LPBYTE)(lpbiSrc+1))+1024; // Will be used for operation
	unsigned nCol4 = ((lpbiSrc->biWidth+3)>>2)<<2;
	LPBYTE lpSrc0 = lpSrc;
	LPBYTE lpSrc1 = lpSrc0+nCol4;
	LPBYTE lpSrc2 = lpSrc1+nCol4;
	for (unsigned i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (unsigned j = 1; j < lpbiSrc->biWidth-1; j++)
		{
			if (lpSrc1[j] == 0xFF)
			{
				//     1  1  x
				//     1  0  x
				//     1  1  x
				if ((lpSrc0[j-1]|lpSrc0[j]|lpSrc1[j-1]|lpSrc2[j-1]|lpSrc2[j])==0)
				{
					lpSrc1[j] = 0xFE;
					nChanges++;
				}
				//     x  1  1
				//     x  0  1
				//     x  1  1
				else if ((lpSrc0[j]|lpSrc0[j+1]|lpSrc1[j+1]|lpSrc2[j]|lpSrc2[j+1])==0)
				{
					lpSrc1[j] = 0xFE;
					nChanges++;
				}
				//     x  x  x
				//     1  0  1
				//     1  1  1
				else if ((lpSrc1[j-1]|lpSrc1[j+1]|lpSrc2[j-1]|lpSrc2[j]|lpSrc2[j+1])==0)
				{
					lpSrc1[j] = 0xFE;
					nChanges++;
				}
				//     1  1  1
				//     1  0  1
				//     x  x  x
				else if ((lpSrc1[j-1]|lpSrc1[j+1]|lpSrc0[j-1]|lpSrc0[j]|lpSrc0[j+1])==0)
				{
					lpSrc1[j] = 0xFE;
					nChanges++;
				}
			}
		}
		lpSrc0 = lpSrc1;
		lpSrc1 = lpSrc2;
		lpSrc2 += nCol4;
	}
	if (nChanges)
	{
		LPBYTE lpSrc1 = lpSrc+nCol4;
		for (unsigned i = 1; i < lpbiSrc->biHeight-1; i++)
		{
			for (unsigned j = 1; j < lpbiSrc->biWidth-1; j++)
			{
				if (lpSrc1[j] == 0xFE)
					lpSrc1[j] = 0;
			}
			lpSrc1 += nCol4;
		}
	}
	return nChanges >= 3;
}

bool Skeleton(LPBITMAPINFOHEADER lpbiDst, LPBITMAPINFOHEADER lpbiSrc, DWORD dwSize)
{
	unsigned i,j;
	int nChanges = 0;
	LPBYTE lpSrc = ((LPBYTE)(lpbiSrc+1))+1024; // Will be used for operation
	unsigned nCol4 = ((lpbiSrc->biWidth+3)>>2)<<2;
	LPBYTE lpSrc0 = lpSrc;
	LPBYTE lpSrc1 = lpSrc0+nCol4;
	LPBYTE lpSrc2 = lpSrc1+nCol4;
	LPBYTE lpDst = ((LPBYTE)(lpbiDst+1))+1024; // Will be used for operation
	LPBYTE lpDst1 = lpDst+nCol4;
	memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024); // !!!!!!!!!!!!!!!!!!
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&!lpSrc0[j-1]&&!lpSrc0[j]&&!lpSrc0[j+1]&&lpSrc2[j-1]&&lpSrc2[j]&&lpSrc2[j+1])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&lpSrc0[j-1]&&lpSrc0[j]&&lpSrc0[j+1]&&!lpSrc2[j-1]&&!lpSrc2[j]&&!lpSrc2[j+1])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&lpSrc0[j-1]&&lpSrc1[j-1]&&lpSrc2[j-1]&&!lpSrc0[j+1]&&!lpSrc1[j+1]&&!lpSrc2[j+1])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&!lpSrc0[j-1]&&!lpSrc1[j-1]&&!lpSrc2[j-1]&&lpSrc0[j+1]&&lpSrc1[j+1]&&lpSrc2[j+1])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&!lpSrc0[j]&&!lpSrc0[j+1]&&!lpSrc1[j+1]&&lpSrc1[j-1]&&lpSrc2[j-1]&&lpSrc2[j])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&!lpSrc0[j]&&!lpSrc0[j-1]&&!lpSrc1[j-1]&&lpSrc1[j+1]&&lpSrc2[j+1]&&lpSrc2[j])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
			if (lpSrc1[j]&&!lpSrc2[j]&&!lpSrc2[j-1]&&!lpSrc1[j-1]&&lpSrc1[j+1]&&lpSrc0[j+1]&&lpSrc0[j])
				lpDst1[j] = 0, nChanges++;
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	lpSrc0 = lpSrc, lpSrc1 = lpSrc0+nCol4, lpSrc2 = lpSrc1+nCol4, lpDst1 = lpDst+nCol4;
	for (i = 1; i < lpbiSrc->biHeight-1; i++)
	{
		for (j = 1; j < lpbiSrc->biWidth-1; j++)
		{
			if (lpSrc1[j]&&!lpSrc2[j]&&!lpSrc2[j+1]&&!lpSrc1[j+1]&&lpSrc1[j-1]&&lpSrc0[j-1]&&lpSrc0[j])
				lpDst1[j] = 0, nChanges++;
		}
		lpSrc0 = lpSrc1, lpSrc1 = lpSrc2, lpSrc2 += nCol4, lpDst1 += nCol4;
	}
	if (nChanges) memcpy(lpSrc, lpDst, dwSize-sizeof(BITMAPINFOHEADER)-1024);
	return nChanges >= 1;
}

const int directX[8] = {0, 1, 1, 1, 0, -1, -1, -1 };
const int directY[8] = {-1, -1, 0, 1, 1, 1, 0, -1 };

void _ContourExternalMark(CWalkFillInfo &info, Contour *pcntr, byte byteMark, byte byteMarkContour)
{
	if (pcntr->nContourSize == 0)
		return;
	int	nDir, nNextDir = ContourGetDirection( pcntr, pcntr->nContourSize-1 );
	for (int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++)
	{
		nDir = nNextDir;
		nNextDir = ContourGetDirection(pcntr, nPoint);

		ASSERT(nDir != -1);
		ASSERT(nNextDir != -1);

		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;

		for (nDir=(nDir < 3)?nDir+5:nDir-3; nDir != nNextDir; nDir=((nDir==7)?0:nDir+1))
		{
			byte byPixel = info.GetPixel(x+directX[nDir], y+directY[nDir]);
			if (byPixel != byteMarkContour)
				info.SetPixel(x+directX[nDir], y+directY[nDir], byteMark);
		}
		if(byteMarkContour)
			info.SetPixel(x, y, byteMarkContour);
	}
}

void _ContourFillArea( CWalkFillInfo &info, byte byteRawBody, byte byteExtMark, byte byteFill, CRect rect )
{
	if( rect == NORECT )
		rect = info.GetRect();

	int	x, y;

	byte	bytePrev, byteCur;
	bool	bFill = false;

	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		byteCur = 0x00;
		bFill = false;
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

	}
}

Contour *_SmoothContour(Contour *pCntr)
{
	Contour *pNew = ::ContourCreate();
	double x,y;
	::ContourApplySmoothFilter(pCntr, 0, &x, &y);
	::ContourAddPoint(pNew, int(x+0.5), int(y+0.5), false);
	for (unsigned i = 1; i < pCntr->nContourSize; i++)
	{
		double x,y;
		::ContourApplySmoothFilter(pCntr, i, &x, &y);
		::ContourAddPoint(pNew, int(x+0.5), int(y+0.5), false);
	}
	::ContourAddPoint(pNew, int(x+0.5), int(y+0.5), false);
	if (pNew->ppoints[0].x == pNew->ppoints[pNew->nContourSize-1].x && pNew->ppoints[0].y == pNew->ppoints[pNew->nContourSize-1].y)
		pNew->nContourSize--;
	return pNew;
};

void CAxisContext::SkeletonAxis()
{
	int i,j;
	// Initialize smoothed image
	Contour *pSmoothCntr = _SmoothContour(pCntr);
	CRect rect = ::ContourCalcRect(pCntr);
	CWalkFillInfo info(rect);
	_ContourExternalMark(info, pSmoothCntr, 0x01, info.GetRawBody());
	_ContourFillArea(info, info.GetRawBody(), 0x01, info.GetFillBody(), rect);
	::ContourDelete(pSmoothCntr);
	// Initialize image for skeletization
	double *pfSegXNew = 0;
	double *pfSegYNew = 0;
	int nPointsNew = 0;
	int nCol = pImgWrp.GetWidth();
	int nRow = pImgWrp.GetHeight();
	/*if (rect.Width() != nCol || rect.Height() != nRow)
	{
		AfxMessageBox(IDS_COMPOSED_OBJECTS, MB_OK|MB_ICONEXCLAMATION);
		return;
	}*/
	int nCol4 = ((nCol+3)>>2)<<2;
	DWORD dwSize = sizeof(BITMAPINFOHEADER)+1024+nCol4*nRow;
	LPBITMAPINFOHEADER lpbi1 = (LPBITMAPINFOHEADER)new BYTE[dwSize];
	LPBITMAPINFOHEADER lpbi2 = (LPBITMAPINFOHEADER)new BYTE[dwSize];
	memset(lpbi1, 0, dwSize);
	lpbi1->biSize = sizeof(BITMAPINFOHEADER);
	lpbi1->biWidth = pImgWrp.GetWidth();
	lpbi1->biHeight = pImgWrp.GetHeight();
	lpbi1->biBitCount = 8;
	lpbi1->biPlanes = 1;
	lpbi1->biClrUsed = lpbi1->biClrImportant = 256;
	lpbi1->biCompression = BI_RGB;
	lpbi1->biSizeImage = 0;
	BYTE *pData = ((BYTE *)(lpbi1+1))+1024;
	BYTE *pData1 = pData;
	int nRowMin = min(nRow,rect.Height());
	int nColMin = min(nCol,rect.Width());
//	memset(pData, 0, nCol4);
	for (i = 1; i < nRowMin-1; i++)
	{
		pData1 += nCol4;
		LPBYTE lpSrc = info.GetRowMask(i);
		for (j = 1; j < nColMin-1; j++)
			pData1[j] = lpSrc[j]==info.GetFillBody()?0xFF:0;
//		pData1[0] = 0;
//		pData1[nCol-1] = 0;
	}
//	memset(pData+nCol4*(nRow-1), 0, nCol4);
	memcpy(lpbi2, lpbi1, dwSize);
	while (Skeleton(lpbi1,lpbi2,dwSize)||ClearEnds(lpbi1));
	int i1 = -1,j1 = -1;
	BYTE *ppData[3];
	ppData[0] = pData;
	ppData[1] = pData+nCol4;
	ppData[2] = ppData[1]+nCol4;
	for (i = 1; i < nRow-1; i++)
	{
		bool bBreak = false;
		for (j = 1; j < nCol-1; j++)
		{
			if (ppData[1][j] == 0xFF)
			{
				BYTE bNeis = ppData[0][j]+ppData[1][j-1]+ppData[1][j+1]+ppData[2][j];
				if (bNeis == 0xFF)
				{
					i1 = i;
					j1 = j;
					bBreak = true;
					break;
				}
			}
			else ASSERT(ppData[1][j] == 0);
		}
		if (bBreak) break;
		ppData[0] = ppData[1];
		ppData[1] = ppData[2];
		ppData[2] = ppData[2]+nCol4;
	}
	ASSERT(i1 != -1 && j1 != -1);
	if (i1 != -1)
	{
		CArray<POINT, POINT&> arr;
		while (1)
		{
			POINT pt;
			pt.x = j1;
			pt.y = i1;
			arr.Add(pt);
			ppData[0] = pData+nCol4*(i1-1);
			ppData[1] = ppData[0]+nCol4;
			ppData[2] = ppData[1]+nCol4;
			ASSERT(ppData[1][j1] == 0xFF);
			ppData[1][j1] = 0;
			if (ppData[1][j1-1] == 0xFF)
				j1 = j1-1;
			else if (ppData[1][j1+1] == 0xFF)
				j1 = j1+1;
			else if (ppData[0][j1] == 0xFF)
				i1 = i1-1;
			else if (ppData[2][j1] == 0xFF)
				i1 = i1+1;
			else
				break;
		}
		if (arr.GetSize())
		{
#if 1
			double a, fL;
			if (arr.GetSize() > nPtsDist)
			{
				int nSize = arr.GetSize();
				nPointsNew = ((nSize)/nPtsDist)+3;
				pfSegXNew = new double[nPointsNew];
				pfSegYNew = new double[nPointsNew];
				int nStep = nPtsDist/2;
				for (i = 0; i < nPointsNew-2; i++)
				{
					int iPoint = min(i*nPtsDist,nSize-1);
#if 0
					// Calculate coordinate of new point as average of coordinates of
					// center point, nPtsDist/2 on left and nPtsDist/2 on right. Scope can be 
					// less, but must be same of negative and positive side.
					int j1 = max(iPoint-nStep,0);
					int j2 = min(iPoint+nStep,nSize-1);
					int nStep1 = min(j2-iPoint,iPoint-j1);
					j1 = iPoint-nStep1;
					j2 = iPoint+nStep1;
					double dSumX = 0., dSumY = 0.;
					for (int j = j1; j <= j2; j++)
					{
						dSumX += arr[j].x;
						dSumY += arr[j].y;
					}
					int nCalcPts = j2-j1+1;
					pfSegXNew[i+1] = dSumX/nCalcPts;
					pfSegYNew[i+1] = dSumY/nCalcPts;
#else
					pfSegXNew[i+1] = arr[iPoint].x;
					pfSegYNew[i+1] = arr[iPoint].y;
#endif
				}
				a = atan2(pfSegYNew[1]-pfSegYNew[2], pfSegXNew[1]-pfSegXNew[2])+fAngle;
				fL = GetLen(pfSegXNew[2]-pfSegXNew[1], pfSegYNew[2]-pfSegYNew[1]);
				a = a-fAngle;
				pfSegXNew[0] = pfSegXNew[1]+fL*cos(a);
				pfSegYNew[0] = pfSegYNew[1]+fL*sin(a);
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegXNew[1], pfSegYNew[1], a, pt))
				{
					pfSegXNew[0] = pt.x;
					pfSegYNew[0] = pt.y;
				}
				a = atan2(pfSegYNew[nPointsNew-2]-pfSegYNew[nPointsNew-3], pfSegXNew[nPointsNew-2]-pfSegXNew[nPointsNew-3])+fAngle;
				fL = GetLen(pfSegXNew[nPointsNew-2]-pfSegXNew[nPointsNew-1], pfSegYNew[nPointsNew-2]-pfSegYNew[nPointsNew-1]);
				a = a-fAngle;
				pfSegXNew[nPointsNew-1] = pfSegXNew[nPointsNew-2]+fL*cos(a);
				pfSegYNew[nPointsNew-1] = pfSegYNew[nPointsNew-2]+fL*sin(a);
				if (GetPointDir(pImgWrp, pfSegXNew[nPointsNew-2], pfSegYNew[nPointsNew-2], a, pt))
				{
					pfSegXNew[nPointsNew-1] = pt.x;
					pfSegYNew[nPointsNew-1] = pt.y;
				}
			}
			else
			{
				pfSegXNew = new double[2];
				pfSegYNew = new double[2];
				pfSegXNew[0] = arr[0].x;
				pfSegYNew[0] = arr[0].y;
				pfSegXNew[1] = arr[arr.GetSize()-1].x;
				pfSegYNew[1] = arr[arr.GetSize()-1].y;
				nPointsNew = 2;
			}
#else
			pfSegXNew = new double[arr.GetSize()];
			pfSegYNew = new double[arr.GetSize()];
			for (i = 0; i < arr.GetSize(); i++)
			{
				pfSegXNew[i] = arr[i].x;
				pfSegYNew[i] = arr[i].y;
				nPointsNew = arr.GetSize();
			}
#endif
		}
	}
	delete lpbi1;
	delete lpbi2;
	if (nPointsNew)
	{
		if (pfSegX) delete pfSegX;
		if (pfSegY) delete pfSegY;
		pfSegX = pfSegXNew;
		pfSegY = pfSegYNew;
		nPoints = nPointsNew;
	}
}

