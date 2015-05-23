// BoundsEdgeFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(_BoundsEdge_)
#define _BoundsEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CBoundsEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CBoundsEdgeFilter>
{
public:
	route_unknown();
public:
	virtual color SelectEdge(IImage3* pSource, long* pHist, long* pAprHist);
	virtual bool IsAllArgsRequired()			{return false;};
	bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

// BoundsEdgeInfo.h: interface for the CBoundsEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CBoundsEdgeInfo : public _ainfo_t<ID_ACTION_BOUNDSEDGE, _dyncreate_t<CBoundsEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CBoundsEdgeInfo>
{
	route_unknown();
public:
	CBoundsEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 
