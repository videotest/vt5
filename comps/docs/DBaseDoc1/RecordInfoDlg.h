#if !defined(AFX_RECORDINFODLG_H__BC09C8D1_B4CE_4D1E_BA61_693E7CCF65AB__INCLUDED_)
#define AFX_RECORDINFODLG_H__BC09C8D1_B4CE_4D1E_BA61_693E7CCF65AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecordInfoDlg dialog

class CRecordInfoDlg : public CDialog
{
// Construction
public:
	CRecordInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecordInfoDlg)
	enum { IDD = IDD_RECORD_INFO };
	CListCtrl	m_ctrlFieldList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecordInfoDlg)
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "RecordInfo" );

	IQueryObjectPtr m_prtQuery;

public:
	void SetQuery( IUnknown* punkQ )
	{
		m_prtQuery = punkQ;
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDINFODLG_H__BC09C8D1_B4CE_4D1E_BA61_693E7CCF65AB__INCLUDED_)
