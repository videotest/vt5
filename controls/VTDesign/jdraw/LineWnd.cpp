// LineWnd.cpp : implementation file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
#include "stdafx.h"
#include <math.h>
#include "LinePicker.h"
#include "LineWnd.h"
#include "dashLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_Line    -1

#define MAX_LineS      9

#include "..\\resource.h"

LineTableEntry CLineWnd::m_crLines[] = 
{
    { 0,    _T("Solid Line")          },
    { 1,    _T("Dash Line")             },
    { 2,    _T("Dot Line")		},

    { 3,    _T("Dash Dot Line")     },
    { 4,    _T("Dash Dot Dot Line")     },
    { 5,    _T("Dash Dot Dot Dot Line")        },

    { 6,    _T("Round Dot Line")     },
    { 7,    _T("Round Dash Dot Line")     },
    { 8,    _T("Round Dash Dot Dot Line")        }
    
};

/////////////////////////////////////////////////////////////////////////////
// CLineWnd

CLineWnd::CLineWnd()
{
    Initialise();
}

CLineWnd::CLineWnd(CPoint p, int crLine, CWnd* pParentWnd)
{
    Initialise();

    m_crLine       = m_crInitialLine = crLine;
    m_pParent        = pParentWnd;


    CLineWnd::Create(p, crLine, pParentWnd);
}

void CLineWnd::Initialise()
{
    m_nNumLines       = sizeof(m_crLines)/sizeof(LineTableEntry);
    ASSERT(m_nNumLines <= MAX_LineS);
    if (m_nNumLines > MAX_LineS)
        m_nNumLines = MAX_LineS;

    m_BoxSize.cx       = 85;
    m_BoxSize.cy       = 20;
    m_nMargin           = 3*::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_Line;
    m_nChosenLineSel    = INVALID_Line;
    m_pParent           = NULL;
    m_crLine          = m_crInitialLine = 1; //Standard Line

    // Idiot check: Make sure the square is at least 24 x 24;
    //if (m_nBoxSize - 2*m_nMargin - 2 < 24) m_nBoxSize = 24 + 2*m_nMargin + 2;

    // Create the font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    m_Font.CreateFontIndirect(&(ncm.lfMessageFont));
}

CLineWnd::~CLineWnd()
{
    m_Font.DeleteObject();
}

BOOL CLineWnd::Create(CPoint p, int crLine, CWnd* pParentWnd)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CLinePicker)));
    m_pParent  = pParentWnd;
    m_crLine = m_crInitialLine = crLine;

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
    FindCellFromLine(crLine);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CLineWnd, CWnd)
    //{{AFX_MSG_MAP(CLineWnd)
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
// CLineWnd message handlers

// For tooltips
BOOL CLineWnd::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

// If an Line key is pressed, then move the selection
void CLineWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int row = m_nCurrentSel;

    if (nChar == VK_DOWN) 
    {
        row++;
        if (row < 0)
            row = 0;
        ChangeSelection(row);
    }

    if (nChar == VK_UP) 
    {
        if (row > 0) row--;
        else /* row == 0 */
            row = m_nNumLines-1; 
        ChangeSelection(row);
    }


    if (nChar == VK_ESCAPE) 
    {
        m_crLine = m_crInitialLine;
        EndSelection(LPN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(LPN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// auto-deletion
void CLineWnd::OnNcDestroy() 
{
    CWnd::OnNcDestroy();
    delete this;
}

void CLineWnd::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    // Draw Line cells
    for (int i = 0; i < m_nNumLines; i++)
        DrawCell(&dc, i);
    
    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

void CLineWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_Line;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    nNewSelection = point.y / m_BoxSize.cy;
    // OK - we have the row of the current selection
    // Has the row selection changed? If yes, then redraw old and new cells.
    if (nNewSelection != m_nCurrentSel)
        ChangeSelection(nNewSelection);

    CWnd::OnMouseMove(nFlags, point);
}

// End selection on LButtonUp
void CLineWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{    
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(LOWORD(pos), HIWORD(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(LPN_SELENDOK);
    else
        EndSelection(LPN_SELENDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CLineWnd implementation



void CLineWnd::FindCellFromLine(int crLine)
{
    if (crLine<=m_nNumLines && crLine>0)
		m_nChosenLineSel = crLine;
    else
        m_nChosenLineSel = INVALID_Line;
}

// Gets the dimensions of the colour cell given by (row,col)
BOOL CLineWnd::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex < 0 || nIndex >= m_nNumLines)
        return FALSE;

    rect->left =  m_nMargin;
    rect->top  = nIndex * m_BoxSize.cy+ m_nMargin;
    rect->right = rect->left + m_BoxSize.cx;
    rect->bottom = rect->top + m_BoxSize.cy;
return TRUE;
}

// Works out an appropriate size and position of this window
void CLineWnd::SetWindowSize()
{
   // CSize TextSize;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_BoxSize.cx + 2*m_nMargin,
                         rect.top  + m_BoxSize.cy*m_nNumLines+ 2*m_nMargin);

 
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

void CLineWnd::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(this)) return;

    // Add a tool for each cell
    for (int i = 0; i < m_nNumLines; i++)
    {
        CRect rect;
        if (!GetCellRect(i, rect)) continue;
            m_ToolTip.AddTool(this, GetLineName(i), rect, 1);
    }
}

 void CLineWnd::ChangeSelection(int nIndex)
{
    CClientDC dc(this);        // device context for drawing


    if (m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumLines)
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_Line;
        DrawCell(&dc, OldSel);
    }

    // Set the current selection as row and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
    DrawCell(&dc, m_nCurrentSel);

    // Store the current colour
    m_crLine = m_nCurrentSel;
    m_pParent->SendMessage(LPN_SELCHANGEBT, (WPARAM) m_crLine, 0);
}

void CLineWnd::EndSelection(int nMessage)
{
    ReleaseCapture();

    // If custom text selected, perform a custom colour selection
    if (nMessage == LPN_SELENDCANCEL)
        m_crLine = m_crInitialLine;

    m_pParent->SendMessage(nMessage, (WPARAM) m_crLine, 0);
    
    DestroyWindow();
}

void CLineWnd::DrawCell(CDC* pDC, int nIndex)
{
    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // fill background
    if (m_nChosenLineSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenLineSel == nIndex)
        pDC->DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);

	CPen pen;
	if (!pen.CreatePen(PS_SOLID, 3, RGB(0,0,0)))
		return;
	CPen *pOldPen = pDC->SelectObject(&pen);
	unsigned Type1[8];
    int c1;
	int width=3;
    rect.DeflateRect(4,4);
	switch(nIndex){
	case 0:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_SOLID);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
		break;
		}
	case 1:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASH);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
		break;
		}
	case 2:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DOT);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 3:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOT);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 4:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOTDOT);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 5:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOTDOTDOT);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 6:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASH_GAP);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 7:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DOT_GAP);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 8:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOT_GAP);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 9:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOTDOT_GAP);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	case 10:
		{
		c1 = CDashLine::GetPattern(Type1, true, width, DL_DASHDOTDOTDOT_GAP);
		CDashLine dashline(*pDC, Type1, c1); //construct the dashline object
		dashline.MoveTo(rect.left, (rect.top+rect.bottom)/2);
	    dashline.LineTo(rect.right, (rect.top+rect.bottom)/2);
	    break;
		}
	}
	pDC->SelectObject(pOldPen);

    // Draw the cell colour
    
    // restore DC and cleanup
}


void CLineWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

    ReleaseCapture();
    //DestroyWindow(); - causes crash when Custom colour dialog appears.
}

// KillFocus problem fix suggested by Paul Wilkerson.
#if _MSC_VER >= 1300
	void CLineWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#else
	void CLineWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);

	// If Deactivating App, cancel this selection
	if (!bActive)
		 EndSelection(LPN_SELENDCANCEL);
}
