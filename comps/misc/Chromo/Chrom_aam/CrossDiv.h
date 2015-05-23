// CrossDivFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
//#include "Filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "action_interactive.h"

#if !defined(AFX_CrossDivFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_CrossDivFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// вспомогательные функции для сортировки точек
int SignedContourArea(_point *pCont, int size);
void Sort4Points(_point *pt4);


/////////////////////////////////////////////////////////////////////////////
class CCrossDivFilter : public CInteractiveAction,
							public CUndoneableActionImpl,
							public _dyncreate_t<CCrossDivFilter>
{
public:
	CCrossDivFilter();
	virtual ~CCrossDivFilter();
	route_unknown();
public:
	//virtual bool InvokeFilter();
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual bool IsAvaible();
	virtual IUnknown *DoGetInterface( const IID &iid );
	_gdi_t<HCURSOR> m_hCurActive;
protected:
	virtual bool DoLButtonDown( _point point );
	//virtual bool DoRButtonDown( _point point );
	//virtual bool DoRButtonUp( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoMouseMove( _point point );
	virtual bool DoChar( int nChar, bool bKeyDown );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call GetActionState( DWORD *pdwState );

	//IImage3Ptr m_Image;
	INamedDataObject2Ptr m_List;
	IUnknownPtr m_ObjectUnk; //current object
	_point m_SqrPoints[5];
	int m_SqrPointsNum;
	virtual void TrackPoint(const _point pt);
	virtual void SeparateTouch();
	virtual void SeparateImg(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
		_point OffSet,_point *p4Point);
	virtual int ReCalcTrack( int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
		_point OffSet,_point *p4Point,int *N4C);
	virtual void CreateTwoObjContours(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
		_point OffSet,_point *p4Point,int *N4C);
	virtual void AddLineToContour(_point *pCont,int *Size,_point pt1,_point pt2);
	_point GetNext4Point(int n,_point *p4Point,int tc,int v);
	virtual void FillContour(byte *img,int cx,int cy,_point *pCont,int Size,
		int xos,int yos,int inside);
	virtual void CCrossDivFilter::GetNextPosition(_point pt,int nContour,_point **pContrs,
		int *pContrsSize,int *ptc,int *ptp,int *pv,int *N4C);
	virtual void AddObjects (int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,_point OffSet);
	virtual void AddObjects1(int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,_point OffSet);
	IUnknownPtr	get_document();
private:
};



#endif // !defined(AFX_CrossDivFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// CrossDivInfo.h: interface for the CCrossDivInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CrossDivINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_CrossDivINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CCrossDivInfo : public _ainfo_t<ID_ACTION_CROSSDIV, _dyncreate_t<CCrossDivFilter>::CreateObject, 0>,
							public _dyncreate_t<CCrossDivInfo>
{
	route_unknown();
public:
	CCrossDivInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	//virtual _bstr_t target()			{return "anydoc";}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_CrossDivINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
