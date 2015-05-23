// Accumulate.cpp: implementation of the CAccumulate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "newdoc.h"
#include "Accumulate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccumulate::CAccumulate()
{
	m_cx = m_cy = 0;
	m_nImages = 0;
}

CAccumulate::~CAccumulate()
{
	DeinitArray();
}

void CAccumulate::DeinitArray()
{
	int i;
	for (i = 0; i < m_RowsR.GetSize(); i++)
		free(m_RowsR.GetAt(i));
	m_RowsR.RemoveAll();
	for (i = 0; i < m_RowsG.GetSize(); i++)
		free(m_RowsG.GetAt(i));
	m_RowsG.RemoveAll();
	for (i = 0; i < m_RowsB.GetSize(); i++)
		free(m_RowsB.GetAt(i));
	m_RowsB.RemoveAll();
}

static void AllocArray(AccArray &arr, int nWidth, int nHeight)
{
	int i,j;
	arr.SetSize(nHeight);
	for (i = 0; i < nHeight; i++)
	{
		AccPix *p = (AccPix *)malloc(nWidth*sizeof(AccPix));
		for (j = 0; j < nWidth; j++)
			p[j] = AccNull;
		arr.SetAt(i, p);
	}
}

void CAccumulate::InitNew(int nWidth, int nHeight, bool bRGB)
{
	m_cx = nWidth;
	m_cy = nHeight;
	AllocArray(m_RowsR, nWidth, nHeight);
	if (bRGB)
	{
		AllocArray(m_RowsG, nWidth, nHeight);
		AllocArray(m_RowsB, nWidth, nHeight);
	}
}

static void CopyArray(AccArray &arrDst, AccArray &arrSrc, int cx, int cy)
{
	int i,j;
	arrDst.SetSize(cy);
	for (i = 0; i < cy; i++)
	{
		AccPix *p = (AccPix *)malloc(cx*sizeof(AccPix));
		AccPix *p1 = arrSrc.GetAt(i);
		for (j = 0; j < cx; j++)
			p[j] = p1[j];
		arrDst.SetAt(i, p);
	}
}

void CAccumulate::ReinitRGB()
{
	CopyArray(m_RowsG, m_RowsR, m_cx, m_cy);
	CopyArray(m_RowsB, m_RowsR, m_cx, m_cy);
}

static void ReinitArray(AccArray &arr, int cx, int cy, int cxNew, int cyNew)
{
	int i,j;
	if (cyNew < cy)
	{
		for (i = cyNew; i < cy; i++)
		{
			free(arr.GetAt(i));
		}
		arr.SetSize(cyNew);
	}
	if (cxNew != cx)
	{
		for (i = 0; i < arr.GetSize(); i++)
		{
			AccPix *p = (AccPix *)realloc(arr.GetAt(i),cxNew*sizeof(AccPix));
			arr.SetAt(i, p);
			if (cxNew > cx)
			{
				AccPix *p = arr.GetAt(i);
				for (j = cx; j < cxNew; j++)
					p[j] = AccNull;
			}
		}
	}
	if (cyNew > cy)
	{
		arr.SetSize(cyNew);
		for (i = cy; i < cyNew; i++)
		{
			AccPix *p = (AccPix *)malloc(cxNew*sizeof(AccPix));
			for (j = 0; j < cxNew; j++)
				p[j] = AccNull;
			arr.SetAt(i, p);
		}
	}
}

void CAccumulate::ReinitSize(int cx, int cy)
{
	ReinitArray(m_RowsR, m_cx, m_cy, cx, cy);
	if (m_RowsG.GetSize())
	{
		ReinitArray(m_RowsG, m_cx, m_cy, cx, cy);
		ReinitArray(m_RowsB, m_cx, m_cy, cx, cy);
	}
	m_cx = cx;
	m_cy = cy;
}

void CAccumulate::DoAddGS(LPBITMAPINFOHEADER lpbi)
{
	memcpy(m_rgb, lpbi+1, sizeof(m_rgb));
	LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
	int nRow = ((lpbi->biWidth+3)>>2)<<2;
	int i,j;
	for (i = 0; i < m_cy; i++)
	{
		AccPix *p0 = m_RowsR.GetAt(i);
		AccPix *p1 = m_RowsG.GetSize()==0?NULL:m_RowsG.GetAt(i);
		AccPix *p2 = m_RowsG.GetSize()==0?NULL:m_RowsB.GetAt(i);
		for (j = 0; j < m_cx; j++)
		{
			p0[j] = (AccPix)(p0[j]+pData[j]);
			if (p1)
			{
				p1[j] = (AccPix)(p1[j]+pData[j]);
				p2[j] = (AccPix)(p2[j]+pData[j]);
			}
		}
		pData += nRow;
	}
}

void CAccumulate::DoGetGS(LPBITMAPINFOHEADER lpbi)
{
	LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
	int nRow = ((lpbi->biWidth+3)>>2)<<2;
	int i,j;
	for (i = 0; i < m_cy; i++)
	{
		AccPix *p0 = m_RowsR.GetAt(i);
		for (j = 0; j < m_cx; j++)
		{
			pData[j] = (BYTE)(p0[j]/m_nImages);
		}
		pData += nRow;
	}
}

void CAccumulate::DoAddTrueColor(LPBITMAPINFOHEADER lpbi)
{
	LPBYTE pData = (LPBYTE)(lpbi+1);
	int nRow = ((3*lpbi->biWidth+3)>>2)<<2;
	int i,j;
	for (i = 0; i < m_cy; i++)
	{
		AccPix *p0 = m_RowsR.GetAt(i);
		AccPix *p1 = m_RowsG.GetAt(i);
		AccPix *p2 = m_RowsB.GetAt(i);
		for (j = 0; j < m_cx; j++)
		{
			p0[j] = (AccPix)(p0[j]+pData[3*j+2]);
			p1[j] = (AccPix)(p1[j]+pData[3*j+1]);
			p2[j] = (AccPix)(p2[j]+pData[3*j+0]);
		}
		pData += nRow;
	}
}

void CAccumulate::DoGetTrueColor(LPBITMAPINFOHEADER lpbi)
{
	LPBYTE pData = (LPBYTE)(lpbi+1);
	int nRow = ((3*lpbi->biWidth+3)>>2)<<2;
	int i,j;
	for (i = 0; i < m_cy; i++)
	{
		AccPix *p0 = m_RowsR.GetAt(i);
		AccPix *p1 = m_RowsG.GetAt(i);
		AccPix *p2 = m_RowsB.GetAt(i);
		for (j = 0; j < m_cx; j++)
		{
			pData[3*j+2] = (BYTE)(p0[j]/m_nImages);
			pData[3*j+1] = (BYTE)(p1[j]/m_nImages);
			pData[3*j+0] = (BYTE)(p2[j]/m_nImages);
		}
		pData += nRow;
	}
}


void CAccumulate::Add(LPBITMAPINFOHEADER lpbi)
{
	bool bRGB = lpbi->biBitCount!=8;
	if (m_nImages == 0)
	{
		DeinitArray();
		InitNew(lpbi->biWidth,lpbi->biHeight,bRGB);
	}
	else if (bRGB && m_RowsG.GetSize() == 0)
		ReinitRGB();
	if (lpbi->biWidth!=m_cx||lpbi->biHeight!=m_cy)
		ReinitSize(lpbi->biWidth,lpbi->biHeight);
	if (lpbi->biBitCount==8)
		DoAddGS(lpbi);
	else if (lpbi->biBitCount==24)
		DoAddTrueColor(lpbi);
	m_nImages++;
}

void CAccumulate::Clear()
{
	DeinitArray();
	m_cx = m_cy = 0;
	m_nImages = 0;
}

unsigned CAccumulate::CalcSize()
{
	DWORD dw,dwSize;
	if (IsTrueColor())
	{
		dw = ((3*m_cx+3)>>2)<<2;
		dwSize = sizeof(BITMAPINFOHEADER)+dw*m_cy;
	}
	else
	{
		dw = ((m_cx+3)>>2)<<2;
		dwSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dw*m_cy;
	}
	return dwSize;
}

void CAccumulate::InitHeader(LPBITMAPINFOHEADER lpbi)
{
	if (IsTrueColor())
	{
		lpbi->biBitCount = 24;
		lpbi->biClrUsed = lpbi->biClrImportant = 0;
	}
	else
	{
		lpbi->biBitCount = 8;
		lpbi->biClrUsed = lpbi->biClrImportant = 256;
		memcpy(lpbi+1, m_rgb, sizeof(m_rgb));
	}
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = m_cx;
	lpbi->biHeight = m_cy;
	lpbi->biCompression = BI_RGB;
	lpbi->biSizeImage = 0;
	lpbi->biPlanes = 1;
	lpbi->biXPelsPerMeter = lpbi->biYPelsPerMeter = 0;
}

void CAccumulate::GetResult(LPBITMAPINFOHEADER lpbi, bool bInitBI)
{
	if (bInitBI)
		InitHeader(lpbi);
	bool bRGB = lpbi->biBitCount!=8;
	if (bRGB)
		DoGetTrueColor(lpbi);
	else
		DoGetGS(lpbi);
}

