/////////////////////////////////////////////////////////////////////////////
// GridCtrl.h : header file
//
// MFC Grid Control - main header
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
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
//#define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
//#define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines

#include "ControlDef.h"
#include "CellRange.h"
#include "GridCell.h"
#include <afxtempl.h>

#ifdef _WIN32_WCE
#   define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
#   define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
#   define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines
#   define GRIDCONTROL_NO_PRINTING    // Do not use printing routines
#   ifdef WCE_NO_PRINTING
#       define _WIN32_WCE_NO_PRINTING
#   endif
#   ifdef WCE_NO_CURSOR
#       define _WIN32_WCE_NO_CURSOR
#   endif
#endif  // _WIN32_WCE

#ifndef GRIDCONTROL_NO_TITLETIPS
#   include "TitleTip.h"
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
#   include "GridDropTarget.h"
#   undef GRIDCONTROL_NO_CLIPBOARD     // Force clipboard functions on
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
#   include <afxole.h>
#endif

// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

// Used for Get/SetItem calls.
typedef control_export_struct _GV_ITEM {
    int      row,col;     // Row and Column of item
    UINT     mask;        // Mask for use in getting/setting cell data
    UINT     nState;      // cell state (focus/hilighted etc)
    UINT     nFormat;     // Format of cell
    CString  strText;     // Text in cell
    int      iImage;      // index of the list view item’s icon
    COLORREF crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM   lParam;      // 32-bit value to associate with item
    LOGFONT  lfFont;      // Cell font
    UINT     nMargin;     // Internal cell margin
} GV_ITEM;

// Grid line/scrollbar selection
#define GVL_NONE                0L
#define GVL_HORZ                1L
#define GVL_VERT                2L
#define GVL_BOTH                3L

// Cell data mask
#define GVIF_TEXT               LVIF_TEXT
#define GVIF_IMAGE              LVIF_IMAGE
#define GVIF_PARAM              LVIF_PARAM
#define GVIF_STATE              LVIF_STATE
#define GVIF_BKCLR              (GVIF_STATE<<1)
#define GVIF_FGCLR              (GVIF_STATE<<2)
#define GVIF_FORMAT             (GVIF_STATE<<3)
#define GVIF_FONT               (GVIF_STATE<<4)
#define GVIF_MARGIN             (GVIF_STATE<<5)

// Cell states
#define GVIS_FOCUSED            0x0001
#define GVIS_SELECTED           0x0002
#define GVIS_DROPHILITED        0x0004
#define GVIS_READONLY           0x0008
#define GVIS_FIXED              0x0010
#define GVIS_MODIFIED           0x0020
#define GVIS_TRACKED            0x0040

// Cell Searching options
#define GVNI_FOCUSED            0x0001
#define GVNI_SELECTED           0x0002
#define GVNI_DROPHILITED        0x0004
#define GVNI_READONLY           0x0008
#define GVNI_FIXED              0x0010
#define GVNI_MODIFIED           0x0020

#define GVNI_ABOVE              LVNI_ABOVE
#define GVNI_BELOW              LVNI_BELOW
#define GVNI_TOLEFT             LVNI_TOLEFT
#define GVNI_TORIGHT            LVNI_TORIGHT
#define GVNI_ALL                (LVNI_BELOW|LVNI_TORIGHT|LVNI_TOLEFT)
#define GVNI_AREA               (LVNI_BELOW|LVNI_TORIGHT)

// Hit test values (not yet implemented)
#define GVHT_DATA               0x0000
#define GVHT_TOPLEFT            0x0001
#define GVHT_COLHDR             0x0002
#define GVHT_ROWHDR             0x0004
#define GVHT_COLSIZER           0x0008
#define GVHT_ROWSIZER           0x0010
#define GVHT_LEFT               0x0020
#define GVHT_RIGHT              0x0040
#define GVHT_ABOVE              0x0080
#define GVHT_BELOW              0x0100

typedef control_export_struct tagNM_GRIDVIEW {
    NMHDR hdr;
    int   iRow;
    int   iColumn;
} NM_GRIDVIEW;

typedef control_export_struct tagGV_DISPINFO {
    NMHDR   hdr;
    GV_ITEM item;
} GV_DISPINFO;

// Messages sent to the grid's parent (More will be added in future)
#define GVN_BEGINDRAG           LVN_BEGINDRAG        // LVN_FIRST-9
#define GVN_BEGINLABELEDIT      LVN_BEGINLABELEDIT   // LVN_FIRST-5
#define GVN_BEGINRDRAG          LVN_BEGINRDRAG
#define GVN_COLUMNCLICK         LVN_COLUMNCLICK
#define GVN_DELETEITEM          LVN_DELETEITEM
#define GVN_ENDLABELEDIT        LVN_ENDLABELEDIT     // LVN_FIRST-6
#define GVN_SELCHANGING         LVN_ITEMCHANGING
#define GVN_SELCHANGED          LVN_ITEMCHANGED
#define GVN_ACTIVATE			LVN_ITEMACTIVATE

// storage typedef for each row in the grid
typedef CTypedPtrArray<CObArray, CGridCellBase*> GRID_ROW;

// DDX_GridControl is used where a DDX_Control call is needed. In some strange
// situations the usual DDX_Control does not result in CGridCtrl::SubclassWindow
// or CGridCtrl::PreSubclassWindow being called. Using this version calls
// CGridCtrl::SubclassWindow directly - ensuring that cell metrics are set properly
class CGridCtrl;
void AFXAPI DDX_GridControl(CDataExchange* pDX, int nIDC, CGridCtrl& rControl);

typedef double CELLHEIGHT;

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

control_export_class CGridCtrl : public CWnd
{
    DECLARE_DYNCREATE(CGridCtrl)
    friend class CGridCellBase;

// Construction
public:
	CGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
    BOOL SubclassWindow(HWND hWnd);

// Attributes
public:
    int  GetRowCount() const                    { return m_nRows; }
    int  GetColumnCount() const                 { return m_nCols; }
    int  GetFixedRowCount() const               { return m_nFixedRows; }
    int  GetFixedColumnCount() const            { return m_nFixedCols; }
    virtual BOOL SetRowCount(int nRows = 10);
    virtual BOOL SetColumnCount(int nCols = 10);
    virtual BOOL SetFixedRowCount(int nFixedRows = 1);
    virtual BOOL SetFixedColumnCount(int nFixedCols = 1);

    int  GetRowHeight(int nRow) const;
    virtual BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    virtual BOOL SetColumnWidth(int col, int width);

    CELLHEIGHT  GetRowHeight2(int nRow) const;
    BOOL SetRowHeight2(int row, CELLHEIGHT height);
    CELLHEIGHT  GetColumnWidth2(int nCol) const;
    BOOL SetColumnWidth2(int col, CELLHEIGHT width);


    BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p) const;
    BOOL GetCellOrigin(const CCellID& cell, LPPOINT p) const;
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect) const;
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect) const;

    BOOL GetTextRect(const CCellID& cell, LPRECT pRect);
    BOOL GetTextRect(int nRow, int nCol, LPRECT pRect);

    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;
    CELLHEIGHT GetVirtualWidth() const;
    CELLHEIGHT GetVirtualHeight() const;

    CSize GetTextExtent(int nRow, int nCol, LPCTSTR str);
    // EFW - Get extent of current text in cell
    inline CSize GetCellTextExtent(int nRow, int nCol)  { return GetTextExtent(nRow, nCol, NULL); }

	CFont* GetFont()							{	return &m_Font;	}
	CFont* GetBoldFont()						{	return &m_BoldFont;	}

	CCellID GetFirstVisibleCell(int nRow = -1, int nCol = -1);


    void     SetTextColor(COLORREF clr)           { m_crTextColour = clr;             }
    COLORREF GetTextColor() const                 { return m_crTextColour;            }
    void     SetTextBkColor(COLORREF clr)         { m_crTextBkColour = clr;           }
    COLORREF GetTextBkColor() const               { return m_crTextBkColour;          }
    void     SetBkColor(COLORREF clr)             { m_crBkColour = clr;               }
    COLORREF GetBkColor() const                   { return m_crBkColour;              }
    void     SetFixedTextColor(COLORREF clr)      { m_crFixedTextColour = clr;        }
    COLORREF GetFixedTextColor() const            { return m_crFixedTextColour;       }
    void     SetFixedBkColor(COLORREF clr)        { m_crFixedBkColour = clr;          }
    COLORREF GetFixedBkColor() const              { return m_crFixedBkColour;         }
    void     SetGridColor(COLORREF clr)           { m_crGridColour = clr;             }
    COLORREF GetGridColor() const                 { return m_crGridColour;            }

    int GetSelectedCount() const;//                  { return m_SelectedCellMap.GetCount(); }

    virtual CCellID SetFocusCell(CCellID cell);
    virtual CCellID SetFocusCell(int nRow, int nCol);
    CCellID GetFocusCell() const                  { return m_idCurrentCell;           }

    void SetImageList(CImageList* pList)          { m_pImageList = pList;             }
    CImageList* GetImageList() const              { return m_pImageList;              }

    void SetGridLines(int nWhichLines = GVL_BOTH) { m_nGridLines = nWhichLines;
                                                    if (::IsWindow(GetSafeHwnd())) Invalidate(); }
    int  GetGridLines() const                     { return m_nGridLines;              }

	void SetDrawFocus(BOOL bDrawFocus = TRUE)     { m_bDrawFocus = bDrawFocus;        }
    BOOL IsDrawFocus() const                      { return m_bDrawFocus;              }
    void SetEditable(BOOL bEditable = TRUE)       { m_bEditable = bEditable;          }
    BOOL IsEditable() const                       { return m_bEditable;               }
    void SetListMode(BOOL bEnableListMode = TRUE);
    BOOL GetListMode() const                      { return m_bListMode;               }
    void SetSingleRowSelection(BOOL bSing = TRUE) { m_bSingleRowSelection = bSing;    }
    BOOL GetSingleRowSelection()                  { return m_bSingleRowSelection && m_bListMode; }
    void EnableSelection(BOOL bEnable = TRUE)     { ResetSelectedRange(); m_bEnableSelection = bEnable; ResetSelectedRange(); }
    BOOL IsSelectable() const                     { return m_bEnableSelection;        }
    void EnableDragAndDrop(BOOL bAllow = TRUE)    { m_bAllowDragAndDrop = bAllow;     }
    BOOL GetDragAndDrop() const                   { return m_bAllowDragAndDrop;       }
    void SetRowResize(BOOL bResize = TRUE)        { m_bAllowRowResize = bResize;      }
    BOOL GetRowResize() const                     { return m_bAllowRowResize;         }
    void SetColumnResize(BOOL bResize = TRUE)     { m_bAllowColumnResize = bResize;   }
    BOOL GetColumnResize() const                  { return m_bAllowColumnResize;      }
    void SetHeaderSort(BOOL bSortOnClick = TRUE)  { m_bSortOnClick = bSortOnClick;    }
    BOOL GetHeaderSort() const                    { return m_bSortOnClick;            }
    void SetHandleTabKey(BOOL bHandleTab = TRUE)  { m_bHandleTabKey = bHandleTab;     }
    BOOL GetHandleTabKey() const                  { return m_bHandleTabKey;           }
    void SetDoubleBuffering(BOOL bBuffer = TRUE)  { m_bDoubleBuffer = bBuffer;        }
    BOOL GetDoubleBuffering() const               { return m_bDoubleBuffer;           }
    void EnableTitleTips(BOOL bEnable = TRUE)     { m_bTitleTips = bEnable;           }
    BOOL GetTitleTips()                           { return m_bTitleTips;              }
    void SetSortColumn(int nCol)                  { m_nSortColumn = nCol;             }
    int  GetSortColumn() const                    { return m_nSortColumn;             }
    void SetSortAscending(BOOL bAscending)        { m_bAscending = bAscending;        }
    BOOL GetSortAscending() const                 { return m_bAscending;              }

    void EnableWysiwygPrinting(BOOL bEnable = TRUE){ m_bWysiwygPrinting = bEnable;    }
    BOOL GetWysiwygPrinting()                     { return m_bWysiwygPrinting;        }
    void EnableHiddenColUnhide(BOOL bEnable = TRUE){ m_bHiddenColUnhide = bEnable;    }
    BOOL GetHiddenColUnhide()                     { return m_bHiddenColUnhide;        }
    void EnableHiddenRowUnhide(BOOL bEnable = TRUE){ m_bHiddenRowUnhide = bEnable;    }
    BOOL GetHiddenRowUnhide()                     { return m_bHiddenRowUnhide;        }

    void SetDefCellMargin( int nMargin)           { m_nDefCellMargin = nMargin;       }
    int GetDefCellMargin() const                  { return m_nDefCellMargin;          }

    int GetDefCellHeight() const                  { return m_nDefCellHeight;          }
    void SetDefCellHeight(int nHeight)            { m_nDefCellHeight = nHeight;       }
    int GetDefCellWidth() const                   { return m_nDefCellWidth;           }
    void SetDefCellWidth(int nWidth)              { m_nDefCellWidth = nWidth;         }

    void SetModified(BOOL bModified = TRUE, int nRow = -1, int nCol = -1);
    BOOL GetModified(int nRow = -1, int nCol = -1);
    BOOL IsCellFixed(int nRow, int nCol);

    CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!

    BOOL   SetItem(const GV_ITEM* pItem);
    BOOL   GetItem(GV_ITEM* pItem);
    BOOL   SetItemText(int nRow, int nCol, LPCTSTR str);
    // The following was virtual. If you want to override, use 
    //  CGridCellLite-derived class's GetText() to accomplish same thing
    CString GetItemText(int nRow, int nCol) const;

    // EFW - 06/13/99 - Added to support printf-style formatting codes.
    // Also supports use with a string resource ID
#if (_WIN32_WCE >= 210)
    BOOL   SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...);
    BOOL   SetItemTextFmtID(int nRow, int nCol, UINT nID, ...);
#endif

    BOOL   SetItemData(int nRow, int nCol, LPARAM lParam);
    LPARAM GetItemData(int nRow, int nCol) const;
    BOOL   SetItemImage(int nRow, int nCol, int iImage);
    int    GetItemImage(int nRow, int nCol) const;
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;
    BOOL   SetItemFormat(int nRow, int nCol, UINT nFormat);
    UINT   GetItemFormat(int nRow, int nCol) const;
    BOOL   SetItemBkColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemBkColour(int nRow, int nCol) const;
    BOOL   SetItemFgColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemFgColour(int nRow, int nCol) const;
    BOOL SetItemFont(int nRow, int nCol, const LOGFONT* lf);
    const LOGFONT* GetItemFont(int nRow, int nCol);

    // Titletip colours - FNA
	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT)  { m_crTTipBackClr = clr;  }
	COLORREF GetTitleTipBackClr()				            { return m_crTTipBackClr; }
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT)  { m_crTTipTextClr = clr;  }
	COLORREF GetTitleTipTextClr()				            { return m_crTTipTextClr; }

    BOOL IsItemEditing(int nRow, int nCol);

    BOOL SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass);
    BOOL SetDefaultCellType( CRuntimeClass* pRuntimeClass);

	int	 GetResizeCaptureRange()			{	return m_nResizeCaptureRange;	}
	void SetResizeCaptureRange(int nSize = 3)	{	 m_nResizeCaptureRange = nSize;	}
	void SettingChange();

// Operations
public:
    virtual int  InsertColumn(LPCTSTR strHeading, UINT nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE,
                      int nColumn = -1);
    virtual int  InsertRow(LPCTSTR strHeading, int nRow = -1);
    virtual BOOL DeleteColumn(int nColumn);
    virtual BOOL DeleteRow(int nRow);
    virtual BOOL DeleteNonFixedRows();
    virtual BOOL DeleteAllItems();
	virtual BOOL MoveColumn(int nCol, int nColAfter);
	virtual BOOL AddAdditionRow();
	virtual BOOL DeleteAdditionRow();


    virtual BOOL AutoSizeRow(int nRow);
    virtual BOOL AutoSizeColumn(int nCol, BOOL bIgnoreHeader = FALSE);
    virtual void AutoSizeRows();
    virtual void AutoSizeColumns();
    virtual void AutoSize(BOOL bIgnoreColumnHeader = FALSE);
    virtual void ExpandColumnsToFit();
    virtual void ExpandRowsToFit();
    virtual void ExpandToFit();

    void Refresh();
    void AutoFill();   // Fill grid with blank cells
	void SetDefaultFont();

    virtual void EnsureVisible(CCellID &cell)       { EnsureVisible(cell.row, cell.col); }
    virtual void EnsureVisible(int nRow, int nCol);
    BOOL IsCellVisible(int nRow, int nCol) const;
    BOOL IsCellVisible(CCellID cell) const;
    BOOL IsCellEditable(int nRow, int nCol) const;
    BOOL IsCellEditable(CCellID &cell) const;
    BOOL IsCellSelected(int nRow, int nCol) const;
    BOOL IsCellSelected(CCellID &cell) const;

    // SetRedraw stops/starts redraws on things like changing the # rows/columns
    // and autosizing, but not for user-intervention such as resizes
    BOOL SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = NULL);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = NULL);
    BOOL RedrawRow(int row);
    BOOL RedrawColumn(int col);

#ifndef _WIN32_WCE
    BOOL Save(LPCTSTR filename);
    BOOL Load(LPCTSTR filename);
#endif

    CCellRange GetCellRange() const;
    CCellRange GetSelectedCellRange() const;
    virtual void SetSelectedRange(const CCellRange& Range, BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    virtual void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
					              BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    BOOL IsValid(int nRow, int nCol) const;
    BOOL IsValid(const CCellID& cell) const;
    BOOL IsValid(const CCellRange& range) const;

#ifndef GRIDCONTROL_NO_CLIPBOARD
    // Clipboard and cut n' paste operations
    virtual void CutSelectedText();
    virtual COleDataSource* CopyTextFromGrid();
    virtual BOOL PasteTextToGrid(CCellID cell, COleDataObject* pDataObject);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
    virtual void OnBeginDrag();
    DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    void OnDragLeave();
    BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void OnEditCut();
    virtual void OnEditCopy();
    virtual void OnEditPaste();
#endif
    virtual void OnEditSelectAll();

    CCellID GetNextItem(CCellID& cell, int nFlags) const;

    BOOL SortTextItems(int nCol, BOOL bAscending);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data = 0);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
// Printing operations
public:
	virtual CCellRange GetCellRangeFromRect(LPRECT pRect);
//print functions
	virtual bool OnGetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
	virtual bool OnGetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	virtual int OnPrintDraw(CDC * pDC, CRect rectTarget, CRect RectDraw);


    void Print();

    // EFW - New printing support functions
    void SetShadedPrintOut(BOOL bEnable = TRUE)     {   m_bShadedPrintOut = bEnable;    }
    BOOL GetShadedPrintOut(void)                    {   return m_bShadedPrintOut;       }

    // Use -1 to have it keep the existing value
    void SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
        int nLeftMargin, int nRightMargin, int nTopMargin,
        int nBottomMargin, int nGap);

    void GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
        int &nLeftMargin, int &nRightMargin, int &nTopMargin,
        int &nBottomMargin, int &nGap);

// Printing overrides for derived classes
public:
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

#endif // #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)

// Implementation
public:
    virtual ~CGridCtrl();

protected:
    BOOL RegisterWindowClass();
    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

public:
    BOOL InvalidateCellRect(const int row, const int col);
    BOOL InvalidateCellRect(const CCellID& cell);
    BOOL InvalidateCellRect(const CCellRange& cellRange);
    void ResetScrolls() {	ResetScrollBars(); }


//protected:
    void EraseBkgnd(CDC* pDC);

    BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect) const;

    BOOL SetCell(int nRow, int nCol, CGridCellBase* pCell);

    int  SetMouseMode(int nMode) { int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int  GetMouseMode() const    { return m_MouseMode; }

    BOOL MouseOverRowResizeArea(CPoint& point) const;
    BOOL MouseOverColumnResizeArea(CPoint& point) const;

    CCellID GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck = TRUE) const;
    CCellID GetTopleftNonFixedCell() const;
	CCellID GetMostLeftFixedColumn() const;
	CCellID GetMostTopFixedRow() const;
    
	CCellRange GetUnobstructedNonFixedCellRange() const;

    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL) const;
	CCellRange GetVisibleFixedColumnRange(LPRECT pRect = NULL) const;
	CCellRange GetVisibleFixedRowRange(LPRECT pRect = NULL) const;

    BOOL IsVisibleVScroll() { return ( (m_nBarState & GVL_VERT) > 0); } 
    BOOL IsVisibleHScroll() { return ( (m_nBarState & GVL_HORZ) > 0); }
    virtual void ResetSelectedRange();
    virtual void ResetScrollBars();
    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
    int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE) const;
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

    BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                   int low, int high);

    CPoint GetPointClicked(int nRow, int nCol, const CPoint& point);

// Overrrides
protected:

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // Printing
    virtual void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Drag n' drop
    virtual CImageList* CreateDragImage(CPoint *pHotSpot);    // no longer necessary
#endif

    // Mouse Clicks
    virtual void  OnFixedColumnClick(CCellID& cell);
    virtual void  OnFixedRowClick(CCellID& cell);

    // Editing
    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
    virtual void  OnEditCell(int nRow, int nCol, CPoint point, UINT nChar);
    virtual void  EndEditing();

    // Drawing
    virtual void  OnDraw(CDC* pDC);
	virtual void  DrawFixedEmptyCell(CDC * pDC, int nRow, int nCol, CRect rect);
	virtual void  DrawEmptyCell(CDC * pDC, int nRow, int nCol, CRect rect);


    // GridCell Creation and Cleanup
    virtual CGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);

// Attributes
protected:
    // General attributes
    COLORREF    m_crTextColour, m_crTextBkColour, m_crBkColour,   // Grid colours
                m_crFixedTextColour, m_crFixedBkColour, m_crGridColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;
    COLORREF    m_crTTipBackClr, m_crTTipTextClr;                 // Titletip colours - FNA
    
    int         m_nGridLines;
    BOOL        m_bEditable;
	BOOL        m_bDrawFocus;
    BOOL        m_bModified;
    BOOL        m_bAllowDragAndDrop;
    BOOL        m_bListMode;
    BOOL        m_bSingleRowSelection;
    BOOL        m_bAllowDraw;
    BOOL        m_bEnableSelection;
    BOOL        m_bSortOnClick;
    BOOL        m_bHandleTabKey;
    BOOL        m_bDoubleBuffer;
    BOOL        m_bTitleTips;
    int         m_nBarState;
    BOOL        m_bWysiwygPrinting;
    BOOL        m_bHiddenColUnhide;
    BOOL        m_bHiddenRowUnhide;
    BOOL        m_bAutoSizeSkipColHdr;

    // Cell size details
    int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CArray<CELLHEIGHT,CELLHEIGHT>  m_arRowHeights, m_arColWidths;
    int         m_nDefCellMargin;
    int         m_nDefCellWidth, m_nDefCellHeight;
    int         m_nVScrollMax, m_nHScrollMax;

    // Fonts and images
    LOGFONT     m_Logfont;
    CFont       m_PrinterFont,  // for the printer
                m_Font,			// for the grid
				m_BoldFont;
				
    CImageList* m_pImageList;

    // Cell data
    CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;

    // Mouse operations such as cell selection
    int         m_MouseMode;
    BOOL        m_bMouseButtonDown;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    CCellID     m_LeftClickDownCell, m_SelectionStartCell;
    CCellID     m_idCurrentCell;
    CCellID		m_OverCell;

    int         m_nHTimerID;
    int         m_nVTimerID;
    int         m_nTimerInterval;
    int         m_nResizeCaptureRange;
	CPoint		m_ptStartResize;
    BOOL        m_bAllowRowResize, m_bAllowColumnResize;
    int         m_nRowsPerWheelNotch;
    CMap<DWORD,DWORD, CCellID, CCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;
	CSelection  m_Sel, m_PrevSel;

    CRuntimeClass* m_pRtcDefault;       // determines kind of CGridCellLite created by default

#ifndef GRIDCONTROL_NO_TITLETIPS
    CTitleTip   m_TitleTip;             // Title tips for cells
#endif

    // Drag and drop
    BOOL        m_bMustUninitOLE;       // Do we need to uninitialise OLE?
    CCellID     m_LastDragOverCell;
#ifndef GRIDCONTROL_NO_DRAGDROP
    CGridDropTarget m_DropTarget;       // OLE Drop target for the grid
#endif

    // Printing information
    CSize       m_CharSize;
    int         m_nPageHeight;
    CSize       m_LogicalPageSize,      // Page size in gridctrl units.
                m_PaperSize;            // Page size in device units.
    // additional properties to support Wysiwyg printing
    int         m_nPageWidth;
    int         m_nPrintColumn;
    int         m_nCurrPrintRow;
    int         m_nNumPages;
    int         m_nPageMultiplier;

    // sorting
    int         m_bAscending;
    int         m_nSortColumn;

    // EFW - Added to support shaded/unshaded printout.  If true, colored
    // cells will print as-is.  If false, all text prints as black on white.
    BOOL        m_bShadedPrintOut;

    // EFW - Added support for user-definable margins.  Top and bottom
    // are in lines.  Left, right, and gap are in characters (avg width
    // is used).
    int         m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
                m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;

protected:
    void SelectAllCells();
    void SelectColumns(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectRows(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectCells(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void OnSelecting(const CCellID& currentCell);

    // Generated message map functions
    //{{AFX_MSG(CGridCtrl)
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnCancelMode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif
#ifndef _WIN32_WCE
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);    // EFW - Added
    afx_msg void OnSysColorChange();
#endif
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg void OnCaptureChanged(CWnd *pWnd);
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
#endif
#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#endif
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

    enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                       MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                       MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                       MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                       MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
                       MOUSE_PREPARE_DRAG, MOUSE_DRAGGING,
					   MOUSE_PREPARE_COLUMN_DRAG, MOUSE_COLUMN_DRAGGING
#endif
    };

public:
	bool SetItemEmpty(int nRow, int nCol);
	bool IsItemEmpty(int nRow, int nCol);

    CArray<CELLHEIGHT,CELLHEIGHT&>& GetWidths()		{	return 	m_arrSaveWidths;	}

	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	CBrush*	GetLightBrush()	{	return &m_brLight;	}

    void			EnableDragColumn(BOOL bAllow = TRUE)     { m_bAllowDragColumn = bAllow;     }
    BOOL			GetDragColumn() const                    { return m_bAllowDragColumn;       }

	virtual int		GetFirstDragableColumn()	{	return GetFixedColumnCount();	}
	virtual bool	EnableColumnDrag(int nCol);
    virtual void	OnBeginColumnDrag();
	virtual void	OnEndColumnDrag(int nCol, int nColAfter);

	CImageList *	CreateDragColumn(int nColumn, CPoint & pt);
	BOOL			DrawCellDrag(CDC* pDC, int nRow, int nCol, CRect rect);

	void			DrawMarker(int nCol1, bool bRight, bool bDraw);
	void			SetMarkerSize(int nSize)		{	m_nMarkerSize = nSize;	}
	int				GetMarkerSize()					{	return m_nMarkerSize;	}

	void			SetMarkerColor(COLORREF	color)	{	m_nMarkerColor = color;	}
	COLORREF		GetMarkerColor()				{	return m_nMarkerColor;	}

	void SetAdditionRow(bool bFlag = true)	{	m_bEnableAdditionRow = bFlag;	}
	bool EnableAdditionRow()					{	return m_bEnableAdditionRow;	}
	bool ExistsAdditionRow() const;
	int  GetAdditionRow() const;

protected:

	virtual void OnSetFocusCell(CCellID cell);
	virtual void OnSetFocusCell(int nRow, int nCol);

	void UpdateSizes();
	void RetrieveSizes();
	CELLHEIGHT GetSaveWidth(int nIndex);
	void SetSaveWidth(int nIndex, CELLHEIGHT nSize);

	void	CreateLightBrush();
	HBITMAP	CreateDitherBitmap(HDC hDC);

protected:
	CArray<CELLHEIGHT,CELLHEIGHT&> m_arrSaveWidths;		
	CBrush		m_brLight;


	int			m_nDragColumn;
	int			m_nDragOverColumn;
	bool		m_bDragColumn;
	BOOL        m_bAllowDragColumn;
	CImageList *m_pDragList;
	CPoint		m_ptDragOffset;
    UINT		m_nDragTimerID;
	CPoint		m_ptMove;

	int			m_nMarkerSize;
	COLORREF	m_nMarkerColor;
	bool		m_bPrevRight;


	GRID_ROW	m_AdditionRow;
	bool		m_bEnableAdditionRow;
	bool		m_bPrinting;
protected:
	void EnableScrollBarCtrl( int nBar, BOOL bEnable );
	BOOL GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL);
	void GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const;
	void SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE );
	BOOL SetScrollInfo( int nBar, SCROLLINFO *psi, BOOL bRedraw = TRUE );
};

inline CGridCellBase* CGridCtrl::GetCell(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return NULL;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return NULL;
    return pRow->GetAt(nCol);
}

inline BOOL CGridCtrl::SetCell(int nRow, int nCol, CGridCellBase* pCell)
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

    pCell->SetCoords( nRow, nCol); 
    pRow->SetAt(nCol, pCell);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
