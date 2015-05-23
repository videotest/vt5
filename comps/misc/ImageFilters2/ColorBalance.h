// ColorBalanceFilter.h: interface for the COpenByReconstructionFilter class.
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

#include "ImagePane.h"


#if !defined(AFX_ColorBalanceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_ColorBalanceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CColorBalanceFilter : public CFilterMorphoBase,
					public _dyncreate_t<CColorBalanceFilter>
{
public:
	route_unknown();
public:
	// bool ColorBalance(int nValue, int nPane, SIZE size,IImage2* pImageSrc,IImage2* pImageDst);
	bool ColorBalance(int nValue, int nPane, SIZE size,CImagePaneCache *pImageSrcPC,IImage2* pImageDst);
	bool ParseColor( int *pParams, char *str, int nPaneCount);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_ColorBalanceFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// ColorBalanceInfo.h: interface for the CColorBalanceInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ColorBalanceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_ColorBalanceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CColorBalanceInfo : public _ainfo_t<ID_ACTION_COLOR_BALANCE, _dyncreate_t<CColorBalanceFilter>::CreateObject, 0>,
							public _dyncreate_t<CColorBalanceInfo>
{
	route_unknown();
public:
	CColorBalanceInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_ColorBalanceINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
