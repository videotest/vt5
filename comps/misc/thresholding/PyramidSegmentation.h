// PyramidFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(_Pyramid_)
#define _Pyramid_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CPyramidFilter : public CFilter,
					public _dyncreate_t<CPyramidFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
protected:
};

// PyramidInfo.h: interface for the CPyramidInfo class.
//
//////////////////////////////////////////////////////////////////////

class CPyramidInfo : public _ainfo_t<ID_ACTION_PYRAMID, _dyncreate_t<CPyramidFilter>::CreateObject, 0>,
							public _dyncreate_t<CPyramidInfo>
{
	route_unknown();
public:
	CPyramidInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif 