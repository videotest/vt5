// DBCommonPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "DBControls.h"
//#include "..\dbasedoc.h"
#include "DBCommonPropPage.h"
#include "ax_ctrl_misc.h"


extern const IID BASED_CODE IID_DDBNumber;
extern const IID BASED_CODE IID_DDBText;
extern const IID BASED_CODE IID_DDBDateTime;
extern const IID BASED_CODE IID_DDBObject;


/////////////////////////////////////////////////////////////////////////////
// CDBCommonPropPage property page

IMPLEMENT_DYNCREATE(CDBCommonPropPage, COlePropertyPage)

BEGIN_MESSAGE_MAP(CDBCommonPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDBCommonPropPage)
	ON_CBN_EDITCHANGE(IDC_TABLE_NAME, OnEditchangeTableName)
	ON_CBN_SELCHANGE(IDC_TABLE_NAME, OnSelchangeTableName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBCommonPropPage, "DBCONTROLS.DBCommonPropPage.1",
0x844f36e6, 0xf378, 0x4aed, 0xb9, 0x12, 0xd2, 0xc6, 0xc, 0x61, 0xec, 0xe)

/////////////////////////////////////////////////////////////////////////////
// CDBCommonPropPage::CDBCommonPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBDateTimePropPage

BOOL CDBCommonPropPage::CDBCommonPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DBCOMMON_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

					
/////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage::CDBDateTimePropPage - Constructor

CDBCommonPropPage::CDBCommonPropPage() :
	COlePropertyPage(IDD, IDS_DBCOMMON_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDBCommonPropPage)
	m_bEnableEnumeration = FALSE;
	m_bAutoLabel = FALSE;
	m_bLabelLeft = -1;
	m_strField = _T("");
	m_strTable = _T("");
	m_bReadOnly = FALSE;
	//}}AFX_DATA_INIT

	m_controlType = ctNotSupported;
}


void CDBCommonPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDBCommonPropPage)
	DDP_Check(pDX, IDC_ENABLEENUMERATION, m_bEnableEnumeration, _T("EnableEnumeration") );
	DDX_Check(pDX, IDC_ENABLEENUMERATION, m_bEnableEnumeration);
	DDP_Check(pDX, IDC_AUTOLABEL, m_bAutoLabel, _T("AutoLabel") );
	DDX_Check(pDX, IDC_AUTOLABEL, m_bAutoLabel);
	DDP_Radio(pDX, IDC_RADIO_LEFT, m_bLabelLeft, _T("LabelPosition") );
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_bLabelLeft);
	DDP_Text(pDX, IDC_FIELD, m_strField, _T("FieldName") );
	DDX_Text(pDX, IDC_FIELD, m_strField);
	DDP_Text(pDX, IDC_TABLE, m_strTable, _T("TableName") );
	DDX_Text(pDX, IDC_TABLE, m_strTable);
	DDP_Check(pDX, IDC_READONLY, m_bReadOnly, _T("ReadOnly") );
	DDX_Check(pDX, IDC_READONLY, m_bReadOnly);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
	
	//Loading
	if( !pDX->m_bSaveAndValidate )
	{

	}

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}



/////////////////////////////////////////////////////////////////////////////
// CDBCommonPropPage message handlers

BOOL CDBCommonPropPage::OnInitDialog() 
{

	m_controlType = ctNotSupported;

	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);	
	
	if( !lpDispatch[0] )
	{
		AfxMessageBox( "Can't find Object dispatch interface" , MB_OK|MB_ICONASTERISK );
		return FALSE;	
	}

	//Determine control type
	IUnknown* punk = NULL;
	
	lpDispatch[0]->QueryInterface( IID_DDBNumber, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;		
		m_controlType = ctNumber;
	}

	lpDispatch[0]->QueryInterface( IID_DDBText, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		m_controlType = ctText;
		
	}

	lpDispatch[0]->QueryInterface( IID_DDBDateTime, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		m_controlType = ctDateTime;		
	}

	lpDispatch[0]->QueryInterface( IID_DDBObject, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		m_controlType = ctObject;		
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_ENABLEENUMERATION ), false );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_READONLY ), false );
	}

	
	
	COlePropertyPage::OnInitDialog();

	//FillTablesName( );

	return FALSE;	            
}

/////////////////////////////////////////////////////////////////////////////
void CDBCommonPropPage::FillTablesName( )
{
	/*
	m_ctrlTableName.ResetContent();


	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )	
		return;
	

	pDispatch = lpDispatch[0];

	sptrIDBControl spDBaseCtrl(pDispatch);

	if( spDBaseCtrl == NULL )
		return;
	
	IUnknown* punk = NULL;
	spDBaseCtrl->GetDBaseDocument( &punk );

	if( punk == NULL )
		return;

	sptrIDBaseStruct spIDBDoc(punk);
	punk->Release();

	if( spIDBDoc )
	{
		int nTableCount;
		if( S_OK == ( spIDBDoc->GetTableCount( &nTableCount ) ) )
		{
			for( int i=0;i<nTableCount;i++ )
			{
				BSTR bstrTableName;
				if( S_OK == ( spIDBDoc->GetTableName( i, &bstrTableName ) ) )
				{
					m_ctrlTableName.AddString( CString::CString(bstrTableName) );
					::SysFreeString( bstrTableName );
				}
			}
		}
		
		m_ctrlTableName.SelectString( 0, m_strTableName );
		SetPropText( "TableName", m_strTableName );
		//Add fields to controls
		OnTableNameChange( true );		
	}

	*/
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBCommonPropPage::OnTableNameChange( bool bAfterLoad )
{
	/*
	CString strNewTable;
	int nIndex = m_ctrlTableName.GetCurSel();
	if( nIndex >= 0 )
	{
		m_ctrlTableName.GetLBText( nIndex, strNewTable );		
	}
	else
	{
		m_ctrlTableName.GetWindowText( strNewTable );
	}

	UpdateData( TRUE );
	SetPropText( "TableName", m_strTableName );


	FillFieldsName( strNewTable, bAfterLoad );	

	UpdateData( TRUE );
	SetPropText( "FieldName", m_strFieldName );
	*/
}

/////////////////////////////////////////////////////////////////////////////
void CDBCommonPropPage::OnEditchangeTableName() 
{
	//OnTableNameChange( false );		//Not from Exchange load
}

/////////////////////////////////////////////////////////////////////////////
void CDBCommonPropPage::OnSelchangeTableName() 
{	
	//OnTableNameChange( false );		//Not from Exchange load
}

/////////////////////////////////////////////////////////////////////////////
void CDBCommonPropPage::FillFieldsName( CString strTableName, bool bAfterLoad )
{
	/*
	m_ctrlFieldName.ResetContent();


	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )	
		return;
	

	pDispatch = lpDispatch[0];

	sptrIDBControl spDBaseCtrl(pDispatch);

	if( spDBaseCtrl == NULL )
		return;
	
	IUnknown* punk = NULL;
	spDBaseCtrl->GetDBaseDocument( &punk );

	if( punk == NULL )
		return;

	sptrIDBaseStruct spIDBDoc(punk);
	punk->Release();

	if( spIDBDoc == NULL )
		return;

	_bstr_t _bstrTable( strTableName );

	int nFieldsCount = 0;
	spIDBDoc->GetFieldCount( _bstrTable, &nFieldsCount );


	short nFieldType;
	for( int i=0;i<nFieldsCount;i++ )	
	{
		BSTR bstrField = 0;
		if( S_OK != spIDBDoc->GetFieldName( _bstrTable, i, &bstrField ) )
			continue;

		_bstr_t _bstrField = bstrField;
		::SysFreeString( bstrField );	bstrField = 0;
		
		
		if( S_OK == ( spIDBDoc->GetFieldInfo( _bstrTable, _bstrField, 
			0, &nFieldType, 
			NULL, NULL,
			NULL, NULL,
			NULL, NULL
			)) )			
		{
			bool bNeedAdd = false;
			if( m_controlType == ctNumber && (FieldType)nFieldType == ftDigit )
				bNeedAdd = true;

			if( m_controlType == ctText  && (FieldType)nFieldType == ftString )
				bNeedAdd = true;

			if( m_controlType == ctDateTime && (FieldType)nFieldType == ftDateTime )
				bNeedAdd = true;

			if( m_controlType == ctObject && (FieldType)nFieldType == ftVTObject )
				bNeedAdd = true;

			if( bNeedAdd )
				m_ctrlFieldName.AddString( (LPCSTR)_bstrField );

			
		}
	}

	if( m_strFieldName.IsEmpty() )
		m_ctrlFieldName.SetCurSel( 0 );
	else
		m_ctrlFieldName.SelectString( 0, m_strFieldName );	
	*/
}
