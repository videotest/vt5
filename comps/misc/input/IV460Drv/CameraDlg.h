#if !defined(AFX_CAMERADLG_H__C51065F0_D433_425D_8C2A_6A3B201EC3A3__INCLUDED_)
#define AFX_CAMERADLG_H__C51065F0_D433_425D_8C2A_6A3B201EC3A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraDlg.h : header file
//

#include "InputInt.h"
#include "DriverWrp.h"

/////////////////////////////////////////////////////////////////////////////
// CCameraDlg dialog

class CCameraDlg : public CDialog
{
// Construction
public:
	CCameraDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCameraDlg)
	enum { IDD = IDD_CAMERA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CDriverWrp m_pDrv;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void UpdateCamera();

	// Generated message map functions
	//{{AFX_MSG(CCameraDlg)
	afx_msg void OnBwCcir();
	afx_msg void OnChangeEFileName();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBwRs170();
	afx_msg void OnUserCamera();
	virtual BOOL OnInitDialog();
	afx_msg void OnRgbCcir();
	afx_msg void OnRgbRs170();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERADLG_H__C51065F0_D433_425D_8C2A_6A3B201EC3A3__INCLUDED_)
