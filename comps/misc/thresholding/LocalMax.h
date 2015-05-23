// LocalMinFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(_LocalMin_)
#define _LocalMin_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CLocalMinFilter : public CThresholdFilterBase,
					public _dyncreate_t<CLocalMinFilter>
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

// LocalMinInfo.h: interface for the CLocalMinInfo class.
//
//////////////////////////////////////////////////////////////////////

class CLocalMinInfo : public _ainfo_t<ID_ACTION_LOCALMIN, _dyncreate_t<CLocalMinFilter>::CreateObject, 0>,
							public _dyncreate_t<CLocalMinInfo>
{
	route_unknown();
public:
	CLocalMinInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 