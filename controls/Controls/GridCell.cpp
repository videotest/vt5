// GridCell.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for the "default" cell type of the
// grid control. Adds in cell editing.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2000. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
// History:
// Eric Woodruff - 20 Feb 2000 - Added PrintCell() plus other minor changes
// Ken Bertelson - 12 Apr 2000 - Split CGridCell into CGridCell and CGridCellBase
// <kenbertelson@hotmail.com>
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCell.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCell, CGridCellBase)

/////////////////////////////////////////////////////////////////////////////
// GridCell

CGridCell::CGridCell()
{
    Reset();
}

CGridCell::~CGridCell()
{
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Attributes

void CGridCell::Reset()
{
    CGridCellBase::Reset();

    m_strText.Empty();
    m_nImage   = -1;
    m_pGrid    = NULL;
    m_bEditing = FALSE;
    m_pEditWnd = NULL;

#ifdef _WIN32_WCE
    m_nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#else
    m_nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS;
#endif
    m_crBkClr = CLR_DEFAULT;     // Background colour (or CLR_DEFAULT)
    m_crFgClr = CLR_DEFAULT;     // Forground colour (or CLR_DEFAULT)
    m_lfFont;                    // Cell font
    m_nMargin = 3;               // Internal cell margin
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

BOOL CGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    DWORD dwStyle = ES_LEFT;
    if (GetFormat() & DT_RIGHT) 
        dwStyle = ES_RIGHT;
    else if (GetFormat() & DT_CENTER) 
        dwStyle = ES_CENTER;

    m_bEditing = TRUE;
    
    // InPlaceEdit auto-deletes itself
    CGridCtrl* pGrid = GetGrid();
	if ((GetState() & GVIS_FIXED) == GVIS_FIXED)
		rect.top++; rect.bottom++;
    m_pEditWnd = new CInPlaceEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    
    return TRUE;
}

void CGridCell::EndEdit()
{
    if (m_pEditWnd)
        ((CInPlaceEdit*)m_pEditWnd)->EndEdit();
}

void CGridCell::OnEndEdit()
{
    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// GridCell


IMPLEMENT_DYNCREATE(CAdditionGridCell, CGridCell)

// Construction/Destruction
CAdditionGridCell::CAdditionGridCell()
{
}

CAdditionGridCell::~CAdditionGridCell()
{
}

DWORD CAdditionGridCell::GetState()
{
	return CGridCell::GetState() | GVIS_READONLY;
}

BOOL CAdditionGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);
	CBrush * pLightBrush = pGrid->GetLightBrush();

    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldTextColor = pDC->GetTextColor();

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = pGrid->GetFixedTextColor();
    else
        TextClr = GetTextClr();

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pGrid->GetFixedBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }
		
    CCellID FocusCell = pGrid->GetFocusCell();
    bool bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol);
	bool bTracked = (GetState() & GVIS_TRACKED) == GVIS_TRACKED;
	
	if (bDrawFixed && bHiliteFixed && pLightBrush && !bTracked)
	{
		pDC->FillRect(rect, pLightBrush);
	}
    else if (bEraseBkgnd)
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		CBrush brush(TextBkClr);
		pDC->FillRect(rect, &brush);
		brush.DeleteObject();
        rect.right--; rect.bottom--;
    }
    pDC->SetTextColor(TextClr);

    // Draw lines only when wanted
    if (bDrawFixed && pGrid->GetGridLines() != GVL_NONE)
    {
        CCellID FocusCell = pGrid->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol);

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        if (bHiliteFixed)
        {
			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&medpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right - 1, rect.top + 1);
			pDC->LineTo(rect.right - 1, rect.bottom - 1);
			pDC->LineTo(rect.left, rect.bottom - 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left - 1, rect.bottom);

			pDC->SelectObject(pOldPen);
			rect.DeflateRect(1,1);

			lightpen.DeleteObject();
			darkpen.DeleteObject();
			medpen.DeleteObject();
        }
        else
        {
			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			if (bTracked)
			{
				pDC->SelectObject(&medpen);
				pDC->MoveTo(rect.right - 1, rect.top + 1);
				pDC->LineTo(rect.right - 1, rect.bottom - 1);
				pDC->LineTo(rect.left + 1, rect.bottom - 1);
			}

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left - 1, rect.bottom);

			pDC->SelectObject(pOldPen);

			rect.DeflateRect(1,1);
			
			lightpen.DeleteObject();
			darkpen.DeleteObject();
			medpen.DeleteObject();
        }
    }
    rect.DeflateRect(GetMargin(), 0);

	// draw asterisk
	int nSize = (rect.Height() < rect.Width() ? rect.Height() : rect.Width()) / 2;
	CPoint ptc(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);
	pDC->MoveTo(ptc.x - nSize, ptc.y);
	pDC->LineTo(ptc.x + nSize, ptc.y);

	pDC->MoveTo(ptc.x, ptc.y - nSize);
	pDC->LineTo(ptc.x, ptc.y + nSize);

	nSize = (int)(nSize * 0.7);

	pDC->MoveTo(ptc.x - nSize, ptc.y - nSize);
	pDC->LineTo(ptc.x + nSize + 1, ptc.y + nSize+ 1);

	pDC->MoveTo(ptc.x + nSize, ptc.y - nSize);
	pDC->LineTo(ptc.x - nSize - 1, ptc.y + nSize + 1);


    // We want to see '&' characters so use DT_NOPREFIX
//    DrawText(pDC->m_hDC, GetText(), -1, rect, GetFormat() | DT_NOPREFIX);

	pDC->SetTextColor(clrOldTextColor);
    pDC->SetBkMode(nOldMode);
    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

BOOL CAdditionGridCell::PrintCell(CDC* pDC, int nRow, int nCol, CRect rect)
{
	return CGridCell::PrintCell(pDC, nRow, nCol, rect);
}

void CAdditionGridCell::SetText(LPCTSTR szText)
{
	TRACE("%s\n", szText);
}

CSize CAdditionGridCell::GetCellExtent(CDC* pDC)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    CFont* pfont = pGrid->GetBoldFont();
    CFont* pOldFont = pDC->SelectObject(pfont);

    CSize size = pDC->GetTextExtent("AA");
    
	pDC->SelectObject(pOldFont);
    
    size += CSize(4*GetMargin(), 2*GetMargin());
    
    return size;
}

