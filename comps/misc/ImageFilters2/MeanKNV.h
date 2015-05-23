// MeanKNVFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MeanKNVFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_MeanKNVFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
class CMeanKNVFilter : public CFilterMorphoBase,
					public _dyncreate_t<CMeanKNVFilter>
{
public:
	route_unknown();
public:
	color** pMaskRowsL;
	color** pMaskRowsR;
	bool MoveHist(int y,int x,SIZE size,int nPane,int nMaskSize,int nParam,color** pRows,long* pHist,long *pAprHist);
	bool InitHist(int y,SIZE size,int nPane,int nMaskSize,int nParam,color** pRows,long* pHist,long *pAprHist);
	bool MeanKNV(long nParam, long nMaskSize, int nPaneCount, SIZE size,IImage2* pSource,IImage2* pResult);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};

/////////////////////////////////////////////////////////////////////////////
class CMeanKNVInfo : public _ainfo_t<ID_ACTION_MEAN_KNV, _dyncreate_t<CMeanKNVFilter>::CreateObject, 0>,
							public _dyncreate_t<CMeanKNVInfo>
{
	route_unknown();
public:
	CMeanKNVInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_MeanKNVINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
