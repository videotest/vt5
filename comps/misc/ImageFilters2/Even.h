// EvenFilter.h: interface for the COpenByReconstructionFilter class.
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


#if !defined(AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CEvenFilter : public CFilterMorphoBase,
					public _dyncreate_t<CEvenFilter>
{
public:
	route_unknown();
public:
	void AutoContrast(color **Rows, int cx, int cy);
	virtual bool InvokeFilter();
	void EvenPlane(color **SrcRows, color **DstRows, int cx, int cy);
	int m_SqrSize; // размер квадрата, >=4
	int m_EvenMethod; // коррекци€ одного цвета/двух/трех
	int m_BackgroundType;  // 0 - авто, 1 - min, 2 - max, 3 - медиана
	BOOL	m_EvenBrightness; //выравнивать только €ркость (иначе - покомпонентно)
	double m_MaxContrast;  // 1 .. 100 - максимальный допустимый контраст
	BOOL	m_AutoContrast; //auto contrast whole image after even
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// EvenInfo.h: interface for the CEvenInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CEvenInfo : public _ainfo_t<ID_ACTION_EVEN, _dyncreate_t<CEvenFilter>::CreateObject, 0>,
							public _dyncreate_t<CEvenInfo>
{
	route_unknown();
public:
	CEvenInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
