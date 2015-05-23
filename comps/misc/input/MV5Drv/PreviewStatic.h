#if !defined(AFX_PREVIEWSTATIC_H__FE369220_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_)
#define AFX_PREVIEWSTATIC_H__FE369220_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic window

class CPreviewStatic : public CStatic
{
// Construction
public:
	CPreviewStatic();

// Attributes
public:
	int m_nDevice;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWSTATIC_H__FE369220_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_)
