// FilterNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FilterNameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterNameDlg dialog


CFilterNameDlg::CFilterNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterNameDlg)
	m_strFilterName = _T("");
	//}}AFX_DATA_INIT
}


void CFilterNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterNameDlg)
	DDX_CBString(pDX, IDC_FILTER_NAME, m_strFilterName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterNameDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterNameDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CFilterNameDlg::AddFilter( CString strFilter )
{
	m_arFilters.Add( strFilter );
}

/////////////////////////////////////////////////////////////////////////////
// CFilterNameDlg message handlers
BOOL CFilterNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_FILTER_NAME);
	if( pCombo && pCombo->GetSafeHwnd() )
	{
		for( int i=0;i<m_arFilters.GetSize();i++ )
		{
			pCombo->AddString( m_arFilters[i] );
		}
	}
	
	return TRUE;
}
