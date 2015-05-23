// VarianceFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(AFX_VarianceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_VarianceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CVarianceFilter : public CFilterMorphoBase,
					public _dyncreate_t<CVarianceFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_VarianceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// VarianceInfo.h: interface for the CVarianceInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VarianceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_VarianceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CVarianceInfo : public _ainfo_t<ID_ACTION_VARIANCE, _dyncreate_t<CVarianceFilter>::CreateObject, 0>,
							public _dyncreate_t<CVarianceInfo>
{
	route_unknown();
public:
	CVarianceInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_VarianceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
