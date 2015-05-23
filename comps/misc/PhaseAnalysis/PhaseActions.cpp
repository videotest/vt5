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

/*inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
} */

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
				if (pRowMask[x] != 0 && pRow[x] >= nLowThreshold && pRow[x] < nHighThreshold)
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
	SetModified(true);
	return true;
}

_ainfo_base::arg	CBinThresholdAllPhasesInfo::s_pargs[] = 
{
	{"Image",szArgumentTypeImage, 0, true, true },
	{"BinResult",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinThresholdAllPhases
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinThresholdAllPhases::CBinThresholdAllPhases()
{
}

bool CBinThresholdAllPhases::InvokeFilter()
{
	IImage3Ptr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();
	int nPhases = ::GetValueInt(::GetAppUnknown(), "\\Phase", "PhasesCount", 0);
	_ptr_t2<byte> phases(256);
	memset((byte*)phases, 0, 256);
//	char szBuff[256];
//	sprintf(szBuff, "BinThresholdAllPhases, PhasesCount=%d\n", nPhases);
//	OutputDebugString(szBuff);
	for (byte iPhase = 0; iPhase < nPhases; iPhase++)
	{
		char szBuffLow[50],szBuffHigh[50];
		sprintf(szBuffLow, "Phase%dLow", iPhase+1);
		sprintf(szBuffHigh, "Phase%dHigh", iPhase+1);
		int nLow = ::GetValueInt(::GetAppUnknown(), "\\Phase", szBuffLow, -1);
		int nHigh = ::GetValueInt(::GetAppUnknown(), "\\Phase", szBuffHigh, -1);
		if (nLow < 0 || nHigh < 0) continue;
		for (int i = nLow; i <= min(nHigh,255); i++)
			phases[i] = 0x80+iPhase;
//		sprintf(szBuff, " Phase%d %d-%d\n", iPhase, nLow, nHigh);
//		OutputDebugString(szBuff);
	}
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
				if (pRowMask[x] != 0)
					pRowDst[x] = phases[pRow[x]>>8];
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
					pRowDst[x] = phases[br>>8];
				}
		}
	}
	_SetNamedDataInt(pDst, _bstr_t("\\Phase"), _bstr_t("PhaseNum"), -1);
	FinishNotification();
	SetModified(true);
	return true;
}

_ainfo_base::arg	CNewPhasePackedInfo::s_pargs[] = 
{
	{"NewPhase",szArgumentTypeInt, "-1", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinResult",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//CNewPhasePacked
////////////////////////////////////////////////////////////////////////////////////////////////////
CNewPhasePacked::CNewPhasePacked()
{
}

bool CNewPhasePacked::InvokeFilter()
{
	IBinaryImagePtr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();
	int nPhase = GetArgLong("NewPhase");
	int nPhases = ::GetValueInt(::GetAppUnknown(), "Phase", "PhasesCount", 0);
	if (nPhase == -1)
		nPhase = nPhases;
	if (nPhase >=  nPhases)
		::SetValue(::GetAppUnknown(), "Phase", "PhasesCount", (long)(nPhase+1));
	int cx,cy;
	pSrc->GetSizes(&cx,&cy);
	_point offset;
	pSrc->GetOffset(&offset);
	if(pDst->CreateNew(cx, cy)!=S_OK) return false;
	pDst->SetOffset(offset, TRUE);
	byte bySet = 0x80+nPhase;
	StartNotification(cy);
	byte *pRow,*pRowDst;
	for (int y = 0; y < cy; y++)
	{
		pSrc->GetRow(&pRow, y, TRUE);
		pDst->GetRow(&pRowDst, y, TRUE);
		for (int x = 0; x < cx; x++)
			if (pRow[x] < 0x80)
				pRowDst[x] = bySet;
			else
				pRowDst[x] = pRow[x];
	}
	_SetNamedDataInt(pDst, _bstr_t("Phase"), _bstr_t("PhaseNum"), -1);
	FinishNotification();
	SetModified(true);
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
	LONG_PTR pos;
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
//CPackPhases
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CPackPhaseInfo::s_pargs[] = 
{
	{"PackedName",szArgumentTypeString, "", true, false },
	{"UnpackedName",szArgumentTypeString, "", true, false },
	{"Phase",szArgumentTypeString, "-1", true, false },
	{0, 0, 0, false, false },
};

CPackPhase::CPackPhase()
{
}

IUnknown *CPackPhase::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}

static IUnknownPtr _FindObject(IUnknown *punkView, _bstr_t bstrName, _bstr_t bstrType)
{
	if (bstrName.length() > 0)
	{
		IViewPtr sptrView(punkView);
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		return IUnknownPtr(::GetObjectByName(punkDoc, bstrName, bstrType), false);
	}
	else
		return IUnknownPtr(::GetActiveObjectFromContext(punkView, bstrType), false);
}

HRESULT CPackPhase::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	IDataContext2Ptr dc(m_ptrTarget);
	if (dc == 0) return E_FAIL;
	_bstr_t bstrPackName = GetArgString("PackedName");
	_bstr_t bstrUnpName = GetArgString("UnpackedName");
	int nPhase = GetArgLong("Phase");
	if (bstrPackName.length() <= 0 || bstrUnpName.length() <= 0) return E_INVALIDARG;
	IUnknownPtr punkUnp = _FindObject(m_ptrTarget, bstrUnpName, _bstr_t(szArgumentTypeBinaryImage));
	IBinaryImagePtr bin(punkUnp);
	if (bin == 0) return E_INVALIDARG;
	if (nPhase == -1)
		nPhase = _GetNamedDataInt(punkUnp, _bstr_t("Phase"), _bstr_t("PhaseNum"), -1);
	int cx,cy;
	bin->GetSizes(&cx, &cy);
	if (nPhase == -1 || cx <= 0 || cy <= 0) return E_INVALIDARG;
	IUnknownPtr punkPack(_FindObject(m_ptrTarget, bstrPackName, _bstr_t(szArgumentTypeBinaryImage)));
	bool bCreate = punkPack==0;
	if (bCreate)
		punkPack = IUnknownPtr(::CreateTypedObject(_bstr_t(szArgumentTypeBinaryImage)), false);
	IBinaryImagePtr binNew(punkPack);
	if (binNew == 0) return E_FAIL;
	int cxNew,cyNew;
	binNew->GetSizes(&cxNew, &cyNew);
	if (cxNew <= 0 || cyNew <= 0)
		binNew->CreateNew(cx, cy);
	else if (cxNew != cx || cyNew != cy)
		return E_INVALIDARG;
	byte byPhase = (byte)(0x80+nPhase);
	for (int y = 0; y < cy; y++)
	{
		byte *pRowDst,*pRowSrc;
		bin->GetRow(&pRowSrc, y, TRUE);
		binNew->GetRow(&pRowDst, y, TRUE);
		for (int x = 0; x < cx; x++)
			if (pRowSrc[x]==0xFF)
				pRowDst[x] = byPhase;
	}
	if (bCreate)
	{
		IViewPtr sptrView(m_ptrTarget);
		if (sptrView == 0) return E_FAIL;
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		if (punkDoc == 0)  return E_FAIL;
		_SetNamedDataInt(binNew, _bstr_t("Phase"), _bstr_t("PhaseNum"), -1);
		INamedObject2Ptr sptrNO2(binNew);
		sptrNO2->SetName(bstrPackName);
		SetToDocData(punkDoc, binNew);
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CUnpackPhases
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CUnpackPhaseInfo::s_pargs[] = 
{
	{"UnpackedName",szArgumentTypeString, "", true, false },
	{"PackedName",szArgumentTypeString, "", true, false },
	{"Phase",szArgumentTypeInt, 0, true, false },
	{0, 0, 0, false, false },
};

CUnpackPhase::CUnpackPhase()
{
}

IUnknown *CUnpackPhase::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}

HRESULT CUnpackPhase::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	IDataContext2Ptr dc(m_ptrTarget);
	if (dc == 0) return E_FAIL;
	_bstr_t bstrPackName = GetArgString("PackedName");
	_bstr_t bstrUnpName = GetArgString("UnpackedName");
	if (bstrPackName.length() <= 0 || bstrUnpName.length() <= 0) return E_INVALIDARG;
	IUnknownPtr punkPack(_FindObject(m_ptrTarget, bstrPackName, _bstr_t(szArgumentTypeBinaryImage)));
	IBinaryImagePtr binPack(punkPack);
	if (binPack == 0) return E_INVALIDARG;
	int nPhase = GetArgLong("Phase");
	int cx,cy;
	binPack->GetSizes(&cx, &cy);
	if (cx <= 0 || cy <= 0) return E_INVALIDARG;
	IUnknownPtr punkUnp = _FindObject(m_ptrTarget, bstrUnpName, _bstr_t(szArgumentTypeBinaryImage));
	bool bCreate = punkUnp==0;
	if (bCreate)
		punkUnp = IUnknownPtr(::CreateTypedObject(_bstr_t(szArgumentTypeBinaryImage)), false);
	IBinaryImagePtr binUnp(punkUnp);
	if (binUnp == 0) return E_INVALIDARG;
	binUnp->CreateNew(cx, cy);
	byte byPhase = (byte)(0x80+nPhase);
	for (int y = 0; y < cy; y++)
	{
		byte *pRowUnp,*pRowPack;
		binUnp->GetRow(&pRowUnp, y, TRUE);
		binPack->GetRow(&pRowPack, y, TRUE);
		for (int x = 0; x < cx; x++)
			if (pRowPack[x]==byPhase)
				pRowUnp[x] = 0xFF;
	}
	if (bCreate)
	{
		IViewPtr sptrView(m_ptrTarget);
		if (sptrView == 0) return E_FAIL;
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		if (punkDoc == 0)  return E_FAIL;
		_SetNamedDataInt(binUnp, _bstr_t("Phase"), _bstr_t("PhaseNum"), nPhase);
		INamedObject2Ptr sptrNO2(binUnp);
		sptrNO2->SetName(bstrUnpName);
		SetToDocData(punkDoc, binUnp);
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CUpdateWindow
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg CUpdateWindowInfo::s_pargs[] = 
{
	{"Invalidate",szArgumentTypeInt, "0", true, false },
	{"UpdateChildren",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

CUpdateWindow::CUpdateWindow()
{
}

static void InvalidateWindowTree(HWND hwnd)
{
	::InvalidateRect((HWND)hwnd, NULL, TRUE);
	for( HWND hwndChild = ::GetWindow(hwnd, GW_CHILD); hwndChild != NULL; hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT))
		InvalidateWindowTree(hwndChild);
}


static void UpdateWindowTree(HWND hwnd)
{
	::UpdateWindow((HWND)hwnd);
	for( HWND hwndChild = ::GetWindow(hwnd, GW_CHILD); hwndChild != NULL; hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT))
		UpdateWindowTree(hwndChild);
}


HRESULT CUpdateWindow::DoInvoke()
{
	if (m_ptrTarget == 0) return E_FAIL;
	BOOL bInvalidate = GetArgLong("Invalidate");
	BOOL bUpdateChildren = GetArgLong("UpdateChildren");
	IWindowPtr wnd(m_ptrTarget);
	if (wnd == 0) return E_FAIL;
	HANDLE hwnd = NULL;
	wnd->GetHandle(&hwnd);
	if (hwnd != 0)
	{
		if (bUpdateChildren)
		{
			if (bInvalidate)
				InvalidateWindowTree((HWND)hwnd);
			UpdateWindowTree((HWND)hwnd);
		}
		else
		{
			if (bInvalidate)
				::InvalidateRect((HWND)hwnd, NULL, TRUE);
			::UpdateWindow((HWND)hwnd);
		}
	}
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


