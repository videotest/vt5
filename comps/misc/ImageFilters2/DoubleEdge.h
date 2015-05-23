// DoubleEdgeFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "binimageint.h"
#include "FilterMorphoBase.h"

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "resource.h"


#if !defined(_DoubleEdge_)
#define _DoubleEdge_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CDoubleEdgeFilter : public CFilterMorphoBase,
					public _dyncreate_t<CDoubleEdgeFilter>
{
public:
	route_unknown();
public:
	SIZE size;
	bool InvokeFilter();
	//color SelectEdge(IImage3* pSource, long* pHist, long* pAprHist);
	virtual bool IsAllArgsRequired()			{return false;};
	bool CanDeleteArgument( CFilterArgument *pa );
protected:
};

// DoubleEdgeInfo.h: interface for the CDoubleEdgeInfo class.
//
//////////////////////////////////////////////////////////////////////

class CDoubleEdgeInfo : public _ainfo_t<ID_ACTION_DOUBLEEDGE, _dyncreate_t<CDoubleEdgeFilter>::CreateObject, 0>,
							public _dyncreate_t<CDoubleEdgeInfo>
{
	route_unknown();
public:
	CDoubleEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 