#if !defined(AFX_BAUMERSTATIC_H__2AB8EE80_094D_49A7_B634_9CF360156832__INCLUDED_)
#define AFX_BAUMERSTATIC_H__2AB8EE80_094D_49A7_B634_9CF360156832__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaumerStatic.h : header file
//

#include "input.h"

/////////////////////////////////////////////////////////////////////////////
// CBaumerStatic window

class CBaumerStatic : public CStatic
{
// Construction

	IInputPreview *m_pPreview;
	bool m_bOvrMode;
public:
	CBaumerStatic();


// Attributes
public:
	CSize m_szImg;
	CPoint m_ptBeg;

// Operations
public:
	void SetPreview(IInputPreview *pPreview);
	bool GetOvrMode() {return m_bOvrMode;}
	void SetOvrMode();

	afx_msg void DoLButtonDown(UINT nFlags, CPoint point);
	afx_msg void DoLButtonUp(UINT nFlags, CPoint point);
	afx_msg void DoMouseMove(UINT nFlags, CPoint point);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaumerStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBaumerStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaumerStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAUMERSTATIC_H__2AB8EE80_094D_49A7_B634_9CF360156832__INCLUDED_)
