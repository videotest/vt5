#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CTransformColor1 : public CFilter,
					public _dyncreate_t<CTransformColor1>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
};

//////////////////////////////////////////////////////////////////////

class CTransformColor1Info : public _ainfo_t<ID_ACTION_TRANSFORMCOLOR1, _dyncreate_t<CTransformColor1>::CreateObject, 0>,
							public _dyncreate_t<CTransformColor1Info>
{
	route_unknown();
public:
	CTransformColor1Info()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


