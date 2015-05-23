// OpenByReconstructionInfo.h: interface for the COpenByReconstructionInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "OpenByReconstructionFilter.h"

#if !defined(AFX_OPENBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_OPENBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class COpenByReconstructionInfo : public _ainfo_t<ID_ACTION_OPEN_BY_RECONSTRUCTION, _dyncreate_t<COpenByReconstructionFilter>::CreateObject, 0>,
							public _dyncreate_t<COpenByReconstructionInfo>
{
	route_unknown();
public:
	COpenByReconstructionInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_OPENBYRECONSTRUCTIONINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
