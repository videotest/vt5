#if !defined(AFX_FORMDIALOG_H__738CFDBC_DF6F_4200_AAD9_3F2A51977FC5__INCLUDED_)
#define AFX_FORMDIALOG_H__738CFDBC_DF6F_4200_AAD9_3F2A51977FC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormDialog dialog
#include "ActiveXSite.h"

class CFormDialog : public CDialog
{
// Construction
public:
	CFormDialog( CWnd *pwndParent = 0 );
	~CFormDialog();

	virtual void SetForm( IUnknown *punkForm );
	afx_msg void OnHelp();

// Dialog Data
	//{{AFX_DATA(CFormDialog)
	enum { IDD = IDD_CONTAINER_DIALOG	};
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CActiveXSite	m_site;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	//}}AFX_MSG
	LRESULT OnCaptionChange(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	virtual void DoCaptionChange(); 

protected:

	CString m_strTitle;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMDIALOG_H__738CFDBC_DF6F_4200_AAD9_3F2A51977FC5__INCLUDED_)
