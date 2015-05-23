// ErrorDlg.cpp : implementation file
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

#include "stdafx.h"
#include "dbasedoc.h"
#include "ErrorDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CErrorsDialog dialog


CErrorsDialog::CErrorsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrorsDialog)
	m_strCode = _T("");
	m_strDescr = _T("");
	m_strMean = _T("");
	m_strSrc = _T("");
	//}}AFX_DATA_INIT

	m_rcSave = NORECT;

}


void CErrorsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorsDialog)
	DDX_Text(pDX, IDC_CODE, m_strCode);
	DDX_Text(pDX, IDC_DECSR, m_strDescr);
	DDX_Text(pDX, IDC_MEAN, m_strMean);
	DDX_Text(pDX, IDC_CODE_SRC, m_strSrc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CErrorsDialog, CDialog)
	//{{AFX_MSG_MAP(CErrorsDialog)
	ON_BN_CLICKED(IDC_SHOW_ADV, OnShowAdv)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CErrorsDialog::InitFromComError( _com_error &e )
{
	_bstr_t bstrSource( e.Source() );
	_bstr_t bstrDescription( e.Description() );

	m_strCode.Format( "%08lx", (LPCSTR)e.Error() );
	m_strMean	= CString( (LPCSTR)e.ErrorMessage() );
	m_strDescr	= CString( (LPCSTR)e.Description() );
	m_strSrc	= CString( (LPCSTR)e.Source() );
	
}

/////////////////////////////////////////////////////////////////////////////
// CErrorsDialog message handlers

BOOL CErrorsDialog::OnInitDialog()
{
	USES_CONVERSION;
	CDialog::OnInitDialog();

	SimpleMode();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CErrorsDialog::SimpleMode()
{
	CWnd* pWndOK = GetDlgItem( IDOK );
	if( !pWndOK )
		return;

	CRect rcClient, rcDlg, rcOK;

	GetClientRect( &rcClient );
	GetWindowRect( &rcDlg );

	ClientToScreen( &rcClient );	
	int nDelta = rcClient.top - rcDlg.top;
	
	m_rcSave = rcDlg;
	pWndOK->GetWindowRect( &rcOK );		
	ScreenToClient( &rcOK );	

	CRect rcNewDlg = rcDlg;

	rcNewDlg.bottom = rcNewDlg.top + nDelta + rcOK.top + rcOK.Height() + 10;

	MoveWindow( &rcNewDlg );

}

/////////////////////////////////////////////////////////////////////////////
void CErrorsDialog::AdvMode()
{
	if( m_rcSave == NORECT )
		return;

	CRect rcDlg;
	
	GetWindowRect( &rcDlg );
	rcDlg.right = rcDlg.left + m_rcSave.Width();
	rcDlg.bottom = rcDlg.top + m_rcSave.Height();

	MoveWindow( &rcDlg );

}

/////////////////////////////////////////////////////////////////////////////
void CErrorsDialog::OnShowAdv() 
{
	AdvMode();	
	
}

