// IterationEdgeFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(_IterationEdge_)
#define _IterationEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CIterationEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CIterationEdgeFilter>
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

// IterationEdgeInfo.h: interface for the CIterationEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CIterationEdgeInfo : public _ainfo_t<ID_ACTION_ITERATIONEDGE, _dyncreate_t<CIterationEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CIterationEdgeInfo>
{
	route_unknown();
public:
	CIterationEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 