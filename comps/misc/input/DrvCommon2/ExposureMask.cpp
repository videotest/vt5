#include "StdAfx.h"
#include "ExposureMask.h"
#include "StdProfile.h"
#include "Utils.h"
#include "ComDef.h"
#include "Defs.h"
#include "image5.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SECNAME _T("Under&OverExposure")

CCamBoolValue g_OUExpEnabled(SECNAME, _T("Enable"), false);


CExposureMask::CExposureMask()
{
	m_bInit = false;
}

void CExposureMask::Init()
{
	if (!m_bInit)
	{
//		m_bEnable = CStdProfileManager::m_pMgr->GetProfileInt(SECNAME, _T("Enable"), 0, true, true)?true:false;
		if ((bool)g_OUExpEnabled)
		{
			m_nUE8 = CStdProfileManager::m_pMgr->GetProfileInt(SECNAME,	_T("UnderExposedThreshold8"),
				0, true, false);
			m_nOE8 = CStdProfileManager::m_pMgr->GetProfileInt(SECNAME, _T("OverExposedThreshold8"),
				255, true, false);
			COLORREF clrUE = CStdProfileManager::m_pMgr->GetProfileColor(SECNAME,
				_T("UnderExposedColor"), RGB(0,128,0), true, true);
			COLORREF clrOE = CStdProfileManager::m_pMgr->GetProfileColor(SECNAME,
				_T("OverExposedColor"), RGB(255,0,0), true, true);
			m_rgbUE.rgbRed = GetRValue(clrUE);
			m_rgbUE.rgbGreen = GetGValue(clrUE);
			m_rgbUE.rgbBlue = GetBValue(clrUE);
			m_rgbUE.rgbReserved = 0;
			m_rgbOE.rgbRed = GetRValue(clrOE);
			m_rgbOE.rgbGreen = GetGValue(clrOE);
			m_rgbOE.rgbBlue = GetBValue(clrOE);
			m_rgbOE.rgbReserved = 0;
		}
		m_bInit = true;
	}
}

void CExposureMask::Reinit()
{
	m_bInit = false;
	Init();
}

bool CExposureMask::IsEnabled()
{
	return (bool)g_OUExpEnabled;
}


void CExposureMask::SetMask(LPBITMAPINFOHEADER lpbi, LPVOID lpData, bool bValidData)
{
	Init();
	if (!(bool)g_OUExpEnabled) return;
	if (lpbi->biBitCount == 8)
	{
		RGBQUAD *pRGB = (RGBQUAD*)(lpbi+1);
		for (int i = 0; i <= m_nUE8; i++)
			pRGB[i] = m_rgbUE;
		for (int i = m_nOE8; i < 256; i++)
			pRGB[i] = m_rgbOE;
	}
	else if (lpbi->biBitCount == 24 && bValidData)
	{
		LPBYTE lpData1 = lpData==NULL?(LPBYTE)(lpbi+1):(LPBYTE)lpData;
		DWORD dwRow = (((lpbi->biWidth*lpbi->biBitCount/8)+3)>>2)<<2;
		for (int y = 0; y < lpbi->biHeight; y++)
		{
			for (int x = 0; x < lpbi->biWidth; x++)
			{
				int br = Bright(lpData1[3*x],lpData1[3*x+1],lpData1[3*x+2]);
				if (br <= m_nUE8)
				{
					lpData1[3*x+0] = m_rgbUE.rgbBlue;
					lpData1[3*x+1] = m_rgbUE.rgbGreen;
					lpData1[3*x+2] = m_rgbUE.rgbRed;
				}
				else if (br >= m_nOE8)
				{
					lpData1[3*x+0] = m_rgbOE.rgbBlue;
					lpData1[3*x+1] = m_rgbOE.rgbGreen;
					lpData1[3*x+2] = m_rgbOE.rgbRed;
				}
			}
			lpData1 += dwRow;
		}
	}
}

void CExposureMask::SetMask(IUnknown *punkImage)
{
	Init();
	if (!(bool)g_OUExpEnabled) return;
	IImage2Ptr sptrImage(punkImage);
	if (sptrImage == 0) return;
	int cx,cy;
	sptrImage->GetSize(&cx, &cy);
	for (int i = 0; i < cy; i++)
	{
		color *pR,*pG,*pB;
		sptrImage->GetRow(i, 0, &pR);
		sptrImage->GetRow(i, 1, &pG);
		sptrImage->GetRow(i, 2, &pB);
		for (int j = 0; j < cx; j++)
		{
			int br = Bright(pR[j]>>8, pG[j]>>8, pB[j]>>8);
			if (br <= m_nUE8)
			{
				pB[j] = ((color)(m_rgbUE.rgbBlue))<<8;
				pG[j] = ((color)(m_rgbUE.rgbGreen))<<8;
				pR[j] = ((color)(m_rgbUE.rgbRed))<<8;
			}
			else if (br >= m_nOE8)
			{
				pB[j] = ((color)(m_rgbOE.rgbBlue))<<8;
				pG[j] = ((color)(m_rgbOE.rgbGreen))<<8;
				pR[j] = ((color)(m_rgbOE.rgbRed))<<8;
			}
		}
	}
}



