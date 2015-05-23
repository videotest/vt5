#if !defined(AFX_DSCOMPRDIALOG_H__82007BB5_B5FC_4F0D_8E04_47CD9AFA1F68__INCLUDED_)
#define AFX_DSCOMPRDIALOG_H__82007BB5_B5FC_4F0D_8E04_47CD9AFA1F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSComprDialog.h : header file
//

#include "input.h"

/////////////////////////////////////////////////////////////////////////////
// CDSComprDialog dialog

class CDSComprDialog : public CDialog
{
// Construction
	IDSCompressionSite *m_pSite;
public:
	CDSComprDialog(IDSCompressionSite *pSite, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSComprDialog)
	enum { IDD = IDD_DSCOMPRESSION };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSComprDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDSComprDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSettings();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSCOMPRDIALOG_H__82007BB5_B5FC_4F0D_8E04_47CD9AFA1F68__INCLUDED_)
