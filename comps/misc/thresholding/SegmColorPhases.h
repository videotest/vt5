#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CSegmColorPhases : public CFilter,
					public _dyncreate_t<CSegmColorPhases>
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

class CSegmColorPhasesInfo : public _ainfo_t<ID_ACTION_SEGMCOLORPHASES, _dyncreate_t<CSegmColorPhases>::CreateObject, 0>,
							public _dyncreate_t<CSegmColorPhasesInfo>
{
	route_unknown();
public:
	CSegmColorPhasesInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
