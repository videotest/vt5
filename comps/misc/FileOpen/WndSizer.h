#if !defined(AFX_WNDSIZER_H__EA42ECC5_7977_11D3_B1C5_AC3B6F29064B__INCLUDED_)
#define AFX_WNDSIZER_H__EA42ECC5_7977_11D3_B1C5_AC3B6F29064B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndSizer.h : header file
//
#define WM_OURSIZE	WM_USER+301
/////////////////////////////////////////////////////////////////////////////
// CWndSizer window
class CFileOpenDlg;
enum sizeModes{smNone, smSetCursor, smSizing};
class CWndSizer : public CWnd
{
// Construction
public:
	CWndSizer();

// Attributes
public:
	CFileOpenDlg* m_pParent;	
	sizeModes sizeMode;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndSizer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndSizer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndSizer)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSIZER_H__EA42ECC5_7977_11D3_B1C5_AC3B6F29064B__INCLUDED_)
