#ifndef __BubbleSelect_h__
#define __BubbleSelect_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterNegative class

class CBubbleSelect : public CFilter,
	public _dyncreate_t<CBubbleSelect>
{

#define CSize SIZE

public:
	route_unknown();
	CBubbleSelect();
	~CBubbleSelect();

	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
}; 

//---------------------------------------------------------------------------
class CBubbleSelectInfo : public _ainfo_t<ID_ACTION_BUBBLESELECT, _dyncreate_t<CBubbleSelect>::CreateObject, 0>,
public _dyncreate_t<CBubbleSelectInfo>
{
	route_unknown();
public:
	CBubbleSelectInfo()
	{
	}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__BubbleSelect_h__
