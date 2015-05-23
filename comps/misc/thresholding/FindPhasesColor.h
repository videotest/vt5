#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindPhasesColor : public CFilter,
					public _dyncreate_t<CFindPhasesColor>
{
public:
	route_unknown();
public:
	bool CanDeleteArgument( CFilterArgument *pa );
	SIZE size;
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

//////////////////////////////////////////////////////////////////////

class CFindPhasesColorInfo : public _ainfo_t<ID_ACTION_FINDPHASESCOLOR, _dyncreate_t<CFindPhasesColor>::CreateObject, 0>,
							public _dyncreate_t<CFindPhasesColorInfo>
{
	route_unknown();
public:
	CFindPhasesColorInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
// Идея: делим на квадратики, в каждом по быстрому ищем 1-2 фазы и их количество, дальше работаем с получ. данными
class CFastFindPhasesColor : public CFilter,
					public _dyncreate_t<CFastFindPhasesColor>
{
public:
	route_unknown();
public:
	bool CanDeleteArgument( CFilterArgument *pa );
	SIZE size;
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

//////////////////////////////////////////////////////////////////////

class CFastFindPhasesColorInfo : public _ainfo_t<ID_ACTION_FASTFINDPHASESCOLOR, _dyncreate_t<CFastFindPhasesColor>::CreateObject, 0>,
							public _dyncreate_t<CFastFindPhasesColorInfo>
{
	route_unknown();
public:
	CFastFindPhasesColorInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
