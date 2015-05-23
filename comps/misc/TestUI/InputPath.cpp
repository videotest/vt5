// InputPath.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "InputPath.h"
#include "\vt5\comps\general\data\misc_types.h"



// CInputPath dialog

IMPLEMENT_DYNAMIC(CInputPathDlg, CDialog)
CInputPathDlg::CInputPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputPathDlg::IDD, pParent)
{
	m_str_object_name.Empty();
	m_str_type.Empty();
	m_str_path.Empty();
}

CInputPathDlg::~CInputPathDlg()
{
}

void CInputPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_ctrl_edit_path);
	DDX_Control(pDX, IDC_COMBO_TYPES, m_ctrl_combo_types);
}


BEGIN_MESSAGE_MAP(CInputPathDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPES, OnCbnSelchangeComboTypes)
	ON_EN_CHANGE(IDC_EDIT_PATH, OnEnChangeEditPath)
END_MESSAGE_MAP()


// CInputPath message handlers

BOOL CInputPathDlg::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog::OnInitDialog();

	// set object's name
	SetDlgItemText( IDC_EDIT_OBJ_NAME, m_str_object_name );

	// init types
	m_ctrl_combo_types.ResetContent();
	for( int i = 0; i < sizeof(g_VarCodes)/sizeof(g_VarCodes[0]); i++ )
	{
		if( g_VarCodes[i].vt != VT_NULL && g_VarCodes[i].vt != VT_UNKNOWN && 
			g_VarCodes[i].vt != VT_DISPATCH && g_VarCodes[i].vt != VT_BITS )
			m_ctrl_combo_types.AddString( g_VarCodes[i].szUserName );
	}
	
	// init path
	m_ctrl_edit_path.Clear();

	// disable ok button
	update_ok();

	return TRUE;
}

void CInputPathDlg::OnOK()
{
	GetDlgItemText( IDC_EDIT_PATH, m_str_path );
	GetDlgItemText( IDC_COMBO_TYPES, m_str_type );
	__super::OnOK();
}

void CInputPathDlg::OnCbnSelchangeComboTypes()
{
	update_ok();
}

void CInputPathDlg::OnEnChangeEditPath()
{
	update_ok();	
}

void CInputPathDlg::update_ok()
{
	CWnd *pWnd = 0;
	pWnd = GetDlgItem(IDOK);
	CString str_path( _T("") ); 
	m_ctrl_edit_path.GetWindowText( str_path );
	str_path.TrimLeft();
	if( pWnd )
		pWnd->EnableWindow( CB_ERR != m_ctrl_combo_types.GetCurSel() && !str_path.IsEmpty() );
}

