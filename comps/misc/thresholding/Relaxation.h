// RelaxationEdgeFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "morph1.h"
#include "ThresholdFilterBase.h"

#include "action_filter.h"
#include "image5.h"
#include "action5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "resource.h"


#if !defined(_RelaxationEdge_)
#define _RelaxationEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CRelaxationEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CRelaxationEdgeFilter>
{
public:
	route_unknown();
public:
	SIZE size;
	double *pQ;
	double *pP;
	double Q(long x,long y,double c);
	double C(double c1,double c2);
	bool InvokeFilter();
	color SelectEdge(IImage3* pSource, long* pHist, long* pAprHist);
	virtual bool IsAllArgsRequired()			{return false;};
	bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

// RelaxationEdgeInfo.h: interface for the CRelaxationEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CRelaxationEdgeInfo : public _ainfo_t<ID_ACTION_RELAXATION, _dyncreate_t<CRelaxationEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CRelaxationEdgeInfo>
{
	route_unknown();
public:
	CRelaxationEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 