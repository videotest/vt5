#if !defined(AFX_EVENTDLG_H__22DF20C4_DA09_11D3_A097_0000B493A187__INCLUDED_)
#define AFX_EVENTDLG_H__22DF20C4_DA09_11D3_A097_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventDlg dialog

class CEventDlg : public CDialog
{
// Construction
public:
	CEventDlg(CWnd* pParent = NULL);   // standard constructor
	~CEventDlg();

	void	SetDlgTitle(CString strTitle){m_strTitle = strTitle;};
	bool	m_bEditCode;
	bool	m_bAnyKilled;

// Dialog Data
	//{{AFX_DATA(CEventDlg)
	enum { IDD = IDD_DIALOG_HANDLERS };
	CListCtrl	m_listEvents;
	//}}AFX_DATA

	CStringArray	m_arrEvents;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditCode();
	afx_msg void OnItemchangedListEvents(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void _OnOK();

	CString m_strTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTDLG_H__22DF20C4_DA09_11D3_A097_0000B493A187__INCLUDED_)
