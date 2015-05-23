#if !defined(AFX_TETWINDOW_H__6C9D96D2_74C1_486E_B000_277C4477DF92__INCLUDED_)
#define AFX_TETWINDOW_H__6C9D96D2_74C1_486E_B000_277C4477DF92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TetWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTetWindow window
class CTetGame;

class CTetWindow : public CWnd
{
// Construction
public:
	CTetWindow();
	virtual ~CTetWindow();

// Attributes
public:
	CTetGame	*GetGame()	const {return m_pgame;};

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTetWindow)
	//}}AFX_VIRTUAL

// Implementation
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CTetWindow)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTetGame	*m_pgame;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TETWINDOW_H__6C9D96D2_74C1_486E_B000_277C4477DF92__INCLUDED_)
