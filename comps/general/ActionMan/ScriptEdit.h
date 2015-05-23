#if !defined(AFX_SCRIPTEDIT_H__B4EAED2C_0E11_4770_973D_187201FEF084__INCLUDED_)
#define AFX_SCRIPTEDIT_H__B4EAED2C_0E11_4770_973D_187201FEF084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit window

class CScriptEdit : public CEdit
{
// Construction
public:
	CScriptEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTEDIT_H__B4EAED2C_0E11_4770_973D_187201FEF084__INCLUDED_)
