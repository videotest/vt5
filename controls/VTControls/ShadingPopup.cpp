// ShadingPopup.cpp : implementation file
//
//Changed 04/06/2007 Andrey Karmanov for visual shading.
//
//
// Original written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

#include "stdafx.h"
#include <math.h>
#include "ShadingPicker.h"
#include "ShadingPopup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_COLOUR    -1
#define COUNT_SHADING 16

/////////////////////////////////////////////////////////////////////////////
// CShadingPopup

CShadingPopup::CShadingPopup()
{
    Initialise();
}

CShadingPopup::CShadingPopup(CPoint p, COLORREF crColour, CWnd* pParentWnd)
{
    Initialise();

    m_crColour       = m_crInitialColour = crColour;
    m_pParent        = pParentWnd;

    CShadingPopup::Create(p, crColour, pParentWnd);
}

void CShadingPopup::Initialise()
{
    m_nNumColours       =  COUNT_SHADING;

    m_nNumColumns       = 0;
    m_nNumRows          = 0;
    m_nBoxSize          = 36;
    m_nMargin           = ::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_COLOUR;
    m_nChosenColourSel  = INVALID_COLOUR;
    m_pParent           = NULL;
    m_crColour          = m_crInitialColour = ::GetSysColor(COLOR_HIGHLIGHT);

    // Idiot check: Make sure the colour square is at least 5 x 5;
    if (m_nBoxSize - 2*m_nMargin - 2 < 10) m_nBoxSize = 10 + 2*m_nMargin + 2;

	for(int i=0; i< COUNT_SHADING; i++)
		m_hShadingIcon[i] = (HICON)::LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_SHADING1)+i, IMAGE_ICON, 32, 32, 0);
}

CShadingPopup::~CShadingPopup()
{
	for(int i=0; i< COUNT_SHADING; i++)
	{
		if(m_hShadingIcon[i])
			DestroyIcon( m_hShadingIcon[i] );
	}
}

BOOL CShadingPopup::Create(CPoint p, COLORREF crColour, CWnd* pParentWnd)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CShadingPicker)));

    m_pParent  = pParentWnd;
    m_crColour = m_crInitialColour = crColour;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
                                              0,
                                              (HBRUSH) (COLOR_BTNFACE+1), 
                                              0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE|WS_POPUP, 
                        p.x, p.y, 100, 80, // size updated soon
                        pParentWnd->GetSafeHwnd(), 0, NULL))
        return FALSE;

    // Set the window size
    SetWindowSize();

    // Find which cell (if any) corresponds to the initial colour
    m_nChosenColourSel = INVALID_COLOUR;

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CShadingPopup, CWnd)
    //{{AFX_MSG_MAP(CShadingPopup)
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
// CShadingPopup message handlers

// If an arrow key is pressed, then move the selection
void CShadingPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
        if (row > 0) 
			row--;
        else 
        { 
            row = GetRow(m_nNumColours-1); 
            col = GetColumn(m_nNumColours-1); 
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_RIGHT) 
    {
        if (col < m_nNumColumns-1) 
            col++;
        else 
            col = 0; row++;
       
        if (GetIndex(row,col) == INVALID_COLOUR)
            row = col = 0;
      
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_LEFT) 
    {
        if (col > 0) 
			col--;
        else 
        {
            if (row > 0) 
			{ 
				row--; 
				col = m_nNumColumns-1; 
			}
            else 
            {
                row = GetRow(m_nNumColours-1); 
                col = GetColumn(m_nNumColours-1); 
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_ESCAPE) 
    {
        m_crColour = m_crInitialColour;
        EndSelection(CPN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(CPN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// auto-deletion
void CShadingPopup::OnNcDestroy() 
{
    CWnd::OnNcDestroy();
    delete this;
}

void CShadingPopup::OnPaint() 
{
    CPaintDC dc(this); // device context for painting 

    // Draw colour cells
    for (int i = 0; i < m_nNumColours; i++)
        DrawCell(&dc, i);
    
    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

void CShadingPopup::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_COLOUR;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // Get the row and column
    nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);

    // In range? If not, default and exit
    if (nNewSelection < 0 || nNewSelection >= m_nNumColours)
    {
		CWnd::OnMouseMove(nFlags, point);
        return;
    }
    
    // OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.
    if (nNewSelection != m_nCurrentSel)
        ChangeSelection(nNewSelection);

    CWnd::OnMouseMove(nFlags, point);
}

// End selection on LButtonUp
void CShadingPopup::OnLButtonUp(UINT nFlags, CPoint point) 
{    
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(LOWORD(pos), HIWORD(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(CPN_SELENDOK);
    else
        EndSelection(CPN_SELENDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CShadingPopup implementation

int CShadingPopup::GetIndex(int row, int col) const
{ 
    if (row < 0 || col < 0 || row >= m_nNumRows || col >= m_nNumColumns)
        return INVALID_COLOUR;
    else
    {
        if (row*m_nNumColumns + col >= m_nNumColours)
            return INVALID_COLOUR;
        else
            return row*m_nNumColumns + col;
    }
}

int CShadingPopup::GetRow(int nIndex) const               
{ 
    if (nIndex < 0 || nIndex >= m_nNumColours)
        return INVALID_COLOUR;
    else
        return nIndex / m_nNumColumns; 
}

int CShadingPopup::GetColumn(int nIndex) const            
{ 
    if (nIndex < 0 || nIndex >= m_nNumColours)
        return INVALID_COLOUR;
    else
        return nIndex % m_nNumColumns; 
}

// Gets the dimensions of the colour cell given by (row,col)
BOOL CShadingPopup::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex < 0 || nIndex >= m_nNumColours)
        return FALSE;

    rect->left = GetColumn(nIndex) * m_nBoxSize + m_nMargin;
    rect->top  = GetRow(nIndex) * m_nBoxSize + m_nMargin;

    // Move everything down if we are displaying a default text area
    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;

    return TRUE;
}

// Works out an appropriate size and position of this window
void CShadingPopup::SetWindowSize()
{
    CSize TextSize;
    // Get the number of columns and rows
    m_nNumColumns = 4; 
    m_nNumRows = m_nNumColours / m_nNumColumns;
    if (m_nNumColours % m_nNumColumns) m_nNumRows++;

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

void CShadingPopup::ChangeSelection(int nIndex)
{
    CClientDC dc(this);        // device context for drawing

    if ((m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumColours))
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_COLOUR;
        DrawCell(&dc, OldSel);
    }

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
    DrawCell(&dc, m_nCurrentSel);

    m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) CLR_DEFAULT, nIndex);
}

void CShadingPopup::EndSelection(int nMessage)
{
    ReleaseCapture();
	m_pParent->SendMessage(nMessage, (WPARAM) m_crColour, m_nCurrentSel);
    DestroyWindow();
}

void CShadingPopup::DrawCell(CDC* pDC, int nIndex)
{
    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // fill background
    if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenColourSel == nIndex)
        pDC->DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);
	// Draw icons
	if(m_hShadingIcon[nIndex])  
		pDC->DrawIcon(rect.left+2,rect.top+2, m_hShadingIcon[nIndex]);
}


void CShadingPopup::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

    ReleaseCapture();
}

// KillFocus problem fix suggested by Paul Wilkerson.
#if _MSC_VER >= 1300
	void CShadingPopup::OnActivateApp(BOOL bActive, DWORD hTask) 
#else
	void CShadingPopup::OnActivateApp(BOOL bActive, HTASK hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);

	// If Deactivating App, cancel this selection
	if (!bActive)
		 EndSelection(CPN_SELENDCANCEL);
}
