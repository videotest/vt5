#pragma once
#include "\vt5\common2\action_filter.h"
#include "resource.h"
#include "image5.h"
#include "binimageint.h"
#include "action_main.h"
#include "measure5.h"

class CBinaryFromObjects :
	public CFilter,
	public _dyncreate_t<CBinaryFromObjects>
{
public:
	route_unknown();
	CBinaryFromObjects(void);
	~CBinaryFromObjects(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
};

//---------------------------------------------------------------------------
class CBinaryFromObjectsInfo : 
public _ainfo_t<ID_ACTION_BINARYFROMOBJECTS, _dyncreate_t<CBinaryFromObjects>::CreateObject, 0>,
public _dyncreate_t<CBinaryFromObjectsInfo>
{
	route_unknown();
public:
	CBinaryFromObjectsInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()  {return "anydoc";}
};