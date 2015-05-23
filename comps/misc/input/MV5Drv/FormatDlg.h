#if !defined(AFX_FORMATDLG_H__0FBD8AC0_A69E_11D3_A98A_8B62A3EB5832__INCLUDED_)
#define AFX_FORMATDLG_H__0FBD8AC0_A69E_11D3_A98A_8B62A3EB5832__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormatDlg dialog

class CFormatDlg : public CDialog
{
// Construction
public:
	CFormatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFormatDlg)
	enum { IDD = IDD_FORMAT };
	CComboBox	m_Format;
	BOOL	m_bGrayScale;
	BOOL	m_bVerticalMirror;
	BOOL	m_bHorizontalMirror;
	int		m_nScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormatDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATDLG_H__0FBD8AC0_A69E_11D3_A98A_8B62A3EB5832__INCLUDED_)
