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

#if !defined(AFX_BCGPOPUPMENU_H__A6E1AD75_D511_11D1_A64D_00A0C93A70EC__INCLUDED_)
#define AFX_BCGPOPUPMENU_H__A6E1AD75_D511_11D1_A64D_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BCGPopupMenu.h : header file
//

#include "bcgcontrolbar.h"
#include "BCGPopupMenuBar.h"

class CBCGToolbarMenuButton;
class CBCGMenuBar;
class CBCGMenuPage;

/////////////////////////////////////////////////////////////////////////////
// CBCGPopupMenu frame

class BCGCONTROLBARDLLEXPORT CBCGPopupMenu : public CMiniFrameWnd
{
	friend class CBCGToolbarMenuButton;
	friend class CBCGMenuPage;
	friend class CBCGFrameWnd;
	friend class CBCGMDIFrameWnd;
	friend class CBCGOleIPFrameWnd;

	DECLARE_SERIAL(CBCGPopupMenu)

public:
	CBCGPopupMenu();
	virtual ~CBCGPopupMenu();

protected:
	CBCGPopupMenu(CBCGMenuPage* pCustPage, LPCTSTR lpszTitle);	// For customization only

// Attributes
public:
	enum ANIMATION_TYPE
	{
		NO_ANIMATION,
		UNFOLD,
		SLIDE,
		FADE
	};

	BOOL m_bAutoDestroyParent;
	HMENU GetMenuBCG () const
	{
		return m_hMenu;
	}

	virtual CBCGPopupMenuBar* GetMenuBar ()
	{
		return &m_wndMenuBar;
	}

	void SetAutoDestroy (BOOL bAutoDestroy = TRUE)
	{
		m_bAutoDestroy = bAutoDestroy;
	}

	CBCGToolbarMenuButton* GetParentButton () const
	{
		return m_pParentBtn;
	}

	static CBCGPopupMenu::ANIMATION_TYPE GetAnimationType ()
	{
		return m_AnimationType;
	}

	static void SetAnimationType (CBCGPopupMenu::ANIMATION_TYPE type)
	{
		m_AnimationType = type;
	}

	BOOL IsScrollable () const
	{
		return m_bScrollable;
	}

	void EnableScrolling (BOOL /*bEnable*/ = TRUE)
	{
		// This method is obsolete;
	}

	void SetMaxWidth (int iMaxWidth);

   // -- By Alex Corazzin (2) --
   CWnd* GetMessageWnd () const
   {
	   return m_pMessageWnd; 
   }

   virtual BOOL PostCommand (UINT uiCommandID);
   // ------------------------

public:
	virtual bool CanProcessGlobalCmd(){	return false; }

protected:
	CWnd		              *m_pMessageWnd; // By Alex Corazzin (2)

	CBCGToolbarMenuButton*	m_pParentBtn;
	CBCGPopupMenuBar		m_wndMenuBar;
	HMENU					m_hMenu;
	CPoint					m_ptLocation;
	CPoint					m_ptLocationAnim;
	CSize					m_FinalSize;
	CSize					m_AnimSize;
	BOOL					m_bIsAnimRight;
	BOOL					m_bIsAnimDown;
	BOOL					m_bAnimationIsDone;
	BOOL					m_bAutoDestroy;

	BOOL					m_bShown;
	BOOL					m_bTobeDstroyed;

	int						m_iMaxWidth;

	CBCGMenuPage* const		m_pMenuCustomizationPage;
	CString					m_strCaption;

	static ANIMATION_TYPE	m_AnimationType;

	BOOL					m_bScrollable;
	CRect					m_rectScrollUp;
	CRect					m_rectScrollDn;
	int						m_iScrollMode;	// -1 - Up, 0 - None, 1 - Down

	int						m_iFadePercent;
	COLORREF*				m_cFadeSrcBits;
	CBitmap					m_iFadeBitmapSrc;
	COLORREF*				m_cFadeDstBits;
	CBitmap					m_iFadeBitmapDst;
	COLORREF*				m_cFadeTmpBits;
	CBitmap					m_iFadeBitmapTmp;

	//-------------------
	// Shadow attributes:
	//-------------------
	int						m_iShadowSize;
	BOOL					m_bSaveShadows;
	CBitmap					m_bmpShadowRight;	// Saved shadow butmaps
	CBitmap					m_bmpShadowBottom;

	static CString			m_strClassName;

// Operations
public:
	void SaveState ();

	CBCGPopupMenu* GetParentPopupMenu () const;
	CBCGToolBar* GetParentToolBar () const;

	CBCGToolbarMenuButton* GetSelItem ();

	int InsertItem (const CBCGToolbarMenuButton& button, int iInsertAt = -1);
	int InsertSeparator (int iInsertAt = -1);

	int GetMenuItemCount () const;
	CBCGToolbarMenuButton* GetMenuItem (int iIndex) const;
	
	BOOL RemoveItem (int iIndex);
	void RemoveAllItems ();

	void SetDefaultItem (UINT uiCmd)
	{
		m_wndMenuBar.m_uiDefaultMenuCmdId = uiCmd;
	}

	static BOOL ActivatePopupMenu (CFrameWnd* pTopFrame, CBCGPopupMenu* pPopupMenu);
	void CloseMenu (BOOL bSetFocusToBar = FALSE);
	void ShowAllCommands ();

	BOOL HideRarelyUsedCommands () const;
	BOOL AreAllCommandsShown () const
	{
		return m_wndMenuBar.m_bAreAllCommandsShown;
	}

	CString GetTitle() {return m_strCaption;}

protected:
	void Initialize ();

	BOOL IsScrollUpAvailable ();
	BOOL IsScrollDnAvailable ();

	void CollapseSubmenus ();
	void DrawImage (CDC* pDC, const CRect& rect, int iImage, BOOL bDrawFrame = FALSE);
	void DrawShadows (CDC& dc, const CRect& rectClient);

	BOOL InitMenuBar ();

	void UpdateBottomWindows (BOOL bCheckOnly = FALSE);

	void DoPaint (CDC* pPaintDC);
	void DrawFade (CDC* pPaintDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPopupMenu)
	public:
    virtual BOOL Create(CWnd* pWndParent, int x, int y, HMENU hMenu, BOOL bLocked = FALSE, BOOL bOwnMessage=FALSE); // Add: Alex Corazzin (2)
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBCGPopupMenu)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#if _MSC_VER >= 1300
		afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
		afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPOPUPMENU_H__A6E1AD75_D511_11D1_A64D_00A0C93A70EC__INCLUDED_)
