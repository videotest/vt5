// AutoContrastFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(AFX_AutoContrastFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AutoContrastFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAutoContrastFilter : public CFilterMorphoBase,
					public _dyncreate_t<CAutoContrastFilter>
{
public:
	route_unknown();
public:
	//bool AutoContrast(int nValue1,int Value2, int nPane, SIZE size,IImage2* pImageSrc,IImage2* pImageDst);
	bool AutoContrast(int nValue1,int nValue2, int nPane, SIZE size,CImagePaneCache* pImageSrcPC,IImage2 *pResult);
	bool ParseColor( int *pParams, char *str, int nPaneCount);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_AutoContrastFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// AutoContrastInfo.h: interface for the CAutoContrastInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AutoContrastINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_AutoContrastINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAutoContrastInfo : public _ainfo_t<ID_ACTION_AUTO_CONTRAST, _dyncreate_t<CAutoContrastFilter>::CreateObject, 0>,
							public _dyncreate_t<CAutoContrastInfo>
{
	route_unknown();
public:
	CAutoContrastInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_AutoContrastINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
