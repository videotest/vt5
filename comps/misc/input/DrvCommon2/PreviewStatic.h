#if !defined(__PreviewStatic_H__)
#define __PreviewStatic_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewStatic.h : header file
//

#include "Input.h"

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic window

class CPreviewStatic : public CStatic
{
	IInputPreview3Ptr m_sptrPrv;
	int m_nDev;
	void WindowToImage(CPoint &pt);
// Construction
public:
	CPreviewStatic(IUnknown *punk, int nDevice);

// Attributes
public:
	CSize m_szPrv;

// Operations
public:
	void InitSizes(bool bSizes);
	void DoLButtonDown(UINT nFlags, CPoint point);
	void DoLButtonUp(UINT nFlags, CPoint point);
	void DoMouseMove(UINT nFlags, CPoint point);
	bool DoSetCursor(UINT nFlags, CPoint point);

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
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
