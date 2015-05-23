#if !defined(AFX_LineWND_H__)
#define AFX_LineWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// LineWnd.h : header file
//
// Written by Jian Zhu (zhu_j4@hotmail.com)
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
// It's free - so you get what you pay for.


// CLineWnd messages
#define LPN_SELCHANGEBT      WM_USER + 1016        // Line Picker Selection change
#define LPN_DROPDOWN         WM_USER + 1017        // Line Picker drop down
#define LPN_CLOSEUP          WM_USER + 1018        // Line Picker close up
#define LPN_SELENDOK         WM_USER + 1019        // Line Picker end OK
#define LPN_SELENDCANCEL     WM_USER + 1020        // Line Picker end (cancelled)

// forward declaration

// To hold the Lines and their names
typedef struct {
    int crLine;
    TCHAR    *szName;
} LineTableEntry;

//struct LineTableEntry;
/////////////////////////////////////////////////////////////////////////////
// CLineWnd window

class CLineWnd : public CWnd
{
// Construction
public:
    CLineWnd();
    CLineWnd(CPoint p, int LineType, CWnd* pParentWnd);
    void Initialise();

// Attributes
public:

// Operations
public:
    BOOL Create(CPoint p, int crLine, CWnd* pParentWnd);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLineWnd)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CLineWnd();

protected:
    BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void FindCellFromLine(int LineType);
    void SetWindowSize();
    void CreateToolTips();
    void ChangeSelection(int nIndex);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    LPCTSTR GetLineName(int nIndex)           { return m_crLines[nIndex].szName; }

// protected attributes
protected:
    static LineTableEntry m_crLines[];
    int            m_nNumLines;
    int            m_nMargin;
    int            m_nCurrentSel;
    int            m_nChosenLineSel;
    CRect          m_WindowRect;
	CSize		   m_BoxSize;
    CFont          m_Font;
    int            m_crInitialLine, m_crLine;
    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;
    CImageList     m_image;
    // Generated message map functions
protected:
    //{{AFX_MSG(CLineWnd)
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

#endif // !defined(AFX_LineWND_H__)
