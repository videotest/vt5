// CrossDiv1Filter.h: interface for the COpenByReconstructionFilter class.
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
#include "CrossDiv.h"

#if !defined(AFX_CrossDiv1FILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_CrossDiv1FILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
class CCrossDiv1Filter : public CCrossDivFilter,
//public CInteractiveAction,
//public CUndoneableActionImpl,
public _dyncreate_t<CCrossDiv1Filter>
{
public:
	CCrossDiv1Filter();
	virtual ~CCrossDiv1Filter();
	route_unknown();
public:
	//virtual bool InvokeFilter();
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual IUnknown *DoGetInterface( const IID &iid );
protected:
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual bool DoLButtonDown( _point point );
	//virtual bool DoRButtonDown( _point point );
	//virtual bool DoRButtonUp( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoMouseMove( _point point );
	virtual bool DoChar( int nChar, bool bKeyDown );

	com_call GetActionState( DWORD *pdwState );

	virtual void TrackPoint(const _point pt);
	virtual void SeparateTouch(const _point pt_x);
	virtual void SeparateImg(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
		_point OffSet,_point pt_x);
	virtual int ReCalcTrack( int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
		_point OffSet,_point *p4Point,int *N4C);

	int Find4Point(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,
		_point OffSet,_point pt_x,_point *p4Point);
	void ReSetPoint1(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,
		_point pt_x,_point* p_pc1,int D,_point *p4Point);
	void ReSetPoint(BYTE *in,int cx,int cy,
		int nContour,_point **pContrs,int *pContrsSize,
		_point pt_x,_point* p_pc1,_point* p_pc2,int D);
	void PointCh(_point* p_pt,int N);
	int CrossLine(_point pt1,_point pt2,_point pt3,_point pt4);
	_point FindMaxP(BYTE *in,int cx,int cy,
		_point *pCPd,int Size,int tp,int v,int dc,int D,int flag);
private:
};



#endif // !defined(AFX_CrossDiv1FILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// CrossDiv1Info.h: interface for the CCrossDiv1Info class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CrossDiv1INFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_CrossDiv1INFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CCrossDiv1Info : public _ainfo_t<ID_ACTION_CROSSDIV1, _dyncreate_t<CCrossDiv1Filter>::CreateObject, 0>,
							public _dyncreate_t<CCrossDiv1Info>
{
	route_unknown();
public:
	CCrossDiv1Info()
	{
	}
	arg	*args()	{return s_pargs;}
	//virtual _bstr_t target()			{return "anydoc";}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_CrossDiv1INFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
