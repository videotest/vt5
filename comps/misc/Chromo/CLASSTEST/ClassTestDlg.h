// ClassTestDlg.h : header file
//

#if !defined(AFX_CLASSTESTDLG_H__6BEDBD78_CDDA_4E99_A0EA_4E9322F52479__INCLUDED_)
#define AFX_CLASSTESTDLG_H__6BEDBD78_CDDA_4E99_A0EA_4E9322F52479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClassTestDlg dialog

class CClassTestDlg : public CDialog
{
// Construction
public:
	CClassTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClassTestDlg)
	enum { IDD = IDD_CLASSTEST_DIALOG };
	CListBox	m_Log;
	CString	m_sCompareDir;
	CString	m_sTechDir;
	CString	m_sTestDir;
	CString	m_sScriptPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClassTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnScript();
	afx_msg void OnBrowseCompareDir();
	afx_msg void OnBrowseTestDir();
	afx_msg void OnTeachBrowse();
	afx_msg void OnBrowseScript();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool DoLoadFile();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSTESTDLG_H__6BEDBD78_CDDA_4E99_A0EA_4E9322F52479__INCLUDED_)
