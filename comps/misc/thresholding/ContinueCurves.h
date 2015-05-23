#pragma once

#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
class CContinueCurves : public CFilter,
					public _dyncreate_t<CContinueCurves>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	//virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
	//bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
protected:
	int cx,cy;
	byte **src, **dst;
};

//////////////////////////////////////////////////////////////////////

class CContinueCurvesInfo : public _ainfo_t<ID_ACTION_CONTINUECURVES, _dyncreate_t<CContinueCurves>::CreateObject, 0>,
							public _dyncreate_t<CContinueCurvesInfo>
{
	route_unknown();
public:
	CContinueCurvesInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


