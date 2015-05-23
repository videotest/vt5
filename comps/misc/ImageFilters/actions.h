#ifndef __sew_actions_h__
#define __sew_actions_h__

#include "action_filter.h"
#include "resource.h"
#include "image5.h"

/////////////////////////////////////////////////////////////////////////////
class CAutomaticBCFilter : public CFilter,
					public _dyncreate_t<CAutomaticBCFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
	color GetLightnessValue( color B, color G, color R );
};


/////////////////////////////////////////////////////////////////////////////
class CAutomaticBCInfo : public _ainfo_t<ID_ACTION_AUTOBC, _dyncreate_t<CAutomaticBCFilter>::CreateObject, 0>,
							public _dyncreate_t<CAutomaticBCInfo>
{
	route_unknown();
public:
	CAutomaticBCInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


/////////////////////////////////////////////////////////////////////////////
class CSelectColorFilter : public CFilter,
					public _dyncreate_t<CSelectColorFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
protected:
};


/////////////////////////////////////////////////////////////////////////////
class CSelectColorInfo : public _ainfo_t<ID_ACTION_SELECT_COLOR, _dyncreate_t<CSelectColorFilter>::CreateObject, 0>,
							public _dyncreate_t<CSelectColorInfo>
{
	route_unknown();
public:
	CSelectColorInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


/////////////////////////////////////////////////////////////////////////////
class CApplyLookupFilter : public CFilter,
						   public _dyncreate_t<CApplyLookupFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
};


/////////////////////////////////////////////////////////////////////////////
class CApplyLookupInfo : public _ainfo_t<ID_ACTION_APPLY_LOOKUP, _dyncreate_t<CApplyLookupFilter>::CreateObject, 0>,
						 public _dyncreate_t<CApplyLookupInfo>
{
	route_unknown();
public:
	CApplyLookupInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};



#endif //__sew_actions_h__



