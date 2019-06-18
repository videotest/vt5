#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "DocView5.h"
#include "misc_utils.h"
#include "MetallParams.h"
#include "array.h"
#include "Object5.h"
#include "measure5.h"
#include "misc_calibr.h"
#include "misc_new.h"
#include "units_const.h"
#include "Stringer.h"
#include "MeasUtils.h"
#include <stdio.h>
#include <alloc.h>
#include <math.h>
#include <crtdbg.h>

static void ReadImportedParams(_CArray<CImportedParameter,10,10> &ImpParams)
{
	ImpParams.SetSize(0);
	INamedDataPtr sptrNDApp(GetAppUnknown());
	CImportedParameter Param;
	Param.lKey = KEY_PARAM_OBJECT1;
	Param.lKeyObj = -1;
	Param.nObject = 0;
	ImpParams.Add(Param);
	Param.lKey = KEY_PARAM_OBJECT2;
	Param.nObject = 1;
	ImpParams.Add(Param);
	long lCount;
	sptrNDApp->SetupSection(_bstr_t("\\Metall\\BorderParameters"));
	sptrNDApp->GetEntriesCount(&lCount);
	for (long i = 0; i < lCount; i++)
	{
		BSTR bstrName;
		sptrNDApp->GetEntryName(i, &bstrName);
		VARIANT var;
		sptrNDApp->GetValue(bstrName, &var);
		if (var.vt == VT_BSTR)
		{
			BSTR bstrValue = var.bstrVal;
			wchar_t *pwcColon = bstrValue?wcschr(bstrValue, L':'):0;
			if (bstrName != 0 && iswdigit(bstrName[0]) && bstrValue != 0 && iswdigit(bstrValue[0]) )
			{
				long lParamKey = _wtol(bstrName);
				int  nObject = _wtoi(bstrValue);
				long lObjParamKey = pwcColon==NULL?-1:_wtol(pwcColon+1);
				if (lParamKey > 0 && nObject >= 0 && nObject <= 1)
				{
					Param.lKey = lParamKey;
					Param.nObject = nObject;
					Param.lKeyObj = lObjParamKey;
					ImpParams.Add(Param);
				}
			}
		}
		else
		{
			_ASSERTE(!"Неправильная секция [Metall\\BorderParameters] в Shell.Data");
		}
	}
}

static ParameterDescriptor *FindBaseParameter(long lKey)
{
	IUnknownPtr punk(FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement));
	ICompManagerPtr	ptrManager(punk);
	if (ptrManager == NULL)
		return NULL;
	int	nGroupCount;
	ptrManager->GetCount(&nGroupCount);
	for (int nGroup = 0; nGroup < nGroupCount; nGroup++)
	{
		IUnknownPtr	ptrG;
		ptrManager->GetComponentUnknownByIdx(nGroup, &ptrG);
		IMeasParamGroupPtr	ptrGroup(ptrG);
		if (ptrGroup == 0) continue;
		long lPos = 0;
		ptrGroup->GetPosByKey(lKey, &lPos);
		if (lPos)
		{
			ParameterDescriptor *pd = NULL;
			ptrGroup->GetNextParam(&lPos, &pd);
			if (pd)
				return pd;
		}
	}
	return NULL;
}

static void _FreeString(BSTR &bstr)
{
	if (bstr) ::SysFreeString(bstr);
	bstr = NULL;
}

static void _SetBSTR(BSTR &bstr, _bstr_t bstrSet)
{
	if (bstr) ::SysFreeString(bstr);
	bstr = bstrSet.copy();
}

void CMetallParams::FreeParamEx(ParameterDescriptorEx *p)
{
	if (p->bstrDefFormat) ::SysFreeString(p->bstrDefFormat);
	if (p->bstrName) ::SysFreeString(p->bstrName);
	if (p->bstrUnit) ::SysFreeString(p->bstrUnit);
	if (p->bstrUserName) ::SysFreeString(p->bstrUserName);
	delete p;
}


CMetallParams::CMetallParams()
{
	Register(GetAppUnknown(), true, 0);
}

CMetallParams::~CMetallParams()
{
	Register(GetAppUnknown(), false, 0);
}

IUnknown *CMetallParams::DoGetInterface(const IID &iid)
{
	if (iid == IID_IMeasParamGroup)
		return (IMeasParamGroup *)this;
	else if (iid == IID_IInitializeObjectVT)
		return (IInitializeObjectVT *)this;
	else if (iid == IID_IPriority)
		return (IPriority *)this;
	else if (iid == IID_IEventListener)
		return (IEventListener *)this;
	else if (iid == IID_IBorderParameters)
		return (IBorderParameters *)this;
	else
		return CObjectBase::DoGetInterface(iid);
}

GuidKey CMetallParams::GetInternalClassID()
{
	return clsidMetallParams;
}

void CMetallParams::DefineParameter(long lKey, BSTR bstrName, BSTR bstrDefFmt,
	CImportedParameter *pImp)
{
	ParameterDescriptorEx *pdescr = new ParameterDescriptorEx;
	memset(pdescr, 0, sizeof(*pdescr));
	pdescr->cbSize = sizeof(ParameterDescriptor);
	pdescr->bstrName = bstrName==0?0:SysAllocString(bstrName);
	pdescr->bstrUserName = bstrName==0?0:SysAllocString(bstrName);
	pdescr->bstrDefFormat = bstrDefFmt==0?0:SysAllocString(bstrDefFmt);
	pdescr->lKey = lKey;
	pdescr->lEnabled = 0;
	if (pImp)
		pdescr->BorderParam = *pImp;
	else
		pdescr->BorderParam.lKey = -1;
	pdescr->pos = m_Params.add_tail(pdescr);
	if (bstrName != 0)
	{
		_bstr_t sMainSection("\\measurement\\parameters\\");
		_bstr_t sParamsSection = sMainSection+_bstr_t(bstrName);
		::SetValue(::GetAppUnknown(), sParamsSection, "Key", lKey);
	}
}

// Result of operation: for object number only name, key and format are valid, for base
// object's parameters anly key is valid.
void CMetallParams::DefineVolatileParams()
{
	_CArray<CImportedParameter,10,10> arrImportedParams;
	ReadImportedParams(arrImportedParams);
	// Mark all borders parameter as deleted - existing will be unmarked
	for (long lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptorEx *p = m_Params.get(lpos);
		if (p->BorderParam.lKey >= 0)
		{
			p->bZombie = true;
			p->lEnabled = 0;
		}
	}
	for (int i = 0; i < arrImportedParams.GetSize(); i++)
	{
		long lpos;
		GetPosByKey(arrImportedParams.GetPtrAt(i).lKey, &lpos);
		if (lpos) // already exist - mark as not deleted
		{
			ParameterDescriptorEx *pFound = m_Params.get(lpos);
			pFound->BorderParam = arrImportedParams.GetPtrAt(i);
			pFound->bZombie = false;
		}
		else // new - add it. not zombie by default
		{
			if (arrImportedParams.GetPtrAt(i).lKeyObj < 0)
			{
				char szName[50];
				sprintf(szName, "Object%d", arrImportedParams.GetPtrAt(i).nObject+1);
				DefineParameter(arrImportedParams.GetPtrAt(i).lKey, _bstr_t(szName),
					_bstr_t("%0.0f"), &arrImportedParams.GetPtrAt(i));
			}
			else
				DefineParameter(arrImportedParams.GetPtrAt(i).lKey, NULL, NULL,
					&arrImportedParams.GetPtrAt(i));
		}
	}
}

void CMetallParams::ReadParameterSettings(ParameterDescriptorEx *pdescr)
{
	_bstr_t	strSection("\\measurement\\parameters\\");
	strSection += pdescr->bstrName;
	if (pdescr->BorderParam.lKey >= 0)
	{
		pdescr->bstrUnit = GetValueString(GetAppUnknown(), szCountUnits, szUnitName, szDefCountUnits).copy();
		pdescr->fCoeffToUnits = 1.;
		pdescr->lEnabled = 0;
	}
	else
	{
		if (pdescr->lKey == KEY_PARAM_STRINGER_LENGHT || pdescr->lKey == KEY_PARAM_STRINGER_SUM_LENGHT ||
			pdescr->lKey == KEY_PARAM_STRINGER_WIDTH || pdescr->lKey == KEY_PARAM_STRINGER_WIDTH_ASTM45)
		{
			pdescr->bstrUnit = GetValueString(GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits).copy();
			pdescr->fCoeffToUnits = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		}
		else if (pdescr->lKey == KEY_PARAM_STRINGER_OBJECTS)
		{
			pdescr->bstrUnit = GetValueString(GetAppUnknown(), szCountUnits, szUnitName, szDefCountUnits).copy();
			pdescr->fCoeffToUnits = 1.;
		}
		else
			GetUnit(etUndefined, &pdescr->bstrUnit, &pdescr->fCoeffToUnits);
		pdescr->lEnabled = ::GetValueInt(GetAppUnknown(), strSection, "Enable", pdescr->lEnabled);
	}
	_bstr_t	strUserName = pdescr->bstrUserName;
	_bstr_t	strDefFormat = pdescr->bstrDefFormat;
	::SysFreeString(pdescr->bstrUserName);
	::SysFreeString(pdescr->bstrDefFormat);
	pdescr->bstrUserName = ::GetValueString(GetAppUnknown(), strSection, "UserName", strUserName).copy();
	pdescr->bstrDefFormat = ::GetValueString(GetAppUnknown(), strSection, "Format", strDefFormat).copy();
	pdescr->lTableOrder = ::GetValueInt( GetAppUnknown(), strSection, "TableOrder", pdescr->lTableOrder );
}

bool CMetallParams::ReinitParameter(ParameterDescriptorEx *pd)
{
	_bstr_t	strSection("\\measurement\\parameters\\");
	ParameterDescriptor *pdBase = FindBaseParameter(pd->BorderParam.lKeyObj);
	if (pdBase)
	{
		char szName[256];
		sprintf(szName, "Object%d %s", pd->BorderParam.nObject+1, (const char *)_bstr_t(pdBase->bstrName));
		_SetBSTR(pd->bstrName, _bstr_t(szName));
		strSection += pd->bstrName;
		pd->fCoeffToUnits = pdBase->fCoeffToUnits;
		_SetBSTR(pd->bstrUnit, _bstr_t(pdBase->bstrUnit));
		_SetBSTR(pd->bstrDefFormat,_bstr_t(pdBase->bstrDefFormat));
		sprintf(szName, "Object%d %s", pd->BorderParam.nObject+1, (const char *)_bstr_t(pdBase->bstrUserName));
		_bstr_t	strUserName = szName;
		_SetBSTR(pd->bstrUserName,::GetValueString(GetAppUnknown(), strSection, "UserName", strUserName));
		return true;
	}
	return false;
}

void CMetallParams::ReloadState(bool bNonVolatile, bool bObjNums, bool bBaseParams)
{
	for (long lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptorEx *pdescr = m_Params.get(lpos);
		if (pdescr->bZombie) continue;
		if (pdescr->BorderParam.lKey >= 0)
		{
			if (pdescr->BorderParam.lKeyObj < 0)
			{
				if (bObjNums)
					ReadParameterSettings(pdescr);
			}
			else
			{
				if (bBaseParams)
					ReinitParameter(pdescr);
			}
		}
		else
			if (bNonVolatile)
				ReadParameterSettings(pdescr);
	}
}

HRESULT CMetallParams::Initialize()
{
	DefineParameter(KEY_PARAM_STRINGER_LENGHT, _bstr_t("Stringer lenght"), _bstr_t("%0.3f"));
	DefineParameter(KEY_PARAM_STRINGER_SUM_LENGHT, _bstr_t("Stringer summary lenght"), _bstr_t("%0.3f"));
	DefineParameter(KEY_PARAM_STRINGER_WIDTH, _bstr_t("Stringer width"), _bstr_t("%0.3f"));
	DefineParameter(KEY_PARAM_STRINGER_WIDTH_ASTM45, _bstr_t("Stringer width ASTM 45"), _bstr_t("%0.3f"));
	DefineParameter(KEY_PARAM_STRINGER_OBJECTS, _bstr_t("Objects in stringer"), _bstr_t("%0.3f"));
	DefineVolatileParams();
	ReloadState(true,true,false);
	return S_OK;
}

HRESULT CMetallParams::CalcValues(IUnknown *punkCalcObject, IUnknown *punkSource)
{
	ICalcObjectPtr sptrCO(punkCalcObject);
	IUnknownPtr punkAxis(find_child_by_interface(punkCalcObject,IID_IStringerAxis));
	IStringerAxisPtr sptrAxis(punkAxis);
	if (sptrAxis == 0) return S_OK;
	double dLength,dWidth,dSumLenght,dWidth45;
	sptrAxis->GetParameter(KEY_PARAM_STRINGER_LENGHT, &dLength);
	sptrAxis->GetParameter(KEY_PARAM_STRINGER_WIDTH, &dWidth);
	sptrAxis->GetParameter(KEY_PARAM_STRINGER_SUM_LENGHT, &dSumLenght);
	sptrAxis->GetParameter(KEY_PARAM_STRINGER_WIDTH_ASTM45, &dWidth45);
	sptrCO->SetValue(KEY_PARAM_STRINGER_LENGHT, dLength);
	sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH, dWidth);
	sptrCO->SetValue(KEY_PARAM_STRINGER_SUM_LENGHT, dSumLenght);
	sptrCO->SetValue(KEY_PARAM_STRINGER_WIDTH_ASTM45, dWidth45);
	return S_OK;
}

HRESULT CMetallParams::GetParamsCount(long *plCount)
{
	*plCount = m_Params.count();
	return S_OK;
}

HRESULT CMetallParams::GetFirstPos(long *plPos)
{
	*plPos = m_Params.head();
	return S_OK;
}

HRESULT CMetallParams::GetNextParam(long *plPos, struct ParameterDescriptor **ppDescriptior )
{
	if (ppDescriptior) *ppDescriptior = m_Params.get(*plPos);
	*plPos = m_Params.next(*plPos);
	return S_OK;
}

HRESULT CMetallParams::GetPosByKey(long lKey, long *plPos)
{
	*plPos = 0;
	for (long lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptorEx *pdescr = m_Params.get(lpos);
		if (pdescr->lKey == lKey)
		{
			*plPos = lpos;
			break;
		}
	}
	return S_OK;
}

HRESULT CMetallParams::InitializeCalculation(IUnknown *punkContainer)
{
	m_ptrContainer = punkContainer;
	for (long lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptorEx *pdescr = m_Params.get(lpos);
		if (pdescr->bZombie) continue;
		if (pdescr->lEnabled)
			m_ptrContainer->DefineParameter(pdescr->lKey, etUndefined, this, 0);
	}
	DefineBorderParameters(punkContainer,true);
	m_ptrContainer = 0;
	return S_OK;
}

HRESULT CMetallParams::FinalizeCalculation()
{
	return S_OK;
}

HRESULT CMetallParams::GetUnit(long lType, BSTR *pbstr, double *pfCoeffToUnits)
{
	_bstr_t bstrType = GetValueString(GetAppUnknown(), "\\Units\\StringerType", "Units", "Unit");
	if (pbstr) *pbstr = bstrType.copy();
	if (pfCoeffToUnits) *pfCoeffToUnits = 1.;
	return S_OK;
}

void CMetallParams::OnNotify(const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize)
{
	if (!strcmp(pszEvent, szEventNewSettings))
	{
		DefineVolatileParams();
		ReloadState(true,true,true);
	}
	else if (!strcmp(pszEvent, "AppInit"))
		ReloadState(false,false,true);
}

HRESULT CMetallParams::GetPriority(long *plPriority)
{
	*plPriority = 0;
	return S_OK;
}

HRESULT CMetallParams::GetFirstPosEx(long *plPos)
{
	*plPos = m_Params.head();
	return S_OK;
}

HRESULT CMetallParams::GetNextParamEx(long *plPos, ParameterDescriptorEx **ppDescriptior)
{
	if (ppDescriptior) *ppDescriptior = m_Params.get(*plPos);
	*plPos = m_Params.next(*plPos);
	return S_OK;
}

HRESULT CMetallParams::DefineBorderParameters(IUnknown *punkContainer, bool bCalcMeasResult)
{
	ICalcObjectPtr sptrCO;
	if (bCalcMeasResult)
	{
		INamedDataObject2Ptr sptrCont(punkContainer);
		long lChildPos;
		sptrCont->GetFirstChildPosition(&lChildPos);
		IUnknownPtr punkObject;
		if (lChildPos) sptrCont->GetNextChild(&lChildPos, &punkObject);
		sptrCO = punkObject;
	}
	ICalcObjectContainerPtr sptrCont(punkContainer);
	for (long lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptorEx *pd = m_Params.get(lpos);
		if (pd->bZombie) continue;
		if (pd->BorderParam.lKey >= 0)
		{
			if (bCalcMeasResult)
			{
				if (sptrCO != 0)
				{
					long lpos = 0;
					sptrCO->GetValuePos(pd->lKey, &lpos);
					if (lpos == 0)
						continue;
				}
				else
					continue;
			}
			if (pd->bstrName == NULL)
				continue;
			_bstr_t	strSection("\\measurement\\parameters\\");
			strSection += pd->bstrName;
			long lEnabled = ::GetValueInt(GetAppUnknown(), strSection, "Enable", 1);
			if (pd->BorderParam.lKeyObj < 0)
			{ // It will be number of one of two base object surrounding this border object
				if (lEnabled)
					sptrCont->DefineParameter(pd->BorderParam.lKey, etCounter, this, NULL);
			}
			else
			{ // One of parameters of some base object.
				if (lEnabled)
					sptrCont->DefineParameter(pd->BorderParam.lKey, etUndefined, this, NULL);
			}
		}
	}
	return S_OK;
}





