// RecordInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "RecordInfoDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CRecordInfoDlg dialog


CRecordInfoDlg::CRecordInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecordInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRecordInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordInfoDlg)
	DDX_Control(pDX, IDC_FIELD_LIST, m_ctrlFieldList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecordInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CRecordInfoDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordInfoDlg message handlers

BOOL CRecordInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* pWndFullSize = GetDlgItem( IDC_FULL_SIZE );
	
	if( pWndFullSize )
		pWndFullSize->SetWindowText( "" );

	if( m_prtQuery == 0 || pWndFullSize == 0 )
		return TRUE;

	::SendMessage( m_ctrlFieldList.GetSafeHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, 
						0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	CRect rc;
	m_ctrlFieldList.GetWindowRect( &rc );
	int nWidth = rc.Width() / 3 - 10;
	
	CString str;
	str.LoadString( IDS_TABLE_NAME );
	m_ctrlFieldList.InsertColumn( 0, str, LVCFMT_CENTER, nWidth ); 

	str.LoadString( IDS_FIELD_NAME );
	m_ctrlFieldList.InsertColumn( 1, str, LVCFMT_CENTER, nWidth ); 

	str.LoadString( IDS_FIELD_LEN );	
	m_ctrlFieldList.InsertColumn( 2, str, LVCFMT_CENTER, nWidth );
	
	int nFieldCount = 0;
	m_prtQuery->GetFieldsCount( &nFieldCount );

	int nRow = 0;
	long lSize = 0;
	for( int i=0;i<nFieldCount;i++ )	
	{
		BSTR bstrTable = 0;
		BSTR bstrField = 0;
		m_prtQuery->GetField( i, 0, &bstrTable, &bstrField );

		long lLen = 0;
		m_prtQuery->GetFieldLen( i, &lLen );
		lSize += lLen;		

		CString strLen;
		strLen.Format( "%d", lLen );

		CString strTable = bstrTable;
		CString strField = bstrField;

		if( bstrTable )
			::SysFreeString( bstrTable );	bstrTable = 0;

		if( bstrField )
			::SysFreeString( bstrField );	bstrField = 0;

		LVITEM lvitem;
		::ZeroMemory( &lvitem, sizeof(LVITEM) );


		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = nRow;		 
		
		lvitem.iSubItem = 0;
		lvitem.pszText = (char*)(LPCSTR)strTable;
		m_ctrlFieldList.InsertItem(&lvitem);

		lvitem.iSubItem = 1;
		lvitem.pszText = (char*)(LPCSTR)strField;
		m_ctrlFieldList.SetItem(&lvitem);

		lvitem.iSubItem = 2;
		lvitem.pszText = (char*)(LPCSTR)strLen;
		m_ctrlFieldList.SetItem(&lvitem);


		nRow++;
	}
	
	str.Format( "%d", lSize / 1024 );
	if( pWndFullSize )
		pWndFullSize->SetWindowText( str );

	
	return TRUE;
}
