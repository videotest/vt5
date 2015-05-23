#ifndef __impl_mouse_h__
#define __impl_mouse_h__

#include "misc_classes.h"
#include "window5.h"

class CMouseImpl
{
protected:
	CMouseImpl();
	virtual ~CMouseImpl();
public:
	void InitMouse( IUnknown	*punkTarget );
	void DeInitMouse();
	virtual LRESULT	DoMouseMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
protected:
	virtual bool DoLButtonDown( _point  point )				{return false;}
	virtual bool DoLButtonUp( _point  point )				{return false;}
	virtual bool DoRButtonDown( _point  point )				{return false;}
	virtual bool DoRButtonUp( _point  point )				{return false;}
	virtual bool DoMouseMove( _point  point )				{return false;}
	virtual bool DoChar( int nChar, bool bKeyDown )			{return false;}
	virtual bool DoSetCursor()								{return false;}
	virtual bool DoLButtonDblClick( _point  point )			{return DoLButtonDown( point );}
	virtual bool DoRButtonDblClick( _point  point )			{return DoRButtonDown( point );}
	

//tracking
	virtual bool DoStartTracking( _point point )			{return false;}
	virtual bool DoFinishTracking( _point point )			{return false;}
	virtual bool DoTrack( _point point )					{return false;}
//tracking
	virtual bool StartTracking( _point point );
	virtual bool EndTracking( bool bTerminate );
	virtual bool CheckTracking();
	virtual void EnsureVisible( _point point );
protected:
	bool	m_bTracking;
	bool	m_bTestTracking;
	bool	m_bLeftButton;
	_point	m_pointTestTracking;
	_point	m_pointLocal;
	_point	m_pointStartTracking;
	IScrollZoomSitePtr	m_ptrSite;
	long	m_nTrackSize;
};

#endif //__impl_mouse_h__

