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

// ButtonsTextList.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonsTextList.h"
#include "afxadv.h"
#include "BCGToolbar.h"
#include "BCGToolbarButton.h"
#include "bcgbarres.h"
#include "bcglocalres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonsTextList

CButtonsTextList::CButtonsTextList() :
	m_sizeButton (0, 0)
{
}

CButtonsTextList::~CButtonsTextList()
{
}


BEGIN_MESSAGE_MAP(CButtonsTextList, CListBox)
	//{{AFX_MSG_MAP(CButtonsTextList)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsTextList message handlers

void CButtonsTextList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDown(nFlags, point);

	int iIndex = GetCurSel ();
	if (iIndex == LB_ERR)
	{
		return;
	}

	//-------------------------------------------
	// Be sure that we realy click into the item!
	//-------------------------------------------
	CRect rect;
	GetItemRect (iIndex, &rect);

	if (!rect.PtInRect (point))
	{
		return;
	}

	//-----------------------------------------------------------
	// Trigger mouse up event (to change selection notification):
	//-----------------------------------------------------------
	SendMessage (WM_LBUTTONUP, nFlags, MAKELPARAM (point.x, point.y));

	//---------------------
	// Get selected button:
	//---------------------
	CBCGToolbarButton* pButton = (CBCGToolbarButton*) GetItemData (iIndex);
	ASSERT_VALID(pButton);

	//--------------------------------------
	// Prepare clipboard data and start drag:
	//--------------------------------------
	COleDataSource srcItem;

	pButton->m_bDragFromCollection = TRUE;
	pButton->PrepareDrag (srcItem);
	pButton->m_bDragFromCollection = FALSE;

	{
		CBCGLocalResource locaRes;
		::SetCursor (AfxGetApp ()->LoadCursor (IDC_BCGBARRES_DELETE));
	}

	srcItem.DoDragDrop
		(DROPEFFECT_COPY|DROPEFFECT_MOVE, &rect, &CBCGToolBar::m_DropSource);
}
//***************************************************************************************
void CButtonsTextList::MeasureItem(LPMEASUREITEMSTRUCT lpMIS) 
{
	ASSERT (lpMIS != NULL);

	UINT uiRowHeight = (UINT) m_sizeButton.cy;
	if (lpMIS->itemHeight < uiRowHeight)
	{
		lpMIS->itemHeight = uiRowHeight;
	}
}
//****************************************************************************************
void CButtonsTextList::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	if (lpDIS->itemID == -1 || lpDIS->itemID == LB_ERR)
	{
		return;
	}

	CBCGToolbarButton* pButton = (CBCGToolbarButton*) GetItemData (lpDIS->itemID);
	ASSERT_VALID(pButton);

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	ASSERT (pDC != NULL);

	CRect rect = lpDIS->rcItem;
	pButton->OnDrawOnCustomizeList (pDC, rect, lpDIS->itemState & ODS_SELECTED);
}
//***************************************************************************************
void CButtonsTextList::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();


	CBCGToolBarImages* pImages = CBCGToolBar::GetImages ();
	if (pImages != NULL)
	{
		m_sizeButton = CSize (	pImages->GetImageSize ().cx + 6,
								pImages->GetImageSize ().cy + 6);
	}	
}
//***************************************************************************************
BOOL CButtonsTextList::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClient;
	GetClientRect (rectClient);

	pDC->FillSolidRect (rectClient, ::GetSysColor (COLOR_BTNFACE));
	return TRUE;
}
