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

// BCGContextMenuManager.h: interface for the CBCGContextMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGCONTEXTMENUMANAGER_H__64F13A54_E5A9_11D1_A08F_00A0C9B05590__INCLUDED_)
#define AFX_BCGCONTEXTMENUMANAGER_H__64F13A54_E5A9_11D1_A08F_00A0C9B05590__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "bcgcontrolbar.h"

class CBCGPopupMenu;

class BCGCONTROLBARDLLEXPORT CBCGContextMenuManager : public CObject
{
public:
	CBCGContextMenuManager();
	virtual ~CBCGContextMenuManager();

// Opreations:
public:
	BOOL AddMenu (UINT uiMenuNameResId, UINT uiMenuResId);
	BOOL AddMenu (LPCTSTR lpszName, UINT uiMenuResId);
//!!!andy modifycation 28.05.99
	BOOL AddMenu (LPCTSTR lpszName, UINT uiMenuNo, HMENU hMenu );
//end andy's modification

	BOOL ShowPopupMenu (UINT uiMenuResId, int x, int y, CWnd* pWndOwner, BOOL bOwnMessage = FALSE);
	CBCGPopupMenu* ShowPopupMenu (HMENU hmenuPopup, int x, int y, CWnd* pWndOwner, BOOL bOwnMessage = FALSE);	// Any menu,
																			// maybe not added
	BOOL LoadState (LPCTSTR lpszProfileName = NULL);
	BOOL SaveState (LPCTSTR lpszProfileName = NULL);

// Customization operations:
	void GetMenuNames (CStringList& listOfNames) const;
	HMENU GetMenuByName (LPCTSTR lpszName) const;
//!!! andy modification - 2.06.99
	HMENU ExportMenu(HMENU hMenu);
	HMENU ExportMenu(UINT uiMenuResId);
///end modification

// Attributes:
protected:
	CMap<UINT, UINT, HMENU, HMENU>			m_Menus;
	CMap<CString, LPCTSTR, HMENU, HMENU> 	m_MenuNames;
};

extern CBCGContextMenuManager*	g_pContextMenuManager;

#endif // !defined(AFX_BCGCONTEXTMENUMANAGER_H__64F13A54_E5A9_11D1_A08F_00A0C9B05590__INCLUDED_)
