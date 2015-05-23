// DBObjectPpg.cpp : implementation file
//

#include "stdafx.h"
#include "dbcontrols.h"
#include "DBObjectPpg.h"
#include "ax_ctrl_misc.h"
#include "obj_types.h"


/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage

IMPLEMENT_DYNCREATE(CDBObjectPropertyPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBObjectPropertyPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDBObjectPropertyPage)
	ON_BN_CLICKED(IDC_AUTO_ASIGN, OnAutoAsign)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBObjectPropertyPage, "DBCONTROLS.DBObjectPropPage.1",
	0x105c5568, 0x1478, 0x4c23, 0xa8, 0xaf, 0xec, 0xd3, 0x9a, 0x32, 0xc, 0xff)


/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage::CDBObjectPropertyPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBObjectPropertyPage

BOOL CDBObjectPropertyPage::CDBObjectPropertyPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DBOBJECT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage::CDBObjectPropertyPage - Constructor

CDBObjectPropertyPage::CDBObjectPropertyPage() :
	COlePropertyPage(IDD, IDS_DBOBJECT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDBObjectPropertyPage)
	m_strViewType = _T("");
	m_bAutoViewType = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage::DoDataExchange - Moves data between page and properties

void CDBObjectPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDBObjectPropertyPage)
	DDP_CBString(pDX, IDC_VIEW_TYPE, m_strViewType, _T("ViewType") );
	DDX_CBString(pDX, IDC_VIEW_TYPE, m_strViewType);
	DDP_Check(pDX, IDC_AUTO_ASIGN, m_bAutoViewType, _T("AutoViewType") );
	DDX_Check(pDX, IDC_AUTO_ASIGN, m_bAutoViewType);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}





/////////////////////////////////////////////////////////////////////////////
// CDBObjectPropertyPage message handlers

BOOL CDBObjectPropertyPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	AnalizeAppearance();

	_bstr_t bstr_type;
	{
		ULONG Ulong;
		LPDISPATCH *lpDispatch = NULL;
		lpDispatch = GetObjectArray(&Ulong);
		if( lpDispatch )
		{
			IDBControlPtr ptr_ctrl( lpDispatch[0] );
			if( ptr_ctrl )
			{
				BSTR bstr_table = 0;
				BSTR bstr_field = 0;

				ptr_ctrl->GetTableName( &bstr_table );
				ptr_ctrl->GetFieldName( &bstr_field );

				CString str_name = bstr_table;
				str_name += ".";
				str_name += bstr_field;

				::SysFreeString( bstr_table );
				::SysFreeString( bstr_field );

				sptrIDBaseDocument spIDBDoc;
				GetDBaseDocument( spIDBDoc );
				if( spIDBDoc )
				{
					IUnknown* punk_obj = ::GetObjectByName( spIDBDoc, str_name );
					if( punk_obj )
					{
						bstr_type = ::GetObjectKind( punk_obj );
						punk_obj->Release();
					}
				}
			}
		}
	}

	CComboBox* pComo = (CComboBox*)GetDlgItem( IDC_VIEW_TYPE );
	if( pComo )
	{
		pComo->ResetContent();
		CStringArray arViewProgID;
		GetAvailableViewProgID( arViewProgID );
		for( int i=0;i<arViewProgID.GetSize();i++ )
		{
			try
			{
				BSTR bstr = arViewProgID[i].AllocSysString();

				IViewPtr ptrV( bstr );

				if( bstr )
					::SysFreeString( bstr );	bstr = 0;

				DWORD dwMatch = mvNone; 
				ptrV->GetMatchType( bstr_type, &dwMatch );
				bool bFull = false, bPartially = false;
				if( ( dwMatch & mvFull ) == mvFull )
					bFull = true;
				if( ( dwMatch & mvPartial ) == mvPartial )
					bPartially = true;
				if( !bFull && !bPartially)
					continue;

				INamedObject2Ptr	ptrNO( ptrV );
				BSTR bstrName = 0;
				ptrNO->GetUserName( &bstrName );
				if (bFull)
					pComo->InsertString(0, CString( bstrName ) );
				else
				pComo->AddString( CString( bstrName ) );

				if( bstrName )
					::SysFreeString( bstrName );	bstrName = 0;
			}
			catch(...)
			{

			}
		}
	}	

	return TRUE;
}

void CDBObjectPropertyPage::OnAutoAsign() 
{
	UpdateData( TRUE );
	AnalizeAppearance();	
}

void CDBObjectPropertyPage::AnalizeAppearance()
{
	CWnd* pwnd = GetDlgItem(IDC_VIEW_TYPE);
	if( !pwnd )
		return;			

	pwnd->EnableWindow( !m_bAutoViewType );
}
