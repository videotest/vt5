#if !defined(AFX_SQLTEXTDLG_H__24464CBE_4FD0_4CB4_90C5_31FF91F968E5__INCLUDED_)
#define AFX_SQLTEXTDLG_H__24464CBE_4FD0_4CB4_90C5_31FF91F968E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SQLTextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSQLTextDlg dialog

class CSQLTextDlg : public CDialog
{
// Construction
public:
	CSQLTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSQLTextDlg)
	enum { IDD = IDD_SQL_EDIT_DLG };
	CString	m_SQLText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSQLTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSQLTextDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "SQLText" );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SQLTEXTDLG_H__24464CBE_4FD0_4CB4_90C5_31FF91F968E5__INCLUDED_)
