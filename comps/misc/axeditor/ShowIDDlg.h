#if !defined(AFX_SHOWIDDLG_H__8835EB71_D107_4B64_8EA0_D1C72684CBD2__INCLUDED_)
#define AFX_SHOWIDDLG_H__8835EB71_D107_4B64_8EA0_D1C72684CBD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowIDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowIDDlg dialog
#include "axactions.h"

class CShowIDDlg : public CDialog
{
// Construction
public:
	CShowIDDlg(CAxActionBase *pa, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowIDDlg)
	enum { IDD = IDD_SHOWIDS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowIDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowIDDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CAxActionBase *m_pa;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWIDDLG_H__8835EB71_D107_4B64_8EA0_D1C72684CBD2__INCLUDED_)
