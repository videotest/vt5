// SharpnessFilter.h: interface for the COpenByReconstructionFilter class.
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

#include "fft_for_sharpness.h"

#if !defined(AFX_SharpnessFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_SharpnessFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/////////////////////////////////////////////////////////////////////////////
class CSharpnessFilter : public CFilterMorphoBase,
					public _dyncreate_t<CSharpnessFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
};



#endif // !defined(AFX_SharpnessFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// SharpnessInfo.h: interface for the CSharpnessInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SharpnessINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_SharpnessINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CSharpnessInfo : public _ainfo_t<ID_ACTION_SHARPNESS, _dyncreate_t<CSharpnessFilter>::CreateObject, 0>,
							public _dyncreate_t<CSharpnessInfo>
{
	route_unknown();
public:
	CSharpnessInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CImg
{
public:
	// M - высота картинки, N - ширина картинки
	CImg(long int M, long int N);
	~CImg(void);
	long int H; // высота картинки
	long int W; // ширина картинки
	complex **Cmplx_image;
	// центрирование картинки (прямое и обратное) 
	void center(complex** &img, long int H, long int W);
	// вычисление ядра свертки
	real R(long int u, long int v, real yl, real yh, real c, real D0, long int H, long int W, real power);
	//real R(long int u, long int v, real yl, real yh, real c, real D0, long int H, long int W);
	// доращивание картинки
	void increase_img(complex** img, long int H, long int W, long int N, complex** &result);

private:
	complex *Cmplx_temp;
	
};
#endif // !defined(AFX_SharpnessINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
