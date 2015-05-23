#pragma once
#include "stdafx.h"
#include "action_main.h"
#include "resource.h"
#include "\vt5\common2\misc_utils.h"
#include "nameconsts.h"
#include "impl_long.h"



/**********************************************************************************/

//////////////////////////////////////////////////////////////////////
//
//	class CActionFindDropAngle 
//
//////////////////////////////////////////////////////////////////////
class CActionFindDropAngle :	public CAction,
								public CLongOperationImpl,
								public _dyncreate_t<CActionFindDropAngle>
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CActionFindDropAngle();
	virtual ~CActionFindDropAngle();

	com_call	GetActionState(DWORD *pdwState);
	com_call	DoInvoke();

};

//////////////////////////////////////////////////////////////////////
//
//	class CActionFindDropAngleInfo
//
//////////////////////////////////////////////////////////////////////
class CActionFindDropAngleInfo : public _ainfo_t<IDS_ACTION_FIND_DROP_ANGLE, _dyncreate_t<CActionFindDropAngle>::CreateObject, 0>,
							public _dyncreate_t<CActionFindDropAngleInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return szTargetAnydoc;}
	static arg	s_pargs[];
};

/**********************************************************************************/













