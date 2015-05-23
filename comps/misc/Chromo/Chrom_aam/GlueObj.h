// GlueObjFilter.h: interface for the COpenByReconstructionFilter class.
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

#if !defined(AFX_GlueObjFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_GlueObjFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
class CGlueObjFilter : public CInteractiveAction,
							public CUndoneableActionImpl,
							public _dyncreate_t<CGlueObjFilter>
{
public:
	CGlueObjFilter();
	virtual ~CGlueObjFilter();
	route_unknown();
public:
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	virtual bool IsAvaible();
	virtual IUnknown *DoGetInterface( const IID &iid );
	_gdi_t<HCURSOR> m_hCurActive;
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
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );

	//IImage3Ptr m_Image;
	INamedDataObject2Ptr m_List;
	IUnknownPtr m_ObjectUnk; //current object
	IUnknownPtr m_Object1Unk; //1st object for glue
	IUnknownPtr m_Object2Unk; //2nd object for glue
	virtual void GlueTouch(_point pt);
	virtual void GlueTwoObj(_point pt, IUnknownPtr obj1, IUnknownPtr obj2, _point ptConn1,  _point ptConn2);
	IUnknownPtr	get_document();
private:
};



#endif // !defined(AFX_GlueObjFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// GlueObjInfo.h: interface for the CGlueObjInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GlueObjINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_GlueObjINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CGlueObjInfo : public _ainfo_t<ID_ACTION_GLUEOBJ, _dyncreate_t<CGlueObjFilter>::CreateObject, 0>,
							public _dyncreate_t<CGlueObjInfo>
{
	route_unknown();
public:
	CGlueObjInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	//virtual _bstr_t target()			{return "anydoc";}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

#endif // !defined(AFX_GlueObjINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
