#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "misc_str.h"
#include "misc_templ.h"
#include "net.h"

/////////////////////////////////////////////////////////////////////////////
class CAdjustParams : public CAction, CLongOperationImpl,
		public _dyncreate_t<CAdjustParams>
{
protected:
	//virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
	CAdjustParams();
	~CAdjustParams();
public:
	IUnknown *GetContextObject(_bstr_t cName, _bstr_t cType);
	com_call DoInvoke();
}; 

//---------------------------------------------------------------------------
class CAdjustParamsInfo : public _ainfo_t<ID_ACTION_ADJUSTPARAMS, _dyncreate_t<CAdjustParams>::CreateObject, 0>,
public _dyncreate_t<CAdjustParamsInfo>
{
        route_unknown();
public:
	CAdjustParamsInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

