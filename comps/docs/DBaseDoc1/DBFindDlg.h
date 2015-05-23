#if !defined(AFX_DBFINDDLG_H__E1050840_3E5E_4A7F_874B_BC5D2108C245__INCLUDED_)
#define AFX_DBFINDDLG_H__E1050840_3E5E_4A7F_874B_BC5D2108C245__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBFindDlg.h : header file
//

#include "FilterCondition.h"

/////////////////////////////////////////////////////////////////////////////
// CDBFindDlg dialog

#include "HelpDlgImpl.h"

class CDBFindDlg : public CDialog,
					public CHelpDlgImpl
{
// Construction
public:
	CDBFindDlg(CWnd* pParent = NULL);   // standard constructor
	~CDBFindDlg();
	void SetQueryObject( sptrIQueryObject spQuery );
	

// Dialog Data
	//{{AFX_DATA(CDBFindDlg)
	enum { IDD = IDD_FIND };
	int		m_bFindInField;
	BOOL	m_bMatchCase;
	BOOL	m_bRegularExpression;
	CString	m_strTextToFind;
	BOOL	m_bFindFromCurRecord;
	int		m_nFieldIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	sptrIQueryObject m_spQuery;
	// Generated message map functions
	//{{AFX_MSG(CDBFindDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFindInField();
	afx_msg void OnRadio2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "DBFind" );

	CArray<_CField*, _CField*> m_arrfi;

public:
	CString m_strTable;
	CString m_strField;


	void _update_controls();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBFINDDLG_H__E1050840_3E5E_4A7F_874B_BC5D2108C245__INCLUDED_)
