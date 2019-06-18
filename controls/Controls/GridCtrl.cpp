// GridCtrl.cpp : implementation file
//
// MFC Grid Control v2.11
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2000. All Rights Reserved.
//
// The code contained in this file is based on the original
// WorldCom Grid control written by Joe Willcoxson,
//        mailto:chinajoe@aol.com
//        http://users.aol.com/chinajoe
// (These addresses may be out of date) The code has gone through 
// so many modifications that I'm not sure if there is even a single 
// original line of code. In any case Joe's code was a great 
// framework on which to build.
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
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
//  History:
//  --------
//  This control is constantly evolving, sometimes due to new features that I
//  feel are necessary, and sometimes due to existing bugs. Where possible I
//  have credited the changes to those who contributed code corrections or
//  enhancements (names in brackets) or code suggestions (suggested by...)
//
//          1.0 - 1.13   20 Feb 1998 - 6 May 1999
//                          First release version. Progressed from being a basic
//                          grid based on the original WorldCom Grid control
//                          written by Joe Willcoxson (mailto:chinajoe@aol.com,
//                          http://users.aol.com/chinajoe) to something a little
//                          more feature rich. Rewritten so many times I doubt
//                          there is a single line of Joe's code left. Many, many,
//                          MANY people sent in bug reports and fixes. Thank you
//                          all.
//
//          2.0         1 Feb 2000
//                          Rewritten to make the grid more object oriented, in
//                          that the CGridCell class now takes care of cell-specific
//                          tasks. This makes the code more robust, but more
//                          importantly it allows the simple insertion of other
//                          types of cells.
//
//          2.01       20 Feb 2000 - Eric Woodruff
//                          Added better support for printing grids and
//                          also fixed some other minor problems.
//
//          2.02       29 Feb 2000 - Brian V. Shifrin, Scot Reed, 
//                          Fixes to reduce flicker, fix font selection bug,
//                          Fixed SetFixed[Row/Col]Count bug
//
//          2.03       28 Mar 2000 - Aqiruse (marked with //FNA)
//                           Titletips now use cell color
//                          
//          2.10       11 Mar 2000 - Ken Bertelson and Chris Maunder
//                          - Additions for virtual CGridCell support of embedded tree 
//                            & cell buttons implementation
//                          - Optional WYSIWYG printing
//                          - Awareness of hidden (0 width/height) rows and columns for 
//                            key movements,  cut, copy, paste, and autosizing
//                          - CGridCell can make title tips display any text rather than 
//                            cell text only
//                          - Minor vis bug fixes
//                          - CGridCtrl now works with CGridCellBase instead of CGridCell
//                            This is a taste of things to come.
//
//          2.11       19 May 2000 - Chris Maunder
//                          - Increasing fixed cells clashed with selected cells (Ivan Ilinov)
//                          - AutoSizeRows obvous bug fixed
//                          - OnLButtonDown fix (Ken Bertelson) 
//                          - ExpandToFit bug fixed (scrollbar space) (Igor Proskuriakov)
//                          - List mode selection/deselection fixed
//                          - Keyboard cell movement improved. You can now see the cells!
//                          - m_nBarState MS madness fixed (Phil K)
//          2.12       26 May 2000 - Martin Richter
//                          - If using TRY/CATCH (winCE) instead of try/catch (win32),
//                            e->Delete is not called
//                          - EnsureVisible "fix" was fixed properly.
//
// TODO: 1) Allow it to work as a virtual list. I've got the code, just not the
//          time to merge it (Thanks to Martin Daly)
//       2) Redo some of the internals so that sparse grids are efficient
//       3) Fix it so that as you drag select, the speed of selection increases
//          with time.
//       4) Get some sleep
//
// NOTE: 1) Grid data is stored row-by-row, so all operations on large numbers
//          of cells should be done row-by-row as well.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemDC.h"
#include "GridCtrl.h"
#include "Resource.h"

// OLE stuff for clipboard operations
#include <afxadv.h>            // For CSharedFile
#include <afxconv.h>           // For LPTSTR -> LPSTR macros

#ifndef GRIDCTRL_CLASSNAME
#define GRIDCTRL_CLASSNAME    _T("MFCGridCtrl")  // Window class name
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Use C++ exception handling instead of structured in non CE version
#ifndef _WIN32_WCE
#   undef TRY
#   undef CATCH
#   undef END_CATCH
#   define TRY try
#   define CATCH(ex_class, ex_object) catch(ex_class* ex_object)
#   define END_CATCH
#endif  // _WIN32_WCE

#define TIMER_HORZ	1
#define TIMER_VERT	2 
#define TIMER_EVENT 100


// Spit out some messages as a sanity check for programmers
#ifdef GRIDCONTROL_NO_TITLETIPS
#pragma message(" -- CGridCtrl: No titletips for cells with large data")
#endif
#ifdef GRIDCONTROL_NO_DRAGDROP
#pragma message(" -- CGridCtrl: No OLE drag and drop")
#endif
#ifdef GRIDCONTROL_NO_CLIPBOARD
#pragma message(" -- CGridCtrl: No clipboard support")
#endif
#ifdef GRIDCONTROL_NO_PRINTING
#pragma message(" -- CGridCtrl: No printing support")
#endif

IMPLEMENT_DYNCREATE(CGridCtrl, CWnd)

void AFXAPI DDX_GridControl(CDataExchange* pDX, int nIDC, CGridCtrl& rControl)
{
    if (rControl.GetSafeHwnd() == NULL)    // not subclassed yet
    {
        ASSERT(!pDX->m_bSaveAndValidate);

        HWND hWndCtrl = pDX->PrepareCtrl(nIDC);

        if (!rControl.SubclassWindow(hWndCtrl))
        {
            ASSERT(FALSE);      // possibly trying to subclass twice?
            AfxThrowNotSupportedException();
        }
#ifndef _AFX_NO_OCC_SUPPORT
        else
        {
            // If the control has reparented itself (e.g., invisible control),
            // make sure that the CWnd gets properly wired to its control site.
            if (pDX->m_pDlgWnd->GetSafeHwnd() != ::GetParent(rControl.GetSafeHwnd()))
                rControl.AttachControlSite(pDX->m_pDlgWnd);
        }
#endif //!_AFX_NO_OCC_SUPPORT
    }
}

// Get the number of lines to scroll with each mouse wheel notch
// Why doesn't windows give us this function???
UINT GetMouseScrollLines()
{
    int nScrollLines = 3;            // reasonable default
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        TCHAR szData[128];
        DWORD dwKeyDataType;
        DWORD dwDataBufSize = sizeof(szData);

        if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
                           (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
        {
            nScrollLines = _tcstoul(szData, NULL, 10);
        }
        RegCloseKey(hKey);
    }

    return nScrollLines;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

CGridCtrl::CGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols)
{
	m_bPrinting = false;
    RegisterWindowClass();

    // Initialize OLE libraries
    m_bMustUninitOLE = FALSE;

#if !defined(GRIDCONTROL_NO_DRAGDROP) || !defined(GRIDCONTROL_NO_CLIPBOARD)
    _AFX_THREAD_STATE* pState = AfxGetThreadState();
    if (!pState->m_bNeedTerm)
    {
        SCODE sc = ::OleInitialize(NULL);
        if (FAILED(sc))
            AfxMessageBox(_T("OLE initialization failed. Make sure that the OLE libraries are the correct version"));
        else
            m_bMustUninitOLE = TRUE;
    }
#endif

    // Store the system colours in case they change. The gridctrl uses
    // these colours, and in OnSysColorChange we can check to see if
    // the gridctrl colours have been changed from the system colours.
    // If they have, then leave them, otherwise change them to reflect
    // the new system colours.
    m_crWindowText        = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour      = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace            = ::GetSysColor(COLOR_3DFACE);
    m_crShadow            = ::GetSysColor(COLOR_3DSHADOW);
    m_crGridColour        = m_crShadow;

    m_nRows               = 0;
    m_nCols               = 0;
    m_nFixedRows          = 0;
    m_nFixedCols          = 0;

    m_nDefCellHeight      = 30;         // These will get changed to something meaningful
    m_nDefCellWidth       = 75;         //    when the window is created or subclassed

    m_nVScrollMax         = 0;          // Scroll position
    m_nHScrollMax         = 0;
    m_nDefCellMargin      = 0;          // cell padding
    m_nRowsPerWheelNotch  = GetMouseScrollLines(); 
                                        // Get the number of lines
                                        // per mouse wheel notch to scroll
    m_nBarState           = GVL_NONE;
    m_MouseMode           = MOUSE_NOTHING;
    m_nGridLines          = GVL_BOTH;
    m_bEditable           = TRUE;
	m_bDrawFocus          = TRUE;
    m_bListMode           = FALSE;
    m_bSingleRowSelection = FALSE;
    m_bMouseButtonDown    = FALSE;
    m_bAllowDraw          = TRUE;       // allow draw updates
    m_bEnableSelection    = TRUE;
    m_bAllowRowResize     = TRUE;
    m_bAllowColumnResize  = TRUE;
    m_bSortOnClick        = TRUE;       // Sort on header row click if in list mode
    m_bHandleTabKey       = TRUE;
#ifdef _WIN32_WCE
    m_bDoubleBuffer       = FALSE;      // Use double buffering to avoid flicker?
#else
    m_bDoubleBuffer       = TRUE;       // Use double buffering to avoid flicker?
#endif
    m_bTitleTips          = TRUE;       // show cell title tips

    m_bWysiwygPrinting    = FALSE;      // use size-to-width printing

    m_bHiddenColUnhide   = TRUE;        // 0-width columns can be expanded via mouse
    m_bHiddenRowUnhide   = TRUE;        // 0-Height rows can be expanded via mouse

    m_bAscending          = TRUE;       // sorting stuff
    m_nSortColumn         = -1;

    m_nHTimerID            = 0;          // For drag-selection
    m_nVTimerID            = 0;          // For drag-selection
    m_nTimerInterval      = 120;         // (in milliseconds)
    
	m_nResizeCaptureRange = 3;          // When resizing columns/row, the cursor has to be
                                        // within +/-3 pixels of the dividing line for
                                        // resizing to be possible
	m_ptStartResize		  = CPoint(-1, -1);
    m_pImageList          = NULL;       // Images in the grid
    m_bAllowDragAndDrop   = FALSE;      // for drag and drop - EFW - off by default

	m_bAllowDragColumn   = true;      // for drag and drop column
	m_bDragColumn = false;
	m_nDragColumn = -1;
	m_nDragOverColumn = -1;
	m_pDragList = 0;
	m_ptDragOffset = CPoint(0 ,0);
    m_nDragTimerID = (UINT)-1;
	m_ptMove = CPoint(-1, -1);

	m_nMarkerSize = 15;
	m_nMarkerColor = RGB(255, 0, 0);
	m_bPrevRight = true;
	m_bEnableAdditionRow = false;

    m_pRtcDefault = RUNTIME_CLASS(CGridCell);
#ifndef _WIN32_WCE
    // Initially use the system message font for the GridCtrl font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    memcpy(&m_Logfont, &(ncm.lfMessageFont), sizeof(LOGFONT));
#else
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
    memcpy(&m_Logfont, &lf, sizeof(LOGFONT));
#endif

	CreateLightBrush();
    // Set up the initial grid size
    SetRowCount(nRows);
    SetColumnCount(nCols);
    SetFixedRowCount(nFixedRows);
    SetFixedColumnCount(nFixedCols);

    // Set the colours
    SetTextColor(m_crWindowText);
    SetTextBkColor(m_crWindowColour);
    SetBkColor(m_crShadow);
    SetFixedTextColor(m_crWindowText);
    SetFixedBkColor(m_cr3DFace);

    SetTitleTipTextClr(CLR_DEFAULT);  //FNA
	SetTitleTipBackClr(CLR_DEFAULT); 

    // set initial selection range (ie. none)
//    m_SelectedCellMap.RemoveAll();
//    m_PrevSelectedCellMap.RemoveAll();
	m_Sel.SetEmpty();
	m_PrevSel.SetEmpty();

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // EFW - Added to support shaded/unshaded printout and
    // user-definable margins.
    m_bShadedPrintOut = TRUE;
    SetPrintMarginInfo(2, 2, 4, 4, 1, 1, 1);
#endif
}

CGridCtrl::~CGridCtrl()
{
    DeleteAllItems();

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.DestroyWindow();
#endif

    DestroyWindow();

    m_Font.DeleteObject();
    m_BoldFont.DeleteObject();

#if !defined(GRIDCONTROL_NO_DRAGDROP) || !defined(GRIDCONTROL_NO_CLIPBOARD)
    // BUG FIX - EFW
    COleDataSource *pSource = COleDataSource::GetClipboardOwner();
    if(pSource)
        COleDataSource::FlushClipboard();

    // Uninitialize OLE support
    if (m_bMustUninitOLE)
        ::OleUninitialize();
#endif
}

// Register the window class if it has not already been registered.
BOOL CGridCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
   // HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, GRIDCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
#ifndef _WIN32_WCE_NO_CURSOR
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
#else
        wndcls.hCursor          = 0;
#endif
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = GRIDCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

// creates the control - use like any other window create control
BOOL CGridCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

    if (!CWnd::Create(GRIDCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
        return FALSE;

#ifndef GRIDCONTROL_NO_DRAGDROP
    m_DropTarget.Register(this);
#endif

    // Create titletips
#ifndef GRIDCONTROL_NO_TITLETIPS
    if (m_bTitleTips && !IsWindow(m_TitleTip.m_hWnd))
        m_TitleTip.Create(this);
#endif

    // The number of rows and columns will only be non-zero if the constructor
    // was called with non-zero initialising parameters. If this window was created
    // using a dialog template then the number of rows and columns will be 0 (which
    // means that the code below will not be needed - which is lucky 'cause it ain't
    // gonna get called in a dialog-template-type-situation.

    HFONT hFont = ::CreateFontIndirect(&m_Logfont);
    OnSetFont((LPARAM)hFont, 0);
    DeleteObject(hFont);

    TRY
    {
        m_arRowHeights.SetSize(m_nRows);    // initialize row heights
        m_arColWidths.SetSize(m_nCols);     // initialize column widths
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
#ifndef _WIN32_WCE
        e->Delete();
#endif
        return FALSE;
    }
    END_CATCH

    for (int i = 0; i < m_nRows; i++)
        m_arRowHeights[i] = m_nDefCellHeight;
    for (i = 0; i < m_nCols; i++)
        m_arColWidths[i] = m_nDefCellWidth;

	EnableScrollBars(SB_BOTH, TRUE);

    // ResetScrollBars(); - called in OnSize anyway
    return TRUE;
}

void CGridCtrl::PreSubclassWindow()
{
    CWnd::PreSubclassWindow();

    HFONT hFont = ::CreateFontIndirect(&m_Logfont);
    OnSetFont((LPARAM)hFont, 0);
    DeleteObject(hFont);

    // ResetScrollBars(); - called in OnSize anyway
}

// Used by DDX_GridControl. This has been found to be necessary because of
// OLE registering problems.
BOOL CGridCtrl::SubclassWindow(HWND hWnd)
{
    if (!CWnd::SubclassWindow(hWnd))
        return FALSE;

#ifndef GRIDCONTROL_NO_DRAGDROP
    m_DropTarget.Register(this);
#endif

#ifndef GRIDCONTROL_NO_TITLETIPS
    if (m_bTitleTips && !IsWindow(m_TitleTip.m_hWnd))
        m_TitleTip.Create(this);
#endif

    return TRUE;
}

// Sends a message to the parent in the form of a WM_NOTIFY message with
// a NM_GRIDVIEW structure attached
LRESULT CGridCtrl::SendMessageToParent(int nRow, int nCol, int nMessage)
{
    if (!IsWindow(m_hWnd))
        return 0;

    NM_GRIDVIEW nmgv;
    nmgv.iRow         = nRow;
    nmgv.iColumn      = nCol;
    nmgv.hdr.hwndFrom = m_hWnd;
    nmgv.hdr.idFrom   = GetDlgCtrlID();
    nmgv.hdr.code     = nMessage;

    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
    else
        return 0;
}

BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
//EFW - Added ON_WM_RBUTTONUP
//{{AFX_MSG_MAP(CGridCtrl)
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_CHAR()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_SYSKEYDOWN()
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_WM_CANCELMODE()
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
#ifndef _WIN32_WCE_NO_CURSOR
    ON_WM_SETCURSOR()
#endif
#ifndef _WIN32_WCE
    ON_WM_RBUTTONUP()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_CAPTURECHANGED()
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
#endif
#if (_WIN32_WCE >= 210)
    ON_WM_SETTINGCHANGE()
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    ON_WM_MOUSEWHEEL()
#endif
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_INPLACE_CONTROL, OnEndInPlaceEdit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::OnPaint()
{
    CPaintDC dc(this);      // device context for painting

    if (m_bDoubleBuffer)    // Use a memory DC to remove flicker
    {
        CMemDC MemDC(&dc);
        OnDraw(&MemDC);
    }
    else                    // Draw raw - this helps in debugging vis problems.
        OnDraw(&dc);
}

BOOL CGridCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE;    // Don't erase the background.
}

// Custom background erasure. This gets called from within the OnDraw function,
// since we will (most likely) be using a memory DC to stop flicker. If we just
// erase the background normally through OnEraseBkgnd, and didn't fill the memDC's
// selected bitmap with colour, then all sorts of vis problems would occur
void CGridCtrl::EraseBkgnd(CDC* pDC)
{
    CRect  VisRect, ClipRect, rect;
    CBrush FixedBack(GetFixedBkColor()),
           TextBack(GetTextBkColor()),
           Back(GetBkColor());

    if (pDC->GetClipBox(ClipRect) == ERROR)
        return;
    GetVisibleNonFixedCellRange(VisRect);

    // Draw Fixed columns background
    int nFixedColumnWidth = GetFixedColumnWidth();
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < VisRect.bottom)
        pDC->FillRect(CRect(ClipRect.left, ClipRect.top, 
                            nFixedColumnWidth, VisRect.bottom),
                      &FixedBack);
        
    // Draw Fixed rows background
    int nFixedRowHeight = GetFixedRowHeight();
    if (ClipRect.top < nFixedRowHeight && 
        ClipRect.right > nFixedColumnWidth && ClipRect.left < VisRect.right)
        pDC->FillRect(CRect(nFixedColumnWidth-1, ClipRect.top,
                            VisRect.right, nFixedRowHeight),
                      &FixedBack);

    // Draw non-fixed cell background
    if (rect.IntersectRect(VisRect, ClipRect)) 
    {
        CRect CellRect(max(nFixedColumnWidth, rect.left), 
                       max(nFixedRowHeight, rect.top),
                       rect.right, rect.bottom);
        pDC->FillRect(CellRect, &TextBack);
    }

    // Draw right hand side of window outside grid
    if (VisRect.right < ClipRect.right) 
        pDC->FillRect(CRect(VisRect.right, ClipRect.top, 
                            ClipRect.right, ClipRect.bottom),
                      &Back);

    // Draw bottom of window below grid
    if (VisRect.bottom < ClipRect.bottom && ClipRect.left < VisRect.right) 
        pDC->FillRect(CRect(ClipRect.left, VisRect.bottom,
                            VisRect.right, ClipRect.bottom),
                      &Back);
}

void CGridCtrl::OnSize(UINT nType, int cx, int cy)
{
    static BOOL bAlreadyInsideThisProcedure = FALSE;
    if (bAlreadyInsideThisProcedure)
        return;

    if (!::IsWindow(m_hWnd))
        return;

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = TRUE;

    EndEditing();        // destroy any InPlaceEdit's

    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = FALSE;
}

UINT CGridCtrl::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS; // DLGC_WANTALLKEYS; //

    if (m_bHandleTabKey && !IsCTRLpressed())
        nCode |= DLGC_WANTTAB;

    return nCode;
}

#ifndef _WIN32_WCE
// If system colours change, then redo colours
void CGridCtrl::OnSysColorChange()
{
    CWnd::OnSysColorChange();

	SettingChange();
}
#endif

#ifndef _WIN32_WCE_NO_CURSOR
// If we are drag-selecting cells, or drag and dropping, stop now
void CGridCtrl::OnCaptureChanged(CWnd *pWnd)
{
    if (pWnd->GetSafeHwnd() == GetSafeHwnd())
        return;

    // kill timer if active
    if (m_nVTimerID != 0)
    {
        KillTimer(m_nVTimerID);
        m_nVTimerID = 0;
    }
    if (m_nHTimerID != 0)
    {
        KillTimer(m_nHTimerID);
        m_nHTimerID = 0;
    }


#ifndef GRIDCONTROL_NO_DRAGDROP
    // Kill drag and drop if active
    if (m_MouseMode == MOUSE_DRAGGING)
        m_MouseMode = MOUSE_NOTHING;
#endif

	if (m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		if (m_bDragColumn)
			m_bDragColumn = false;

		if (m_pDragList)
		{
			m_pDragList->EndDrag();
			m_pDragList->DeleteImageList();
			delete m_pDragList, m_pDragList = 0;;
		}
		
		DrawMarker(m_nDragOverColumn, m_bPrevRight, false);
//		m_nDragColumn = -1;
//		m_nDragOverColumn = -1;
//		m_MouseMode = MOUSE_NOTHING;
	}
}
#endif

#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
// If system settings change, then redo colours
void CGridCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CWnd::OnSettingChange(uFlags, lpszSection);
	SettingChange();
}
#endif

void CGridCtrl::SettingChange()
{
    if (GetTextColor() == m_crWindowText)                   // Still using system colours
        SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetTextBkColor() == m_crWindowColour)
        SetTextBkColor(::GetSysColor(COLOR_WINDOW));
    if (GetBkColor() == m_crShadow)
        SetBkColor(::GetSysColor(COLOR_3DSHADOW));
    if (GetFixedTextColor() == m_crWindowText)
        SetFixedTextColor(::GetSysColor(COLOR_WINDOWTEXT));
    if (GetFixedBkColor() == m_cr3DFace)
        SetFixedBkColor(::GetSysColor(COLOR_3DFACE));

    m_crWindowText   = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace       = ::GetSysColor(COLOR_3DFACE);
    m_crShadow       = ::GetSysColor(COLOR_3DSHADOW);

    m_nRowsPerWheelNotch = GetMouseScrollLines(); // Get the number of lines
}


// For drag-selection. Scrolls hidden cells into view
// TODO: decrease timer interval over time to speed up selection over time
void CGridCtrl::OnTimer(UINT_PTR nIDEvent)
{

    if (nIDEvent != WM_HSCROLL && nIDEvent != WM_VSCROLL)
	{
 		return;
	}

    CPoint pt, origPt;

#ifdef _WIN32_WCE
    if (m_MouseMode == MOUSE_NOTHING)
        return;
    origPt = GetMessagePos();
#else
    if (!GetCursorPos(&origPt))
        return;
#endif

    ScreenToClient(&origPt);

    CRect rect;
    GetClientRect(rect);

	CCellID cell;
	CCellRange range;

	CCellID FocusCell = GetFocusCell();
	pt = origPt;
    if (nIDEvent == WM_VSCROLL)
	{
	    int nFixedRowHeight = GetFixedRowHeight();
		if (pt.y > rect.bottom)
		{
			CPoint pt1(pt.x, rect.bottom);
			CPoint pt2 = pt;
			ClientToScreen(&pt1);
			ClientToScreen(&pt2);

			int nCount = abs((pt2.y - pt1.y) / GetDefCellHeight());
			int nTime = 0;
			if (nCount > 0)
			{
				int nInterval = m_nTimerInterval / nCount;
				if (nInterval < 25)
				{
					nInterval = 25;
					nTime = nCount - int(m_nTimerInterval / 25);
				}
				KillTimer(m_nVTimerID);
				m_nVTimerID = SetTimer(WM_VSCROLL, nInterval, 0);
			}
			for (int i = 0; i <= nTime; i++)
				SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);

			if (pt.x < rect.left)
				pt.x = rect.left;
			if (pt.x > rect.right)
				pt.x = rect.right;
			pt.y = rect.bottom;
			
			cell = GetCellFromPt(pt);
			range = GetVisibleNonFixedCellRange();
			if (IsValid(range))
			{
				if (cell.row < range.Top())
					cell.row = range.Top();

				if (cell.row > range.Bottom())
					cell.row = range.Bottom();

				if (cell.col < range.Left())
					cell.col = range.Left();

				if (cell.col > range.Right())
					cell.col = range.Right();
			}

			if (m_MouseMode != MOUSE_COLUMN_DRAGGING)
			{
				OnSelecting(cell);
				if (FocusCell != cell)
					SetFocusCell(cell);

				CRect CellRect;
				if (GetCellRect(cell, &CellRect))
				{
					CGridCellBase* pCell = GetCell(cell.row, cell.col);
					bool bPtrInRect = CellRect.PtInRect(origPt) == TRUE;
					if (m_OverCell != cell || !bPtrInRect)
					{
						if (IsValid(m_OverCell))
						{
							CGridCellBase* pOldCell = GetCell(m_OverCell.row, m_OverCell.col);
							if (pOldCell)
								pOldCell->OnMouseLeave(m_OverCell.row, m_OverCell.col, origPt);
						}

						m_OverCell = bPtrInRect ? cell : CCellID(-1, -1);
						if (pCell && bPtrInRect)
							pCell->OnMouseEnter(m_OverCell.row, m_OverCell.col, origPt);
					}
					if (pCell && bPtrInRect)
						pCell->OnMouseOver(m_OverCell.row, m_OverCell.col, origPt);
				}
			}
		}
		else if (pt.y < nFixedRowHeight)
		{
			CPoint pt1(pt.x, nFixedRowHeight);
			CPoint pt2 = pt;
			ClientToScreen(&pt1);
			ClientToScreen(&pt2);

			int nCount = abs((pt1.y - pt2.y) / GetDefCellHeight());
			int nTime = 0;
			if (nCount > 0)
			{
				int nInterval = m_nTimerInterval / nCount;
				if (nInterval < 25)
				{
					nInterval = 25;
					nTime = nCount - int(m_nTimerInterval / 25);
				}
				KillTimer(m_nVTimerID);
				m_nVTimerID = SetTimer(WM_VSCROLL, nInterval, 0);
			}
			
			for (int i = 0; i <= nTime; i++)
				SendMessage(WM_VSCROLL, SB_LINEUP, 0);

			if (pt.x < rect.left)
				pt.x = rect.left;
			if (pt.x > rect.right)
				pt.x = rect.right;
			pt.y = nFixedRowHeight + 1;
			
			cell = GetCellFromPt(pt);
			range = GetVisibleNonFixedCellRange();
			if (IsValid(range))
			{
				if (cell.row < range.Top())
					cell.row = range.Top();

				if (cell.row > range.Bottom())
					cell.row = range.Bottom();

				if (cell.col < range.Left())
					cell.col = range.Left();

				if (cell.col > range.Right())
					cell.col = range.Right();
			}
			
			if (m_MouseMode != MOUSE_COLUMN_DRAGGING)
			{
				OnSelecting(cell);
				if (FocusCell != cell)
					SetFocusCell(cell);

				CRect CellRect;
				if (GetCellRect(cell, &CellRect))
				{
					CGridCellBase* pCell = GetCell(cell.row, cell.col);
					bool bPtrInRect = CellRect.PtInRect(origPt) == TRUE;
					if (m_OverCell != cell || !bPtrInRect)
					{
						if (IsValid(m_OverCell))
						{
							CGridCellBase* pOldCell = GetCell(m_OverCell.row, m_OverCell.col);
							if (pOldCell)
								pOldCell->OnMouseLeave(m_OverCell.row, m_OverCell.col, origPt);
						}

						m_OverCell = bPtrInRect ? cell : CCellID(-1, -1);
						if (pCell && bPtrInRect)
							pCell->OnMouseEnter(m_OverCell.row, m_OverCell.col, origPt);
					}
					if (pCell && bPtrInRect)
						pCell->OnMouseOver(m_OverCell.row, m_OverCell.col, origPt);
				}
			}
		}
	}

	if (nIDEvent == WM_HSCROLL)
	{
		int nFixedColWidth = GetFixedColumnWidth();
		pt = origPt;
		if (pt.x > rect.right)
		{
			CPoint pt1(rect.right, pt.y);
			CPoint pt2 = pt;
			ClientToScreen(&pt1);
			ClientToScreen(&pt2);

			int nCount = abs((pt1.x - pt2.x) / GetDefCellWidth());
			TRACE ("nCount %d\n", nCount);
			int nTime = 0;
			if (nCount > 0)
			{
				int nInterval = 2 * m_nTimerInterval * GetDefCellWidth() / abs(pt1.x - pt2.x);
				if (nInterval < 25)
				{
					nInterval = 25;
					nTime = nCount - 2 *int(m_nTimerInterval / 25);
					if (nTime < 0)
						nTime = 0;
				}
				KillTimer(m_nHTimerID);
				m_nHTimerID = SetTimer(WM_HSCROLL, nInterval, 0);
			}
			
//			int nPrev = GetScrollPos32(SB_HORZ);
			for (int i = 0; i <= nTime; i++)
				SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
//			int nNext = GetScrollPos32(SB_HORZ);
			
			if (pt.y < rect.top)
				pt.y = rect.top;
			if (pt.y > rect.bottom)
				pt.y = rect.bottom;
			pt.x = rect.right;
			
			cell = GetCellFromPt(pt);
			range = GetVisibleNonFixedCellRange();
			if (IsValid(range))
			{
				if (cell.row < range.Top())
					cell.row = range.Top();

				if (cell.row > range.Bottom())
					cell.row = range.Bottom();

				if (cell.col < range.Left())
					cell.col = range.Left();

				if (cell.col > range.Right())
					cell.col = range.Right();
			}
			
			if (m_MouseMode != MOUSE_COLUMN_DRAGGING)
			{
				OnSelecting(cell);
				if (FocusCell != cell)
					SetFocusCell(cell);

				CRect CellRect;
				if (GetCellRect(cell, &CellRect))
				{
					CGridCellBase* pCell = GetCell(cell.row, cell.col);
					bool bPtrInRect = CellRect.PtInRect(origPt) == TRUE;
					if (m_OverCell != cell || !bPtrInRect)
					{
						if (IsValid(m_OverCell))
						{
							CGridCellBase* pOldCell = GetCell(m_OverCell.row, m_OverCell.col);
							if (pOldCell)
								pOldCell->OnMouseLeave(m_OverCell.row, m_OverCell.col, origPt);
						}

						m_OverCell = bPtrInRect ? cell : CCellID(-1, -1);
						if (pCell && bPtrInRect)
							pCell->OnMouseEnter(m_OverCell.row, m_OverCell.col, origPt);
					}
					if (pCell && bPtrInRect)
						pCell->OnMouseOver(m_OverCell.row, m_OverCell.col, origPt);
				}
			}
			else // column drag
			{
				m_ptMove = pt;

				if (m_pDragList)
					m_pDragList->EndDrag();

				DrawMarker(m_nDragOverColumn, m_bPrevRight, false);

/*				CPoint ptnext(rect.right - (nNext - nPrev), pt.y);
				ClientToScreen(&ptnext);
				SetCursorPos(ptnext.x, ptnext.y);
*/
//				UpdateWindow();
				DrawMarker(m_nDragOverColumn, m_bPrevRight, true);

				if (m_pDragList)
				{
					m_pDragList->BeginDrag(0, m_ptDragOffset);
					m_pDragList->DragEnter(this, CPoint(m_ptMove.x, m_ptDragOffset.y));
					m_pDragList->DragMove(CPoint(m_ptMove.x, m_ptDragOffset.y + 2));
				}

//				CCellID cell = GetCellFromPt(m_ptMove);
				int nOldCol = m_nDragOverColumn;
				bool bOldRight = m_bPrevRight;

				if (EnableColumnDrag(cell.col))
					m_nDragOverColumn = cell.col;
				
				CPoint ptOrig;
				GetCellOrigin(0, m_nDragOverColumn, &ptOrig);
				int nC = ptOrig.x + GetColumnWidth(m_nDragOverColumn) / 2;
				m_bPrevRight = nC < m_ptMove.x ? true : false;

				if (nOldCol != m_nDragOverColumn || bOldRight != m_bPrevRight)
				{
					if (abs(nOldCol - m_nDragOverColumn) == 1 && bOldRight != m_bPrevRight)
						return;
					DrawMarker(nOldCol, bOldRight, false);
					DrawMarker(m_nDragOverColumn, m_bPrevRight, true);
				}
			}
		}
		else if (pt.x < nFixedColWidth)
		{
			CPoint pt1(nFixedColWidth, pt.y);
			CPoint pt2 = pt;
			ClientToScreen(&pt1);
			ClientToScreen(&pt2);

			int nCount = abs((pt2.x - pt1.x) / GetDefCellWidth());
			TRACE ("nCount %d\n", nCount);
			int nTime = 0;
			if (nCount > 0)
			{
				int nInterval = 2 * m_nTimerInterval * GetDefCellWidth() / abs(pt2.x - pt1.x);
				if (nInterval < 25)
				{
					nInterval = 25;
					nTime = nCount - 2 * int(m_nTimerInterval / 25);
					if (nTime < 0)
						nTime = 0;
				}
				KillTimer(m_nHTimerID);
				m_nHTimerID = SetTimer(WM_HSCROLL, nInterval, 0);
			}
			
//			int nPrev = GetScrollPos32(SB_HORZ);
			for (int i = 0; i <= nTime; i++)
				SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
//			int nNext = GetScrollPos32(SB_HORZ);

			if (pt.y < rect.top)
				pt.y = rect.top;
			if (pt.y > rect.bottom)
				pt.y = rect.bottom;
			pt.x = nFixedColWidth + 1;

			cell = GetCellFromPt(pt);
			range = GetVisibleNonFixedCellRange();
			if (IsValid(range))
			{
				if (cell.row < range.Top())
					cell.row = range.Top();

				if (cell.row > range.Bottom())
					cell.row = range.Bottom();

				if (cell.col < range.Left())
					cell.col = range.Left();

				if (cell.col > range.Right())
					cell.col = range.Right();
			}
			
			if (m_MouseMode != MOUSE_COLUMN_DRAGGING)
			{
				OnSelecting(cell);
				if (FocusCell != cell)
					SetFocusCell(cell);

				CRect CellRect;
				if (GetCellRect(cell, &CellRect))
				{
					CGridCellBase* pCell = GetCell(cell.row, cell.col);
					bool bPtrInRect = CellRect.PtInRect(origPt) == TRUE;
					if (m_OverCell != cell || !bPtrInRect)
					{
						if (IsValid(m_OverCell))
						{
							CGridCellBase* pOldCell = GetCell(m_OverCell.row, m_OverCell.col);
							if (pOldCell)
								pOldCell->OnMouseLeave(m_OverCell.row, m_OverCell.col, origPt);
						}

						m_OverCell = bPtrInRect ? cell : CCellID(-1, -1);
						if (pCell && bPtrInRect)
							pCell->OnMouseEnter(m_OverCell.row, m_OverCell.col, origPt);
					}
					if (pCell && bPtrInRect)
						pCell->OnMouseOver(m_OverCell.row, m_OverCell.col, origPt);
				}
			}
			else
			{
				m_ptMove = pt;

				if (m_pDragList)
					m_pDragList->EndDrag();

				DrawMarker(m_nDragOverColumn, m_bPrevRight, false);

//				UpdateWindow();
				DrawMarker(m_nDragOverColumn, m_bPrevRight, true);

				if (m_pDragList)
				{
					m_pDragList->BeginDrag(0, m_ptDragOffset);
					m_pDragList->DragEnter(this, CPoint(m_ptMove.x, m_ptDragOffset.y));
					m_pDragList->DragMove(CPoint(m_ptMove.x, m_ptDragOffset.y + 2));
				}

//				CCellID cell = GetCellFromPt(m_ptMove);
				int nOldCol = m_nDragOverColumn;
				bool bOldRight = m_bPrevRight;

				if (EnableColumnDrag(cell.col))
					m_nDragOverColumn = cell.col;
				
				CPoint ptOrig;
				GetCellOrigin(0, m_nDragOverColumn, &ptOrig);
				int nC = ptOrig.x + GetColumnWidth(m_nDragOverColumn) / 2;
				m_bPrevRight = nC < m_ptMove.x ? true : false;

				if (nOldCol != m_nDragOverColumn || bOldRight != m_bPrevRight)
				{
					if (abs(nOldCol - m_nDragOverColumn) == 1 && bOldRight != m_bPrevRight)
						return;
					DrawMarker(nOldCol, bOldRight, false);
					DrawMarker(m_nDragOverColumn, m_bPrevRight, true);
				}
			}

		}
	}
//	TRACE ("Timer : select Cell(%d, %d)\n", cell.row, cell.col);
}

void CGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (!IsValid(m_idCurrentCell))
    {
        CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
        return;
    }

	switch (nChar)
	{
	case VK_TAB:
	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_HOME:
	case VK_END:
	    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_ACTIVATE);
		break;

	default:
		CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

// move about with keyboard
void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE && m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		m_bMouseButtonDown = FALSE;

	#ifndef _WIN32_WCE_NO_CURSOR
		ClipCursor(NULL);
	#endif

		if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
		{
			ReleaseCapture();
			KillTimer(m_nHTimerID);
			KillTimer(m_nVTimerID);
			m_nHTimerID = 0;
			m_nVTimerID = 0;
		}
		m_bDragColumn = false;
		if (m_pDragList)
		{
			m_pDragList->EndDrag();
			m_pDragList->DeleteImageList();
			delete m_pDragList, m_pDragList = 0;;
		}

		DrawMarker(m_nDragOverColumn, m_bPrevRight, false);
		m_nDragOverColumn -= m_bPrevRight ? 0 : 1;

		m_nDragColumn = -1;
		m_nDragOverColumn = -1;
		m_MouseMode = MOUSE_NOTHING;

	#ifndef _WIN32_WCE_NO_CURSOR
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	#endif
		return;
	}

    if (!IsValid(m_idCurrentCell))
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    CCellID next = m_idCurrentCell;
    BOOL bChangeLine = FALSE;

    if (IsCTRLpressed())
    {
        switch (nChar)
        {
        case 'A':
            OnEditSelectAll();
            break;
#ifndef GRIDCONTROL_NO_CLIPBOARD
        case 'X':
            OnEditCut();
            break;
        case VK_INSERT:
        case 'C':
            OnEditCopy();
            break;
        case 'V':
            OnEditPaste();
            break;
#endif
        }
    }

#ifndef GRIDCONTROL_NO_CLIPBOARD
    if (IsSHIFTpressed() &&(nChar == VK_INSERT))
        OnEditPaste();
#endif

    BOOL bFoundVisible;
    int iOrig;

    switch (nChar)
    {
    case VK_DELETE: 
        if (IsCellEditable(m_idCurrentCell.row, m_idCurrentCell.col))
        {
            SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_BEGINLABELEDIT);
            SetItemText(m_idCurrentCell.row, m_idCurrentCell.col, _T(""));
            SetModified(TRUE, m_idCurrentCell.row, m_idCurrentCell.col);
            SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_ENDLABELEDIT);
            RedrawCell(m_idCurrentCell);
        }
        break;
        
    case VK_TAB:    
        if (IsSHIFTpressed())
        {
            if (next.col > m_nFixedCols) 
                next.col--;
            else if (next.col == m_nFixedCols && next.row > m_nFixedRows) 
            {
                next.row--; 
                next.col = GetColumnCount() - 1; 
                bChangeLine = TRUE;
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        }
        else
        {
            if (next.col <(GetColumnCount() - 1)) 
                next.col++;
            else if (next.col == (GetColumnCount() - 1) && 
                next.row <(GetRowCount() - 1))
            {
                next.row++; 
                next.col = m_nFixedCols; 
                bChangeLine = TRUE;
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        } 
        break;
        
    case VK_DOWN:   
            // don't let user go to a hidden row
            bFoundVisible = FALSE;
            iOrig = next.row;
            next.row++;
            while( next.row < GetRowCount())
            {
                if( GetRowHeight( next.row) > 0)
                {
                    bFoundVisible = TRUE;
                    break;
                }
                next.row++;
            }
            if( !bFoundVisible)
                next.row = iOrig;

            break;
        
    case VK_UP:     
            // don't let user go to a hidden row
            bFoundVisible = FALSE;
            iOrig = next.row;
            next.row--;
            while( next.row >= m_nFixedRows)
            {
                if( GetRowHeight( next.row) > 0)
                {
                    bFoundVisible = TRUE;
                    break;
                }
                next.row--;
            }
            if( !bFoundVisible)
                next.row = iOrig;

            break;
        
    case VK_RIGHT:  
            // don't let user go to a hidden column
            bFoundVisible = FALSE;
            iOrig = next.col;
            next.col++;
            while( next.col < GetColumnCount())
            {
                if( GetColumnWidth( next.col) > 0)
                {
                    bFoundVisible = TRUE;
                    break;
                }
                next.col++;
            }
            if( !bFoundVisible)
                next.col = iOrig;

            break;
        
    case VK_LEFT:   
            // don't let user go to a hidden column
            bFoundVisible = FALSE;
            iOrig = next.col;
            next.col--;
            while( next.col >= m_nFixedCols)
            {
                if( GetColumnWidth( next.col) > 0)
                {
                    bFoundVisible = TRUE;
                    break;
                }
                next.col--;
            }
            if( !bFoundVisible)
                next.col = iOrig;

            break;
        
    case VK_NEXT:   
        {
            CCellID idOldTopLeft = GetTopleftNonFixedCell();
            SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
            CCellID idNewTopLeft = GetTopleftNonFixedCell();

            int increment = idNewTopLeft.row - idOldTopLeft.row;
            if (increment)
            {
                next.row += increment;
                if (next.row >(GetRowCount() - 1))
                    next.row = GetRowCount() - 1;
            }
            else
                next.row = GetRowCount() - 1;
            break;
        }

    case VK_PRIOR:  
        {
            CCellID idOldTopLeft = GetTopleftNonFixedCell();
            SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
            CCellID idNewTopLeft = GetTopleftNonFixedCell();
            
            int increment = idNewTopLeft.row - idOldTopLeft.row;
            if (increment) 
            {
                next.row += increment;
                if (next.row < m_nFixedRows) 
                    next.row = m_nFixedRows;
            }
            else
                next.row = m_nFixedRows;
            break;
        }
        
    case VK_HOME:   
            // Home and Ctrl-Home work more like Excel
            //  and don't let user go to a hidden cell
            if (IsCTRLpressed())
            {
                SendMessage(WM_VSCROLL, SB_TOP, 0);
                SendMessage(WM_HSCROLL, SB_LEFT, 0);
                next.row = m_nFixedRows;
                next.col = m_nFixedCols;
            }
            else
            {
                SendMessage(WM_HSCROLL, SB_LEFT, 0);
                next.col = m_nFixedCols;
            }
            // adjust column to avoid hidden columns and rows
            while( next.col < GetColumnCount() - 1)
            {
                if( GetColumnWidth( next.col) > 0)
                    break;
                next.col++;
            }
            while( next.row < GetRowCount() - 1)
            {
                if( GetRowHeight( next.row) > 0)
                    break;
                next.row++;
            }
            break;
        
    case VK_END:    
        // End and Ctrl-End work more like Excel
        //  and don't let user go to a hidden cell
        if (IsCTRLpressed())
        {
            SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
            next.row = GetRowCount() - 1;
            next.col = GetColumnCount() - 1;
        }
        else
        {
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
            next.col = GetColumnCount() - 1;
        }
        // adjust column to avoid hidden columns and rows
        while( next.col > m_nFixedCols + 1)
        {
            if( GetColumnWidth( next.col) > 0)
                break;
            next.col--;
        }
        while( next.row > m_nFixedRows + 1)
        {
            if( GetRowHeight( next.row) > 0)
                break;
            next.row--;
        }

        break;
        
    case VK_F2:
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), VK_LBUTTON);
        break;

    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    if (next != m_idCurrentCell)
    {
        // While moving with the Cursorkeys the current ROW/CELL will get selected
        // OR Selection will get expanded when SHIFT is pressed
        // Cut n paste from OnLButtonDown - Franco Bez
        // Added check for NULL mouse mode - Chris Maunder.
        if (m_MouseMode == MOUSE_NOTHING)
        {
//            m_PrevSelectedCellMap.RemoveAll();
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            if (!IsSHIFTpressed() || nChar == VK_TAB)
                m_SelectionStartCell = next;
            OnSelecting(next);
            m_MouseMode = MOUSE_NOTHING;
        }

        SetFocusCell(next);
//        SendMessageToParent(next.row, next.col, GVN_ACTIVATE);

        if (!IsCellVisible(next))
        {

            switch (nChar)
            {
            case VK_RIGHT:  
                SendMessage(WM_HSCROLL, SB_LINERIGHT, 0); 
                break;
                
            case VK_LEFT:   
                SendMessage(WM_HSCROLL, SB_LINELEFT, 0);  
                break;
                
            case VK_DOWN:   
                SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  
                break;
                
            case VK_UP:     
                SendMessage(WM_VSCROLL, SB_LINEUP, 0);    
                break;                
                
            case VK_TAB:    
                if (IsSHIFTpressed())
                {
                    if (bChangeLine) 
                    {
                        SendMessage(WM_VSCROLL, SB_LINEUP, 0);
                        SetScrollPos32(SB_HORZ, m_nHScrollMax);
                        break;
                    }
                    else 
                        SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
                }
                else
                {
                    if (bChangeLine) 
                    {
                        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
                        SetScrollPos32(SB_HORZ, 0);
                        break;
                    }
                    else 
                        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
                }
                break;
            }
            EnsureVisible(next); // Make sure cell is visible
//            Invalidate();
//			UpdateWindow();
        }
        EnsureVisible(next); // Make sure cell is visible
    }
}

void CGridCtrl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef GRIDCONTROL_USE_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
#endif

	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

// Instant editing of cells when keys are pressed
void CGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // EFW - BUG FIX
    if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && nChar != VK_ESCAPE)
    {
        if (!m_bHandleTabKey || (m_bHandleTabKey && nChar != VK_TAB))
            OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), nChar);
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

// Callback from any CInPlaceEdits that ended. This just calls OnEndEditCell,
// refreshes the edited cell and moves onto next cell if the return character
// from the edit says we should.
void CGridCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
    GV_ITEM     *pgvItem = &pgvDispInfo->item;

    // In case OnEndInPlaceEdit called as window is being destroyed
    if (!IsWindow(GetSafeHwnd()))
        return;

    OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->strText);
    //InvalidateCellRect(CCellID(pgvItem->row, pgvItem->col));

    SendMessageToParent(pgvItem->row, pgvItem->col, GVN_ENDLABELEDIT);

    switch (pgvItem->lParam)
    {
        case VK_TAB:
        case VK_DOWN:
        case VK_UP:
        case VK_RIGHT:
        case VK_LEFT:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_HOME:
        case VK_END:
            OnKeyDown(pgvItem->lParam, 0, 0);
            OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), pgvItem->lParam);
    }

    *pResult = 0;
}

// Handle horz scrollbar notifications
void CGridCtrl::OnHScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEditing();

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
#endif

    int scrollPos = GetScrollPos32(SB_HORZ);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
    case SB_LINERIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            // may have contiguous hidden columns.  Blow by them
            while (idTopLeft.col < (GetColumnCount()-1)
                    && GetColumnWidth( idTopLeft.col) < 1 )
            {
                idTopLeft.col++;
            }
            int xScroll = GetColumnWidth(idTopLeft.col);
            SetScrollPos32(SB_HORZ, scrollPos + xScroll);
            if (GetScrollPos32(SB_HORZ) == scrollPos)
			{
                // didn't work 
				// we can suppose virtual width is not agree with scroll width
				// so we need to increase scroll width to needed value
				int nW = (int)GetVirtualWidth();
				SCROLLINFO sf;
				ZeroMemory(&sf, sizeof(SCROLLINFO));
				sf.cbSize = sizeof(SCROLLINFO);
				sf.fMask = SIF_RANGE;
				if (GetScrollInfo(SB_HORZ, &sf, SIF_RANGE) && (sf.nMax - sf.nMin) < nW - 1)
				{
					ResetScrollBars();
				    scrollPos = GetScrollPos32(SB_HORZ);
		            SetScrollPos32(SB_HORZ, scrollPos + xScroll);
					if (GetScrollPos32(SB_HORZ) == scrollPos)
						break;          

				}
			}

            rect.left = GetFixedColumnWidth();
            //rect.left = GetFixedColumnWidth() + xScroll;
            //ScrollWindow(-xScroll, 0, rect);
            //rect.left = rect.right - xScroll;
            InvalidateRect(rect);
        }
        break;

    case SB_LINELEFT:
        if (scrollPos > 0 && idTopLeft.col > GetFixedColumnCount())
        {
            int iColToUse = idTopLeft.col-1;
            // may have contiguous hidden columns.  Blow by them
            while(  iColToUse > GetFixedColumnCount()
                    && GetColumnWidth( iColToUse) < 1 )
            {
                iColToUse--;
            }

            int xScroll = GetColumnWidth(iColToUse);
            SetScrollPos32(SB_HORZ, max(0, scrollPos - xScroll));
            rect.left = GetFixedColumnWidth();
            //ScrollWindow(xScroll, 0, rect);
            //rect.right = rect.left + xScroll;
            InvalidateRect(rect);
        }
        break;

    case SB_PAGERIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            rect.left = GetFixedColumnWidth();
            int offset = rect.Width();
            int pos = min(m_nHScrollMax, scrollPos + offset);
            SetScrollPos32(SB_HORZ, pos);
            rect.left = GetFixedColumnWidth();
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGELEFT:
        if (scrollPos > 0)
        {
            rect.left = GetFixedColumnWidth();
            int offset = -rect.Width();
            int pos = max(0, scrollPos + offset);
            SetScrollPos32(SB_HORZ, pos);
            rect.left = GetFixedColumnWidth();
            InvalidateRect(rect);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            SetScrollPos32(SB_HORZ, GetScrollPos32(SB_HORZ, TRUE));
            CCellID idNewTopLeft = GetTopleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            {
                rect.left = GetFixedColumnWidth();
                InvalidateRect(rect);
            }
        }
        break;
        
    case SB_LEFT:
        if (scrollPos > 0)
        {
            SetScrollPos32(SB_HORZ, 0);
            Invalidate();
        }
        break;
        
    case SB_RIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            SetScrollPos32(SB_HORZ, m_nHScrollMax);
            Invalidate();
        }
        break;
        
        
    default: 
        break;
    }
}

// Handle vert scrollbar notifications
void CGridCtrl::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEditing();

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
#endif

    // Get the scroll position ourselves to ensure we get a 32 bit value
    int scrollPos = GetScrollPos32(SB_VERT);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
    case SB_LINEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
            // may have contiguous hidden rows.  Blow by them
            while(  idTopLeft.row < (GetRowCount()-1)
                    && GetRowHeight( idTopLeft.row) < 1 )
            {
                idTopLeft.row++;
            }

            int yScroll = GetRowHeight(idTopLeft.row);
            SetScrollPos32(SB_VERT, scrollPos + yScroll);
            if (GetScrollPos32(SB_VERT) == scrollPos)
			{
                // didn't work 
				// we can suppose virtual width is not agree with scroll width
				// so we need to increase scroll width to needed value
				int nH = (int)GetVirtualHeight();
				SCROLLINFO sf;
				ZeroMemory(&sf, sizeof(SCROLLINFO));
				sf.cbSize = sizeof(SCROLLINFO);
				sf.fMask = SIF_RANGE;
				if (GetScrollInfo(SB_VERT, &sf, SIF_RANGE) && (sf.nMax - sf.nMin) < nH - 1)
				{
					ResetScrollBars();
				    scrollPos = GetScrollPos32(SB_VERT);
					SetScrollPos32(SB_VERT, scrollPos + yScroll);
					if (GetScrollPos32(SB_VERT) == scrollPos)
						break;          

				}
			}
            rect.top = GetFixedRowHeight();
            //rect.top = GetFixedRowHeight() + yScroll;
            //ScrollWindow(0, -yScroll, rect);
            //rect.top = rect.bottom - yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_LINEUP:
        if (scrollPos > 0 && idTopLeft.row > GetFixedRowCount())
        {
            int iRowToUse = idTopLeft.row-1;
            // may have contiguous hidden rows.  Blow by them
            while(  iRowToUse > GetFixedRowCount()
                    && GetRowHeight( iRowToUse) < 1 )
            {
                iRowToUse--;
            }

            int yScroll = GetRowHeight( iRowToUse);
            SetScrollPos32(SB_VERT, max(0, scrollPos - yScroll));
            rect.top = GetFixedRowHeight();
            //ScrollWindow(0, yScroll, rect);
            //rect.bottom = rect.top + yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
            rect.top = GetFixedRowHeight();
            scrollPos = min(m_nVScrollMax, scrollPos + rect.Height());
            SetScrollPos32(SB_VERT, scrollPos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEUP:
        if (scrollPos > 0)
        {
            rect.top = GetFixedRowHeight();
            int offset = -rect.Height();
            int pos = max(0, scrollPos + offset);
            SetScrollPos32(SB_VERT, pos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(rect);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            SetScrollPos32(SB_VERT, GetScrollPos32(SB_VERT, TRUE));
            CCellID idNewTopLeft = GetTopleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            {
                rect.top = GetFixedRowHeight();
                InvalidateRect(rect);
            }
        }
        break;
        
    case SB_TOP:
        if (scrollPos > 0)
        {
            SetScrollPos32(SB_VERT, 0);
            Invalidate();
        }
        break;
        
    case SB_BOTTOM:
        if (scrollPos < m_nVScrollMax)
        {
            SetScrollPos32(SB_VERT, m_nVScrollMax);
            Invalidate();
        }
        
    default: 
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl implementation functions

void CGridCtrl::OnDraw(CDC* pDC)
{
    if (!m_bAllowDraw)
        return;

    CRect clipRect;
    if (pDC->GetClipBox(&clipRect) == ERROR)
        return;

    EraseBkgnd(pDC);            // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.

    CRect rect;
    int row, col;
    CGridCellBase* pCell;

	CRect ClientRect;
	GetClientRect(&ClientRect);

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth  = GetFixedColumnWidth();

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();

    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;
        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            pCell = GetCell(row, col);
            if (pCell)
			{
				UINT uState = pCell->GetState();
				if (IsCellSelected(row, col))
					uState |= GVIS_SELECTED;
				else
					uState &= ~GVIS_SELECTED;
				pCell->SetState(uState);
                pCell->Draw(pDC, row, col, rect, FALSE);
			}
        }
    }

    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;            // gone past cliprect
            if (rect.right < clipRect.left)
                continue;         // Reached cliprect yet?

            pCell = GetCell(row, col);
            if (pCell)
			{
				UINT uState = pCell->GetState();
				if (IsCellSelected(row, col))
					uState |= GVIS_SELECTED;
				else
					uState &= ~GVIS_SELECTED;
				pCell->SetState(uState);
                pCell->Draw(pDC, row, col, rect, FALSE);
			}
        }
    }

	rect.top = rect.bottom + 1;
	rect.bottom = ClientRect.bottom;
    rect.right = -1;
    for (col = 0; col < m_nFixedCols; col++)
    {
        rect.left = rect.right+1;
        rect.right = rect.left + GetColumnWidth(col)-1;

        if (rect.left > clipRect.right)
            break;            // gone past cliprect
        if (rect.right < clipRect.left)
            continue;         // Reached cliprect yet?

        DrawFixedEmptyCell(pDC, GetRowCount() - 1, col, rect);
    }

    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            if (pCell)
			{
				UINT uState = pCell->GetState();
				if (IsCellSelected(row, col))
					uState |= GVIS_SELECTED;
				else
					uState &= ~GVIS_SELECTED;
				pCell->SetState(uState);
                pCell->Draw(pDC, row, col, rect, FALSE);
			}
        }

        rect.left = rect.right+1;
        rect.right = ClientRect.right;
        if (!(rect.left > clipRect.right) && !(rect.right < clipRect.left))
			DrawFixedEmptyCell(pDC, row, -1, rect);
    }

    // draw rest of non-fixed cells
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            // TRACE(_T("Cell %d,%d type: %s\n"), row, col, pCell->GetRuntimeClass()->m_lpszClassName);
            if (pCell)
			{
				UINT uState = pCell->GetState();
				if (IsCellSelected(row, col))
					uState |= GVIS_SELECTED;
				else
					uState &= ~GVIS_SELECTED;
				pCell->SetState(uState);
                pCell->Draw(pDC, row, col, rect, FALSE);
			}
        }

        rect.left = rect.right+1;
        rect.right = ClientRect.right;
        if (!(rect.left > clipRect.right) && !(rect.right < clipRect.left))
			DrawEmptyCell(pDC, row, -1, rect);
    }

	{
		rect.top = rect.bottom+1;
		rect.bottom = ClientRect.bottom - 1;
		rect.right = nFixedColWidth-1;
		for (col = minVisibleCol; col <= maxVisibleCol; col++)
		{
			rect.left = rect.right+1;
			rect.right = rect.left + GetColumnWidth(col)-1;

			if (rect.left > clipRect.right)
				break;        // gone past cliprect
			if (rect.right < clipRect.left)
				continue;     // Reached cliprect yet?

			DrawEmptyCell(pDC, GetRowCount() - 1, col, rect);
		}

		rect.left = rect.right+1;
		rect.right = ClientRect.right;
		if (!(rect.left > clipRect.right) && !(rect.right < clipRect.left))
			DrawEmptyCell(pDC, row, -1, rect);
	}
    CPen pen, lightPen;
    TRY
    {
        pen.CreatePen(PS_SOLID, 0, m_crGridColour);
		//[AY] - drawing problem
		//lightPen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW/*COLOR_3DHILIGHT*/));
		lightPen.CreatePen(PS_SOLID, 0, m_crGridColour);
		
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
        int x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            x += GetColumnWidth(col);
	        pDC->SelectObject(&pen);
            pDC->MoveTo(x-1, nFixedRowHeight);
            pDC->LineTo(x-1, ClientRect.bottom);
//            pDC->LineTo(x-1, VisRect.bottom);

//	        pDC->SelectObject(&lightPen);
//            pDC->LineTo(x-1, ClientRect.bottom);
        }
        pDC->SelectObject(&lightPen);
        pDC->MoveTo(x-1, nFixedRowHeight);
        pDC->LineTo(x-1, ClientRect.bottom);
    }

    // draw horizontal lines (drawn at bottom of each cell)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
    {
        int y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++)
        {
            y += GetRowHeight(row);
	        pDC->SelectObject(&lightPen);//pen);
            pDC->MoveTo(nFixedColWidth, y-1);
            pDC->LineTo(VisRect.right,  y-1);

			pDC->SelectObject(&lightPen);
            pDC->LineTo(ClientRect.right,  y-1);
        }
//        pDC->MoveTo(nFixedColWidth-1, y);
//        pDC->LineTo(nFixedColWidth-1, ClientRect.bottom);
    }

    pDC->SelectStockObject(NULL_PEN);
	pen.DeleteObject();
	lightPen.DeleteObject();


}

void CGridCtrl::DrawFixedEmptyCell(CDC * pDC, int nRow, int nCol, CRect rect)
{
    if (!pDC)
        return;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldColor = pDC->GetTextColor();

	CGridCellBase * pCell = GetCell(nRow, nCol);
    // Set up text and background colours
    COLORREF TextBkClr;

    if (pCell && pCell->GetBackClr() != CLR_DEFAULT)
        TextBkClr = pCell->GetBackClr();
    else
        TextBkClr = GetFixedBkColor();

    rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
	CBrush brush(TextBkClr);
	pDC->FillRect(rect, &brush);
    rect.right--; rect.bottom--;
	brush.DeleteObject();

    // Setup font and if fixed, draw fixed cell highlighting

    // Draw lines only when wanted
    if (GetGridLines() != GVL_NONE)
    {
		CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
			 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
			 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
			*pOldPen = pDC->GetCurrentPen();

		pDC->SelectObject(&lightpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom);
/*
		pDC->SelectObject(&medpen);
		pDC->MoveTo(rect.right - 1, rect.top + 1);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left + 1, rect.bottom - 1);
*/
		pDC->SelectObject(&darkpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.left - 1, rect.bottom);

		pDC->SelectObject(pOldPen);

		lightpen.DeleteObject();
		darkpen.DeleteObject();
		medpen.DeleteObject();
    }
	pDC->SetTextColor(clrOldColor);
    pDC->SetBkMode(nOldMode);
    pDC->RestoreDC(nSavedDC);
}

void CGridCtrl::DrawEmptyCell(CDC * pDC, int nRow, int nCol, CRect rect)
{
    if (!pDC)
        return;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return;           //  though cell is hidden

    //TRACE3("Drawing %scell %d, %d\n", bDrawFixed? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);

	CGridCellBase * pCell = GetCell(nRow, nCol);
    // Set up text and background colours
    COLORREF TextBkClr;

    if (pCell)
	{
		if (pCell->GetBackClr() != CLR_DEFAULT)
			TextBkClr = pCell->GetBackClr();
		else
			TextBkClr = GetTextBkColor();
	}
    else
        TextBkClr = GetFixedBkColor();
		
    rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
	CBrush brush(TextBkClr);
	pDC->FillRect(rect, &brush);
    rect.right--; rect.bottom--;
	brush.DeleteObject();

    pDC->SetBkMode(nOldMode);
    pDC->RestoreDC(nSavedDC);
}

////////////////////////////////////////////////////////////////////////////////////////
// CGridCtrl Cell selection stuff

// Is a given cell designation valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(int nRow, int nCol) const
{
    return (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
}

BOOL CGridCtrl::IsValid(const CCellID& cell) const
{
    return IsValid(cell.row, cell.col);
}

// Is a given cell range valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(const CCellRange& range) const
{
    return (range.GetMinRow() >= 0 && range.GetMinCol() >= 0 &&
        range.GetMaxRow() >= 0 && range.GetMaxCol() >= 0 &&
        range.GetMaxRow() < m_nRows && range.GetMaxCol() < m_nCols &&
        range.GetMinRow() <= range.GetMaxRow() && range.GetMinCol() <= range.GetMaxCol());
}

// Enables/Disables redraw for certain operations like columns auto-sizing etc,
// but not for user caused things such as selection changes.
BOOL CGridCtrl::SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars /* = FALSE */)
{
	BOOL bPrevState = m_bAllowDraw;
    if (bAllowDraw && !m_bAllowDraw && m_hWnd && ::IsWindow(m_hWnd))
        Invalidate();

    m_bAllowDraw = bAllowDraw;
    if (bResetScrollBars)
        ResetScrollBars();

	return bPrevState;
}

// Forces a redraw of a cell immediately (using a direct DC construction,
// or the supplied dc)
BOOL CGridCtrl::RedrawCell(const CCellID& cell, CDC* pDC /* = NULL */)
{
    return RedrawCell(cell.row, cell.col, pDC);
}

BOOL CGridCtrl::RedrawCell(int nRow, int nCol, CDC* pDC /* = NULL */)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

    if (!m_bAllowDraw || !IsCellVisible(nRow, nCol))
        return FALSE;

    CRect rect;
    if (!GetCellRect(nRow, nCol, rect))
        return FALSE;

    if (!pDC)
    {
        pDC = GetDC();
        if (pDC)
            bMustReleaseDC = TRUE;
    }

    if (pDC)
    {
        // Redraw cells directly
        if (nRow < m_nFixedRows || nCol < m_nFixedCols)
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);
        }
        else
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);

            // Since we have erased the background, we will need to redraw the gridlines
            CPen pen;
            TRY
            {
                pen.CreatePen(PS_SOLID, 0, m_crGridColour);
            }
            CATCH(CException, e)
            {
#ifndef _WIN32_WCE
                e->Delete();
#endif
            } END_CATCH

            CPen* pOldPen = (CPen*) pDC->SelectObject(&pen);
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                pDC->MoveTo(rect.left,    rect.bottom);
                pDC->LineTo(rect.right + 1, rect.bottom);
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                pDC->MoveTo(rect.right, rect.top);
                pDC->LineTo(rect.right, rect.bottom + 1);
            }
            pDC->SelectObject(pOldPen);
        }
    } else
        InvalidateRect(rect, TRUE);     // Could not get a DC - invalidate it anyway
    // and hope that OnPaint manages to get one

    if (bMustReleaseDC)
        ReleaseDC(pDC);

    return bResult;
}

// redraw a complete row
BOOL CGridCtrl::RedrawRow(int row)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int col = 0; col < GetColumnCount(); col++)
        bResult = RedrawCell(row, col, pDC) && bResult;
    if (pDC)
        ReleaseDC(pDC);

    return bResult;
}

// redraw a complete column
BOOL CGridCtrl::RedrawColumn(int col)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int row = 0; row < GetRowCount(); row++)
        bResult = RedrawCell(row, col, pDC) && bResult;
    if (pDC)
        ReleaseDC(pDC);

    return bResult;
}


// Sets the currently selected cell, returning the previous current cell
CCellID CGridCtrl::SetFocusCell(int nRow, int nCol)
{
    return SetFocusCell(CCellID(nRow, nCol));
}

CCellID CGridCtrl::SetFocusCell(CCellID cell)
{
    if (cell == m_idCurrentCell)
        return m_idCurrentCell;

//	if (!cell.row)
//		TRACE ("SetFocusCell(%d, %d)\n", cell.row, cell.col);

//	if (!cell.col)
//		TRACE ("SetFocusCell(%d, %d)\n", cell.row, cell.col);
	EnsureVisible(cell);

    CCellID idPrev = m_idCurrentCell;

    // EFW - Bug Fix - Force focus to be in a non-fixed cell
    if(cell.row != -1 && cell.row < GetFixedRowCount())
        cell.row = GetFixedRowCount();
    if(cell.col != -1 && cell.col < GetFixedColumnCount())
        cell.col = GetFixedColumnCount();

    m_idCurrentCell = cell;

    if (IsValid(idPrev))
    {
        SendMessageToParent(idPrev.row, idPrev.col, GVN_SELCHANGING);

		UINT nState = GetItemState(idPrev.row, idPrev.col);
		nState &= ~GVIS_FOCUSED;
//		if (IsCellSelected(idPrev.row, idPrev.col))
//			nState &= ~GVIS_SELECTED;

        SetItemState(idPrev.row, idPrev.col, nState);
//        RedrawCell(idPrev); // comment to reduce flicker
		InvalidateCellRect(idPrev.row, idPrev.col);

        if (idPrev.col != m_idCurrentCell.col)
		{
			InvalidateCellRect(CCellRange(0, idPrev.col, m_nFixedRows - 1, idPrev.col));
/*            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, idPrev.col);*/
		}
        if (idPrev.row != m_idCurrentCell.row)
		{
			InvalidateCellRect(CCellRange(idPrev.row, 0, idPrev.row, m_nFixedCols - 1));
/*            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(idPrev.row, col);*/
		}
    }

    if (IsValid(m_idCurrentCell))
    {
        SetItemState(m_idCurrentCell.row, m_idCurrentCell.col,
            GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);

        RedrawCell(m_idCurrentCell); // comment to reduce flicker

        if (idPrev.col != m_idCurrentCell.col)
		{
			InvalidateCellRect(CCellRange(0, m_idCurrentCell.col, m_nFixedRows - 1, m_idCurrentCell.col));
/*            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, m_idCurrentCell.col);*/
		}
        if (idPrev.row != m_idCurrentCell.row)
		{
			InvalidateCellRect(CCellRange(m_idCurrentCell.row, 0, m_idCurrentCell.row, m_nFixedCols - 1));
/*            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(m_idCurrentCell.row, col);*/
		}

        SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_SELCHANGED);

        // EFW - New addition.  If in list mode, make sure the selected
        // row highlight follows the cursor.
        if (FALSE && m_bListMode)
        {
			m_PrevSel.SetEmpty();
//            m_PrevSelectedCellMap.RemoveAll();
            m_MouseMode = MOUSE_SELECT_ROW;
            OnSelecting(m_idCurrentCell);

            // Leave this off so that you can still drag the highlight around
            // without selecting rows.
            // m_MouseMode = MOUSE_NOTHING;
        }
    }

    return idPrev;
}

// Sets the range of currently selected cells
void CGridCtrl::SetSelectedRange(const CCellRange& Range,
                                 BOOL bForceRepaint /* = FALSE */, BOOL bSelectCells/*=TRUE*/)
{
    SetSelectedRange(Range.GetMinRow(), Range.GetMinCol(),
                     Range.GetMaxRow(), Range.GetMaxCol(),
                     bForceRepaint, bSelectCells);
}

void CGridCtrl::SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                                 BOOL bForceRepaint /* = FALSE */, BOOL bSelectCells/*=TRUE*/)
{
	bSelectCells;
	bForceRepaint;
    if (!m_bEnableSelection)
        return;

//    CDC* pDC = NULL;
//    if (bForceRepaint)
//        pDC = GetDC();

    // EFW - Bug fix - Don't allow selection of fixed rows
    if(nMinRow >= 0 && nMinRow < GetFixedRowCount())
        nMinRow = GetFixedRowCount();
    if(nMaxRow >= 0 && nMaxRow < GetFixedRowCount())
        nMaxRow = GetFixedRowCount();
    if(nMinCol >= 0 && nMinCol < GetFixedColumnCount())
        nMinCol = GetFixedColumnCount();
    if(nMaxCol >= 0 && nMaxCol < GetFixedColumnCount())
        nMaxCol = GetFixedColumnCount();

    CCellRange newRange(nMinRow, nMinCol, nMaxRow, nMaxCol);
	CSelection unsel = m_Sel - newRange;

    if (!GetSingleRowSelection() && newRange.IsValid())
		unsel -= m_PrevSel;
// unselect 
	for (POSITION pos = unsel.GetRanges().GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = unsel.GetRanges().GetNext(pos);
		if (!range.IsValid())
			continue;

		InvalidateCellRect(range);
	}

// select
	// save already selected
	CSelection tempsel = m_Sel - unsel;
	// set new selection
	m_Sel.SetEmpty();
	m_Sel.AddRange(newRange);
	// calc needed to select 
	CSelection sel = m_Sel - tempsel;
	// add prev selected
	m_Sel += tempsel;

   	if (!sel.IsEmpty())
	{
		for (POSITION pos = sel.GetRanges().GetHeadPosition(); pos != NULL; )
		{
			CCellRange range = sel.GetRanges().GetNext(pos);
			if (!range.IsValid())
				continue;

			InvalidateCellRect(range);
		}
	}

//    if (pDC != NULL)
//        ReleaseDC(pDC);
}

// selects all cells
void CGridCtrl::SelectAllCells()
{
    if (!m_bEnableSelection)
        return;

    SetSelectedRange(m_nFixedRows, m_nFixedCols, GetRowCount()-1, GetColumnCount()-1);
}

// selects columns
void CGridCtrl::SelectColumns(CCellID currentCell, 
                              BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.col == m_idCurrentCell.col) return;
    if (currentCell.col < m_nFixedCols)
        return;
    if (!IsValid(currentCell))
        return;

    SetSelectedRange(GetFixedRowCount(),
                     min(m_SelectionStartCell.col, currentCell.col),
                     GetRowCount()-1,
                     max(m_SelectionStartCell.col, currentCell.col),
                     bForceRedraw, bSelectCells);
}

// selects rows
void CGridCtrl::SelectRows(CCellID currentCell, 
                           BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.row; == m_idCurrentCell.row) return;
    if (currentCell.row < m_nFixedRows)
        return;
    if (!IsValid(currentCell))
        return;

    if (GetSingleRowSelection())
        SetSelectedRange(currentCell.row, GetFixedColumnCount(),
                         currentCell.row, GetColumnCount()-1, 
                         FALSE, bSelectCells);
    else
        SetSelectedRange(min(m_SelectionStartCell.row, currentCell.row),
                         GetFixedColumnCount(),
                         max(m_SelectionStartCell.row, currentCell.row),
                         GetColumnCount()-1,
                         bForceRedraw, bSelectCells);
}

// selects cells
void CGridCtrl::SelectCells(CCellID currentCell, 
                            BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    int row = currentCell.row;
    int col = currentCell.col;
    if (row < m_nFixedRows || col < m_nFixedCols)
        return;
    if (!IsValid(currentCell))
        return;

    // Prevent unnecessary redraws
    //if (currentCell == m_LeftClickDownCell)  return;
    //else if (currentCell == m_idCurrentCell) return;

    SetSelectedRange(min(m_SelectionStartCell.row, row),
                     min(m_SelectionStartCell.col, col),
                     max(m_SelectionStartCell.row, row),
                     max(m_SelectionStartCell.col, col),
                     bForceRedraw, bSelectCells);
}

// Called when mouse/keyboard selection is a-happening.
void CGridCtrl::OnSelecting(const CCellID& currentCell)
{
    if (!m_bEnableSelection)
        return;

    switch (m_MouseMode)
    {
    case MOUSE_SELECT_ALL:
        SelectAllCells();
        break;
    case MOUSE_SELECT_COL:
        SelectColumns(currentCell);
        break;
    case MOUSE_SELECT_ROW:
        SelectRows(currentCell);
        break;
    case MOUSE_SELECT_CELLS:
        SelectCells(currentCell);
        break;
    }

    // EFW - Bug fix [REMOVED CJM: this will cause infinite loop in list mode]
    SetFocusCell(max(currentCell.row, m_nFixedRows), max(currentCell.col, m_nFixedCols));
}

#ifndef GRIDCONTROL_NO_CLIPBOARD

////////////////////////////////////////////////////////////////////////////////////////
// Clipboard functions

// Deletes the contents from the selected cells
void CGridCtrl::CutSelectedText()
{
    if (!IsEditable())
        return;

	for (POSITION pos = m_Sel.GetRanges().GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = m_Sel.GetRanges().GetNext(pos);
		if (!range.IsValid())
			continue;

        CCellID cell;
        for (int row = range.GetMinRow(); row <= range.GetMaxRow(); row++)
		{
            for (int col = range.GetMinCol(); col <= range.GetMaxCol(); col++)
            {
				cell = CCellID(row, col);
		        if (IsValid(cell))
				{
					if (!IsCellEditable(cell))
						continue;
					CGridCellBase* pCell = GetCell(cell.row, cell.col);
					if (pCell)
					{
						SendMessageToParent(cell.row, cell.col, GVN_BEGINLABELEDIT);
						pCell->SetText(_T(""));
						SetModified(TRUE, cell.row, cell.col);
						SendMessageToParent(cell.row, cell.col, GVN_ENDLABELEDIT);
					}
				}
            }
		}
	}
	Invalidate();
}

// Copies text from the selected cells to the clipboard
COleDataSource* CGridCtrl::CopyTextFromGrid()
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
    for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
    {
        // don't copy hidden cells
        if( m_arRowHeights[row] <= 0 )
            continue;

        str.Empty();
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
            str += _T("\n");
        
        sf.Write(T2A(str.GetBuffer(1)), str.GetLength());
        str.ReleaseBuffer();
    }
    
    char c = '\0';
    sf.Write(&c, 1);

    DWORD dwLen = (DWORD)sf.GetLength();
    HGLOBAL hMem = sf.Detach();
    if (!hMem)
        return NULL;

    hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    if (!hMem)
        return NULL;

    // Cache data
    COleDataSource* pSource = new COleDataSource();
    pSource->CacheGlobalData(CF_TEXT, hMem);

    return pSource;
}

// Pastes text from the clipboard to the selected cells
BOOL CGridCtrl::PasteTextToGrid(CCellID cell, COleDataObject* pDataObject)
{
    if (!IsValid(cell) || !IsCellEditable(cell) || !pDataObject->IsDataAvailable(CF_TEXT))
        return FALSE;

    // Get the text from the COleDataObject
    HGLOBAL hmem = pDataObject->GetGlobalData(CF_TEXT);
    CMemFile sf((BYTE*) ::GlobalLock(hmem), ::GlobalSize(hmem));

    // CF_TEXT is ANSI text, so we need to allocate a char* buffer
    // to hold this.
    LPSTR szBuffer = new char[::GlobalSize(hmem)];
    if (!szBuffer)
        return FALSE;

    sf.Read(szBuffer, ::GlobalSize(hmem));
    ::GlobalUnlock(hmem);

    // Now store in generic TCHAR form so we no longer have to deal with
    // ANSI/UNICODE problems
    CString strText = szBuffer;
    delete szBuffer;

    // Parse text data and set in cells...
    strText.LockBuffer();
    CString strLine = strText;
    int nLine = 0;

    // Find the end of the first line
    int nIndex;
    do
    {
        int nColumn = 0;
        nIndex = strLine.Find(_T("\n"));

        // Store the remaining chars after the newline
        CString strNext = (nIndex < 0)? _T("")  : strLine.Mid(nIndex + 1);

        // Remove all chars after the newline
        if (nIndex >= 0)
            strLine = strLine.Left(nIndex);

        // Make blank entries a "space"
        if (strLine.IsEmpty() && nIndex >= 0)
            strLine = _T(" ");

        LPTSTR szLine = strLine.GetBuffer(1);

        // Break the current line into tokens (tab or comma delimited)
        LPTSTR pszCellText = _tcstok(szLine, _T("\t,\n"));

        // skip hidden rows
        int iRowVis = cell.row + nLine;
        while( iRowVis < GetRowCount())
        {
            if( GetRowHeight( iRowVis) > 0)
                break;
            nLine++;
            iRowVis++;
        }

        while (pszCellText != NULL)
        {
            // skip hidden columns
            int iColVis = cell.col + nColumn;
            while( iColVis < GetColumnCount())
            {
                if( GetColumnWidth( iColVis) > 0)
                    break;
                nColumn++;
                iColVis++;
            }

            CCellID TargetCell(iRowVis, iColVis);
            if (IsValid(TargetCell))
            {
                CString strCellText = pszCellText;
                strCellText.TrimLeft();
                strCellText.TrimRight();

                SendMessageToParent(TargetCell.row, TargetCell.col, GVN_BEGINLABELEDIT);
                SetItemText(TargetCell.row, TargetCell.col, strCellText);
                SetModified(TRUE, TargetCell.row, TargetCell.col);
                SendMessageToParent(TargetCell.row, TargetCell.col, GVN_ENDLABELEDIT);

                // Make sure cell is not selected to avoid data loss
                SetItemState(TargetCell.row, TargetCell.col,
                    GetItemState(TargetCell.row, TargetCell.col) & ~GVIS_SELECTED);
            }

            pszCellText = _tcstok(NULL, _T("\t,\n"));
            nColumn++;
        }

        strLine.ReleaseBuffer();
        strLine = strNext;
        nLine++;
    } while (nIndex >= 0);

    strText.UnlockBuffer();
    Invalidate();

    return TRUE;
}
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP

// Start drag n drop
void CGridCtrl::OnBeginDrag()
{
    if (!m_bAllowDragAndDrop)
        return;

    COleDataSource* pSource = CopyTextFromGrid();
    if (pSource)
    {
        SendMessageToParent(GetSelectedCellRange().GetTopLeft().row,
            GetSelectedCellRange().GetTopLeft().col,
            GVN_BEGINDRAG);

        m_MouseMode = MOUSE_DRAGGING;
        DROPEFFECT dropEffect = pSource->DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE);

        if (dropEffect & DROPEFFECT_MOVE)
            CutSelectedText();

        if (pSource)
            delete pSource;    // Did not pass source to clipboard, so must delete
    }
}

// Handle drag over grid
DROPEFFECT CGridCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState,
                                 CPoint point)
{
    // Any text data available for us?
    if (!m_bAllowDragAndDrop || !IsEditable() || !pDataObject->IsDataAvailable(CF_TEXT))
        return DROPEFFECT_NONE;

    // Find which cell we are over and drop-highlight it
    CCellID cell = GetCellFromPt(point, FALSE);

    // If not valid, set the previously drop-highlighted cell as no longer drop-highlighted
    if (!IsValid(cell))
    {
        OnDragLeave();
        m_LastDragOverCell = CCellID(-1,-1);
        return DROPEFFECT_NONE;
    }

    if (!IsCellEditable(cell))
        return DROPEFFECT_NONE;

    // Have we moved over a different cell than last time?
    if (cell != m_LastDragOverCell)
    {
        // Set the previously drop-highlighted cell as no longer drop-highlighted
        if (IsValid(m_LastDragOverCell))
        {
            UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
            SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
                nState & ~GVIS_DROPHILITED);
            RedrawCell(m_LastDragOverCell);
        }

        m_LastDragOverCell = cell;

        // Set the new cell as drop-highlighted
        if (IsValid(m_LastDragOverCell))
        {
            UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
            SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
                nState | GVIS_DROPHILITED);
            RedrawCell(m_LastDragOverCell);
        }
    }

    // Return an appropraite value of DROPEFFECT so mouse cursor is set properly
    if (dwKeyState & MK_CONTROL)
        return DROPEFFECT_COPY;
    else
        return DROPEFFECT_MOVE;
}

// Something has just been dragged onto the grid
DROPEFFECT CGridCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState,
                                  CPoint point)
{
    // Any text data available for us?
    if (!m_bAllowDragAndDrop || !pDataObject->IsDataAvailable(CF_TEXT))
        return DROPEFFECT_NONE;

    // Find which cell we are over and drop-highlight it
    m_LastDragOverCell = GetCellFromPt(point, FALSE);
    if (!IsValid(m_LastDragOverCell))
        return DROPEFFECT_NONE;

    if (!IsCellEditable(m_LastDragOverCell))
        return DROPEFFECT_NONE;

    if (IsValid(m_LastDragOverCell))
    {
        UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
        SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
            nState | GVIS_DROPHILITED);
        RedrawCell(m_LastDragOverCell);
    }

    // Return an appropraite value of DROPEFFECT so mouse cursor is set properly
    if (dwKeyState & MK_CONTROL)
        return DROPEFFECT_COPY;
    else
        return DROPEFFECT_MOVE;
}

// Something has just been dragged away from the grid
void CGridCtrl::OnDragLeave()
{
    // Set the previously drop-highlighted cell as no longer drop-highlighted
    if (IsValid(m_LastDragOverCell))
    {
        UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
        SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
            nState & ~GVIS_DROPHILITED);
        RedrawCell(m_LastDragOverCell);
    }
}

// Something has just been dropped onto the grid
BOOL CGridCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/,
                       CPoint /* point */)
{
    if (!m_bAllowDragAndDrop || !IsCellEditable(m_LastDragOverCell))
        return FALSE;

    m_MouseMode = MOUSE_NOTHING;
    OnDragLeave();

    return PasteTextToGrid(m_LastDragOverCell, pDataObject);
}
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
void CGridCtrl::OnEditCut()
{
    if (!IsEditable())
        return;

    COleDataSource* pSource = CopyTextFromGrid();
    if (!pSource)
        return;

    pSource->SetClipboard();
    CutSelectedText();
}

void CGridCtrl::OnEditCopy()
{
    COleDataSource* pSource = CopyTextFromGrid();
    if (!pSource)
        return;

    pSource->SetClipboard();
}

void CGridCtrl::OnEditPaste()
{
    if (!IsEditable())
        return;

    // Get the Focus cell, or if none, get the topleft (non-fixed) cell
    CCellID cell = GetFocusCell();
    if (!IsValid(cell))
        cell = GetTopleftNonFixedCell();
    if (!IsValid(cell))
        return;

    // Attach a COleDataObject to the clipboard and paste the data to the grid
    COleDataObject obj;
    if (obj.AttachClipboard())
        PasteTextToGrid(cell, &obj);
}
#endif

void CGridCtrl::OnEditSelectAll()
{
    SelectAllCells();
}

#ifndef GRIDCONTROL_NO_CLIPBOARD
void CGridCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    CCellRange Selection = GetSelectedCellRange();
    pCmdUI->Enable(Selection.Count() && IsValid(Selection));
}

void CGridCtrl::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    CCellRange Selection = GetSelectedCellRange();
    pCmdUI->Enable(IsEditable() && Selection.Count() && IsValid(Selection));
}

void CGridCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    CCellID cell = GetFocusCell();

    BOOL bCanPaste = IsValid(cell) && IsCellEditable(cell) &&
        ::IsClipboardFormatAvailable(CF_TEXT);

    pCmdUI->Enable(bCanPaste);
}
#endif

void CGridCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bEnableSelection);
}

////////////////////////////////////////////////////////////////////////////////////////
// hittest-like functions

// TRUE if the mouse is over a row resize area
BOOL CGridCtrl::MouseOverRowResizeArea(CPoint& point) const
{
    if (point.x >= GetFixedColumnWidth())
        return FALSE;

    CCellID idCurrentCell = GetCellFromPt(point);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start))
        return FALSE;

    int endy = start.y + GetRowHeight(idCurrentCell.row);

    if ((point.y - start.y <= m_nResizeCaptureRange && idCurrentCell.row != 0) ||
        abs(endy - point.y) <= m_nResizeCaptureRange)
    {
        return TRUE;
    }
    else
        return FALSE;
}

// TRUE if the mouse is over a column resize area
BOOL CGridCtrl::MouseOverColumnResizeArea(CPoint& point) const
{
    if (point.y >= GetFixedRowHeight())
        return FALSE;

    CCellID idCurrentCell = GetCellFromPt(point);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start))
        return FALSE;

    int endx = start.x + GetColumnWidth(idCurrentCell.col);

    if ((point.x - start.x <= m_nResizeCaptureRange && idCurrentCell.col != 0) ||
        abs(endx - point.x) <= m_nResizeCaptureRange)
    {
        return TRUE;
    }
    else
        return FALSE;
}

// Get cell from point
CCellID CGridCtrl::GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck /*=TRUE*/) const
{
    CCellID cellID; // return value

    CCellID idTopLeft = GetTopleftNonFixedCell();
    if (!IsValid(idTopLeft))
        return cellID;

    // calculate column index
    int fixedColWidth = GetFixedColumnWidth();

    if (point.x < 0 || (!bAllowFixedCellCheck && point.x < fixedColWidth)) // not in window
        cellID.col = -1;
	else if (point.x == 0 && (!bAllowFixedCellCheck && point.x < fixedColWidth))
		cellID.col = idTopLeft.col;
    else if (point.x < fixedColWidth) // in fixed col
    {
        int xpos = 0;
        for (int col = 0; col < m_nFixedCols; col++)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
        }
        cellID.col = col;
    }
    else    // in non-fixed col
    {
        int xpos = fixedColWidth;
        for (int col = idTopLeft.col; col < GetColumnCount(); col++)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
        }

        if (col >= GetColumnCount())
            cellID.col = GetColumnCount() - 1;
        else
            cellID.col = col;
    }

    // calculate row index
    int fixedRowHeight = GetFixedRowHeight();
    if (point.y < 0 || (!bAllowFixedCellCheck && point.y < fixedRowHeight)) // not in window
        cellID.row = -1;
	else if (point.y == 0 && (!bAllowFixedCellCheck && point.y < fixedRowHeight))
		cellID.row = idTopLeft.row;
    else if (point.y < fixedRowHeight) // in fixed col
    {
        int ypos = 0;
        for (int row = 0; row < m_nFixedRows; row++)
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
        }
        cellID.row = row;
    }
    else
    {
        int ypos = fixedRowHeight;
        for (int row = idTopLeft.row; row < GetRowCount(); row++)
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
        }

        if (row >= GetRowCount())
            cellID.row = GetRowCount() - 1;
        else
            cellID.row = row;
    }

    return cellID;
}

////////////////////////////////////////////////////////////////////////////////
// CGridCtrl cellrange functions

// Gets the first non-fixed cell ID
CCellID CGridCtrl::GetTopleftNonFixedCell() const
{
    int nVertScroll = GetScrollPos32(SB_VERT), 
        nHorzScroll = GetScrollPos32(SB_HORZ);

    int nColumn = m_nFixedCols, nRight = 0;
    while (nRight < nHorzScroll && nColumn < (GetColumnCount()-1))
        nRight += GetColumnWidth(nColumn++);

    int nRow = m_nFixedRows, nTop = 0;
    while (nTop < nVertScroll && nRow < (GetRowCount()-1))
        nTop += GetRowHeight(nRow++);

    //TRACE("TopLeft cell is row %d, col %d\n",nRow, nColumn);
    return CCellID(nRow, nColumn);
}

// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/) const
{
    CRect rect;
	if (!::IsWindow(GetSafeHwnd()))
	{
		if (pRect)
		{
	        pRect->left = pRect->top = pRect->right = pRect->bottom = 0;
		}
		return CCellRange(-1,-1,-1,-1);
	}
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    // calc bottom
    int bottom = GetFixedRowHeight();
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
    int right = GetFixedColumnWidth();
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
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// Gets the most left fixed column cell ID
CCellID CGridCtrl::GetMostLeftFixedColumn() const
{
    int nHorzScroll = GetScrollPos32(SB_HORZ);

    int nColumn = m_nFixedCols, nRight = 0;
    while (nRight < nHorzScroll && nColumn < (GetColumnCount()-1))
        nRight += GetColumnWidth(nColumn++);

    //TRACE("TopLeft cell is row %d, col %d\n",nRow, nColumn);
    return CCellID(0, nColumn);
}


// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleFixedColumnRange(LPRECT pRect /*=NULL*/) const
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetMostLeftFixedColumn();

    // calc bottom
    int bottom = GetFixedRowHeight();
    // calc right
    int right = GetFixedColumnWidth();
    for (int i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);

    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, GetFixedRowCount() -1, maxVisibleCol);
}

// Gets the most top fixed row cell ID
CCellID CGridCtrl::GetMostTopFixedRow() const
{
    int nVertScroll = GetScrollPos32(SB_VERT);

    int nRow = m_nFixedRows, nTop = 0;
    while (nTop < nVertScroll && nRow < (GetRowCount()-1))
        nTop += GetRowHeight(nRow++);

    //TRACE("TopLeft cell is row %d, col %d\n",nRow, nColumn);
    return CCellID(nRow, 0);
}

// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleFixedRowRange(LPRECT pRect /*=NULL*/) const
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetMostTopFixedRow();

    // calc bottom
    int bottom = GetFixedRowHeight();

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
    int right = GetFixedColumnWidth();
/*    for (int i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
*/
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, 0, maxVisibleRow, GetFixedColumnCount() - 1);
}

// used by ResetScrollBars() - This gets only fully visible cells
CCellRange CGridCtrl::GetUnobstructedNonFixedCellRange() const
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    // calc bottom
    int bottom = GetFixedRowHeight();
    for (int i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
            break;
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);
    if (maxVisibleRow > 0 && bottom > rect.bottom)
        maxVisibleRow--;

    // calc right
    int right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
            break;
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (maxVisibleCol > 0 && right > rect.right)
        maxVisibleCol--;


    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// Returns the minimum bounding range of the current selection
// If no selection, then the returned CCellRange will be invalid
CCellRange CGridCtrl::GetSelectedCellRange() const
{
    CCellRange Selection(GetRowCount(), GetColumnCount(), -1,-1);

	Selection.SetMinRow(m_Sel.GetMinRow());
	Selection.SetMinCol(m_Sel.GetMinCol());
	Selection.SetMaxRow(m_Sel.GetMaxRow());
	Selection.SetMaxCol(m_Sel.GetMaxCol());

    return Selection;
}

// Returns ALL the cells in the grid
CCellRange CGridCtrl::GetCellRange() const
{
    return CCellRange(0, 0, GetRowCount() - 1, GetColumnCount() - 1);
}

// Resets the selected cell range to the empty set.
void CGridCtrl::ResetSelectedRange()
{
	m_PrevSel.SetEmpty();
//    m_PrevSelectedCellMap.RemoveAll();
    SetSelectedRange(-1,-1,-1,-1);
    SetFocusCell(-1,-1);
}

// Get/Set scroll position using 32 bit functions
int CGridCtrl::GetScrollPos32(int nBar, BOOL bGetTrackPos /* = FALSE */) const
{
	if( m_bPrinting )return 0;
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);

	CScrollBar	*p = GetScrollBarCtrl( nBar);
	HWND	hwnd = m_hWnd;

	if( p )
	{
		nBar = SB_CTL;
		hwnd = p->GetSafeHwnd();
	}

	

    if (bGetTrackPos)
    {
		si.fMask = SIF_TRACKPOS;

		if( nBar == SB_CTL )
		{
			if( ::SendMessage( hwnd, SBM_GETSCROLLINFO, 0, (LPARAM)&si ) )
				return si.nTrackPos;
		}
		else
		{
			if (::GetScrollInfo(hwnd, nBar, &si))
				return si.nTrackPos;
		}
    }
    else
    {
		si.fMask = SIF_POS;

		if( nBar == SB_CTL )
		{
			if( ::SendMessage( hwnd, SBM_GETSCROLLINFO, 0, (LPARAM)&si ) )
				return si.nPos;
		}
		else
		{
			if (::GetScrollInfo(hwnd, nBar, &si) )
				return si.nPos;
		}
    }

    return 0;
}

BOOL CGridCtrl::SetScrollPos32(int nBar, int nPos, BOOL bRedraw /* = TRUE */)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS|SIF_TRACKPOS;
    si.nPos   = nPos;
	si.nTrackPos= nPos;
	
	return SetScrollInfo(nBar, &si, bRedraw);
}

void CGridCtrl::EnableScrollBarCtrl( int nBar, BOOL bEnable )
{
	HWND	hwnd = GetScrollBarCtrl( nBar )->GetSafeHwnd();
	if( hwnd )::EnableWindow( hwnd, bEnable );
	else CWnd::EnableScrollBarCtrl( nBar, bEnable );
}

BOOL CGridCtrl::SetScrollInfo( int nBar, SCROLLINFO *psi, BOOL fRedraw )
{
	HWND	hwnd = GetScrollBarCtrl( nBar )->GetSafeHwnd();
	if( hwnd )return ::SendMessage( hwnd, SBM_SETSCROLLINFO, fRedraw, (LPARAM)psi );
	else return CWnd::SetScrollInfo(nBar, psi, fRedraw);
}

BOOL CGridCtrl::GetScrollInfo(int nBar, LPSCROLLINFO psi, UINT nMask )
{
	HWND	hwnd = GetScrollBarCtrl( nBar )->GetSafeHwnd();
	if( hwnd )
	{
		psi->fMask = nMask;
		return ::SendMessage( hwnd, SBM_GETSCROLLINFO, 0, (LPARAM)psi );
	}
	else return CWnd::GetScrollInfo(nBar, psi, nMask);
}

void CGridCtrl::GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
{
	HWND	hwnd = GetScrollBarCtrl( nBar )->GetSafeHwnd();

	if( hwnd )::SendMessage( hwnd, SBM_GETRANGE, (WPARAM)lpMinPos, (LPARAM)lpMaxPos );
	else CWnd::GetScrollRange( nBar, lpMinPos, lpMaxPos );
}

void CGridCtrl::SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw )
{
	HWND	hwnd = GetScrollBarCtrl( nBar )->GetSafeHwnd();
	if( hwnd )::SendMessage( hwnd, SBM_SETRANGE, nMinPos, nMaxPos );
	else CWnd::SetScrollRange( nBar, nMinPos, nMaxPos, bRedraw );
}

void CGridCtrl::EnableScrollBars(int nBar, BOOL bEnable /*=TRUE*/)
{
    if (bEnable)
    {
        if (/*!IsVisibleHScroll() && */(nBar == SB_HORZ || nBar == SB_BOTH))
        {
            EnableScrollBarCtrl(SB_HORZ, bEnable);
            m_nBarState |= GVL_HORZ;
        }
        
        if (/*!IsVisibleVScroll() && */(nBar == SB_VERT || nBar == SB_BOTH))
        {
            EnableScrollBarCtrl(SB_VERT, bEnable);
            m_nBarState |= GVL_VERT;
       }
    }
    else
    {
        if (/* IsVisibleHScroll() && */(nBar == SB_HORZ || nBar == SB_BOTH))
        {
            EnableScrollBarCtrl(SB_HORZ, bEnable);
            m_nBarState &= ~GVL_HORZ; 
        }
        
        if (/* IsVisibleVScroll() && */(nBar == SB_VERT || nBar == SB_BOTH))
        {
            EnableScrollBarCtrl(SB_VERT, bEnable);
            m_nBarState &= ~GVL_VERT;
        }
    }
}

// If resizing or cell counts/sizes change, call this - it'll fix up the scroll bars
void CGridCtrl::ResetScrollBars()
{
    if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd())) 
        return;
    
    CRect rect;
    
    // This would have caused OnSize event - Brian 
    //EnableScrollBars(SB_BOTH, FALSE); 
    
    GetClientRect(rect);
    
    if (rect.left == rect.right || rect.top == rect.bottom)
        return;
    
    if (IsVisibleVScroll())
        rect.right += GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
    
    if (IsVisibleHScroll())
        rect.bottom += GetSystemMetrics(SM_CYHSCROLL) + GetSystemMetrics(SM_CYBORDER);
    
    rect.left += GetFixedColumnWidth();
    rect.top += GetFixedRowHeight();
    
    
    if (rect.left >= rect.right || rect.top >= rect.bottom)
    {
        EnableScrollBarCtrl(SB_BOTH, FALSE);
        return;
    }
    
    CRect VisibleRect(GetFixedColumnWidth(), GetFixedRowHeight(), rect.right, rect.bottom);
    CRect VirtualRect(GetFixedColumnWidth(), GetFixedRowHeight(), (int)GetVirtualWidth(), (int)GetVirtualHeight());
    
    
    // Removed to fix single row scrollbar problem (Pontus Goffe)
    // CCellRange visibleCells = GetUnobstructedNonFixedCellRange();
    // if (!IsValid(visibleCells)) return;
        
    //TRACE(_T("Visible: %d x %d, Virtual %d x %d.  H %d, V %d\n"), 
    //      VisibleRect.Width(), VisibleRect.Height(),
    //      VirtualRect.Width(), VirtualRect.Height(),
    //      IsVisibleHScroll(), IsVisibleVScroll());

    // If vertical scroll bar, horizontal space is reduced
    if (VisibleRect.Height() < VirtualRect.Height())
        VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
    // If horz scroll bar, vert space is reduced
    if (VisibleRect.Width() < VirtualRect.Width())
        VisibleRect.bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
    
    // Recheck vertical scroll bar
    //if (VisibleRect.Height() < VirtualRect.Height())
    // VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
    
    if (VisibleRect.Height() < VirtualRect.Height())
    {
        EnableScrollBars(SB_VERT, TRUE); 
        m_nVScrollMax = VirtualRect.Height() - 1;
    }
    else
    {
        EnableScrollBars(SB_VERT, FALSE); 
        m_nVScrollMax = 0;
    }

    if (VisibleRect.Width() < VirtualRect.Width())
    {
        EnableScrollBars(SB_HORZ, TRUE); 
        m_nHScrollMax = VirtualRect.Width() - 1;
    }
    else
    {
        EnableScrollBars(SB_HORZ, FALSE); 
        m_nHScrollMax = 0;
    }

    ASSERT(m_nVScrollMax < INT_MAX && m_nHScrollMax < INT_MAX); // This should be fine

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE;
    si.nPage = (m_nHScrollMax>0)? VisibleRect.Width() : 0;
	// A.M. fix: SetScrollInfo(.,.,TRUE) to redraw when SetScrollRange not called (bug 2608).
    SetScrollInfo(SB_HORZ, &si, TRUE); 
//    SetScrollInfo(SB_HORZ, &si, FALSE); 
    si.nPage = (m_nVScrollMax>0)? VisibleRect.Height() : 0;
    SetScrollInfo(SB_VERT, &si, TRUE);
//    SetScrollInfo(SB_VERT, &si, FALSE);

	int nMin, nMax;
	GetScrollRange(SB_VERT, &nMin, &nMax);
	if (nMin != 0 || nMax != m_nVScrollMax)
		SetScrollRange(SB_VERT, 0, m_nVScrollMax, TRUE);

	GetScrollRange(SB_HORZ, &nMin, &nMax);
	if (nMin != 0 || nMax != m_nHScrollMax)
		SetScrollRange(SB_HORZ, 0, m_nHScrollMax, TRUE);

    EnableScrollBars(SB_VERT, m_nVScrollMax != 0); 
    EnableScrollBars(SB_HORZ, m_nHScrollMax != 0); 

    //TRACE(_T("H scroll: %d, V Scroll %d\n"), m_nHScrollMax, m_nVScrollMax);
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column position functions

// returns the top left point of the cell. Returns FALSE if cell not visible.
BOOL CGridCtrl::GetCellOrigin(int nRow, int nCol, LPPOINT p) const
{
    int i;

    if (!IsValid(nRow, nCol))
        return FALSE;

    CCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = GetTopleftNonFixedCell();

	CPoint pt(0, 0);

	if (nCol >= m_nFixedCols && nCol < idTopLeft.col)
	{
		p->x = pt.x;
		for (i = nCol; i < idTopLeft.col; i++)
			p->x -= GetColumnWidth(i);
	}
	else
	{
		p->x = 0;
		if (nCol < m_nFixedCols)                      // is a fixed column
			for (i = 0; i < nCol; i++)
				p->x += GetColumnWidth(i);
		else 
		{                                        // is a scrollable data column
			for (i = 0; i < m_nFixedCols; i++)
				p->x += GetColumnWidth(i);
			for (i = idTopLeft.col; i < nCol; i++)
				p->x += GetColumnWidth(i);
		}
	}

	if (nRow >= m_nFixedRows && nRow < idTopLeft.row)
	{
		p->y = pt.y;
		for (i = nRow; i < idTopLeft.row; i++)
			p->y -= GetRowHeight(i);

	}
	else
	{
        p->y = 0;
        if (nRow < m_nFixedRows)                      // is a fixed row
            for (i = 0; i < nRow; i++)
                p->y += GetRowHeight(i);
		else 
        {                                        // is a scrollable data row
			for (i = 0; i < m_nFixedRows; i++)
				p->y += GetRowHeight(i);
			for (i = idTopLeft.row; i < nRow; i++)
				p->y += GetRowHeight(i);
        }
    }    
    return TRUE;
}

BOOL CGridCtrl::GetCellOrigin(const CCellID& cell, LPPOINT p) const
{
    return GetCellOrigin(cell.row, cell.col, p);
}

// Returns the bounding box of the cell
BOOL CGridCtrl::GetCellRect(const CCellID& cell, LPRECT pRect) const
{
    return GetCellRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetCellRect(int nRow, int nCol, LPRECT pRect) const
{
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin))
        return FALSE;

    pRect->left   = CellOrigin.x;
    pRect->top    = CellOrigin.y;
    pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
    pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;

    //TRACE("Row %d, col %d: L %d, T %d, W %d, H %d:  %d,%d - %d,%d\n",
    //      nRow,nCol, CellOrigin.x, CellOrigin.y, GetColumnWidth(nCol), GetRowHeight(nRow),
    //      pRect->left, pRect->top, pRect->right, pRect->bottom);

    return TRUE;
}

BOOL CGridCtrl::GetTextRect(const CCellID& cell, LPRECT pRect)
{
    return GetTextRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetTextRect(int nRow, int nCol, LPRECT pRect)
{
    CGridCellBase* pCell = GetCell( nRow, nCol);
    if( pCell == NULL)
        return FALSE;
    
    if( !GetCellRect( nRow, nCol, pRect) )
        return FALSE;

    return pCell->GetTextRect( pRect);
}

// Returns the bounding box of a range of cells
BOOL CGridCtrl::GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect) const
{
    CPoint MinOrigin,MaxOrigin;

    if (!GetCellOrigin(cellRange.GetMinRow(), cellRange.GetMinCol(), &MinOrigin))
        return FALSE;
    if (!GetCellOrigin(cellRange.GetMaxRow(), cellRange.GetMaxCol(), &MaxOrigin))
        return FALSE;

    lpRect->left   = MinOrigin.x;
    lpRect->top    = MinOrigin.y;
    lpRect->right  = MaxOrigin.x + GetColumnWidth(cellRange.GetMaxCol()) - 1;
    lpRect->bottom = MaxOrigin.y + GetRowHeight(cellRange.GetMaxRow()) - 1;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// Grid attribute functions

void CGridCtrl::SetDefaultFont()
{
#ifndef _WIN32_WCE
    // Initially use the system message font for the GridCtrl font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    memcpy(&m_Logfont, &(ncm.lfMessageFont), sizeof(LOGFONT));
#else
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
    memcpy(&m_Logfont, &lf, sizeof(LOGFONT));
#endif
    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(&m_Logfont);
	LOGFONT lfBold = m_Logfont;
	lfBold.lfWeight = SELECTED_CELL_FONT_WEIGHT;
	m_BoldFont.DeleteObject();
    m_BoldFont.CreateFontIndirect(&lfBold);
	for (int nRow = 0; nRow < GetRowCount(); nRow++)
	{
		for (int nColumn = 0; nColumn < GetColumnCount(); nColumn++)
		{
			CGridCellBase* pCell = GetCell(nRow, nColumn);
			if (pCell)
				pCell->SetFont(&m_Logfont);
		}
	}
}

LRESULT CGridCtrl::OnSetFont(WPARAM hFont, LPARAM /*lParam */)
{
    LRESULT result = Default();

    // Get the logical font
    LOGFONT lf;
    if (!GetObject((HFONT) hFont, sizeof(LOGFONT), &lf))
        return result;

    // Store font as the global default
	LOGFONT lfOld = m_Logfont;
    memcpy(&m_Logfont, &lf, sizeof(LOGFONT));
	CSize OldCharSize(0, 0);
	CSize NewCharSize(0, 0);

    // reset all cells' fonts
/*    for (int row = 0; row < GetRowCount(); row++)
        for (int col = 0; col < GetColumnCount(); col++)
            SetItemFont(row, col, &m_Logfont);
*/
    // Get the font size and hence the default cell size
    CDC* pDC = GetDC();
    if (pDC)
    {
		CFont * pOldFont = pDC->SelectObject(&m_BoldFont);
		OldCharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
		pDC->SelectObject(pOldFont);

        m_Font.DeleteObject();
        m_Font.CreateFontIndirect(&m_Logfont);

		lf.lfWeight = SELECTED_CELL_FONT_WEIGHT;
		m_BoldFont.DeleteObject();
        m_BoldFont.CreateFontIndirect(&lf);

        pOldFont = pDC->SelectObject(&m_Font);

        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);

        m_nDefCellMargin = pDC->GetTextExtent(_T(" "), 1).cx;
        pDC->SelectObject(pOldFont);

		pDC->SelectObject(&m_BoldFont);
		NewCharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
		pDC->SelectObject(pOldFont);

        ReleaseDC(pDC);

//EFW - Use one quarter of the font height, not twice the width of a space
//        m_nDefCellHeight = tm.tmHeight+tm.tmExternalLeading + 2*m_nMargin;
        m_nDefCellHeight = tm.tmHeight + tm.tmExternalLeading + (tm.tmHeight / 4);
        m_nDefCellWidth  = tm.tmAveCharWidth * 12 + 2 * m_nDefCellMargin;
    }

	if (OldCharSize != NewCharSize)
	{
		double fRatioH = (double)NewCharSize.cy / (double)OldCharSize.cy;
		for (int row = 0; row < GetRowCount(); row++)
		{
			CELLHEIGHT nHeight = GetRowHeight2(row);
			SetRowHeight2(row, (CELLHEIGHT)(nHeight * fRatioH));
		}
		double fRatioW = (double)NewCharSize.cx / (double)OldCharSize.cx;
		for (int col = 0; col < GetColumnCount(); col++)
		{
			CELLHEIGHT nWidth = GetColumnWidth2(col);
			SetColumnWidth2(col, (CELLHEIGHT)(nWidth * fRatioW));
		}
	}
	if (lfOld.lfHeight != m_Logfont.lfHeight)
	{
		for (int nRow = 0; nRow < GetRowCount(); nRow++)
		{
			for (int nColumn = 0; nColumn < GetColumnCount(); nColumn++)
			{
				CGridCellBase* pCell = GetCell(nRow, nColumn);
				if (pCell)
					pCell->SetFont(&m_Logfont);
			}
		}
	}

    if (::IsWindow(GetSafeHwnd()))
        Invalidate();

    return result;
}

LRESULT CGridCtrl::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return (LRESULT) (HFONT) m_Font;
}

#ifndef _WIN32_WCE_NO_CURSOR
BOOL CGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT)
    {
		CPoint pt(GetMessagePos());
		ScreenToClient(&pt);
		CCellID cell = GetCellFromPt(pt);

        switch (m_MouseMode)
        {
        case MOUSE_OVER_COL_DIVIDE:
			{
				UINT uCursor = IDC_CURSOR_WIDTH;

				CPoint start;
				if (GetCellOrigin(0, cell.col, &start) && 
					(pt.x - start.x <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(0, cell.col - 1, &start))
					{
						if (GetColumnWidth(cell.col - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_COLUMN_SPLIT;
					}
				}
				
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				
			}
            break;
        case MOUSE_OVER_ROW_DIVIDE:
			{
				UINT uCursor = IDC_CURSOR_HEIGHT;
				CPoint start;
				if (GetCellOrigin(cell.row, 0, &start) && 
					(pt.y - start.y <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(cell.row - 1, 0, &start))
					{
						if (GetRowHeight(cell.row - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_ROW_SPLIT;
					}
				}
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			}
            break;
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_DRAGGING:
            break;
#endif
        case MOUSE_COLUMN_DRAGGING:
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
            break;

        default:
            if (IsValid(cell))
            {
                CGridCellBase* pCell = GetCell(cell.row, cell.col);
                if (pCell && pCell->OnSetCursor())
					return TRUE;
            }

			if (cell.row < GetFixedRowCount() && cell.col < GetFixedColumnCount())
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				return true;
			}
			else if (cell.row < GetFixedRowCount())
			{
				CPoint start;
				if (GetCellOrigin(cell.row, cell.col, &start) && 
					(pt.x >= start.x && pt.x <= start.x + GetColumnWidth(cell.col)))
				{
					AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
					if (pState && IsDrawFocus())
						SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(IDC_CURSOR_COLUMN)));
					else
						SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
					return true;
				}
			}
			else if (cell.col < GetFixedColumnCount())
			{
				CPoint start;
				if (GetCellOrigin(cell.row, cell.col, &start) && 
					(pt.y >= start.y && pt.y <= start.y + GetRowHeight(cell.row)))
				{
					AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
					if (pState && IsDrawFocus())
						SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(IDC_CURSOR_ROW)));
					else
						SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
					return true;
				}
			}

            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        }
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
#endif

////////////////////////////////////////////////////////////////////////////////////
// Row/Column count functions

BOOL CGridCtrl::SetFixedRowCount(int nFixedRows)
{
    if (m_nFixedRows == nFixedRows)
        return TRUE;

    ASSERT(nFixedRows >= 0);

    ResetSelectedRange();

    if (nFixedRows > GetRowCount())
        if (!SetRowCount(nFixedRows))
            return FALSE;
        
        if (m_idCurrentCell.row < nFixedRows)
            SetFocusCell(-1, - 1);
        
        if (nFixedRows > m_nFixedRows)
        {
            for (int i = m_nFixedRows; i < nFixedRows; i++)
                for (int j = GetFixedColumnCount(); j < GetColumnCount(); j++)
                {
                    SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED);
                    SetItemBkColour(i, j, CLR_DEFAULT );
                    SetItemFgColour(i, j, CLR_DEFAULT );
                }
        }
        else
        {
            for (int i = nFixedRows; i < m_nFixedRows; i++)
                for (int j = GetFixedColumnCount(); j < GetColumnCount(); j++)
                {
                    SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXED);
                    SetItemBkColour(i, j, CLR_DEFAULT );
                    SetItemFgColour(i, j, CLR_DEFAULT );
                }
        }
        
        m_nFixedRows = nFixedRows;
        
        Refresh();
        
        return TRUE;
}

BOOL CGridCtrl::SetFixedColumnCount(int nFixedCols)
{
    if (m_nFixedCols == nFixedCols)
        return TRUE;

    ASSERT(nFixedCols >= 0);

    ResetSelectedRange();

    if (nFixedCols > GetColumnCount())
        if (!SetColumnCount(nFixedCols))
            return FALSE;

    if (m_idCurrentCell.col < nFixedCols)
        SetFocusCell(-1, - 1);

    if (nFixedCols > m_nFixedCols)
    {
        for (int i = GetFixedRowCount(); i < GetRowCount(); i++)
            for (int j = m_nFixedCols; j < nFixedCols; j++)
            {
                SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED);
                SetItemBkColour(i, j, CLR_DEFAULT );
                SetItemFgColour(i, j, CLR_DEFAULT );
            }
    }
    else
    {
        for (int i = GetFixedRowCount(); i < GetRowCount(); i++)
            for (int j = nFixedCols; j < m_nFixedCols; j++)
            {
                SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXED);
                SetItemBkColour(i, j, CLR_DEFAULT );
                SetItemFgColour(i, j, CLR_DEFAULT );
            }
    }
        
    m_nFixedCols = nFixedCols;
        
    Refresh();
        
    return TRUE;
}

BOOL CGridCtrl::SetRowCount(int nRows)
{
    ASSERT(nRows >= 0);
	if (!DeleteAdditionRow())
		return false;

    if (nRows == GetRowCount() && nRows)
	{
	    if (EnableAdditionRow())
	        return AddAdditionRow();
		return true;
	}

    if (nRows < m_nFixedRows)
        m_nFixedRows = nRows;

    if (m_idCurrentCell.row >= nRows)
        SetFocusCell(-1, - 1);

    int addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the GridCell objects
    // in each column within each row
    if (addedRows < 0)
    {
        for (int row = nRows; row < m_nRows; row++)
        {
            // Delete cells
            for (int col = 0; col < m_nCols; col++)
                DestroyCell(row, col);
            
            // Delete rows
            GRID_ROW* pRow = m_RowData[row];
            if (pRow)
                delete pRow;
        }
    }
    
    // Change the number of rows.
    m_nRows = nRows;
    m_RowData.SetSize(m_nRows);
    m_arRowHeights.SetSize(nRows);

    // If we have just added rows, we need to construct new elements for each cell
    // and set the default row height
    if (addedRows > 0)
    {
        // initialize row heights and data
        int startRow = nRows - addedRows;
        for (int row = startRow; row < GetRowCount(); row++)
        {
            m_arRowHeights[row] = m_nDefCellHeight;
// modified
//			m_arRowHeights[row] = GetSaveHeight(row);
// end of modified
            m_RowData[row] = new GRID_ROW;
            m_RowData[row]->SetSize(m_nCols);
            for (int col = 0; col < m_nCols; col++)
            {
                GRID_ROW* pRow = m_RowData[row];
                if (pRow)
                    pRow->SetAt(col, CreateCell(row, col));
            }
        }
    }
    // else
    //    ResetSelectedRange();
    
    if (EnableAdditionRow())
	{
		if (!AddAdditionRow())
			return false;
	}
    SetModified();
    ResetScrollBars();
    Refresh();
    
// modified
//	UpdateSizes();
// end of modified

    return TRUE;
}

BOOL CGridCtrl::AddAdditionRow()
{
	if (!EnableAdditionRow())
		return false;

	if (ExistsAdditionRow())
	{
		CGridCellBase * pCell = GetCell(m_nRows - 1, 0);
		if (pCell && !pCell->IsKindOf(RUNTIME_CLASS(CAdditionGridCell)))
			SetCellType(m_nRows - 1, 0, RUNTIME_CLASS(CAdditionGridCell));
		return true;
	}

    if (m_nCols < 1)
        SetColumnCount(1);

	if (m_nRows == 0 || (m_nRows > 0 && m_RowData.GetAt(m_nRows - 1) != &m_AdditionRow))
	{
		m_RowData.Add(&m_AdditionRow);
        m_nRows++;
        m_arRowHeights.Add((CELLHEIGHT)GetDefCellHeight());
	    m_AdditionRow.SetSize(m_nCols);

		// Initialise cell data
		for (int col = 0; col < m_nCols; col++)
		{
			m_AdditionRow.SetAt(col, CreateCell(m_RowData.GetSize() - 1, col));
			SetColumnWidth2(col, GetSaveWidth(col));
		}
		// Set row title
		CGridCellBase * pCell = GetCell(m_nRows - 1, 0);
		if (pCell && !pCell->IsKindOf(RUNTIME_CLASS(CAdditionGridCell)))
			SetCellType(m_nRows - 1, 0, RUNTIME_CLASS(CAdditionGridCell));
    }

	return true;
}

BOOL CGridCtrl::DeleteAdditionRow()
{
	if (!ExistsAdditionRow())
		return true;

    for (int col = 0; col < GetColumnCount(); col++)
        DestroyCell(m_nRows - 1, col);

    m_RowData.RemoveAt(m_nRows - 1);
    m_arRowHeights.RemoveAt(m_nRows - 1);

    m_nRows--;
    if (m_nRows < m_nFixedRows)
        m_nFixedRows--;
    
    if (m_nRows == m_idCurrentCell.row)
        m_idCurrentCell.row--;// = m_idCurrentCell.col = -1;
    else if (m_nRows < m_idCurrentCell.row)
        m_idCurrentCell.row--;
    
    ResetScrollBars();
	return true;
}

bool CGridCtrl::ExistsAdditionRow() const
{
	if (m_nRows > 0 && m_RowData.GetAt(m_nRows - 1) == &m_AdditionRow)
		return true;
	return false;
}

int  CGridCtrl::GetAdditionRow() const
{
	if (!ExistsAdditionRow())
		return -1;

	return m_nRows - 1;
}


BOOL CGridCtrl::SetColumnCount(int nCols)
{
    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount())
        return TRUE;

    if (nCols < m_nFixedCols)
        m_nFixedCols = nCols;

    if (m_idCurrentCell.col >= nCols)
        SetFocusCell(-1, - 1);

    int addedCols = nCols - GetColumnCount();

    // If we are about to lose columns, then we need to delete the GridCell objects
    // within each column
    if (addedCols < 0)
    {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < GetColumnCount(); col++)
                DestroyCell(row, col);
    }

    // Change the number of columns.
    m_nCols = nCols;
    m_arColWidths.SetSize(nCols);
    
    // Change the number of columns in each row.
    for (int i = 0; i < m_nRows; i++)
        if (m_RowData[i])
            m_RowData[i]->SetSize(nCols);
        
    // If we have just added columns, we need to construct new elements for each cell
    // and set the default column width
    if (addedCols > 0)
    {
        // initialized column widths
        int startCol = nCols - addedCols;
        for (int col = startCol; col < GetColumnCount(); col++)
		{
			CELLHEIGHT nSize = GetSaveWidth(col);
			if (nSize == (CELLHEIGHT)0)
				nSize = m_nDefCellWidth;
            m_arColWidths[col] = nSize;
			SetSaveWidth(col, nSize);
		}
        
        // initialise column data
        for (int row = 0; row < m_nRows; row++)
            for (col = startCol; col < GetColumnCount(); col++)
            {
                GRID_ROW* pRow = m_RowData[row];
                if (pRow)
                    pRow->SetAt(col, CreateCell(row, col));
            }
    }
    
    SetModified();
    ResetScrollBars();
    Refresh();
    
	RetrieveSizes();

    return TRUE;
}

// Insert a column at a given position, or add to end of columns (if nColumn = -1)
int CGridCtrl::InsertColumn(LPCTSTR strHeading,
                            UINT nFormat /* = DT_CENTER|DT_VCENTER|DT_SINGLELINE */,
                            int nColumn  /* = -1 */)
{
    // If the insertion is for a specific column, check it's within range.
    if (nColumn >= 0 && nColumn >= GetColumnCount())
        return -1;

//	CCellID FocusCell = GetFocusCell();
//    ResetSelectedRange();

    // Gotta be able to at least _see_ some of the column.
    if (m_nRows < 1)
        SetRowCount(1);

    if (nColumn < 0)
    {
        nColumn = m_nCols;
        m_arColWidths.Add((CELLHEIGHT)0);
        for (int row = 0; row < m_nRows; row++)
        {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow)
                return -1;
            pRow->Add(CreateCell(row, nColumn));
// modified
//			SetRowHeight(row, GetSaveHeight(row));
// end of modified
        }
    } 
    else
    {
        m_arColWidths.InsertAt(nColumn, 0.);
        for (int row = 0; row < m_nRows; row++) 
        {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow)
                return -1;
            pRow->InsertAt(nColumn, CreateCell(row, nColumn));
// modified
//			SetRowHeight(row, GetSaveHeight(row));
// end of modified
        }
    }
    
    m_nCols++;
    
    // Initialise column data
    SetItemText(0, nColumn, strHeading);
    for (int row = 0; row < m_nRows; row++) 
		SetItemFormat(row, nColumn, nFormat);
    
    // initialized column width
//    m_arColWidths[nColumn] = GetTextExtent(0, nColumn, strHeading).cx;
// modified
	CELLHEIGHT nSize = 0;
	if (nColumn < m_arrSaveWidths.GetSize()) 
		nSize = GetSaveWidth(nColumn);
	if (nSize == (CELLHEIGHT)0)
	{
		nSize = GetTextExtent(0, nColumn, strHeading).cx;
	}

	if (nSize < GetDefCellWidth())
		nSize = GetDefCellWidth();

    m_arColWidths[nColumn] = nSize;
	SetSaveWidth(nColumn, nSize);
// end of modified
    
    if (m_idCurrentCell.col != -1 && nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col++;
/*
	m_SelectionStartCell = FocusCell;
    if (m_bListMode)
    {
        m_MouseMode = MOUSE_SELECT_ROW;
        OnSelecting(FocusCell.row < GetFixedRowCount() ? CCellID(GetRowCount()-1, FocusCell.col) : FocusCell);
		SetFocusCell(FocusCell);
    }
    else 
    {
        m_MouseMode = MOUSE_SELECT_COL;
        OnSelecting(FocusCell);
		SetFocusCell(FocusCell);
    }
*/	
	InvalidateRect(NULL);
    ResetScrollBars();

    SetModified();
    
    return nColumn;
}

// Insert a row at a given position, or add to end of rows (if nRow = -1)
int CGridCtrl::InsertRow(LPCTSTR strHeading, int nRow /* = -1 */)
{
    // If the insertion is for a specific row, check it's within range.
	if (!DeleteAdditionRow())
		return false;

    if (nRow >= 0 && nRow >= GetRowCount() - 1)
	{
	    if (EnableAdditionRow())
			AddAdditionRow();
        return -1;
	}

//	CCellID FocusCell = GetFocusCell();
//    ResetSelectedRange();

    // Gotta be able to at least _see_ some of the row.
    if (m_nCols < 1)
        SetColumnCount(1);

    // Adding a row to the bottom
    if (nRow < 0)
    {
        nRow = m_nRows;
        m_arRowHeights.Add((CELLHEIGHT)0);
        m_RowData.Add(new GRID_ROW);
    }
    else
    {
        m_arRowHeights.InsertAt(nRow, (CELLHEIGHT)0);
        m_RowData.InsertAt(nRow, new GRID_ROW);
    }

    m_nRows++;
    m_RowData[nRow]->SetSize(m_nCols);

    // Initialise cell data
    for (int col = 0; col < m_nCols; col++)
    {
        GRID_ROW* pRow = m_RowData[nRow];
        if (!pRow)
            return -1;
        pRow->SetAt(col, CreateCell(nRow, col));
    }

    // Set row title
    SetItemText(nRow, 0, strHeading);

    // initialized row height
    m_arRowHeights[nRow] = m_nDefCellHeight;

    if (m_idCurrentCell.row != -1 && nRow < m_idCurrentCell.row)
        m_idCurrentCell.row++;

//	if (FocusCell.row != -1 && nRow < FocusCell.row)
//		m_idCurrentCell.row++;

    if (EnableAdditionRow())
	    AddAdditionRow();
/*
	m_SelectionStartCell = FocusCell;
    if (m_bListMode)
    {
        m_MouseMode = MOUSE_SELECT_ROW;
        OnSelecting(FocusCell.row < GetFixedRowCount() ? CCellID(GetRowCount()-1, FocusCell.col) : FocusCell);
		SetFocusCell(FocusCell);
    }
    else 
    {
        m_MouseMode = MOUSE_SELECT_COL;
        OnSelecting(FocusCell);
		SetFocusCell(FocusCell);
    }
*/	
	InvalidateRect(NULL);
    ResetScrollBars();
    SetModified();

    return nRow;
}

///////////////////////////////////////////////////////////////////////////////
// Cell creation stuff

BOOL CGridCtrl::SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass)
{
    ASSERT(IsValid(nRow, nCol));
    if (!IsValid(nRow, nCol))
        return FALSE;

    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return FALSE;
    }

    CGridCellBase* pNewCell = (CGridCellBase*) pRuntimeClass->CreateObject();

    CGridCellBase* pCurrCell = GetCell(nRow, nCol);
    if (pCurrCell)
        *pNewCell = *pCurrCell;

    SetCell(nRow, nCol, pNewCell);
    delete pCurrCell;

    return TRUE;
}

BOOL CGridCtrl::SetDefaultCellType( CRuntimeClass* pRuntimeClass)
{
    ASSERT( pRuntimeClass != NULL );
    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return FALSE;
    }
    m_pRtcDefault = pRuntimeClass;
    return TRUE;
}

// Creates a new grid cell and performs any necessary initialisation
/*virtual*/ CGridCellBase* CGridCtrl::CreateCell(int nRow, int nCol)
{
    if (!m_pRtcDefault || !m_pRtcDefault->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return NULL;
    }
    CGridCellBase* pCell = (CGridCellBase*) m_pRtcDefault->CreateObject();
    if (!pCell)
        return NULL;

    pCell->SetGrid(this);
    pCell->SetFont(&m_Logfont);    // Make font default grid font
    pCell->SetCoords(nRow, nCol); 

    // Make format same as cell above
    if (nRow > 0 && nCol >= 0 && nCol < m_nCols)
        pCell->SetFormat(GetItemFormat(nRow - 1, nCol));

    if (nCol < m_nFixedCols || nRow < m_nFixedRows)
        pCell->SetState(pCell->GetState() | GVIS_FIXED);

    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
/*virtual*/ void CGridCtrl::DestroyCell(int nRow, int nCol)
{
    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    SetItemState(nRow, nCol, 0);

    delete GetCell(nRow, nCol);
}

BOOL CGridCtrl::DeleteColumn(int nColumn)
{
    if (nColumn < 0 || nColumn >= GetColumnCount())
        return FALSE;

    ResetSelectedRange();

    for (int row = 0; row < GetRowCount(); row++)
    {
        GRID_ROW* pRow = m_RowData[row];
        if (!pRow)
            return FALSE;

        DestroyCell(row, nColumn);
        
        pRow->RemoveAt(nColumn);
    }
    m_arColWidths.RemoveAt(nColumn);
    m_nCols--;
    if (nColumn < m_nFixedCols)
        m_nFixedCols--;
    
    if (nColumn == m_idCurrentCell.col)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col--;
    
    ResetScrollBars();
// modified
	RetrieveSizes();
// end of modified


    SetModified();

    return TRUE;
}

BOOL CGridCtrl::DeleteRow(int nRow)
{
    if (nRow < 0 || nRow > GetRowCount() - 1)
        return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow)
        return FALSE;

    ResetSelectedRange();

    for (int col = 0; col < GetColumnCount(); col++)
        DestroyCell(nRow, col);

    delete pRow;
    m_RowData.RemoveAt(nRow);
    m_arRowHeights.RemoveAt(nRow);

    m_nRows--;
    if (nRow < m_nFixedRows)
        m_nFixedRows--;
    
    if (nRow == m_idCurrentCell.row)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nRow < m_idCurrentCell.row)
        m_idCurrentCell.row--;
    
    ResetScrollBars();
// modified
	RetrieveSizes();
// end of modified


    SetModified();
    
    return TRUE;
}

// Handy function that removes all non-fixed rows
BOOL CGridCtrl::DeleteNonFixedRows()
{
    ResetSelectedRange();

    int nFixed = GetFixedRowCount();
    int nCount = GetRowCount() - 1;

    // Delete all data rows
    for (int nRow = nCount; nRow >= nFixed; nRow--)
        DeleteRow(nRow);

    return TRUE;
}

// Removes all rows, columns and data from the grid.
BOOL CGridCtrl::DeleteAllItems()
{
    ResetSelectedRange();
	if (!DeleteAdditionRow())
		return false;

    m_arColWidths.RemoveAll();
    m_arRowHeights.RemoveAll();

    // Delete all cells in the grid
    for (int row = 0; row < m_nRows; row++)
    {
        for (int col = 0; col < m_nCols; col++)
            DestroyCell(row, col);

        GRID_ROW* pRow = m_RowData[row];
        delete pRow;
    }

    // Remove all rows
    m_RowData.RemoveAll();

    m_idCurrentCell.row = m_idCurrentCell.col = -1;
    m_nRows = m_nFixedRows = m_nCols = m_nFixedCols = 0;

    ResetScrollBars();

    SetModified();

    return TRUE;
}

void CGridCtrl::AutoFill()
{
    if (!::IsWindow(m_hWnd))
        return;

    CRect rect;
    GetClientRect(rect);

    SetColumnCount(rect.Width() / m_nDefCellWidth + 1);
    SetRowCount(rect.Height() / m_nDefCellHeight + 1);
    SetFixedRowCount(1);
    SetFixedColumnCount(1);
    ExpandToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

// Set CListCtrl::GetNextItem for details
CCellID CGridCtrl::GetNextItem(CCellID& cell, int nFlags) const
{
    if ((nFlags & GVNI_ALL) == GVNI_ALL)
    {	// GVNI_ALL Search whole Grid beginning from cell
        //          First row (cell.row) -- ONLY Columns to the right of cell
        //          following rows       -- ALL  Columns
        int row = cell.row , col = cell.col + 1;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++)
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return CCellID(row, col);
            }
            // go to First Column
            col = GetFixedColumnCount();
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW && 
             (nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {   // GVNI_AREA Search Grid beginning from cell to Lower-Right of Grid
        //           Only rows starting with  cell.row and below
        //           All rows   -- ONLY Columns to the right of cell
        int row = cell.row;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            int col = cell.col + 1;
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++) 
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return CCellID(row, col);
            }
        }
    }
    else if ((nFlags & GVNI_ABOVE) == GVNI_ABOVE) 
    {
        for (int row = cell.row - 1; row >= GetFixedRowCount(); row--) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(row, cell.col);
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW)
    {
        for (int row = cell.row + 1; row < GetRowCount(); row++) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(row, cell.col);
        }
    } 
    else if ((nFlags & GVNI_TOLEFT) == GVNI_TOLEFT)
    {
        for (int col = cell.col - 1; col >= GetFixedColumnCount(); col--) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(cell.row, col);
        }
    }
    else if ((nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {
        for (int col = cell.col + 1; col < GetColumnCount(); col++) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(cell.row, col);
        }
    }
    
    return CCellID(-1, -1);
}

// Sorts on a given column using the cell text
BOOL CGridCtrl::SortTextItems(int nCol, BOOL bAscending)
{
    SetSortColumn(nCol);
    SetSortAscending(bAscending);
    ResetSelectedRange();
    SetFocusCell(-1, - 1);
    return SortTextItems(nCol, bAscending, GetFixedRowCount(), - 1);
}

// recursive sort implementation
BOOL CGridCtrl::SortTextItems(int nCol, BOOL bAscending, int low, int high)
{
    if (nCol >= GetColumnCount())
        return FALSE;

    if (high == -1)
        high = GetRowCount() - 1;

    int lo = low;
    int hi = high;

    if (hi <= lo)
        return FALSE;
    
    CString midItem = GetItemText((lo + hi)/2, nCol);
    
    // loop through the list until indices cross
    while (lo <= hi)
    {
        // Find the first element that is greater than or equal to the partition 
        // element starting from the left Index.
        if (bAscending)
            while (lo < high  && GetItemText(lo, nCol) < midItem)
                ++lo;
            else
                while (lo < high && GetItemText(lo, nCol) > midItem)
                    ++lo;
                
                // Find an element that is smaller than or equal to  the partition 
                // element starting from the right Index.
                if (bAscending)
                    while (hi > low && GetItemText(hi, nCol) > midItem)
                        --hi;
                    else
                        while (hi > low && GetItemText(hi, nCol) < midItem)
                            --hi;
                        
                        // If the indexes have not crossed, swap if the items are not equal
                        if (lo <= hi)
                        {
                            // swap only if the items are not equal
                            if (GetItemText(lo, nCol) != GetItemText(hi, nCol))
                            {
                                for (int col = 0; col < GetColumnCount(); col++)
                                {
                                    CGridCellBase *pCell = GetCell(lo, col);
                                    SetCell(lo, col, GetCell(hi, col));
                                    SetCell(hi, col, pCell);
                                }
                                CELLHEIGHT nRowHeight = m_arRowHeights[lo];
                                m_arRowHeights[lo] = m_arRowHeights[hi];
                                m_arRowHeights[hi] = nRowHeight;
                            }
                            
                            ++lo;
                            --hi;
                        }
    }
    
    // If the right index has not reached the left side of array
    // must now sort the left partition.
    if (low < hi)
        SortTextItems(nCol, bAscending, low, hi);
    
    // If the left index has not reached the right side of array
    // must now sort the right partition.
    if (lo < high)
        SortTextItems(nCol, bAscending, lo, high);
    
    return TRUE;
}

// Sorts on a given column using the supplied compare function (see CListCtrl::SortItems)
BOOL CGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending,
                          LPARAM data /* = 0 */)
{
    SetSortColumn(nCol);
    SetSortAscending(bAscending);
    ResetSelectedRange();
    SetFocusCell(-1, - 1);
    return SortItems(pfnCompare, nCol, bAscending, data, GetFixedRowCount(), -1);
}

// recursive sort implementation
BOOL CGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                          int low, int high)
{
    if (nCol >= GetColumnCount())
        return FALSE;

    if (high == -1)
        high = GetRowCount() - 1;

    int lo = low;
    int hi = high;
    
    if (hi <= lo)
        return FALSE;
    
    LPARAM midItem = GetItemData((lo + hi)/2, nCol);
    
    // loop through the list until indices cross
    while (lo <= hi)
    {
        // Find the first element that is greater than or equal to the partition 
        // element starting from the left Index.
        if (bAscending)
            while (lo < high  && pfnCompare(GetItemData(lo, nCol), midItem, data) < 0)
                ++lo;
            else
                while (lo < high && pfnCompare(GetItemData(lo, nCol), midItem, data) > 0)
                    ++lo;
                
                // Find an element that is smaller than or equal to  the partition 
                // element starting from the right Index.
                if (bAscending)
                    while (hi > low && pfnCompare(GetItemData(hi, nCol), midItem, data) > 0)
                        --hi;
                    else
                        while (hi > low && pfnCompare(GetItemData(hi, nCol), midItem, data) < 0)
                            --hi;
                        
                        // If the indexes have not crossed, swap if the items are not equal
                        if (lo <= hi)
                        {
                            // swap only if the items are not equal
                            if (pfnCompare(GetItemData(lo, nCol), GetItemData(hi, nCol), data) != 0)
                            {
                                for (int col = 0; col < GetColumnCount(); col++)
                                {
                                    CGridCellBase *pCell = GetCell(lo, col);
                                    SetCell(lo, col, GetCell(hi, col));
                                    SetCell(hi, col, pCell);
                                }
                                CELLHEIGHT nRowHeight = m_arRowHeights[lo];
                                m_arRowHeights[lo] = m_arRowHeights[hi];
                                m_arRowHeights[hi] = nRowHeight;
                            }
                            
                            ++lo;
                            --hi;
                        }
    }
    
    // If the right index has not reached the left side of array
    // must now sort the left partition.
    if (low < hi)
        SortItems(pfnCompare, nCol, bAscending, data, low, hi);
    
    // If the left index has not reached the right side of array
    // must now sort the right partition.
    if (lo < high)
        SortItems(pfnCompare, nCol, bAscending, data, lo, high);
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

BOOL CGridCtrl::SetItem(const GV_ITEM* pItem)
{
    if (!pItem)
        return FALSE;

    CGridCellBase* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell)
        return FALSE;

    SetModified(pItem->row, pItem->col);

    if (pItem->mask & GVIF_TEXT)
        pCell->SetText(pItem->strText);
    if (pItem->mask & GVIF_PARAM)
        pCell->SetData(pItem->lParam);
    if (pItem->mask & GVIF_IMAGE)
        pCell->SetImage(pItem->iImage);
    if (pItem->mask & GVIF_STATE)
        pCell->SetState(pItem->nState);
    if (pItem->mask & GVIF_FORMAT)
        pCell->SetFormat(pItem->nFormat);
    if (pItem->mask & GVIF_BKCLR)
        pCell->SetBackClr(pItem->crBkClr);
    if (pItem->mask & GVIF_FGCLR)
        pCell->SetTextClr(pItem->crFgClr);
    if (pItem->mask & GVIF_FONT)
        pCell->SetFont(&(pItem->lfFont));
    if( pItem->mask & GVIF_MARGIN)
        pCell->SetMargin( pItem->nMargin);
    
    return TRUE;
}

BOOL CGridCtrl::GetItem(GV_ITEM* pItem)
{
    if (!pItem)
        return FALSE;
    CGridCellBase* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell)
        return FALSE;

    if (pItem->mask & GVIF_TEXT)
        pItem->strText = GetItemText(pItem->row, pItem->col);
    if (pItem->mask & GVIF_PARAM)
        pItem->lParam  = pCell->GetData();;
    if (pItem->mask & GVIF_IMAGE)
        pItem->iImage  = pCell->GetImage();
    if (pItem->mask & GVIF_STATE)
        pItem->nState  = pCell->GetState();
    if (pItem->mask & GVIF_FORMAT)
        pItem->nFormat = pCell->GetFormat();
    if (pItem->mask & GVIF_BKCLR)
        pItem->crBkClr = pCell->GetBackClr();
    if (pItem->mask & GVIF_FGCLR)
        pItem->crFgClr = pCell->GetTextClr();
    if (pItem->mask & GVIF_FONT)
        memcpy(&(pItem->lfFont), pCell->GetFont(), sizeof(LOGFONT));
    if( pItem->mask & GVIF_MARGIN)
        pItem->nMargin = pCell->GetMargin();

    return TRUE;
}

BOOL CGridCtrl::SetItemText(int nRow, int nCol, LPCTSTR str)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetText(str);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

#if (_WIN32_WCE >= 210)
// EFW - 06/13/99 - Added to support printf-style formatting codes
BOOL CGridCtrl::SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...)
{
    CString strText;

    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    // Format the message text
    va_start(argptr, szFmt);
    strText.FormatV(szFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

// EFW - 06/13/99 - Added to support string resource ID.  Supports
// a variable argument list too.
BOOL CGridCtrl::SetItemTextFmtID(int nRow, int nCol, UINT nID, ...)
{
    CString strFmt, strText;
    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    // Format the message text
    va_start(argptr, nID);
    VERIFY(strFmt.LoadString(nID));
    strText.FormatV(strFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}
#endif

BOOL CGridCtrl::SetItemData(int nRow, int nCol, LPARAM lParam)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetData(lParam);
    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

LPARAM CGridCtrl::GetItemData(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return (LPARAM) 0;

    return pCell->GetData();
}

BOOL CGridCtrl::SetItemImage(int nRow, int nCol, int iImage)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetImage(iImage);
    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

int CGridCtrl::GetItemImage(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return -1;

    return pCell->GetImage();
}

BOOL CGridCtrl::SetItemState(int nRow, int nCol, UINT state)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    // If the cell is being unselected, remove it from the selected list
    if (IsCellSelected(nRow, nCol) && !(state & GVIS_SELECTED))
		m_Sel -= CCellID(nRow, nCol);

    // If cell is being selected, add it to the list of selected cells
    else if (!IsCellSelected(nRow, nCol) && (state & GVIS_SELECTED))
		m_Sel += CCellID(nRow, nCol);

    // Set the cell's state
    pCell->SetState(state);

    return TRUE;
}

UINT CGridCtrl::GetItemState(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

	UINT uState = pCell->GetState();
	if (m_Sel.InRange(CCellID(nRow, nCol)))
		uState |= GVIS_SELECTED; 
	else 
		uState &= ~GVIS_SELECTED; 
    return uState;
}

BOOL CGridCtrl::SetItemFormat(int nRow, int nCol, UINT nFormat)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    pCell->SetFormat(nFormat);
    return TRUE;
}

UINT CGridCtrl::GetItemFormat(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetFormat();
}

BOOL CGridCtrl::SetItemBkColour(int nRow, int nCol, COLORREF cr /* = CLR_DEFAULT */)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    pCell->SetBackClr(cr);
    return TRUE;
}

COLORREF CGridCtrl::GetItemBkColour(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetBackClr();
}

BOOL CGridCtrl::SetItemFgColour(int nRow, int nCol, COLORREF cr /* = CLR_DEFAULT */)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;
    
    pCell->SetTextClr(cr);
    return TRUE;
}

COLORREF CGridCtrl::GetItemFgColour(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;
    
    return pCell->GetTextClr();
}

BOOL CGridCtrl::SetItemFont(int nRow, int nCol, const LOGFONT* plf)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;
    
    pCell->SetFont(plf);
    
    return TRUE;
}

const LOGFONT* CGridCtrl::GetItemFont(int nRow, int nCol)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) 
        return &m_Logfont;
    
    return pCell->GetFont();
}

BOOL CGridCtrl::IsItemEditing(int nRow, int nCol)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    return pCell->IsEditing();
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column size functions

CELLHEIGHT CGridCtrl::GetVirtualWidth() const
{
    CELLHEIGHT lVirtualWidth = 0;
    int iColCount = GetColumnCount();
    for (int i = 0; i < iColCount; i++)
        lVirtualWidth += m_arColWidths[i];

    return lVirtualWidth;
}

CELLHEIGHT CGridCtrl::GetVirtualHeight() const
{
    CELLHEIGHT lVirtualHeight = 0;
    int iRowCount = GetRowCount();
    for (int i = 0; i < iRowCount; i++)
        lVirtualHeight += m_arRowHeights[i];

    return lVirtualHeight;
}

int CGridCtrl::GetRowHeight(int nRow) const
{
    //ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1;

    return (int)m_arRowHeights[nRow];
}

int CGridCtrl::GetColumnWidth(int nCol) const
{
    //ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1;

    return (int)m_arColWidths[nCol];
}

BOOL CGridCtrl::SetRowHeight(int nRow, int height)
{
    ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0)
        return FALSE;

    m_arRowHeights[nRow] = height;
// modified
//	SetSaveHeight(nRow, height);
// end of modified
    ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::SetColumnWidth(int nCol, int width)
{
    ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0)
        return FALSE;

    m_arColWidths[nCol] = width;
// modified
	SetSaveWidth(nCol, width);
// end of modified
    ResetScrollBars();

    return TRUE;
}

CELLHEIGHT CGridCtrl::GetRowHeight2(int nRow) const
{
    //ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1.;

    return m_arRowHeights[nRow];
}

CELLHEIGHT CGridCtrl::GetColumnWidth2(int nCol) const
{
    //ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1.;

    return m_arColWidths[nCol];
}

BOOL CGridCtrl::SetRowHeight2(int nRow, CELLHEIGHT height)
{
    ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0)
        return FALSE;

    m_arRowHeights[nRow] = height;
// modified
//	SetSaveHeight(nRow, height);
// end of modified
    ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::SetColumnWidth2(int nCol, CELLHEIGHT width)
{
    ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0)
        return FALSE;

    m_arColWidths[nCol] = width;
// modified
	SetSaveWidth(nCol, width);
// end of modified
    ResetScrollBars();

    return TRUE;
}

int CGridCtrl::GetFixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++)
        nHeight += GetRowHeight(i);

    return nHeight;
}

int CGridCtrl::GetFixedColumnWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++)
        nWidth += GetColumnWidth(i);

    return nWidth;
}

BOOL CGridCtrl::AutoSizeColumn(int nCol, BOOL bIgnoreHeader /*=FALSE*/)
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return FALSE;

    //  Skip hidden columns when autosizing
    if( GetColumnWidth( nCol) <=0 )
        return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return FALSE;

    int nWidth = 0;
    int nNumRows = GetRowCount();

    int iStartRow = 0;
    if( bIgnoreHeader)
        iStartRow = GetFixedRowCount();

    for (int nRow = iStartRow; nRow < nNumRows; nRow++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);
        if (size.cx > nWidth)
            nWidth = size.cx;
    }

    m_arColWidths[nCol] = nWidth;

    ReleaseDC(pDC);
// modified
	SetSaveWidth(nCol, nWidth);
// end of modified
    ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::AutoSizeRow(int nRow)
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return FALSE;

    //  Skip hidden rows when autosizing
    if( GetRowHeight( nRow) <=0 )
        return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return FALSE;

    int nHeight = 0;
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);
        if (size.cy > nHeight)
            nHeight = size.cy;
    }
    m_arRowHeights[nRow] = nHeight;

    ReleaseDC(pDC);
// modified
//	SetSaveHeight(nRow, nHeight);
// end of modified
    ResetScrollBars();

    return TRUE;
}

void CGridCtrl::AutoSizeColumns()
{
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Skip hidden columns when autosizing
        if( GetColumnWidth( nCol) > 0 )
            AutoSizeColumn(nCol);
    }
}

void CGridCtrl::AutoSizeRows()
{
    int nNumRows = GetRowCount();
    for (int nRow = 0; nRow < nNumRows; nRow++)
    {
        //  Skip hidden rows when autosizing
        if( GetRowHeight( nRow) > 0 )
            AutoSizeRow(nRow);
    }
}

// sizes all rows and columns
// faster than calling both AutoSizeColumns() and AutoSizeRows()
void CGridCtrl::AutoSize(BOOL bIgnoreColumnHeader)
{
    CDC* pDC = GetDC();
    if (!pDC)
        return;

    int nNumColumns = GetColumnCount();
    int nNumRows = GetRowCount();

    int nCol, nRow;

    int iStartRow = 0;
    if( bIgnoreColumnHeader)
        iStartRow = GetFixedRowCount();

    CSize size;
    for (nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Don't size hidden columns or rows
        if( GetColumnWidth( nCol) > 0 )
        {
            // Skip columns that are hidden, but now initialize
            m_arColWidths[nCol] = 0;
            for (nRow = iStartRow; nRow < nNumRows; nRow++)
            {
                if( GetRowHeight( nRow) > 0 )
                {
                    // Skip rows that are hidden, but now initialize
//                    m_arRowHeights[nRow] = 0;

                    CGridCellBase* pCell = GetCell(nRow, nCol);
                    if (pCell)
                        size = pCell->GetCellExtent(pDC);
                    if (size.cx >(int) m_arColWidths[nCol])
                        m_arColWidths[nCol] = size.cx;
					SetSaveWidth(nCol, m_arColWidths[nCol]);
                    if (size.cy >(int) m_arRowHeights[nRow])
                        m_arRowHeights[nRow] = size.cy;
                }
				else
					m_arRowHeights[nRow] = m_nResizeCaptureRange;

            }
        }
    }
    ReleaseDC(pDC);

    ResetScrollBars();
    if (m_bAllowDraw)
        Invalidate();
}

void CGridCtrl::ExpandColumnsToFit()
{
    if (GetColumnCount() <= 0)
        return;

    EnableScrollBars(SB_BOTH, FALSE);

    CRect rect;
    GetClientRect(rect);

    CELLHEIGHT virtualWidth = GetVirtualWidth();
    int nDifference = rect.Width() -(int) virtualWidth;
    int nColumnAdjustment = nDifference / GetColumnCount();

    for (int i = 0; i < GetColumnCount(); i++)
	{
        m_arColWidths[i] += nColumnAdjustment;
		SetSaveWidth(i, m_arColWidths[i]);
	}

    if (nDifference > 0)
    {
        int leftOver = nDifference % GetColumnCount();
        for (i = 0; i < leftOver; i++)
		{
            m_arColWidths[i] += 1;
			SetSaveWidth(i, m_arColWidths[i]);
		}
    }
    else 
    {
        int leftOver = (-nDifference) % GetColumnCount();
        for (i = 0; i < leftOver; i++)
		{
            m_arColWidths[i] -= 1;
			SetSaveWidth(i, m_arColWidths[i]);
		}
    }

    if (m_bAllowDraw)
        Invalidate();
}

void CGridCtrl::ExpandRowsToFit()
{
    if (GetRowCount() <= 0)
        return;

    EnableScrollBars(SB_BOTH, FALSE); 

    CRect rect;
    GetClientRect(rect);
    
    long virtualHeight = (long)GetVirtualHeight();
    int nDifference = rect.Height() -(int) virtualHeight;
    int nRowAdjustment = nDifference / GetRowCount();
    
    for (int i = 0; i < GetRowCount(); i++)
	{
        m_arRowHeights[i] += nRowAdjustment;
	}
    
    if (nDifference > 0)
    {
        int leftOver = nDifference % GetRowCount();
        for (i = 0; i < leftOver; i++)
		{
            m_arRowHeights[i] += 1;
		}
    } 
    else 
    {
        int leftOver = (-nDifference) % GetRowCount();
        for (i = 0; i < leftOver; i++)
		{
            m_arRowHeights[i] -= 1;
		}
    }

    if (m_bAllowDraw)
        Invalidate();
}

void CGridCtrl::ExpandToFit()
{
    ExpandColumnsToFit();   // This will remove any existing horz scrollbar
    ExpandRowsToFit();      // This will remove any existing vert scrollbar
    ResetScrollBars();
    ExpandColumnsToFit();   // Just in case the first adjustment was with a vert
                            // scrollbar in place
    Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////
// Attributes

void CGridCtrl::SetListMode(BOOL bEnableListMode /*=TRUE*/)
{
    ResetSelectedRange();
    SetSortColumn(-1);
    m_bListMode = bEnableListMode;
    Invalidate();
}

BOOL CGridCtrl::IsCellFixed(int nRow, int nCol)
{
    return (nRow < GetFixedRowCount() || nCol < GetFixedColumnCount());
}

void CGridCtrl::SetModified(BOOL bModified /*=TRUE*/, int nRow /*=-1*/, int nCol /*=-1*/)
{
    SetSortColumn(-1);

    if (nRow > 0 && nCol > 0)
    {
        if (bModified)
        {
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) | GVIS_MODIFIED);
            m_bModified = TRUE;
        }
        else
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) & ~GVIS_MODIFIED);
    }
    else
        m_bModified = bModified;

    if (!m_bModified)
    {
        for (int row = 0; row < GetRowCount(); row++)
            for (int col = 0; col < GetColumnCount(); col++)
                SetItemState(row, col, GetItemState(row, col) & ~GVIS_MODIFIED);
    }
}

BOOL CGridCtrl::GetModified(int nRow /*=-1*/, int nCol /*=-1*/)
{
    if (nRow > 0 && nCol > 0)
        return ( (GetItemState(nRow, nCol) & GVIS_MODIFIED) == GVIS_MODIFIED );
    else
        return m_bModified;
}

/////////////////////////////////////////////////////////////////////////////////////
// GridCtrl cell visibility tests and invalidation/redraw functions

void CGridCtrl::Refresh()
{
    if (GetSafeHwnd() && m_bAllowDraw)
        Invalidate();
}

// EnsureVisible supplied by Roelf Werkman
void CGridCtrl::EnsureVisible(int nRow, int nCol)
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;

	CRect rectWindow;
	GetClientRect(rectWindow);

	CRect VisibleRect(0, 0, 0, 0);
    CCellRange VisibleCells = GetVisibleNonFixedCellRange(VisibleRect);
	CCellRange FullVisibleCells = GetUnobstructedNonFixedCellRange();

	if (FullVisibleCells.InRange(nRow, nCol))
	{
		// already visible
		return;
	}
	
	CCellID cellBottomRight = FullVisibleCells.GetBottomRight();
	CCellID cellTopLeft = FullVisibleCells.GetTopLeft();	
	
	if(GetFixedColumnCount()>=nCol)
		nCol = cellBottomRight.col;
	

	CRect ItemRect;
	if (!GetCellRect(nRow, nCol, ItemRect) || ItemRect.IsRectEmpty())
		return;




	// get rects for top-left and bottom-right cells
	CRect TLRect(-1,-1,-1,-1);
	CRect BRRect(-1,-1,-1,-1);

	if (!GetCellRect(cellTopLeft.row, cellTopLeft.col, TLRect))
		return;
	if (TLRect==CRect(-1,-1,-1,-1))
		return;

	if (!GetCellRect(cellBottomRight.row, cellBottomRight.col, BRRect))
		return;
	if (BRRect==CRect(-1,-1,-1,-1))
		return;

	// calc offset by X axis
	int nOffsetX = 0;
	// if given cell is on left from top-left
	if (ItemRect.left < TLRect.left)
	{
		// offset by X is simple difference b/w lefts of both cells
		nOffsetX = ItemRect.left - TLRect.left;
	}
	else if (ItemRect.right > BRRect.right) // given cell is on right from bottom-right
	{
		// A.M. fix BT5171. VisibleRect includes fixed rows and columns.
		// Should compare with only free area in VisibleRect.
		if (ItemRect.Width() >= (VisibleRect.Width()-GetFixedColumnWidth()))
		{
			nOffsetX = ItemRect.left - TLRect.left;
		}
		else
		{
			//[AY]
			//int nW = ItemRect.right - TLRect.left;
			//nW -= (rectWindow.right - TLRect.left);

			int nW = ItemRect.right - TLRect.right;
			nW -= (rectWindow.right - TLRect.right);
			
			int nCnt = cellTopLeft.col;
			int nMaxCol = GetColumnCount();
			while (nW > 0 && nCnt < nMaxCol)
			{
				int nD = GetColumnWidth(nCnt++);
				nW -= nD;
				nOffsetX += nD;
			}
		}
	}

	// calc offset by Y axis
	int nOffsetY = 0;

	// if given cell is on left from top-left
	if (ItemRect.top < TLRect.top)
	{
		// offset by Y is simple difference b/w tops of both cells
		nOffsetY = ItemRect.top - TLRect.top;
	}
	else if (ItemRect.bottom > BRRect.bottom) // given cell is on bottom from bottom-right
	{
		// A.M. fix BT5171. VisibleRect includes fixed rows and columns.
		// Should compare with only free area in VisibleRect.
		if (ItemRect.Height() >= VisibleRect.Height()-GetFixedRowHeight())
		{
			nOffsetY = ItemRect.top - TLRect.top;
		}
		else
		{
			//[AY]
			//int nH = ItemRect.bottom - TLRect.top;
			//nH -= (rectWindow.bottom - TLRect.top);
			int nH = ItemRect.bottom - TLRect.bottom;
			nH -= (rectWindow.bottom - TLRect.bottom);
			int nCnt = cellTopLeft.row;
			int nMaxRow = GetRowCount();
			while (nH > 0 && nCnt < nMaxRow)
			{
				int nD = GetRowHeight(nCnt++);
				nH -= nD;
				nOffsetY += nD;
			}
		}
	}


///////////////////////////////////////////
	int nCurPosX = GetScrollPos32(SB_HORZ);
	int nNewPosX = nCurPosX + nOffsetX;
	int nMinX, nMaxX;

	GetScrollRange(SB_HORZ, &nMinX, &nMaxX);

	if (nNewPosX < nMinX)
		nNewPosX = nMinX;

	if (nNewPosX > nMaxX)
		nNewPosX = nMaxX;

	if (nNewPosX != nCurPosX)
	{
		SetScrollPos(SB_HORZ, nNewPosX, true);
//		SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nNewPosX), 0);
	}


	int nCurPosY = GetScrollPos32(SB_VERT);
	int nNewPosY = nCurPosY + nOffsetY;
	int nMinY, nMaxY;

	GetScrollRange(SB_VERT, &nMinY, &nMaxY);

	if (nNewPosY < nMinY)
		nNewPosY = nMinY;

	if (nNewPosY > nMaxY)
		nNewPosY = nMaxY;

	if (nNewPosY != nCurPosY)
	{
		SetScrollPos(SB_VERT, nNewPosY, true);
//		SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nNewPosY), 0);
	}
	Invalidate();
//	UpdateWindow();
///////////////////////////////////////////
/*
    // Move one more if we only see a snall bit of the cell
    CRect rectCell;
    GetCellRect(nRow, nCol, rectCell);

    // The previous fix was fixed properly by Martin Richter <martin.richter@grutzeck.de>
	if (rectCell.Width() < rectWindow.Width() - GetFixedColumnWidth())
	{
		while (rectCell.right > rectWindow.right)
		{
			SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
			GetCellRect(nRow, nCol, rectCell);
		}
	}

	if (rectCell.Height() < rectWindow.Height() - GetFixedRowHeight())
	{
		while (rectCell.bottom > rectWindow.bottom)
		{
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
			GetCellRect(nRow, nCol, rectCell);
		}
	}
*/
}

BOOL CGridCtrl::IsCellEditable(CCellID &cell) const
{
    return IsCellEditable(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellEditable(int nRow, int nCol) const
{
    return IsEditable() && ((GetItemState(nRow, nCol) & GVIS_READONLY) != GVIS_READONLY);
}

BOOL CGridCtrl::IsCellSelected(CCellID &cell) const
{
    return IsCellSelected(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellSelected(int nRow, int nCol) const
{
    return IsSelectable() && m_Sel.InRange(CCellID(nRow, nCol));
}

BOOL CGridCtrl::IsCellVisible(CCellID cell) const
{
    return IsCellVisible(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellVisible(int nRow, int nCol) const
{
    if (!IsWindow(m_hWnd))
        return FALSE;

    int x, y;

    CCellID TopLeft;
    if (nCol >= GetFixedColumnCount() || nRow >= GetFixedRowCount())
    {
        TopLeft = GetTopleftNonFixedCell();
        if (nCol >= GetFixedColumnCount() && nCol < TopLeft.col)
            return FALSE;
        if (nRow >= GetFixedRowCount() && nRow < TopLeft.row)
            return FALSE;
    }
    
    CRect rect;
    GetClientRect(rect);
    if (nCol < GetFixedColumnCount())
    {
        x = 0;
        for (int i = 0; i <= nCol; i++) 
        {
//            x += GetColumnWidth(i);    
            if (x >= rect.right)
                return FALSE;
            x += GetColumnWidth(i);    
        }
    } 
    else 
    {
        x = GetFixedColumnWidth();
        for (int i = TopLeft.col; i <= nCol; i++) 
        {
//            x += GetColumnWidth(i);    
            if (x >= rect.right)
                return FALSE;
            x += GetColumnWidth(i);    
        }
    }
    
    if (nRow < GetFixedRowCount())
    {
        y = 0;
        for (int i = 0; i <= nRow; i++) 
        {
//            y += GetRowHeight(i);    
            if (y >= rect.bottom)
                return FALSE;
            y += GetRowHeight(i);    
        }
    } 
    else 
    {
        if (nRow < TopLeft.row)
            return FALSE;
        y = GetFixedRowHeight();
        for (int i = TopLeft.row; i <= nRow; i++) 
        {
//            y += GetRowHeight(i);    
            if (y >= rect.bottom)
                return FALSE;
            y += GetRowHeight(i);    
        }
    }
    
    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellID& cell)
{
    return InvalidateCellRect(cell.row, cell.col);
}

BOOL CGridCtrl::InvalidateCellRect(const int row, const int col)
{
    if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
        return FALSE;

    //ASSERT(IsValid(row, col));

	//paul 17.09.2001. Remove assert. Reason: So more ASSERT DLG.Change with next lies: 
	if( !IsValid(row, col) )
	{
		TRACE( "\nWarning.Bad col, row on InvalidateCellRect." );
		return false;
	}

    if (!IsCellVisible(row, col))
        return FALSE;

    CRect rect;
    if (!GetCellRect(row, col, rect))
        return FALSE;
    rect.right++;
    rect.bottom++;
    InvalidateRect(rect, TRUE);

    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellRange& cellRange)
{
    //ASSERT(IsValid(cellRange));

	//paul 17.09.2001. Remove assert. Reason: So more ASSERT DLG.Change with next lies: 
	if( !IsValid(cellRange) )
	{
		TRACE( "\nWarning.Bad cellRange on InvalidateCellRect." );
		return false;
	}

    if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
        return FALSE;

    CCellRange visibleCellRange = GetVisibleNonFixedCellRange().Intersect(cellRange);
    CCellRange visibleFixedColumn = GetVisibleFixedColumnRange().Intersect(cellRange);
    CCellRange visibleFixedRow = GetVisibleFixedRowRange().Intersect(cellRange);

    CRect rect;
    if (!GetCellRangeRect(visibleCellRange, rect))
        return FALSE;

	CRect FixedColRect;
    if (GetCellRangeRect(visibleFixedColumn, FixedColRect))
		rect.UnionRect(rect, FixedColRect);

    CRect FixedRowRect;
    if (GetCellRangeRect(visibleFixedRow, FixedRowRect))
		rect.UnionRect(rect, FixedRowRect);

	rect.right++;
    rect.bottom++;
    InvalidateRect(rect, TRUE);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl Mouse stuff

// Handles mouse wheel notifications
// Note - if this doesn't work for win95 then use OnRegisteredMouseWheel instead
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
BOOL CGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    if (m_nRowsPerWheelNotch == -1)
    {
        int nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0)
            for (int i = 0; i < nPagesScrolled; i++)
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
        else
            for (int i = 0; i > nPagesScrolled; i--)
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0)
            for (int i = 0; i < nRowsScrolled; i++)
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
        else
            for (int i = 0; i > nRowsScrolled; i--)
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
#endif // !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)

void CGridCtrl::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;
#endif

	if (m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		m_ptMove = point;
		if (m_ptMove.x < GetFixedColumnWidth())
			m_ptMove.x = GetFixedColumnWidth();

		if (m_ptMove.x > rect.right)
			m_ptMove.x = rect.right;

		if (m_pDragList)
		{
			m_pDragList->DragMove(CPoint(m_ptMove.x, m_ptDragOffset.y + 2));
			// get column
			CCellID cell = GetCellFromPt(point);
			int nOldCol = m_nDragOverColumn;
			bool bOldRight = m_bPrevRight;

			if (EnableColumnDrag(cell.col))
				m_nDragOverColumn = cell.col;
			
			CPoint ptOrig;
			GetCellOrigin(0, m_nDragOverColumn, &ptOrig);
			int nC = ptOrig.x + GetColumnWidth(m_nDragOverColumn) / 2;
			m_bPrevRight = nC < point.x ? true : false;

			if (nOldCol != m_nDragOverColumn || bOldRight != m_bPrevRight)
			{
				if (!(abs(nOldCol - m_nDragOverColumn) == 1 && bOldRight != m_bPrevRight))
				{
					DrawMarker(nOldCol, bOldRight, false);
					DrawMarker(m_nDragOverColumn, m_bPrevRight, true);
				}
			}
		}
		return;
	}

    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message hs been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bMouseButtonDown
    // BOOL bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);


    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bMouseButtonDown ||
        (m_bMouseButtonDown && m_MouseMode == MOUSE_NOTHING))
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
				UINT uCursor = IDC_CURSOR_WIDTH;

				CPoint start;
				if (GetCellOrigin(0, m_LeftClickDownCell.col, &start) && 
					(point.x - start.x <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(0, m_LeftClickDownCell.col - 1, &start))
					{
						if (GetColumnWidth(m_LeftClickDownCell.col - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_COLUMN_SPLIT;
					}
				}
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
				UINT uCursor = IDC_CURSOR_HEIGHT;

				CPoint start;
				if (GetCellOrigin(m_LeftClickDownCell.row, 0, &start) && 
					(point.y - start.y <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(m_LeftClickDownCell.row - 1, 0, &start))
					{
						if (GetRowHeight(m_LeftClickDownCell.row - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_ROW_SPLIT;
					}
				}
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
#ifndef _WIN32_WCE_NO_CURSOR
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING)
        {
            CCellID idCurrentCell = GetCellFromPt(point);
            
			CRect CellRect;
			GetCellRect(idCurrentCell, &CellRect);
            // Let the cell know that a big fat cursor is currently hovering
            // over it.
            CGridCellBase* pCell = GetCell(idCurrentCell.row, idCurrentCell.col);
			bool bPtrInRect = CellRect.PtInRect(point) == TRUE;
			if (m_OverCell != idCurrentCell || !bPtrInRect)
			{
				if (IsValid(m_OverCell))
				{
					CGridCellBase* pOldCell = GetCell(m_OverCell.row, m_OverCell.col);
					if (pOldCell)
						pOldCell->OnMouseLeave(m_OverCell.row, m_OverCell.col, point);
				}

				m_OverCell = bPtrInRect ? idCurrentCell : CCellID(-1, -1);
				if (pCell && bPtrInRect)
					pCell->OnMouseEnter(m_OverCell.row, m_OverCell.col, point);
			}
            if (pCell && bPtrInRect)
                pCell->OnMouseOver(m_OverCell.row, m_OverCell.col, point);

#ifndef GRIDCONTROL_NO_TITLETIPS
            // Titletips anyone? anyone?
            if (m_bTitleTips)
            {
//                CRect TextRect, CellRect;
                CRect TextRect = CellRect;
                CGridCellBase* pCell = GetCell(idCurrentCell.row, idCurrentCell.col);
                if (pCell)
                {
                    LPCTSTR szTipText = pCell->GetTipText();
                    if (szTipText && szTipText[0]
                        && !pCell->IsEditing()
                        /*&& GetCellRect( idCurrentCell.row, idCurrentCell.col, &TextRect)*/
                        && pCell->GetTipTextRect( &TextRect)
						&& !TextRect.IsRectEmpty()
                        /*&& GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect)*/ )
                    {
						int nOffset = 0;
//						if ((pCell->GetState() & GVIS_FIXED) == GVIS_FIXED)
//						{
//							nOffset = point.x - CellRect.left;
//						}
						GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect);
						if( CellRect.PtInRect( point ) )
	                        m_TitleTip.Show(TextRect, pCell->GetTipText(), nOffset, CellRect,
		                                pCell->GetFont(),  GetTitleTipTextClr(), GetTitleTipBackClr());
                    }
                }
            }
#endif
        }

        m_LastMousePoint = point;
        return;
    }

    if (!IsValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (m_bMouseButtonDown)
    {
        switch (m_MouseMode)
        {
        case MOUSE_SELECT_ALL:
            break;

        case MOUSE_SELECT_COL:
        case MOUSE_SELECT_ROW:
        case MOUSE_SELECT_CELLS:
            {
                CCellID idCurrentCell = GetCellFromPt(point);
                if (!IsValid(idCurrentCell))
                    return;

				CCellID FocusCell = GetFocusCell();

                if (idCurrentCell != FocusCell)
                {
                    OnSelecting(idCurrentCell);

                    // EFW - BUG FIX - Keep the appropriate cell row and/or
                    // column focused.  A fix in SetFocusCell() will place
                    // the cursor in a non-fixed cell as needed.
                    if ((idCurrentCell.row >= m_nFixedRows &&
						 idCurrentCell.col >= m_nFixedCols))
                    {

                        SetFocusCell(idCurrentCell);
                    }
					else if (m_MouseMode == MOUSE_SELECT_COL)
					{
						SetFocusCell(FocusCell.row, idCurrentCell.col);
//						SetFocusCell(/*idCurrentCell);//*/GetFirstVisibleCell(-1, idCurrentCell.col));
					}
                    else if (m_MouseMode == MOUSE_SELECT_ROW)
					{
						SetFocusCell(idCurrentCell.row, FocusCell.col);
//						SetFocusCell(/*idCurrentCell);//*/GetFirstVisibleCell(idCurrentCell.row));
					}
                }

                break;
            }

        case MOUSE_SIZING_COL:
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;

                CRect oldInvertedRect(m_LastMousePoint.x, rect.top, m_LastMousePoint.x + 2, rect.bottom);
                CRect oldInvertedRect2(m_ptStartResize.x - 1, rect.top, m_ptStartResize.x + 1, rect.bottom);
                pDC->InvertRect(&oldInvertedRect);
//                pDC->InvertRect(&oldInvertedRect2);

                CRect newInvertedRect(point.x, rect.top, point.x + 2, rect.bottom);
                CRect newInvertedRect2(m_ptStartResize.x - 1, rect.top, m_ptStartResize.x + 1, rect.bottom);
                pDC->InvertRect(&newInvertedRect);
//                pDC->InvertRect(&newInvertedRect2);
                ReleaseDC(pDC);
            }
            break;
            
        case MOUSE_SIZING_ROW:        
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;
                
                CRect oldInvertedRect(rect.left, m_LastMousePoint.y, rect.right, m_LastMousePoint.y + 2);
                CRect oldInvertedRect2(rect.left, m_ptStartResize.y - 1, rect.right, m_ptStartResize.y + 1);
                pDC->InvertRect(&oldInvertedRect);
//                pDC->InvertRect(&oldInvertedRect2);

                CRect newInvertedRect(rect.left, point.y, rect.right, point.y + 2);
                CRect newInvertedRect2(rect.left, m_ptStartResize.y - 1, rect.right, m_ptStartResize.y + 1);
                pDC->InvertRect(&newInvertedRect);
//                pDC->InvertRect(&newInvertedRect2);
                ReleaseDC(pDC);
            }
            break;
            
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_PREPARE_EDIT:
        case MOUSE_PREPARE_DRAG:
            m_MouseMode = MOUSE_PREPARE_DRAG;
            OnBeginDrag();    
            break;
#endif
        case MOUSE_PREPARE_COLUMN_DRAG:
            OnBeginColumnDrag();    
            break;
        }    
    }
    
    m_LastMousePoint = point;
}

CPoint CGridCtrl::GetPointClicked(int nRow, int nCol, const CPoint& point)
{
    CPoint PointCellOrigin;
    if( !GetCellOrigin( nRow, nCol, &PointCellOrigin)  )
        return CPoint( 0, 0);

    CPoint PointClickedCellRelative( point);
    PointClickedCellRelative -= PointCellOrigin;
    return PointClickedCellRelative;
}

void CGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    TRACE0("CGridCtrl::OnLButtonDblClk\n");

    CCellID cell = GetCellFromPt(point);
    if( !IsValid( cell) )
    {
        //ASSERT(FALSE);
        return;
    }

    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE)
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.x - start.x <= m_nResizeCaptureRange)     // Clicked right of border
            cell.col--;


        //  ignore columns that are hidden and look left towards first visible column
        BOOL bFoundVisible = FALSE;
        while( cell.col >= 0)
        {
            if( GetColumnWidth( cell.col) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            cell.col--;
        }
        if( !bFoundVisible)
            return;

        AutoSizeColumn(cell.col);
        Invalidate();
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE)
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.y - start.y <= m_nResizeCaptureRange)     // Clicked below border
            cell.row--;


//  ignore rows that are hidden and look upt towards first visible row
        BOOL bFoundVisible = FALSE;
        while( cell.row >= 0)
        {
            if( GetRowHeight( cell.row) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            cell.row--;
        }
        if( !bFoundVisible)
            return;
        AutoSizeRow(cell.row);
        Invalidate();
    }
    else if (m_MouseMode == MOUSE_NOTHING)
    {
        CPoint pointClickedRel;
        pointClickedRel = GetPointClicked( cell.row, cell.col, point);

		if (IsCellEditable(cell))
		{
            OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
		}
/*
        if (cell.row >= m_nFixedRows && IsValid(m_LeftClickDownCell) && cell.col >= m_nFixedCols)
        {
            OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }
        else if (m_bListMode)
        {
            if (!IsValid(cell))
                return;
            if (cell.row >= m_nFixedRows && cell.col < GetFixedColumnCount())
                OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }
*/
        if (IsValid(cell))
        {
            CGridCellBase* pCell = GetCell(cell.row, cell.col);
            if (pCell)
                pCell->OnDblClick(cell.row, cell.col, pointClickedRel);
            SendMessageToParent(cell.row, cell.col, NM_DBLCLK);
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// A.M. - fixed (gluck 2578 in bugtrack)
	OnMouseMove(0,point);
	// end
#ifdef GRIDCONTROL_USE_TITLETIPS
    // EFW - Bug Fix
    m_TitleTip.Hide();  // hide any titletips
#endif

    // TRACE0("CGridCtrl::OnLButtonDown\n");
    // CWnd::OnLButtonDown(nFlags, point);

    m_bMouseButtonDown   = TRUE; // 21.02.2006 build 99 - SBT 1689, переместили эту строку до SetFocus
	// - т.к. во время выполнения SetFocus может вылезти MsgBox и фокус мы потеряем

    SetFocus();

    m_LeftClickDownPoint = point;
//	CCellID PrevClickDownCell = m_LeftClickDownCell;
    m_LeftClickDownCell  = GetCellFromPt(point);
    if (!IsValid(m_LeftClickDownCell))
        return;

	TRACE("Clicked cell:%d,%d\n", m_LeftClickDownCell.row, m_LeftClickDownCell.col );
/*
	if (PrevClickDownCell == m_LeftClickDownCell && 
		PrevClickDownCell.row < GetFixedRowCount() && 
		!m_bListMode && GetDragColumn())
	{
		m_MouseMode = MOUSE_PREPARE_COLUMN_DRAG;
		return;
	}
*/
    m_SelectionStartCell = (nFlags & MK_SHIFT)? m_idCurrentCell : m_LeftClickDownCell;

    EndEditing();

    // tell the cell about it -- only things like a btn embedded in a cell
    //  would look at these msgs
    CGridCellBase* pCell = GetCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
    if (pCell)
        pCell->OnClickDown(m_LeftClickDownCell.row, m_LeftClickDownCell.col, GetPointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, point));

    // If the user clicks on a selected cell, then prepare to drag it.
    // (If the user moves the mouse, then dragging occurs)
    if (IsCellSelected(m_LeftClickDownCell))
    {
        // If control is pressed then unselect the cell or row (depending on the list mode)
        if (nFlags & MK_CONTROL)
        {
            DWORD dwState = GetItemState(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
            SetItemState(m_LeftClickDownCell.row, m_LeftClickDownCell.col,
                dwState & ~GVIS_SELECTED);
			
            SetFocusCell(m_LeftClickDownCell);
//            if (GetListMode())
//               SelectRows(m_LeftClickDownCell, TRUE, FALSE);
//            else
//                SelectCells(m_LeftClickDownCell, TRUE, FALSE);
            return;
        }
#ifndef GRIDCONTROL_NO_DRAGDROP
        else if (m_bAllowDragAndDrop)
            m_MouseMode = MOUSE_PREPARE_DRAG;
#endif
		else 
		{
			m_PrevSel.SetEmpty();
			SetSelectedRange(-1,-1,-1,-1);
			SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
						 max(m_LeftClickDownCell.col, m_nFixedCols));
		}
    }
    // If the user clicks on the current cell, then prepare to edit it.
    // (If the user moves the mouse, then dragging occurs)
    else if (m_LeftClickDownCell == m_idCurrentCell)
    {
        if (IsCellEditable(m_LeftClickDownCell))
            m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    }

    else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE &&
             m_MouseMode != MOUSE_OVER_ROW_DIVIDE)        
    {

//[AY]
 /*       SetFocusCell(-1, - 1);
        if (GetRowCount() > GetFixedRowCount() && 
            GetColumnCount() > GetFixedColumnCount())
            SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
                         max(m_LeftClickDownCell.col, m_nFixedCols));*/
    }
    
    SetCapture();
    
    if (m_MouseMode == MOUSE_NOTHING)
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
				UINT uCursor = IDC_CURSOR_WIDTH;
				CPoint start;
				CCellID cell = GetCellFromPt(point);
				if (GetCellOrigin(0, cell.col, &start) && 
					(point.x - start.x <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(0, cell.col - 1, &start))
					{
						if (GetColumnWidth(cell.col - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_COLUMN_SPLIT;
					}
				}
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
				UINT uCursor = IDC_CURSOR_HEIGHT;

				CCellID cell = GetCellFromPt(point);
				CPoint start;
				if (GetCellOrigin(cell.row, 0, &start) && 
					(point.y - start.y <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(cell.row - 1, 0, &start))
					{
						if (GetRowHeight(cell.row - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_ROW_SPLIT;
					}
				}
				AFX_MODULE_STATE * pState = AfxGetStaticModuleState();
				if (pState)
					SetCursor(::LoadCursor(pState->m_hCurrentResourceHandle, MAKEINTRESOURCE(uCursor)));
				else
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        // else if (m_MouseMode != MOUSE_NOTHING)
        //{
        //    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        //    m_MouseMode = MOUSE_NOTHING;
        //}
    }
    
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
    {
        m_MouseMode = MOUSE_SIZING_COL;
        CPoint start;
        if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start))
            return;

        if( !m_bHiddenColUnhide)
        {
            //  ignore columns that are hidden and look left towards first visible column
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellRightBorder = point.x - start.x > m_nResizeCaptureRange;

            if( bIsCellRightBorder
                && m_LeftClickDownCell.col + 1 >= GetColumnCount() )
            {
                // clicked on last column's right border

                // if last column is visible, don't do anything
                if( m_LeftClickDownCell.col >= 0)
                    bLookForVisible = FALSE;
            }

            if( bLookForVisible)
            {
                // clicked on column divider other than last right border
                BOOL bFoundVisible = FALSE;
                int iOffset = 1;

                if( bIsCellRightBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.col - iOffset >= 0)
                {
                    if( GetColumnWidth( m_LeftClickDownCell.col - iOffset) > 0)
                    {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.col--;
                }
                if( !bFoundVisible)
                    return;
            }
        }

        
		if (point.x - start.x <= m_nResizeCaptureRange)        // clicked right of border
            if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start))
                return;

		m_ptStartResize = start;

		CRect rect;
        GetClientRect(rect);
        CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);
        CRect invertedRect2(m_ptStartResize.x - 1, rect.top, m_ptStartResize.x + 1, rect.bottom);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            pDC->InvertRect(&invertedRect2);
            ReleaseDC(pDC);
        }

        rect.left = start.x;
        ClientToScreen(rect);
#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
    {
        m_MouseMode = MOUSE_SIZING_ROW;
        CPoint start;
        if (!GetCellOrigin(m_LeftClickDownCell, &start))
            return;

        if( !m_bHiddenRowUnhide)
        {
            //  ignore rows that are hidden and look up towards first visible row
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellBottomBorder = point.y - start.y > m_nResizeCaptureRange;

            if( bIsCellBottomBorder
                && m_LeftClickDownCell.row + 1 >= GetRowCount() )
            {
                // clicked on last row's bottom border

                // if last row is visible, don't do anything
                if( m_LeftClickDownCell.row >= 0)
                    bLookForVisible = FALSE;
            }

            if( bLookForVisible)
            {
                // clicked on row divider other than last bottom border
                BOOL bFoundVisible = FALSE;
                int iOffset = 1;

                if( bIsCellBottomBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.row - iOffset >= 0)
                {
                    if( GetRowHeight( m_LeftClickDownCell.row - iOffset) > 0)
                    {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.row--;
                }
                if( !bFoundVisible)
                    return;
            }
        }

        if (point.y - start.y <= m_nResizeCaptureRange)            // clicked below border
            if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start))
                return;

		m_ptStartResize = start;
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);
        CRect invertedRect2(rect.left, m_ptStartResize.y - 1, rect.right, m_ptStartResize.y + 1);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
			pDC->InvertRect(&invertedRect2);
            ReleaseDC(pDC);
        }

        rect.top = start.y;
        ClientToScreen(rect);
#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else
#ifndef GRIDCONTROL_NO_DRAGDROP
    if (m_MouseMode != MOUSE_PREPARE_DRAG) // not sizing or editing -- selecting
#endif
    {
        // If Ctrl pressed, save the current cell selection. This will get added
        // to the new cell selection at the end of the cell selection process
		m_PrevSel.SetEmpty();
//        m_PrevSelectedCellMap.RemoveAll();
        if (nFlags & MK_CONTROL)
        {
			m_PrevSel = m_Sel;
/*
            for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
            {
                DWORD key;
                CCellID cell;
                m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
                m_PrevSelectedCellMap.SetAt(key, cell);
            }
*/
        }
        
        if (m_LeftClickDownCell.row < GetFixedRowCount())
		{
            OnFixedRowClick(m_LeftClickDownCell);
		}
        else if (m_LeftClickDownCell.col < GetFixedColumnCount())
		{
            OnFixedColumnClick(m_LeftClickDownCell);
		}
        else
        {
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            OnSelecting(m_LeftClickDownCell);
        }

		if (m_LeftClickDownCell.row < GetFixedRowCount())
			m_nHTimerID = SetTimer(WM_HSCROLL, m_nTimerInterval * 2, 0);
		else if (m_LeftClickDownCell.col < GetFixedColumnCount())
			m_nVTimerID = SetTimer(WM_VSCROLL, m_nTimerInterval, 0);
		else
		{
			m_nHTimerID = SetTimer(WM_HSCROLL, m_nTimerInterval * 2, 0);
			m_nVTimerID = SetTimer(WM_VSCROLL, m_nTimerInterval, 0);
		}
        
    }   

    m_LastMousePoint = point;

}

void CGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TRACE0("CGridCtrl::OnLButtonUp\n");

    CWnd::OnLButtonUp(nFlags, point);

    m_bMouseButtonDown = FALSE;

#ifndef _WIN32_WCE_NO_CURSOR
    ClipCursor(NULL);
#endif

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        KillTimer(m_nHTimerID);
        KillTimer(m_nVTimerID);
        m_nHTimerID = 0;
        m_nVTimerID = 0;
    }

    CPoint pointClickedRel;
    pointClickedRel = GetPointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point);

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current cell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT)
    {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, pointClickedRel, VK_LBUTTON);
    }
#ifndef GRIDCONTROL_NO_DRAGDROP
    // m_MouseMode == MOUSE_PREPARE_DRAG only if user clicked down on a selected cell
    // and then didn't move mouse before clicking up (releasing button)
    else if (m_MouseMode == MOUSE_PREPARE_DRAG) 
    {
        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick(m_idCurrentCell.row, m_idCurrentCell.col, GetPointClicked(m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, NM_CLICK);
        SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_ACTIVATE);

        ResetSelectedRange();
    }
#endif
    else if (m_MouseMode == MOUSE_SIZING_COL)
    {
        CRect rect;
        GetClientRect(rect);
        
        CRect invertedRect(m_LastMousePoint.x, rect.top, m_LastMousePoint.x + 2, rect.bottom);
		CRect invertedRect2;
		invertedRect2.SetRectEmpty();
		if (m_ptStartResize.x != -1 && m_ptStartResize.y != -1)
			invertedRect2 = CRect(m_ptStartResize.x - 1, rect.top, m_ptStartResize.x + 1, rect.bottom);
        
        CDC* pDC = GetDC();

        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
			if (!invertedRect2.IsRectEmpty())
				pDC->InvertRect(&invertedRect2);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {   
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;

			int nW = 2 * m_nResizeCaptureRange;
/*			CDC* pDC = GetDC();
			if (pDC)
			{
				CFont * pOldFont = 0;
				pOldFont = (CFont*) pDC->SelectObject(GetBoldFont());
		        nW = pDC->GetTextExtent(_T(".")).cx;
				pDC->SelectObject(pOldFont);

				ReleaseDC(pDC);
			}
*/
			CGridCellBase * pCell = GetCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);

			nW += 2 * (pCell ? pCell->GetMargin() : 0);
			nW = point.x - start.x < nW ? nW : point.x - start.x;
            SetColumnWidth(m_LeftClickDownCell.col, nW);
			m_ptStartResize = CPoint(-1, -1);
            ResetScrollBars();
            Invalidate();
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW)
    {
        CRect rect;
        GetClientRect(rect);
        CPoint start;
        if (!GetCellOrigin(m_LeftClickDownCell, &start))
            return;

        CRect invertedRect(rect.left, m_LastMousePoint.y, rect.right, m_LastMousePoint.y + 2);
		CRect invertedRect2;
		invertedRect2.SetRectEmpty();
		if (m_ptStartResize.x != -1 && m_ptStartResize.y != -1)
			invertedRect2 = CRect(rect.left, m_ptStartResize.y - 1 , rect.right, m_ptStartResize.y + 1);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
			if (!invertedRect2.IsRectEmpty())
				pDC->InvertRect(&invertedRect2);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point  && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;
            
			int nH = 2 * m_nResizeCaptureRange;
/*	        CDC* pDC = GetDC();
			if (pDC)
			{
				CFont * pOldFont = 0;
				pOldFont = (CFont*) pDC->SelectObject(GetBoldFont());
		        nH = pDC->GetTextExtent(_T(".")).cy;
				pDC->SelectObject(pOldFont);

				ReleaseDC(pDC);
			}
*/
			CGridCellBase * pCell = GetCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);

			nH += 2 * (pCell ? pCell->GetMargin() : 0);
			nH = point.y - start.y < nH ? nH : point.y - start.y;
            SetRowHeight(m_LeftClickDownCell.row, nH);
			m_ptStartResize = CPoint(-1, -1);
            ResetScrollBars();
            Invalidate();
        }
    }
	else if (m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		m_bDragColumn = false;
		if (m_pDragList)
		{
			m_pDragList->EndDrag();
			m_pDragList->DeleteImageList();
			delete m_pDragList, m_pDragList = 0;;
		}

//		DrawMarker(m_nDragOverColumn, m_bPrevRight, false);
		m_nDragOverColumn -= m_bPrevRight ? 0 : 1;

		OnEndColumnDrag(m_nDragColumn, m_nDragOverColumn);
	
		m_nDragColumn = -1;
		m_nDragOverColumn = -1;
	}
    else
    {
        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick(m_idCurrentCell.row, m_idCurrentCell.col, GetPointClicked(m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, NM_CLICK);
        SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_ACTIVATE);
    }
    
    m_MouseMode = MOUSE_NOTHING;
    
#ifndef _WIN32_WCE_NO_CURSOR
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
    
    if (!IsValid(m_LeftClickDownCell))
        return;
    
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        pOwner->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED),
        (LPARAM) GetSafeHwnd());
}

#ifndef _WIN32_WCE
// EFW - Added to forward right click to parent so that a context
// menu can be shown without deriving a new grid class.
void CGridCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonUp(nFlags, point);
/*

    CCellID FocusCell = GetCellFromPt(point);

    EndEditing();        // Auto-destroy any InPlaceEdit's

    // If not a valid cell, pass -1 for row and column
    if(!IsValid(FocusCell))
        SendMessageToParent(-1, -1, NM_RCLICK);
    else
    {
        SetFocusCell(-1,-1);
        SetFocusCell(max(FocusCell.row, m_nFixedRows),
            max(FocusCell.col, m_nFixedCols));

        // tell the cell about it
        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnRClick(m_idCurrentCell.row, m_idCurrentCell.col, GetPointClicked(m_idCurrentCell.row, m_idCurrentCell.col, point) );

        SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, NM_RCLICK);
    }
*/
}
#endif

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing

// EFW - New print margin support functions
void CGridCtrl::SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
    int nLeftMargin, int nRightMargin, int nTopMargin,
    int nBottomMargin, int nGap)
{
    // If any parameter is -1, keep the existing setting
    if(nHeaderHeight > -1)
        m_nHeaderHeight = nHeaderHeight;
    if(nFooterHeight > -1)
        m_nFooterHeight = nFooterHeight;
    if(nLeftMargin > -1)
        m_nLeftMargin = nLeftMargin;
    if(nRightMargin > -1)
        m_nRightMargin = nRightMargin;
    if(nTopMargin > -1)
        m_nTopMargin = nTopMargin;
    if(nBottomMargin > -1)
        m_nBottomMargin = nBottomMargin;
    if(nGap > -1)
        m_nGap = nGap;
}

void CGridCtrl::GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
    int &nLeftMargin, int &nRightMargin, int &nTopMargin,
    int &nBottomMargin, int &nGap)
{
    nHeaderHeight = m_nHeaderHeight;
    nFooterHeight = m_nFooterHeight;
    nLeftMargin = m_nLeftMargin;
    nRightMargin = m_nRightMargin;
    nTopMargin = m_nTopMargin;
    nBottomMargin = m_nBottomMargin;
    nGap = m_nGap;
}

void CGridCtrl::Print()
{
    CDC dc;
    CPrintDialog printDlg(FALSE);

    if (printDlg.DoModal() != IDOK)             // Get printer settings from user
        return;

    dc.Attach(printDlg.GetPrinterDC());         // attach a printer DC
    dc.m_bPrinting = TRUE;

    CString strTitle;
    strTitle.LoadString(AFX_IDS_APP_TITLE);

    if( strTitle.IsEmpty() )
    {
        CWnd *pParentWnd = GetParent();
        while (pParentWnd)
        {
            pParentWnd->GetWindowText(strTitle);
            if (strTitle.GetLength())  // can happen if it is a CView, CChildFrm has the title
                break;
            pParentWnd = pParentWnd->GetParent();
        }
    }

    DOCINFO di;                                 // Initialise print doc details
    memset(&di, 0, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dc, &Info);                // Initialise printing
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++)
    {
        dc.StartPage();                         // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                    // Print page
        bPrintingOK = (dc.EndPage() > 0);       // end page
    }
    OnEndPrinting(&dc, &Info);                  // Clean up after printing

    if (bPrintingOK)
        dc.EndDoc();                            // end a print job
    else
        dc.AbortDoc();                          // abort job.

    dc.Detach();                                // detach the printer DC
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing overridables - for Doc/View print/print preview framework

// EFW - Various changes in the next few functions to support the
// new print margins and a few other adjustments.
void CGridCtrl::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
    // OnBeginPrinting() is called after the user has committed to
    // printing by OK'ing the Print dialog, and after the framework
    // has created a CDC object for the printer or the preview view.

    // This is the right opportunity to set up the page range.
    // Given the CDC object, we can determine how many rows will
    // fit on a page, so we can in turn determine how many printed
    // pages represent the entire document.

    ASSERT(pDC && pInfo);
    if (!pDC || !pInfo) return;

    // Get a DC for the current window (will be a screen DC for print previewing)
    CDC *pCurrentDC = GetDC();        // will have dimensions of the client area
    if (!pCurrentDC) return;

    CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
    CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));

    // Create the printer font
    int nFontSize = -10;
    CString strFontName = "Arial";
    m_PrinterFont.CreateFont(nFontSize, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                             OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, strFontName);

    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Get the average character width (in GridCtrl units) and hence the margins
    m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
    m_CharSize.cx /= 52;
    int nMargins = (m_nLeftMargin+m_nRightMargin)*m_CharSize.cx;

    // Get the page sizes (physical and logical)
    m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

    if( m_bWysiwygPrinting)
    {
        m_LogicalPageSize.cx = ScreenPixelsPerInch.cx * m_PaperSize.cx / PaperPixelsPerInch.cx * 3 / 4;
        m_LogicalPageSize.cy = ScreenPixelsPerInch.cy * m_PaperSize.cy / PaperPixelsPerInch.cy * 3 / 4;
    }
    else
    {
        m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

        m_LogicalPageSize.cx = (int)GetVirtualWidth()+nMargins;
#ifdef _WIN32_WCE
        m_LogicalPageSize.cy = (m_LogicalPageSize.cx * m_PaperSize.cy) / m_PaperSize.cx;
#else
        m_LogicalPageSize.cy = MulDiv(m_LogicalPageSize.cx, m_PaperSize.cy, m_PaperSize.cx);
#endif
    }

    m_nPageHeight = m_LogicalPageSize.cy - GetFixedRowHeight()
                       - (m_nHeaderHeight+m_nFooterHeight + 2*m_nGap)*m_CharSize.cy;

    // Get the number of pages. Assumes no row is bigger than the page size.
    int nTotalRowHeight = 0;
    m_nNumPages = 1;
    for (int row = GetFixedRowCount(); row < GetRowCount(); row++)
    {
        nTotalRowHeight += GetRowHeight(row);
        if (nTotalRowHeight > m_nPageHeight) {
            m_nNumPages++;
            nTotalRowHeight = GetRowHeight(row);
        }
    }

    // now, figure out how many additional pages must print out if rows ARE bigger
    //  than page size
    int iColumnOffset = 0;
    int i1;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }
    m_nPageWidth = m_LogicalPageSize.cx - iColumnOffset
                    - nMargins;
    m_nPageMultiplier = 1;

    if( m_bWysiwygPrinting)
    {
        int iTotalRowWidth = 0;
        for( i1 = GetFixedColumnCount(); i1 < GetColumnCount(); i1++)
        {
            iTotalRowWidth += GetColumnWidth( i1);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPageMultiplier++;
                iTotalRowWidth = GetColumnWidth( i1);
            }
        }
        m_nNumPages *= m_nPageMultiplier;
    }

    // Set up the print info
    pInfo->SetMaxPage(m_nNumPages);
    pInfo->m_nCurPage = 1;                        // start printing at page# 1

    ReleaseDC(pCurrentDC);
    pDC->SelectObject(pOldFont);
}

void CGridCtrl::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
    if (!pDC || !pInfo)
        return;

    //CRect rcPage(pInfo->m_rectDraw);
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Set the page map mode to use GridCtrl units, and setup margin
    pDC->SetMapMode(MM_ANISOTROPIC);
    pDC->SetWindowExt(m_LogicalPageSize);
    pDC->SetViewportExt(m_PaperSize);
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, 0);

    // Header
    pInfo->m_rectDraw.top    = 0;
    pInfo->m_rectDraw.left   = 0;
    pInfo->m_rectDraw.right  = m_LogicalPageSize.cx - (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;
    pInfo->m_rectDraw.bottom = m_nHeaderHeight * m_CharSize.cy;
    PrintHeader(pDC, pInfo);
    pDC->OffsetWindowOrg(0, -m_nHeaderHeight * m_CharSize.cy);

    // Gap between header and column headings
    pDC->OffsetWindowOrg(0, -m_nGap * m_CharSize.cy);

    pDC->OffsetWindowOrg(0, -GetFixedRowHeight());

    // We need to find out which row to start printing for this page.
    int nTotalRowHeight = 0;
    UINT nNumPages = 1;
    m_nCurrPrintRow = GetFixedRowCount();


    // Not only the row, but we need to figure out column, too

    // Can print 4 pages, where page 1 and 2 represent the same rows but
    // with different WIDE columns.
    //
    // .......
    // .1 .2 .  If representing page 3  -->    iPageIfIgnoredWideCols = 2
    // .......                                 iWideColPageOffset = 0
    // .3 .4 .  If representing page 2  -->    iPageIfIgnoredWideCols = 1
    // .......                                 iWideColPageOffset = 1

    int iPageIfIgnoredWideCols = pInfo->m_nCurPage / m_nPageMultiplier;
    int iWideColPageOffset = pInfo->m_nCurPage - ( iPageIfIgnoredWideCols * m_nPageMultiplier);
    if( iWideColPageOffset > 0)
        iPageIfIgnoredWideCols++;

    if( iWideColPageOffset == 0)
        iWideColPageOffset = m_nPageMultiplier;
    iWideColPageOffset--;

    // calculate current print row based on iPageIfIgnoredWideCols
    while(  m_nCurrPrintRow < GetRowCount()
            && (int)nNumPages < iPageIfIgnoredWideCols)
    {
        nTotalRowHeight += GetRowHeight(m_nCurrPrintRow);
        if (nTotalRowHeight > m_nPageHeight) {
            nNumPages++;
            if ((int)nNumPages == iPageIfIgnoredWideCols) break;
            nTotalRowHeight = GetRowHeight(m_nCurrPrintRow);
        }
        m_nCurrPrintRow++;
    }

    m_nPrintColumn = GetFixedColumnCount();
    int iTotalRowWidth = 0;
    int i1, i2;

    // now, calculate which print column to start displaying
    for( i1 = 0; i1 < iWideColPageOffset; i1++)
    {
        for( i2 = m_nPrintColumn; i2 < GetColumnCount(); i2++)
        {
            iTotalRowWidth += GetColumnWidth( i2);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPrintColumn = i2;
                iTotalRowWidth = 0;
                break;
            }
        }
    }


    PrintRowButtons( pDC, pInfo);   // print row buttons on each page
    int iColumnOffset = 0;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }

    // Print the column headings
    pInfo->m_rectDraw.bottom = GetFixedRowHeight();

    if( m_nPrintColumn == GetFixedColumnCount())
    {
        // have the column headings fcn draw the upper left fixed cells
        //  for the very first columns, only
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;
        m_nPrintColumn = 0;
        PrintColumnHeadings(pDC, pInfo);
        m_nPageWidth -= iColumnOffset;
        m_nPrintColumn = GetFixedColumnCount();

        pDC->OffsetWindowOrg( -iColumnOffset, -GetFixedRowHeight());
    }
    else
    {
        pDC->OffsetWindowOrg( -iColumnOffset, +GetFixedRowHeight());
        PrintColumnHeadings(pDC, pInfo);
        pDC->OffsetWindowOrg( 0, -GetFixedRowHeight());
    }


    if (m_nCurrPrintRow >= GetRowCount()) return;

    // Draw as many rows as will fit on the printed page.
    // Clip the printed page so that there is no partially shown
    // row at the bottom of the page (the same row which will be fully
    // shown at the top of the next page).

    BOOL bFirstPrintedRow = TRUE;
    CRect rect;
    rect.bottom = -1;
    while (m_nCurrPrintRow < GetRowCount())
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(m_nCurrPrintRow) - 1;

        if (rect.bottom > m_nPageHeight) break;            // Gone past end of page

        rect.right = -1;

        // modified to allow printing of wide grids on multiple pages
        for (int col = m_nPrintColumn; col < GetColumnCount(); col++)
        {
            rect.left = rect.right+1;
            rect.right =    rect.left
                            + GetColumnWidth( col)
                            - 1;

            if( rect.right > m_nPageWidth)
                break;

            CGridCellBase* pCell = GetCell(m_nCurrPrintRow, col);
            if (pCell)
                pCell->PrintCell(pDC, m_nCurrPrintRow, col, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (col == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if (m_nCurrPrintRow == 0) {
                    pDC->MoveTo(rect.left-Overlap, rect.top);
                    pDC->LineTo(rect.right, rect.top);
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (bFirstPrintedRow)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if (col == 0) {
                    pDC->MoveTo(rect.left, rect.top-Overlap);
                    pDC->LineTo(rect.left, rect.bottom);
                }
            }

        }
        m_nCurrPrintRow++;
        bFirstPrintedRow = FALSE;
    }


    // Footer
    pInfo->m_rectDraw.bottom = m_nFooterHeight * m_CharSize.cy;
    pDC->SetWindowOrg( -m_nLeftMargin * m_CharSize.cx,
        -m_LogicalPageSize.cy + m_nFooterHeight * m_CharSize.cy);
    PrintFooter(pDC, pInfo);

    // SetWindowOrg back for next page
    pDC->SetWindowOrg(0,0);

    pDC->SelectObject(pOldFont);
}

void CGridCtrl::PrintColumnHeadings(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.bottom = -1;

    BOOL bFirst = TRUE;
    // modified to allow column hdr printing of multi-page wide grids
    for (int row = 0; row < GetFixedRowCount(); row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row) - 1;

        rect.right = -1;
        for (int col = m_nPrintColumn; col < GetColumnCount(); col++)
        {
            rect.left = rect.right+1;
            rect.right =    rect.left
                            + GetColumnWidth( col)
                            - 1;

            if( rect.right > m_nPageWidth)
                break;

            CGridCellBase* pCell = GetCell(row, col);
            if (pCell)
                pCell->PrintCell(pDC, row, col, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (col == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if (row == 0) {
                    pDC->MoveTo(rect.left-Overlap, rect.top);
                    pDC->LineTo(rect.right, rect.top);
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (row == 0)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if( bFirst) {
                    pDC->MoveTo(rect.left-1, rect.top-Overlap);
                    pDC->LineTo(rect.left-1, rect.bottom);
                    bFirst = FALSE;
                }
            }

        }
    }

    pDC->SelectObject(pOldFont);
}

/*****************************************************************************
Prints line of row buttons on each page of the printout.  Assumes that
the window origin is setup before calling

*****************************************************************************/
void CGridCtrl::PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.right = -1;

    BOOL bFirst = TRUE;
    for( int iCol = 0; iCol < GetFixedColumnCount(); iCol++)
    {
        rect.left = rect.right+1;
        rect.right =    rect.left
                        + GetColumnWidth( iCol)
                        - 1;

        rect.bottom = -1;
        for( int iRow = m_nCurrPrintRow; iRow < GetRowCount(); iRow++)
        {
            rect.top = rect.bottom+1;
            rect.bottom = rect.top + GetRowHeight( iRow) - 1;

            if( rect.bottom > m_nPageHeight)
                break;

            CGridCellBase* pCell = GetCell(iRow, iCol);
            if (pCell)
                pCell->PrintCell(pDC, iRow, iCol, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (iCol == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if( bFirst) {
                    pDC->MoveTo(rect.left-Overlap, rect.top-1);
                    pDC->LineTo(rect.right, rect.top-1);
                    bFirst = FALSE;
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (iRow == 0)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if (iCol == 0) {
                    pDC->MoveTo(rect.left, rect.top-Overlap);
                    pDC->LineTo(rect.left, rect.bottom);
                }
            }

        }
    }
    pDC->SelectObject(pOldFont);
}

void CGridCtrl::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
    // print App title on top right margin
    CString strRight;
    strRight.LoadString(AFX_IDS_APP_TITLE);

    // print parent window title in the centre (Gert Rijs)
    CString strCenter;
    CWnd *pParentWnd = GetParent();
    while (pParentWnd)
    {
        pParentWnd->GetWindowText(strCenter);
        if (strCenter.GetLength())  // can happen if it is a CView, CChildFrm has the title
            break;
        pParentWnd = pParentWnd->GetParent();
    }

    CFont   BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    VERIFY(m_PrinterFont.GetLogFont(&lf));
    lf.lfWeight = FW_BOLD;
    VERIFY(BoldFont.CreateFontIndirect(&lf));

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect   rc(pInfo->m_rectDraw);
    if( !strCenter.IsEmpty() )
        pDC->DrawText( strCenter, &rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();

    // draw ruled-line across top
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.bottom);
    pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void CGridCtrl::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
    // page numbering on left
    CString strLeft;
    strLeft.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage() );

    // date and time on the right
    CString strRight;
    CTime t = CTime::GetCurrentTime();
    strRight = t.Format(_T("%c"));
    
    CRect rc(pInfo->m_rectDraw);

    // draw ruled line on bottom
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.top);
    pDC->LineTo(rc.right, rc.top);

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // EFW - Bug fix - Force text color to black.  It doesn't always
    // get set to a printable color when it gets here.
    pDC->SetTextColor(RGB(0, 0, 0));

    if( !strLeft.IsEmpty() )
        pDC->DrawText( strLeft, &rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();
}

void CGridCtrl::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    m_PrinterFont.DeleteObject();
}

#endif  // !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)


#ifndef _WIN32_WCE
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl persistance

BOOL CGridCtrl::Save(LPCTSTR filename)
{
    CStdioFile File;
    CFileException ex;

    if (!File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText, &ex))
    {
        ex.ReportError();
        return FALSE;
    }

    TRY
    {
        int nNumColumns = GetColumnCount();
        for (int i = 0; i < nNumColumns; i++)
        {
            File.WriteString(GetItemText(0,i));
            File.WriteString((i==(nNumColumns-1))? _T("\n"):_T(","));
        }

        for (i = 0; i < GetRowCount(); i++)
        {
            for (int j = 0; j < nNumColumns; j++)
            {
                File.WriteString(GetItemText(i,j));
                File.WriteString((j==(nNumColumns-1))? _T("\n"):_T(","));
            }
        }

        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to save grid list"));
#ifndef _WIN32_WCE
        e->Delete();
#endif
        return FALSE;
    }
    END_CATCH

    return TRUE;
}

BOOL CGridCtrl::Load(LPCTSTR filename)
{
    TCHAR *token, *end;
    TCHAR buffer[1024];
    CStdioFile File;
    CFileException ex;

    if (!File.Open(filename, CFile::modeRead | CFile::typeText))
    {
        ex.ReportError();
        return FALSE;
    }

    DeleteAllItems();

    TRY
    {
        // Read Header off file
        File.ReadString(buffer, 1024);

        // Get first token
        for (token=buffer, end=buffer;
             *end && (*end != _T(',')) && (*end != _T('\n')); 
             end++)
            ;

        if ((*end == _T('\0')) && (token == end))
            token = NULL;

        *end = _T('\0');

        while (token)
        {
            InsertColumn(token);

            // Get next token
            for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
               end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');
        }

        // Read in rest of data
        int nItem = 0;
        while (File.ReadString(buffer, 1024))
        {
            // Get first token
            for (token=buffer, end=buffer;
              *end && (*end != _T(',')) && (*end != _T('\n')); end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');

            int nSubItem = 0;
            while (token)
            {
                if (!nSubItem)
                    InsertRow(token);
                else
                    SetItemText(nItem, nSubItem, token);

                // Get next token
                for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
                  end++)
                    ;

                if ((*end == _T('\0')) && (token == end))
                    token = NULL;

                *end = _T('\0');

                nSubItem++;
            }
            nItem++;
        }

        AutoSizeColumns();
        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to load grid data"));
#ifndef _WIN32_WCE
        e->Delete();
#endif
        return FALSE;
    }
    END_CATCH

    return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl overrideables

#ifndef GRIDCONTROL_NO_DRAGDROP
// This is no longer needed since I've changed to OLE drag and drop - but it's
// still cool code. :)
CImageList* CGridCtrl::CreateDragImage(CPoint *pHotSpot)
{
    CDC* pDC = GetDC();
    if (!pDC)
        return NULL;

    CRect rect;
    CCellID cell = GetFocusCell();
    if (!GetCellRect(cell.row, cell.col, rect))
	{
		ReleaseDC(pDC);
        return NULL;
	}
    
    // Translate coordinate system
    rect.BottomRight() = CPoint(rect.Width(), rect.Height());
    rect.TopLeft()     = CPoint(0, 0);
    *pHotSpot = rect.BottomRight(); 
    
    // Create a new imagelist (the caller of this function has responsibility
    // for deleting this list)
    CImageList* pList = new CImageList;
    if (!pList || !pList->Create(rect.Width(), rect.Height(), ILC_MASK, 1, 1))
    {    
        if (pList)
            delete pList;

		ReleaseDC(pDC);
        return NULL;
    }
    
    // Create mem DC and bitmap
    CDC MemDC;
    CBitmap bm;
    MemDC.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = MemDC.SelectObject(&bm);
    MemDC.SetWindowOrg(0, 0);
    
    // Draw cell onto bitmap in memDC
    CGridCellBase* pCell = GetCell(cell.row, cell.col);
    if (pCell)
        pCell->Draw(&MemDC, cell.row, cell.col, rect, FALSE);
    
    // Clean up
    ReleaseDC(pDC);

    MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();
    
    // Add the bitmap we just drew to the image list.
    pList->Add(&bm, GetTextBkColor());
    bm.DeleteObject();

    return pList;
}
#endif

void CGridCtrl::OnFixedRowClick(CCellID& cell)
{
    if (!IsValid(cell))
        return;

    if (m_bListMode)
    {
        m_MouseMode = MOUSE_SELECT_ROW;
		CCellID cellNew = GetFirstVisibleCell(-1, cell.col);
		m_SelectionStartCell = cellNew;
        OnSelecting(cellNew);
		SetFocusCell(cellNew);
        if (m_bSortOnClick)
		{
			CWaitCursor waiter;
			if (cell.col == GetSortColumn())
				SortTextItems(cell.col, !GetSortAscending());
			else
				SortTextItems(cell.col, TRUE);
			Invalidate();
		}
		else
		{
			m_MouseMode = MOUSE_PREPARE_COLUMN_DRAG;
		}
    }
    else if (cell.col < GetFixedColumnCount())
    {
        m_MouseMode = MOUSE_SELECT_ALL;
        OnSelecting(cell);
		SetFocusCell(GetFirstVisibleCell());
    }
    else 
    {
        m_MouseMode = MOUSE_SELECT_COL;
        OnSelecting(cell);
//		SetFocusCell(GetFirstVisibleCell(-1, cell.col));
		m_MouseMode = MOUSE_PREPARE_COLUMN_DRAG;
    }
}

CCellID CGridCtrl::GetFirstVisibleCell(int nRow/* = -1*/, int nCol/* = -1*/)
{
	CCellID cell = GetTopleftNonFixedCell();
	if (nRow == -1 && nCol == -1)
		return cell;

	if (nRow == -1)
	{
		cell.col = nCol;
	}
	else if (nCol == -1)
	{
		cell.row = nRow;
	}
	else
	{
		if (IsValid(CCellID(nRow, nCol)))
			cell = CCellID(nRow, nCol);
	}
	return cell;
}
/*
CCellID CGridCtrl::GetLastVisibleCell(int nRow, int nCol)
{
	CCellID cell(-1, -1);

    CRect rect;
	if (!::IsWindow(GetSafeHwnd()))
		return CCellID(-1, -1);

    GetClientRect(rect);
    CCellID idTopLeft = GetTopleftNonFixedCell();

    // calc bottom
    int bottom = GetFixedRowHeight();
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
    int right = GetFixedColumnWidth();
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
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
	

	if (nRow == -1 && nCol == -1)
		return cell;

	if (nRow == -1)
	{
		cell.col = nCol;
	}
	else if (nCol == -1)
	{
		cell.row = nRow;
	}
	else
	{
		if (!IsValid(CCellID(nRow, nCol)))
			cell = CCellID(nRow, nCol);
	}
	
	return cell;
}
*/
void CGridCtrl::OnFixedColumnClick(CCellID& cell)
{
    if (!IsValid(cell))
        return;

//    if (m_bListMode && (GetItemState(cell.row, m_nFixedCols) & GVNI_SELECTED))
//    {
//        OnEditCell(cell.row, cell.col, VK_LBUTTON);
//        return;
//    }

    if (cell.row < GetFixedRowCount())
    {
        m_MouseMode = MOUSE_SELECT_ALL;
        OnSelecting(cell);
		SetFocusCell(GetFirstVisibleCell());
    }
    else
    {
        m_MouseMode = MOUSE_SELECT_ROW;
        OnSelecting(cell);
		SetFocusCell(GetFirstVisibleCell(cell.row));
    }
}

// Gets the extent of the text pointed to by str (no CDC needed)
// By default this uses the selected font (which is a bigger font)
CSize CGridCtrl::GetTextExtent(int nRow, int nCol, LPCTSTR str)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return CSize(0, 0);
    else
        return pCell->GetTextExtent(str);
}

void CGridCtrl::OnEditCell(int nRow, int nCol, CPoint point, UINT nChar)
{
#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
#endif

    // Can we do it?
    CCellID cell(nRow, nCol);
    if (!IsValid(cell) || !IsCellEditable(nRow, nCol))
        return;

    // Can we see what we are doing?
    EnsureVisible(nRow, nCol);
    if (!IsCellVisible(nRow, nCol))
        return;

    // Where, exactly, are we gonna do it??
    CRect rect;
    if (!GetCellRect(cell, rect))
        return;

    // Tell Mum and Dad what we are doing
    SendMessageToParent(nRow, nCol, GVN_BEGINLABELEDIT);

    // Let's do it...
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (pCell)
        pCell->Edit(nRow, nCol, rect, point, IDC_INPLACE_CONTROL, nChar);
}

void CGridCtrl::EndEditing()
{
    CCellID cell = GetFocusCell();
    if (!IsValid(cell)) return;
    CGridCellBase *pCell = GetCell(cell.row, cell.col);
    if (pCell)
        pCell->EndEdit();
}

void CGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
{
    CString strCurrent = GetItemText(nRow, nCol);
    if (strCurrent != str)
    {
        SetModified(TRUE, nRow, nCol);
        SetItemText(nRow, nCol, str);
    }

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (pCell)
        pCell->OnEndEdit();
}

CString CGridCtrl::GetItemText(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols)
        return _T("");

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return _T("");

    return pCell->GetText();
}

int CGridCtrl::GetSelectedCount() const
{
	int nCount = 0;
	CSelection sel = m_Sel;
	for (POSITION pos = sel.GetRanges().GetHeadPosition(); pos != NULL; )
	{
		CCellRange range = sel.GetRanges().GetNext(pos);
		if (range.IsValid())
			nCount += range.Count();
	}
	return nCount;
}


BOOL CGridCtrl::MoveColumn(int nCol, int nColAfter)
{
	if (nCol < GetFixedColumnCount() || nCol >= GetColumnCount() ||
		nColAfter < GetFixedColumnCount()-1 || nColAfter >= GetColumnCount())
		return false;

	if (nCol == nColAfter+1 || nCol == nColAfter)
		return true;

	CCellID FocusCell = GetFocusCell();

    ResetSelectedRange();

	int nNewCol = nColAfter + 1;
	CELLHEIGHT nSize = m_arColWidths[nCol];
	if (nCol < nNewCol)
	{
		m_arColWidths.InsertAt(nNewCol, nSize);
		m_arColWidths.RemoveAt(nCol);
	}
	else
	{
		m_arColWidths.RemoveAt(nCol);
		m_arColWidths.InsertAt(nNewCol, nSize);
	}
	m_OverCell  = CCellID(-1, -1);

    for (int row = 0; row < m_nRows; row++) 
    {
        GRID_ROW* pRow = m_RowData[row];
        if (!pRow)
            return false;
		
		CGridCellBase * pCell = pRow->GetAt(nCol);
		if (row < m_nFixedRows)
			pCell->OnMouseLeave(row, nCol, CPoint(-1, -1));

		if (nCol < nNewCol)
		{
			pRow->InsertAt(nNewCol, pCell);
			pRow->RemoveAt(nCol);
		}
		else
		{
			pRow->RemoveAt(nCol);
			pRow->InsertAt(nNewCol, pCell);
		}
    }

	nNewCol = nCol < nNewCol ? nColAfter : nNewCol;
    ResetScrollBars();

	if (FocusCell.col == nCol)
		FocusCell.col = nNewCol;

	m_SelectionStartCell = FocusCell;
    if (m_bListMode)
    {
        m_MouseMode = MOUSE_SELECT_ROW;
        OnSelecting(FocusCell.row < GetFixedRowCount() ? GetFirstVisibleCell(-1, nNewCol) : FocusCell);
		SetFocusCell(FocusCell);
    }
    else 
    {
        m_MouseMode = MOUSE_SELECT_COL;
        OnSelecting(FocusCell);
		SetFocusCell(FocusCell);
    }

	InvalidateRect(NULL);
//	UpdateWindow();
    SetModified();
    
	return true;
}


// modified
void CGridCtrl::OnSetFocusCell(CCellID/* cell*/)
{
/*
	if (m_bListMode)
	{
		if (cell.row < GetFixedRowCount())
			return;
		SelectRows(cell);
	}
*/
}

void CGridCtrl::OnSetFocusCell(int nRow, int nCol)
{
    OnSetFocusCell(CCellID(nRow, nCol));
}
//// modified
CScrollBar* CGridCtrl::GetScrollBarCtrl(int nBar) const
{
	return (m_hWnd && ::IsWindow(m_hWnd) && GetParent()) ? GetParent()->GetScrollBarCtrl(nBar) : 0;
}

bool CGridCtrl::SetItemEmpty(int nRow, int nCol)
{
	if (ExistsAdditionRow() && nRow == GetRowCount() - 1)
		return true;

	CGridCellBase * pCell = GetCell(nRow, nCol);
	if (!pCell)
		return false;
	pCell->SetText(_T("--"));
	return true;
}

bool CGridCtrl::IsItemEmpty(int nRow, int nCol)
{
	if (ExistsAdditionRow() && nRow == GetRowCount() - 1)
		return true;

	CGridCellBase * pCell = GetCell(nRow, nCol);
	if (!pCell)
		return false;
	
	return lstrcmp(pCell->GetText(), _T("--")) == 0;
}


void CGridCtrl::UpdateSizes()
{
/*	for (int i = 0; i < m_arrSaveHeights.GetSize(); i++)
	{
		if (m_arRowHeights.GetSize() > i)
			m_arRowHeights[i] = m_arrSaveHeights[i];
	}
*/
	for (int i = 0; i < m_arrSaveWidths.GetSize(); i++)
	{
		if (m_arColWidths.GetSize() > i)
			m_arColWidths[i] = m_arrSaveWidths[i];
	}
}

void CGridCtrl::RetrieveSizes()
{
/*	for (int i = 0; i < m_arRowHeights.GetSize(); i++)
	{
		if (i < m_arrSaveHeights.GetSize())
			m_arrSaveHeights[i] = m_arRowHeights[i];
		else
			m_arrSaveHeights.SetAtGrow(i, m_arRowHeights[i]);
	}
*/
	for (int i = 0; i < m_arColWidths.GetSize(); i++)
	{
		if (i < m_arrSaveWidths.GetSize())
			m_arrSaveWidths[i] = m_arColWidths[i];
		else 
			m_arrSaveWidths.SetAtGrow(i, m_arColWidths[i]);
	}
}
/*
int CGridCtrl::GetSaveHeight(int nIndex)
{
	if (nIndex < m_arrSaveHeights.GetSize())
		return m_arrSaveHeights[nIndex];

	return m_nDefCellHeight;
}
*/
CELLHEIGHT CGridCtrl::GetSaveWidth(int nIndex)
{
	if (nIndex < m_arrSaveWidths.GetSize())
		return m_arrSaveWidths[nIndex];

	return m_nDefCellWidth;
}
/*
void CGridCtrl::SetSaveHeight(int nIndex, int nSize)
{
	if (nIndex >= 0 && nIndex < m_arrSaveHeights.GetSize())
		m_arrSaveHeights[nIndex] = nSize;
	else
	{
		int nPrev = m_arrSaveHeights.GetSize();
		m_arrSaveHeights.SetSize(nIndex + 1);
		if (nPrev >= 0)
		{
			for (int i = nPrev; i < m_arrSaveHeights.GetSize(); i++)
				m_arrSaveHeights[i] = m_nDefCellWidth;
		}

		m_arrSaveHeights[nIndex] = nSize;
	}
}
*/
void CGridCtrl::SetSaveWidth(int nIndex, CELLHEIGHT nSize)
{
	if (nIndex >= 0 && nIndex < m_arrSaveWidths.GetSize())
		m_arrSaveWidths[nIndex] = nSize;
	else
	{
		int nPrev = m_arrSaveWidths.GetSize();
		m_arrSaveWidths.SetSize(nIndex + 1);
		if (nPrev >= 0)
		{
			for (int i = nPrev; i < m_arrSaveWidths.GetSize(); i++)
				m_arrSaveWidths[i] = GetColumnCount() > i ? GetColumnWidth(i) : m_nDefCellWidth;
		}

		m_arrSaveWidths[nIndex] = nSize;
	}
}

void CGridCtrl::CreateLightBrush()
{
	CWindowDC dc(NULL);
	int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
	m_brLight.DeleteObject ();

	COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	COLORREF clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	
	if (nBitsPerPixel > 8)
	{
		COLORREF clrLight = RGB (
								GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) - GetRValue(clrBtnFace)) / 2),
								GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) - GetGValue(clrBtnFace)) / 2),
								GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 2)
								);

		m_brLight.CreateSolidBrush(clrLight);
	}
	else
	{
		HBITMAP hbmGray = CreateDitherBitmap(dc.GetSafeHdc());
		ASSERT(hbmGray != NULL);
		CBitmap bmp;
		bmp.Attach(hbmGray);
		m_brLight.CreatePatternBrush(&bmp);
	}
}

HBITMAP CGridCtrl::CreateDitherBitmap (HDC hDC)
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[16];
	} 
	bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor(COLOR_BTNFACE);

	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	return hbm;
}

// D'n'd Column /////////////////////////////////////////////////////////////////////////////////

void CGridCtrl::OnCancelMode()
{
	if (m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		m_bMouseButtonDown = FALSE;

	#ifndef _WIN32_WCE_NO_CURSOR
		ClipCursor(NULL);
	#endif

		if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
		{
			ReleaseCapture();
			KillTimer(m_nHTimerID);
			KillTimer(m_nVTimerID);
			m_nHTimerID = 0;
			m_nVTimerID = 0;
		}
		m_bDragColumn = false;
		if (m_pDragList)
		{
			m_pDragList->EndDrag();
			m_pDragList->DeleteImageList();
			delete m_pDragList, m_pDragList = 0;;
		}

		DrawMarker(m_nDragOverColumn, m_bPrevRight, false);
		m_nDragOverColumn -= m_bPrevRight ? 0 : 1;

//		OnEndColumnDrag(m_nDragColumn, m_nDragOverColumn);
		m_nDragColumn = -1;
		m_nDragOverColumn = -1;
		m_MouseMode = MOUSE_NOTHING;

	#ifndef _WIN32_WCE_NO_CURSOR
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	#endif
	}
	CWnd::OnCancelMode();
}


bool CGridCtrl::EnableColumnDrag(int nCol)
{
	if (!GetDragColumn())
		return false;

	if (nCol < GetFirstDragableColumn() || nCol < GetFixedColumnCount() || nCol >= GetColumnCount())
		return false;

	return true;
}

void CGridCtrl::OnEndColumnDrag(int nCol, int nColAfter)
{
	BOOL bRet = MoveColumn(nCol, nColAfter);
	ASSERT(bRet);
}

void CGridCtrl::OnBeginColumnDrag()
{
	if (IsValid(m_LeftClickDownCell) && EnableColumnDrag(m_LeftClickDownCell.col))
	{
		if (m_nDragColumn != -1)
			return;

		m_MouseMode = MOUSE_COLUMN_DRAGGING;
		if (m_pDragList)
		{
			m_pDragList->DeleteImageList();
			delete m_pDragList, m_pDragList = 0;;
		}
		m_nDragColumn = m_LeftClickDownCell.col;
		m_ptDragOffset = m_LastMousePoint;
		m_ptMove = m_LastMousePoint;
		m_pDragList = CreateDragColumn(m_nDragColumn, m_ptDragOffset);
		if (!m_pDragList)
		{
			m_nDragColumn = -1;
			return;
		}

        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		SetCapture();

		m_bDragColumn = true;
		m_ptMove.y++;m_ptMove.x++;
		m_pDragList->BeginDrag(0, m_ptDragOffset);
		m_pDragList->DragEnter(this, m_ptMove);
		m_pDragList->DragMove(CPoint(m_ptMove.x, m_ptDragOffset.y + 2));
		m_pDragList->DragShowNolock(true);

		CPoint ptOrig;
		GetCellOrigin(0, m_nDragColumn, &ptOrig);
		int nC = ptOrig.x + GetColumnWidth(m_nDragColumn) / 2;
		m_bPrevRight = nC < m_ptMove.x ? true : false;

		m_nDragOverColumn = m_nDragColumn;
		DrawMarker(m_nDragColumn, m_bPrevRight, true);
	}
}

CImageList* CGridCtrl::CreateDragColumn(int nColumn, CPoint & pt)
{
	if (!EnableColumnDrag(nColumn))
		return NULL;

    CDC* pDC = GetDC();
    if (!pDC)
        return NULL;

    CRect rect;
	CCellRange range(0, nColumn, GetFixedRowCount() - 1, nColumn);
	if (!range.IsValid())
	{
		ReleaseDC(pDC);
		return NULL;
	}

	if (!GetCellRangeRect(range, rect))
	{
		ReleaseDC(pDC);
		return NULL;
	}

	pt.x -= rect.left;
	pt.y -= rect.top;

//	rect.right++; rect.bottom++;
    
    // Translate coordinate system
    rect.BottomRight() = CPoint(rect.Width(), rect.Height());
    rect.TopLeft()     = CPoint(0, 0);
    
    // Create a new imagelist (the caller of this function has responsibility
    // for deleting this list)
    CImageList* pList = new CImageList;
    if (!pList || !pList->Create(rect.Width(), rect.Height(), ILC_MASK, 1, 1))
    {    
        if (pList)
            delete pList;

		ReleaseDC(pDC);
        return NULL;
    }
    
    // Create mem DC and bitmap
    CDC MemDC;
    CBitmap bm;
    MemDC.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = MemDC.SelectObject(&bm);
    MemDC.SetWindowOrg(0, 0);
    
    // Draw cell onto bitmap in memDC
	CRect drawRect = rect;
	drawRect.bottom = -1;
	for (int i = 0; i < GetFixedRowCount(); i++)
	{
		CRect cellRect;
		GetCellRect(i, nColumn, &cellRect);

		drawRect.top = drawRect.bottom + 1;
		drawRect.bottom += cellRect.Height();

		DrawCellDrag(&MemDC, i, nColumn, drawRect);
	}

    // Clean up
    MemDC.SelectObject(pOldBitmap);
    ReleaseDC(pDC);
	MemDC.DeleteDC();
    
    // Add the bitmap we just drew to the image list.
    pList->Add(&bm, GetTextBkColor());
    bm.DeleteObject();

    return pList;
}

void CGridCtrl::DrawMarker(int nCol1, bool bRight, bool bDraw)
{
	CPoint pt = CPoint(-1, -1);

	if (!GetCellOrigin(0, nCol1, &pt))
		return;

	int nHeight = GetFixedRowHeight();
	if (bRight)
		pt.x += GetColumnWidth(nCol1);

	if (bDraw)
	{
		CDC * pDC = GetDC();
		if (!pDC)
			return;

		CPen pen;
		pen.CreatePen(PS_SOLID/*|PS_ENDCAP_SQUARE*/, 3, m_nMarkerColor);

		CPen *	pOldPen = pDC->SelectObject(&pen);

		pDC->MoveTo(pt.x - 1, nHeight);
		pDC->LineTo(pt.x - m_nMarkerSize / 3, nHeight + m_nMarkerSize / 3);
		pDC->MoveTo(pt.x - 1, nHeight);
		pDC->LineTo(pt.x - 1 + m_nMarkerSize / 3, nHeight + m_nMarkerSize / 3);
		pDC->MoveTo(pt.x - 1, nHeight);
		pDC->LineTo(pt.x - 1, nHeight + m_nMarkerSize);

		pDC->SelectObject(pOldPen);

		pen.DeleteObject();

		ReleaseDC(pDC);
	}
	else
	{
		CRect rc(pt.x - m_nMarkerSize / 2, nHeight - 2, pt.x + m_nMarkerSize / 2, nHeight + m_nMarkerSize + 2);
		InvalidateRect(rc);

		CCellID TLCell = GetTopleftNonFixedCell();
		if (GetCellOrigin(TLCell, &pt))
		{
			rc = CRect(pt.x - m_nMarkerSize / 2, nHeight - 2, pt.x + m_nMarkerSize / 2, nHeight + m_nMarkerSize + 2);
			InvalidateRect(rc);
		}

		UpdateWindow();
	}
}

BOOL CGridCtrl::DrawCellDrag(CDC* pDC, int nRow, int nCol, CRect rect)
{
    if (!pDC)
        return false;

	CGridCellBase * pCell = GetCell(nRow, nCol);
	if (!pCell)
		return false;

	COLORREF clOldFixedBack = GetFixedBkColor();

	SetFixedBkColor(::GetSysColor(COLOR_BTNSHADOW));
	DWORD dwPrevState = pCell->GetState();
	pCell->SetState(GVIS_FIXED);
	BOOL bRet = pCell->Draw(pDC, nRow, nCol, rect, true);
	pCell->SetState(dwPrevState);
	SetFixedBkColor(clOldFixedBack);

	return bRet;
}

//print functions
CCellRange CGridCtrl::GetCellRangeFromRect(LPRECT pRect)
{
	CRect rect(pRect);
	if (rect.IsRectEmpty())
		return CCellRange();

    CCellID idTopLeft = GetCellFromPt(rect.TopLeft(), true);

    // calc bottom
    int bottom = 0;
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
    int right = 0;
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


// NOTE:
// nUserPosX & nUserPosY & nNewUserPosX & nNewUserPosY used by grid
bool CGridCtrl::OnGetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
{
	// get virtual width
	long nWidth = (long)GetVirtualWidth();
	bool bContinue = false;
	m_bPrinting = true;

	// grid is longer that given max_width
	if (nWidth > nUserPosX + nMaxWidth)
	{
		// 2 cases: a) first call 
		// in this case given user_pos == 0 && given new_usr_pos == 0
		//			b) next call 
		// in this case user_pos undefined (previous new_usr_pos) && given new_usr_pos undefined
		// calc last from previous printed column
		CCellID cellFirst = GetCellFromPt(CPoint(nUserPosX, 0), true);
		if (cellFirst.row < 0)
			cellFirst.row = 0;

		if (cellFirst.col < 0) // error
		{
			/* vanek BT 596 - 27.10.2003
			nReturnWidth = nWidth - nUserPosX;
			nNewUserPosX = nWidth + 1;
			m_bPrinting = false;
			return false;
			*/   
			cellFirst.col = 0;
		}
 
		nNewUserPosX = nUserPosX;
		CRect cellRect;
		if (GetCellRect(cellFirst, cellRect))
			nNewUserPosX = cellRect.left;

		int nLastCol = cellFirst.col;
		for (nLastCol = cellFirst.col; nLastCol < GetColumnCount(); nLastCol++)
		{
			int nW = GetColumnWidth(nLastCol);
			// check column is full visible
			if ((nNewUserPosX + nW) > (nUserPosX + nMaxWidth))
				break;
			// add column width to return_width 
			nNewUserPosX += nW;
		}
		if (cellFirst.col >= nLastCol) // too long column
		{
			nReturnWidth = nMaxWidth;
			nNewUserPosX = nNewUserPosX + nReturnWidth + 1;
		}
		else
		{
			nReturnWidth = nNewUserPosX - nUserPosX;
			nNewUserPosX += 1;
		}
		bContinue = true;
	}
	else// grid is no shorter that given max_width
	{
		// calc newUserPosX & return width
		// 2 cases: a) whole grid is shorter that given max_width
		// in this case given user_pos == 0 && given new_usr_pos undefined
		//			b) last part of grid is shorter than given max_width
		// in this case user_pos undefined (0 || previous new_usr_pos) && given new_usr_pos undefined
			
		if (nUserPosX == 0)// for case a:
		{
			nReturnWidth = nWidth - nUserPosX;
			nNewUserPosX = nWidth + 1;
			m_bPrinting = false;
			return false;
		}
		
		// else for case b:
		// get first cell
		CCellID cellFirst = GetCellFromPt(CPoint(nNewUserPosX, 0), true);
		if (cellFirst.row < 0)
			cellFirst.row = 0;

		if (cellFirst.col < 0) // error
		{
			/* vanek
			nReturnWidth = nWidth - nUserPosX;
			nNewUserPosX = nWidth + 1;
			m_bPrinting = false;
			return false;		*/
			cellFirst.col = 0;
		}

		nNewUserPosX = nUserPosX;
		CRect cellRect;
		if (GetCellRect(cellFirst, cellRect))
			nNewUserPosX = cellRect.left;

		int nLastCol = cellFirst.col;
		for (nLastCol = cellFirst.col; nLastCol < GetColumnCount(); nLastCol++)
			nNewUserPosX += GetColumnWidth(nLastCol);

		nReturnWidth = nNewUserPosX - nUserPosX;
		nNewUserPosX += 1;
		bContinue = false;
	}
	m_bPrinting = false;
	return bContinue;
}

bool CGridCtrl::OnGetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
{
	m_bPrinting = true;
	// get virtual height
	long nHeight = (long)GetVirtualHeight();
	bool bContinue = false;
	// grid is longer that given max_height
	if (nHeight > nUserPosY + nMaxHeight)
	{
		// calc last printed column
		CCellID cellFirst = GetCellFromPt(CPoint(0, nUserPosY), true);

		if (cellFirst.col < 0)
			cellFirst.col = 0;

		if (cellFirst.row < 0) 
		{
			/* vanek BT 596 - 27.10.2003
			nReturnHeight = nHeight - nUserPosY;
			nNewUserPosY = nHeight + 1;
			m_bPrinting = false;
			return false; */
			cellFirst.row = 0;
		}

		nNewUserPosY = nUserPosY;
		CRect cellRect;
		if (GetCellRect(cellFirst, cellRect))
			nNewUserPosY = cellRect.top;

		int nLastRow = cellFirst.row;
		for (nLastRow = cellFirst.row; nLastRow < GetRowCount(); nLastRow++)
		{
			int nH = GetRowHeight(nLastRow);
			if ((nNewUserPosY + nH) > (nUserPosY + nMaxHeight))
				break;
			nNewUserPosY += nH;
		}
		nReturnHeight = nNewUserPosY - nUserPosY;

		if (cellFirst.row >= nLastRow) // too long row
		{
			nReturnHeight = nMaxHeight;
			nNewUserPosY = nUserPosY + nReturnHeight + 1;
		}
		else
		{
			nReturnHeight = nNewUserPosY - nUserPosY;
			nNewUserPosY += 1;
		}

		bContinue = true;
	}
	else// grid is no longer that given max_height
	{
		// calc newUserPosY & return рушпре
		// 2 cases: a) whole grid is shorter that given max_height
		// in this case given user_pos == 0 && given new_usr_pos undefined
		//			b) last part of grid is shorter than given max_height
		// in this case user_pos undefined (0 || previous new_usr_pos) && given new_usr_pos undefined
			
		if (nUserPosY == 0)// for case a:
		{
			nReturnHeight = nHeight - nUserPosY;
			nNewUserPosY = nHeight + 1;
			m_bPrinting = false;
			return false;

		}
		// else for case b:
		// get first cell
		CCellID cellFirst = GetCellFromPt(CPoint(0, nNewUserPosY), true);
		if (cellFirst.col < 0)
			cellFirst.col = 0;

		if (cellFirst.row < 0) // error
		{
			/* vanek
			nReturnHeight = nHeight - nUserPosY;
			nNewUserPosY = nHeight + 1;
			m_bPrinting = false;
			return false;*/
			cellFirst.row = 0;
		}

		nNewUserPosY = nUserPosY;
		CRect cellRect;
		if (GetCellRect(cellFirst, cellRect))
			nNewUserPosY = cellRect.top;

		int nLastRow = cellFirst.row;
		for (nLastRow = cellFirst.row; nLastRow < GetRowCount(); nLastRow++)
			nNewUserPosY += GetRowHeight(nLastRow);

		nReturnHeight = nNewUserPosY - nUserPosY;
		nNewUserPosY += 1;
		bContinue = false;

	}

	m_bPrinting = false;
	return bContinue;
}


#include "\vt5\awin\trace.h"

int CGridCtrl::OnPrintDraw(CDC * pDC, CRect rectTarget, CRect RectDraw)
{
	if (!pDC || !m_hWnd)
		return 0;

//DWORD dw1 = GetTickCount();

	m_bPrinting = true;

	int nSave = pDC->SaveDC();

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportOrg(rectTarget.TopLeft());
	pDC->SetWindowExt(CSize(RectDraw.Width(), RectDraw.Height()));
	pDC->SetViewportExt(CSize(rectTarget.Width(), rectTarget.Height()));

	CRgn ClipRegion;
	if (ClipRegion.CreateRectRgnIndirect(rectTarget))
		pDC->SelectClipRgn(&ClipRegion);

	ClipRegion.DeleteObject();

    // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.
    CRect  rect;
    CBrush FixedBack(GetFixedBkColor()),
           TextBack(GetTextBkColor());


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
				if (IsCellSelected(row, col) && !m_bPrinting)
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
		//[AY] - drawing problem
		//lightPen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW/*COLOR_3DHILIGHT*/));
		lightPen.CreatePen(PS_SOLID, 0, m_crGridColour);
    }
    CATCH (CResourceException, e)
    {
#ifndef _WIN32_WCE
        e->Delete();
#endif
		m_bPrinting = false;
        return 0;
    }
    END_CATCH
    pDC->SelectObject(&lightPen);

    // draw vertical lines (drawn at ends of cells)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
    {
        int x = max(nFixedColWidth, RectDraw.left) - RectDraw.left;//nFixedColWidth;
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

	if (nSave)
		pDC->RestoreDC(nSave);

	m_bPrinting = false;
//DWORD dw2 = GetTickCount();
//message<0>("CGridCtrl::OnPrintDraw %dms", dw2-dw1);


	return rect.bottom;
}


void CGridCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);	

	m_bMouseButtonDown   = FALSE;

	//paul 17.09.2001. Reason if grid have ComboBox cell. Select enable after choose from combo.
	if (m_MouseMode == MOUSE_NOTHING)
		m_bMouseButtonDown   = FALSE;
	
}
