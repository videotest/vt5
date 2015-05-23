#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CLinesThreshold : public CFilter,
					public _dyncreate_t<CLinesThreshold>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
protected:
	int		m_nWorkPane;
	int		m_nThreshold;
	int		m_nMask;
};

//////////////////////////////////////////////////////////////////////

class CLinesThresholdInfo : public _ainfo_t<ID_ACTION_LINESTHRESHOLD, _dyncreate_t<CLinesThreshold>::CreateObject, 0>,
							public _dyncreate_t<CLinesThresholdInfo>
{
	route_unknown();
public:
	CLinesThresholdInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


