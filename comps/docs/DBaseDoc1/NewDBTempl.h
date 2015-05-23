#pragma once

#include "HelpDlgImpl.h"

/////////////////////////////////////////////////////////////////////////////
//
// CNewDBTempl dialog
//
/////////////////////////////////////////////////////////////////////////////
class CNewDBTempl :	public CDialog,
					public CHelpDlgImpl
{
	DECLARE_DYNAMIC(CNewDBTempl)
	IMPLEMENT_HELP( "CNewDBTemplate" );

public:
	CNewDBTempl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewDBTempl();

// Dialog Data
	enum { IDD = IDD_NEW_DBASE_TEMPLATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL	OnInitDialog();
	IUnknownPtr		m_ptr_document;
	CString			m_dbd_path;
	bool			m_bopen;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBrowse();
};
