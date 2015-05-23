#ifndef __Compare_h__
#define __Compare_h__

#include "action_main.h"
#include "action_interactive.h"
#include "action_undo.h"
#include "impl_long.h"
#include "resource.h"



class CActionCompare :	public CAction,	public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionCompare>
{
public:
	route_unknown();
public:
	
	CActionCompare();
	com_call DoInvoke();
};

class CActionCompareInfo : public _ainfo_t<ID_ACTION_COMPARE, _dyncreate_t<CActionCompare>::CreateObject, 0>,
					   public _dyncreate_t<CActionCompare>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "app";}
	static arg	s_pargs[];
};


#endif //__Compare_h__