// GradientFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(AFX_GradientFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_GradientFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CGradientFilter : public CFilterMorphoBase,
					public _dyncreate_t<CGradientFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_GradientFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// GradientInfo.h: interface for the CGradientInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GradientINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_GradientINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CGradientInfo : public _ainfo_t<ID_ACTION_GRADIENT, _dyncreate_t<CGradientFilter>::CreateObject, 0>,
							public _dyncreate_t<CGradientInfo>
{
	route_unknown();
public:
	CGradientInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_GradientINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
