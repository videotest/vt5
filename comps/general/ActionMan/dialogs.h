#if !defined(AFX_DIALOGS_H__B9F5B623_5C60_11D3_A63A_0090275995FE__INCLUDED_)
#define AFX_DIALOGS_H__B9F5B623_5C60_11D3_A63A_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogs.h : header file
//

#include "ScriptEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CEnumDialog dialog

class CEnumDialog : public CDialog
{
// Construction
public:
	CEnumDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnumDialog)
	enum { IDD = IDD_ENUMACTIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnumDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnumDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg dialog

class CScriptDlg : public CDialog
{
// Construction
public:
	CScriptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScriptDlg)
	enum { IDD = IDD_SCRIPTDLG };
	CString	m_strScriptText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptDlg)
	afx_msg void OnPrev();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHscrollScript();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	void RestoreScripts();
	void StoreScripts();

	CStringArray	m_scripts;
	CScriptEdit		m_ScriptEdit;  // subclassed edit control

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGS_H__B9F5B623_5C60_11D3_A63A_0090275995FE__INCLUDED_)
