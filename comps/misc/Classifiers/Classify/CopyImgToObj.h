#if !defined(__CopyImgToObj_H__)
#define __CopyImgToObj_H__

#include "action_main.h"
#include "action_interactive.h"
#include "action_undo.h"
#include "action_filter.h"
#include "impl_long.h"
#include "resource.h"
#include "LearningInfo.h"

class CActionCopyImageToObjects : public CFilter, public _dyncreate_t<CActionCopyImageToObjects>
{
protected:
	virtual bool CanDeleteArgument( CFilterArgument *pa );
public:
	route_unknown();
public:
	
	CActionCopyImageToObjects();
	virtual bool InvokeFilter();
};

class CActionCopyImageToObjectsInfo : public _ainfo_t<ID_ACTION_COPYIMAGES, _dyncreate_t<CActionCopyImageToObjects>::CreateObject, 0>,
	public _dyncreate_t<CActionCopyImageToObjectsInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


#endif