#if !defined(AFX_TABEDITOR_H__C0A368A4_5EE6_11D3_9999_0000B493A187__INCLUDED_)
#define AFX_TABEDITOR_H__C0A368A4_5EE6_11D3_9999_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabEditor.h : header file
//

class CTab;
class CTabBeam;
/////////////////////////////////////////////////////////////////////////////
// CTabEditor window

class CTabEditor : public CEdit
{
// Construction
public:
	CTabEditor();

// Attributes
public:
	CTab* pEditTab;
	CTabBeam* pTabBeam;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabEditor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabEditor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabEditor)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABEDITOR_H__C0A368A4_5EE6_11D3_9999_0000B493A187__INCLUDED_)
