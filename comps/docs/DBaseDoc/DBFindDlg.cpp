// DBFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBFindDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CDBFindDlg dialog


CDBFindDlg::CDBFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBFindDlg)
	m_bFindInField = 0;
	m_bMatchCase = FALSE;
	m_bRegularExpression = FALSE;
	m_strTextToFind = _T("");
	m_bFindFromCurRecord = FALSE;
	m_nFieldIndex = -1;
	//}}AFX_DATA_INIT
}

CDBFindDlg::~CDBFindDlg()
{
	for( int i=0;i<m_arrfi.GetSize();i++ )
		delete m_arrfi[i];

	m_arrfi.RemoveAll();

}


void CDBFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBFindDlg)
	DDX_Radio(pDX, IDC_FIND_IN_FIELD, m_bFindInField);
	DDX_Check(pDX, IDC_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_REGULAR_EXPRESSION, m_bRegularExpression);
	DDX_Text(pDX, IDC_FIND_TEXT, m_strTextToFind);
	DDX_Check(pDX, IDC_FROM_CUR_RECORD, m_bFindFromCurRecord);
	DDX_CBIndex(pDX, IDC_FIELD_NAME, m_nFieldIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBFindDlg, CDialog)
	//{{AFX_MSG_MAP(CDBFindDlg)
	ON_BN_CLICKED(IDC_FIND_IN_FIELD, OnFindInField)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFindDlg message handlers

/////////////////////////////////////////////////////////////////////////////
void CDBFindDlg::SetQueryObject( sptrIQueryObject spQuery )
{
	m_spQuery = spQuery;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBFindDlg::OnInitDialog() 
{	
	CDialog::OnInitDialog();

//	CHelpDlgImpl::InitHelpImlp( GetSafeHwnd(), "DBaseDoc", "DBFindDlg" );

	//Need recover
	
	ASSERT( m_spQuery != NULL );

	if( m_spQuery == NULL )	
		return TRUE;	


	//Filter support
	IDBChildPtr ptrChild( m_spQuery );
	if( ptrChild == NULL )
		return TRUE;


	IUnknown* punkDoc = NULL;
	ptrChild->GetParent( &punkDoc );
	if( punkDoc == NULL )
		return FALSE;

	sptrIDBaseDocument spDBDoc( punkDoc );
	IDBaseStructPtr spDBStruct( punkDoc );
																	
	punkDoc->Release();
						
	if( spDBDoc == NULL || spDBStruct == NULL )
		return TRUE;


	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_FIELD_NAME );
	if( pCombo == NULL || !::IsWindow( pCombo->GetSafeHwnd() ) )
		return TRUE;

	pCombo->ResetContent();



	int nFieldsCount = -1;
	IS_OK( m_spQuery->GetFieldsCount( &nFieldsCount ) );
	for( int i=0;i<nFieldsCount;i++ )
	{
		BSTR bstrTableName, bstrFieldName;
		if( S_OK == ( m_spQuery->GetField( i, 0, &bstrTableName, &bstrFieldName ) ) )
		{					
			CString strTable = bstrTableName;
			CString strField = bstrFieldName;			

			if( bstrTableName )
				::SysFreeString( bstrTableName );

			if( bstrFieldName )
				::SysFreeString( bstrFieldName );

			FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );
			if( !(
				fieldType == ftString ||
				fieldType == ftDigit ||
				fieldType == ftDateTime
				))
				continue;

			BSTR bstrCaption = 0;
			short nType = 0;
			if( S_OK != spDBStruct->GetFieldInfo( _bstr_t((LPCSTR)strTable), 
											_bstr_t((LPCSTR)strField), 
											&bstrCaption, &nType, 0, 0, 0, 0, 0, 0 ) )
				continue;

			_CField* pfi = new _CField;
			m_arrfi.Add( pfi );
			pfi->m_strTable = strTable;
			pfi->m_strField = strField;
			pfi->m_strCaption = bstrCaption;			


			if( bstrCaption )
				::SysFreeString( bstrCaption );	bstrCaption = 0;

			pCombo->AddString( pfi->m_strCaption );


		}
	}
	

	sptrISelectQuery spSelectQuery( m_spQuery );
	if( spSelectQuery == NULL )
		return TRUE;

	BSTR bstrTable, bstrField;
	BSTR bstrTextToFind;

	spSelectQuery->GetFindSettings( &m_bFindInField, &bstrTable, &bstrField,
								&m_bMatchCase, &m_bRegularExpression, &bstrTextToFind  );

	//always get from active view
	//m_strTextToFind = bstrTextToFind;	


	m_bFindInField = !m_bFindInField;

	if( m_strTable.IsEmpty() || m_strField.IsEmpty() )
	{
		::GetActiveFieldFromActiveView( m_strTable, m_strField, m_strTextToFind );
	}

	if( m_strTable.IsEmpty() || m_strField.IsEmpty() )
	{
		m_strTable		= bstrTable;
		m_strField		= bstrField;
	}

	if( m_strTextToFind.IsEmpty() )
		m_strTextToFind = bstrTextToFind;



	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrTextToFind )
		::SysFreeString( bstrTextToFind );

	for( i=0;i<m_arrfi.GetSize();i++ )
	{
		if( m_strTable == m_arrfi[i]->m_strTable && m_strField == m_arrfi[i]->m_strField )
		{
			m_nFieldIndex = i;
			break;
		}
	}

	if( m_strTable.IsEmpty() && m_strField.IsEmpty() && m_nFieldIndex < 0 )
		m_nFieldIndex = 0;

	UpdateData( false );


	_update_controls();
	
	
	return TRUE;
	            
}

void CDBFindDlg::OnOK() 
{	
	UpdateData( true );
	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}

	CDialog::OnOK();
}

LRESULT CDBFindDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
/*	if( CHelpDlgImpl::IsHelpMsg( message ) )
	{
		return CHelpDlgImpl::FilterHelpMsg( message, wParam, lParam );
	}*/
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDBFindDlg::OnFindInField() 
{
	_update_controls();
	
}

void CDBFindDlg::OnRadio2() 
{
	_update_controls();
}

void CDBFindDlg::_update_controls()
{
	if( !GetDlgItem(IDC_FIND_IN_FIELD) || !GetDlgItem(IDC_FIELD_NAME) )
		return;

	if( ((CButton*)GetDlgItem(IDC_FIND_IN_FIELD))->GetCheck() == 0 )
		GetDlgItem(IDC_FIELD_NAME)->EnableWindow( false );
	else
		GetDlgItem(IDC_FIELD_NAME)->EnableWindow( true );
}

