#pragma once

#include "\vt5\common2\action_filter.h"
#include "resource.h"
#include "image5.h"
#include "binimageint.h"

class CWatershed :
	public CFilter,
	public _dyncreate_t<CWatershed>
{
public:
	route_unknown();
	CWatershed(void);
	~CWatershed(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
};

//--------------------------------
class CWatershedInfo : 
public _ainfo_t<ID_ACTION_WATERSHED, _dyncreate_t<CWatershed>::CreateObject, 0>,
public _dyncreate_t<CWatershedInfo>
{
	route_unknown();
public:
	CWatershedInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};
