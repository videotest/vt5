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

// BCGToolbarSystemMenuButton.cpp: implementation of the CBCGToolbarSystemMenuButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxpriv.h>
#include "BCGToolBar.h"
#include "BCGToolbarSystemMenuButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGToolbarSystemMenuButton, CBCGToolbarMenuButton, VERSIONABLE_SCHEMA | 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolbarSystemMenuButton::CBCGToolbarSystemMenuButton()
{
	m_hSysMenuIcon = NULL;
	m_hSystemMenu = NULL;
}
//****************************************************************************************
CBCGToolbarSystemMenuButton::CBCGToolbarSystemMenuButton (HMENU hSystemMenu, HICON hSystemIcon) :
	CBCGToolbarMenuButton (0, hSystemMenu, -1)
{
	m_hSysMenuIcon = hSystemIcon;
	m_hSystemMenu = hSystemMenu;
}
//****************************************************************************************
CBCGToolbarSystemMenuButton::~CBCGToolbarSystemMenuButton()
{
}
//****************************************************************************************
void CBCGToolbarSystemMenuButton::CopyFrom (const CBCGToolbarButton& s)
{
	CBCGToolbarMenuButton::CopyFrom (s);

	const CBCGToolbarSystemMenuButton& src = (const CBCGToolbarSystemMenuButton&) s;

	m_hSysMenuIcon = src.m_hSysMenuIcon;
	m_hSystemMenu = src.m_hSystemMenu;
}
//****************************************************************************************
SIZE CBCGToolbarSystemMenuButton::OnCalculateSize (CDC* /*pDC*/, const CSize& sizeDefault, 
													BOOL /*bHorz*/)
{
	return CSize (	::GetSystemMetrics (SM_CXSMICON),
					sizeDefault.cy);
}
//****************************************************************************************
void CBCGToolbarSystemMenuButton::OnDraw (CDC* pDC, const CRect& rect, 
					CBCGToolBarImages* /*pImages*/,
					BOOL /*bHorz*/, BOOL /*bCustomizeMode*/,
					BOOL /*bHighlight*/,
					BOOL /*bDrawBorder*/, BOOL /*bGrayDisabledButtons*/)
{
	if (m_hSysMenuIcon != NULL)
	{
		int iHeight = GetSystemMetrics (SM_CYSMICON);
		int iOffset = (rect.Height () - iHeight) / 2;

		::DrawIconEx (*pDC, rect.left, rect.top + iOffset, m_hSysMenuIcon,
			GetSystemMetrics (SM_CXSMICON), iHeight,
			0, NULL, DI_NORMAL);
	}
}
//****************************************************************************************
void CBCGToolbarSystemMenuButton::OnDblClick (CWnd* pWnd)
{
	if (CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

	ASSERT (pWnd != NULL);

	//////////////////////////////////////////////
	// Modify by Yuhu.Wang on 99/11/04
	// Make sure to close the popup menu and
	// find the MDI frame correctly.
	//--------------------------------------------
	OnCancelMode ();

	CFrameWnd* pParentFrame = pWnd->GetParentFrame ();
	if(pParentFrame != NULL && pParentFrame->IsKindOf (RUNTIME_CLASS (CMiniDockFrameWnd)))
	{
		pParentFrame = (CFrameWnd*) pParentFrame->GetParent ();
	}

	CMDIFrameWnd* pMDIFrame = 
		DYNAMIC_DOWNCAST (CMDIFrameWnd, pParentFrame);

	if (pMDIFrame != NULL)
	{
		CMDIChildWnd* pChild = pMDIFrame->MDIGetActive ();
		ASSERT_VALID (pChild);

		//-------------------------------------------
		// Jan Vasina: Check if window can be closed
		//-------------------------------------------
		BOOL bCloseIsDisabled = FALSE;

		CMenu* pSysMenu = pChild->GetSystemMenu (FALSE);
		if (pSysMenu != NULL)
		{
			MENUITEMINFO menuInfo;
			ZeroMemory(&menuInfo,sizeof(MENUITEMINFO));
			menuInfo.cbSize = sizeof(MENUITEMINFO);
			menuInfo.fMask = MIIM_STATE;

			pSysMenu->GetMenuItemInfo (SC_CLOSE, &menuInfo);
			bCloseIsDisabled =	((menuInfo.fState & MFS_GRAYED) || 
								(menuInfo.fState & MFS_DISABLED));
		}

		if (!bCloseIsDisabled)
		{
			pChild->SendMessage (WM_SYSCOMMAND, SC_CLOSE);
		}
	}
	//--------------------------------------------
	//////////////////////////////////////////////
}
//****************************************************************************************
void CBCGToolbarSystemMenuButton::CreateFromMenu (HMENU hMenu)
{
	m_hSystemMenu = hMenu;
}
//****************************************************************************************
HMENU CBCGToolbarSystemMenuButton::CreateMenu () const
{
	ASSERT (m_hSystemMenu != NULL);

	HMENU hMenu = CBCGToolbarMenuButton::CreateMenu ();
	if (hMenu == NULL)
	{
		return NULL;
	}

	//---------------------------------------------------------------------
	// System menu don't produce updating command statuses via the
	// standard MFC idle command targeting. So, we should enable/disable
	// system menu items according to the standard system menu status:
	//---------------------------------------------------------------------
	CMenu* pMenu = CMenu::FromHandle (hMenu);
	ASSERT_VALID (pMenu);

	CMenu* pSysMenu = CMenu::FromHandle (m_hSystemMenu);
	ASSERT_VALID (pSysMenu);

	int iCount = (int) pSysMenu->GetMenuItemCount ();
	for (int i = 0; i < iCount; i ++)
	{
		UINT uiState = pSysMenu->GetMenuState (i, MF_BYPOSITION);
		UINT uiCmd = pSysMenu->GetMenuItemID (i);

		if (uiState & MF_CHECKED)
		{
			pMenu->CheckMenuItem (uiCmd, MF_CHECKED);
		}

		if (uiState & MF_DISABLED)
		{
			pMenu->EnableMenuItem (uiCmd, MF_DISABLED);
		}

		if (uiState & MF_GRAYED)
		{
			pMenu->EnableMenuItem (uiCmd, MF_GRAYED);
		}
	}

	return hMenu;
}
