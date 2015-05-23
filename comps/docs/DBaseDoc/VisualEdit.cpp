// VisualEdit.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "VisualEdit.h"

#include "DBStructDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CAddField dialog


CAddField::CAddField(CWnd* pParent /*=NULL*/)
	: CDialog(CAddField::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddField)
	m_strDefValue = _T("");
	m_strFieldName = _T("");
	m_strFieldType = _T("");
	m_bDefValue = FALSE;
	m_bReqValue = FALSE;
	//}}AFX_DATA_INIT


	m_ft = ftNotSupported;
}

CAddField::CAddField(IDBaseStruct* pDBStruct, BSTR bstrTable, CWnd* pParent)
	: CDialog(CAddField::IDD, pParent)
	, m_pDBStruct(pDBStruct)
	, m_bstrTable(bstrTable)
{
	//{{AFX_DATA_INIT(CAddField)
	m_strDefValue = _T("");
	m_strFieldName = _T("");
	m_strFieldType = _T("");
	m_bDefValue = FALSE;
	m_bReqValue = FALSE;
	//}}AFX_DATA_INIT
}


void CAddField::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddField)
	DDX_Text(pDX, IDC_DEF_VALUE, m_strDefValue);
	DDX_CBString(pDX, IDC_FIELD_TYPE, m_strFieldType);
	DDX_Check(pDX, IDC_NEED_DEF, m_bDefValue);
	DDX_Check(pDX, IDC_NEED_REQ, m_bReqValue);
	DDX_CBString(pDX, IDC_FIELD_NAME, m_strFieldName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddField, CDialog)
	//{{AFX_MSG_MAP(CAddField)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NEED_DEF, OnBnClickedNeedDef)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddField message handlers
BOOL CAddField::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_FIELD_TYPE);
	if( !pCombo )
		return TRUE;


	_CFieldInfo fi;
	fi.m_FieldType = m_ft;

	CString str = CDBStructGrid::GetFieldTypeAsString( &fi );


	if( !str.IsEmpty() )
	{
		pCombo->AddString( str );
		pCombo->SetCurSel( 0 );
		pCombo->EnableWindow( false );
	}
	else if( m_ft == ftVTObject )
	{
		CStringArray arrTypes, arrUserName;
		CDBStructGrid::GetAvailableTypes( arrTypes, arrUserName, FIELD_TYPE_VT | FIELD_TYPE_FILTER_VT );

		CString strImageUserName;
		for( int i=0;i<arrTypes.GetSize();i++ )
		{
			if( arrTypes[i] = szTypeImage )
			{
				strImageUserName = arrUserName[i];
				break;
			}
		}

		for( i=0;i<arrTypes.GetSize();i++ )
			pCombo->AddString( arrUserName[i] );
		
		pCombo->SelectString( 0, strImageUserName );

		if( GetDlgItem(IDC_NEED_DEF) )
			GetDlgItem(IDC_NEED_DEF)->EnableWindow( false );

		if( GetDlgItem(IDC_DEF_VALUE) )
			GetDlgItem(IDC_DEF_VALUE)->EnableWindow( false );

		if( GetDlgItem(IDC_NEED_REQ) )
			GetDlgItem(IDC_NEED_REQ)->EnableWindow( false );
	}
	if (!m_bDefValue)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEF_VALUE), FALSE);


	CComboBox* pcbFieldName = (CComboBox*)GetDlgItem(IDC_FIELD_NAME);
	if( !pcbFieldName )
		return TRUE;

	pcbFieldName->AddString(m_strFieldName);
	if(m_pDBStruct){
		int cntFields=0;
		m_pDBStruct->GetFieldCount(m_bstrTable,&cntFields);
		for (int i=0; i<cntFields; i++)
		{
			CComBSTR fieldName;
			m_pDBStruct->GetFieldName(m_bstrTable,i,&fieldName);
			short fieldType=0;
			HRESULT hr=m_pDBStruct->GetFieldInfo(m_bstrTable,fieldName,0,&fieldType,
				0,0,0,0,0,0);
			if(SUCCEEDED(hr)){
				if(ftVTObject==fieldType)
				{
					pcbFieldName->AddString(ATL::CW2CT(fieldName));
				}
			}
		}
	}
	
	return TRUE;  	              
}

void CAddField::OnBnClickedNeedDef()
{
	UpdateData();
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEF_VALUE), m_bDefValue);
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteField dialog


CDeleteField::CDeleteField(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteField::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteField)
	m_strField = _T("");
	m_strTable = _T("");
	//}}AFX_DATA_INIT
}


void CDeleteField::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteField)
	DDX_Text(pDX, IDC_FIELD_NAME, m_strField);
	DDX_Text(pDX, IDC_TABLE_NAME, m_strTable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteField, CDialog)
	//{{AFX_MSG_MAP(CDeleteField)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteField message handlers

