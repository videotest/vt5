// ColorSeg1.h: interface for the CColorSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSEG1_H__5625B4AC_E661_461F_B19C_B8623544D732__INCLUDED_)
#define AFX_COLORSEG1_H__5625B4AC_E661_461F_B19C_B8623544D732__INCLUDED_

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
// CColorSeg class
class CColorSeg : public CFilterMorphoBase,
public _dyncreate_t<CColorSeg>
{
	
public:
	route_unknown();
	CColorSeg();
	~CColorSeg();
	
	virtual bool InvokeFilter();
	virtual bool FindProb(int x0,int y0,int z0,int r,int *P0);
	virtual bool FindMed(int x0,int y0,int z0,int r,double *xm0,double *ym0,double *zm0,int *P0);
	virtual bool FindMax(int x0,int y0,int z0,int r,double dmin,double d1,
		int *xmax,int *ymax,int *zmax,int *P0);
	virtual bool FindNoZerPoint(int cx,int cy,int i,int j,BYTE *p_work1 ,BYTE **p_p_work1y);
	virtual BYTE FindNoZerPoint1(int cx,int cy,int ic,int jc,BYTE *p_work1 ,BYTE *p_work1y);
	virtual bool ZerroSpher(int x0,int y0,int z0,int r);
	virtual void DeleteSmolSegments(BYTE* p_Image,int cx,int cy,int Nmin,BYTE *p_VectCol);
	virtual void NconDelete(BYTE *p_DImage,BYTE *p_DImage1,int cx,int cy,int VectCount,int Ncon);
	virtual void FillNcon(BYTE *p_DImage,BYTE *p_DImage1,int cx,int cy);
public:
	WORD *p_Gist;
	WORD *p_Gistz;
	WORD *p_Gistyz;
	BYTE iRow;
	WORD iUp;
	int m_Preset;
	int m_Radius;
	int m_Nmin;
	int m_Nmin1;
	int m_Ncon;
	int m_CountLim;
	bool m_SegColors;
}; 

class CColorSegInfo : public _ainfo_t<ID_ACTION_COLOR_SEG, _dyncreate_t<CColorSeg>::CreateObject, 0>,
public _dyncreate_t<CColorSegInfo>
{
	route_unknown();
public:
	CColorSegInfo()
	{
	}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

#endif // !defined(AFX_COLORSEG1_H__5625B4AC_E661_461F_B19C_B8623544D732__INCLUDED_)
