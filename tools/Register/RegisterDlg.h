// RegisterDlg.h : header file
//

#if !defined(AFX_REGISTERDLG_H__6EC92DEF_1E7C_4B6C_8379_F71B68024EAE__INCLUDED_)
#define AFX_REGISTERDLG_H__6EC92DEF_1E7C_4B6C_8379_F71B68024EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog
#include "Register.h"

class CRegisterDlg : public CDialog
{
// Construction
public:
	CRegisterDlg(CRegisterVersion * pData, CWnd * pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTER_DIALOG };
	CListBox	m_Output;
	CButton		m_btnRemove;
	CButton		m_btnAdd;
	CListBox	m_listCtrl;
	CString		m_strAppName;
	CString		m_strPath;
	CString		m_strSuffix;
	//}}AFX_DATA

	CRegisterVersion * m_pData;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

public:

	void RegClean(char* szDir);
	
	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnRegister();
	afx_msg LRESULT OnFindApp(WPARAM, LPARAM);
	afx_msg void OnBrowse();
	afx_msg void OnUnregister();
	afx_msg void OnExit();
	afx_msg void OnBnClickedClean();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void RegisterFiles(bool bRegister = true);
	// Clean Registry in CLSID where refence to location dir
	void RegCleanLoc(const char* szLocDir);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__6EC92DEF_1E7C_4B6C_8379_F71B68024EAE__INCLUDED_)
