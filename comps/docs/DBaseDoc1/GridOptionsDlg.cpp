// GridOptions.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "GridOptionsDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CBrowseGrid dialog
CBrowseGrid::CBrowseGrid()
{

}

/////////////////////////////////////////////////////////////////////////////
// CGridOptionsDlg dialog
CGridOptionsDlg::CGridOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridOptionsDlg)
	m_bUseFilterRow = FALSE;
	m_bUseSortRow = FALSE;
	//}}AFX_DATA_INIT
}

CGridOptionsDlg::~CGridOptionsDlg()
{

}


void CGridOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridOptionsDlg)
	DDX_Check(pDX, IDC_USE_FILTER_ROW, m_bUseFilterRow);
	DDX_Check(pDX, IDC_USE_SORT_ROW, m_bUseSortRow);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CGridOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CGridOptionsDlg)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CGridOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//need recover
	/*
	
	InitSourceGrid();
	InitTargetGrid();

	if( m_spActiveQuery )
	{
		m_spActiveQuery->GetUseFilterRowInGrid( &m_bUseFilterRow );
		m_spActiveQuery->GetUseSortRowInGrid( &m_bUseSortRow );		
	}
	*/
	
	


	SetButtonAvailable();	
	
	return TRUE;
}

void CGridOptionsDlg::SetActiveQuery( IUnknown* pUnkActiveQuery )
{
	m_spActiveQuery = pUnkActiveQuery;
}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::SetButtonAvailable()
{
	BOOL bAvailableADD_ALL = FALSE;
	BOOL bAvailableREMOVE_ALL = FALSE;
	BOOL bAvailableADD = FALSE;
	BOOL bAvailableREMOVE = FALSE;

	
	int nSourseRowCount		= m_ctrlGridSource.GetRowCount();
	int nSourseRowCurrent	= m_ctrlGridSource.GetFocusCell().row;
	
	int nTargetRowCount		= m_ctrlGridTarget.GetRowCount();
	int nTargetRowCurrent	= m_ctrlGridTarget.GetFocusCell().row;

	if( nSourseRowCount > 1 )
		bAvailableADD_ALL = TRUE;

	if( nSourseRowCurrent >= 1 )
		bAvailableADD = TRUE;

	if( nTargetRowCount > 1 )
		bAvailableREMOVE_ALL = TRUE;

	if( nTargetRowCurrent >= 1 )
		bAvailableREMOVE = TRUE;



	GetDlgItem(IDC_ADD_ALL)->EnableWindow( bAvailableADD_ALL );
	GetDlgItem(IDC_REMOVE_ALL)->EnableWindow( bAvailableREMOVE_ALL );
	GetDlgItem(IDC_ADD)->EnableWindow( bAvailableADD );
	GetDlgItem(IDC_REMOVE)->EnableWindow( bAvailableREMOVE );
	
}


/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::InitSourceGrid()
{
	//Set position
	CWnd* pWnd = GetDlgItem(IDC_STATIC_SOURCE_GRID);
	ASSERT( pWnd );
	CRect rc;


	pWnd->GetWindowRect( &rc );
	ScreenToClient( &rc );

	m_ctrlGridSource.Create( rc, this, GRID_SOURCE_ID );	
									 
	//m_ctrlGridSource.SetAdditionRow( FALSE );

	m_ctrlGridSource.InsertColumn( "Table" );
	m_ctrlGridSource.InsertColumn( "Field" );
	m_ctrlGridSource.InsertColumn( "Caption" );


	int nColumnWidth = rc.Width() / 3;
	m_ctrlGridSource.SetColumnWidth( 0, nColumnWidth );
	m_ctrlGridSource.SetColumnWidth( 1, nColumnWidth );
	m_ctrlGridSource.SetColumnWidth( 2, rc.Width() - nColumnWidth * 2 - 2 );	


	m_ctrlGridSource.SetFixedRowCount(1);
	m_ctrlGridSource.SetSingleRowSelection( FALSE );
	m_ctrlGridSource.SetEditable( FALSE );

	if( m_spActiveQuery == NULL )
		return;


	//Filter support
	IDBChildPtr ptrChild( m_spActiveQuery );
	if( ptrChild == NULL )
		return;


	
	IUnknown* punkDoc = NULL;
	ptrChild->GetParent( &punkDoc );
	if( punkDoc == NULL )
		return;

	sptrIDBaseDocument spDBDoc( punkDoc );
																	
	punkDoc->Release();
						
	if( spDBDoc == NULL )
		return;

	/*
	sptrINamedDataObject2 spNDO2( m_spActiveQuery );
	if( spNDO2 == NULL)
		return;

	IUnknown* pUnkDoc = NULL;
	spNDO2->GetData( &pUnkDoc );
	if( pUnkDoc == NULL )
		return;

	sptrIDBaseDocument spDBDoc( pUnkDoc );
	
	pUnkDoc->Release();

	if( spDBDoc == NULL )
		return;	
		*/

	int nRow = 1;

	int nFieldCount = 0;
	m_spActiveQuery->GetFieldsCount( &nFieldCount );
	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		m_spActiveQuery->GetField( i, 0, &bstrTable, &bstrField );
		
		CString strTable = bstrTable;
		CString strField = bstrField;

		FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );
		CString strCaption  = ::GetFieldCaption( spDBDoc, strTable, strField );

		if( fieldType == ftDigit ||
			fieldType == ftString ||
			fieldType == ftDateTime 
			)
		{
			
			//int nRow = m_ctrlGridSource.InsertRow( "", -1 );
			nRow++;
			m_ctrlGridSource.SetRowCount( nRow );
			
			m_ctrlGridSource.SetItemText( nRow-1, 0, strTable );
			m_ctrlGridSource.SetItemText( nRow-1, 1, strField );
			m_ctrlGridSource.SetItemText( nRow-1, 2, strCaption );			
		}


		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
	}

}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::InitTargetGrid()
{
	//Set position
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TARGET_GRID);
	ASSERT( pWnd );
	CRect rc;

	pWnd->GetWindowRect( &rc );
	ScreenToClient( &rc );


	m_ctrlGridTarget.Create( rc, this, GRID_TARGET_ID );	
	
	m_ctrlGridTarget.InsertColumn( "Table" );
	m_ctrlGridTarget.InsertColumn( "Field" );
	m_ctrlGridTarget.InsertColumn( "Header" );
	m_ctrlGridTarget.InsertColumn( "Width" );	

	int nColumnWidth = rc.Width() / 4;
	m_ctrlGridTarget.SetColumnWidth( 0, nColumnWidth );
	m_ctrlGridTarget.SetColumnWidth( 1, nColumnWidth );
	m_ctrlGridTarget.SetColumnWidth( 2, nColumnWidth );	
	m_ctrlGridTarget.SetColumnWidth( 3, rc.Width() - nColumnWidth * 3 - 3 );	

	m_ctrlGridTarget.SetFixedRowCount(1);

	if( m_spActiveQuery == NULL )
		return;


	//Filter support
	IDBChildPtr ptrChild( m_spActiveQuery );
	if( ptrChild == NULL )
		return;
	
	IUnknown* punkDoc = NULL;
	ptrChild->GetParent( &punkDoc );
	if( punkDoc == NULL )
		return;

	sptrIDBaseDocument spDBDoc( punkDoc );
																	
	punkDoc->Release();
						
	if( spDBDoc == NULL )
		return;

	//need recover
	/*
	int nGridFieldCount = 0;
	m_spActiveQuery->GetGridFieldCount( &nGridFieldCount );
	for( int i=0;i<nGridFieldCount;i++)
	{
		BSTR bstrTable, bstrField, bstrColumnCaption;
		int nWidth;

		m_spActiveQuery->GetGridField( i, &bstrTable, &bstrField, &bstrColumnCaption, &nWidth );
		
		CString strTable = bstrTable;
		CString strField = bstrField;
		CString strColumnCaption = bstrColumnCaption;		


		int nRow = m_ctrlGridTarget.InsertRow( "" );
		m_ctrlGridTarget.SetItemText( nRow, 0, strTable );
		m_ctrlGridTarget.SetItemText( nRow, 1, strField );
		m_ctrlGridTarget.SetItemText( nRow, 2, strColumnCaption );				

		CString strWidth;
		strWidth.Format( "%d", nWidth );
		m_ctrlGridTarget.SetItemText( nRow, 3, strWidth );
		

		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
		::SysFreeString( bstrColumnCaption );
	}
	*/

}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::SaveChanges()
{

}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::OnAddAll() 
{
	int nSourseRowCount	= m_ctrlGridSource.GetRowCount();
	
	if( nSourseRowCount <= 1 )
		return;

	m_ctrlGridTarget.SetRowCount( 1 );

	for( int i=1;i<nSourseRowCount;i++ )
	{
		//BOOL   SetItemText(int nRow, int nCol, LPCTSTR str);
		CString strTable	= m_ctrlGridSource.GetItemText( i, 0 );
		CString strField	= m_ctrlGridSource.GetItemText( i, 1 );
		CString strCaption	= m_ctrlGridSource.GetItemText( i, 2 );

		m_ctrlGridTarget.SetRowCount( i + 1 );
		m_ctrlGridTarget.SetItemText( i, 0, strTable );
		m_ctrlGridTarget.SetItemText( i, 1, strField );
		m_ctrlGridTarget.SetItemText( i, 2, strCaption );
		m_ctrlGridTarget.SetItemText( i, 3, "80" );
	}
		

	
	//int nSourseRowCurrent	= m_ctrlGridSource.GetFocusCell().row;

}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::OnRemoveAll() 
{
	m_ctrlGridTarget.SetRowCount( 1 );
}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::OnAdd() 
{
	int nSourseRowCurrent	= m_ctrlGridSource.GetFocusCell().row;
	int nSourseRowCount		= m_ctrlGridSource.GetRowCount();
	
	if( nSourseRowCurrent < 1 )
		return;		
	
	int nTargetRowCount		= m_ctrlGridTarget.GetRowCount();

	m_ctrlGridTarget.SetRowCount( nTargetRowCount + 1 );


	CString strTable	= m_ctrlGridSource.GetItemText( nSourseRowCurrent, 0 );
	CString strField	= m_ctrlGridSource.GetItemText( nSourseRowCurrent, 1 );
	CString strCaption	= m_ctrlGridSource.GetItemText( nSourseRowCurrent, 2 );

	
	//nTargetRowCurrent++;
	m_ctrlGridTarget.SetItemText( nTargetRowCount, 0, strTable );
	m_ctrlGridTarget.SetItemText( nTargetRowCount, 1, strField );
	m_ctrlGridTarget.SetItemText( nTargetRowCount, 2, strCaption );
	m_ctrlGridTarget.SetItemText( nTargetRowCount, 3, "80" );	
}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::OnRemove() 
{
	int nTargetRowCurrent	= m_ctrlGridTarget.GetFocusCell().row;
	int nTargetRowCount		= m_ctrlGridTarget.GetRowCount();

	if( nTargetRowCurrent < 1 )
		return;

	m_ctrlGridTarget.DeleteRow( nTargetRowCurrent );


	
}


/////////////////////////////////////////////////////////////////////////////
BOOL CGridOptionsDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( wParam == GRID_SOURCE_ID || wParam == GRID_TARGET_ID  )
	{
		NM_GRIDVIEW* pnmgv = (NM_GRIDVIEW*)lParam;
		if( pnmgv )
		{
			if( pnmgv->hdr.code == GVN_SELCHANGED )
			{
				SetButtonAvailable();
			}
		}
	}
	

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/////////////////////////////////////////////////////////////////////////////
void CGridOptionsDlg::OnOK() 
{	

	UpdateData( TRUE );
	if( m_spActiveQuery == NULL )
		return;

	//need recover
	/*

	int nCount;
	m_spActiveQuery->GetGridFieldCount( &nCount );

	//At first delete
	for( int i=nCount-1;i>=0;i--)		
	{
		m_spActiveQuery->DeleteGridField( i );
	}
	

	int nTargetRowCount	= m_ctrlGridTarget.GetRowCount();
	//Now fill

	for( int iRow=1;iRow<nTargetRowCount;iRow++ )
	{
		CString strTable	= m_ctrlGridTarget.GetItemText( iRow, 0 );
		CString strField	= m_ctrlGridTarget.GetItemText( iRow, 1 );
		CString strHeader	= m_ctrlGridTarget.GetItemText( iRow, 2 );
		CString strWidth	= m_ctrlGridTarget.GetItemText( iRow, 3 );
		int nWidth = atoi( strWidth );

		m_spActiveQuery->AddGridField(
					_bstr_t( (LPCTSTR)strTable ),
					_bstr_t( (LPCTSTR)strField ),
					_bstr_t( (LPCTSTR)strHeader ),
					nWidth
					);
	}

	m_spActiveQuery->SetUseFilterRowInGrid( m_bUseFilterRow );
	m_spActiveQuery->SetUseSortRowInGrid( m_bUseSortRow );
	*/
	
	CDialog::OnOK();
}
