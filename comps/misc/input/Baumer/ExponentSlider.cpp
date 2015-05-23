// ExponentSlider.cpp : implementation file
//

#include "stdafx.h"
#include "baumer.h"
#include "ExponentSlider.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int __GetTickValue(int nValue)
{
	int nTickValue = 1;
	while (nTickValue*10 < nValue)
		nTickValue *= 10;
	return nTickValue;
}

void FillExponentArray(CUIntArray &arr, int nMin, int nMax)
{
	int nTick = __GetTickValue(nMin);
	int nMin1 = nMin/nTick*nTick;
	for (int i = nMin1; i < nMax; i += nTick)
	{
		arr.Add(i);
		if (i >= nTick*10)
			nTick *= 10;
	}
	arr.Add(i);
}

/////////////////////////////////////////////////////////////////////////////
// CExponentSlider

CExponentSlider::CExponentSlider()
{
}

CExponentSlider::~CExponentSlider()
{
}


BEGIN_MESSAGE_MAP(CExponentSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CExponentSlider)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExponentSlider message handlers

int CExponentSlider::IntPosByExtPos(int nValue)
{
	int i = 0;
	if (nValue > (int)m_arr[0])
	{
		for (i = 0; i < (int)m_arr.GetSize()-1; i++)
		{
			if (nValue < (int)m_arr[i+1])
				break;
		}
	}
	return i;
}

void CExponentSlider::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	FillExponentArray(m_arr, nMin, nMax);
	CSliderCtrl::SetRange(0,m_arr.GetSize()-1,bRedraw);
}


void CExponentSlider::SetPos(int nValue)
{
	CSliderCtrl::SetPos(IntPosByExtPos(nValue));
}

int CExponentSlider::GetPos()
{
	int n = CSliderCtrl::GetPos();
	return m_arr[n];
}

int CExponentSlider::NextValue(int nPos, int nDelta)
{
	int i = IntPosByExtPos(nPos);
	i += nDelta;
	i = max(i,0);
	i = min(i,m_arr.GetSize()-1);
	return m_arr[i];
}

