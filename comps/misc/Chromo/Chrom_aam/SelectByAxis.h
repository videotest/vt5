// SelectByAxisFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
//#include "Filter.h"
#include "image5.h"
#include "misc_utils.h"
#include "dpoint.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "action_interactive.h"

#if !defined(AFX_SelectByAxisFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_SelectByAxisFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
class CSelectByAxisFilter : public CInteractiveAction,
							public CUndoneableActionImpl,
							//public CFilter,
							public _dyncreate_t<CSelectByAxisFilter>
{
public:
	CSelectByAxisFilter();
	virtual ~CSelectByAxisFilter();
	route_unknown();
public:
	//virtual bool InvokeFilter();
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual bool IsAvaible();
	virtual IUnknown *DoGetInterface( const IID &iid );
	_gdi_t<HCURSOR> m_hCurActive;
	_gdi_t<HCURSOR> m_hCurShiftActive;
protected:
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );
	virtual bool DoLButtonDown( _point point );
	virtual bool DoRButtonDown( _point point );
	virtual bool DoRButtonUp( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoMouseMove( _point point );
	virtual bool DoChar( int nChar, bool bKeyDown );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call GetActionState( DWORD *pdwState );

	//IUndoneableAction
	com_call Undo();
	com_call Redo();

private:
	IImage3Ptr m_image;
	INamedDataObject2Ptr m_ptrList;
	_point m_FirstPoint;
	_point *m_SplinePoints;
	int m_SplinePointsNum;
	int m_action; //1 - выделение оси, 2 - установка центромеры

	IUnknownPtr	get_document();
	IImage3Ptr	CreateObjectImage(_dpoint *curve, int point_num, int &i0, int &i1);
	bool SetCentromereClick(INamedDataObject2Ptr ptrList, int x0, int y0, IUnknownPtr ptrDoc);
};



#endif // !defined(AFX_SelectByAxisFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// SelectByAxisInfo.h: interface for the CSelectByAxisInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SelectByAxisINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_SelectByAxisINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CSelectByAxisInfo : public _ainfo_t<ID_ACTION_SELECTBYAXIS, _dyncreate_t<CSelectByAxisFilter>::CreateObject, 0>,
							public _dyncreate_t<CSelectByAxisInfo>
{
	route_unknown();
public:
	CSelectByAxisInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	//virtual _bstr_t target()			{return "anydoc";}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_SelectByAxisINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
