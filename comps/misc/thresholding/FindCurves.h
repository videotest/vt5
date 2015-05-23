#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindCurves : public CFilter,
					public _dyncreate_t<CFindCurves>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	//virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
	//bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
protected:
	int m_nMaxSplit;
	double m_fTolerance;
	int cx,cy;
	byte **src, **dst;
	void FindCurve();
};

//////////////////////////////////////////////////////////////////////

class CFindCurvesInfo : public _ainfo_t<ID_ACTION_FINDCURVES, _dyncreate_t<CFindCurves>::CreateObject, 0>,
							public _dyncreate_t<CFindCurvesInfo>
{
	route_unknown();
public:
	CFindCurvesInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


