#if !defined(__LinePICK_H__)
#define __LinePICK_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// LinePicker.h : header file
//
// Written by Tao Zhu (tzzt@yahoo.com) and Jian Zhu (jianz2@yahoo.com)
// Copyright (c) 1999.

// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.

#include "LineWnd.h"
#include "dashline.h"
/////////////////////////////////////////////////////////////////////////////
// CLinePicker window

void AFXAPI DDX_LinePicker(CDataExchange *pDX, int nIDC, DashStyle &LineType);

/////////////////////////////////////////////////////////////////////////////
// CLinePicker window

class CLinePicker : public CButton
{
// Construction
public:
    CLinePicker();
    DECLARE_DYNCREATE(CLinePicker);

// Attributes
public:
    int GetLine();
    void     SetLine(int LineType); 

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLinePicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CLinePicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track colour changes?
    int      m_Line;
    CRect    m_ArrowRect;

    // Generated message map functions
protected:
    //{{AFX_MSG(CLinePicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    afx_msg LRESULT OnSelEndOK(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelEndCancel(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelChange(WPARAM lParam, LPARAM wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__LinePICK_H__)
