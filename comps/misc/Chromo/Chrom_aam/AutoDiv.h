#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
#include "math.h"
#include "stdio.h"


#if !defined(AFX_AutoDiv_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AutoDiv_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAutoDivFilter : public CFilter,
					public _dyncreate_t<CAutoDivFilter>
{
public:
	route_unknown();
public:
	CAutoDivFilter();
	virtual ~CAutoDivFilter();
	virtual bool InvokeFilter();
	com_call DoUndo();
	com_call DoRedo();
public:
	int m_MinContourSize;
protected:
	void DivideChrom( byte *in,int cx ,int cy ,
		int ContCount , _point **pCountrs ,int *pCountrsSize,
		byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int *pCountImg);
	void ReSetTouchPoint(byte *in1,int cx,int cy,int ContCount,
		_point **pContrs,int *pContrsSize,
		_point *pP1,_point *pP2,int CountP,
		int whide);
	void FNeaLine(_point *pP1,_point *pP2,int CountP,int whide);
	void FindShot(byte *in_img,int cx,int cy,int ContCount,
		_point **pContrs,int *pContrsSize,
		int *pPixCount,_point *pPix);
	void SelectTrueDiv(byte *in,int cx,int cy,int ContCount,_point **pContrs,
								int *pContrsSize,_point *pP1,_point *pP2,
								int CountP,int whide,int pPixCount,_point *pPix,
								byte *pFlag);
	void FindXDiv(byte *in,int cx,int cy,int ContCount,_point **pContrs,
		int *pContrsSize,_point *pP1,_point *pP2,
		int CountP,int whide,
		int *pCountPX,_point *pPX11,_point *pPX12,_point *pPX21,
		_point *pPX22,int *FlagPX);
	int TouchCross(_point pPX11,_point pPX12,_point pPX13,_point pPX14,
		_point pPX21,_point pPX22,_point pPX23,_point pPX24,int wide);
	int TouchLineCross(_point pP1,_point pP2,
		_point pPX1,_point pPX2,_point pPX3,_point pPX4,int wide);
void CAutoDivFilter::DrawDivLine(byte* in,int cx,int cy,_point pt1,_point pt2);
void CAutoDivFilter::ErroImg(byte *img,int cx,int cy,int N);
int CAutoDivFilter::Separate(byte *img,int cx,int cy,
					      byte **pOutImg,int *pcx,int *pcy,_point *OffSet);
void CAutoDivFilter::Restavr(byte *img,int cx,int cy,
						  byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int CountImg);
void CAutoDivFilter::ReSet4Point(int nContour,_point **pContrs,int *pContrsSize,
							  _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22,
							  int *FlagPX,int CountPX);
void CAutoDivFilter::SeparateRect(byte **pOutImg,int *pcx,int *pcy,_point *OffSet,
							   _point pPX11,_point pPX12,_point pPX21,_point pPX22,
							   int *pCountImg);
int CAutoDivFilter::FindD(_point pt1,_point pt2,_point pt3,double *maxd,byte *in,int cx);
void CAutoDivFilter::ScanLine(byte* in,int cx,int cy,_point pt1,_point pt2,double *o);
int CAutoDivFilter::FindD1(_point pt1,_point pt2,_point pt3,double *maxd,byte *in,int cx);
int CAutoDivFilter::GetAng(_point pt11,_point pt12,_point pt21,_point pt22,double *ang);
void CAutoDivFilter::FindNoTouchDiv(_point *pP1,_point *pP2,
								_point *pP11,_point *pP12,_point *pP21,_point *pP22,
								_point *pP11s,_point *pP12s,_point *pP21s,_point *pP22s,
							    double *pD11,double *pD12,double *pD21,double *pD22,
								_point *pDP11,_point *pDP12,_point *pDP21,_point *pDP22,
							    byte *pFlag,int CountP,int whide,int cx,int cy);
void CAutoDivFilter::FindTouchDiv(_point *pP1,_point *pP2,
							  _point *pP11,_point *pP12,_point *pP21,_point *pP22,
							  _point *pP11s,_point *pP12s,_point *pP21s,_point *pP22s,
							  double *pD11,double *pD12,double *pD21,double *pD22,
							  _point *pDP11,_point *pDP12,_point *pDP21,_point *pDP22,
							  byte *pFlag,int CountP,int whide,int cx,int cy);
void CAutoDivFilter::DrawLine(byte* in,int cx,int cy,_point pt1,_point pt2);
int CAutoDivFilter::Skelet(byte *img,int cx,int cy);
void CAutoDivFilter::DelShortLine(byte *img,int cx,int cy,int ItNum,int whide);
void CAutoDivFilter::FindXX(byte *img,int cx,int cy,int ItNum,int whide,
						 _point *pPX1,_point *pPX2,int *pCountPX);
void CAutoDivFilter::FindNP(byte *img,byte *in,int cx,int cy,int ItNum,int whide,
						 _point *pPX1,_point *pPX2,
						 _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22,
						 int *FlagPX,int CountPX);
int CAutoDivFilter::FillCross(byte **pOutImg,int *pcx,int *pcy,_point *OffSet,int *pCountImg,
						   int N,int xp,int yp, int *nCrossArea);
void CAutoDivFilter::aaa1(byte *img,byte *in,int cx,int cy,int ItNum,int whide,
					   _point pPX1,
				       _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22);
void CAutoDivFilter::aaa2(byte *img,byte *in,int cx,int cy,int ItNum,int whide,
					   _point pPX1,_point pPX2,
				       _point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22);
void CAutoDivFilter::ScanCross(byte *in,int cx,int cy,int ItNum,int whide,
							_point pt11,_point pt12,_point pt21,_point pt22,
							_point pt31,_point pt32,_point pt41,_point pt42,
							_point *pPX11,_point *pPX12,_point *pPX21,_point *pPX22);
int CAutoDivFilter::ScanAng(byte *in,int cx,int cy,
						 _point pt11,_point pt12,_point pt21,_point pt22,
						 _point *pPX);
int CAutoDivFilter::ScanLine(byte *in,int cx,int cy,int x1,int y1,int x2,int y2,
						  int xc,int yc,_point *pPX);

_list_t2 <IUnknown*, FreeReleaseUnknown> m_undo_images;
	//images, которые в процессе работы были переведены из виртуальных в реальные.
	//заполняем список по ходу invoke, на undo - все копируем в parent и переводим в виртуальные
};



#endif // !defined(AFX_AutoDivFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// AutoDivInfo.h: interface for the CAutoDivInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AutoDivINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_AutoDivINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAutoDivInfo : public _ainfo_t<ID_ACTION_AUTODIV, _dyncreate_t<CAutoDivFilter>::CreateObject, 0>,
							public _dyncreate_t<CAutoDivInfo>
{
	route_unknown();
public:
	CAutoDivInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_AutoDiv_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
