#if !defined(AFX_VTBUTTON_H__4408E162_F79A_4571_A2D3_29BAD809A71E__INCLUDED_)
#define AFX_VTBUTTON_H__4408E162_F79A_4571_A2D3_29BAD809A71E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVTButton window

class CVTButton : public CButton
{
// Construction
public:
	CVTButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTButton)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_IsArrow;
	CFont m_Font;
	int m_SliderCurrent;
	int m_SliderTo;
	int m_SliderFrom;
	BOOL m_IsUpButton;
	BOOL m_IsMovable;
	BOOL m_IsAutoRepeat;
	BOOL SetBmp(UINT nIDResource);
	virtual ~CVTButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVTButton)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

//	BOOL m_IsDragged;
	POINT m_oldpoint;
	
	BOOL m_IsBitmap;
	CBitmap m_Bitmap;
	BOOL m_Checked;
	BOOL m_IsMouseOver;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTBUTTON_H__4408E162_F79A_4571_A2D3_29BAD809A71E__INCLUDED_)
