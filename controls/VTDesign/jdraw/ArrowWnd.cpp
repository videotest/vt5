// ArrowWnd.cpp : implementation file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
#include "stdafx.h"
#include <math.h>
#include "ArrowPicker.h"
#include "ArrowWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ARROW    -1

#define MAX_ARROWS      6

#include "..\\resource.h"

ArrowTableEntry CArrowWnd::m_crArrows[] = 
{
    { 0,    _T("No Arrow")          },
    { 1,    _T("Arrow")             },
    { 2,    _T("Open Arrow")		},

    { 3,    _T("Stealth Arrow")     },
    { 4,    _T("Diamond Arrow")     },
    { 5,    _T("Oval Arrow")        }
    
};

/////////////////////////////////////////////////////////////////////////////
// CArrowWnd

CArrowWnd::CArrowWnd()
{
    Initialise();
}

CArrowWnd::CArrowWnd(CPoint p, int crArrow, CWnd* pParentWnd)
{
    Initialise();

    m_crArrow       = m_crInitialArrow = crArrow;
    m_pParent        = pParentWnd;
	m_image.Create( IDB_ARROWS, 22, 0, RGB(255,0,0) );

    CArrowWnd::Create(p, crArrow, pParentWnd);
}

void CArrowWnd::Initialise()
{
    m_nNumArrows       = sizeof(m_crArrows)/sizeof(ArrowTableEntry);
    ASSERT(m_nNumArrows <= MAX_ARROWS);
    if (m_nNumArrows > MAX_ARROWS)
        m_nNumArrows = MAX_ARROWS;

    m_nNumColumns       = 0;
    m_nNumRows          = 0;
    m_nBoxSize          = 30;
    m_nMargin           = 3*::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_ARROW;
    m_nChosenArrowSel  = INVALID_ARROW;
    m_pParent           = NULL;
    m_crArrow          = m_crInitialArrow = 1; //Standard Arrow

    // Idiot check: Make sure the square is at least 24 x 24;
    //if (m_nBoxSize - 2*m_nMargin - 2 < 24) m_nBoxSize = 24 + 2*m_nMargin + 2;

    // Create the font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    m_Font.CreateFontIndirect(&(ncm.lfMessageFont));
}

CArrowWnd::~CArrowWnd()
{
    m_Font.DeleteObject();
}

BOOL CArrowWnd::Create(CPoint p, int crArrow, CWnd* pParentWnd)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CArrowPicker)));
    m_pParent  = pParentWnd;
    m_crArrow = m_crInitialArrow = crArrow;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
                                              0,
                                              (HBRUSH) (COLOR_BTNFACE+1), 
                                              0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE|WS_POPUP, 
                        p.x, p.y, 100, 100, // size updated soon
                        pParentWnd->GetSafeHwnd(), 0, NULL))
        return FALSE;
        
    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Find which cell (if any) corresponds to the initial colour
    FindCellFromArrow(crArrow);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CArrowWnd, CWnd)
    //{{AFX_MSG_MAP(CArrowWnd)
    ON_WM_NCDESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowWnd message handlers

// For tooltips
BOOL CArrowWnd::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

// If an arrow key is pressed, then move the selection
void CArrowWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int row = GetRow(m_nCurrentSel),
        col = GetColumn(m_nCurrentSel);

    if (nChar == VK_DOWN) 
    {
        row++;
        if (GetIndex(row,col) < 0)
            row = col = 0;
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_UP) 
    {
        if (row > 0) row--;
        else /* row == 0 */
        {
            row = GetRow(m_nNumArrows-1); 
			col = GetColumn(m_nNumArrows-1); 
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_RIGHT) 
    {
		if (col < m_nNumColumns-1) 
            col++;
        else 
        { 
            col = 0; row++;
        }

        if (GetIndex(row,col) == INVALID_ARROW)
			row = col = 0;
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_LEFT) 
    {
		if (col > 0) col--;
        else /* col == 0 */
        {
            if (row > 0) 
			{	row--;
				col = m_nNumColumns-1;
			}
            else 
            {
	            row = GetRow(m_nNumArrows-1); 
                col = GetColumn(m_nNumArrows-1); 
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_ESCAPE) 
    {
        m_crArrow = m_crInitialArrow;
        EndSelection(APN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(APN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// auto-deletion
void CArrowWnd::OnNcDestroy() 
{
    CWnd::OnNcDestroy();
    delete this;
}

void CArrowWnd::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    // Draw arrow cells
    for (int i = 0; i < m_nNumArrows; i++)
        DrawCell(&dc, i);
    
    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

void CArrowWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_ARROW;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);
    // OK - we have the row and column of the current selection
    // Has the row/col selection changed? If yes, then redraw old and new cells.
    if (nNewSelection != m_nCurrentSel)
        ChangeSelection(nNewSelection);

    CWnd::OnMouseMove(nFlags, point);
}

// End selection on LButtonUp
void CArrowWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{    
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(LOWORD(pos), HIWORD(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(APN_SELENDOK);
    else
        EndSelection(APN_SELENDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CArrowWnd implementation

int CArrowWnd::GetIndex(int row, int col) const
{ 
    if (row*m_nNumColumns + col >= m_nNumArrows)
        return INVALID_ARROW;
    else
        return row*m_nNumColumns + col;
}

int CArrowWnd::GetRow(int nIndex) const               
{ 
    return nIndex / m_nNumColumns; 
}

int CArrowWnd::GetColumn(int nIndex) const            
{ 
    return nIndex % m_nNumColumns; 
}

void CArrowWnd::FindCellFromArrow(int crArrow)
{
    if (crArrow<=m_nNumArrows && crArrow>0)
		m_nChosenArrowSel = crArrow;
    else
        m_nChosenArrowSel = INVALID_ARROW;
}

// Gets the dimensions of the colour cell given by (row,col)
BOOL CArrowWnd::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex < 0 || nIndex >= m_nNumArrows)
        return FALSE;

    rect->left = GetColumn(nIndex) * m_nBoxSize + m_nMargin;
    rect->top  = GetRow(nIndex) * m_nBoxSize + m_nMargin;
    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;
return TRUE;
}

// Works out an appropriate size and position of this window
void CArrowWnd::SetWindowSize()
{
    CSize TextSize;
   // Get the number of columns and rows
    m_nNumColumns = 3;
    m_nNumRows = m_nNumArrows / m_nNumColumns;
    if (m_nNumArrows % m_nNumColumns) m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_nNumColumns*m_nBoxSize + 2*m_nMargin,
                         rect.top  + m_nNumRows*m_nBoxSize + 2*m_nMargin);

 
    // Need to check it'll fit on screen: Too far right?
    CSize ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    if (m_WindowRect.right > ScreenSize.cx)
        m_WindowRect.OffsetRect(-(m_WindowRect.right - ScreenSize.cx), 0);

    // Too far left?
    if (m_WindowRect.left < 0)
        m_WindowRect.OffsetRect( -m_WindowRect.left, 0);

    // Bottom falling out of screen?
    if (m_WindowRect.bottom > ScreenSize.cy)
    {
        CRect ParentRect;
        m_pParent->GetWindowRect(ParentRect);
        m_WindowRect.OffsetRect(0, -(ParentRect.Height() + m_WindowRect.Height()));
    }

    // Set the window size and position
    MoveWindow(m_WindowRect, TRUE);
}

void CArrowWnd::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(this)) return;

    // Add a tool for each cell
    for (int i = 0; i < m_nNumArrows; i++)
    {
        CRect rect;
        if (!GetCellRect(i, rect)) continue;
            m_ToolTip.AddTool(this, GetArrowName(i), rect, 1);
    }
}

void CArrowWnd::ChangeSelection(int nIndex)
{
    CClientDC dc(this);        // device context for drawing


    if (m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumArrows)
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_ARROW;
        DrawCell(&dc, OldSel);
    }

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
    DrawCell(&dc, m_nCurrentSel);

    // Store the current colour
    m_crArrow = GetArrow(m_nCurrentSel);
    m_pParent->SendMessage(APN_SELCHANGEBT, (WPARAM) m_crArrow, 0);
}

void CArrowWnd::EndSelection(int nMessage)
{
    ReleaseCapture();

    // If custom text selected, perform a custom colour selection
    if (nMessage == APN_SELENDCANCEL)
        m_crArrow = m_crInitialArrow;

    m_pParent->SendMessage(nMessage, (WPARAM) m_crArrow, 0);
    
    DestroyWindow();
}

void CArrowWnd::DrawCell(CDC* pDC, int nIndex)
{
    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // fill background
    if (m_nChosenArrowSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenArrowSel == nIndex)
        pDC->DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);

    CPen   pen;
    pen.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_BTNTEXT));

    CPen*   pOldPen   = (CPen*)   pDC->SelectObject(&pen);

    // Draw the cell colour
    rect.DeflateRect(4, 4);
    
	POINT pt;
	pt.x=rect.left;
	pt.y=rect.top;
    m_image.Draw( pDC, nIndex, pt, ILD_TRANSPARENT );

    // restore DC and cleanup
    pDC->SelectObject(pOldPen);
    pen.DeleteObject();

}


void CArrowWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

    ReleaseCapture();
    //DestroyWindow(); - causes crash when Custom colour dialog appears.
}

// KillFocus problem fix suggested by Paul Wilkerson.
#if _MSC_VER >= 1300
	void CArrowWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#else
	void CArrowWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);

	// If Deactivating App, cancel this selection
	if (!bActive)
		 EndSelection(APN_SELENDCANCEL);
}
