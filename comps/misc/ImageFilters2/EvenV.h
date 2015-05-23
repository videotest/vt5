// EvenV.h: interface for the COpenByReconstructionFilter class.
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

//#if !defined(AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
//#define AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CEvenVFilter : public CFilterMorphoBase,
					public _dyncreate_t<CEvenVFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	void EvenPlane(color **SrcRows, color **DstRows, int cx, int cy);
	int m_SqrNumber; // количество квадратов в одной стороне изображения
	int m_EvenMethod; // вычитание или деление фона
	int m_BackgroundType;  // 0 - авто, 1 - min, 2 - max, 3 - медиана
	BOOL	m_EvenBrightness; //выравнивать только яркость (иначе - покомпонентно)
	int m_Contrast; // величина контраста от -100 до 100
	CEvenVFilter()
	{
		m_SqrNumber = 10; m_EvenMethod = 0; m_BackgroundType = 0; 
		m_EvenBrightness = 0; m_Contrast = 0;
	}
protected:
};



//#endif // !defined(AFX_EvenFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// EvenInfo.h: interface for the CEvenInfo class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
//#define AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_
//
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CEvenVInfo : public _ainfo_t<ID_ACTION_EVENV, _dyncreate_t<CEvenVFilter>::CreateObject, 0>,
							public _dyncreate_t<CEvenVInfo>
{
	route_unknown();
public:
	CEvenVInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

//#endif // !defined(AFX_EvenINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)