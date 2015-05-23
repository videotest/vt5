#pragma once
#include "\vt5\common2\action_filter.h"
#include "resource.h"
#include "image5.h"
#include "measure5.h"

class CMergeObjects :
	public CFilter,
	public _dyncreate_t<CMergeObjects>
{
public:
	route_unknown();
	CMergeObjects(void);
	~CMergeObjects(void);
	virtual bool InvokeFilter();
};

//---------------------------------------------------------------------------
class CMergeObjectsInfo : 
public _ainfo_t<ID_ACTION_MERGEOBJECTS, _dyncreate_t<CMergeObjects>::CreateObject, 0>,
public _dyncreate_t<CMergeObjectsInfo>
{
	route_unknown();
public:
	CMergeObjectsInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};
