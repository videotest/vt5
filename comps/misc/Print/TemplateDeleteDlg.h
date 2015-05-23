#if !defined(AFX_TEMPLATEDELETEDLG_H__A06CA45D_1C1B_4DBA_A888_56542431733B__INCLUDED_)
#define AFX_TEMPLATEDELETEDLG_H__A06CA45D_1C1B_4DBA_A888_56542431733B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateDeleteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateDeleteDlg dialog

class CTemplateDeleteDlg : public CDialog
{
// Construction
public:
	CTemplateDeleteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateDeleteDlg)
	enum { IDD = IDD_TEMPLATE_DELETE_DLG };
	CListBox	m_ctrlTemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDeleteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateDeleteDlg)
	afx_msg void OnDeleteBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDELETEDLG_H__A06CA45D_1C1B_4DBA_A888_56542431733B__INCLUDED_)
