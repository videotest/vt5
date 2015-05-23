#if !defined(AFX_NUMBERCTRLDLG_H__D3021259_FB28_438B_BBAF_2C87FACBD50E__INCLUDED_)
#define AFX_NUMBERCTRLDLG_H__D3021259_FB28_438B_BBAF_2C87FACBD50E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumberCtrlDlg.h : header file
//
#include "PrintFieldAXCtl.h"

/////////////////////////////////////////////////////////////////////////////
// CNumberCtrlDlg dialog

class CNumberCtrlDlg : public CDialog
{
// Construction
public:
	CNumberCtrlDlg(CWnd* pParent, print_field_params *pparams );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNumberCtrlDlg)
	enum { IDD = IDD_NUMERIC_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumberCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNumberCtrlDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFloat();
	afx_msg void OnInteger();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void _load_data( bool bFloat );
	bool _store_data( bool btest );
	void _update_controls();

	print_field_params	*m_pparams, m_work;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMBERCTRLDLG_H__D3021259_FB28_438B_BBAF_2C87FACBD50E__INCLUDED_)
