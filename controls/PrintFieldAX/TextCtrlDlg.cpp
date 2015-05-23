// TextCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "TextCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextCtrlDlg dialog


CTextCtrlDlg::CTextCtrlDlg(CWnd* pParent, print_field_params *pparams )
	: CDialog(CTextCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextCtrlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pparams = pparams;
}


void CTextCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextCtrlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CTextCtrlDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextCtrlDlg message handlers

BOOL CTextCtrlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	if( m_pparams->flags & PFF_TEXT_STYLE_MULTYLINE )
		CheckDlgButton( IDC_MULTYLINE, 1 );
	else if( m_pparams->flags & PFF_TEXT_STYLE_COMBOBOX )
		CheckDlgButton( IDC_COMBOBOX, 1 );
	else if( m_pparams->flags & PFF_TEXT_STYLE_COMBOBOX2 )
		CheckDlgButton( IDC_COMBOBOX_DROPLIST, 1 );
	else
		CheckDlgButton( IDC_SINGLELINE, 1 );

//	CheckDlgButton( IDC_SETDATE, (m_pparams->flags & PFF_TEXT_STYLE_INITBYDATE) == PFF_TEXT_STYLE_INITBYDATE );
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextCtrlDlg::OnOK() 
{
	m_pparams->flags &= ~PFF_TEXT_STYLE_MASK;
	if( IsDlgButtonChecked( IDC_MULTYLINE ) )
		m_pparams->flags |= PFF_TEXT_STYLE_MULTYLINE;
	else if( IsDlgButtonChecked( IDC_COMBOBOX ) )
		m_pparams->flags |= PFF_TEXT_STYLE_COMBOBOX;
	else if( IsDlgButtonChecked( IDC_COMBOBOX_DROPLIST ) )
		m_pparams->flags |= PFF_TEXT_STYLE_COMBOBOX2;
//	if( IsDlgButtonChecked( IDC_SETDATE ) )
//		m_pparams->flags |= PFF_TEXT_STYLE_INITBYDATE;
	
	CDialog::OnOK();
}
