#pragma once

#include "action_filter.h"
#include "nameconsts.h"
#include "resource.h"
#include "defs.h"
#include "Object5.h"
//#include "classes5.h"
//#include "array.h"

class CFindBorder : public CFilter, public _dyncreate_t<CFindBorder>
{
public:
	route_unknown();
public:
	CFindBorder();
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};

class CFindBorderInfo : public _ainfo_t<ID_ACTION_FIND_BORDER, _dyncreate_t<CFindBorder>::CreateObject, 0>,
	public _dyncreate_t<CFindBorderInfo>
{
	route_unknown();
public:
	CFindBorderInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
