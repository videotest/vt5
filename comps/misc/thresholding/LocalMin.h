// LocalMaxFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(_LocalMax_)
#define _LocalMax_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CLocalMaxFilter : public CThresholdFilterBase,
					public _dyncreate_t<CLocalMaxFilter>
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

// LocalMaxInfo.h: interface for the CLocalMaxInfo class.
//
//////////////////////////////////////////////////////////////////////

class CLocalMaxInfo : public _ainfo_t<ID_ACTION_LOCALMAX, _dyncreate_t<CLocalMaxFilter>::CreateObject, 0>,
							public _dyncreate_t<CLocalMaxInfo>
{
	route_unknown();
public:
	CLocalMaxInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 