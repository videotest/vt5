#if !defined(AFX_APPWINDOW_H__E790124F_3A7D_11D3_A604_0090275995FE__INCLUDED_)
#define AFX_APPWINDOW_H__E790124F_3A7D_11D3_A604_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppWindow window
#include "GfxOutBarCtrl.h"

class CAppWindow : public CGfxOutBarCtrl,
				public CDockWindowImpl,
				public CWindowImpl,
				public CEventListenerImpl,
				public CNamedObjectImpl,
				public CHelpInfoImpl
{
	DECLARE_DYNCREATE(CAppWindow)

	ENABLE_MULTYINTERFACE();

// Construction
public:
	CAppWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppWindow)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
	virtual void PostNcDestroy();

// Implementation
public:
	virtual ~CAppWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAppWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnOutbarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CAppWindow)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAppWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	//implementation abstract member from CWindowImpl
	virtual CWnd *GetWindow(){return this;}
	//use own create methodics
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	//receiving notification
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//some helper for control
	long GetSelItem();				//return the select item
	void SetSelItem( long nPos );	//setup the select item

	CImageList	m_listSmall, m_listLarge;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPWINDOW_H__E790124F_3A7D_11D3_A604_0090275995FE__INCLUDED_)
