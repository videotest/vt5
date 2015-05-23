#if !defined(AFX_CONVERTIMAGEDLG_H__F85D544A_B1FB_11D3_A552_0000B493A187__INCLUDED_)
#define AFX_CONVERTIMAGEDLG_H__F85D544A_B1FB_11D3_A552_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvertImageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConvertImageDlg dialog

class CConvertImageDlg : public CDialog
{
// Construction
public:
	CConvertImageDlg(CWnd* pParent = NULL);   // standard constructor
	~CConvertImageDlg();

	CString		m_strCurSystem;
	CImageList	m_imgCC;
// Dialog Data
	//{{AFX_DATA(CConvertImageDlg)
	enum { IDD = IDD_DIALOG_CONVERT_IMAGE };
	CStatic	m_stCurIcon;
	CComboBoxEx	m_cbConvertTo;
	CStatic	m_stCurSystem;
	CString	m_stFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConvertImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConvertImageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTIMAGEDLG_H__F85D544A_B1FB_11D3_A552_0000B493A187__INCLUDED_)
