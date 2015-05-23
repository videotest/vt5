#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
//#include "DocView5.h"
#include "misc_utils.h"
#include "Etalon.h"
#include <stdio.h>
#include <alloc.h>

inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CApproxObjSize
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CApproxObjSizeInfo::s_pargs[] = 
{
	{"Lines",szArgumentTypeInt, "1", true, false },
	{"Low",szArgumentTypeInt, "128", true, false },
	{"High",szArgumentTypeInt, "255", true, false },
	{"MinSize",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

CApproxObjSize::CApproxObjSize()
{
}

HRESULT CApproxObjSize::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	int nLow = GetArgLong("Low");
	int nHigh = GetArgLong("High");
	int nLines = GetArgLong("High");
	int nMinSize = GetArgLong("MinSize");
	IUnknownPtr punkImage(::GetActiveObjectFromContext(m_ptrTarget, szArgumentTypeImage), false);
	if (punkImage == 0)
		return E_FAIL;
	IImage3Ptr sptrImage(punkImage);
	int cx,cy;
	sptrImage->GetSize(&cx, &cy);
	int nLine = cy/nLines;
	IUnknownPtr punkCC;
	sptrImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	if (sptrCC == 0)
		return E_FAIL;
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	_bstr_t bstrName(bstr);
	::SysFreeString(bstr);
	if (bstrName != _bstr_t("Gray") && bstrName != _bstr_t("RGB")) return E_FAIL;
	bool bRGB = bstrName != _bstr_t("RGB");
	bool bObjNow;
	long lSum = 0;
	int nObjs = 0;
	for (int y = nLine/2; y < cy; y += nLine)
	{
		color *pRow0,*pRow1,*pRow2;
		sptrImage->GetRow(y, 0, &pRow0);
		if (bRGB)
		{
			sptrImage->GetRow(y, 1, &pRow1);
			sptrImage->GetRow(y, 2, &pRow2);
		}
		int x = 0;
		int nObjBeg;
		for (x = 0; x < cx; x++)
		{
			int nCur = bRGB?Bright(pRow2[x],pRow1[x],pRow0[x]):pRow0[x];
			bool bObj = nCur >= nLow && nCur <= nHigh;
			if (x == 0)
			{
				if (bObj)
					nObjBeg = 0;
			}
			else if (bObj)
			{
				if (!bObjNow)
					nObjBeg = x;
			}
			else 
			{
				if (bObjNow && x-nObjBeg>=nMinSize)
				{
					lSum += x-nObjBeg;
					nObjs++;
				}
			}
			bObjNow = bObj;
		}
	}
	long nObjSize = lSum/nObjs;
	::SetValue(::GetAppUnknown(), "Metall", "ApproximateObjectSize", nObjSize);
	return S_OK;
}


