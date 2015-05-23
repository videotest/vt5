// ChoiceTable.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "ChoiceTable.h"


/////////////////////////////////////////////////////////////////////////////
// CChoiceTable dialog


CChoiceTable::CChoiceTable(CWnd* pParent /*=NULL*/)
	: CDialog(CChoiceTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChoiceTable)
	m_strChoiceTable = _T("");
	//}}AFX_DATA_INIT
}


void CChoiceTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChoiceTable)
	DDX_Control(pDX, IDC_TABLE__LIST, m_ctrlTableList);
	DDX_LBString(pDX, IDC_TABLE__LIST, m_strChoiceTable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChoiceTable, CDialog)
	//{{AFX_MSG_MAP(CChoiceTable)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChoiceTable message handlers
/////////////////////////////////////////////////////////////////////////////
void CChoiceTable::AddTable( CString strTable )
{
	m_arTable.Add( strTable );
}

/////////////////////////////////////////////////////////////////////////////
void CChoiceTable::SetTitle( CString strTitle )
{
	m_strTitle = strTitle;
}

/////////////////////////////////////////////////////////////////////////////
void CChoiceTable::SetCurTable( CString strTable )
{
	m_strTable = strTable;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CChoiceTable::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( !m_strTitle.IsEmpty() )
		SetWindowText( m_strTitle );

	m_ctrlTableList.ResetContent();
	for( int i=0;i<m_arTable.GetSize();i++ )
		m_ctrlTableList.AddString( m_arTable[i] );
	
	m_ctrlTableList.FindString( 0, m_strTable );	
	
	return TRUE;	            
}
/////////////////////////////////////////////////////////////////////////////
// CAddTableDlg dialog


CAddTableDlg::CAddTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddTableDlg)
	m_strTableName = _T("");
	//}}AFX_DATA_INIT
}


void CAddTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddTableDlg)
	DDX_Text(pDX, IDC_TABLE, m_strTableName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddTableDlg, CDialog)
	//{{AFX_MSG_MAP(CAddTableDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddTableDlg message handlers
