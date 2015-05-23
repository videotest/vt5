#pragma once

#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////

class CFindStripes : public CFilter,
public _dyncreate_t<CFindStripes>
{
public:
	route_unknown();
	CFindStripes();
	~CFindStripes();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	
		//return false;
		return CFilter::CanDeleteArgument(pa);
	};
}; 


//---------------------------------------------------------------------------
class CFindStripesInfo : 
public _ainfo_t<ID_ACTION_FINDSTRIPES, _dyncreate_t<CFindStripes>::CreateObject, 0>,
public _dyncreate_t<CFindStripesInfo>
{
	route_unknown();
public:
	CFindStripesInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

