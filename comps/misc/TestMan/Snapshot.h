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

class CSnapshot : public CAction,
public _dyncreate_t<CSnapshot>
{
public:
	route_unknown();
	CSnapshot();
	~CSnapshot();

	com_call DoInvoke();
}; 


//---------------------------------------------------------------------------
class CSnapshotInfo : 
public _ainfo_t<ID_ACTION_SNAPSHOT, _dyncreate_t<CSnapshot>::CreateObject, 0>,
public _dyncreate_t<CSnapshotInfo>
{
	route_unknown();
public:
	CSnapshotInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return szTargetApplication;}
	static arg s_pargs[];
};
