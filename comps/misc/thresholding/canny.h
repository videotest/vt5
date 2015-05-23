#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CCannyThreshold : public CFilter,
					public _dyncreate_t<CCannyThreshold>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
	bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
protected:
	int		m_nWorkPane;
	int		m_nThreshold;
	int		m_nKind;
};

//////////////////////////////////////////////////////////////////////

class CCannyThresholdInfo : public _ainfo_t<ID_ACTION_CANNYTHRESHOLD, _dyncreate_t<CCannyThreshold>::CreateObject, 0>,
							public _dyncreate_t<CCannyThresholdInfo>
{
	route_unknown();
public:
	CCannyThresholdInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


