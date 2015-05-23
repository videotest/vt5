#if !defined(AFX_FONTVIEWER_H__74982966_A7C0_11D3_A546_0000B493A187__INCLUDED_)
#define AFX_FONTVIEWER_H__74982966_A7C0_11D3_A546_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontViewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontViewer window

class CFontViewer : public CStatic
{
// Construction
public:
	CFontViewer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontViewer();

	void	SetTextColor(COLORREF	color);
	void	SetBackColor(COLORREF	color);
	void	SetTransparent(bool bTransparent);
	void	SetFont(const LOGFONT* lpLogFont);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontViewer)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	COLORREF	m_clrText;
	COLORREF	m_clrBack;
	bool		m_bTransparent;
	LOGFONT		m_Font;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTVIEWER_H__74982966_A7C0_11D3_A546_0000B493A187__INCLUDED_)
