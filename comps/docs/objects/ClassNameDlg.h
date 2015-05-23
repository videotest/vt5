#if !defined(AFX_CLASSNAMEDLG_H__F6683D05_EAFD_45EC_9794_52FAB0BF71BA__INCLUDED_)
#define AFX_CLASSNAMEDLG_H__F6683D05_EAFD_45EC_9794_52FAB0BF71BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClassNameDlg dialog

class CClassNameDlg : public CDialog
{
// Construction
public:
	CClassNameDlg(CWnd* pParent = NULL, CString strName = "");   // standard constructor


	CString GetName() {return m_strName;};
	
// Dialog Data
	//{{AFX_DATA(CClassNameDlg)
	enum { IDD = IDD_CLASS_NAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClassNameDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	CString m_strName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSNAMEDLG_H__F6683D05_EAFD_45EC_9794_52FAB0BF71BA__INCLUDED_)
