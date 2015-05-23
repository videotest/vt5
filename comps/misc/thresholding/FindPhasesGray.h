#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindPhasesGray : public CFilter,
					public _dyncreate_t<CFindPhasesGray>
{
public:
	route_unknown();
public:
	bool CanDeleteArgument( CFilterArgument *pa );
	SIZE size;
	int		m_nWorkPane;
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

//////////////////////////////////////////////////////////////////////

class CFindPhasesGrayInfo : public _ainfo_t<ID_ACTION_FINDPHASESGRAY, _dyncreate_t<CFindPhasesGray>::CreateObject, 0>,
							public _dyncreate_t<CFindPhasesGrayInfo>
{
	route_unknown();
public:
	CFindPhasesGrayInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
