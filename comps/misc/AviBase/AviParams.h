#pragma once

//#include "action_filter.h"
#include "data5.h"
#include "data_main.h"
#include "nameconsts.h"
#include "resource.h"
#include "defs.h"
#include "Object5.h"
//#include "classes5.h"
#include "impl_event.h"
//#include "array.h"
#include "Params.h"
#include "Measure5.h"
#include "avi_int.h"
#include "\vt5\awin\misc_list.h"


// {099D4E44-917E-4a62-9635-F20942404A21}
static const GUID clsidAviParams = 
{ 0x99d4e44, 0x917e, 0x4a62, { 0x96, 0x35, 0xf2, 0x9, 0x42, 0x40, 0x4a, 0x21 } };

#define pszAviParamsProgID "AviBase.AviMeasParams"

class CAviParams : public CObjectBase, public IMeasParamGroup, public IInitializeObjectVT,
	public CEventListenerImpl, public IPriority, public _dyncreate_t<CAviParams>
{
	static void FreeParam(ParameterDescriptor *p);
	_list_t<ParameterDescriptor*,FreeParam> m_Params;
	ICalcObjectContainerPtr	m_ptrContainer;
	void ReadParameterSettings(ParameterDescriptor *pdescr);
	void DefineParameter(long lKey, BSTR bstrName, BSTR bstrFormat);
	void ReloadState();
	route_unknown();
protected:
	virtual void OnNotify(const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize);
public:
	CAviParams();
	~CAviParams();
	virtual IUnknown *DoGetInterface(const IID &iid);
	//IPersist helper
	virtual GuidKey GetInternalClassID();
	// For IMeasParamGroup
 	com_call CalcValues(IUnknown *punkCalcObject, IUnknown *punkSource );
	com_call GetParamsCount(long *plCount);
	com_call GetFirstPos(LONG_PTR *plPos);
	com_call GetNextParam(LONG_PTR *plPos, struct ParameterDescriptor **ppDescriptior );
	com_call GetPosByKey(long lKey, LONG_PTR *plPos);
	com_call InitializeCalculation( IUnknown *punkContainer );
	com_call FinalizeCalculation();
	com_call GetUnit(long lType, BSTR *pbstr, double *pfCoeffToUnits);
	// For IInitializeObjectVT
	com_call Initialize();
	// IPriority
	com_call GetPriority(long *plPriority);
};

