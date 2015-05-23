#include "StdAfx.h"
#include "focus.h"
#include "ImageUtil.h"
#include "misc_templ.h"
#include "CamValues.h"


CCamIntValue g_nRowsPer1Calc(_T("Focus"), _T("RowsPer1Calc"), 1, 1, 100);

class CFocusContext
{
public:
	LPBITMAPINFOHEADER m_lpbi;
	CRect m_rc;
	int m_cx,m_cy,m_cx4,m_cy4;
	DWORD m_dwRow;
	LPBYTE m_lpDIBData;
	int m_nPane;
	int m_nIncr;

	double m_dSum;
	int m_nSum;

	int Calc(LPBITMAPINFOHEADER lpbi, int nPane, CRect rc)
	{
		Init(lpbi, nPane, rc);
		Init2();
		IterateRows();
		return Result();
	}

	virtual void Init(LPBITMAPINFOHEADER lpbi, int nPane, CRect rc)
	{
		m_lpbi = lpbi;
		m_cx = lpbi->biWidth;
		m_cy = lpbi->biHeight;
		m_cx4 = m_cx/4;
		m_cy4 = m_cy/4;
		m_dwRow = DIBRowSize(m_lpbi);
		m_lpDIBData = (LPBYTE)DIBData(m_lpbi);
		m_nIncr = m_lpbi->biBitCount/8;
		m_nPane = max(nPane,m_nIncr-1);
		m_dSum = 0.;
		m_nSum = 0;
		m_rc = rc;
	}

	virtual void Init2() {}

	LPBYTE Row(int y)
	{
		return m_lpDIBData+y*m_dwRow;
	}

	void AddValue(double dVal)
	{
		m_dSum += dVal;
		m_nSum++;
	}
	void AddValue(int nVal)
	{
		AddValue((double)nVal);
	}
	int Result()
	{
		return (int)(m_nSum>0?m_dSum/m_nSum:0.);
	}
	void IterateRows()
	{
		for (int y = m_rc.top; y < m_rc.bottom; y += g_nRowsPer1Calc)
		{
			LPBYTE lpRow = Row(y);
			OnRow(lpRow);
		}
	}
/*	void IteratePixels(LPBYTE lpRow)
	{
		for (int x = m_cx4; x < m_cx-m_cx4; x++)
		{
			BYTE byPix = lpRow[x*m_nIncr+m_nPane];
			OnPixel(x, byPix);
		}
	}*/

	virtual void OnRow(LPBYTE lpRow) {}
//	virtual void OnPixel(int x, BYTE byPixel) {}
};

class CFocusCtx0 : public CFocusContext
{
public:
	void OnRow(LPBYTE lpRow)
	{
		BYTE byMin = 0xFF, byMax = 0;
		for (int x = m_rc.left; x < m_rc.right; x++)
		{
			BYTE byPix = lpRow[x*m_nIncr+m_nPane];
			if (byPix < byMin) byMin = byPix;
			if (byPix > byMax) byMax = byPix;
		}
		AddValue(double(abs(byMax-byMin)));
	}
};

class CFocusCtx1 : public CFocusContext
{
public:
	void OnRow(LPBYTE lpRow)
	{
		int nMax = 0;
		for (int x = m_rc.left; x < m_rc.right; x++)
		{
			int nDiff = abs(int(lpRow[x+1])-int(lpRow[x]));
			if (nDiff > nMax) nMax = nDiff;
		}
		AddValue(double(nMax));
	}
};

static int _sort(const void*p1,const void*p2)
{
	return *(color*)p1>*(color*)p2?1:*(color*)p1==*(color*)p2?0:-1;
}

class CFocusCtx2 : public CFocusContext
{
public:
	_ptr_t2<byte> arr;
	void Init2()
	{
		arr.alloc(m_cx/2);
	}
	void OnRow(LPBYTE lpRow)
	{
		byte byMin = 0xFF,byMax = 0;
		memcpy((byte*)arr,lpRow+m_rc.left,m_rc.Width());
		qsort((byte*)arr,m_rc.Width(),1,_sort);
		int n = max(m_rc.Width()/8,1);
		double dSum1 = 0.,dSum2 = 0.;
		for (int i = 0; i < n; i++)
			dSum1 += double(arr[i]);
		dSum1 /= n;
		for (i = 0; i < n; i++)
			dSum2 += double(arr[m_rc.Width()-i-1]);
		dSum2 /= n;
		double dDiff = dSum1>dSum2?dSum1-dSum2:dSum2-dSum1;
		AddValue(dDiff);
	}
};

CCamBoolValue g_Focus(_T("Focus"), _T("Enable"), false);
CCamIntValue g_CalcMethod(_T("Focus"), _T("CalcMethod"), 0, 0, 2);
CCamIntValue g_RGBPlane(_T("Focus"), _T("RGBPane"), 1, 0, 2);

CCamIntValue g_MaxFocus(NULL, NULL, 0, INT_MIN, INT_MAX, IRepr_Static);
CCamIntValue g_FocusValue(NULL, NULL, 0, INT_MIN, INT_MAX, IRepr_Static);

int CalcFocus(LPBITMAPINFOHEADER lpbi)
{
	int nPane = lpbi->biBitCount==24?(int)g_RGBPlane:0;
	CRect rcFocus = g_FocusRect.SafeGetRect(lpbi->biWidth, lpbi->biHeight);
	int nFocus;
	if (g_CalcMethod == 1)
	{
		CFocusCtx1 c;
		nFocus = c.Calc(lpbi, nPane, rcFocus);
	}
	else if (g_CalcMethod == 2)
	{
		CFocusCtx2 c;
		nFocus = c.Calc(lpbi, nPane, rcFocus);
	}
	else
	{
		CFocusCtx0 c;
		nFocus = c.Calc(lpbi, nPane, rcFocus);
	}
	g_FocusValue.SetValueInt(nFocus, true);
	if (nFocus > (int)g_MaxFocus)
		g_MaxFocus.SetValueInt(nFocus, true);
	return nFocus;
}

CFocusRect::CFocusRect() : CCamValueRect(_T("Focus\\Rect"))
{
}

bool CFocusRect::NeedDrawRect()
{
	return __super::NeedDrawRect()&&(bool)g_Focus;
}

CFocusRect g_FocusRect;

