#if !defined(AFX_RESIZEDLG_H__10339AB3_092D_11D4_A0CA_0000B493A187__INCLUDED_)
#define AFX_RESIZEDLG_H__10339AB3_092D_11D4_A0CA_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

class CResizeDlg : public CDialog
{
// Construction
public:
	CResizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResizeDlg)
	enum { IDD = IDD_DIALOG_RESIZE };
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	//}}AFX_DATA

	long m_nWidth;
	long m_nHeight;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResizeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZEDLG_H__10339AB3_092D_11D4_A0CA_0000B493A187__INCLUDED_)
