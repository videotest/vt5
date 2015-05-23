#if !defined(AFX_FILTERNUCLEUS_H__FD479A14_501F_4CB4_9734_F1DA93321910__INCLUDED_)
#define AFX_FILTERFILTERNUCLEUS_H__FD479A14_501F_4CB4_9734_F1DA93321910__INCLUDED_

#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CFilterNucleus : public CFilterMorphoBase,
	public _dyncreate_t<CFilterNucleus>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	CFilterNucleus();
	virtual ~CFilterNucleus();
	int m_SmoothSize;
	int m_EvenSize;
	int m_FindAreaSize;
	int m_NucleiTolerance;
private:
	void Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy, int Mask);
	void SelectDarkArea(color **src, color **dst, int x0, int y0, SIZE size, int tolerance);
	void LoadIni();
	int m_FillColor;
};


/////////////////////////////////////////////////////////////////////////////
class CFilterNucleusInfo : public _ainfo_t<ID_ACTION_NUCLEUS, _dyncreate_t<CFilterNucleus>::CreateObject, 0>,
							public _dyncreate_t<CFilterNucleusInfo>
{
	route_unknown();
public:
	CFilterNucleusInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_FILTERFILTERNUCLEUS_H__FD479A14_501F_4CB4_9734_F1DA93321910__INCLUDED_)
