#if !defined(AFX_SHADINGPICKER_H__ECB26D97_B6F8_45BC_A6EB_65B9D58FE79D__INCLUDED_)
#define AFX_SHADINGPICKER_H__ECB26D97_B6F8_45BC_A6EB_65B9D58FE79D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ShadingPicker.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
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

#include "ShadingPopup.h"

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker window

void AFXAPI DDX_ShadingPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour);

/////////////////////////////////////////////////////////////////////////////
// CShadingPicker window

#define CP_MODE_BK   2  // edit background colour (default)

class CShadingPicker : public CButton
{
// Construction
public:
    CShadingPicker();
    DECLARE_DYNCREATE(CShadingPicker);


public:
	void ShowPopup(){
		OnClicked();
	};

// Attributes
public:
    COLORREF GetColour();
    void     SetColour(COLORREF crColour); 

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

    void     SetSelectionMode(UINT nMode)              { m_nSelectionMode = nMode; }
    UINT     GetSelectionMode()                        { return m_nSelectionMode; };

    void     SetBkColour(COLORREF crColourBk);
    COLORREF GetBkColour()                             { return m_crColourBk; }

	void	 SetIndexShading(short nValue);
	short	 GetIndexShading()						   { return m_indexShading; }

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CShadingPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CShadingPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track colour changes?
    COLORREF m_crColourBk;
    COLORREF m_crColourText;
    UINT     m_nSelectionMode;
    CRect    m_ArrowRect;
	short	 m_indexShading;

    // Generated message map functions
protected:
    //{{AFX_MSG(CShadingPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    afx_msg LRESULT OnSelEndOK(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelEndCancel(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelChange(WPARAM lParam, LPARAM wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ShadingPicker_H__D0B75901_9830_11D1_9C0F_00A0243D1382__INCLUDED_)
