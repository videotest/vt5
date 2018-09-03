#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "DocView5.h"
#include "misc_utils.h"
#include "AviParams.h"
#include "Object5.h"
#include "measure5.h"
#include "misc_calibr.h"
#include "misc_new.h"
#include "units_const.h"
//#include "MeasUtils.h"
#include <stdio.h>
#include <alloc.h>
#include <math.h>

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

void CAviParams::FreeParam(ParameterDescriptor *p)
{
	if (p->bstrDefFormat) ::SysFreeString(p->bstrDefFormat);
	if (p->bstrName) ::SysFreeString(p->bstrName);
	if (p->bstrUnit) ::SysFreeString(p->bstrUnit);
	if (p->bstrUserName) ::SysFreeString(p->bstrUserName);
	delete p;
}


CAviParams::CAviParams()
{
	Register(GetAppUnknown(), true, 0);
}

CAviParams::~CAviParams()
{
	Register(GetAppUnknown(), false, 0);
}

IUnknown *CAviParams::DoGetInterface(const IID &iid)
{
	if (iid == IID_IMeasParamGroup)
		return (IMeasParamGroup *)this;
	else if (iid == IID_IInitializeObjectVT)
		return (IInitializeObjectVT *)this;
	else if (iid == IID_IPriority)
		return (IPriority *)this;
	else if (iid == IID_IEventListener)
		return (IEventListener *)this;
	else
		return CObjectBase::DoGetInterface(iid);
}

GuidKey CAviParams::GetInternalClassID()
{
	return clsidAviParams;
}

void CAviParams::DefineParameter(long lKey, BSTR bstrName, BSTR bstrDefFmt)
{
	ParameterDescriptor *pdescr = new ParameterDescriptor;
	memset(pdescr, 0, sizeof(*pdescr));
	pdescr->cbSize = sizeof(ParameterDescriptor);
	pdescr->bstrName = bstrName==0?0:SysAllocString(bstrName);
	pdescr->bstrUserName = bstrName==0?0:SysAllocString(bstrName);
	pdescr->bstrDefFormat = bstrDefFmt==0?0:SysAllocString(bstrDefFmt);
	pdescr->lKey = lKey;
	pdescr->lEnabled = 0;
	pdescr->pos = (LONG_PTR)m_Params.add_tail(pdescr);
	if (bstrName != 0)
	{
		_bstr_t sMainSection("\\measurement\\parameters\\");
		_bstr_t sParamsSection = sMainSection+_bstr_t(bstrName);
		::SetValue(::GetAppUnknown(), sParamsSection, "Key", lKey);
	}
}

void CAviParams::ReadParameterSettings(ParameterDescriptor *pdescr)
{
	_bstr_t	strSection("\\measurement\\parameters\\");
	strSection += pdescr->bstrName;
	{
		if (pdescr->lKey == KEY_TIME)
		{
			_SetBSTR( pdescr->bstrUnit, GetValueString(GetAppUnknown(), szTimeUnits, szUnitName, szDefTimeUnits) );
			pdescr->fCoeffToUnits = GetValueDouble( GetAppUnknown(), szTimeUnits, szUnitCoeff, 1 );
		}
		pdescr->lEnabled = ::GetValueInt(GetAppUnknown(), strSection, "Enable", pdescr->lEnabled);
	}
	_bstr_t	strUserName = pdescr->bstrUserName;
	_bstr_t	strDefFormat = pdescr->bstrDefFormat;
	_SetBSTR( pdescr->bstrUserName, ::GetValueString(GetAppUnknown(), strSection, "UserName", strUserName) );
	_SetBSTR( pdescr->bstrDefFormat, ::GetValueString(GetAppUnknown(), strSection, "Format", strDefFormat) );
	pdescr->lTableOrder = ::GetValueInt( GetAppUnknown(), strSection, "TableOrder", pdescr->lTableOrder );
}

void CAviParams::ReloadState()
{
	for (TPOS lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptor *pdescr = m_Params.get(lpos);
		ReadParameterSettings(pdescr);
	}
}

HRESULT CAviParams::Initialize()
{
	DefineParameter(KEY_TIME, _bstr_t("Time"), _bstr_t("%0.3f"));
	ReloadState();
	return S_OK;
}

HRESULT CAviParams::CalcValues(IUnknown *punkCalcObject, IUnknown *punkSource)
{
	ICalcObjectPtr sptrCO(punkCalcObject);
	IMeasureObjectPtr sptrMO(sptrCO);
	IUnknownPtr punkImage;
	if (sptrMO != 0)
		sptrMO->GetImage(&punkImage);
	INamedDataObject2Ptr sptrNDO2Image(punkImage);
	IUnknownPtr punkBaseImage;
	if (sptrNDO2Image != 0)
		sptrNDO2Image->GetParent(&punkBaseImage);
	IAviImagePtr sptrAvi(punkBaseImage);
	if (sptrAvi != 0)
	{
		_variant_t varCurrFrame( (long)(-1) );
		sptrAvi->get_CurFrame( &varCurrFrame );
		long lCurrFrame = (long)(varCurrFrame);
		variant_t var_fps(double(0.));
		sptrAvi->get_FramePerSecond( &var_fps );
		double fFramesPerSecond = (double)var_fps;
		if (fFramesPerSecond > 0.)
		{
			double dTime = double(lCurrFrame)/fFramesPerSecond;
			sptrCO->SetValue(KEY_TIME, dTime);
		}
	}
	return S_OK;
}

HRESULT CAviParams::GetParamsCount(long *plCount)
{
	*plCount = m_Params.count();
	return S_OK;
}

HRESULT CAviParams::GetFirstPos(LONG_PTR *plPos)
{
	*plPos = (LONG_PTR)m_Params.head();
	return S_OK;
}

HRESULT CAviParams::GetNextParam(LONG_PTR *plPos, struct ParameterDescriptor **ppDescriptior )
{
	if (ppDescriptior) *ppDescriptior = m_Params.get((TPOS)*plPos);
	*plPos = (LONG_PTR)m_Params.next((TPOS)*plPos);
	return S_OK;
}

HRESULT CAviParams::GetPosByKey(long lKey, LONG_PTR *plPos)
{
	*plPos = 0;
	for (TPOS lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptor *pdescr = m_Params.get(lpos);
		if (pdescr->lKey == lKey)
		{
			*plPos = (LONG_PTR)lpos;
			break;
		}
	}
	return S_OK;
}

HRESULT CAviParams::InitializeCalculation(IUnknown *punkContainer)
{
	m_ptrContainer = punkContainer;
	for (TPOS lpos = m_Params.head(); lpos != 0; lpos = m_Params.next(lpos))
	{
		ParameterDescriptor *pdescr = m_Params.get(lpos);
		if (pdescr->lEnabled)
			m_ptrContainer->DefineParameter(pdescr->lKey, etUndefined, this, 0);
	}
	m_ptrContainer = 0;
	return S_OK;
}

HRESULT CAviParams::FinalizeCalculation()
{
	return S_OK;
}

HRESULT CAviParams::GetUnit(long lType, BSTR *pbstr, double *pfCoeffToUnits)
{
	*pbstr = GetValueString(GetAppUnknown(), szTimeUnits, szUnitName, szDefTimeUnits).copy();
	*pfCoeffToUnits = GetValueDouble( GetAppUnknown(), szTimeUnits, szUnitCoeff, 1 );
	return S_OK;
}

void CAviParams::OnNotify(const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize)
{
	if (!strcmp(pszEvent, szEventNewSettings))
		ReloadState();
	else if (!strcmp(pszEvent, "AppInit"))
		ReloadState();
}

HRESULT CAviParams::GetPriority(long *plPriority)
{
	*plPriority = 0;
	return S_OK;
}






