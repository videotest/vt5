#if !defined(AFX_ACTIONLOGWINDOW_H__A9386D09_221B_11D3_A5D6_0000B493FF1B__INCLUDED_)
#define AFX_ACTIONLOGWINDOW_H__A9386D09_221B_11D3_A5D6_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionLogWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActionLogWindow window

class CActionLogWindow : public CListCtrl, 
		public CWindowImpl,
		public CDockWindowImpl,
		public CRootedObjectImpl,
		public CEventListenerImpl,
		public CNamedObjectImpl,
		public CHelpInfoImpl
{
	DECLARE_DYNCREATE(CActionLogWindow)
	GUARD_DECLARE_OLECREATE(CActionLogWindow)

	ENABLE_MULTYINTERFACE();

// Construction
public:
	CActionLogWindow();

// Attributes
public:
	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionLogWindow)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CActionLogWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CActionLogWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CActionLogWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_INTERFACE_MAP()

	bool	m_bInitialized;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONLOGWINDOW_H__A9386D09_221B_11D3_A5D6_0000B493FF1B__INCLUDED_)
