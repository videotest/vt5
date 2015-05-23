#if !defined(AFX_MESSAGEWINDOW_H__DE706043_717B_11D3_A65C_0090275995FE__INCLUDED_)
#define AFX_MESSAGEWINDOW_H__DE706043_717B_11D3_A65C_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageWindow.h : header file
//
#include "tooltipwnd.h"
#include "monsteractions.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageWindow window

class CMessageWindow : public CWnd
{
// Construction
public:
	CMessageWindow( CActionMonsterMessage *pa );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageWindow)
	public:
	virtual BOOL Create( CWnd* pParentWnd, const char *szMessage );
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMessageWindow();
	void SetMessageText( const char *szMessage );
	void PlaceWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMessageWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CActionMonsterMessage	*m_pParent;
	CString	m_strMessage;
	int	m_iWidth, m_iHeight;

	CFont   m_fontText;
	CRgn    m_rgn;
	CRgn    m_rgnTri;
	CRgn    m_rgnComb;	
	CRect   m_rectText;

	COLORREF m_clrBkColor;
	COLORREF m_clrFrameColor;
	COLORREF m_clrTextColor;
	int      m_iFontHeight;
	CString  m_strFontName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEWINDOW_H__DE706043_717B_11D3_A65C_0090275995FE__INCLUDED_)
