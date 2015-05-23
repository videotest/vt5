// FuzzySetsEdgeFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(_FuzzySetsEdge_)
#define _FuzzySetsEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CFuzzySetsEdgeFilter : public CThresholdFilterBase,
					public _dyncreate_t<CFuzzySetsEdgeFilter>
{
public:
	route_unknown();
public:
	long* m_pAprHist;
	SIZE m_size;
	double Hf(double i);
	long m_C;
	double* m_M1;
	double* m_M0;
	double m(long g,long t);
	double F(color t);

	virtual color SelectEdge(IImage3* pSource, long* pHist, long* pAprHist);
	virtual bool IsAllArgsRequired()			{ return false; }
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images

	bool CanDeleteArgument( CFilterArgument *pa );
protected:
};

// FuzzySetsEdgeInfo.h: interface for the CFuzzySetsEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CFuzzySetsEdgeInfo : public _ainfo_t<ID_ACTION_FUZZYSETSEDGE, _dyncreate_t<CFuzzySetsEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CFuzzySetsEdgeInfo>
{
	route_unknown();
public:
	CFuzzySetsEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 