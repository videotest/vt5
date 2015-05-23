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

// BCGContextMenuManager.cpp: implementation of the CBCGContextMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGContextMenuManager.h"
#include "BCGPopupMenu.h"
#include "MenuHash.h"
#include "globals.h"
#include "RegPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMenuHash g_menuHash;

static const CString strMenusProfile = _T("BCGContextMenuManager");

CBCGContextMenuManager*	g_pContextMenuManager = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGContextMenuManager::CBCGContextMenuManager()
{
	ASSERT (g_pContextMenuManager == NULL);
	g_pContextMenuManager = this;
}
//***********************************************************************************************
CBCGContextMenuManager::~CBCGContextMenuManager()
{
	for (POSITION pos = m_Menus.GetStartPosition (); pos != NULL;)
	{
		UINT uiResId;
		HMENU hMenu;

		m_Menus.GetNextAssoc (pos, uiResId, hMenu);
		::DestroyMenu (hMenu);
	}

	g_pContextMenuManager = NULL;
}
//**********************************************************************************
BOOL CBCGContextMenuManager::AddMenu(UINT uiMenuNameResId, UINT uiMenuResId)
{
	CString strMenuName;
	strMenuName.LoadString (uiMenuNameResId);

	return AddMenu (strMenuName, uiMenuResId);
}
//***********************************************************************************************
BOOL CBCGContextMenuManager::AddMenu(LPCTSTR lpszName, UINT uiMenuResId)
{
	ASSERT (lpszName != NULL);

	CMenu menu;
	if (!menu.LoadMenu (uiMenuResId))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	HMENU hExMenu;
	if (m_Menus.Lookup (uiMenuResId, hExMenu))
	{
		//------------------------------------------
		// Menu with the same name is already exist!
		//------------------------------------------
		ASSERT (FALSE);
		return FALSE;
	}

	HMENU hMenu = menu.Detach ();

	m_Menus.SetAt (uiMenuResId, hMenu);
	m_MenuNames.SetAt (lpszName, hMenu);

	return TRUE;
}
//***********************************************************************************************
BOOL CBCGContextMenuManager::ShowPopupMenu (UINT uiMenuResId, int x, int y, 
											CWnd* pWndOwner, BOOL bOwnMessage)
{
	HMENU hMenu;
	if (!m_Menus.Lookup (uiMenuResId, hMenu) || hMenu == NULL)
	{
		return FALSE;
	}

	if (x == -1 && y == -1 &&	// Undefined position
		pWndOwner != NULL)
	{
		CRect rectParent;
		pWndOwner->GetClientRect (&rectParent);
		pWndOwner->ClientToScreen (&rectParent);

		x = rectParent.left + 5;
		y = rectParent.top + 5;
	}

	HMENU hmenuPopup = ::GetSubMenu (hMenu, 0);
	if (hmenuPopup == NULL)
	{
		#ifdef _DEBUG

		MENUITEMINFO info;
		memset (&info, 0, sizeof (MENUITEMINFO));

		if (!::GetMenuItemInfo (hMenu, 0, TRUE, &info))
		{
			TRACE (_T("Invalid menu: %d\n"), uiMenuResId);
		}
		else
		{
			ASSERT (info.hSubMenu == NULL);
			TRACE (_T("Menu %d, first option '%s' doesn't contain popup menu!\n"), 
					uiMenuResId, info.dwTypeData);
		}

		#endif // _DEBUG
		return FALSE;
	}

	return ShowPopupMenu (hmenuPopup, x, y, pWndOwner, bOwnMessage) != NULL;
}
//***********************************************************************************************
CBCGPopupMenu* CBCGContextMenuManager::ShowPopupMenu (HMENU hmenuPopup, int x, int y,
											CWnd* pWndOwner, BOOL bOwnMessage)
{
	ASSERT (hmenuPopup != NULL);
	if (!bOwnMessage)
	{
		while (pWndOwner != NULL && pWndOwner->GetStyle() & WS_CHILD)
		{
			pWndOwner = pWndOwner->GetParent ();
		}
	}

	CBCGPopupMenu* pPopupMenu = new CBCGPopupMenu;
	if (!globalData.bIsWindowsNT4)
	{
		pPopupMenu->SetAutoDestroy (FALSE);
	}

	if (!pPopupMenu->Create (pWndOwner, x, y, hmenuPopup, FALSE, bOwnMessage))
	{
		return NULL;
	}

	return pPopupMenu;
}
//***********************************************************************************************
void CBCGContextMenuManager::GetMenuNames (CStringList& listOfNames) const
{
	listOfNames.RemoveAll ();

	for (POSITION pos = m_MenuNames.GetStartPosition (); pos != NULL;)
	{
		CString strName;
		HMENU hMenu;

		m_MenuNames.GetNextAssoc (pos, strName, hMenu);
		listOfNames.AddTail (strName);
	}
}
//***********************************************************************************************
HMENU CBCGContextMenuManager::GetMenuByName (LPCTSTR lpszName) const
{
	HMENU hMenu;
	if (!m_MenuNames.Lookup (lpszName, hMenu))
	{
		return NULL;
	}

	return hMenu;
}
//***********************************************************************************************
BOOL CBCGContextMenuManager::LoadState (LPCTSTR lpszProfileName)
{
	CString strProfileName = ::BCGGetRegPath (strMenusProfile, lpszProfileName);

	for (POSITION pos = m_Menus.GetStartPosition (); pos != NULL;)
	{
		UINT uiResId;
		HMENU hMenu;

		m_Menus.GetNextAssoc (pos, uiResId, hMenu);
		ASSERT (hMenu != NULL);

		HMENU hPopupMenu = ::GetSubMenu (hMenu, 0);
		ASSERT (hPopupMenu != NULL);

		CBCGPopupMenuBar bar;
		if (!bar.ImportFromMenu (hPopupMenu))
		{
			return FALSE;
		}

		if (bar.LoadState (strProfileName, 0, uiResId))
		{
			g_menuHash.SaveMenuBar (hPopupMenu, &bar);
		}
	}

	return TRUE;
}
//***********************************************************************************************
BOOL CBCGContextMenuManager::SaveState (LPCTSTR lpszProfileName)
{
	CString strProfileName = ::BCGGetRegPath (strMenusProfile, lpszProfileName);

	for (POSITION pos = m_Menus.GetStartPosition (); pos != NULL;)
	{
		UINT uiResId;
		HMENU hMenu;

		m_Menus.GetNextAssoc (pos, uiResId, hMenu);
		ASSERT (hMenu != NULL);

		HMENU hPopupMenu = ::GetSubMenu (hMenu, 0);
		ASSERT (hPopupMenu != NULL);

		CBCGPopupMenuBar bar;
		if (g_menuHash.LoadMenuBar (hPopupMenu, &bar))
		{
			if (!bar.SaveState (strProfileName, 0, uiResId))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//***********************************************************************************************
//!!!andy modifycation 28.05.99
BOOL CBCGContextMenuManager::AddMenu (LPCTSTR lpszName, UINT uiMenuNo, HMENU hMenu )
{
	HMENU hExMenu;
	if( m_Menus.Lookup( uiMenuNo, hExMenu ) )
	{
		//------------------------------------------
		// Menu with the same name is already exist!
		//------------------------------------------
		ASSERT (FALSE);
		return FALSE;
	}

	m_Menus.SetAt( uiMenuNo, hMenu );
	m_MenuNames.SetAt( lpszName, hMenu );

	return TRUE;
}
	// modified by Klimov
HMENU CBCGContextMenuManager::ExportMenu(HMENU hMenu)
{
	CBCGPopupMenuBar bar;
	
	if (!g_menuHash.LoadMenuBar(hMenu, &bar))
		return NULL;
	
	HMENU hres = bar.ExportToMenu();
	
	return hres;
}

HMENU CBCGContextMenuManager::ExportMenu(UINT uiMenuResId)
{
	HMENU hMenu;
	if (!m_Menus.Lookup (uiMenuResId, hMenu) || hMenu == NULL)
		return NULL;

	HMENU hmenuPopup = ::GetSubMenu(hMenu, 0);
	if (!hmenuPopup)
		return NULL;
	
	return ExportMenu(hmenuPopup);
}

//end andy's modification
