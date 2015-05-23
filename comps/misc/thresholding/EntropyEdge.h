// EntropyEdgeFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(_EntropyEdge_)
#define _EntropyEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CEntropyEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CEntropyEdgeFilter>
{
public:
	route_unknown();
public:
	double E(double X);
	virtual color SelectEdge(IImage3* pSource, long* pHist, long* pAprHist);
	virtual bool IsAllArgsRequired()			{return false;};
	bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

// EntropyEdgeInfo.h: interface for the CEntropyEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CEntropyEdgeInfo : public _ainfo_t<ID_ACTION_ENTROPYEDGE, _dyncreate_t<CEntropyEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CEntropyEdgeInfo>
{
	route_unknown();
public:
	CEntropyEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 