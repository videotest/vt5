// ChowKanekoFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(_ChowKaneko_)
#define _ChowKaneko_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CChowKanekoFilter : public CThresholdFilterBase,
					public _dyncreate_t<CChowKanekoFilter>
{
public:
	route_unknown();
public:
	double E(double X);
	int m_EdgeType;
	bool CanDeleteArgument( CFilterArgument *pa );
	long SelectEdge();
	void InitHist(IImage3 *pSource,long x,long y);
	long m_seg_offset;
	SIZE sizeEdges;
	long *pEdges;
	long *pHist;
	int m_seg_size;
	SIZE size;
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

// ChowKanekoInfo.h: interface for the CChowKanekoInfo class.
//
//////////////////////////////////////////////////////////////////////

class CChowKanekoInfo : public _ainfo_t<ID_ACTION_CHOWKANEKO, _dyncreate_t<CChowKanekoFilter>::CreateObject, 0>,
							public _dyncreate_t<CChowKanekoInfo>
{
	route_unknown();
public:
	CChowKanekoInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 