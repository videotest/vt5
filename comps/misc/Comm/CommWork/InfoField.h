#if !defined(__InfoField_H__)
#define __InfoField_H__


#include "action_main.h"
#include "action_interactive.h"
#include "action_undo.h"
#include "impl_long.h"
#include "resource.h"


class CActionInfoField : public CAction, public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionInfoField>
{
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionInfoField();
	com_call DoInvoke();
};

class CActionInfoFieldInfo : public _ainfo_t<ID_ACTION_INFOFIELD, _dyncreate_t<CActionInfoField>::CreateObject, 0>,
	public _dyncreate_t<CActionInfoField>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


#endif
