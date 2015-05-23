// BinaryMinimumEdgeFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(_BinaryMinimumEdge_)
#define _BinaryMinimumEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CBinaryMinimumEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CBinaryMinimumEdgeFilter>
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

// BinaryMinimumEdgeInfo.h: interface for the CBinaryMinimumEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CBinaryMinimumEdgeInfo : public _ainfo_t<ID_ACTION_MINBINARYEDGE, _dyncreate_t<CBinaryMinimumEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CBinaryMinimumEdgeInfo>
{
	route_unknown();
public:
	CBinaryMinimumEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 