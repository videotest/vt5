#include "StdAfx.h"
//#include "common.h"
#include <ComDef.h>
#include "Defs.h"
#include "Image5.h"
#include "MV500VT5Grab.h"
#include "StdProfile.h"

#define RANGE(x, lo, hi)	max(lo, min(hi, x))

static void YUVToPlanes(color *pRed, color *pGreen, color *pBlue, unsigned y, int u, int v)
{
    int ScaledY = RANGE(y, 16, 235) * 1024;
//    int ScaledY = y * 1024;
    int red1, green1, blue1;
    int red, green, blue;
    red1 = (ScaledY + (1404 * v)) / 1024;
    green1 = (ScaledY - (715 * v) - (344 * u)) / 1024;
    blue1 = (ScaledY + (1774 * u)) / 1024;
    red = RANGE(red1, 0, 255);
    green = RANGE( green1, 0, 255);
    blue = RANGE( blue1, 0, 255);

	*pBlue = blue<<8;
	*pGreen = green<<8;
	*pRed = red<<8;
}

static void _CopyGS8ToGS(BOOL bVerticalMirror, BOOL bHorizontalMirror, IImage3 *pimg, LPBYTE pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (bHorizontalMirror)
	{
		if (!bVerticalMirror)
			pSrc += (dwRowSrc)*(cy-1);
		long lRowSrc = bVerticalMirror?dwRowSrc:-(long)dwRowSrc;
		for (int y = 0; y < cy; y++)
		{
			LPBYTE pSrc1 = pSrc;
			color *pRow;
			pimg->GetRow(y, 0, &pRow);
			for (int x = 0; x < cx; x++)
				pRow[x] = ((color)pSrc1[cx-1-x])<<8;
			pSrc += lRowSrc;
		}
	}
	else if (!bVerticalMirror)
	{
		pSrc += (dwRowSrc)*(cy-1);
		for (int y = 0; y < cy; y++)
		{
			LPBYTE pSrc1 = pSrc;
			color *pRow;
			pimg->GetRow(y, 0, &pRow);
			for (int x = 0; x < cx; x++)
				*pRow++ = ((color)*pSrc1++)<<8;
			pSrc -= dwRowSrc;
		}
	}
	else
	{
		for (int y = 0; y < cy; y++)
		{
			LPBYTE pSrc1 = pSrc;
			color *pRow;
			pimg->GetRow(y, 0, &pRow);
			for (int x = 0; x < cx; x++)
				*pRow++ = ((color)*pSrc1++)<<8;
			pSrc += dwRowSrc;
		}
	}
}

static void _CopyYUVToGS(BOOL bVerticalMirror, IImage3 *pimg, LPWORD pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc/2)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRow;
			pimg->GetRow(y, 0, &pRow);
		for (int x = 0; x < cx; x++)
		{
			WORD w = pSrc[x];
			pRow[x] = w<<8;
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc/2;
		else
			pSrc += dwRowSrc/2;
	}
}

static void _CopyYUVToRGB(BOOL bVerticalMirror, IImage3 *pimg, LPWORD pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc/2)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRowR,*pRowG,*pRowB;
		pimg->GetRow(y, 0, &pRowR);
		pimg->GetRow(y, 1, &pRowG);
		pimg->GetRow(y, 2, &pRowB);
		for (int x = 0; x < cx; x+=2)
		{
			WORD w1 = pSrc[x];
			WORD w2 = pSrc[x+1];
			unsigned y1,y2;
			int u,v;
			y1 = w1&0xFF;
			u  = (w1>>8)-128;
			y2 = w2&0xFF;
			v  = (w2>>8)-128;
			YUVToPlanes(pRowR+x,pRowG+x,pRowB+x,y1,u,v);
			YUVToPlanes(pRowR+x+1,pRowG+x+1,pRowB+x+1,y2,u,v);
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc/2;
		else
			pSrc += dwRowSrc/2;
	}
}

static void _CopyRGB888ToGS(BOOL bVerticalMirror, IImage3 *pimg, LPBYTE pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRow;
			pimg->GetRow(y, 0, &pRow);
		for (int x = 0; x < cx; x++)
		{
			pRow[x] = ((color)pSrc[3*x+0])<<8;
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc;
		else
			pSrc += dwRowSrc;
	}
}

static void _CopyRGB888ToRGB(BOOL bVerticalMirror, IImage3 *pimg, LPBYTE pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRowR,*pRowG,*pRowB;
		pimg->GetRow(y, 0, &pRowR);
		pimg->GetRow(y, 1, &pRowG);
		pimg->GetRow(y, 2, &pRowB);
		for (int x = 0; x < cx; x++)
		{
			pRowR[x] = ((color)pSrc[3*x+2])<<8;
			pRowG[x] = ((color)pSrc[3*x+1])<<8;
			pRowB[x] = ((color)pSrc[3*x+0])<<8;
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc;
		else
			pSrc += dwRowSrc;
	}
}

static void _CopyRGB555ToGS(BOOL bVerticalMirror, IImage3 *pimg, LPWORD pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc/2)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRow;
		pimg->GetRow(y, 0, &pRow);
		for (int x = 0; x < cx; x++)
		{
			WORD w = pSrc[x];
			pRow[x] = ((BYTE)((w>>7)&0xFF))<<8;
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc/2;
		else
			pSrc += dwRowSrc/2;
	}
}

static void _CopyRGB555ToRGB(BOOL bVerticalMirror, IImage3 *pimg, LPWORD pSrc, DWORD dwRowSrc, int cx, int cy)
{
	if (!bVerticalMirror)
		pSrc += (dwRowSrc/2)*(cy-1);
	for (int y = 0; y < cy; y++)
	{
		color *pRowR,*pRowG,*pRowB;
		pimg->GetRow(y, 0, &pRowR);
		pimg->GetRow(y, 1, &pRowG);
		pimg->GetRow(y, 2, &pRowB);
		for (int x = 0; x < cx; x++)
		{
			WORD w = pSrc[x];
			pRowR[x] = ((w>>7)&0xFF)<<8;
			pRowG[x] = ((w>>2)&0xFF)<<8;
			pRowB[x] = ((w<<3)&0xFF)<<8;
		}
		if (!bVerticalMirror)
			pSrc -= dwRowSrc/2;
		else
			pSrc += dwRowSrc/2;
	}
}

void CMV500VT5Grab::CopyGrabbedImage(int nDev, IUnknown *pUnkImg)
{
	CopyGrabbedImage(pUnkImg, m_lpData, 0);
}

void CMV500VT5Grab::CopyGrabbedImage(IUnknown *pUnkImg, LPBYTE lpData, DWORD dwSize)
{
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE, true);
	BOOL bHorizontalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE, true);
	BOOL bGrayScale = IsGrayScale(false);
	int cx = ((m_szGrab.cx+3)>>2)<<2;
	int cy = ((m_szGrab.cy+3)>>2)<<2;
	_bstr_t sCC = bGrayScale?"GRAY":"RGB";
	IImage3Ptr imageNew( pUnkImg );
	imageNew->CreateImage(cx, cy, sCC,-1);
	if (m_GrabMode == GM_GS8)
	{
		DWORD dwRowSrc = ((m_szGrab.cx+3)>>2)<<2;
		_CopyGS8ToGS(bVerticalMirror, bHorizontalMirror, imageNew, lpData, dwRowSrc, cx, cy);
	}
	else if (m_GrabMode == GM_YUV422)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		if (bGrayScale)
			_CopyYUVToGS(bVerticalMirror, imageNew, (LPWORD)lpData, dwRowSrc, cx, cy);
		else
			_CopyYUVToRGB(bVerticalMirror, imageNew, (LPWORD)lpData, dwRowSrc, cx, cy);
	}
	else if (m_GrabMode == GM_RGB888)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*3+3)>>2)<<2;
		if (bGrayScale)
			_CopyRGB888ToGS(bVerticalMirror, imageNew, (LPBYTE)lpData, dwRowSrc, cx, cy);
		else
			_CopyRGB888ToRGB(bVerticalMirror, imageNew, (LPBYTE)lpData, dwRowSrc, cx, cy);
	}
	else if (m_GrabMode == GM_RGB555)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		if (bGrayScale)
			_CopyRGB555ToGS(bVerticalMirror, imageNew, (LPWORD)lpData, dwRowSrc, cx, cy);
		else
			_CopyRGB555ToRGB(bVerticalMirror, imageNew, (LPWORD)lpData, dwRowSrc, cx, cy);
	}
}

void CMV500VT5Grab::CopyGrabbedImage(IDIBProvider *pDIBPrv, int nDev)
{
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	BOOL bHorizontalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
	LPBITMAPINFOHEADER lpbi = NULL;
	BOOL bGrayScale = IsGrayScale(false);
	if (m_GrabMode == GM_GS8)
	{
		ASSERT(bGrayScale);
		DWORD dwRowSrc = ((m_szGrab.cx+3)>>2)<<2;
		DWORD dwRowDst = ((m_szGrab.cx+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(pDIBPrv, m_szGrab, true);
		if (!lpbi) return;
		_CopyGS8(bVerticalMirror, bHorizontalMirror, _GetData(lpbi), (BYTE *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_YUV422)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(pDIBPrv, m_szGrab, bGrayScale?true:false);
		if (!lpbi) return;
		_CopyYUV422(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (WORD *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB888)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*3+3)>>2)<<2;
		DWORD dwRowDst =  bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(pDIBPrv, m_szGrab, bGrayScale?true:false);
		if (!lpbi) return;
		_CopyRGB888(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (BYTE *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB555)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(pDIBPrv, m_szGrab, bGrayScale?true:false);
		if (!lpbi) return;
		_CopyRGB555(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (WORD *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
}


void CMV500VT5Grab::Grab(int nDev, IUnknown *punkImg, bool bWait)
{
	if (bWait)
	{
		for (int i = 0; i < 4; i++)
			MV5WaitForVSync();
	}
	CopyGrabbedImage(nDev,punkImg);
}

void CMV500VT5Grab::OnSetValueString(LPCTSTR lpSec, LPCTSTR lpName, LPCTSTR lpValue, CArray<IInputPreviewSite *,IInputPreviewSite *&> &arrSites)
{
}

void CMV500VT5Grab::OnSetValueInt(LPCTSTR lpSec, LPCTSTR lpName, int nValue, CArray<IInputPreviewSite *,IInputPreviewSite *&> &arrSites)
{
	CString sSec(lpSec);
	CString sEntry(lpName);
	if (sSec == _T("Format") && (sEntry == _T("HorizontalMirror") || sEntry == _T("VerticalMirror") ||
		sEntry == _T("Scale")) || sSec == _T("Source") && (sEntry == _T("GrabWindowX") ||
		sEntry == _T("GrabWindowY") || sEntry == _T("GrabWindowDX") || sEntry == _T("GrabWindowDY") ||
		sEntry == _T("VideoStandard")) || sSec == _T("Settings") && (sEntry == _T("UseGS8") ||
		sEntry == _T("UseYUVMode") || sEntry == _T("UseRGB24") || sEntry == _T("UseOverlay")) ||
		sSec == _T("AVI") && (sEntry == _T("SizeX") || sEntry == _T("SizeY")) && m_bAviPreview )
	{
		if (m_bOnGrab)
		{
			GrabMode gm = m_GrabMode;
			DeinitGrab();
			InitGrab(gm, m_bAviPreview);
			StartGrab();
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->OnChangeSize();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("Connector"))
	{
		if (m_bOnGrab)
		{
			MV5SetVideoChannel(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("Hue"))
	{
		if (sSec == _T("Source") && m_bOnGrab)
		{
			MV5SetHue(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("Brightness"))
	{
		if (m_bOnGrab)
		{
			MV5SetBrightness(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("Contrast"))
	{
		if (m_bOnGrab)
		{
			MV5SetContrast(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("Saturation"))
	{
		if (m_bOnGrab)
		{
			if (IsGrayScale(m_bAviPreview))
				MV5SetSaturation(0);
			else
				MV5SetSaturation(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("GrayScale"))
	{
		if (m_bOnGrab)
		{
			if (IsGrayScale(m_bAviPreview))
				MV5SetSaturation(0);
			else
			{
				int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Saturation"), 128);
				MV5SetSaturation(n);
			}
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("VCRMode"))
	{
		if (m_bOnGrab)
		{
			MV5SetVCRMode(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
	else if (sSec == _T("Source") && sEntry == _T("AGC"))
	{
		if (m_bOnGrab)
		{
			MV5SetAGC(nValue);
			for (int i = 0; i < arrSites.GetSize(); i++)
				arrSites[i]->Invalidate();
		}
	}
}





