#if !defined(AFX_CHOICETABLE_H__FC2EAD09_444C_4CCF_B6BF_D3863EABD22B__INCLUDED_)
#define AFX_CHOICETABLE_H__FC2EAD09_444C_4CCF_B6BF_D3863EABD22B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChoiceTable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChoiceTable dialog

class CChoiceTable : public CDialog
{
// Construction
public:
	CChoiceTable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChoiceTable)
	enum { IDD = IDD_CHOICE_TABLE };
	CListBox	m_ctrlTableList;
	CString	m_strChoiceTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChoiceTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChoiceTable)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "ChoiceTable" );

protected:
	CString m_strTitle;
	CString m_strTable;
	CStringArray m_arTable;


public:
	void AddTable( CString strTable );
	void SetTitle( CString strTitle );
	void SetCurTable( CString strTable );
};

/////////////////////////////////////////////////////////////////////////////
// CAddTableDlg dialog

class CAddTableDlg : public CDialog
{
// Construction
public:
	CAddTableDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddTableDlg)
	enum { IDD = IDD_ADD_TABLE };
	CString	m_strTableName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddTableDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	IMPLEMENT_HELP( "AddTable" );
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOICETABLE_H__FC2EAD09_444C_4CCF_B6BF_D3863EABD22B__INCLUDED_)
