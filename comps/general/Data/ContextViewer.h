#if !defined(AFX_CONTEXTVIEWER_H__79775EB6_4AE1_11D3_87F8_0000B493FF1B__INCLUDED_)
#define AFX_CONTEXTVIEWER_H__79775EB6_4AE1_11D3_87F8_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContextViewer.h : header file
//
#include "ContextWrp.h"
#include "PushButton.h"
/////////////////////////////////////////////////////////////////////////////
// CContextViewer window

class CContextViewer :	public CWnd, //CContextWrpType,  
					 	public CWindowImpl,
						public CDockWindowImpl,
						public CRootedObjectImpl,
						public CEventListenerImpl,
						public CNamedObjectImpl,
						public CMsgListenerImpl,
						public CHelpInfoImpl
{
	DECLARE_DYNCREATE(CContextViewer)
	GUARD_DECLARE_OLECREATE(CContextViewer)
	ENABLE_MULTYINTERFACE();

// Construction
public:
	CContextViewer();
	virtual ~CContextViewer();

// Attributes
public:
	virtual CWnd *GetWindow();
	virtual bool DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID);
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	void SetViewObj(bool bShow = false);
	void SetViewType(bool bShow = true);
	bool IsViewType()	{	return m_bViewType;		}
	bool IsViewObj()	{	return !m_bViewType;	}

	void SetNumeric();
	void RemoveNumeric();

	void Expand(bool bExpand = true);

	bool CalcLayout();
	bool CreateButtons();

	void ShowButtons(BOOL bShow);
	BOOL IsShowButtons();

	void ReturnFocus();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextViewer)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void OnFinalRelease();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

// Generated message map functions
protected:
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	void OnNotifyViewType( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	void OnNotifyViewObject( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	void RemoveTempFiles();

	// override from msg listner
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );

	
	//{{AFX_MSG(CContextViewer)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButtonType();
	afx_msg void OnClickButtonObj();
	afx_msg void OnClickButtonNumberOn();
	afx_msg void OnClickButtonNumberOff();
	afx_msg void OnClickButtonExpand();
	afx_msg void OnClickButtonCollapse();
	afx_msg void OnClickButtonAbc();
	afx_msg void OnClickButtonTime();
	afx_msg void OnDestroy();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}


	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CContextViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_INTERFACE_MAP()
	
	BEGIN_INTERFACE_PART(Viewer, IMenuInitializer2)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
		com_call UpdateActionState( BSTR bstrAction, UINT id, DWORD *pdwFlags );
	END_INTERFACE_PART(Viewer);

	BEGIN_INTERFACE_PART(ContextView, IContextView)
		com_call GetView(IUnknown ** ppunkView);
		com_call SetView(IUnknown * punkView);
		com_call SetReadOnlyMode(BOOL bMode);
		com_call GetReadOnlyMode(BOOL * pbMode);
	END_INTERFACE_PART(ContextView);

protected:
	GuidKey	m_lLastViewKey; // last used view
	GuidKey	m_lLastDocKey;	 // last used doc
	CContextWrpType		m_ViewType;
	CContextWrpObject	m_ViewObj;
	IViewPtr	m_sptrView;

	CPushButton	m_btnType;
	CPushButton	m_btnObj;
	CPushButton	m_btnAbc;
	CPushButton	m_btnTime;
	CPushButton	m_btnNumOn;
	CPushButton	m_btnNumOff;
	//CPushButton	m_btnExpand;
	//CPushButton	m_btnCollapse;

	bool m_bShowButtons;	// flag show buttons
	bool m_bViewType;		// flag shows which context view is active now 
	int		m_nBarHeight;
	CSize	m_ButtonSize;
	CStringArray	m_arrTempFiles;
	HWND m_hFocusWnd;
	// vanek!!!
	int m_nButtonStyle;

	COLORREF GetPushBtnTransparentColor(){ return RGB(192,192,192);} //Gray
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTVIEWER_H__79775EB6_4AE1_11D3_87F8_0000B493FF1B__INCLUDED_)
