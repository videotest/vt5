#if !defined(AFX_ARROWWND_H__)
#define AFX_ARROWWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ArrowWnd.h : header file
//
// Written by Tao Zhu (tzzt@yahoo.com) and Jian Zhu (jianz2@yahoo.com)
// Copyright (c) 1999.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.


// CArrowWnd messages
#define APN_SELCHANGEBT      WM_USER + 1011        // Arrow Picker Selection change
#define APN_DROPDOWN         WM_USER + 1012        // Arrow Picker drop down
#define APN_CLOSEUP          WM_USER + 1013        // Arrow Picker close up
#define APN_SELENDOK         WM_USER + 1014        // Arrow Picker end OK
#define APN_SELENDCANCEL     WM_USER + 1015        // Arrow Picker end (cancelled)

// forward declaration

// To hold the Arrows and their names
typedef struct {
    int crArrow;
    TCHAR    *szName;
} ArrowTableEntry;

//struct ArrowTableEntry;
/////////////////////////////////////////////////////////////////////////////
// CArrowWnd window

class CArrowWnd : public CWnd
{
// Construction
public:
    CArrowWnd();
    CArrowWnd(CPoint p, int arrowType, CWnd* pParentWnd);
    void Initialise();

// Attributes
public:

// Operations
public:
    BOOL Create(CPoint p, int crArrow, CWnd* pParentWnd);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CArrowWnd)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CArrowWnd();

protected:
    BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void FindCellFromArrow(int arrowType);
    void SetWindowSize();
    void CreateToolTips();
    void ChangeSelection(int nIndex);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    int GetArrow(int nIndex)              { return m_crArrows[nIndex].crArrow; }
    LPCTSTR GetArrowName(int nIndex)           { return m_crArrows[nIndex].szName; }
    int  GetIndex(int row, int col) const;
    int  GetRow(int nIndex) const;
    int  GetColumn(int nIndex) const;

// protected attributes
protected:
    static ArrowTableEntry m_crArrows[];
    int            m_nNumArrows;
    int            m_nNumColumns, m_nNumRows;
    int            m_nBoxSize, m_nMargin;
    int            m_nCurrentSel;
    int            m_nChosenArrowSel;
    CRect          m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont          m_Font;
    int            m_crInitialArrow, m_crArrow;
    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;
    CImageList     m_image;
    // Generated message map functions
protected:
    //{{AFX_MSG(CArrowWnd)
    afx_msg void OnNcDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
#if _MSC_VER >= 1300
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWWND_H__)
