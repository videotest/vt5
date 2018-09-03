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

// CBCGToolbarCustomize.h : header file
//
// CBCGToolbarCustomize is a modeless property sheet that is 
// created once and not destroyed until the application
// closes.  It is initialized and controlled from
// CPropertyFrame.
 
#ifndef __CBCGTOOLBARCUSTOMIZE_H__
#define __CBCGTOOLBARCUSTOMIZE_H__

#include "bcgcontrolbar.h"
#include "CBCGToolbarCustomizePages.h"
#include "BCGToolsPage.h"
#include "MenuPage.h"
#include "MousePage.h"
#include "KeyboardPage.h"
#include "OptionsPage.h"

class CBCGToolBarImages;
class CBCGToolbarButton;
class CBCGToolBar;
class CBCGUserTool;

//---------------------
// Customization flags:
//---------------------
#define BCGCUSTOMIZE_MENU_SHADOWS		0x0001	// Allow chnage menu shadow appearance
#define BCGCUSTOMIZE_TEXT_LABELS		0x0002	// Allow chnage toolbar text lables below the image
#define BCGCUSTOMIZE_LOOK_2000			0x0004	// Allow chnage look 2000
#define BCGCUSTOMIZE_MENU_ANIMATIONS	0x0008	// Allow chnage menu animations
#define BCGCUSTOMIZE_NOHELP				0x0010	// Remove help button from the customization dialog
#define BCGCUSTOMIZE_CONTEXT_HELP		0x0020	// Add '?' to caption and "What's This" context menu

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarCustomize

class BCGCONTROLBARDLLEXPORT CBCGToolbarCustomize : public CPropertySheet
{
	friend class CBCGToolBar;
	friend class CBCGToolsPage;
	friend class CToolsList;

	DECLARE_DYNAMIC(CBCGToolbarCustomize)

// Construction
public:
	CBCGToolbarCustomize (CFrameWnd* pWndParentFrame, 
						BOOL bAutoSetFromMenus = FALSE,
						UINT uiFlags = (BCGCUSTOMIZE_MENU_SHADOWS | BCGCUSTOMIZE_TEXT_LABELS | BCGCUSTOMIZE_LOOK_2000 | BCGCUSTOMIZE_MENU_ANIMATIONS),
						CList <CRuntimeClass*, CRuntimeClass*>* plistCustomPages = NULL);
// Attributes
public:
	UINT GetFlags () const
	{
		return m_uiFlags;
	}

protected:
	//------------------------------------------
	// Toolbar+menu items divided by categories:
	//------------------------------------------
	CMap<CString, LPCTSTR, CObList*, CObList*>	m_ButtonsByCategory;
	CStringList									m_strCategoriesList;
													// Need for order!
	//----------------
	// Property pages:
	//----------------	
	CBCGCustomizePage*	m_pCustomizePage;
	CBCGToolbarsPage*	m_pToolbarsPage;
	CBCGKeyboardPage*	m_pKeyboardPage;
	CBCGMenuPage*		m_pMenuPage;
	CBCGMousePage*		m_pMousePage;
	CBCGOptionsPage*	m_pOptionsPage;
	CBCGToolsPage*		m_pToolsPage;
	
	// ET: Support for additional custom pages
	CList<CPropertyPage*,CPropertyPage*>	m_listCustomPages;

	CString				m_strAllCommands;

	CFrameWnd*			m_pParentFrame;
	BOOL				m_bAutoSetFromMenus;
	UINT				m_uiFlags;

// Operations
public:

	void AddButton (UINT uiCategoryId, const CBCGToolbarButton& button,
					int iInsertAfter = -1);
	void AddButton (LPCTSTR lpszCategory, const CBCGToolbarButton& button,
					int iInsertAfter = -1);

	int RemoveButton (UINT uiCategoryId, UINT uiCmdId);
	int RemoveButton (LPCTSTR lpszCategory, UINT uiCmdId);

	void ReplaceButton (UINT uiCmd, const CBCGToolbarButton& button);

	BOOL AddToolBar (UINT uiCategoryId, UINT uiToolbarResId);
	BOOL AddToolBar (LPCTSTR lpszCategory, UINT uiToolbarResId);
	BOOL AddMenu (UINT uiMenuResId);
	void AddMenuCommands (const CMenu* pMenu, BOOL bPopup, LPCTSTR lpszCategory = NULL);

	BOOL RenameCategory(LPCTSTR lpszCategoryOld, LPCTSTR lpszCategoryNew);

	BOOL SetUserCategory (LPCTSTR lpszCategory);

	void EnableUserDefinedToolbars (BOOL bEnable = TRUE);
	void EnableTools (CObList* pToolsList);	// List of CBCGUserTool-derived objects

	void FiilCategotiesComboBox (CComboBox& wndCategory) const;
	void FiilCategotiesListBox (CListBox& wndCategory) const;
	void FillAllCommandsList (CListBox& wndListOfCommands) const;

	LPCTSTR GetCommandName (UINT uiCmd) const;
protected:
	void SetFrameCustMode (BOOL bCustMode);
	void ShowToolBar (CBCGToolBar* pToolBar, BOOL bShow);
	//[AY]
	void AddToolBar( CBCGToolBar *pbar );
	void RemoveToolBar( CBCGToolBar *pbar );

	void SetupFromMenus ();
	void AddUserTools (LPCTSTR lpszCategory);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGToolbarCustomize)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//------------------
	// Tools page hooks:
	//------------------
	virtual void OnInitToolsPage () {}
	virtual void OnBeforeChangeTool (CBCGUserTool* /*pSelTool*/) {}
	virtual void OnAfterChangeTool (CBCGUserTool* /*pSelTool*/) {}

public:
	virtual BOOL Create();

// Implementation
public:
	 virtual ~CBCGToolbarCustomize();
	 virtual void PostNcDestroy();

// Generated message map functions
protected:
	//{{AFX_MSG(CBCGToolbarCustomize)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __CBCGTOOLBARCUSTOMIZE_H__