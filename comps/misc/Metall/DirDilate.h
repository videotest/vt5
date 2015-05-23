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

class CDirDilate : public CFilter,
public _dyncreate_t<CDirDilate>
{
public:
	route_unknown();
	CDirDilate();
	~CDirDilate();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	
		if(pa->m_bInArgument)
		{
			return false;
		}
		
		return CFilter::CanDeleteArgument(pa);
	};
	
protected:
	void FindThresholds(color **src, int cx, int cy, double th1=0.5, double th2=0.25, int nMethod=1);
	void Segmentation(color **src, byte **bin, int cx, int cy);
	int m_back;
	int m_fore1;
	int m_fore2;
	int m_threshold1;
	int m_threshold2;
}; 


//---------------------------------------------------------------------------
class CDirDilateInfo : 
public _ainfo_t<ID_ACTION_DIRDILATE, _dyncreate_t<CDirDilate>::CreateObject, 0>,
public _dyncreate_t<CDirDilateInfo>
{
	route_unknown();
public:
	CDirDilateInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};
