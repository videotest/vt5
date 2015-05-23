#if !defined(AFX_EDITCOMMENT_H__C9C10E28_9354_11D3_A52D_0000B493A187__INCLUDED_)
#define AFX_EDITCOMMENT_H__C9C10E28_9354_11D3_A52D_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditComment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditComment window

class CActionEditComment;
class CEditComment : public CEdit
{
// Construction
public:
	CEditComment(CWnd* pParent, CActionEditComment* pAction, double nFontZoom);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditComment)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditComment();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditComment)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


	void _SetTextInPage(CString strText);


	CString					m_strText;
	CActionEditComment*		m_pAction;
	CWnd*					m_pParent;

public:
	CFont					m_fontText;
	UINT					m_nAlign;
	CPoint					m_ptLastScrollPos;
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCOMMENT_H__C9C10E28_9354_11D3_A52D_0000B493A187__INCLUDED_)
