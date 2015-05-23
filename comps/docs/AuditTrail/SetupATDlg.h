#if !defined(AFX_SETUPATDLG_H__1A5C8746_2A44_4FB8_8A11_8B51947FD9BF__INCLUDED_)
#define AFX_SETUPATDLG_H__1A5C8746_2A44_4FB8_8A11_8B51947FD9BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupATDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupATDlg dialog

class CSetupATDlg : public CDialog
{
// Construction
public:
	CSetupATDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupATDlg)
	enum { IDD = IDD_DIALOG_SETUP };
	CListBox	m_listActions;
	CListBox	m_listAudited;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupATDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupATDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	virtual void OnOK();
	afx_msg void OnSetfocusListAudited();
	afx_msg void OnSetfocusListAvail();
	afx_msg void OnSelchangeListAudited();
	afx_msg void OnSelchangeListAvail();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	void _SetCounters();
private:
	long m_nAuditedCount;
	long m_nActionsCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPATDLG_H__1A5C8746_2A44_4FB8_8A11_8B51947FD9BF__INCLUDED_)
