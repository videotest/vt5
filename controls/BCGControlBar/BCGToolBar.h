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

//
// bcgtoolbar.h : definition of CBCGToolBar
//
// This code is based on the Microsoft Visual C++ sample file
// TOOLBAR.C from the OLDBARS example
//

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "bcgcontrolbar.h"
#include "BCGToolbarDropTarget.h"
#include "BCGToolbarDropSource.h"
#include "BCGToolBarImages.h"
#include "BCGToolbarButton.h"
#include "BCGControlBarImpl.h"
#include "CmdUsageCount.h"
#include "BCGCommandManager.h"
#include "globals.h"

class CBCGToolbarMenuButton;
class CCustomizeButton;

//----------------------------------
// BCGToolbar notification messages:
//----------------------------------
BCGCONTROLBARDLLEXPORT extern UINT BCGM_TOOLBARMENU;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_CUSTOMIZETOOLBAR;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_CREATETOOLBAR;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_DELETETOOLBAR;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_CUSTOMIZEHELP;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_RESETTOOLBAR;
BCGCONTROLBARDLLEXPORT extern UINT BCGM_SHOWREGULARMENU;


static const int dwDefaultToolbarStyle = (WS_CHILD | WS_VISIBLE | CBRS_TOP | 
										  CBRS_GRIPPER | CBRS_HIDE_INPLACE);

class BCGCONTROLBARDLLEXPORT CBCGToolBar : public CControlBar
{
	friend class CBCGToolbarDropTarget;
	friend class CBCGToolbarsPage;
	friend class CBCGOptionsPage;
	friend class CButtonsTextList;
	friend class CBCGCommandManager;

	DECLARE_SERIAL(CBCGToolBar)

protected:
	BOOL						m_bInCommand;

	//--------------
	// Construction:
	//--------------
public:
	CBCGToolBar();
	virtual BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = dwDefaultToolbarStyle,
			UINT nID = AFX_IDW_TOOLBAR,
			const char *pszName = 0);
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = dwDefaultToolbarStyle,
		CRect rcBorders = CRect(1, 1, 1, 1),
		UINT nID = AFX_IDW_TOOLBAR,
		const char *pszName = 0);

	//----------------------------------------------------------------
	// Set toolbar buttons image resources.
	// You should use SINGLE CBCGToolBarImages for ALL your toolbars!
	//----------------------------------------------------------------
	static BOOL SetUserImages (CBCGToolBarImages* pUserImages);

	//------------------------------
	// Clear all images except user:
	//------------------------------
	static void ResetAllImages();

	//--------------------------------
	// Dimension manipulation methods:
	//--------------------------------
	static void SetSizes (SIZE sizeButton, SIZE sizeImage);
	static void SetMenuSizes (SIZE sizeButton, SIZE sizeImage);
	static CSize GetMenuImageSize ();
	static CSize GetMenuButtonSize ();

	void SetLockedSizes (SIZE sizeButton, SIZE sizeImage);
	void SetHeight (int cyHeight);

	//-----------------
	// Toolbar context:
	//-----------------
	virtual BOOL LoadBitmap (UINT uiResID, UINT uiColdResID = 0, 
					UINT uiMenuResID = 0, BOOL bLocked = FALSE,
					UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0);
	virtual BOOL LoadToolBar (UINT uiResID, UINT uiColdResID = 0, 
					UINT uiMenuResID = 0, BOOL bLocked = FALSE,
					UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0);

	//----------------------------
	// Toolbar buttons add/remove:
	//----------------------------
	BOOL SetButtons(const UINT* lpIDArray, int nIDCount);

	int InsertButton (const CBCGToolbarButton& button, int iInsertAt = -1);
	int InsertSeparator (int iInsertAt = -1);

	BOOL ReplaceButton (UINT uiCmd, const CBCGToolbarButton& button);
	
	BOOL RemoveButton (int iIndex);
	virtual void RemoveAllButtons ();

	static BOOL IsLastCommandFromButton (CBCGToolbarButton* pButton);
	static BOOL AddToolBarForImageCollection (UINT uiResID);

	static void SetNonPermittedCommands (CList<UINT, UINT>& lstCommands);
	static BOOL IsCommandPermitted (UINT uiCmd)
	{
		return m_lstUnpermittedCommands.Find (uiCmd) == NULL;
	}

	static void SetBasicCommands (CList<UINT, UINT>& lstCommands);
	static void AddBasicCommand (UINT uiCmd);
	
	static BOOL IsBasicCommand (UINT uiCmd)
	{
		return m_lstBasicCommands.Find (uiCmd) != NULL;
	}

	static const CList<UINT, UINT>& GetBasicCommands ()
	{
		return m_lstBasicCommands;
	}

	static BOOL IsCommandRarelyUsed (UINT uiCmd);

	static void AddCommandUsage (UINT uiCommand)
	{
		m_UsageCount.AddCmd (uiCommand);
	}

	virtual int GetRowHeight () const
	{
		if (m_bDrawTextLabels)
		{
			ASSERT (m_nMaxBtnHeight > 0);
			return m_nMaxBtnHeight;
		}

		return max (globalData.GetTextHeight (m_dwStyle & CBRS_ORIENT_HORZ),
			m_bMenuMode ?
			m_sizeMenuButton.cy > 0 ?
				m_sizeMenuButton.cy : m_sizeButton.cy :
			GetButtonSize ().cy);
	}

	virtual int GetColumnWidth () const
	{
		return m_bMenuMode ?
			m_sizeMenuButton.cx > 0 ?
				m_sizeMenuButton.cx : m_sizeButton.cx :
			GetButtonSize ().cx;
	}

	static void SetHelpMode (BOOL bOn = TRUE);
	virtual void Deactivate ();
	void RestoreFocus ();

	static bool m_bPressed;

	void SetToolBarBtnText (UINT nBtnIndex,
							LPCTSTR szText = NULL,
							BOOL bShowText = TRUE,
							BOOL bShowImage = TRUE);


	//andy
	virtual void EnableLargeIcons (BOOL bEnable);
	
	static void SetLargeIcons (BOOL bLargeIcons = TRUE);
	static BOOL IsLargeIcons ()
	{
		return m_bLargeIcons;
	}

	CSize GetButtonSize () const
	{
		return m_bLocked ?
			m_bLargeIconsAreEnbaled ? m_sizeCurButtonLocked : m_sizeButtonLocked :
			m_bLargeIconsAreEnbaled ? m_sizeCurButton : m_sizeButton;
	}

	CSize GetImageSize () const
	{
		return m_bLocked ?
			m_bLargeIconsAreEnbaled ? m_sizeCurImageLocked : m_sizeImageLocked :
			m_bLargeIconsAreEnbaled ? m_sizeCurImage : m_sizeImage;
	}

	//------------
	// Attributes:
	//------------
public: 
	//andy
	virtual BOOL IsMenuMode() const				{return m_bMenuMode;}
	BOOL		IsApprenceDislpayed() const		{return m_bButtonApprernceMode;}
	
	//paul 19.12.2002 to avoid CControlBar::SetBarStyle ASSERT
	void			SetBarStyleEx( DWORD dwStyle );
	//paul 23.04.2003
	virtual	bool	CanDragDropButtons(){ return true;}



	// standard control bar things
	int CommandToIndex(UINT nIDFind) const;
	UINT GetItemID(int nIndex) const;
	
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	virtual void GetInvalidateItemRect(int nIndex, LPRECT lpRect) const;

	UINT GetButtonStyle(int nIndex) const;
	virtual void SetButtonStyle(int nIndex, UINT nStyle);

	int GetCount () const;
	int ButtonToIndex(const CBCGToolbarButton* pButton) const;
	CBCGToolbarButton* GetButton (int iIndex) const;

	// Find all buttons specified by the given command ID from the all
	// toolbars:
	static int GetCommandButtons (UINT uiCmd, CObList& listButtons);
	
	static BOOL SetCustomizeMode (BOOL bSet = TRUE);
	static BOOL IsCustomizeMode ()
	{
		return m_bCustomizeMode;
	}

	static CBCGToolBar* FromHandlePermanent (HWND hwnd);
	static CBCGToolBarImages* GetImages ()
	{
		return &m_Images;
	}
	static CBCGToolBarImages* GetUserImages ()
	{
		return m_pUserImages;
	}

	static int GetDefaultImage (UINT uiID)
	{
		int iImage;
		if (m_DefaultImages.Lookup (uiID, iImage))
		{
			return iImage;
		}

		return -1;
	}

	int GetImagesOffset () const
	{
		return m_iImagesOffset;
	}

	CBCGToolbarButton* GetHighlightedButton () const;

	static void SetHotTextColor (COLORREF clrText);
	static COLORREF GetHotTextColor ();

	void SetHotBorder (BOOL bShowHotBorder)
	{
		m_bShowHotBorder = bShowHotBorder;
	}

	BOOL GetHotBorder () const
	{
		return m_bShowHotBorder;
	}

	void SetGrayDisabledButtons (BOOL bGrayDisabledButtons)
	{
		m_bGrayDisabledButtons = bGrayDisabledButtons;
	}

	BOOL GetGrayDisabledButtons () const
	{
		return m_bGrayDisabledButtons;
	}

	//------------------------------------------------------
	// Enable/disable quick customization mode ("Alt+drag"):
	//------------------------------------------------------
	static void EnableQuickCustomization (BOOL bEnable = TRUE)
	{
		m_bAltCustomization = bEnable;
	}

	static void SetLook2000 (BOOL bLook2000 = TRUE);

	static BOOL IsLook2000 ()
	{
		return m_bLook2000;
	}

	void EnableCustomizeButton (BOOL bEnable, UINT uiCustomizeCmd, const CString& strCustomizeText);
	void EnableTextLabels (BOOL bEnable = TRUE);

	virtual BOOL AllowChangeTextLabels () const
	{
		return TRUE;
	}

	BOOL AreTextLabels () const
	{
		return m_bTextLabels;
	}

	void SetMaskMode (BOOL bMasked)
	{
		m_bMasked = bMasked;
	}

	void SetPermament (BOOL bPermament = TRUE)
	{
		m_bPermament = bPermament;
	}

	// By Jay Giganti 
	BOOL GetIgnoreSetText () 
	{
		return m_bIgnoreSetText;
	}

	void SetIgnoreSetText (BOOL bValue)
	{
		m_bIgnoreSetText = bValue;
	}

public:
	// for changing button info
	void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
	void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);

	BOOL SetButtonText(int nIndex, LPCTSTR lpszText);
	CString GetButtonText( int nIndex ) const;
	void GetButtonText( int nIndex, CString& rString ) const;

	// Save/load toobar state + buttons:
	void Serialize (CArchive& ar);
	virtual BOOL LoadState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL RemoveStateFromRegistry (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	static BOOL LoadParameters (LPCTSTR lpszProfileName = NULL);
	static BOOL SaveParameters (LPCTSTR lpszProfileName = NULL);

	virtual BOOL CanBeRestored () const;
	virtual BOOL CanBeClosed () const
	{
		return !m_bPermament;
	}

	virtual BOOL RestoreOriginalstate ();
	virtual void OnReset () {}

	static void ResetAll ();

	virtual void AdjustLayout ();

	virtual int HitTest(CPoint point);

	virtual BOOL TranslateChar (UINT nChar);

	virtual BOOL PrevMenu ();
	virtual BOOL NextMenu ();
	BOOL ProcessCommand (CBCGToolbarButton* pButton);
	CBCGToolbarMenuButton* GetDroppedDownMenu (int* pIndex = NULL) const;

	BOOL SetHot (CBCGToolbarButton *pMenuButton);

	virtual BOOL OnSetDefaultButtonText (CBCGToolbarButton* pButton);

	BOOL IsDragButton (const CBCGToolbarButton* pButton) const
	{
		return pButton == m_pDragButton;
	}

	virtual void  OnFillBackground (CDC* /*pDC*/) {}

	static BOOL	m_bExtCharTranslation;

// Implementation
public:
	virtual ~CBCGToolBar();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout (int nLength, DWORD dwMode);

	virtual CSize CalcSize (BOOL bVertDock);
	int WrapToolBar (int nWidth);

	virtual void OnChangeHot (int iHot);

protected:
	virtual CSize CalcLayout (DWORD dwMode, int nLength = -1);
	void  SizeToolBar (int nLength, BOOL bVert = FALSE);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool m_bEnableDrag;
	int  InsertButton (CBCGToolbarButton* pButton, int iInsertAt = -1);
	void InvalidateButton(int nIndex);
	void UpdateButton(int nIndex);

	virtual BOOL DrawButton (CDC* pDC, CBCGToolbarButton* pButton, 
		CBCGToolBarImages* pImages, BOOL bHighlighted,
		BOOL bDrawDisabledImages);
	virtual void DrawDragMarker (CDC* pDC);

	void RebuildAccelerationKeys ();
	virtual CFrameWnd* GetCommandTarget () const
	{
		// determine target of command update
		CFrameWnd* pTarget = (CFrameWnd*) GetOwner();
		if (pTarget == NULL || !pTarget->IsFrameWnd())
		{
			pTarget = GetParentFrame ();
		}

		return pTarget;
	}

protected:
	friend class CBCGWorkspace;

	static CBCGToolBarImages	m_Images;				// Shared toolbar images
	static CBCGToolBarImages	m_ColdImages;			// Shared toolbar "cold" images
	static CBCGToolBarImages	m_DisabledImages;		// Shared disabled images

	static CBCGToolBarImages	m_MenuImages;
	static CBCGToolBarImages	m_DisabledMenuImages;

	static CBCGToolBarImages*	m_pUserImages;			// Shared user-defined images

	CBCGToolBarImages	m_ImagesLocked;				// "Locked" toolbar images
	CBCGToolBarImages	m_ColdImagesLocked;			// "Locked" toolbar "cold" images
	CBCGToolBarImages	m_MenuImagesLocked;			// "Locked" toolbar menu images
	CBCGToolBarImages	m_DisabledImagesLocked;		// "Locked" toolbar disabled images
	CBCGToolBarImages	m_DisabledMenuImagesLocked; // "Locked" toolbar menu disabled images
	BOOL				m_bLocked;
	BOOL				m_bLargeIconsAreEnbaled;

	CBCGControlBarImpl	m_Impl;

	BOOL				m_bMasked;
	BOOL				m_bPermament;	// Can't be closed
	BOOL				m_bButtonApprernceMode;

	BOOL				m_bTextLabels;	// Text labels below the image are available
	BOOL				m_bDrawTextLabels;
	int					m_nMaxBtnHeight;// Actual only if m_bTextLabels is TRUE

	static CMap<UINT, UINT, int, int>	m_DefaultImages;

	static CSize m_sizeButton;			// original size of button
	static CSize m_sizeImage;			// original size of glyph
	static CSize m_sizeMenuButton;		// size of button on the menu
	static CSize m_sizeMenuImage;		// size of image on the menu
	static CSize m_sizeCurButton;		// size of button
	static CSize m_sizeCurImage;		// size of glyph

	CSize m_sizeButtonLocked;			// original size of button of the locked toolbars
	CSize m_sizeImageLocked;			// original size of glyph of the locked toolbars
	CSize m_sizeCurButtonLocked;		// size of button
	CSize m_sizeCurImageLocked;			// size of glyph

	int m_iButtonCapture;       // index of button with capture (-1 => none)
	int m_iHighlighted;			// highlighted button index
	int m_iSelected;			// selected button index
	int	m_iHot;

	int m_iRebarPaneWidth;

	CObList	m_Buttons;
	CBCGToolbarDropTarget	m_DropTarget;
	
	static CBCGToolbarDropSource m_DropSource;
	static BOOL m_bCustomizeMode;
	static BOOL m_bAltCustomizeMode;

	int			m_iDragIndex;
	CRect		m_rectDrag;
	CPen		m_penDrag;
	CBCGToolbarButton* m_pDragButton;
	CPoint		m_ptStartDrag;
	BOOL		m_bIsDragCopy;

	BOOL		m_bStretchButton;
	CRect		m_rectTrack;

	int			m_iImagesOffset;
	UINT		m_uiOriginalResID;	// Toolbar resource ID

	BOOL		m_bTracked;
	CPoint		m_ptLastMouse;

	BOOL		m_bMenuMode;

	CWnd*		m_pWndLastCapture;
	HWND		m_hwndLastFocus;

	static COLORREF	m_clrTextHot;
	
	static HHOOK m_hookMouseHelp;	// Mouse hook for the help mode
	static CBCGToolBar* m_pLastHookedToolbar;

	CMap<UINT, UINT&, CBCGToolbarButton*, CBCGToolbarButton*&>	m_AcellKeys;	// Keyborad acceleration keys

	static BOOL m_bShowTooltips;
	static BOOL m_bShowShortcutKeys;
	static BOOL m_bLargeIcons;

	static CList<UINT, UINT>	m_lstUnpermittedCommands;
	static CList<UINT, UINT>	m_lstBasicCommands;

	static CCmdUsageCount	m_UsageCount;

	BOOL		m_bShowHotBorder;
	BOOL		m_bGrayDisabledButtons;
	BOOL		m_bIgnoreSetText;

	static BOOL	m_bAltCustomization;
	static BOOL m_bLook2000;

	CCustomizeButton*	m_pCustomizeBtn;

	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual DWORD RecalcDelayShow(AFX_SIZEPARENTPARAMS* lpLayout);

	virtual int FindDropIndex (const CPoint point, CRect& rectDrag) const;
	virtual void AdjustLocations ();

	virtual BOOL OnSendCommand (const CBCGToolbarButton* /*pButton*/)	{	return FALSE;	}

	virtual BOOL AllowSelectDisabled () const	{	return FALSE;	}

	virtual void DrawSeparator (CDC* pDC, const CRect& rect, BOOL bHorz);
	virtual CBCGToolbarButton* CreateDroppedButton (COleDataObject* pDataObject);
	virtual BOOL OnKey (UINT /*nChar*/)				{	return FALSE;	}
	virtual void OnCustomizeMode (BOOL bSet);

	virtual BOOL EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup);

	void AddRemoveSeparator (const CBCGToolbarButton* pButton,
						const CPoint& ptStart, const CPoint& ptDrop);
	void ShowCommandMessageString (UINT uiCmdId);

	static LRESULT CALLBACK BCGToolBarMouseProc (int nCode, WPARAM wParam, LPARAM lParam);

	BOOL DropDownMenu (CBCGToolbarButton* pButton);
	int CalcMaxButtonHeight ();

	virtual BOOL OnUserToolTip (CBCGToolbarButton* pButton, CString& strTTText) const;

	//{{AFX_MSG(CBCGToolBar)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnToolbarAppearance();
	afx_msg void OnToolbarDelete();
	afx_msg void OnToolbarImage();
	afx_msg void OnToolbarImageAndText();
	afx_msg void OnToolbarStartGroup();
	afx_msg void OnToolbarText();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBcgbarresToolbarReset();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBcgbarresCopyImage();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBcgbarresToolbarNewMenu();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	afx_msg LRESULT OnHelpHitTest(WPARAM,LPARAM);
//andy
	afx_msg LRESULT OnPaint( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
private:
	bool m_bChecked;
};

#endif //!_TOOLBAR_H_
