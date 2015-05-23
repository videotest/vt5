// PeaksEdgeFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(_PeaksEdge_)
#define _PeaksEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CPeaksEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CPeaksEdgeFilter>
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

// PeaksEdgeInfo.h: interface for the CPeaksEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CPeaksEdgeInfo : public _ainfo_t<ID_ACTION_PEAKSEDGE, _dyncreate_t<CPeaksEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CPeaksEdgeInfo>
{
	route_unknown();
public:
	CPeaksEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 
