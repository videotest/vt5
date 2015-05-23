#ifndef __wndbase_h__
#define __wndbase_h__

#include "defs.h"
#include "utils.h"
#include "cmnbase.h"
#include "window5.h"
#include "helpbase.h"
#include "nameconsts.h"

class std_dll CWndClassHelper
{
protected:
	CString	m_strClassName;
public:
	const char *GetClassName()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//if( m_strClassName.IsEmpty() )
			m_strClassName = ::AfxRegisterWndClass( CS_DBLCLKS/*|CS_HREDRAW|CS_VREDRAW*/|CS_OWNDC|CS_GLOBALCLASS, 
							::LoadCursor(NULL, IDC_ARROW), 	(HBRUSH)(COLOR_WINDOW+1) );
		return m_strClassName;
	}
static CWndClassHelper helper;
};


class std_dll CWindowImpl : public CImplBase
{
public:
	CWindowImpl();
	virtual ~CWindowImpl();

	virtual CWnd *GetWindow() = 0;
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	virtual bool DoTranslateAccelerator( MSG *pmsg ){return false;}
public:
	BEGIN_INTERFACE_PART_EXPORT(Wnd, IWindow)
		com_call CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID );
		com_call DestroyWindow();
		com_call GetHandle( HANDLE *phWnd );
		com_call HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand );
		com_call PreTranslateMsg( MSG *pmsg, BOOL *pbReturn );
		com_call GetWindowFlags( DWORD *pdwFlags );
		com_call SetWindowFlags( DWORD dwFlags );
		com_call PreTranslateAccelerator( MSG *pmsg );
	END_INTERFACE_PART(Wnd)
public:
	CString	m_strWindowTitle;
	CString	m_strClassName;
	DWORD	m_dwFlags;
};

class std_dll CWindow2Impl : public CWindowImpl
{
	CPtrList	m_ptrs;
	CPtrList	m_ptrsDrawObjects;
public:
	CWindow2Impl();
	virtual ~CWindow2Impl();

	BEGIN_INTERFACE_PART_EXPORT(Wnd2, IWindow2)
		com_call CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID );
		com_call DestroyWindow();
		com_call GetHandle( HANDLE *phWnd );
		com_call HelpHitTest( POINT pt, BSTR *pbstrHelpFileName, DWORD *pdwTopicID, DWORD *pdwHelpCommand );
		com_call PreTranslateMsg( MSG *pmsg, BOOL *pbReturn );
		com_call GetWindowFlags( DWORD *pdwFlags );
		com_call SetWindowFlags( DWORD dwFlags );
		com_call PreTranslateAccelerator( MSG *pmsg );
		com_call AttachMsgListener( IUnknown *punk );
		com_call DetachMsgListener( IUnknown *punk );
		com_call AttachDrawing( IUnknown *punk );		//unknown should support IID_IDrawObject
		com_call DetachDrawing( IUnknown *punk );
	END_INTERFACE_PART(Wnd2)
protected:
	void _AttachListener( IUnknown *punkListener );
	void _DetachListener( IUnknown *punkListener );
protected:
	bool ProcessMessage( MSG *pmsg, LRESULT* pResult );
	bool ProcessDrawing( CDC &dc, CRect rcDraw, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );	//the implememtation should call it from WM_PAINT handler
};

class std_dll CMsgListenerImpl : public CImplBase
{	
protected:
	CMsgListenerImpl();
	virtual ~CMsgListenerImpl();
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );
protected:
	BEGIN_INTERFACE_PART_EXPORT(MsgList, IMsgListener)
		com_call OnMessage( MSG *pmsg, LRESULT *plReturn );
	END_INTERFACE_PART(MsgList)
};

class std_dll CDockWindowImpl : public CImplBase
{
public:
	CDockWindowImpl();
protected:
	BEGIN_INTERFACE_PART_EXPORT(Dock, IDockableWindow)
		com_call GetSize( SIZE *psize, BOOL bVert );
		com_call GetDockSide( DWORD	*pdwSite );
		com_call GetFixed(BOOL *pbFixed);
		com_call OnShow();
		com_call OnHide();
	END_INTERFACE_PART(Dock)

protected:
	virtual bool OnChangeSize( CSize &sizeReq, bool bVert );
	virtual void OnShow(){};
	virtual void OnHide(){};

	CSize	m_size;
	bool	m_bFixed;
	DWORD	m_dwDockSide;
};

class std_dll CScrollZoomSiteImpl : public CImplBase
{
public:
	CScrollZoomSiteImpl();
public:
	BEGIN_INTERFACE_PART_EXPORT(ScrollSite, IScrollZoomSite2)
		com_call GetScrollBarCtrl( DWORD sbCode, HANDLE *phCtrl );
		com_call GetClientSize( SIZE *psize );
		com_call SetClientSize( SIZE sizeNew );
		com_call GetScrollPos( POINT *pptPos );
		com_call SetScrollPos( POINT ptPos );
		com_call GetVisibleRect( RECT *pRect );
		com_call EnsureVisible( RECT rect );
		com_call GetZoom( double *pfZoom );
		com_call SetZoom( double fZoom );
		com_call SetScrollOffsets( RECT rcOffsets );
		com_call SetAutoScrollMode( BOOL bSet );
		com_call GetAutoScrollMode( BOOL* pbSet );
		com_call LockScroll( BOOL bLock );
		com_call UpdateScroll( WORD sbCode );
		com_call GetFitDoc( BOOL* pbFitDoc );
		com_call SetFitDoc( BOOL bFitDoc );
	END_INTERFACE_PART(ScrollSite)
protected:
	virtual CScrollBar *GetScrollBarCtrl( int sbCode ) = 0;
	virtual CWnd *GetWindow() = 0;
//functions shold be called from window's message
	virtual void OnSize();
	virtual void OnScroll( UINT sbCode, UINT sbActionCode, UINT nPos );
	virtual void OnInitScrollSite();

//zooming functions
	virtual void _SetZoom( double fZoom );
	virtual double _GetZoom()
	{	return m_fZoom;	}
//scrolling
	virtual void _SetClientSize( SIZE size );
	virtual CSize _GetClientSize()
	{	return m_size;	}
	virtual void _SetScrollPos( CPoint ptScroll );
	virtual CPoint _GetScrollPos()
	{	return m_ptScroll;	}
//visibility and bounds
	virtual bool _EnsureVisible( RECT rc );
	virtual CRect _GetVisibleRect();
	virtual void _SetBoundsRect( RECT rc );
//private helpers
	virtual CRect GetScrollRect();
	virtual CSize GetClientSize();
	virtual void UpdateScroll( int sbCode );
protected:
	CSize	m_size;
	double	m_fZoom;
	CPoint	m_ptScroll;
	CRect	m_rcBounds;
	bool	m_bAutoScrollMode;
	bool	m_bFitDoc; // Zoom документа по окну

	bool	m_bLock;
	bool	m_bLockNotify;
public:
	bool	GetAutoScrollMode(){return m_bAutoScrollMode;}
	
/*	bool	m_pLockUpdate;
	class XLockUpdate;
	{
		CScrollZoomSiteImpl *m_pimpl;
		bool	m_bLastLock;
	public:
		XLockUpdate( CScrollZoomSiteImpl *pimpl )
		{
			m_pimpl = pimpl;
			m_bLastLock = m_pimpl->m_pLockUpdate;
			m_pimpl->m_pLockUpdate = true;
		}
		~XLockUpdate()
		{
			m_pimpl->m_pLockUpdate = m_bLastLock;
		}
	}*/
};

class std_dll CStatusPaneImpl : public CImplBase
{
public:
	CStatusPaneImpl();
	~CStatusPaneImpl();

protected:
	BEGIN_INTERFACE_PART_EXPORT(Pane, IStatusPane)
		com_call GetWidth( int *pWidth );
		com_call GetSide( DWORD *pdwSide );
	END_INTERFACE_PART(Pane)

protected:
	int				m_nWidth;
	PaneSide		m_paneSide;

public:
	IStatusBarPtr	m_sptrStatus;
};


class std_dll CStatusPaneBase : public CWnd,
							   public CWindowImpl,
							   public CStatusPaneImpl,
							   public CNamedObjectImpl
{
	DECLARE_INTERFACE_MAP();
	DECLARE_MESSAGE_MAP();
public:
	ENABLE_MULTYINTERFACE();
	CStatusPaneBase();

	virtual CWnd *GetWindow(){return (CWnd*)this;};

	// Generated message map functions
	//{{AFX_MSG(CStatusPaneBase)
	//}}AFX_MSG
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}

	virtual void OnFinalRelease();
};



class std_dll CPropertyPageBase : public CDialog, 
						public CWindowImpl,
						public CNamedObjectImpl,
						public CHelpInfoImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
	DECLARE_MESSAGE_MAP();
protected:
	CPropertyPageBase( UINT nID );
	~CPropertyPageBase();
//attributes
public:
	//return the "initialized" state
	bool	IsInitialized() const		{return m_bInitCalled;}
public:
	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	virtual BOOL OnInitDialog();

	//{{AFX_MSG(CPropertyPageBase)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	
	virtual void OnFinalRelease();

	virtual bool OnSetActive(){return true;};

	virtual void DoLoadSettings(){};
	virtual void DoStoreSettings(){};

protected:
	BEGIN_INTERFACE_PART_EXPORT(Page, IOptionsPage)
		com_call GetSize( SIZE *psize );
		com_call OnSetActive();
		com_call LoadSettings();
		com_call StoreSettings();
		com_call GetCaption( BSTR *pbstrCaption );
	END_INTERFACE_PART(Page);

	BEGIN_INTERFACE_PART_EXPORT(Persist, IPersist)
		com_call GetClassID(CLSID *pClassID ); 
	END_INTERFACE_PART(Persist)

	virtual void GetClassID( CLSID* pClassID) = 0;

	//message map
public:
	void UpdateAction();
protected:
	UINT	m_nID;
//the notify controller
	IUnknown	*m_punkNotifyCtrl;
//some variables
	bool	m_bInitCalled;

};


extern UINT export_data WM_NOTIFY_CHILD;

#endif //__wndbase_h__
