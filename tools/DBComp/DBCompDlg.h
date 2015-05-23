// DBCompDlg.h : header file
//

#if !defined(AFX_DBCOMPDLG_H__BE3AA192_E0EF_4508_BA3F_6A39310711C7__INCLUDED_)
#define AFX_DBCOMPDLG_H__BE3AA192_E0EF_4508_BA3F_6A39310711C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDBCompDlg dialog

class COwnDaoWorkspace :public CDaoWorkspace
{
public:
	COwnDaoWorkspace(){}
	~COwnDaoWorkspace(){}

	static void PASCAL CompactDatabase(LPCTSTR lpszSrcName,
		LPCTSTR lpszDestName, LPCTSTR lpszLocale, int nOptions,
		LPCTSTR lpszPassword);
protected:
	static void AFX_CDECL InitializeEngine();
};

class CDBCompDlg : public CDialog
{
public:
	CDBCompDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDBCompDlg)
	enum { IDD = IDD_DBCOMP_DIALOG };
	CStatic	m_Status;
	CEdit	m_wndDBPath;
	CButton	m_btnSize;
	CString	m_strDBPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBCompDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDBCompDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFileSelect();
	afx_msg void OnChangeEdit();
	afx_msg void OnStartCompacting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCOMPDLG_H__BE3AA192_E0EF_4508_BA3F_6A39310711C7__INCLUDED_)
