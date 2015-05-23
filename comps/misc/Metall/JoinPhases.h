#pragma once
#include "\vt5\common2\action_filter.h"
#include "resource.h"
#include "image5.h"
#include "binimageint.h"

class CJoinPhases :
	public CFilter,
	public _dyncreate_t<CJoinPhases>
{
public:
	route_unknown();
	CJoinPhases(void);
	~CJoinPhases(void);
	virtual bool InvokeFilter();
};

//---------------------------------------------------------------------------
class CJoinPhasesInfo : 
public _ainfo_t<ID_ACTION_JOINPHASES, _dyncreate_t<CJoinPhases>::CreateObject, 0>,
public _dyncreate_t<CJoinPhasesInfo>
{
	route_unknown();
public:
	CJoinPhasesInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};