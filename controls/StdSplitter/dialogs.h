#if !defined(AFX_DIALOGS_H__3E912141_605D_11D3_9999_0000B493A187__INCLUDED_)
#define AFX_DIALOGS_H__3E912141_605D_11D3_9999_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameDialog dialog

class CNameDialog : public CDialog
{
// Construction
public:
	CNameDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNameDialog)
	enum { IDD = IDD_DIALOGNAME };
	CString	m_strName;
	//}}AFX_DATA

	CString m_strTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGS_H__3E912141_605D_11D3_9999_0000B493A187__INCLUDED_)
