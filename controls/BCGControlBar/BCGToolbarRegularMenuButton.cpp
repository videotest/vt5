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

// BCGToolbarRegularMenuButton.cpp: implementation of the CBCGToolbarRegularMenuButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGToolbarRegularMenuButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CBCGToolbarRegularMenuButton, CBCGToolbarMenuButton, VERSIONABLE_SCHEMA | 1)

BOOL CBCGToolbarRegularMenuButton::OnClick(CWnd* pWnd, BOOL bDelay)
{
	// Standardverarbeitung
	CBCGToolbarMenuButton::OnClick(pWnd, bDelay);

	if (CBCGToolBar::IsCustomizeMode())
		return FALSE;

	CPoint ptPos;

	if (m_bHorz)
	{
		ptPos.x = m_rect.left; 
		ptPos.y = m_rect.bottom;
	}

	else
	{
		ptPos.x = m_rect.right; 
		ptPos.y = m_rect.top;
	}

	CWnd* pParent = m_pWndParent == NULL ? 
						AfxGetMainWnd () :
						m_pWndParent->GetTopLevelFrame ();
	pParent->ClientToScreen(&ptPos);

	pParent->SendMessage(BCGM_SHOWREGULARMENU, m_nID, MAKELONG(ptPos.x, ptPos.y));
	return TRUE;
}
