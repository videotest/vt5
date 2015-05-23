#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////

class CFreeze : public CAction,
public _dyncreate_t<CFreeze>
{
public:
	route_unknown();
	CFreeze();
	~CFreeze();

	com_call DoInvoke();
}; 


//---------------------------------------------------------------------------
class CFreezeInfo : 
public _ainfo_t<ID_ACTION_FREEZE, _dyncreate_t<CFreeze>::CreateObject, 0>,
public _dyncreate_t<CFreezeInfo>
{
	route_unknown();
public:
	CFreezeInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return szTargetApplication;}
	static arg s_pargs[];
};