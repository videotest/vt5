// PlaceTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "constant.h"
#include "dbasedoc.h"
#include "PlaceTargetDlg.h"
#include "Avi_int.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlaceTargetDlg dialog


CPlaceTargetDlg::CPlaceTargetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaceTargetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlaceTargetDlg)
	m_nFieldIndex = -1;
	m_bNewRecord = FALSE;
	m_nTarget = 0;
	//}}AFX_DATA_INIT
}

CPlaceTargetDlg::~CPlaceTargetDlg()
{
	for( int i=0;i<m_arrfi.GetSize();i++ )
		delete m_arrfi[i];

	m_arrfi.RemoveAll();
}

void CPlaceTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlaceTargetDlg)
	DDX_LBIndex(pDX, IDC_FIELDS, m_nFieldIndex);
	DDX_Check(pDX, IDC_NEW_RECORD, m_bNewRecord);
	DDX_Radio(pDX, IDC_TARGET, m_nTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlaceTargetDlg, CDialog)
	//{{AFX_MSG_MAP(CPlaceTargetDlg)
	ON_BN_CLICKED(IDC_TARGET, OnTarget)
	ON_BN_CLICKED(IDC_RADIO2, OnPutToDoc)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaceTargetDlg message handlers

BOOL CPlaceTargetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetControls();

	CListBox* plist = (CListBox*)GetDlgItem( IDC_FIELDS );
	if( !plist || m_ptrQuery == 0 || m_ptrDBStruct == 0 )
		return TRUE;

	// A.M. SBT 839 - protection against insertion AVI files into database
	bool bAvi = m_strVTObjectType == _T("Image") ? ::CheckInterface(m_punkObject, IID_IAviObject) : false;

	bool bonly_same_type = ( 1L == ::GetValueInt( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "NotifyPutToDataSameType", 1L ) );

	int nActiveField = -1;	

	m_ptrQuery->GetActiveField( &nActiveField );

	int nFieldsCount = -1;
	IS_OK( m_ptrQuery->GetFieldsCount( &nFieldsCount ) );
	for( int i=0;i<nFieldsCount;i++ )
	{
		BSTR bstrTableName, bstrFieldName;
		bstrTableName = bstrFieldName = 0;
		if( S_OK == ( m_ptrQuery->GetField( i, 0, &bstrTableName, &bstrFieldName ) ) )
		{					
			CString strTable = bstrTableName;
			CString strField = bstrFieldName;			

			if( bstrTableName )
				::SysFreeString( bstrTableName );


			if( bstrFieldName  )
				::SysFreeString( bstrFieldName );

			FieldType fieldType = ::GetFieldType( m_ptrDBStruct, strTable, strField );
			if( !( fieldType == ftVTObject ) )
				continue;

			BSTR bstrCaption = 0;
			BSTR bstrType = 0;
			short nType = 0;
			if( S_OK != m_ptrDBStruct->GetFieldInfo( _bstr_t((LPCSTR)strTable), 
											_bstr_t((LPCSTR)strField), 
											&bstrCaption, &nType, 0, 0, 0, 0, 0, &bstrType ) )
				continue;


			CString str_caption = bstrCaption;
			if( bstrCaption )
				::SysFreeString( bstrCaption );	bstrCaption = 0;

			CString str_type = bstrType;
			if( bstrType )
				::SysFreeString( bstrType );	bstrType = 0;            
			
			if( bonly_same_type && ( str_type != m_strVTObjectType || bAvi) )
				continue;
			

			_CField* pfi = new _CField;
			m_arrfi.Add( pfi );
			pfi->m_strTable = strTable;
			pfi->m_strField = strField;     
			pfi->m_strCaption = str_caption;


			plist->AddString( pfi->m_strCaption );

			if( nActiveField == i )
				plist->SetCurSel( plist->GetCount() - 1 );

		}
	}	

	BOOL bEnableCreateInDoc = ::GetValueInt(::GetAppUnknown(), "\\Database\\NotifyPutToData",
		"EnableCreateInDoc", TRUE);

	if( !bEnableCreateInDoc)
	{
		::CheckDlgButton( GetSafeHwnd(), IDC_RADIO1, 1 );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO2), false);
	if( !plist->GetCount() )
		{
			plist->EnableWindow( false );
			::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_RECORD ), false );
			::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_TARGET ), false );		
			::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDOK), false);
		}
		else
			plist->SetCurSel(0);
	}
	else if( !plist->GetCount() )
	{
		plist->EnableWindow( false );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_RECORD ), false );
		::CheckDlgButton( GetSafeHwnd(), IDC_RADIO2, 1 );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_TARGET ), false );		
	}

	
	return TRUE;	            
}

/////////////////////////////////////////////////////////////////////////////
void CPlaceTargetDlg::SetControls()
{
	UpdateData();

	bool bEnable = ( m_nTarget == 0 );

	if( GetDlgItem(IDC_FIELDS) )
		GetDlgItem(IDC_FIELDS)->EnableWindow( bEnable );

	if( GetDlgItem(IDC_NEW_RECORD) )
		GetDlgItem(IDC_NEW_RECORD)->EnableWindow( bEnable );	

}

/////////////////////////////////////////////////////////////////////////////
void CPlaceTargetDlg::OnTarget() 
{
	SetControls();	
}

/////////////////////////////////////////////////////////////////////////////
void CPlaceTargetDlg::OnPutToDoc() 
{
	SetControls();
}

/////////////////////////////////////////////////////////////////////////////
void CPlaceTargetDlg::OnOK() 
{
	UpdateData( true );
	if( m_nFieldIndex >= 0 && m_nFieldIndex < m_arrfi.GetSize() )
	{
		m_strTable = m_arrfi[m_nFieldIndex]->m_strTable;
		m_strField = m_arrfi[m_nFieldIndex]->m_strField;
	}
	
	CDialog::OnOK();
}
