// GridCellBase.cpp : implementation file
//
// MFC Grid Control - Main grid cell base class
//
// Provides the implementation for the base cell type of the
// grid control. No data is stored (except for state) but default
// implementations of drawing, printingetc provided. MUST be derived
// from to be used.
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
// Ken Bertelson - 12 Apr 2000 - Split CGridCell into CGridCell and CGridCellBase
// <kenbertelson@hotmail.com>
// C Maunder     - 19 May 2000 - Fixed sort arrow drawing (Ivan Ilinov)
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCtrl.h"
#include "GridCellBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellBase, CObject)

/////////////////////////////////////////////////////////////////////////////
// GridCellBase

CGridCellBase::CGridCellBase()
{
    Reset();
}

CGridCellBase::~CGridCellBase()
{
}

/////////////////////////////////////////////////////////////////////////////
// GridCellBase Operations

void CGridCellBase::Reset()
{
    m_nState  = 0;
}

void CGridCellBase::operator=(CGridCellBase& cell)
{
    SetGrid(cell.GetGrid());    // do first in case of dependencies

    SetText(cell.GetText());
    SetImage(cell.GetImage());
    SetData(cell.GetData());
    SetState(cell.GetState());
    SetFormat(cell.GetFormat());
    SetTextClr(cell.GetTextClr());
    SetBackClr(cell.GetBackClr());
    SetFont(cell.GetFont());
    SetMargin(cell.GetMargin());
}


/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

// EFW - Various changes to make it draw cells better when using alternate
// color schemes.  Also removed printing references as that's now done
// by PrintCell() and fixed the sort marker so that it doesn't draw out
// of bounds.
BOOL CGridCellBase::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);
	CBrush * pLightBrush = pGrid->GetLightBrush();

    //TRACE3("Drawing %scell %d, %d\n", bDrawFixed? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldTextColor = pDC->GetTextColor();
    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = (bDrawFixed)? pGrid->GetFixedTextColor() : pGrid->GetTextColor();
    else
        TextClr = GetTextClr();

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = (bDrawFixed)? pGrid->GetFixedBkColor() : pGrid->GetTextBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

    // Draw cell background and highlighting (if necessary)
    if ((GetState() & GVIS_FOCUSED) || (GetState() & GVIS_DROPHILITED))
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if (GetState() & GVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_3DDKSHADOW);
            TextClr = ::GetSysColor(COLOR_3DHIGHLIGHT);
            bEraseBkgnd = TRUE;
        }

        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
			brush.DeleteObject();
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if (pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        // Use same color as text to outline the cell so that it shows
        // up if the background is black.
		if(pGrid->IsDrawFocus())
		{
			CBrush brush(TextClr);
			pDC->FrameRect(rect, &brush);
			brush.DeleteObject();
		}
        pDC->SetTextColor(TextClr);

        // Adjust rect after frame draw if no grid lines
        if (pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        rect.DeflateRect(1,1);
    }
    else if ((GetState() & GVIS_SELECTED))
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DDKSHADOW));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_3DHIGHLIGHT));
    }
    else
    {
		
        CCellID FocusCell = pGrid->GetFocusCell();
        bool bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol);
		bool bTracked = (GetState() & GVIS_TRACKED) == GVIS_TRACKED;
		
		bHiliteFixed = bHiliteFixed && pGrid->IsDrawFocus();

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

    }

    // Setup font and if fixed, draw fixed cell highlighting
	bool bSmall = rect.Width() <= (2 * pGrid->GetResizeCaptureRange() + 1);

	BOOL bHiliteFixed = false;
	bool bTracked = (GetState() & GVIS_TRACKED) == GVIS_TRACKED;
	bTracked = bTracked && pGrid->IsDrawFocus();
    // Draw lines only when wanted
    if (bDrawFixed && pGrid->GetGridLines() != GVL_NONE)
    {
        CCellID FocusCell = pGrid->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol);

		bHiliteFixed = bHiliteFixed && pGrid->IsDrawFocus();

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        if (bHiliteFixed)
        {
//            rect.right++; rect.bottom++;
//            pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
//            rect.DeflateRect(1,1);

			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&medpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			if (bSmall)
				rect.right--;
			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right - 1, rect.top + 1);
			pDC->LineTo(rect.right - 1, rect.bottom - 1);
			pDC->LineTo(rect.left, rect.bottom - 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left - 1, rect.bottom);
			
			if (bSmall)
			{
				rect.right++;
				pDC->SelectObject(&darkpen);
				pDC->MoveTo(rect.right, rect.top);
				pDC->LineTo(rect.right, rect.bottom);
				pDC->LineTo(rect.left - 1, rect.bottom);
			}


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

			if (bSmall)
				rect.right--;

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

			if (bSmall)
			{
				rect.right++;
				pDC->SelectObject(&darkpen);
				pDC->MoveTo(rect.right, rect.top);
				pDC->LineTo(rect.right, rect.bottom);
				pDC->LineTo(rect.left - 1, rect.bottom);
			}

			pDC->SelectObject(pOldPen);
			rect.DeflateRect(1,1);

			lightpen.DeleteObject();
			darkpen.DeleteObject();
			medpen.DeleteObject();
        }
    }
    CFont* pFont = (bHiliteFixed) ? pGrid->GetBoldFont() : pGrid->GetFont();
    CFont * pOldFont = pDC->SelectObject(pFont);

    // Draw Text and image

    rect.DeflateRect(GetMargin(), 0);

    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();

            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;

            if (nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
                && nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin()))
            {
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0)
    {
        CSize size = pDC->GetTextExtent(_T("M"));
        int nOffset = 2;

        // Base the size of the triangle on the smaller of the column
        // height or text height with a slight offset top and bottom.
        // Otherwise, it can get drawn outside the bounds of the cell.
        size.cy -= (nOffset * 2);

        if (size.cy >= rect.Height())
            size.cy = rect.Height() - (nOffset * 2);

        size.cx = size.cy;      // Make the dimensions square

        // Only draw if it'll fit!
        if (size.cx + rect.left < rect.right + (int)(2*GetMargin()))
        {
            CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
            CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
            if (pGrid->GetSortAscending())
            {
                // Draw triangle pointing upwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( rect.right - size.cx + 1, rect.top + nOffset + size.cy + 1);
                pDC->LineTo( rect.right - (size.cx / 2) + 1, rect.top + nOffset + 1 );
                pDC->LineTo( rect.right + 1, rect.top + nOffset + size.cy + 1);
                pDC->LineTo( rect.right - size.cx + 1, rect.top + nOffset + size.cy + 1);

                pDC->SelectObject(&penShadow);
                pDC->MoveTo( rect.right - size.cx, rect.top + nOffset + size.cy );
                pDC->LineTo( rect.right - (size.cx / 2), rect.top + nOffset );
                pDC->LineTo( rect.right, rect.top + nOffset + size.cy );
                pDC->LineTo( rect.right - size.cx, rect.top + nOffset + size.cy );
                pDC->SelectObject(pOldPen);
            }
            else
            {
                // Draw triangle pointing downwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( rect.right - size.cx + 1, rect.top + nOffset + 1 );
                pDC->LineTo( rect.right - (size.cx / 2) + 1, rect.top + nOffset + size.cy + 1 );
                pDC->LineTo( rect.right + 1, rect.top + nOffset + 1 );
                pDC->LineTo( rect.right - size.cx + 1, rect.top + nOffset + 1 );
    
                pDC->SelectObject(&penShadow);
                pDC->MoveTo( rect.right - size.cx, rect.top + nOffset );
                pDC->LineTo( rect.right - (size.cx / 2), rect.top + nOffset + size.cy );
                pDC->LineTo( rect.right, rect.top + nOffset );
                pDC->LineTo( rect.right - size.cx, rect.top + nOffset );
                pDC->SelectObject(pOldPen);
            }

            rect.right -= size.cy;
	        penShadow.DeleteObject();
		    penLight.DeleteObject();
        }
    }

    // We want to see '&' characters so use DT_NOPREFIX
    DrawText(pDC->m_hDC, GetText(), -1, rect, GetFormat() | DT_NOPREFIX);

    pDC->SelectObject(pOldFont);
	pDC->SetTextColor(clrOldTextColor);
	pDC->SetBkMode(nOldMode);

    pDC->RestoreDC(nSavedDC);


    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Mouse and Cursor events

// Not yet implemented
void CGridCellBase::OnMouseEnter(int nRow, int nCol, CPoint pt)
{
    UNUSED_ALWAYS(pt);
//    TRACE("Mouse entered cell(%d, %d) at point(%d, %d)\n", nRow, nCol, pt.x, pt.y);
    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

	if (!pGrid || !bDrawFixed)
		return;

	DWORD dwState = GetState();
	dwState |= GVIS_TRACKED;
	SetState(dwState);
//	TRACE ("OnMouseEnter : cell (%d,%d) GVIS_TRACKED == %d\n", nRow, nCol, (GetState() & GVIS_TRACKED) == GVIS_TRACKED);

	pGrid->InvalidateCellRect(nRow, nCol);
	pGrid->UpdateWindow();
}

void CGridCellBase::OnMouseOver(int nRow, int nCol, CPoint pt)
{
    UNUSED_ALWAYS(nRow);
    UNUSED_ALWAYS(nCol);
    UNUSED_ALWAYS(pt);
    //TRACE0("Mouse over cell(%d, %d) at point(%d, %d)\n", nRow, nCol, pt.x, pt.y);
}

// Not Yet Implemented
void CGridCellBase::OnMouseLeave(int nRow, int nCol, CPoint pt)
{
//    TRACE("Mouse leaved cell(%d, %d) at point(%d, %d)\n", nRow, nCol, pt.x, pt.y);
    UNUSED_ALWAYS(pt);
    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

	if (!pGrid || !bDrawFixed)
		return;

	DWORD dwState = GetState();
	dwState &= ~GVIS_TRACKED;
	SetState(dwState);

//	TRACE ("OnMouseLeave: cell (%d,%d) GVIS_TRACKED == %d\n", nRow, nCol, (GetState() & GVIS_TRACKED) == GVIS_TRACKED);

	pGrid->InvalidateCellRect(nRow, nCol);
	pGrid->UpdateWindow();
}

void CGridCellBase::OnClick(int nRow, int nCol, CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    UNUSED_ALWAYS(nRow);
    UNUSED_ALWAYS(nCol);
    TRACE2("Mouse Left btn up in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnClickDown(int nRow, int nCol, CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    UNUSED_ALWAYS(nRow);
    UNUSED_ALWAYS(nCol);
    TRACE2("Mouse Left btn down in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnRClick(int nRow, int nCol, CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    UNUSED_ALWAYS(nRow);
    UNUSED_ALWAYS(nCol);
    TRACE2("Mouse right-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnDblClick(int nRow, int nCol, CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    UNUSED_ALWAYS(nRow);
    UNUSED_ALWAYS(nCol);
    TRACE2("Mouse double-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

// Return TRUE if you set the cursor
BOOL CGridCellBase::OnSetCursor()
{
#ifndef _WIN32_WCE_NO_CURSOR
//    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
//    return TRUE;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Sizing

BOOL CGridCellBase::GetTextRect( LPRECT pRect)  // i/o:  i=dims of cell rect; o=dims of text rect
{
    if (GetImage() >= 0)
    {
        IMAGEINFO Info;

        CGridCtrl* pGrid = GetGrid();
        CImageList* pImageList = pGrid->GetImageList();
        
        if (pImageList->GetImageInfo( GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            pRect->left += nImageWidth + GetMargin();
        }
    }

    return TRUE;
}

// By default this uses the selected font (which is a bigger font)
CSize CGridCellBase::GetTextExtent(LPCTSTR str)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    CDC* pDC = pGrid->GetDC();
    if (!pDC) 
		return CSize(0,0);

    CFont *pOldFont = 0;

    if (GetState() & GVIS_FIXED)
        pOldFont = (CFont*) pDC->SelectObject(pGrid->GetBoldFont());
    else
        pOldFont = (CFont*) pDC->SelectObject(pGrid->GetFont());
    CSize size;

    // EFW - If null, use current cell text
    if (!str)
        size = pDC->GetTextExtent(GetText());
    else
        size = pDC->GetTextExtent(str);

    pDC->SelectObject(pOldFont);
    pGrid->ReleaseDC(pDC);

    return size + CSize(2*GetMargin(), 2*GetMargin());
}

CSize CGridCellBase::GetCellExtent(CDC* pDC)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    CFont* pfont = (GetState() & GVIS_FIXED) ? pGrid->GetBoldFont() : pGrid->GetFont();

    CSize size;
    int nFormat = GetFormat();

    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    CFont* pOldFont = pDC->SelectObject(pfont);
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = GetText();
		if (str.IsEmpty())
			str = _T("AA");
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth, 0);
        pDC->DrawText(GetText(), rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
	{
		CString str = GetText();
		if (str.IsEmpty())
			str = _T("AA");

        size = pDC->GetTextExtent(GetText());
	}
    pDC->SelectObject(pOldFont);
    
    size += CSize(4*GetMargin(), 2*GetMargin());
    
    CSize ImageSize(0,0);
    if (pGrid->GetImageList()) 
    {
        int nImage = GetImage();
        if (nImage >= 0) 
        {
            IMAGEINFO Info;
            if (pGrid->GetImageList()->GetImageInfo(nImage, &Info))
                ImageSize = CSize(Info.rcImage.right-Info.rcImage.left+1, 
                                  Info.rcImage.bottom-Info.rcImage.top+1);
        }
    }
    
    return CSize(size.cx + ImageSize.cx, max(size.cy, ImageSize.cy));
}

// EFW - Added to print cells so that grids that use different colors are
// printed correctly.
BOOL CGridCellBase::PrintCell(CDC* pDC, int /*nRow*/, int /*nCol*/, CRect rect)
{
#if defined(_WIN32_WCE_NO_PRINTING) || defined(GRIDCONTROL_NO_PRINTING)
    return FALSE;
#else
    COLORREF crFG, crBG;
    GV_ITEM Item;

    CGridCtrl* pGrid = GetGrid();
    if (!pGrid || !pDC)
        return FALSE;

    if (rect.Width() <= 0
        || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();

    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);

    pDC->SetBkMode(TRANSPARENT);

    if (pGrid->GetShadedPrintOut())
    {
        // Use custom color if it doesn't match the default color and the
        // default grid background color.  If not, leave it alone.
        if (bDrawFixed)
            crBG = (GetBackClr() != CLR_DEFAULT) ? GetBackClr() : pGrid->GetFixedBkColor();
        else
            crBG = (GetBackClr() != CLR_DEFAULT && GetBackClr() != pGrid->GetTextBkColor()) ?
                GetBackClr() : CLR_DEFAULT;

        // Use custom color if the background is different or if it doesn't
        // match the default color and the default grid text color.  If not,
        // use black to guarantee the text is visible.
        if (bDrawFixed)
            crFG = (GetBackClr() != CLR_DEFAULT) ? GetTextClr() : pGrid->GetFixedTextColor();
        else
            crFG = (GetBackClr() != CLR_DEFAULT ||
                (GetTextClr() != CLR_DEFAULT && GetTextClr() != pGrid->GetTextColor())) ?
                    GetTextClr() : RGB(0, 0, 0);

        // If not printing on a color printer, adjust the foreground color
        // to a gray scale if the background color isn't used so that all
        // colors will be visible.  If not, some colors turn to solid black
        // or white when printed and may not show up.  This may be caused by
        // coarse dithering by the printer driver too (see image note below).
        if (pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT)
            crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59,
                GetBValue(crFG) * 0.11);

        // Only erase the background if the color is not the default
        // grid background color.
        if (crBG != CLR_DEFAULT)
        {
            CBrush brush(crBG);
            rect.right++; rect.bottom++;
            pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;
        }
    }
    else
    {
        crBG = CLR_DEFAULT;
        crFG = RGB(0, 0, 0);
    }

    pDC->SetTextColor(crFG);

    // Create the appropriate font and select into DC.
    CFont Font;

    // Bold the fixed cells if not shading the print out.  Use italic
    // font it it is enabled.
    const LOGFONT* plfFont = GetFont();
    if (bDrawFixed && !pGrid->GetShadedPrintOut())
    {
        Font.CreateFont(plfFont->lfHeight, 0, 0, 0, FW_BOLD, plfFont->lfItalic, 0, 0,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
#ifndef _WIN32_WCE
            PROOF_QUALITY,
#else
            DEFAULT_QUALITY,
#endif
            VARIABLE_PITCH | FF_SWISS, plfFont->lfFaceName);
    }
    else
        Font.CreateFontIndirect(plfFont);

    pDC->SelectObject(&Font);

    // Draw lines only when wanted on fixed cells.  Normal cell grid lines
    // are handled in OnPrint.
    if (pGrid->GetGridLines() != GVL_NONE && bDrawFixed)
    {
        CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
             darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
            *pOldPen = pDC->GetCurrentPen();

        pDC->SelectObject(&lightpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.left, rect.top);
        pDC->LineTo(rect.left, rect.bottom);

        pDC->SelectObject(&darkpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.right, rect.bottom);
        pDC->LineTo(rect.left, rect.bottom);

        rect.DeflateRect(1,1);
        pDC->SelectObject(pOldPen);
    }

    rect.DeflateRect(GetMargin(), 0);

    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        // NOTE: If your printed images look like fuzzy garbage, check the
        //       settings on your printer driver.  If it's using coarse
        //       dithering and/or vector graphics, they may print wrong.
        //       Changing to fine dithering and raster graphics makes them
        //       print properly.  My HP 4L had that problem.

        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw without clipping so as not to lose text when printed for real
    DrawText(pDC->m_hDC, GetText(), -1, rect,
        GetFormat() | DT_NOCLIP | DT_NOPREFIX);

    pDC->RestoreDC(nSavedDC);
    Font.DeleteObject();

    return TRUE;
#endif
}

/*****************************************************************************
Callable by derived classes, only
*****************************************************************************/
LRESULT CGridCellBase::SendMessageToParent(int nRow, int nCol, int nMessage)
{
    CGridCtrl* pGrid = GetGrid();
    if (pGrid)
        return pGrid->SendMessageToParent(nRow, nCol, nMessage);
    else
        return 0;
}