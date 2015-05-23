#pragma once

#include "action_filter.h"
#include "image5.h"
#include "docview5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "resource.h"
#include "ObjectPool.h"

class CTakeObjectsRects : public CFilter,
        public _dyncreate_t<CTakeObjectsRects>
{
public:
	route_unknown();
public:
	CTakeObjectsRects(void);
	~CTakeObjectsRects(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	if(pa->m_bstrArgName == _bstr_t("Image")) return false;
		return __super::CanDeleteArgument(pa);
	};
	CObjectPool m_ObjectPool;
};

class CTakeObjectsRectsInfo : public _ainfo_t<ID_ACTION_TAKEOBJECTSRECTS, _dyncreate_t<CTakeObjectsRects>::CreateObject, 0>,
public _dyncreate_t<CTakeObjectsRectsInfo>
{
	route_unknown();
public:
	CTakeObjectsRectsInfo() { }
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};
