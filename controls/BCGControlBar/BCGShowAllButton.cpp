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

// BCGShowAllButton.cpp: implementation of the CBCGShowAllButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGShowAllButton.h"
#include "BCGMenuBar.h"
#include "BCGPopupMenuBar.h"
#include "BCGPopupMenu.h"
//#include "MenuImages.h"
#include "globals.h"
#include "KeyHelper.h"
#include "bcgbarres.h"
#include "bcglocalres.h"

IMPLEMENT_DYNCREATE(CBCGShowAllButton, CBCGToolbarMenuButton)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGShowAllButton::CBCGShowAllButton()
{
}
//***************************************************************************************
CBCGShowAllButton::~CBCGShowAllButton()
{
}
//***************************************************************************************
void CBCGShowAllButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* /*pImages*/,
								BOOL /*bHorz*/, BOOL /*bCustomizeMode*/, BOOL bHighlight,
								BOOL /*bDrawBorder*/, BOOL /*bGrayDisabledButtons*/)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	CRect rectBar = rect;
	rectBar.DeflateRect (0, 1);
	rectBar.right += TEXT_MARGIN;

	//---------------
	// Interior grey:
	//---------------
	if (bHighlight)
	{
		CBCGToolBarImages::FillDitheredRect (pDC, rectBar);
	}
	else
	{
		pDC->FillRect (rectBar, &globalData.brBtnFace);
	}

	//-----------------------
	// Draw "show all" image:
	//-----------------------
	CPoint pointImage (
		rectBar.left + (rectBar.Width () - CMenuImages::Size ().cx) / 2,
		rectBar.top + (rectBar.Height () - CMenuImages::Size ().cy) / 2);

	CMenuImages::Draw (pDC, CMenuImages::IdArowShowAll, pointImage);

	//--------------------
	// Draw button border:
	//--------------------
	if (m_nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		//-----------------------
		// Pressed in or checked:
		//-----------------------
		pDC->Draw3dRect (&rectBar,
			globalData.clrBtnShadow,
			globalData.clrBtnHilite);
	}
	else if (bHighlight)
	{
		pDC->Draw3dRect (&rectBar, globalData.clrBtnHilite,
								globalData.clrBtnShadow);
	}
}
//***********************************************************************************
SIZE CBCGShowAllButton::OnCalculateSize (
								CDC* /*pDC*/,
								const CSize& sizeDefault,
								BOOL /*bHorz*/)
{
	return CSize (sizeDefault.cx, CMenuImages::Size ().cy + 2 * TEXT_MARGIN);
}
//************************************************************************************
BOOL CBCGShowAllButton::OnClick (CWnd* /*pWnd*/, BOOL bDelay)
{
	CBCGPopupMenuBar* pParentMenuBar = DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);
	if (pParentMenuBar == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (bDelay)
	{
		if (CBCGMenuBar::IsShowAllCommandsDelay ())
		{
			pParentMenuBar->StartPopupMenuTimer (this);
		}

		return TRUE;
	}

	CBCGPopupMenu* pParentMenu = 
		DYNAMIC_DOWNCAST (CBCGPopupMenu, pParentMenuBar->GetParent ());
	if (pParentMenu == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	pParentMenu->ShowAllCommands ();
	return TRUE;
}
//************************************************************************************
BOOL CBCGShowAllButton::OpenPopupMenu (CWnd* pWnd)
{
	return OnClick (pWnd, FALSE);
}
//************************************************************************************
BOOL CBCGShowAllButton::OnToolHitTest (const CWnd* /*pWnd*/, TOOLINFO* pTI)
{
	ASSERT (pTI != NULL);

	CString strText;
	CString strKey;

	ACCEL accel;
	accel.fVirt = FVIRTKEY | FCONTROL;
	accel.key = VK_DOWN;

	CKeyHelper helper (&accel);
	helper.Format (strKey);

	CBCGLocalResource locaRes;
	strText.Format (IDS_BCGBARRES_EXPAND_FMT, strKey);

	pTI->lpszText = (LPTSTR) ::calloc ((strText.GetLength () + 1), sizeof (TCHAR));
	_tcscpy (pTI->lpszText, strText);

	pTI->uId = 0;
	return TRUE;
}
