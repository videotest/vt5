#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CThresholdByEdges : public CFilter,
					public _dyncreate_t<CThresholdByEdges>
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
};

//////////////////////////////////////////////////////////////////////

class CThresholdByEdgesInfo : public _ainfo_t<ID_ACTION_THRESHOLDBYEDGES, _dyncreate_t<CThresholdByEdges>::CreateObject, 0>,
							public _dyncreate_t<CThresholdByEdgesInfo>
{
	route_unknown();
public:
	CThresholdByEdgesInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


