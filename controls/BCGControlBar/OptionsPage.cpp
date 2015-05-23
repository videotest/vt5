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

// OptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "bcgbarres.h"
#include "bcgcontrolbar.h"
#include "OptionsPage.h"
#include "BCGToolBar.h"
#include "BCGMenuBar.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "bcglocalres.h"
#include "CBCGToolbarCustomize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGOptionsPage property page

IMPLEMENT_DYNCREATE(CBCGOptionsPage, CPropertyPage)

CBCGOptionsPage::CBCGOptionsPage(BOOL bIsMenuBarExist) : 
	CPropertyPage(CBCGOptionsPage::IDD),
	m_bIsMenuBarExist (bIsMenuBarExist)
{
	//{{AFX_DATA_INIT(CBCGOptionsPage)
	m_bShowTooltips = CBCGToolBar::m_bShowTooltips;
	m_bShowShortcutKeys = CBCGToolBar::m_bShowShortcutKeys;
	m_bRecentlyUsedMenus = CBCGMenuBar::m_bRecentlyUsedMenus;
	m_bShowAllMenusDelay = CBCGMenuBar::m_bShowAllMenusDelay;
	m_bLargeIcons = CBCGToolBar::m_bLargeIcons;
	m_bLook2000 = CBCGToolBar::IsLook2000 ();
	//}}AFX_DATA_INIT

}

CBCGOptionsPage::~CBCGOptionsPage()
{
}

void CBCGOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCGOptionsPage)
	DDX_Control(pDX, IDC_BCGBARRES_LOOK2000, m_wndLook2000);
	DDX_Control(pDX, IDC_BCGBARRES_LARGE_ICONS, m_wndLargeIcons);
	DDX_Control(pDX, IDC_BCGBARRES_SHOW_RECENTLY_USED_MENUS, m_wndRUMenus);
	DDX_Control(pDX, IDC_BCGBARRES_RESET_USAGE_DATA, m_wndResetUsageBtn);
	DDX_Control(pDX, IDC_RU_MENUS_TITLE, m_wndRuMenusLine);
	DDX_Control(pDX, IDC_RU_MENUS_LINE, m_wndRuMenusTitle);
	DDX_Control(pDX, IDC_BCGBARRES_SHOW_MENUS_DELAY, m_wndShowAllMenusDelay);
	DDX_Control(pDX, IDC_BCGBARRES_SHOW_TOOLTIPS_WITH_KEYS, m_wndShowShortcutKeys);
	DDX_Check(pDX, IDC_BCGBARRES_SHOW_TOOLTIPS, m_bShowTooltips);
	DDX_Check(pDX, IDC_BCGBARRES_SHOW_TOOLTIPS_WITH_KEYS, m_bShowShortcutKeys);
	DDX_Check(pDX, IDC_BCGBARRES_SHOW_RECENTLY_USED_MENUS, m_bRecentlyUsedMenus);
	DDX_Check(pDX, IDC_BCGBARRES_SHOW_MENUS_DELAY, m_bShowAllMenusDelay);
	DDX_Check(pDX, IDC_BCGBARRES_LARGE_ICONS, m_bLargeIcons);
	DDX_Check(pDX, IDC_BCGBARRES_LOOK2000, m_bLook2000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCGOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBCGOptionsPage)
	ON_BN_CLICKED(IDC_BCGBARRES_SHOW_TOOLTIPS_WITH_KEYS, OShowTooltipsWithKeys)
	ON_BN_CLICKED(IDC_BCGBARRES_SHOW_TOOLTIPS, OnShowTooltips)
	ON_BN_CLICKED(IDC_BCGBARRES_RESET_USAGE_DATA, OnResetUsageData)
	ON_BN_CLICKED(IDC_BCGBARRES_SHOW_RECENTLY_USED_MENUS, OnShowRecentlyUsedMenus)
	ON_BN_CLICKED(IDC_BCGBARRES_SHOW_MENUS_DELAY, OnShowMenusDelay)
	ON_BN_CLICKED(IDC_BCGBARRES_LARGE_ICONS, OnLargeIcons)
	ON_BN_CLICKED(IDC_BCGBARRES_LOOK2000, OnBcgbarresLook2000)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGOptionsPage message handlers

BOOL CBCGOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_wndShowShortcutKeys.EnableWindow (m_bShowTooltips);
	m_wndShowAllMenusDelay.EnableWindow (m_bRecentlyUsedMenus);

	if (CBCGToolBar::m_lstBasicCommands.IsEmpty () || !m_bIsMenuBarExist)
	{
		m_wndRUMenus.ShowWindow (SW_HIDE);
		m_wndResetUsageBtn.ShowWindow (SW_HIDE);
		m_wndRuMenusLine.ShowWindow (SW_HIDE);
		m_wndRuMenusTitle.ShowWindow (SW_HIDE);
		m_wndShowAllMenusDelay.ShowWindow (SW_HIDE);
	}

	CBCGToolbarCustomize* pWndParent = DYNAMIC_DOWNCAST (CBCGToolbarCustomize, GetParent ());
	ASSERT (pWndParent != NULL);

	if ((pWndParent->GetFlags () & BCGCUSTOMIZE_LOOK_2000) == 0)
	{
		m_wndLook2000.ShowWindow (SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//*******************************************************************************
void CBCGOptionsPage::OShowTooltipsWithKeys() 
{
	UpdateData ();
	CBCGToolBar::m_bShowShortcutKeys = m_bShowShortcutKeys;
}
//*******************************************************************************
void CBCGOptionsPage::OnShowTooltips() 
{
	UpdateData ();

	CBCGToolBar::m_bShowTooltips = m_bShowTooltips;
	m_wndShowShortcutKeys.EnableWindow (m_bShowTooltips);
}
//******************************************************************************
void CBCGOptionsPage::OnResetUsageData() 
{
	CBCGLocalResource locaRes;
	if (AfxMessageBox (IDS_BCGBARRES_RESET_USAGE_WARNING, MB_YESNO) == IDYES)
	{
		CBCGToolBar::m_UsageCount.Reset ();
	}
}
//*******************************************************************************
void CBCGOptionsPage::OnShowRecentlyUsedMenus() 
{
	UpdateData ();
	m_wndShowAllMenusDelay.EnableWindow (m_bRecentlyUsedMenus);

	CBCGMenuBar::m_bRecentlyUsedMenus = m_bRecentlyUsedMenus;
}
//*******************************************************************************
void CBCGOptionsPage::OnShowMenusDelay() 
{
	UpdateData ();

	CBCGMenuBar::m_bShowAllMenusDelay = m_bShowAllMenusDelay;
}
//*******************************************************************************
void CBCGOptionsPage::OnLargeIcons() 
{
	UpdateData ();
	CBCGToolBar::SetLargeIcons (m_bLargeIcons);
}
//*******************************************************************************
void CBCGOptionsPage::OnBcgbarresLook2000() 
{
	UpdateData ();
	CBCGToolBar::SetLook2000 (m_bLook2000);	
	AfxGetMainWnd()->Invalidate();
}
