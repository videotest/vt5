// ChildFrm.h : interface of the CShellFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__C48CE9C1_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_CHILDFRM_H__C48CE9C1_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellDoc;
class CShellView;

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
enum FrameType{
	ftSDI, 
	ftMDI,
	ftPDI,
	ftDDI,
	ftHIDE
};



#include "shelldockBar.h"


class CChildFrameBar : public CShellDockBar
{
	DECLARE_DYNAMIC(CChildFrameBar)
public:	
	CChildFrameBar(IUnknown *punkBar);
	CString m_strName;
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	CMiniDockFrameWnd* GetMiniDockWnd();
	void SetInterface( IUnknown *punkBar );
	void SetName( CString strName );
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);
	void StoreDockPosition();
	
	virtual CString	GetName( bool bUserName = false ){return m_strName;}

	//{{AFX_VIRTUAL(CChildFrameBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	
protected:	
	

	//{{AFX_MSG(CChildFrameBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);			
	afx_msg void OnMove(int x, int y);			
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnCloseDockBar(WPARAM wParam, LPARAM lParam);	
	//}}AFX_MSG
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()

	BOOL	m_bOldVisible;
};

class CMainFrame;
class CToolBarInfo;

//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


class CShellFrameToolBar;
									   
class CShellFrame : public CMDIChildWnd,
						CNumeredObjectImpl,
						CWindowImpl
{
	DECLARE_DYNCREATE(CShellFrame)
	ENABLE_MULTYINTERFACE();
public:
	CShellFrame();
	HMENU GetFrameMenu(){
		return m_hMenuShared;
	}
	void DoRecalcLayout(UINT nType, int cx, int cy);


//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
public:

	bool	IsAlwaysZoomed()			{return m_bAlwaysZoomed;}

	FrameType GetFrameType()
	{	return m_frameType;}

	void NotifyParent( CShellFrame* pNewChild);
	BOOL SetActive( BOOL bActive );
	CChildFrameBar* GetFrameDockBar(){
		return m_pFrameDockBar;
	}

	BOOL IsFloatingDDIFrame();


protected:

	CChildFrameBar* m_pFrameDockBar;

protected:
	FrameType	m_frameType;
	CMainFrame* GetMainFrame();
	
	BOOL m_bLockedViews; // Views can not be destroyed
	
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************



// Operations
public:
	bool ActivateLasMDIChild( bool bActivateOtherType = false );

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |	
//	virtual BOOL DestroyWindow();
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	virtual void ActivateFrame(int nCmdShow = -1);	
//	protected:
//	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void OnFinalRelease();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ActivateFrame(int nCmdShow = -1);	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL



//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |	
public:
	virtual BOOL CreatePDI(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void OnUpdateFrameMenu(BOOL bActive, CWnd* pActivateWnd,
		HMENU hMenuAlt);
	CMDIFrameWnd* GetMDIFrame();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);	
protected:
	virtual CWnd* GetMessageBar();	
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************






//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);		
//	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg BOOL OnNcActivate(BOOL bActive);
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


// Implementation
public:
	virtual ~CShellFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual CWnd *GetWindow(){return this;}

	POSITION GetFirstViewPosition();			//access to the views list
	CShellView *GetNextView( POSITION &rPos );	//return view and get next poss

	BOOL UpdateClientEdge(LPRECT lpRect)	
	{return CMDIChildWnd::UpdateClientEdge(lpRect);}

// Generated message map functions
protected:
	virtual LPUNKNOWN GetInterfaceHook(const void*);
	//{{AFX_MSG(CShellFrame)
	afx_msg void OnClose();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);		
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnNcActivate(BOOL bActive);	
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);

	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART(Frm, IFrameWindow2)
		com_call CreateView( UINT nID, BSTR bstrProgID, IUnknown **ppunkView );
		com_call DestroyView( IUnknown	*ppunkView );
		com_call GetActiveView( IUnknown **ppunkView );
		com_call GetDocument( IUnknown **ppunkDoc );
		com_call GetSplitter( IUnknown **ppunkSplitter );
		com_call RecalcLayout();
		com_call Maximize();
		com_call Restore();
		com_call GetLockedFlag(BOOL *pbLocked);
		com_call SetLockedFlag(BOOL bLocked);
	END_INTERFACE_PART(Frm)

	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CSplitterStandard)
		afx_msg LPDISPATCH GetSplitter();
		afx_msg void OnUserTitleChanged();
	//}}AFX_DISPATCH
	CString m_strUserTitle;

public:
	sptrISplitterWindow	m_sptrSplitter;	//splitter window unknown
public:

	CWnd	*GetSplitterWnd();			//return temporaly CWnd pointer to the splitter (from component)
	void SetDocument( CShellDoc *pdoc )	//change document
	{	m_pDocument = pdoc;	}
	virtual CDocument* GetActiveDocument();

	void AddView( CShellView *pview );
	void RemoveView( CShellView *pview );
protected:
	CPtrList	m_listViews;	//list views
	CShellDoc	*m_pDocument;	//document pointer
	bool		m_bInitialized;	//initiallized flag
	CSize		m_sizeClient;


protected:
	bool m_bInMDIActivateState;
public:
	bool IsInMDIActivateState(){ return m_bInMDIActivateState;}
	void SetInMDIActivateState( bool bVal ){ m_bInMDIActivateState = bVal;}


//Shell frame toolbar inside
public:
	CShellFrameToolBar* CreateToolBar( CToolBarInfo* pti );
	int GetToolBarCount();
	void DestroyAllToolBars();
	bool m_bToolbarCreateProcess;


	void _RecalcLayoyt( bool bOnlyIfToolbarExist );
	POSITION GetFirstToolBarPos();
	CShellFrameToolBar* GetNextToolBar( POSITION& pos );	

protected:
	CPtrList m_ptrToolBarList;	
	void DestroyToolBarList();

	bool m_bDestroyInProgress;
	bool	m_bCreateComplete;
public:
	virtual void GetMessageString(UINT nID, CString& rMessage) const;

	CString	m_strDocTemplateName;
	bool	m_bAlwaysZoomed;

	bool	TryActivateView();

};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__C48CE9C1_F947_11D2_A596_0000B493FF1B__INCLUDED_)
