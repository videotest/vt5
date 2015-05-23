#if !defined(AFX_EXECUTEDLG_H__16C00DA3_6720_11D4_AF0A_0000E8DF68C3__INCLUDED_)
#define AFX_EXECUTEDLG_H__16C00DA3_6720_11D4_AF0A_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExecuteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg dialog

class CExecuteDlg : public CDialog
{
// Construction
public:
	CExecuteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExecuteDlg)
	enum { IDD = IDD_EXECUTE_SQL };
	CListCtrl	m_ctrlList;
	CButton	m_ctrlWorkButton;
	CString	m_strSQL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExecuteDlg)
	afx_msg void OnBtnWork();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	IMPLEMENT_HELP( "Execute" );

protected:
	sptrIDBaseDocument m_spDBaseDoc;
public:
	void SetDBaseDocument( IUnknown* pUnkDoc );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXECUTEDLG_H__16C00DA3_6720_11D4_AF0A_0000E8DF68C3__INCLUDED_)
