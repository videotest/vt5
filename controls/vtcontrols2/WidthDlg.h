#if !defined(AFX_WIDTHDLG_H__E559A8E8_BE9D_4D0A_AFD0_55517AB2582D__INCLUDED_)
#define AFX_WIDTHDLG_H__E559A8E8_BE9D_4D0A_AFD0_55517AB2582D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WidthDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWidthDlg dialog

class CWidthDlg : public CDialog
{
// Construction
public:
	int m_nWidth;
	int m_Width;
	int m_Min;
	int m_Max;
	CWidthDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWidthDlg)
	enum { IDD = IDD_DIALOGWIDTH };
	CEdit	m_edit;
	CSpinButtonCtrl	m_spin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWidthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWidthDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIDTHDLG_H__E559A8E8_BE9D_4D0A_AFD0_55517AB2582D__INCLUDED_)
