#ifndef __mousebase_h__
#define __mousebase_h__

#include "defs.h"
#include "wndbase.h"
#include "object5.h"


class std_dll CMouseImpl
{
public:
	enum MouseEvent
	{
		meLDown = WM_LBUTTONDOWN,
		meLUp = WM_LBUTTONUP,
		meLDblClick = WM_LBUTTONDBLCLK,
		meRDown = WM_RBUTTONDOWN,
		meRUp = WM_RBUTTONUP,
		meRDblClick = WM_RBUTTONDBLCLK,
		meMove = WM_MOUSEMOVE,
		meSetCursor = WM_SETCURSOR,
		meStartTrack = 1,
		meTrack = 2, 
		meFinishTrack =3
	};

	CMouseImpl();
	~CMouseImpl();
protected:
	int	m_iTrackDeltaPos;	//if after button down mouse move to the specified delta pos,
							//initialize tracking mode
	int	m_iTrackVisibleRange;//visible "delta"

	bool m_bCanTrackByRButton;
	bool m_bTrackByRButton;
//virtuals - should be overrided
	virtual IUnknown *GetTarget() = 0;	//without AddRef!!!
public:
	bool StartTracking( CPoint pt );
	void LockMovement( bool bLock, CRect rect = NORECT );
public:
	virtual bool DoLButtonDown( CPoint pt ){return false;};
	virtual bool DoLButtonUp( CPoint pt ){return false;};
	virtual bool DoLButtonDblClick( CPoint pt ){return false;};
	virtual bool DoRButtonDown( CPoint pt ){return false;};
// Then user calls context menu during track operation and click by right button on
// context menu, then old context menu wouldn't deleted (see BCG) and new will be created.
// (error BT 3024). Disable context menu during track operation to avoid these problems.
	virtual bool DoRButtonUp( CPoint pt ){return m_bInTrackMode;};
	virtual bool DoRButtonDblClick( CPoint pt ){return false;};
	virtual bool DoMouseMove( CPoint pt ){return false;};
	virtual bool DoStartTracking( CPoint pt ){return false;};
	virtual bool DoTrack( CPoint pt ){return false;};
	virtual bool DoFinishTracking( CPoint pt ){return false;};
	virtual bool DoSetCursor( CPoint point ){return false;};
	virtual bool DoChar( UINT nChar ){return false;};
	virtual bool DoVirtKey( UINT_PTR nVirtKey ){return false;};

	virtual bool CanStartTracking( CPoint pt ){return true;};
protected:	//advanced overrideable
	virtual bool DoEnterMode(CPoint point);
	virtual bool DoLeaveMode();
	bool _MouseEvent( MouseEvent me, CPoint point );//central processing of all mouse events
public:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void ForceEnabled( bool bEnable )		{m_bForceEnabled = bEnable;};
	void _AjustCoords( CPoint &pt );
	bool _CheckCoords( CPoint &pt );
protected:
	CPoint	m_pointCurrent;	//the current point

	bool	m_bInTrackMode;
	bool	m_bTestTrackMode;
	CPoint	m_ptPossibleStartTrackAbs;
	CPoint	m_ptStartTrack;
	bool	m_bAutoScrollExecuted;
	
public:
	
	static bool Enable( bool bEnable );
protected:
	bool	m_bForceEnabled;
	static bool s_bEnabled;

	bool			m_bLockMovement;
	CRect			m_rectLock;
	HCURSOR			m_hcurStop;
};


class std_dll CDrawControllerBase : public CCmdTargetEx
{
	DECLARE_INTERFACE_MAP();
public:
	CDrawControllerBase();
	~CDrawControllerBase();
public:
	BEGIN_INTERFACE_PART_EXPORT(DrawCtrl, IMouseDrawController)
		com_call DoDraw( HDC hDC, IUnknown *punkVS, POINT pointPos, BOOL bErase );
	END_INTERFACE_PART(DrawCtrl)
public:
	virtual void Install();
	virtual void Uninstall();
	virtual void Redraw( bool bDraw );
public:
	virtual void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase ) = 0;
};



#endif //__mousebase_h__