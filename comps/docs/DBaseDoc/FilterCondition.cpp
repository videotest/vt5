// FilterConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FilterCondition.h"
#include "FilterBase.h"


/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg dialog
CFilterConditionDlg::CFilterConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterConditionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterConditionDlg)
	m_bOrUnion = 0;
	m_strValue = _T("");
	m_strValueTo = _T("");
	m_strCondition = _T("=");
	m_nFieldIndex = -1;
	//}}AFX_DATA_INIT

}

/////////////////////////////////////////////////////////////////////////////
CFilterConditionDlg::~CFilterConditionDlg()
{
	for( int i=0;i<m_arrfi.GetSize();i++ )
		delete m_arrfi[i];

	m_arrfi.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterConditionDlg)
	DDX_Radio(pDX, IDC_AND_UNION, m_bOrUnion);
	DDX_CBString(pDX, IDC_VALUE, m_strValue);
	DDX_CBString(pDX, IDC_VALUE_TO, m_strValueTo);
	DDX_CBString(pDX, IDC_CONDITION, m_strCondition);
	DDX_CBIndex(pDX, IDC_FIELD, m_nFieldIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterConditionDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterConditionDlg)
	ON_CBN_EDITCHANGE(IDC_FIELD, OnEditchangeField)
	ON_CBN_SELCHANGE(IDC_FIELD, OnSelchangeField)
	ON_CBN_EDITCHANGE(IDC_CONDITION, OnEditchangeCondition)
	ON_CBN_SELCHANGE(IDC_CONDITION, OnSelchangeCondition)
	ON_CBN_EDITCHANGE(IDC_VALUE, OnEditchangeValue)
	ON_CBN_SELCHANGE(IDC_VALUE, OnSelchangeValue)
	ON_CBN_EDITCHANGE(IDC_VALUE_TO, OnEditchangeValueTo)
	ON_CBN_SELCHANGE(IDC_VALUE_TO, OnSelchangeValueTo)
	ON_BN_CLICKED(IDC_AND_UNION, OnAndUnion)
	ON_BN_CLICKED(IDC_OR, OnOrUnion)
	ON_CBN_DROPDOWN(IDC_VALUE, OnDropdownValue)
	ON_CBN_DROPDOWN(IDC_VALUE_TO, OnDropdownValueTo)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg message handlers
void CFilterConditionDlg::SetFilterHolder( IUnknown* punk )
{
	m_ptrFilterHolder = punk;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CFilterConditionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT( m_ptrFilterHolder != NULL );

	if( m_ptrFilterHolder == NULL )
		return false;	

	FillTableField();

	if( m_strTable.IsEmpty() || m_strField.IsEmpty() )
	{
		::GetActiveFieldFromActiveView( m_strTable, m_strField, m_strValue );
	}

	for( int i=0;i<m_arrfi.GetSize();i++ )
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

	ShowPreview( false );

	AnalizeValueToEnable( false );
	

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::FillTableField()
{
	if( m_ptrFilterHolder == NULL )
		return;
	
	//field
	CComboBox* pComboField = (CComboBox*)GetDlgItem(IDC_FIELD);
	if( pComboField == NULL )
		return;

	IDBaseStructPtr ptrDBS( m_ptrFilterHolder );
	if( ptrDBS == 0 )
		return;


	int nCount = 0;
	ptrDBS->GetTableCount( &nCount );
	for( int i=0;i<nCount;i++ )
	{
		
		BSTR bstr = 0;
		if( S_OK != ptrDBS->GetTableName( i, &bstr ))
			continue;
		_bstr_t bstrTable( bstr );

		if( bstr )
			::SysFreeString( bstr );	bstr = 0;

		int nFieldCount = 0;
		if( S_OK != ptrDBS->GetFieldCount( bstrTable, &nFieldCount ) )
			continue;
		
		for( int j=0;j<nFieldCount;j++ )
		{
			if( S_OK != ptrDBS->GetFieldName( bstrTable, j, &bstr ) )
				continue;
			_bstr_t bstrField = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;

			BSTR bstrCaption = 0;
			short nType = 0;
			if( S_OK != ptrDBS->GetFieldInfo( bstrTable, bstrField, &bstrCaption, &nType, 0, 0, 0, 0, 0, 0 ) )
				continue;

			FieldType ft = (FieldType)nType;

			if( ft == ftDigit || ft == ftString || ft == ftDateTime )
			{
				_CField* pfi = new _CField;
				m_arrfi.Add( pfi );
				pfi->m_strTable = (LPCSTR)bstrTable;
				pfi->m_strField = (LPCSTR)bstrField;
				pfi->m_strCaption = bstrCaption;

				pComboField->AddString( pfi->m_strCaption );
			}

			if( bstrCaption )
				::SysFreeString( bstrCaption );	bstrCaption = 0;
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnEditchangeField() 
{
	ShowPreview();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnSelchangeField() 
{
	ShowPreview();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnEditchangeCondition() 
{
	ShowPreview();
	AnalizeValueToEnable( );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnSelchangeCondition() 
{
	ShowPreview();
	AnalizeValueToEnable( );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnEditchangeValue() 
{
	ShowPreview();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnSelchangeValue() 
{
	ShowPreview();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnEditchangeValueTo() 
{
	ShowPreview();	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnSelchangeValueTo() 
{
	ShowPreview();
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnAndUnion() 
{	
	ShowPreview();
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnOrUnion() 
{	
	ShowPreview();
}


void CFilterConditionDlg::ShowPreview( bool bUpdateData )
{
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if( pPreview == NULL )
		return;

	if( bUpdateData )
		_UpdateData();

	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}

	CFilterCondition filterCond;	
	filterCond.m_strTable = m_strTable;
	filterCond.m_strField = m_strField;

	filterCond.m_strCondition = m_strCondition;

	filterCond.m_strValue = m_strValue;
	filterCond.m_strValueTo = m_strValueTo;

	filterCond.m_bANDunion = !m_bOrUnion;

	pPreview->SetWindowText( 
		(LPCSTR)filterCond.GetFilterConditionForOrganizer( m_ptrFilterHolder, false, false ) );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::AnalizeValueToEnable( bool bUpdateData )
{
	CWnd* pValueToCombo = GetDlgItem( IDC_VALUE_TO );
	if( !pValueToCombo || !::IsWindow(pValueToCombo->GetSafeHwnd()) )
		return;

	if( bUpdateData )
		_UpdateData();

	CString strCond = m_strCondition;
	strCond.MakeLower();

	if( strCond == "from" )	
		pValueToCombo->EnableWindow( TRUE );
	else
		pValueToCombo->EnableWindow( FALSE );

	
}


/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnDropdownValue() 
{
		
	_UpdateData();

	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_VALUE);
	FillCombo( pCombo, m_strTable, m_strField );

}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::OnDropdownValueTo() 
{
		
	_UpdateData();

	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}


	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_VALUE_TO);
	FillCombo( pCombo, m_strTable, m_strField );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::FillCombo( CComboBox* pCombo, CString strTable, CString strField )
{

	if( !pCombo || !::IsWindow( pCombo->GetSafeHwnd() ))
		return;

	pCombo->ResetContent();

	
	CString strSQL;
	strSQL.Format( "SELECT Distinct( CStr(%s)) FROM %s where %s <> null",
					(LPCTSTR)strField, (LPCTSTR)strTable, (LPCTSTR)strField );

	sptrIApplication spApp( ::GetAppUnknown() );
	if( spApp == NULL )
		return;

	IUnknown* pUnkDoc = NULL;
	spApp->GetActiveDocument( &pUnkDoc );
	if( pUnkDoc == NULL )
		return;

	sptrIDBaseDocument spDBDoc( pUnkDoc );
	pUnkDoc->Release();

	if( spDBDoc == NULL )
		return;


	IDBConnectionPtr ptrDBC( spDBDoc );
	if( ptrDBC == 0 )
		return;
	

	IUnknown* pUnkConn = NULL;
	ptrDBC->GetConnection( &pUnkConn );
	if( pUnkConn == NULL)
		return;


	ADO::_ConnectionPtr ptrConn( pUnkConn );
	pUnkConn->Release();


	ADO::_RecordsetPtr spRecordset;
	try{
		_variant_t vConn = (IDispatch*)ptrConn;
		spRecordset.CreateInstance( __uuidof(ADO::Recordset) );
		spRecordset->CursorLocation = ADO::adUseClient;
		spRecordset->Open( _bstr_t(strSQL), vConn, 
					ADO::adOpenStatic, ADO::adLockReadOnly, ADO::adCmdText);

		if( VARIANT_FALSE == spRecordset->ADOEOF )
			spRecordset->MoveFirst();
			
		while( VARIANT_FALSE == spRecordset->ADOEOF )
		{

			ADO::FieldsPtr spFields = spRecordset->Fields;
			if( spFields != NULL )
			{
				ADO::FieldPtr spField = spFields->GetItem( (long)0 );
				if( spField )
				{
					_variant_t var;
					var = spField->GetValue();		
					if( !(var.vt == VT_NULL || var.vt == VT_EMPTY) )
					{
						var.ChangeType( VT_BSTR );
						CString strValue = var.bstrVal;										
						pCombo->AddString( strValue );
						/*if( bcorrect )
						{
							if( ftDigit == ::GetFieldType( spDBaseDoc, strTable, strField ) )
							int npos = strValue.Find( ',' );
							if( npos != -1 ) strValue.SetAt( npos, '.' );
						}
						*/						
					}
				}
			}
			spRecordset->MoveNext();
		}			
		
	}
	catch(...)	
	{					
	}
}


/////////////////////////////////////////////////////////////////////////////
CString CFilterConditionDlg::GetValue( UINT IDC_COMBO )
{	
	CString strResult;
	CComboBox* pCombo;
	pCombo = (CComboBox*)GetDlgItem( IDC_COMBO );
	if( pCombo )
	{
		int nIndex = pCombo->GetCurSel();
		if( nIndex >= 0 )
		{
			pCombo->GetLBText( nIndex, strResult );			
		}		
		else
		{
			GetDlgItem( IDC_COMBO )->GetWindowText( strResult );
		}
	}	

	return strResult;

}

/////////////////////////////////////////////////////////////////////////////
void CFilterConditionDlg::_UpdateData()
{
	CString strFieldTable, strValue, strValueTo, strCondition;
	//strFieldTable	= GetValue( IDC_FIELD );
	strValue		= GetValue( IDC_VALUE );
	strValueTo		= GetValue( IDC_VALUE_TO );
	strCondition	= GetValue( IDC_CONDITION );

	UpdateData();

	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}

	//m_strFieldTable	= strFieldTable; 
	m_strValue		= strValue;
	m_strValueTo	= strValueTo;
	m_strCondition	= strCondition;
}


void CFilterConditionDlg::OnOK() 
{
	_UpdateData();
	bool bvalid = true;
	/*if( !m_strValue.GetLength() )
	{
		bvalid = false;
		AfxMessageBox( IDS_EMPTY_VALUE );
		::SetFocus( ::GetDlgItem( m_hWnd, IDC_VALUE ) );
	}*/

	if( bvalid && !_tcsicmp( m_strCondition, "from" ) && !m_strValueTo.GetLength() )
	{
		bvalid = false;
		AfxMessageBox( IDS_EMPTY_VALUE );
		::SetFocus( ::GetDlgItem( m_hWnd, IDC_VALUE_TO ) );
	}


	if( bvalid )
		CDialog::OnOK();
}
