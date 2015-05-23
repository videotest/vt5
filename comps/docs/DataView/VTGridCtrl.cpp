// VTGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "dataview.h"
#include "VTGridCtrl.h"
#include "GridCell.h"
#include "GridView.h"
#include "DataViewDef.h"
#include "TimeTest.h"
#include "Propbag.h"

// OLE stuff for clipboard operations
#include <afxadv.h>            // For CSharedFile
#include <afxconv.h>           // For LPTSTR -> LPSTR macros


#define TIMER_EVENT 100

/////////////////////////////////////////////////////////////////////////////
// CVTGridCell

IMPLEMENT_DYNCREATE(CVTGridCell, CGridCell);


LPCTSTR  CVTGridCell::GetText()
{
	return CGridCell::GetText();
}

BOOL CVTGridCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	CVTGridCtrl* pgrid = GetVTGrid();
	if( pgrid && pgrid->GetAdditionRow() != nRow )
	{
		if (m_eCellType == egctManualParam || 
			m_eCellType == egctClassName ||
			m_eCellType == egctClassColor)
			return CGridCell::Edit(nRow, nCol, rect, point, nID, nChar);
		if( nRow == 0 && pgrid->m_pGridView->m_cols[nCol]->pparam )
			return CGridCell::Edit(nRow, nCol, rect, point, nID, nChar);
	}

	return false;
}

CVTGridCtrl* CVTGridCell::GetVTGrid()
{
	if (!m_pGrid || !m_pGrid->IsKindOf(RUNTIME_CLASS(CVTGridCtrl)))
		return 0;
	return (CVTGridCtrl*)m_pGrid;
}

/*
BOOL CVTGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
    if (!m_pGrid || !pDC)
        return FALSE;

	bool bFixed = (GetState() & GVIS_FIXED) == GVIS_FIXED;
	DWORD dwFormat = GetFormat();
	if (bFixed || GetType() == egctMeasUnit)
	{
		if ((dwFormat & DT_CENTER) != DT_CENTER)
		{
			dwFormat &= ~(DT_LEFT|DT_RIGHT);
			dwFormat |= DT_CENTER;
		}
	}
	else
	{
		if (GetType() == egctText && (dwFormat & DT_RIGHT) != DT_RIGHT)
		{
			dwFormat &= ~(DT_LEFT|DT_CENTER);
			dwFormat |= DT_RIGHT;
		}
	}

	if (GetType() == egctNumber)
		m_strText.Format("%d", nRow - 1);
	else if (GetType() == egctResult)
		m_strText = _T("*");

	if (nCol == m_pGrid->GetFixedColumnCount())//GetType() == egctImage || GetType() == egctMeasUnit)
	{
		SetTextClr(m_pGrid->GetFixedTextColor());
		SetBackClr(m_pGrid->GetFixedBkColor());
	}
	
	BOOL bRet = CGridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);

	return bRet;
}
*/
BOOL CVTGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	m_row = nRow;

    CVTGridCtrl* pGrid = GetVTGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

	
	if( pGrid && m_row >= 0 )pGrid->GetDispInfoRow( m_row );


    BOOL bDrawFixed = ((GetState() & GVIS_FIXED) == GVIS_FIXED);

	CBrush * pLightBrush = pGrid->GetLightBrush();

    //TRACE3("Drawing %scell %d, %d\n", bDrawFixed? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clOldColor = pDC->GetTextColor();

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = (bDrawFixed || GetType() == egctImage)? pGrid->GetFixedTextColor() : pGrid->GetTextColor();
    else
        TextClr = GetTextClr();

	CVTGridCtrl * pVTGrid = GetVTGrid();
	if (!bDrawFixed && pVTGrid)
		SetBackClr(pVTGrid->GetCellBackColor(nRow, nCol));

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = (bDrawFixed || GetType() == egctImage)? pGrid->GetFixedBkColor() : pGrid->GetTextBkColor();
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
        CBrush brush(TextClr);
        pDC->FrameRect(rect, &brush);
        pDC->SetTextColor(TextClr);
		brush.DeleteObject();

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
        bool bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol) &&
			!pGrid->m_bPrinting;
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
            rect.right--; rect.bottom--;
			brush.DeleteObject();
        }
        pDC->SetTextColor(TextClr);

    }

    // Setup font and if fixed, draw fixed cell highlighting
	bool bSmall = rect.Width() <= (2 * pGrid->GetResizeCaptureRange() + 1);

	BOOL bHiliteFixed = false;
	bool bTracked = (GetState() & GVIS_TRACKED) == GVIS_TRACKED;
    // Draw lines only when wanted
    if (bDrawFixed && pGrid->GetGridLines() != GVL_NONE)
    {
        CCellID FocusCell = pGrid->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        bHiliteFixed = pGrid->IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol) &&
			!pGrid->m_bPrinting;

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

	CFont	font;
	CFont * pFont = (bHiliteFixed) ? pGrid->GetBoldFont() : pGrid->GetFont();

	if( m_lfFont.lfHeight != 0 )
	{
		font.CreateFontIndirect( &m_lfFont );
		pFont = &font;
	}
    
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
        }
    }

	if (GetType() == egctNumber)
		m_strText.Format("%d", nRow - 1);
	else if (GetType() == egctResult)
		m_strText = _T("*");

   // We want to see '&' characters so use DT_NOPREFIX
	if (nCol >= pGrid->GetFixedColumnCount() && nRow >= pGrid->GetFixedRowCount() && 
		nRow != pGrid->GetAdditionRow() && (GetType() == egctManualParam || GetType() == egctRegularParam))
	{
		CString str(GetText());
		int nPos = str.Find(_T('.'));
		CRect OutRect = rect;

		if (pGrid->IsItemEmpty(nRow, nCol))
		{
			OutRect = rect;//do nothing
		}
		else if (nPos == -1) // hasn't "."  == integer
		{
			DWORD dwFormat = GetFormat() & ~(DT_END_ELLIPSIS|DT_PATH_ELLIPSIS|DT_MODIFYSTRING);
			// get needed rect
			CRect FullRect(0, 0, 0, 0), PartRect(0, 0, 0, 0);
			pDC->DrawText(GetText(), -1, FullRect, dwFormat|DT_CALCRECT);
			// get part of rect
			pDC->DrawText(_T("."), -1, PartRect, dwFormat|DT_CALCRECT);

			OutRect.right = rect.left + rect.Width() / 2 + PartRect.Width() + 1;
			OutRect.left = OutRect.right - FullRect.Width() - 1;

			if (OutRect.left < rect.left)
				OutRect.left = rect.left;

			if (OutRect.right > rect.right)
				OutRect.right = rect.right;
		}
		else
		{
			DWORD dwFormat = GetFormat();
			dwFormat &= ~(DT_END_ELLIPSIS|DT_PATH_ELLIPSIS|DT_MODIFYSTRING);
			// get needed rect
			CRect FullRect(0, 0, 0, 0), PartRect(0, 0, 0, 0);
			pDC->DrawText(GetText(), -1, FullRect, dwFormat|DT_CALCRECT);
			// get part of rect
			CString strPart = str.Left(nPos);
			pDC->DrawText(strPart, -1, PartRect, dwFormat|DT_CALCRECT);

			OutRect.left = rect.left + rect.Width() / 2 - PartRect.Width() - 1;
			OutRect.right = OutRect.left + FullRect.Width() + 1;

			if (OutRect.left < rect.left)
				OutRect.left = rect.left;

			if (OutRect.right > rect.right)
				OutRect.right = rect.right;
		}
		rect = OutRect;

	}

	DrawText(pDC->m_hDC, GetText(), -1, rect, GetFormat());
    pDC->SelectObject(pOldFont);
	pDC->SetTextColor(clOldColor);
    pDC->SetBkMode(nOldBkMode);

    pDC->RestoreDC(nSavedDC);

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVTGridCtrl

IMPLEMENT_DYNCREATE(CVTGridCtrl, CGridCtrl);

CVTGridCtrl::CVTGridCtrl()
{
	m_pGridView = 0;
	m_RClickedCell = CCellID(-1, -1);
	m_bEnableWidthNotify = false;
	m_bEnableHeightNotify = false; // vanek
//	m_arrCellType.RemoveAll();
}

CVTGridCtrl::~CVTGridCtrl()
{
//	m_arrCellType.RemoveAll();
}


BEGIN_MESSAGE_MAP(CVTGridCtrl, CGridCtrl)
	//{{AFX_MSG_MAP(CVTGridCtrl)
	ON_WM_CREATE()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVTGridCtrl message handlers



int CVTGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CWnd * pWnd = GetParent();
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)) != 0);

	m_pGridView = (CGridViewBase*) (pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)) ? pWnd : 0);

	return 0;
}

void CVTGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CGridCtrl::OnRButtonDown(nFlags, point);

    CCellID FocusCell = GetCellFromPt(point);
	m_RClickedCell = FocusCell;

    EndEditing();        // Auto-destroy any InPlaceEdit's

    // If not a valid cell, pass -1 for row and column
    SetFocusCell(-1,-1);
    if (IsValid(FocusCell))
    {
//        SetFocusCell(-1,-1);
		CCellID cellNew = FocusCell;
		if (m_bListMode)
		{
			m_MouseMode = MOUSE_SELECT_ROW;
			cellNew = GetFirstVisibleCell(FocusCell.row, FocusCell.col);
		}
		else
		{
			cellNew = GetFirstVisibleCell(FocusCell.row, FocusCell.col);
		}
		m_SelectionStartCell = cellNew;
		// A.M. fix. BT4686, SBT1444
        if (cellNew.row < GetFixedRowCount())
            OnFixedRowClick(cellNew);
        else if (cellNew.col < GetFixedColumnCount())
            OnFixedColumnClick(cellNew);
        else
        {
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            OnSelecting(cellNew);
        }
	    m_MouseMode = MOUSE_NOTHING;
		SetFocusCell(cellNew);
        SendMessageToParent(cellNew.row, cellNew.col, GVN_ACTIVATE);
    }
}

// Creates a new grid cell and performs any necessary initialisation
CGridCellBase* CVTGridCtrl::CreateCell(int nRow, int nCol)
{
//    ASSERT(IsValid(nRow, nCol));
//    if (!IsValid(nRow, nCol))
//        return NULL;

	CGridCellBase* pCell =  0;
	if (m_pGridView)
		pCell = m_pGridView->CreateCell(nRow, nCol);
	else
		pCell = new CVTGridCell;

    if (!pCell)
        return NULL;

    // Make format same as cell above
/*	if (!(nRow >= GetFixedRowCount() && 
		  pCell->GetType() != egctMeasUnit && nRow != GetAdditionRow()) && 
		  (nRow < GetFixedRowCount() && nCol >= 0 && nCol < GetFixedColumnCount()))
        pCell->SetFormat(GetItemFormat(nRow - 1, nCol));
*/
	if (nRow > 0 && nCol >= 0 && nCol < GetColumnCount())
		pCell->SetFormat(GetItemFormat(nRow - 1, nCol));

    pCell->SetFont(&m_Logfont);    // Make font default grid font
    pCell->SetGrid(this);

    if (nCol < m_nFixedCols || nRow < m_nFixedRows)
        pCell->SetState(pCell->GetState() | GVIS_FIXED);

	if (ExistsAdditionRow() && nRow == GetRowCount() - 1)
		return pCell;

    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
 void CVTGridCtrl::DestroyCell(int nRow, int nCol)
{
    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    SetItemState(nRow, nCol, 0);

    delete GetCell(nRow, nCol);
}

void CVTGridCtrl::ChangeCellType(int nRow, int nCol, EGridCellType type)
{
	if (IsValid(nRow, nCol))
	{
/*		CVTGridCell * pCell = (CVTGridCell*)GetCell(nRow, nCol);
		if (pCell)
		{
			pCell->SetType(type);
			if (type == egctManualParam || type == egctRegularParam)
				pCell->SetText(_T("--"));
		}*/
	}
}


int  CVTGridCtrl::InsertColumn(LPCTSTR strHeading, UINT nFormat, int nColumn)
{
	bool	bOldEnable = m_bEnableWidthNotify;
	m_bEnableWidthNotify = false;

	bool bOldEnableHeightNotify = m_bEnableHeightNotify; m_bEnableHeightNotify = false; // vanek

	int nIndex = CGridCtrl::InsertColumn(strHeading, nFormat, nColumn);
	m_bEnableWidthNotify = bOldEnable;
	m_bEnableHeightNotify = bOldEnableHeightNotify; // vanek
	if (m_pGridView)
		m_pGridView->OnInsertColumn(nIndex);
	

	return nIndex;
}

int  CVTGridCtrl::InsertRow(LPCTSTR strHeading, int nRow)
{
	bool	bOldEnable = m_bEnableWidthNotify;
	m_bEnableWidthNotify = false;

	bool bOldEnableHeightNotify = m_bEnableHeightNotify; m_bEnableHeightNotify = false; // vanek

	int nIndex = CGridCtrl::InsertRow(strHeading, nRow);
	if (m_pGridView)
		m_pGridView->OnInsertRow(nIndex);
	m_bEnableWidthNotify = bOldEnable;

	m_bEnableHeightNotify = bOldEnableHeightNotify; // vanek

	return nIndex;
}

BOOL CVTGridCtrl::DeleteColumn(int nColumn)
{
	BOOL bRet = CGridCtrl::DeleteColumn(nColumn);
	if (m_pGridView && bRet)
		m_pGridView->OnDeleteColumn(nColumn);

	return bRet;
}

BOOL CVTGridCtrl::DeleteRow(int nRow)
{
	BOOL bRet = CGridCtrl::DeleteRow(nRow);
	if (m_pGridView)
		m_pGridView->OnDeleteRow(nRow);

	return bRet;
}

BOOL CVTGridCtrl::DeleteNonFixedRows()
{
	return CGridCtrl::DeleteNonFixedRows();
}

BOOL CVTGridCtrl::DeleteAllItems()
{
	if( !CGridCtrl::DeleteAllItems() )
		return false;
	if (m_pGridView)
	{
		m_pGridView->OnSetColumnCount( 0 );
		m_pGridView->OnSetRowCount( 0 );
	}

	return true;
	
}

BOOL CVTGridCtrl::SetRowHeight(int row, int height)
{
	if( height == -1 )	// need set default height
		height = m_nDefCellHeight;

	BOOL bRet = CGridCtrl::SetRowHeight(row, height);


	// [vanek] 11.12.2003: формирование "пустого" undo при ручном изменении высоты строки - 
	// к моменту вызова акции было уже становлено новое значение высоты 
	//if (m_pGridView && m_bEnableHeightNotify )
	//	m_pGridView->OnSetRowHeight(row, height);
    if( m_bEnableHeightNotify )
	{	// установка через акцию
		if( m_pGridView )
			m_pGridView->OnSetRowHeight(row, height);
	}
	else
	{	// установка напрямую
		IGridViewPtr ptrView(m_pGridView->GetControllingUnknown());
		IUnknownPtr ptrObject;
		if(ptrView!=0) ptrView->GetObjectByRow(row, &ptrObject);
		INamedPropBagPtr ptrBag(ptrObject);
		if(ptrBag!=0)
		{
			_variant_t var = long(height);
			ptrBag->SetProperty( _bstr_t( ROW_HEIGHT ), var );
		}
	}
	//

	return bRet;
}

BOOL CVTGridCtrl::SetColumnWidth(int col, int width)
{
	BOOL bRet = CGridCtrl::SetColumnWidth(col, width);

	if (m_pGridView && m_bEnableWidthNotify )
		m_pGridView->OnSetColumnWidth(col, width);

	return bRet;
}

BOOL CVTGridCtrl::SetColumnCount(int nCols)
{
	bool	bOldEnable = m_bEnableWidthNotify;
	m_bEnableWidthNotify = false;

	bool bOldEnableHeightNotify = m_bEnableHeightNotify; m_bEnableHeightNotify = false; // vanek

	BOOL bRet = CGridCtrl::SetColumnCount(nCols);
	m_bEnableWidthNotify = bOldEnable;

	m_bEnableHeightNotify = bOldEnableHeightNotify; // vanek
	
	if (m_pGridView)
		m_pGridView->OnSetColumnCount(nCols);
	
	return bRet;
}

BOOL CVTGridCtrl::SetRowCount(int nCols)
{
	bool	bOldEnable = m_bEnableWidthNotify;
	m_bEnableWidthNotify = false;

	bool bOldEnableHeightNotify = m_bEnableHeightNotify; m_bEnableHeightNotify = false; // vanek

	BOOL bRet = CGridCtrl::SetRowCount(nCols);
	m_bEnableWidthNotify = bOldEnable;
	
	m_bEnableHeightNotify = bOldEnableHeightNotify; // vanek

	if (m_pGridView)
		m_pGridView->OnSetRowCount(nCols);
	
	return bRet;
}


BOOL CVTGridCtrl::SetFixedColumnCount(int nFixedCols)
{
	BOOL bRet = CGridCtrl::SetFixedColumnCount(nFixedCols);

	if (m_pGridView)
		m_pGridView->OnSetFixedColumnCount(nFixedCols);
	
	return bRet;
}

void CVTGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CGridCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	if (m_pGridView)
		m_pGridView->OnHorizontalScroll(nSBCode, nPos, pScrollBar);

}

void CVTGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CGridCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	if (m_pGridView)
		m_pGridView->OnVertivalScroll(nSBCode, nPos, pScrollBar);
}

bool CVTGridCtrl::EnableColumnDrag(int nCol)
{
	if (!CGridCtrl::EnableColumnDrag(nCol))
		return false;

	if (m_pGridView)
		return m_pGridView->EnableDragColumn(nCol);
	
	return true;
}


void CVTGridCtrl::OnEndColumnDrag(int nCol, int nColAfter)
{
//	CGridCtrl::OnEndColumnDrag(nCol, nColAfter);
	if (m_pGridView)
		m_pGridView->OnEndColumnDrag(nCol, nColAfter);
}

void CVTGridCtrl::OnBeginColumnDrag()
{
	CGridCtrl::OnBeginColumnDrag();

	if (m_pGridView)
		m_pGridView->OnBeginDragColumn(m_LeftClickDownCell.col);
}


BOOL CVTGridCtrl::AutoSizeRow(int nRow)
{
	if (!CGridCtrl::AutoSizeRow(nRow))
		return false;

	if (m_pGridView)
		m_pGridView->SyncResBar();

	return true;
}

BOOL CVTGridCtrl::AutoSizeColumn(int nCol, BOOL bIgnoreHeader)
{
	if (!CGridCtrl::AutoSizeColumn(nCol, bIgnoreHeader))
		return false;

	if (m_pGridView)
		m_pGridView->SyncResBar();

	if (m_pGridView && m_bEnableWidthNotify )
		m_pGridView->OnSetColumnWidth(nCol, GetColumnWidth( nCol ));
	return true;
}

void CVTGridCtrl::AutoSizeRows()
{
	CGridCtrl::AutoSizeRows();
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

void CVTGridCtrl::AutoSizeColumns()
{
	CGridCtrl::AutoSizeColumns();
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

void CVTGridCtrl::AutoSize(BOOL bIgnoreColumnHeader)
{
	CGridCtrl::AutoSize(bIgnoreColumnHeader);
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

void CVTGridCtrl::ExpandColumnsToFit()
{
	CGridCtrl::ExpandColumnsToFit();
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

void CVTGridCtrl::ExpandRowsToFit()
{
	CGridCtrl::ExpandRowsToFit();
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

void CVTGridCtrl::ExpandToFit()
{
	CGridCtrl::ExpandToFit();
	if (m_pGridView)
		m_pGridView->SyncResBar();
}

COLORREF CVTGridCtrl::GetCellBackColor(int nRow, int nCol)
{
	if (m_pGridView)
		return m_pGridView->GetCellBackColor(nRow, nCol);
	return CLR_DEFAULT;
}
/*
void CVTGridCtrl::OnPrintDraw(CDC* pDC, CRect & RectDraw)
{
    // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.
    CRect  rect;
    CBrush FixedBack(GetFixedBkColor()),
           TextBack(GetTextBkColor());//,
//           Back(GetBkColor());

	CRgn ClipRegion;
//	if (ClipRegion.CreateRectRgnIndirect(RectDraw))
//		pDC->SelectClipRgn(&ClipRegion, RGN_XOR);

//	ClipRegion.DeleteObject();

    CCellRange VisCellRange = GetCellRangeFromRect(RectDraw);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();

    int minVisibleRow = VisCellRange.GetMinRow(),
        minVisibleCol = VisCellRange.GetMinCol();

	CCellID cellFirst(minVisibleRow, minVisibleCol);
	CRect cellRect;
	int nXstart = 0;
	int nYstart = 0;
	if (GetCellRect(cellFirst, cellRect))
	{
		nXstart = cellRect.left;
		nYstart = cellRect.top;
	}

    int nFixedColWidth = GetFixedColumnWidth();
    int nFixedRowHeight = GetFixedRowHeight();

    // Draw Fixed columns background
    if (RectDraw.left < nFixedColWidth && RectDraw.top < RectDraw.bottom)
	{
		CRect rc(0, 0, min(RectDraw.right, nFixedColWidth) - RectDraw.left, RectDraw.bottom - RectDraw.top);
        pDC->FillRect(rc, &FixedBack);
	}

    // Draw Fixed rows background
    if (RectDraw.top < nFixedRowHeight && RectDraw.right > nFixedColWidth && RectDraw.left < RectDraw.right)
	{
		CRect rc(max(nFixedColWidth, RectDraw.left) - RectDraw.left, 0, RectDraw.right - RectDraw.left, max(nFixedRowHeight, RectDraw.top) - RectDraw.top);
        pDC->FillRect(rc, &FixedBack);
	}

    // Draw non-fixed cell background
	{
		CRect CellRect(max(nFixedColWidth, RectDraw.left) - RectDraw.left, max(nFixedRowHeight, RectDraw.top) - RectDraw.top, RectDraw.right - RectDraw.left, RectDraw.bottom - RectDraw.top);
		if (CellRect.left < CellRect.right && CellRect.top < CellRect.bottom)
			pDC->FillRect(CellRect, &TextBack);
	}

    int row, col;
    CGridCellBase * pCell = 0;

    // draw rest of non-fixed cells
    rect.bottom = nYstart-1 - RectDraw.top;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > RectDraw.bottom - RectDraw.top)
            break;                // Gone past RectDraw
        if (rect.bottom < RectDraw.top - RectDraw.top)
            continue;             // Reached RectDraw yet?

        rect.right = nXstart-1 - RectDraw.left;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > RectDraw.right - RectDraw.left)
                break;        // gone past RectDraw
            if (rect.right < RectDraw.left - RectDraw.left)
                continue;     // Reached RectDraw yet?

            pCell = GetCell(row, col);
            // TRACE(_T("Cell %d,%d type: %s\n"), row, col, pCell->GetRuntimeClass()->m_lpszClassName);
            if (pCell)
			{
				UINT uState = pCell->GetState();
				if (IsCellSelected(row, col))
					uState |= GVIS_SELECTED;
				else
					uState &= ~GVIS_SELECTED;

				pCell->SetState(uState & ~(GVIS_SELECTED|GVIS_FOCUSED));
                pCell->Draw(pDC, row, col, rect, FALSE);
				pCell->SetState(uState);
			}
        }
    }

    CPen pen, lightPen;
    TRY
    {
        pen.CreatePen(PS_SOLID, 0, m_crGridColour);
		lightPen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
    }
    CATCH (CResourceException, e)
    {
#ifndef _WIN32_WCE
        e->Delete();
#endif
        return;
    }
    END_CATCH
    pDC->SelectObject(&lightPen);

    // draw vertical lines (drawn at ends of cells)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
    {
        int x = max(nFixedColWidth, RectDraw.left) - RectDraw.left;//nFixedColWidth;
//        int x = nFixedColWidth;
		int ystart = max(nFixedRowHeight, RectDraw.top) - RectDraw.top;
		int yend = RectDraw.bottom - RectDraw.top;
		if (ystart < yend)
		{
			for (col = minVisibleCol; col <= maxVisibleCol; col++)
			{
				if (col >= GetFixedColumnCount())
				{
					x += GetColumnWidth(col);
					pDC->MoveTo(x-1, ystart);//RectDraw.top);//
					pDC->LineTo(x-1 , yend);
				}
			}
		}
    }

    // draw horizontal lines (drawn at bottom of each cell)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
    {
        int y = max(nFixedRowHeight, RectDraw.top - 1) - RectDraw.top;//nFixedRowHeight;
		int xstart = max(nFixedColWidth, RectDraw.left) - RectDraw.left;
		int xend = RectDraw.right - RectDraw.left;
		if (xstart < xend)
		{
			for (row = minVisibleRow; row <= maxVisibleRow; row++)
			{
				if (row >= GetFixedRowCount())
				{
					y += GetRowHeight(row);
					pDC->MoveTo(xstart, y-1);
					pDC->LineTo(xend,  y-1);
				}
			}
		}
    }

    pDC->SelectStockObject(NULL_PEN);
	pen.DeleteObject();
	lightPen.DeleteObject();
}


CCellRange CVTGridCtrl::GetCellRangeFromRect(LPRECT pRect)
{
	CRect rect(pRect);
	if (rect.IsRectEmpty())
		return CCellRange();

//    CCellID idTopLeft = GetTopleftNonFixedCell();
    CCellID idTopLeft = GetCellFromPoint(rect.TopLeft());


    // calc bottom
    int bottom = 0;//GetFixedRowHeight();
    for (int i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
            bottom = rect.bottom;
            break;
        }
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int right = 0;//GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}
*/

BOOL CVTGridCtrl::GetDispInfoRow( long row )
{
	if( row == GetAdditionRow() )return true;
	if( m_pGridView )
		return m_pGridView->OnGetDispInfoRow( row );
	return true;
}

CCellID CVTGridCtrl::SetFocusCell(CCellID cell)
{
	CCellID cellOldVisble = GetFirstVisibleCell().col;
	CCellID cellRet = CGridCtrl::SetFocusCell(cell);
	CCellID cellNewVisble = GetFirstVisibleCell().col;

	if (m_pGridView)
		m_pGridView->OnSetFocusCell(cell.row, cell.col, cellOldVisble != cellNewVisble );

	return cellRet;
}

void CVTGridCtrl::SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
					  BOOL bForceRepaint, BOOL bSelectCells)
{
	if( nMaxRow >= GetAdditionRow() )nMaxRow = GetAdditionRow()-1;
	if( nMaxCol >= GetColumnCount() )nMaxCol = GetColumnCount()-1;

	if( nMinRow == nMaxRow && nMinCol == nMaxCol )
	{
		CGridCtrl::SetSelectedRange(-1, -1, -1, -1, bForceRepaint, bSelectCells);
		return;
	}

	if( nMaxRow <= 0 || nMaxCol <= 0 )
		return;

	/*if( nMinRow > nMaxRow-1 ||
		nMinCol > nMaxCol-1 )
		return;*/

	CGridCtrl::SetSelectedRange(nMinRow, nMinCol, nMaxRow, nMaxCol, bForceRepaint, bSelectCells);
	if (m_pGridView)m_pGridView->OnChangeSelection();
}

void CVTGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bEnableWidthNotify = true;
	m_bEnableHeightNotify = true; // vanek
	CGridCtrl::OnLButtonDown(nFlags, point);
}

void CVTGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CGridCtrl::OnLButtonUp(nFlags, point);
	m_bEnableWidthNotify = false;
	m_bEnableHeightNotify = false; // vanek
}

void CVTGridCtrl::EnsureVisible(int nRow, int nCol)
{
	int	nScrollHOld = GetScrollPos32(SB_HORZ);
	CGridCtrl::EnsureVisible(nRow, nCol);
	if( nScrollHOld != GetScrollPos32(SB_HORZ) )
		if (m_pGridView)m_pGridView->SyncResBar();;
}

void CVTGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_bEnableWidthNotify = true;
	m_bEnableHeightNotify = true; // vanek
	CGridCtrl::OnLButtonDblClk(nFlags, point);
}

// Copies text from the selected cells to the clipboard
COleDataSource* CVTGridCtrl::CopyTextFromGrid()
{
    USES_CONVERSION;

    CCellRange Selection = GetSelectedCellRange();
    if (!IsValid(Selection))
        return NULL;

    // Write to shared file (REMEBER: CF_TEXT is ANSI, not UNICODE, so we need to convert)
    CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

    // Get a tab delimited string to copy to cache
    CString str;
    CGridCellBase *pCell;

	// Copy first row with parameter name and unit
	{
		pCell = GetCell(0, 1);
		if (pCell)
			str += pCell->GetText();
		str += _T("\t");
        for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
        {
            // don't copy hidden cells
            if( m_arColWidths[col] <= 0 )
                continue;

            pCell = GetCell(0, col);
            if (pCell)
                str += pCell->GetText();
            pCell = GetCell(1, col);
			if (pCell)
			{
				CString sUnit = pCell->GetText();
				if (!sUnit.IsEmpty())
				{
					str += _T(",");
					str += sUnit;
				}
			}
            if (col != Selection.GetMaxCol()) 
                str += _T("\t");
        }
        str += _T("\r\n");
        
        sf.Write(T2W(str.GetBuffer(1)), str.GetLength()*sizeof(wchar_t));
        str.ReleaseBuffer();
	}

	for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
    {
        // don't copy hidden cells
        if( m_arRowHeights[row] <= 0 )
            continue;

        str.Empty();
        pCell = GetCell(row, 1);
        if (pCell)
            str += pCell->GetText();
        str += _T("\t");
        for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
        {
            // don't copy hidden cells
            if( m_arColWidths[col] <= 0 )
                continue;

            pCell = GetCell(row, col);
            if (pCell && IsCellSelected(row, col))
            {
                // if (!pCell->GetText())
                //    str += _T(" ");
                // else 
                str += pCell->GetText();
            }
            if (col != Selection.GetMaxCol()) 
                str += _T("\t");
        }
        if (row != Selection.GetMaxRow()) 
            str += _T("\r\n");
        
        sf.Write(T2W(str.GetBuffer(1)), str.GetLength()*sizeof(wchar_t));
        str.ReleaseBuffer();
    }
    
    wchar_t c = L'\0';
    sf.Write(&c, sizeof(c));

    DWORD dwLen = (DWORD)sf.GetLength();
    HGLOBAL hMem = sf.Detach();
    if (!hMem)
        return NULL;

    hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    if (!hMem)
        return NULL;

    // Cache data
    COleDataSource* pSource = new COleDataSource();
    pSource->CacheGlobalData(CF_UNICODETEXT, hMem);

    return pSource;
}
