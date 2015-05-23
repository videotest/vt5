#if !defined(AFX_TEXTCTRLDLG_H__99F573F0_2C4D_4623_8C27_FE8EE48FC107__INCLUDED_)
#define AFX_TEXTCTRLDLG_H__99F573F0_2C4D_4623_8C27_FE8EE48FC107__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextCtrlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextCtrlDlg dialog
#include "PrintFieldAXCtl.h"

class CTextCtrlDlg : public CDialog
{
// Construction
public:
	CTextCtrlDlg(CWnd* pParent, print_field_params *pparams );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextCtrlDlg)
	enum { IDD = IDD_TEXT_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextCtrlDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	print_field_params	*m_pparams;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTCTRLDLG_H__99F573F0_2C4D_4623_8C27_FE8EE48FC107__INCLUDED_)
