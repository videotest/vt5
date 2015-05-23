#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "DocView5.h"
#include "misc_utils.h"
#include "PhaseActions.h"
#include "PropBag.h"
#include <stdio.h>
#include <alloc.h>

inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}

static void _SetNamedDataInt(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, int nValue)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return;
	INamedDataPtr sptrNDO(punk);
	if (sptrNDO == 0)
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}
	sptrNDO->SetupSection(bstrSect);
	sptrNDO->SetValue(bstrKey, _variant_t(long(nValue)));
}

static int _GetNamedDataInt(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, int nDefault)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return nDefault;
	INamedDataPtr sptrNDO(punk);
	if (sptrNDO == 0) return nDefault;
	sptrNDO->SetupSection(bstrSect);
	_variant_t var;
	sptrNDO->GetValue(bstrKey, &var);
	if (var.vt == VT_I2)
		return var.iVal;
	else if (var.vt == VT_I4)
		return var.lVal;
	else
		return nDefault;
}

_ainfo_base::arg	CBinThresholdPhaseInfo::s_pargs[] = 
{
	{"Low",szArgumentTypeInt, "0", true, false },
	{"High",szArgumentTypeInt, "256", true, false },
	{"Phase",szArgumentTypeInt, "0", true, false },
	{"BinImage",szArgumentTypeImage, 0, true, true },
	{"BinResult",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinThresholdPhase
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinThresholdPhase::CBinThresholdPhase()
{
}

bool CBinThresholdPhase::InvokeFilter()
{
	IImage3Ptr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();
	int nLowThreshold = GetArgLong("Low");
	nLowThreshold = min(max(nLowThreshold,0),256);
	int nHighThreshold = GetArgLong("High");
	nHighThreshold = min(max(nHighThreshold,nLowThreshold),256);
	nLowThreshold = nLowThreshold*256;
	nHighThreshold = nHighThreshold*256;
	int nPhase = GetArgLong("Phase");
	int cx,cy;
	pSrc->GetSize(&cx,&cy);
	color *pRow,*pRowR,*pRowG,*pRowB;
	byte  *pRowMask,*pRowDst;
	IUnknownPtr punkCC;
	pSrc->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	_bstr_t bstrName(bstr);
	::SysFreeString(bstr);
	_point offset;
	pSrc->GetOffset(&offset);
	if(pDst->CreateNew(cx, cy)!=S_OK) return false;
	pDst->SetOffset(offset, TRUE);
	StartNotification(cy);
	if (bstrName == _bstr_t("GRAY"))
	{
		for (int y = 0; y < cy; y++)
		{
			pSrc->GetRow(y, 0, &pRow);
			pSrc->GetRowMask(y, &pRowMask);
			pDst->GetRow(&pRowDst, y, TRUE);
			for (int x = 0; x < cx; x++)
				if (pRowMask[x] != 0 && pRow[x] >= nLowThreshold && pRow[x] <= nHighThreshold)
					pRowDst[x] = 0xFF;
		}
	}
	else if (bstrName == _bstr_t("RGB"))
	{
		for (int y = 0; y < cy; y++)
		{
			pSrc->GetRow(y, 0, &pRowR);
			pSrc->GetRow(y, 1, &pRowG);
			pSrc->GetRow(y, 2, &pRowB);
			pSrc->GetRowMask(y, &pRowMask);
			pDst->GetRow(&pRowDst, y, TRUE);
			for (int x = 0; x < cx; x++)
				if (pRowMask[x] != 0)
				{
					color br = Bright(pRowB[x], pRowG[x], pRowR[x]);
					if (br >= nLowThreshold && br <= nHighThreshold)
						pRowDst[x] = 0xFF;
				}
		}
	}
	_SetNamedDataInt(pDst, _bstr_t("Phase"), _bstr_t("PhaseNum"), nPhase);
	FinishNotification();
	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//CSwitchPhase
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CSwitchPhaseInfo::s_pargs[] = 
{
	{"Phase",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};

CSwitchPhase::CSwitchPhase()
{
}

HRESULT CSwitchPhase::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	INamedPropBagPtr bag(m_ptrTarget);
	if (bag==0) return E_FAIL;
	long lPhase = GetArgLong("Phase");
	bag->SetProperty(_bstr_t("Phase"),_variant_t(lPhase));
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CNewPhase
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CNewPhaseInfo::s_pargs[] = 
{
	{"PhaseNumber",szArgumentTypeInt, "-1", true, false },
	{"Name",szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

CNewPhase::CNewPhase()
{
}

IUnknown *CNewPhase::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}

HRESULT CNewPhase::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	IDataContext2Ptr dc(m_ptrTarget);
	if (dc == 0) return E_FAIL;
	long pos;
	bool bFirst = true;
	int cxNew,cyNew;
	IUnknownPtr punkNew(::CreateTypedObject(_bstr_t(szArgumentTypeBinaryImage)), false);
	IBinaryImagePtr binNew(punkNew);
	IUnknownPtr punk;
	dc->GetFirstObjectPos(_bstr_t(szArgumentTypeBinaryImage), &pos);
	int nPhase,nMaxPhase = 0;
	BOOL bSelect;
	while (pos)
	{
		dc->GetNextObject(_bstr_t(szArgumentTypeBinaryImage), &pos, &punk);
		nPhase = _GetNamedDataInt(punk, _bstr_t("Phase"), _bstr_t("PhaseNum"), -1);
		nMaxPhase = max(nMaxPhase,nPhase+1);
		dc->GetObjectSelect(punk, &bSelect);
		if (bSelect)
		{
			IBinaryImagePtr bin(punk);
			if (bFirst)
			{
				bin->GetSizes(&cxNew, &cyNew);
				binNew->CreateNew(cxNew, cyNew);
				bFirst = false;
				for (int y = 0; y < cyNew; y++)
				{
					byte *pRowDst,*pRowSrc;
					bin->GetRow(&pRowSrc, y, TRUE);
					binNew->GetRow(&pRowDst, y, TRUE);
					for (int x = 0; x < cxNew; x++)
						pRowDst[x] = pRowSrc[x]&0x80?0:0xFF;
				}
			}
			else
			{
				int cx,cy;
				bin->GetSizes(&cx, &cy);
				if (cx == cxNew && cy == cyNew)
				{
					for (int y = 0; y < cyNew; y++)
					{
						byte *pRowDst,*pRowSrc;
						bin->GetRow(&pRowSrc, y, TRUE);
						binNew->GetRow(&pRowDst, y, TRUE);
						for (int x = 0; x < cxNew; x++)
							if (pRowSrc[x]&0x80)
								pRowDst[x] = 0;
					}
				}
			}
		}
	}
	if (!bFirst)
	{
		IViewPtr sptrView(m_ptrTarget);
		if (sptrView == 0) return E_FAIL;
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		if (punkDoc == 0)  return E_FAIL;
		int nPhaseNum = GetArgLong("PhaseNumber");
		if (nPhaseNum == -1) nPhaseNum = nMaxPhase;
		_SetNamedDataInt(binNew, _bstr_t("Phase"), _bstr_t("PhaseNum"), nPhaseNum);
		_bstr_t bstrName = GetArgString("Name");
		INamedObject2Ptr sptrNO2(binNew);
		sptrNO2->SetName(bstrName);
		SetToDocData(punkDoc, binNew);
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CUpdateWindow
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CUpdateWindowInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CUpdateWindow::CUpdateWindow()
{
}

HRESULT CUpdateWindow::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	IWindowPtr wnd(m_ptrTarget);
	if (wnd == 0) return E_FAIL;
	HANDLE hwnd = NULL;
	wnd->GetHandle(&hwnd);
	if (hwnd != 0)
		::UpdateWindow((HWND)hwnd);
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CSleep
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg CSleepInfo::s_pargs[] = 
{
	{"TimeMsec",szArgumentTypeInt, "1000", true, false },
	{0, 0, 0, false, false },
};

CSleep::CSleep()
{
}

HRESULT CSleep::DoInvoke()
{
	CHourglass hg;
	int nMSecs = GetArgLong("TimeMsec");
	Sleep(nMSecs);
	return S_OK;
}


