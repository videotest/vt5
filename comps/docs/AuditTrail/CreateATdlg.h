#if !defined(AFX_CREATEATDLG_H__C502FCFD_1032_4735_AEF1_9B59F4241A0C__INCLUDED_)
#define AFX_CREATEATDLG_H__C502FCFD_1032_4735_AEF1_9B59F4241A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateATdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateATdlg dialog

class CCreateATdlg : public CDialog
{
// Construction
public:
	CCreateATdlg(CWnd* pParent = NULL);   // standard constructor

	CStringArray	m_arrImages;

// Dialog Data
	//{{AFX_DATA(CCreateATdlg)
	enum { IDD = IDD_DIALOG_CREATE_AT };
	CListCtrl	m_listImages;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateATdlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateATdlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonSelect();
	afx_msg void OnButtonDeselect();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEATDLG_H__C502FCFD_1032_4735_AEF1_9B59F4241A0C__INCLUDED_)
