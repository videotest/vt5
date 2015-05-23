#if !defined(AFX_ERRORDLG_H__EAC8D343_DDCB_11D0_8470_00A0C905A456__INCLUDED_)
#define AFX_ERRORDLG_H__EAC8D343_DDCB_11D0_8470_00A0C905A456__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ErrorDlg.h : header file
//
// This is a part of the Microsoft Foundation Classes and
// Templates (MFC&T).
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// MFC&T Reference and related electronic documentation provided
// with the library.  See these sources for detailed information
// regarding the MFC&T product.
//

/////////////////////////////////////////////////////////////////////////////
// struct MYERRORINFO


/////////////////////////////////////////////////////////////////////////////
// CErrorsDialog dialog

class CErrorsDialog : public CDialog
{
// Construction
public:
	CErrorsDialog(CWnd* pParent = NULL);   // standard constructor
	~CErrorsDialog()
	{
	}

	void InitFromComError( _com_error &e );

// Dialog Data
	//{{AFX_DATA(CErrorsDialog)
	enum { IDD = IDD_ERRORS };
	CString	m_strCode;
	CString	m_strDescr;
	CString	m_strMean;
	CString	m_strSrc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CErrorsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowAdv();
	afx_msg void OnShowSimple();
	afx_msg void OnOksimple();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "Errors" );

	void SimpleMode();
	void AdvMode();

	CRect	m_rcSave;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRORDLG_H__EAC8D343_DDCB_11D0_8470_00A0C905A456__INCLUDED_)
