#pragma once

#include "action_filter.h"
#include "data5.h"
#include "data_main.h"
#include "nameconsts.h"
#include "resource.h"
#include "defs.h"
#include "Object5.h"
#include "classes5.h"
#include "impl_event.h"
#include "array.h"
#include "Params.h"
#include "\vt5\awin\misc_list.h"

#define KEY_PARAM_STRINGER_MORPHOLOGY    KEY_BASE_METALL
#define KEY_PARAM_STRINGER_LENGHT        KEY_BASE_METALL+1
#define KEY_PARAM_STRINGER_SUM_LENGHT    KEY_BASE_METALL+2
#define KEY_PARAM_STRINGER_WIDTH         KEY_BASE_METALL+3
#define KEY_PARAM_STRINGER_WIDTH_ASTM45  KEY_BASE_METALL+4
#define KEY_PARAM_STRINGER_OBJECTS       KEY_BASE_METALL+5

struct CImportedParameter
{
	long lKey;
	int  nObject;
	long lKeyObj;
};

struct ParameterDescriptorEx : public ParameterDescriptor
{
	CImportedParameter BorderParam;
	bool bZombie;
};

interface IBorderParameters : public IUnknown
{
	com_call GetFirstPosEx(TPOS *plPos) = 0;
	com_call GetNextParamEx(TPOS *plPos, ParameterDescriptorEx **ppdescr) = 0;
	com_call DefineBorderParameters(IUnknown *punkContainer, bool bCalcMeasResult) = 0;
};

declare_interface(IBorderParameters, "B68A403F-A9F5-4c12-B7B2-D5D2BA161634");

// {26C2A33F-1D9B-45ae-B5D1-E10540569250}
static const GUID clsidMetallParams = 
{ 0x26c2a33f, 0x1d9b, 0x45ae, { 0xb5, 0xd1, 0xe1, 0x5, 0x40, 0x56, 0x92, 0x50 } };

#define pszMetallParamsProgID    "Metall.MetallMeasParams"

class CMetallParams : public CObjectBase, public IMeasParamGroup, public IInitializeObjectVT,
	public CEventListenerImpl, public IPriority, public IBorderParameters, public _dyncreate_t<CMetallParams>
{
	static void FreeParamEx(ParameterDescriptorEx *p);
	_list_t<ParameterDescriptorEx*,FreeParamEx> m_Params;
	ICalcObjectContainerPtr	m_ptrContainer;
	void DefineParameter(long lKey, BSTR bstrName, BSTR bstrFormat, CImportedParameter *pImp = NULL);
	void DefineVolatileParams();
	void ReadParameterSettings(ParameterDescriptorEx *pdescr);
	bool ReinitParameter(ParameterDescriptorEx *pd);
	void ReloadState(bool bNonVolatile, bool bObjNums, bool bBaseParams);
	route_unknown();
protected:
	virtual void OnNotify(const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize);
public:
	CMetallParams();
	~CMetallParams();
	virtual IUnknown *DoGetInterface(const IID &iid);
	//IPersist helper
	virtual GuidKey GetInternalClassID();
	// For IMeasParamGroup
 	com_call CalcValues(IUnknown *punkCalcObject, IUnknown *punkSource );
	com_call GetParamsCount(long *plCount);
	com_call GetFirstPos(LPOS *plPos);
	com_call GetNextParam(LPOS *plPos, struct ParameterDescriptor **ppDescriptior);
	com_call GetPosByKey(long lKey, LPOS *plPos);
	com_call InitializeCalculation( IUnknown *punkContainer );
	com_call FinalizeCalculation();
	com_call GetUnit(long lType, BSTR *pbstr, double *pfCoeffToUnits);
	// For IInitializeObjectVT
	com_call Initialize();
	// IPriority
	com_call GetPriority(long *plPriority);
	// IBorderParameters
	com_call GetFirstPosEx(TPOS *plPos);
	com_call GetNextParamEx(TPOS *plPos, ParameterDescriptorEx **ppdescr);
	com_call DefineBorderParameters(IUnknown *punkContainer, bool bCalcMeasResult);
};


