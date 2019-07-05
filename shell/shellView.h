// shellView.h : interface of the CShellView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLVIEW_H__C48CE9C5_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_SHELLVIEW_H__C48CE9C5_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellFrame;
class CMainFrame;

enum IntelliScrollEnum
{
	isNone			= 0,	
	isWhellHorz		= 1,
	isWhellVert		= 2,
	isMiddleSet		= 3,
	isMiddleHorz	= 4,
	isMiddleVert	= 5,
};

enum MouseScrollBtn
{
	msbNone		= 0,
	msbMiddle	= 1,
	msbRight	= 2,
};

enum WheelHorzSwitch
{
	whsNone	= 0,
	whsCtrl	= 1,
	whsShift	= 2,
};

class CShellView : public CView,
					public CNumeredObjectImpl,
					public CScrollZoomSiteImpl,
					public CMapInterfaceImpl
{
	ENABLE_MULTYINTERFACE()

//Intelli mouse emulation. Start
protected:
	IntelliScrollEnum	m_ItelliMode;	
	HCURSOR				m_hCurWhellHorz;
	HCURSOR				m_hCurWhellVert;
	HCURSOR				m_hCurMiddleSet;
	HCURSOR				m_hCurMiddleHorz;
	HCURSOR				m_hCurMiddleVert;
	HCURSOR				m_hCurSave;	

//mouse wheel
	bool				m_bUseMouseWheel;
	WheelHorzSwitch		m_wheelHorzSwitch;
	int					m_nWheelStep;
	bool				m_bInvertWheelHorzDirection;
	bool				m_bInvertWheelVertDirection;

//mouse scroll
	bool				m_bUseMouseScroll;	
	MouseScrollBtn		m_mouseScrollBtn;

	bool				m_bInvertScrollHorzDirection;
	bool				m_bInvertScrollVertDirection;

//key scroll
	bool				m_bEnableKeyScroll;
	int					m_nKeyStep;

	CPoint				m_ptMousePosSave;
	bool				m_bScrollBtnDown;

	void				InitIntelliSettings();
	void				DestroyIntelliSettings();

	bool				ProcessScrollButtonUp( UINT nFlags, CPoint point );
	bool				ProcessScrollButtonDown( UINT nFlags, CPoint point );		

	bool				_OnMButtonDown(UINT nFlags, CPoint point);
	bool				_OnMButtonUp(UINT nFlags, CPoint point);
	bool				_OnRButtonDown(UINT nFlags, CPoint point);
	bool				_OnRButtonUp(UINT nFlags, CPoint point);	
	bool				_OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	bool				_OnMouseMove(UINT nFlags, CPoint point);



	void				ScrollDelta( CPoint ptDelta );

//Intelli mouse emulation. End

protected:
	CShellFrame		*m_pFrame;	//Pointer to frame window
	CAggrManager	m_aggrs;	//Array of aggregated component
	CSize			m_sizeClient;
public: // create from serialization only
	CShellView( BSTR bstrViewProgID  = 0 );
	DECLARE_DYNCREATE(CShellView)

	friend class CShellFrame;
	friend class CMainFrame;

// Attributes
public:
	CShellDoc* GetDocument();					//return the document site
	CShellFrame* GetFrame(){return m_pFrame;};

// Operations
public:
	bool Init( CShellFrame *pfrm, CShellDoc *pdoc, IUnknown *punkView );
	IUnknown *GetViewUnknown();					//return the view component

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalRelease();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);	
//	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	

// Implementation
public:
	virtual ~CShellView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShellView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART( Site, IViewSite )
		com_call GetFrameWindow( IUnknown **ppunkFrame );
		com_call InitView( IUnknown *punkView );
		com_call UpdateObjectInfo();
	END_INTERFACE_PART( Site );

	virtual CWnd *GetWindow();				//overrided virtual from CWindowImpl interface
	virtual CScrollBar *GetScrollBarCtrl( int sbCode );
	virtual void _SetClientSize( SIZE size );


	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CShellView)
	afx_msg double GetZoom();
	afx_msg void SetZoom(double newValue);
	afx_msg LPDISPATCH GetViewDispatch( );	
	afx_msg LPDISPATCH GetDataContextDispatch( );	
	afx_msg BSTR GetViewName();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()


	void AddStatusBarPanes( bool bAdd );
	void UpdateStatusBar();
	

protected:
	CString	m_strClassName;
	CString	m_strViewObjects;
	bool	m_bDestroyCalled;

	static GUID	m_guid_active_view;

	//cache interface
	virtual IUnknown* GetInterfaceHook( const void* p );
	virtual IUnknown* raw_get_interface( const GUID* piid );
};

#ifndef _DEBUG  // debug version in shellView.cpp
inline CShellDoc* CShellView::GetDocument()
   { return (CShellDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLVIEW_H__C48CE9C5_F947_11D2_A596_0000B493FF1B__INCLUDED_)
