/////////////////////////////////////////////////////////////////////////////
// Titletip.h : header file
#if !defined(AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CTitleTip window

class CTitleTip : public CWnd
{
// Construction
public:
	CTitleTip();
	virtual ~CTitleTip();
	virtual BOOL Create(CWnd *pParentWnd);

// Attributes
public:

// Operations
public:
	void Show(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset = 0, 
			  int nExtent = 0, LPRECT lpHoverRect = NULL, CFont * pFont = NULL,
              COLORREF crTextClr = CLR_DEFAULT, COLORREF crBackClr = CLR_DEFAULT);
    void Hide();
	void StartShow();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleTip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd *	m_pParentWnd;
	CRect	m_rectTitle;
    CRect	m_rectHover;
    DWORD	m_dwLastLButtonDown;
    DWORD	m_dwDblClickMsecs;

	CRect		m_rectShow;
	COLORREF	m_clrBack;
	COLORREF	m_clrText;
	CFont *		m_pFont;
	CString 	m_strShowTitle;
	UINT		m_nTimer;
	int			m_nCount;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTitleTip)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_)
