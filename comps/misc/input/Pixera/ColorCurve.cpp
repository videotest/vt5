#include "StdAfx.h"
#include "ColorCurve.h"
#include "PixSdk.h"
#include "StdProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static bool ReadCurvePoints(CArray<CPoint,CPoint&> &arrPts, LPCTSTR lpszSection)
{
	for (int i = 0; i < 256; i++)
	{
		TCHAR szBuff[256];
		_itot(i, szBuff, 10);
		int y = CStdProfileManager::m_pMgr->GetProfileInt(lpszSection, szBuff, -1, false, true);
		if (y >= 0 && y <= 255)
			arrPts.Add(CPoint(i,y));
	}
	if (arrPts.GetSize() == 0)
		return false;
	if (arrPts[0].x > 0)
		arrPts.InsertAt(0, CPoint(0,0));
	if (arrPts[arrPts.GetSize()-1].x < 255)
		arrPts.Add(CPoint(255,255));
	return true;
}

static void MakeCurveByPoints(BYTE *pData, CArray<CPoint,CPoint&> &arrPts)
{
	int i;
	if (arrPts.GetSize() == 0)
	{
		for (int i = 0; i < 256; i++)
			pData[i] = i;
		return;
	}
	for (i = 0; i < arrPts.GetSize()-1; i++)
	{
		CPoint pt1 = arrPts[i];
		CPoint pt2 = arrPts[i+1];
		if (pt2.x == pt1.x) continue;
		for (int x = pt1.x; x < pt2.x; x++)
		{
			int y = pt1.y+(pt2.y-pt1.y)*(x-pt1.x)/(pt2.x-pt1.x);
			pData[x] = y;
		}
	}
	CPoint pt = arrPts[arrPts.GetSize()-1];
	pData[pt.x] = (BYTE)pt.y;
}

void CColorCurve::Load()
{
	LPBYTE pbRed = m_pbRed;
	LPBYTE pbGreen = m_pbGreen;
	LPBYTE pbBlue = m_pbBlue;
	CArray<CPoint,CPoint&> arrRed,arrGreen,arrBlue;
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("ColorCurve"), _T("Enable"), FALSE, true, true))
	{
		ReadCurvePoints(arrRed, _T("ColorCurve\\Red"));
		ReadCurvePoints(arrGreen, _T("ColorCurve\\Green"));
		ReadCurvePoints(arrBlue, _T("ColorCurve\\Blue"));
	}
	if (arrRed.GetSize() == 0 && arrGreen.GetSize() == 0 && arrBlue.GetSize() == 0)
	{
		m_bCurve = false;
		m_pbRed = NULL;
		m_pbGreen = NULL;
		m_pbBlue = NULL;
	}
	else
	{
		m_pbRed = new BYTE[256];
		m_pbGreen = new BYTE[256];
		m_pbBlue = new BYTE[256];
		MakeCurveByPoints(m_pbRed, arrRed);
		MakeCurveByPoints(m_pbGreen, arrGreen);
		MakeCurveByPoints(m_pbBlue, arrBlue);
		m_bCurve = true;
	}
	delete pbRed;
	delete pbGreen;
	delete pbBlue;
}


void CColorCurve::Convert(LPBITMAPINFOHEADER lpbi)
{
	DWORD dwRow = (lpbi->biWidth*3+3)/4*4;
	LPBYTE lpData = (LPBYTE)(lpbi+1);
	int cy = abs(lpbi->biHeight);
	int cx = lpbi->biWidth;
	for (int y = 0; y < cy; y++)
	{
		LPBYTE lpData1 = lpData;
		for (int x = 0; x < cx; x++)
		{
			*lpData1 = m_pbBlue[*lpData1];
			lpData1++;
			*lpData1 = m_pbGreen[*lpData1];
			lpData1++;
			*lpData1 = m_pbRed[*lpData1];
			lpData1++;
		}
		lpData += dwRow;
	}
}

