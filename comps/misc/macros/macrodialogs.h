#if !defined(AFX_MACRODIALOGS_H__8E8E8349_6B33_11D3_A652_0090275995FE__INCLUDED_)
#define AFX_MACRODIALOGS_H__8E8E8349_6B33_11D3_A652_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// macrodialogs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg dialog

class CEnterNameDlg : public CDialog
{
// Construction
public:
	CEnterNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterNameDlg)
	enum { IDD = IDD_ENTER_NAME };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterNameDlg)
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACRODIALOGS_H__8E8E8349_6B33_11D3_A652_0090275995FE__INCLUDED_)
