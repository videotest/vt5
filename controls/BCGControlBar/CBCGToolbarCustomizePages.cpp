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

// CBCGToolbarCustomizePages.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>
#include "bcgbarres.h"
#include "CBCGToolbarCustomize.h"
#include "CBCGToolbarCustomizePages.h"
#include "BCGtoolbar.h"
#include "BCGToolbarButton.h"
#include "bcglocalres.h"
#include "BCGPopupMenuBar.h"
#include "ToolbarNameDlg.h"
#include "BCGCommandManager.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "BCGDropDown.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBCGCustomizePage, CPropertyPage)
IMPLEMENT_DYNCREATE(CBCGToolbarsPage, CPropertyPage)

extern CObList	gAllToolbars;

/////////////////////////////////////////////////////////////////////////////
// CBCGCustomizePage property page

CBCGCustomizePage::CBCGCustomizePage() : 
	CPropertyPage(CBCGCustomizePage::IDD)
{
	//{{AFX_DATA_INIT(CBCGCustomizePage)
	m_strButtonDescription = _T("");
	//}}AFX_DATA_INIT
}

CBCGCustomizePage::~CBCGCustomizePage()
{
}

void CBCGCustomizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCGCustomizePage)
	DDX_Control(pDX, IDC_BCGBARRES_CATEGORY, m_wndCategory);
	DDX_Control(pDX, IDC_BCGBARRES_USER_TOOLS, m_wndTools);
	DDX_Text(pDX, IDC_BCGBARRES_BUTTON_DESCR, m_strButtonDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCGCustomizePage, CPropertyPage)
	//{{AFX_MSG_MAP(CBCGCustomizePage)
	ON_LBN_SELCHANGE(IDC_BCGBARRES_USER_TOOLS, OnSelchangeUserTools)
	ON_LBN_SELCHANGE(IDC_BCGBARRES_CATEGORY, OnSelchangeCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBCGCustomizePage::OnSelchangeCategory() 
{
	UpdateData ();

	int iIndex = m_wndCategory.GetCurSel ();
	if (iIndex == LB_ERR)
	{
		ASSERT (FALSE);
		return;
	}

	CWaitCursor wait;
	m_wndTools.SetRedraw (FALSE);

	m_wndTools.ResetContent ();

	//------------------------------------------
	// Only "All commands" list shoud be sorted!
	//------------------------------------------
	CString strCategory;
	m_wndCategory.GetText (iIndex, strCategory);

	BOOL bAllCommands =  (strCategory == m_strAllCategory);

	OnChangeSelButton (NULL);

	CObList* pCategoryButtonsList = 
		(CObList*) m_wndCategory.GetItemData (iIndex);
	ASSERT_VALID (pCategoryButtonsList);

	for (POSITION pos = pCategoryButtonsList->GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) pCategoryButtonsList->GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->m_bUserButton = (CMD_MGR.GetCmdImage (pButton->m_nID, FALSE) == -1);

		int iIndex = -1;
		
		if (bAllCommands)
		{
			for (int i = 0; iIndex == -1 && i < m_wndTools.GetCount (); i ++)
			{
				CString strCommand;
				m_wndTools.GetText (i, strCommand);

				int nres = stricmp( strCommand, pButton->m_strText );
				if( nres > 0 )
					iIndex = m_wndTools.InsertString (i, pButton->m_strText);
			}
		}

		if (iIndex == -1)	// Not inserted yet
		{
			iIndex = m_wndTools.AddString (pButton->m_strText);
		}

		m_wndTools.SetItemData (iIndex, (DWORD) pButton);
	}

	m_wndTools.SetRedraw (TRUE);
}
//**************************************************************************************
void CBCGCustomizePage::OnSelchangeUserTools() 
{
	int iIndex = m_wndTools.GetCurSel ();
	if (iIndex == LB_ERR)
	{
		OnChangeSelButton (NULL);
	}
	else
	{
		OnChangeSelButton ((CBCGToolbarButton*) m_wndTools.GetItemData (iIndex));
	}
}
//**************************************************************************************
BOOL CBCGCustomizePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CBCGToolbarCustomize* pWndParent = DYNAMIC_DOWNCAST (CBCGToolbarCustomize, GetParent ());
	ASSERT (pWndParent != NULL);

	pWndParent->FiilCategotiesListBox (m_wndCategory);
	
	m_wndCategory.SetCurSel (0);
	OnSelchangeCategory ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//**********************************************************************************************
void CBCGCustomizePage::OnChangeSelButton (CBCGToolbarButton* pSelButton)
{
	m_strButtonDescription = _T("");

	if (pSelButton != NULL)
	{
		if (pSelButton->m_nID == 0)
		{
			m_strButtonDescription = pSelButton->m_strText;
		}
		else
		{
			CFrameWnd* pParent = GetParentFrame ();
			if (pParent != NULL && pParent->GetSafeHwnd () != NULL)
			{
				pParent->GetMessageString (pSelButton->m_nID,
							m_strButtonDescription);
			}
		}
	}

	m_pSelButton = pSelButton;
	UpdateData (FALSE);
}
//*************************************************************************************
void CBCGCustomizePage::SetUserCategory (LPCTSTR lpszCategory)
{
	ASSERT (lpszCategory != NULL);
	m_strUserCategory = lpszCategory;
}
//*************************************************************************************
void CBCGCustomizePage::SetAllCategory (LPCTSTR lpszCategory)
{
	ASSERT (lpszCategory != NULL);
	m_strAllCategory = lpszCategory;
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarsPage property page

CBCGToolbarsPage::CBCGToolbarsPage(CFrameWnd* pParentFrame) : 
	CPropertyPage(CBCGToolbarsPage::IDD),
	m_bUserDefinedToolbars (FALSE),
	m_pParentFrame (pParentFrame)
{
	//{{AFX_DATA_INIT(CBCGToolbarsPage)
	m_bTextLabels = FALSE;
	//}}AFX_DATA_INIT

	m_pSelectedToolbar = NULL;
	ASSERT_VALID (m_pParentFrame);
}

CBCGToolbarsPage::~CBCGToolbarsPage()
{
}

void CBCGToolbarsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCGToolbarsPage)
	DDX_Control(pDX, IDC_BCGBARRES_TEXT_LABELS, m_wndTextLabels);
	DDX_Control(pDX, IDC_BCGBARRES_RENAME_TOOLBAR, m_bntRenameToolbar);
	DDX_Control(pDX, IDC_BCGBARRES_NEW_TOOLBAR, m_btnNewToolbar);
	DDX_Control(pDX, IDC_BCGBARRES_DELETE_TOOLBAR, m_btnDelete);
	DDX_Control(pDX, IDC_BCGBARRES_RESET, m_btnReset);
	DDX_Control(pDX, IDC_BCGBARRES_TOOLBAR_LIST, m_wndToobarList);
	DDX_Check(pDX, IDC_BCGBARRES_TEXT_LABELS, m_bTextLabels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCGToolbarsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBCGToolbarsPage)
	ON_LBN_SELCHANGE(IDC_BCGBARRES_TOOLBAR_LIST, OnSelchangeToolbarList)
	ON_LBN_DBLCLK(IDC_BCGBARRES_TOOLBAR_LIST, OnDblclkToolbarList)
	ON_BN_CLICKED(IDC_BCGBARRES_RESET, OnReset)
	ON_BN_CLICKED(IDC_BCGBARRES_RESET_ALL, OnResetAll)
	ON_BN_CLICKED(IDC_BCGBARRES_DELETE_TOOLBAR, OnDeleteToolbar)
	ON_BN_CLICKED(IDC_BCGBARRES_NEW_TOOLBAR, OnNewToolbar)
	ON_BN_CLICKED(IDC_BCGBARRES_RENAME_TOOLBAR, OnRenameToolbar)
	ON_BN_CLICKED(IDC_BCGBARRES_TEXT_LABELS, OnBcgbarresTextLabels)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CBCGToolbarsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
//andy!!!
	GetDlgItem( IDC_BCGBARRES_TEXT_LABELS )->ShowWindow( SW_HIDE );
//end

	if (!m_bUserDefinedToolbars)
	{
		m_btnNewToolbar.EnableWindow (FALSE);

		m_btnNewToolbar.ShowWindow (SW_HIDE);
		m_btnDelete.ShowWindow (SW_HIDE);
		m_bntRenameToolbar.ShowWindow (SW_HIDE);
	}
	
	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);

			//------------------------------
			// Don't add dropdown toolbars!
			//------------------------------
			if (!pToolBar->IsKindOf (RUNTIME_CLASS (CBCGDropDownToolBar)))
			{
				//----------------------------------------------------------------------
				 // Check, if toolbar belongs to this dialog's parent main frame window
				// (by Peter Schregle):
				//----------------------------------------------------------------------
				if (m_pParentFrame->GetTopLevelFrame() == 
						pToolBar->GetTopLevelFrame () &&
					!pToolBar->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)) &&
					!pToolBar->m_bMasked)
				{
					CString strName;
					pToolBar->GetWindowText (strName);

					if (strName.IsEmpty ())
					{
						CBCGLocalResource locaRes;
						strName.LoadString (IDS_BCGBARRES_UNTITLED_TOOLBAR);
					}

					int iIndex = m_wndToobarList.AddString (strName);
					m_wndToobarList.SetItemData (iIndex, (DWORD) pToolBar);

					if (pToolBar->GetStyle () & WS_VISIBLE)
					{
						m_wndToobarList.SetCheck (iIndex, 1);
					}

					m_wndToobarList.EnableCheck (iIndex, pToolBar->CanBeClosed ());
				}
			}
		}
	}

	CBCGToolbarCustomize* pWndParent = DYNAMIC_DOWNCAST (CBCGToolbarCustomize, GetParent ());
	ASSERT (pWndParent != NULL);

	if ((pWndParent->GetFlags () & BCGCUSTOMIZE_TEXT_LABELS) == 0)
	{
		m_wndTextLabels.ShowWindow (SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//*************************************************************************************
void CBCGToolbarsPage::OnSelchangeToolbarList() 
{
	int iIndex = m_wndToobarList.GetCurSel ();
	if (iIndex == LB_ERR)
	{
		m_pSelectedToolbar = NULL;
		m_btnReset.EnableWindow (FALSE);
		m_btnDelete.EnableWindow (FALSE);
		m_bntRenameToolbar.EnableWindow (FALSE);
		m_wndTextLabels.EnableWindow (FALSE);
		return;
	}

	m_pSelectedToolbar = (CBCGToolBar*) m_wndToobarList.GetItemData (iIndex);
	ASSERT_VALID(m_pSelectedToolbar);

	m_btnReset.EnableWindow (m_pSelectedToolbar->CanBeRestored ());
	m_btnDelete.EnableWindow (IsUserDefinedToolBar (m_pSelectedToolbar));
	m_bntRenameToolbar.EnableWindow (IsUserDefinedToolBar (m_pSelectedToolbar));
	m_wndTextLabels.EnableWindow (m_pSelectedToolbar->AllowChangeTextLabels ());

	m_bTextLabels = m_pSelectedToolbar->AreTextLabels ();
	UpdateData (FALSE);
}
//*************************************************************************************
void CBCGToolbarsPage::OnDblclkToolbarList() 
{
	int iIndex = m_wndToobarList.GetCurSel ();
	if (iIndex != LB_ERR)
	{
		m_pSelectedToolbar = (CBCGToolBar*) m_wndToobarList.GetItemData (iIndex);
		ASSERT_VALID(m_pSelectedToolbar);

		if (m_pSelectedToolbar->CanBeClosed ())
		{
			m_wndToobarList.SetCheck (iIndex, !m_wndToobarList.GetCheck (iIndex));
		}
		else
		{
			MessageBeep ((UINT) -1);
		}
	}

	OnSelchangeToolbarList ();
}
//*************************************************************************************
void CBCGToolbarsPage::ShowToolBar (CBCGToolBar* pToolBar, BOOL bShow)
{
	if (m_wndToobarList.GetSafeHwnd () == NULL)
	{
		return;
	}

	for (int i = 0; i < m_wndToobarList.GetCount (); i ++)
	{
		CBCGToolBar* pListToolBar = (CBCGToolBar*) m_wndToobarList.GetItemData (i);
		ASSERT_VALID(pListToolBar);

		if (pListToolBar == pToolBar)
		{
			m_wndToobarList.SetCheck (i, bShow);
			break;
		}
	}
}
//**************************************************************************************
void CBCGToolbarsPage::OnReset() 
{
	//[AY]
	CWnd	*pParentFrame = AfxGetMainWnd();
	pParentFrame->SendMessage( BCGM_RESETTOOLBAR );
//	CBCGToolbarCustomize* pWndParent = DYNAMIC_DOWNCAST (CBCGToolbarCustomize, GetParent ());
//	pWndParent->EndDialog( IDOK );

	/*ASSERT (m_pSelectedToolbar != NULL);
	ASSERT (m_pSelectedToolbar->CanBeRestored ());

	{
		CBCGLocalResource locaRes;

		CString strName;
		m_pSelectedToolbar->GetWindowText (strName);

		CString strPrompt;
		strPrompt.Format (IDS_BCGBARRES_RESET_TOOLBAR_FMT, strName);

		if (MessageBox (strPrompt, NULL, MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}

	m_pSelectedToolbar->RestoreOriginalstate ();*/
}
//**************************************************************************************
void CBCGToolbarsPage::OnResetAll() 
{
	//[AY]
	CWnd	*pParentFrame = AfxGetMainWnd();
	pParentFrame->SendMessage( BCGM_RESETTOOLBAR );
//	CBCGToolbarCustomize* pWndParent = DYNAMIC_DOWNCAST (CBCGToolbarCustomize, GetParent ());
//	pWndParent->EndDialog( IDOK );
/*	{
		CBCGLocalResource locaRes;
		if (AfxMessageBox (IDS_BCGBARRES_RESET_ALL_TOOLBARS, MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}

	CMD_MGR.ClearAllCmdImages ();

	//------------------------------------------
	// Fill image hash by the default image ids:
	//------------------------------------------
	for (POSITION pos = CBCGToolBar::m_DefaultImages.GetStartPosition (); pos != NULL;)
	{
		UINT uiCmdId;
		int iImage;

		CBCGToolBar::m_DefaultImages.GetNextAssoc (pos, uiCmdId, iImage);
		CMD_MGR.SetCmdImage (uiCmdId, iImage, FALSE);
	}

	for (int i = 0; i < m_wndToobarList.GetCount (); i ++)
	{
		CBCGToolBar* pListToolBar = (CBCGToolBar*) m_wndToobarList.GetItemData (i);
		ASSERT_VALID(pListToolBar);

		if (pListToolBar->CanBeRestored ())
		{
			pListToolBar->RestoreOriginalstate ();
		}
	}*/
}
//**********************************************************************************
void CBCGToolbarsPage::OnDeleteToolbar() 
{
	ASSERT (m_pSelectedToolbar != NULL);
	ASSERT (IsUserDefinedToolBar (m_pSelectedToolbar));

	CFrameWnd* pParentFrame = GetParentFrame ();
	if (pParentFrame == NULL)
	{
		MessageBeep (MB_ICONASTERISK);
		return;
	}

	{
		CBCGLocalResource locaRes;

		CString strName;
		m_pSelectedToolbar->GetWindowText (strName);

		CString strPrompt;
		strPrompt.Format (IDS_BCGBARRES_DELETE_TOOLBAR_FMT, strName);

		if (MessageBox (strPrompt, NULL, MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}

	if (pParentFrame->SendMessage (BCGM_DELETETOOLBAR, 0, (LPARAM) m_pSelectedToolbar)
		== 0)
	{
		MessageBeep (MB_ICONASTERISK);
		return;
	}

	//[AY] remove text
}
//**********************************************************************************
void CBCGToolbarsPage::OnNewToolbar()
{
	CString strToolbarName;
	{
		CBCGLocalResource locaRes;

		CToolbarNameDlg dlg (this);
		if (dlg.DoModal () != IDOK)
		{
			return;
		}

		strToolbarName = dlg.m_strToolbarName.Trim();
	}

	CFrameWnd* pParentFrame = GetParentFrame ();
	if (pParentFrame == NULL)
	{
		MessageBeep (MB_ICONASTERISK);
		return;
	}

	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT_VALID(pToolBar);
		CString strName1;
		pToolBar->GetWindowText (strName1);
		if (strName1 == strToolbarName)
		{
			AfxMessageBox(IDS_TOOLBAR_ALREADY_EXIST, MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}


	CBCGToolBar* pNewToolbar = 
		(CBCGToolBar*)pParentFrame->SendMessage (BCGM_CREATETOOLBAR, 0,
			(LPARAM) (LPCTSTR) strToolbarName);
	if (pNewToolbar == NULL)
	{
		return;
	}

	//[AY] remove text
}
//**********************************************************************************
void CBCGToolbarsPage::OnRenameToolbar() 
{
	ASSERT (m_pSelectedToolbar != NULL);
	ASSERT (IsUserDefinedToolBar (m_pSelectedToolbar));

	CString strToolbarName;
	{
		CBCGLocalResource locaRes;

		CToolbarNameDlg dlg (this);
		m_pSelectedToolbar->GetWindowText (dlg.m_strToolbarName);

		if (dlg.DoModal () != IDOK)
		{
			return;
		}

		strToolbarName = dlg.m_strToolbarName.Trim();
	}

	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT_VALID(pToolBar);
		CString strName1;
		pToolBar->GetWindowText (strName1);
		if (strName1 == strToolbarName)
		{
			AfxMessageBox(IDS_TOOLBAR_ALREADY_EXIST, MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}

	m_pSelectedToolbar->SetWindowText (strToolbarName);
	if (m_pSelectedToolbar->IsFloating ())
	{
		//-----------------------------
		// Change floating frame title:
		//-----------------------------
		CFrameWnd* pParent = m_pSelectedToolbar->GetParentFrame();

		CMiniDockFrameWnd* pDockFrame =
			DYNAMIC_DOWNCAST (CMiniDockFrameWnd, pParent);
		if (pDockFrame != NULL)
		{
			pDockFrame->SetWindowText (strToolbarName);
			pDockFrame->m_wndDockBar.SetWindowText (strToolbarName);
		}
	}

	m_wndToobarList.DeleteString (m_wndToobarList.GetCurSel ());

	int iIndex = m_wndToobarList.AddString (strToolbarName);
	m_wndToobarList.SetItemData (iIndex, (DWORD) m_pSelectedToolbar);

	if (m_pSelectedToolbar->GetStyle () & WS_VISIBLE)
	{
		m_wndToobarList.SetCheck (iIndex, 1);
	}

	m_wndToobarList.SetCurSel (iIndex);
	m_wndToobarList.SetTopIndex (iIndex);

	OnSelchangeToolbarList ();
}
//*************************************************************************************
BOOL CBCGToolbarsPage::IsUserDefinedToolBar (const CBCGToolBar* pToolBar) const
{
	ASSERT_VALID (m_pParentFrame);

	CBCGMDIFrameWnd* pMainMDIFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, m_pParentFrame);
	if (pMainMDIFrame != NULL)
	{
		return pMainMDIFrame->m_Impl.IsUserDefinedToolbar (pToolBar);
	}

	CBCGFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, m_pParentFrame);
	if (pMainFrame != NULL)
	{
		return pMainFrame->m_Impl.IsUserDefinedToolbar (pToolBar);
	}

	return FALSE;
}
//******************************************************************************
BOOL CBCGToolbarsPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT uiCode = HIWORD (wParam);
	UINT uiID = LOWORD (wParam);

	if (uiCode == CLBN_CHKCHANGE && uiID == IDC_BCGBARRES_TOOLBAR_LIST)
	{
		int iIndex = m_wndToobarList.GetCurSel ();
		if (iIndex != LB_ERR)
		{
			CBCGToolBar* pToolbar = (CBCGToolBar*) m_wndToobarList.GetItemData (iIndex);
			ASSERT_VALID (pToolbar);

			CFrameWnd* pParentFrame = (pToolbar->m_pDockSite == NULL) ?
				pToolbar->GetDockingFrame() : pToolbar->m_pDockSite;
			ASSERT_VALID(pParentFrame);

			if (pToolbar->CanBeClosed ())
			{
				//-------------------
				// Show/hide toolbar:
				//-------------------
				pParentFrame->ShowControlBar (pToolbar,
					m_wndToobarList.GetCheck (iIndex), FALSE);
			}
			else if (m_wndToobarList.GetCheck (iIndex) == 0)
			{
				//----------------------------------
				// Toolbar should be visible always!
				//----------------------------------
				m_wndToobarList.SetCheck (iIndex, TRUE);
				MessageBeep ((UINT) -1);
			}
		}
	}
	
	return CPropertyPage::OnCommand(wParam, lParam);
}
//***********************************************************************************
void CBCGToolbarsPage::OnBcgbarresTextLabels() 
{
	UpdateData ();

	ASSERT_VALID (m_pSelectedToolbar);
	m_pSelectedToolbar->EnableTextLabels (m_bTextLabels);
}


//[AY]
void CBCGToolbarsPage::AddToolBar( CBCGToolBar *pNewToolbar )
{	
	if( !GetSafeHwnd() )return;
	ASSERT_VALID (pNewToolbar);

	if (m_pParentFrame->GetTopLevelFrame() != 
						pNewToolbar->GetTopLevelFrame () ||
					pNewToolbar->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)) ||
					pNewToolbar->m_bMasked)
					return;

	CString strName;
	pNewToolbar->GetWindowText( strName );

	if (strName.IsEmpty ())
	{
		CBCGLocalResource locaRes;
		strName.LoadString (IDS_BCGBARRES_UNTITLED_TOOLBAR);
	}

	int iIndex = m_wndToobarList.AddString (strName);
	m_wndToobarList.SetItemData (iIndex, (DWORD) pNewToolbar);

	m_wndToobarList.SetCheck (iIndex, 1);
	m_wndToobarList.SetCurSel (iIndex);
	m_wndToobarList.SetTopIndex (iIndex);

	OnSelchangeToolbarList ();
}
void CBCGToolbarsPage::RemoveToolBar( CBCGToolBar *pToolBar )
{	
	if( !GetSafeHwnd() )return;
	for (int i = 0; i < m_wndToobarList.GetCount (); i ++)
	{
		CBCGToolBar* pListToolBar = (CBCGToolBar*) m_wndToobarList.GetItemData (i);
		ASSERT_VALID(pListToolBar);

		if (pListToolBar == pToolBar)
		{
			m_wndToobarList.DeleteString (i);
			m_wndToobarList.SetCurSel (0);
			OnSelchangeToolbarList ();
			break;
		}
	}
}
