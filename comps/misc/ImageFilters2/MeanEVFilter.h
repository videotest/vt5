// MeanEVFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(AFX_MeanEVFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_MeanEVFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CMeanEVFilter : public CFilterMorphoBase,
					public _dyncreate_t<CMeanEVFilter>
{
public:
	route_unknown();
public:
	color** pMaskRowsL;
	color** pMaskRowsR;
	bool MoveHist(int y,int x,SIZE size,int nPane,int nMaskSize,int nParam,color** pRows,long* pHist,long *pAprHist);
	bool InitHist(int y,SIZE size,int nPane,int nMaskSize,int nParam,color** pRows,long* pHist,long *pAprHist);
	bool MeanEV(long nParam, long nMaskSize, int nPane, SIZE size,
		/*IImage2* pImageSrc*/CImagePaneCache *pImageSrcPC, IImage2* pImageDst);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_MeanEVFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// MeanEVInfo.h: interface for the CMeanEVInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MeanEVINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_MeanEVINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CMeanEVInfo : public _ainfo_t<ID_ACTION_MEAN_EV, _dyncreate_t<CMeanEVFilter>::CreateObject, 0>,
							public _dyncreate_t<CMeanEVInfo>
{
	route_unknown();
public:
	CMeanEVInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_MeanEVINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
