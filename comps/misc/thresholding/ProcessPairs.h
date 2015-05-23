#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CProcessPairs : public CFilter,
					public _dyncreate_t<CProcessPairs>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
protected:
};

//////////////////////////////////////////////////////////////////////

class CProcessPairsInfo : public _ainfo_t<ID_ACTION_PROCESSPAIRS, _dyncreate_t<CProcessPairs>::CreateObject, 0>,
							public _dyncreate_t<CProcessPairsInfo>
{
	route_unknown();
public:
	CProcessPairsInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


