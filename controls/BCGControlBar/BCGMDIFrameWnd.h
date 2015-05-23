//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#if !defined(AFX_BCGMDIFRAMEWND_H__08B9EC04_DCE3_11D1_A64F_00A0C93A70EC__INCLUDED_)
#define AFX_BCGMDIFRAMEWND_H__08B9EC04_DCE3_11D1_A64F_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BCGMDIFrameWnd.h : header file
//

#include "bcgcontrolbar.h"
#include "BCGFrameImpl.h"
#include "BCGMainClientAreaWnd.h"

class CBCGPopupMenu;
class CBCGToolBar;
class CBCGToolbarMenuButton;
class CBCGToolbarButton;

/////////////////////////////////////////////////////////////////////////////
// CBCGMDIFrameWnd frame

class BCGCONTROLBARDLLEXPORT CBCGMDIFrameWnd : public CMDIFrameWnd
{
	friend class CBCGMainClientAreaWnd;
	friend class CBCGMenuBar;
	friend class CBCGToolbarsPage;
	friend class CBCGWorkspace;
	friend class CBCGPopupMenu;
	friend class CBCGToolbarCustomize;

	DECLARE_DYNCREATE(CBCGMDIFrameWnd)

protected:
	CBCGMDIFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:
	BOOL IsMenuBarAvailable () const
	{
		return m_Impl.GetMenuBar () != NULL;
	}

	virtual BOOL CanShowMenuBar()
	{
		return TRUE;
	}


	const CBCGMenuBar* GetMenuBar () const
	{
		return m_Impl.GetMenuBar ();
	}

	UINT GetDefaultResId () const
	{
		return m_Impl.m_nIDDefaultResource;
	}

	CBCGPopupMenu*	GetActivePopup () const
	{
		return m_Impl.m_pActivePopupMenu;
	}
//!!!andy modification - 02.06.99
	const CBCGToolBar	*CreateNewToolbar( const char *sz )
	{
		return m_Impl.CreateNewToolBar( sz );
	}

	bool DeleteToolBar( CBCGToolBar	*ptool )
	{
		return m_Impl.DeleteToolBar( ptool ) == TRUE;
	}
//end modification

//A.M. modification (18.04.2006, BT5157)
	virtual bool GetEnableChangeMenu(LPCTSTR lpstrMenuName) {return true;}
//end modification
//A.M. modification (31.05.2006, BT5262)
	virtual bool IsVerticalDockEnabled() {return true;}
//end modification


protected:
	BOOL					m_bIsTlbCustMode;
	CBCGMainClientAreaWnd	m_wndClientArea;
	HMENU					m_hmenuWindow;
	CBCGFrameImpl			m_Impl;
	BOOL					m_bContextHelp;
	BOOL					m_bDoSubclass;
	UINT					m_uiWindowsDlgMenuId;
	CString					m_strWindowsDlgMenuText;
	BOOL					m_bShowWindowsDlgAlways;

// Operations
public:
	void EnableDocking (DWORD dwDockStyle);
	void DockControlBarLeftOf (CControlBar* pBar, CControlBar* pLeftOf);

	void SetupToolbarMenu (	CMenu& menu,
							const UINT uiViewUserToolbarCmdFirst,
							const UINT uiViewUserToolbarCmdLast)
	{
		m_Impl.SetupToolbarMenu (menu, uiViewUserToolbarCmdFirst, uiViewUserToolbarCmdLast);
	}

	void EnableWindowsDialog (UINT uiMenuId, 
		LPCTSTR lpszMenuText,
		BOOL bShowAllways = FALSE);
		// Enable built-in CBCGWindowsManagerDlg dialog. When bShowAllways
		// is TRUE, show windows dialog everytime; otherwise only instead of
		// the statndard "Windows..." dialog (appear if more than 9 windows 
		// are open)
	void ShowWindowsDialog ();

protected:
	virtual BOOL OnSetMenu (HMENU hmenu);

// Implementation:
	void InitUserToobars (LPCTSTR lpszRegEntry, UINT uiUserToolbarFirst, UINT uiUserToolbarLast)
	{
		m_Impl.InitUserToobars (lpszRegEntry, uiUserToolbarFirst, uiUserToolbarLast);
	}

	void LoadUserToolbars ()
	{
		m_Impl.LoadUserToolbars ();
	}

	void SaveUserToolbars ()
	{
		m_Impl.SaveUserToolbars ();
	}

	BOOL IsDockStateValid (const CDockState& state)
	{
		return m_Impl.IsDockStateValid (state);
	}


	CBCGToolBar* GetUserBarByIndex (int iIndex) const
	{
		return m_Impl.GetUserBarByIndex (iIndex);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGMDIFrameWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual HMENU GetWindowMenuPopup (HMENU hMenuBar);
	BOOL ShowPopupMenu (CBCGPopupMenu* pMenuPopup);

public:
	virtual BOOL OnShowPopupMenu (CBCGPopupMenu* /*pMenuPopup*/)
	{
		return TRUE;
	}

	virtual void OnClosePopupMenu (CBCGPopupMenu* pMenuPopup);
	virtual BOOL OnDrawMenuImage (	CDC* /*pDC*/, 
									const CBCGToolbarMenuButton* /*pMenuButton*/, 
									const CRect& /*rectImage*/)
	{
		return FALSE;
	}

	virtual BOOL OnMenuButtonToolHitTest (CBCGToolbarButton* /*pButton*/, TOOLINFO* /*pTI*/)
	{
		return FALSE;
	}

	virtual BOOL GetToolbarButtonToolTipText (CBCGToolbarButton* /*pButton*/, CString& /*strTTText*/)
	{
		return FALSE;
	}

	virtual BOOL OnEraseMDIClientBackground (CDC* /*pDC*/)
	{
		return FALSE;
	}

// Implementation
protected:
	virtual ~CBCGMDIFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CBCGMDIFrameWnd)
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnToolbarCreateNew(WPARAM,LPARAM);
	afx_msg LRESULT OnToolbarDelete(WPARAM,LPARAM);
	afx_msg void OnContextHelp ();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGMDIFRAMEWND_H__08B9EC04_DCE3_11D1_A64F_00A0C93A70EC__INCLUDED_)
