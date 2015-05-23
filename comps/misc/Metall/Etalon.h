#pragma once

#include "action_main.h"
#include "resource.h"

class CApproxObjSize : public CAction, public _dyncreate_t<CApproxObjSize>
{
public:
	route_unknown();
public:

	CApproxObjSize();
	com_call DoInvoke();
};

class CApproxObjSizeInfo : public _ainfo_t<ID_ACTION_APPROX_OBJ_SIZE, _dyncreate_t<CApproxObjSize>::CreateObject, 0>,
	public _dyncreate_t<CApproxObjSizeInfo>
{
	route_unknown();
public:
	CApproxObjSizeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

