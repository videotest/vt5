// OpenByReconstructionFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(AFX_CLOSEBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_CLOSEBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CCloseByReconstructionFilter : public CFilterMorphoBase,
					public _dyncreate_t<CCloseByReconstructionFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_CLOSEBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// CloseByReconstructionInfo.h: interface for the CCloseByReconstructionInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOSEBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_CLOSEBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CCloseByReconstructionInfo : public _ainfo_t<ID_ACTION_CLOSE_BY_RECONSTRUCTION, _dyncreate_t<CCloseByReconstructionFilter>::CreateObject, 0>,
							public _dyncreate_t<CCloseByReconstructionInfo>
{
	route_unknown();
public:
	CCloseByReconstructionInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_CLOSEBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)

