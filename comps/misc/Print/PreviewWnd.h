#if !defined(AFX_PREVIEWWND_H__B670C1BD_A976_42F8_94D9_F382D01A0008__INCLUDED_)
#define AFX_PREVIEWWND_H__B670C1BD_A976_42F8_94D9_F382D01A0008__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd window

class CPreviewWnd : public CStatic,
					public CScrollZoomSiteImpl
{
// Construction
public:
	CPreviewWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWWND_H__B670C1BD_A976_42F8_94D9_F382D01A0008__INCLUDED_)
