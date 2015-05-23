#if !defined(AFX_WIZARDDLG_H__042F37E7_D0FB_4D1E_8BC6_FF5850803876__INCLUDED_)
#define AFX_WIZARDDLG_H__042F37E7_D0FB_4D1E_8BC6_FF5850803876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizardDlg dialog

class CWizardDlg : public CDialog
{
// Construction
public:
	CWizardDlg(CWnd* pParent = NULL, CSize size = CSize(200,200), CString strName = "Wizard");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWizardDlg)
	enum { IDD = IDD_WIZARD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWizardDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnBtnClick(UINT nID);
	DECLARE_MESSAGE_MAP()

protected:
	CSize		m_size;
	CString		m_strName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDDLG_H__042F37E7_D0FB_4D1E_8BC6_FF5850803876__INCLUDED_)
