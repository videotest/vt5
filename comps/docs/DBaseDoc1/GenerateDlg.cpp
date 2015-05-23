// GenerateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "GenerateDlg.h"
#include "Generate.h"
#include "constant.h"

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg dialog


CGenerateDlg::CGenerateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenerateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenerateDlg)	
	m_strUIName = _T("");
	m_strEvent = _T("");
	//}}AFX_DATA_INIT
}


void CGenerateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenerateDlg)	
	DDX_CBString(pDX, IDC_UINAME, m_strUIName);
	DDX_CBString(pDX, IDC_EVENT, m_strEvent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenerateDlg, CDialog)
	//{{AFX_MSG_MAP(CGenerateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg message handlers
void CGenerateDlg::SetDBaseDocument( IUnknown* pUnkDBDoc )
{
	m_spDBaseDoc = pUnkDBDoc;
}

BOOL CGenerateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_spDBaseDoc == NULL )
		return TRUE;

	CComboBox* pEventCombo	= (CComboBox*)GetDlgItem( IDC_EVENT );
	CComboBox* pUINameCombo	= (CComboBox*)GetDlgItem( IDC_UINAME );

	if( !pEventCombo || !::IsWindow( pEventCombo->GetSafeHwnd() ) )
		return TRUE;

	if( !pUINameCombo || !::IsWindow( pUINameCombo->GetSafeHwnd() ) )
		return TRUE;

	pEventCombo->ResetContent();
	pEventCombo->AddString( szEventOnAddTable );
	pEventCombo->AddString( szEventOnEditTable );
	pEventCombo->AddString( szEventOnDropTable );

	pEventCombo->AddString( szEventGenerateAXFormByQuery );
	pEventCombo->AddString( szEventGenerateReportByQuery );
	pEventCombo->AddString( szEventGenerateReportByAXForm );

	pEventCombo->AddString( szEventAnalizeAXForm );
	pEventCombo->AddString( szEventAnalizeReport );
	pEventCombo->AddString( szEventAnalizeQuery );

	pEventCombo->AddString( szEventAnalizeAllAXForms );
	pEventCombo->AddString( szEventAnalizeAllReports );
	pEventCombo->AddString( szEventAnalizeAllQueries );


	//Add all tables
	pUINameCombo->ResetContent();
	int nTableCount = 0;
	//m_spDBaseDoc->GetTableCount( &nTableCount );
	for( int i=0;i<nTableCount;i++ )
	{
		BSTR bstrTableName = 0;
		//if( S_OK == m_spDBaseDoc->GetTableName( i, &bstrTableName ) )
		{
			CString strTableName = bstrTableName;
			::SysFreeString( bstrTableName );
			pUINameCombo->AddString( strTableName );
		}
	}
	
	
	//Add all query
	sptrIDataContext2 spDC( m_spDBaseDoc );
	if( spDC == NULL )
		return TRUE;

	long nQueryCount = 0;
	_bstr_t bstrQueryType( szTypeQueryObject );
	
	spDC->GetObjectCount( bstrQueryType, &nQueryCount );
	for( i=0;i<nQueryCount;i++ )
	{
		IUnknown* pUnkQuery = NULL;

		pUnkQuery = ::GetObjectFromContextByPos( spDC, bstrQueryType, i );
		
		//spDC->GetObject( bstrQueryType, i, NULL, &pUnkQuery );
		if( pUnkQuery != NULL )
		{
			CString strQueryName = ::GetObjectName( pUnkQuery );
			pUnkQuery->Release();

			pUINameCombo->AddString( strQueryName );
		}
	}	

	//Add all AXForm

	long nAXFormCount = 0;
	_bstr_t bstrAXFormType( szTypeBlankForm );
	
	spDC->GetObjectCount( bstrAXFormType, &nAXFormCount );
	for( i=0;i<nAXFormCount;i++ )
	{
		IUnknown* pUnkAXForm = NULL;

		pUnkAXForm = ::GetObjectFromContextByPos( spDC, bstrAXFormType, i );

		//spDC->GetObject( bstrAXFormType, i, NULL, &pUnkAXForm );
		if( pUnkAXForm != NULL )
		{
			CString strAXFomrName = ::GetObjectName( pUnkAXForm );
			pUnkAXForm->Release();

			pUINameCombo->AddString( strAXFomrName );
		}
	}	

	if( m_strEvent.IsEmpty() )
		pEventCombo->SetCurSel( 0 );


	return TRUE;
}

