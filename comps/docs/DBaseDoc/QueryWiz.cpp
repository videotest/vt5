// QueryWiz.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "QueryWiz.h"
#include "types.h"

#include "Wizards.h"


/////////////////////////////////////////////////////////////////////////////
// CEditQueryPage property page

IMPLEMENT_DYNCREATE(CEditQueryPage, CPropertyPage)

CEditQueryPage::CEditQueryPage() : CPropertyPage(CEditQueryPage::IDD)
{
	m_pArrTableIfo = 0;
	//{{AFX_DATA_INIT(CEditQueryPage)
	m_nSimpleQuery = 0;
	m_strSQL = _T("");
	m_strMainTable = _T("");
	//}}AFX_DATA_INIT

	m_bExternalFillTableInfo = false;
}

CEditQueryPage::~CEditQueryPage()
{
	for( int i=0;i<m_arrTableInfo.GetSize();i++ )
		delete m_arrTableInfo[i];

	m_arrTableInfo.RemoveAll();
}

void CEditQueryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditQueryPage)
	DDX_Control(pDX, IDC_MAIN_TABLE, m_ctrlMainTable);
	DDX_Radio(pDX, IDC_SIMPLE_QUERY, m_nSimpleQuery);
	DDX_Text(pDX, IDC_SQL, m_strSQL);
	DDX_CBString(pDX, IDC_MAIN_TABLE, m_strMainTable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditQueryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CEditQueryPage)
	ON_BN_CLICKED(IDC_SIMPLE_QUERY, OnSimpleQuery)
	ON_BN_CLICKED(IDC_RADIO2, OnAdvancedQuery)
	ON_BN_CLICKED(IDC_INSERT_FIELD, OnInsertField)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CEditQueryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	

	FillTableInfo();

	if( !m_pArrTableIfo )
		return TRUE;

	for( int i=0;i<m_pArrTableIfo->GetSize();i++ )
	{	
		if( m_strMainTable.IsEmpty() ) 
			m_strMainTable = (*m_pArrTableIfo)[i]->m_strTableName;

		m_ctrlMainTable.AddString( (*m_pArrTableIfo)[i]->m_strTableName );
	}	

	//m_ctrlMainTable.FindString( 0, m_strMainTable );
	
	UpdateData( FALSE );	
	
	SetEnable2Controls();
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
void CEditQueryPage::OnSimpleQuery() 
{
	UpdateData( TRUE );
	SetEnable2Controls();	
}

/////////////////////////////////////////////////////////////////////////////
void CEditQueryPage::OnAdvancedQuery() 
{
	UpdateData( TRUE );
	SetEnable2Controls();
}

/////////////////////////////////////////////////////////////////////////////
void CEditQueryPage::SetEnable2Controls()
{
	CWnd* p1 = GetDlgItem( IDC_MAIN_TABLE );
	CWnd* p2 = GetDlgItem( IDC_SQL );
	CWnd* p3 = GetDlgItem( IDC_INSERT_FIELD );
	CWnd* p4 = GetDlgItem( IDC_USE_TEMPLATE );

	if( p1 ) p1->EnableWindow( !m_nSimpleQuery );
	if( p2 ) p2->EnableWindow( m_nSimpleQuery );
	if( p3 ) p3->EnableWindow( m_nSimpleQuery );
	if( p4 ) p4->EnableWindow( m_nSimpleQuery );		
}

/////////////////////////////////////////////////////////////////////////////
void CEditQueryPage::OnInsertField() 
{
	CFieldChooserDlg dlg;
	dlg.SetTableInfo( m_pArrTableIfo );
	if( dlg.DoModal() == IDOK )
	{		
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SQL);
		if( !pEdit )
			return;

		if( !dlg.m_strFields.IsEmpty() && !dlg.m_strTable.IsEmpty() )
		{
			CString str_sql;
			str_sql.Format( "select %s from %s", dlg.m_strFields, dlg.m_strTable );
			pEdit->SetWindowText( str_sql );
			//pEdit->ReplaceSel( dlg.m_strFields );
		}
	}	
}


/////////////////////////////////////////////////////////////////////////////
bool CEditQueryPage::FillTableInfo()
{
	if( m_bExternalFillTableInfo )
		return false;

	for( int i=0;i<m_arrTableInfo.GetSize();i++ )
		delete m_arrTableInfo[i];

	m_arrTableInfo.RemoveAll();

	m_pArrTableIfo = &m_arrTableInfo;


	CWnd* pParent = GetParent();
	CPropertyPage::OnSetActive();
	if( !( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)) ) )
		return false;
	
	CWizardSheet* pSheet = (CWizardSheet*)pParent;

	_CTableInfo* ptiNew = new _CTableInfo( "New" );
	bool bRes = ::GetDBFieldInfo( pSheet, m_arrTableInfo, ptiNew  );

	if( ptiNew->m_arrFieldInfo.GetSize() > 0 )
		m_arrTableInfo.Add( ptiNew );
	else
		delete ptiNew;


	m_ctrlMainTable.ResetContent();

	bool bFoundMainTable = false;
	for( i=0;i<m_pArrTableIfo->GetSize();i++ )
	{	
		m_ctrlMainTable.AddString( (*m_pArrTableIfo)[i]->m_strTableName );
		
		if( m_strMainTable == (*m_pArrTableIfo)[i]->m_strTableName )
			bFoundMainTable = true;

	}	


	if( !bFoundMainTable && m_pArrTableIfo->GetSize() > 0 )
	{
		m_strMainTable = (*m_pArrTableIfo)[0]->m_strTableName;
		m_ctrlMainTable.SetCurSel( 0 );
	}


	UpdateData( FALSE );



	return bRes;
}


/////////////////////////////////////////////////////////////////////////////
// CTumbnailPage

IMPLEMENT_DYNCREATE(CTumbnailPage, CPropertyPage)

CTumbnailPage::CTumbnailPage() : CPropertyPage(CTumbnailPage::IDD)	
{
	m_pArrTableIfo = 0;
	m_bSimpleQuery = false;

	//{{AFX_DATA_INIT(CTumbnailPage)
	m_nGenerateHeight = 100;
	m_nGenerateWidth = 100;
	m_nViewHeight = 100;
	m_nViewWidth = 100;
	m_nBorderSize = 5;
	m_nCaption = -1;
	m_nField = -1;
	//}}AFX_DATA_INIT
}

CTumbnailPage::~CTumbnailPage()
{
	DestroyFieldArray();
}

void CTumbnailPage::DestroyFieldArray()
{
	for( int i=0;i<m_arrfiCaption.GetSize();i++ )
		delete m_arrfiCaption[i];

	m_arrfiCaption.RemoveAll();

	for( i=0;i<m_arrfiThumbnail.GetSize();i++ )
		delete m_arrfiThumbnail[i];

	m_arrfiThumbnail.RemoveAll();
}


void CTumbnailPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTumbnailPage)
	DDX_Text(pDX, IDC_GENERATE_HEIGHT, m_nGenerateHeight);
	DDX_Text(pDX, IDC_GENERATE_WIDTH, m_nGenerateWidth);
	DDX_Text(pDX, IDC_VIEW_HEIGHT, m_nViewHeight);
	DDX_Text(pDX, IDC_VIEW_WIDTH, m_nViewWidth);
	DDX_Text(pDX, IDC_BORDER_SIZE, m_nBorderSize);
	DDX_CBIndex(pDX, IDC_TUMBNAIL_CAPTION, m_nCaption);
	DDX_CBIndex(pDX, IDC_TUMBNAIL_FIELD, m_nField);

	DDV_MinMaxInt(pDX, m_nViewWidth, 20, 200);
	DDV_MinMaxInt(pDX, m_nViewHeight, 20, 200);
	DDV_MinMaxInt(pDX, m_nBorderSize, 0, 200);	
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTumbnailPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTumbnailPage)
	ON_CBN_SELCHANGE(IDC_TUMBNAIL_FIELD, OnSelchangeTumbnailField)
	ON_CBN_SELCHANGE(IDC_TUMBNAIL_CAPTION, OnSelchangeTumbnailCaption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTumbnailDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CTumbnailPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();		

	if( !m_pArrTableIfo)
		return TRUE;


	FillControlsFromData( m_pArrTableIfo );


	return TRUE;	            
}



/////////////////////////////////////////////////////////////////////////////
// CFieldChooserDlg dialog


CFieldChooserDlg::CFieldChooserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFieldChooserDlg::IDD, pParent)
{
	m_pArrTableIfo = 0;
	//{{AFX_DATA_INIT(CFieldChooserDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFieldChooserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldChooserDlg)
	DDX_Control(pDX, IDC_FIELDS, m_ctrlFields);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldChooserDlg, CDialog)
	//{{AFX_MSG_MAP(CFieldChooserDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_FIELDS, OnNMClickFields)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldChooserDlg message handlers

BOOL CFieldChooserDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlFields.DeleteAllItems();

	if( m_pArrTableIfo == 0 )
		return TRUE;
	


	for( int i=0;i<m_pArrTableIfo->GetSize();i++ )
	{
		_CTableInfo* pti = (*m_pArrTableIfo)[i];

		HTREEITEM htiTable = m_ctrlFields.InsertItem( pti->m_strTableName );
		for( int j=0;j<pti->m_arrFieldInfo.GetSize();j++ )
		{
			_CFieldInfo* pfi = pti->m_arrFieldInfo[j];
			m_ctrlFields.InsertItem( pfi->m_strFieldName, htiTable );
		}
		m_ctrlFields.Expand( htiTable, TVE_EXPAND );
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CFieldChooserDlg::OnOK() 
{
	m_strFields = "";
	m_strTable = "";

	bool bFirstEntry = true;

	HTREEITEM htiTable = m_ctrlFields.GetNextItem( 0, TVGN_ROOT );
	while( htiTable )
	{
		CString strTable = m_ctrlFields.GetItemText( htiTable );

		HTREEITEM htiField = m_ctrlFields.GetNextItem( htiTable, TVGN_CHILD );
		
		while( htiField )
		{
			if( m_ctrlFields.GetCheck( htiField ) )
			{
				CString strFormat;
				CString strField = m_ctrlFields.GetItemText( htiField );
				strFormat = strField;//.Format( "%s.%s", strTable, strField );
				m_strTable = strTable;

				if( !bFirstEntry )
					m_strFields += ", ";
				else
					bFirstEntry = false;

				m_strFields += strFormat;
			}
			htiField = m_ctrlFields.GetNextItem( htiField, TVGN_NEXT );
		}
		htiTable = m_ctrlFields.GetNextItem( htiTable, TVGN_NEXT );
	}				

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CQueryNamePage property page

IMPLEMENT_DYNCREATE(CQueryNamePage, CPropertyPage)

CQueryNamePage::CQueryNamePage() : CPropertyPage(CQueryNamePage::IDD)
{
	//{{AFX_DATA_INIT(CQueryNamePage)
	m_strQueryName = _T("");
	//}}AFX_DATA_INIT

	m_strQueryName.LoadString( IDS_NEW_QUERY_PREFIX );
}

CQueryNamePage::~CQueryNamePage()
{
}

void CQueryNamePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryNamePage)
	DDX_Text(pDX, IDC_QUERY_NAME, m_strQueryName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryNamePage, CPropertyPage)
	//{{AFX_MSG_MAP(CQueryNamePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryNamePage message handlers

BOOL CEditQueryPage::_OnSetActive() 
{	
	PROCESS_SET_ACTIVE_MSG()
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEditQueryPage::OnSetActive() 
{
	if( _OnSetActive()  )
	{
		CWnd* pParent = GetParent();
		CPropertyPage::OnSetActive();
		if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))
		{
			CWizardSheet* pSheet = (CWizardSheet*)pParent;
			//if( pSheet->GetActivePage() == this )
				FillTableInfo();
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEditQueryPage::OnWizardBack() 
{
	PROCESS_WIZARD_BACK_MSG()
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEditQueryPage::OnWizardNext() 
{
	UpdateData( TRUE );
	if( m_nSimpleQuery == 0 )
	{
		if( m_strMainTable.IsEmpty() )
		{
			AfxMessageBox( IDS_WARNING_TABLE_EMPTY, MB_ICONSTOP );
			GetDlgItem( IDC_MAIN_TABLE )->SetFocus();
			return -1;
		}		
	}
	else
	{
		if( m_strSQL.IsEmpty() )
		{
			AfxMessageBox( IDS_WARNING_SQL_EMPTY, MB_ICONSTOP );
			GetDlgItem( IDC_SQL )->SetFocus();
			return -1;
		}		
	}

	PROCESS_WIZARD_NEXT_MSG()
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEditQueryPage::OnWizardFinish() 
{
	PROCESS_WIZARD_FINISH_MSG()
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQueryNamePage::OnSetActive() 
{	
	PROCESS_SET_ACTIVE_MSG()
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CQueryNamePage::OnWizardBack() 
{
	PROCESS_WIZARD_BACK_MSG()
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CQueryNamePage::OnWizardNext() 
{
	UpdateData( TRUE );
	if( m_strQueryName.IsEmpty() )
	{
		GetDlgItem(IDC_QUERY_NAME)->SetFocus();
		AfxMessageBox( IDS_WARNING_QUERY_NAME_EMPTY, MB_ICONSTOP );
		return -1;
	}

	PROCESS_WIZARD_NEXT_MSG()
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQueryNamePage::OnWizardFinish() 
{
	PROCESS_WIZARD_FINISH_MSG()
}

/////////////////////////////////////////////////////////////////////////////
void CTumbnailPage::FillControlsFromData( TABLEINFO_ARR_PTR pArrTable )
{	
	DestroyFieldArray();

	CComboBox* pComboCaption  = (CComboBox*) GetDlgItem(IDC_TUMBNAIL_CAPTION);
	CComboBox* pComboTumbnail = (CComboBox*) GetDlgItem(IDC_TUMBNAIL_FIELD);	

	if( pComboCaption == NULL || !::IsWindow( pComboCaption->GetSafeHwnd() ))
		return;

	if( pComboTumbnail == NULL || !::IsWindow( pComboTumbnail->GetSafeHwnd() ))
		return;

	pComboCaption->ResetContent();
	pComboTumbnail->ResetContent();


	{
		CWnd* pParent = GetParent();		
		if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))
		{
			CWizardSheet* pSheet = (CWizardSheet*)pParent;

			CEditQueryPage* pPage = (CEditQueryPage*)pSheet->FindPage(RUNTIME_CLASS(CEditQueryPage), 0);
			if( pPage )
			{
				m_bSimpleQuery	= pPage->m_nSimpleQuery == 0;
				m_strMainTable	= pPage->m_strMainTable;
				m_strSQL		= pPage->m_strSQL;
			}
		}
	}

	CStringArray ar_simple_fields;

	if( !m_bSimpleQuery )
	{
		char sz_buf[512];	sz_buf[0] = 0;
		CString str_sql = m_strSQL;
		str_sql.MakeLower();
		char* psz = (char*)strstr( str_sql, "select " );
		if( psz )
		{
			psz += strlen( "select" );
			if( strlen(psz) < sizeof(sz_buf) )
			{
				strcpy( sz_buf, psz );
				psz = strstr( sz_buf, " from " );
				if( psz )
				{
					*psz = 0;
					psz = strtok( sz_buf, "," );
					while( psz )
					{
						CString str_lower = psz;
						str_lower.Trim();
						str_lower.MakeLower();
						ar_simple_fields.Add( str_lower );
						psz = strtok( 0, "," );
					}
				}
			}
		}
	}


	for( int i=0;i<pArrTable->GetSize();i++ )
	{
		_CTableInfo* pti = (*pArrTable)[i];

		if( m_bSimpleQuery && m_strMainTable != pti->m_strTableName )
			continue;

		for( int j=0;j<pti->m_arrFieldInfo.GetSize();j++ )
		{
			_CFieldInfo* pfi = pti->m_arrFieldInfo[j];

			if( !m_bSimpleQuery )
			{
				bool bfound = false;
				for(int idx=0;idx<ar_simple_fields.GetSize(); idx++ )
				{
					if( !stricmp( pfi->m_strFieldName, ar_simple_fields.GetAt(idx) ) )
					{
						bfound = true;
						break;
					}
				}
				if( !bfound )
					continue;
			}

			if( pfi->m_FieldType == ftString )
			{
				_CField* pf = new _CField;
				
				pf->m_strTable		= pti->m_strTableName;
				pf->m_strField		= pfi->m_strFieldName;
				pf->m_strCaption	= ( !pfi->m_strUserName.IsEmpty() ? pfi->m_strUserName : pfi->m_strFieldName );

				m_arrfiCaption.Add( pf );

				pComboCaption->AddString( pf->m_strCaption );
				
			}
			else if( pfi->m_FieldType == ftVTObject && pfi->m_strVTObjectType == _T("Image"))
			{
				_CField* pf = new _CField;
				
				pf->m_strTable		= pti->m_strTableName;
				pf->m_strField		= pfi->m_strFieldName;
				pf->m_strCaption	= ( !pfi->m_strUserName.IsEmpty() ? pfi->m_strUserName : pfi->m_strFieldName );

				m_arrfiThumbnail.Add( pf );				

				pComboTumbnail->AddString( pf->m_strCaption );	
			}
		}
	}

	for( i=0;i<m_arrfiCaption.GetSize();i++ )
	{
		if( m_strCaptionTable == m_arrfiCaption[i]->m_strTable 
			&& m_strCaptionField == m_arrfiCaption[i]->m_strField )
		{
			m_nCaption = i;
			break;
		}
	}

	if( m_strCaptionTable.IsEmpty() && m_strCaptionField.IsEmpty() && m_nCaption < 0 )
		m_nCaption = 0;


	for( i=0;i<m_arrfiThumbnail.GetSize();i++ )
	{
		if( m_strThumbnailTable == m_arrfiThumbnail[i]->m_strTable 
			&& m_strThumbnailField == m_arrfiThumbnail[i]->m_strField )
		{
			m_nField = i;
			break;
		}
	}

	if( m_strThumbnailTable.IsEmpty() && m_strThumbnailField.IsEmpty() && m_nField < 0 )
		m_nField = 0;


	UpdateData( false );


	GetTableFieldsFromDlg( );
	
}

/////////////////////////////////////////////////////////////////////////////
void CTumbnailPage::GetTableFieldsFromDlg( )
{
	UpdateData();

	if( m_nCaption >= 0 && m_nCaption < m_arrfiCaption.GetSize() )
	{
		m_strCaptionTable = m_arrfiCaption[m_nCaption]->m_strTable;
		m_strCaptionField = m_arrfiCaption[m_nCaption]->m_strField;
	}

	if( m_nField >= 0 && m_nField < m_arrfiThumbnail.GetSize() )
	{
		m_strThumbnailTable = m_arrfiThumbnail[m_nField]->m_strTable;
		m_strThumbnailField = m_arrfiThumbnail[m_nField]->m_strField;
	}

}


/////////////////////////////////////////////////////////////////////////////
BOOL CTumbnailPage::OnSetActive() 
{

	// [vanek] BT2000: 3505
	/*
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN2 ))->SetRange( 1, 300 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN3 ))->SetRange( 1, 300 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 ))->SetRange( 1, 300 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 ))->SetRange( 1, 300 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN6 ))->SetRange( 1, 300 );	
	*/

	// теперь инициализируем те кнопки-счетичики, которые имеются на форме( IDC_SPIN1 - IDC_SPIN5 )
	CSpinButtonCtrl* pspin_btn = 0;
	pspin_btn = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN1 );
	if( pspin_btn )
		pspin_btn->SetRange( 1, 300 );

	pspin_btn = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN2 );
	if( pspin_btn )
		pspin_btn->SetRange( 1, 300 );

	pspin_btn = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN3 );
	if( pspin_btn )
		pspin_btn->SetRange( 1, 300 );

	pspin_btn = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN4 );
	if( pspin_btn )
		pspin_btn->SetRange( 1, 300 );

	pspin_btn = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN5 );
	if( pspin_btn )
		pspin_btn->SetRange( 1, 300 );


	CWnd* pWiz = GetParent();
	if( pWiz && pWiz->IsKindOf( RUNTIME_CLASS(CWizardSheet)) )
	{

		TABLEINFO_ARR arrTable;
		_CTableInfo* ptiNew = new _CTableInfo( "New" );
		bool bRes = ::GetDBFieldInfo( (CWizardSheet*)pWiz, arrTable, ptiNew  );

		if( ptiNew->m_arrFieldInfo.GetSize() > 0 )
			arrTable.Add( ptiNew );
		else
			delete ptiNew;

		if( bRes )
			FillControlsFromData( &arrTable );
		else
			FillControlsFromData( m_pArrTableIfo );

		for( int i=0;i<arrTable.GetSize();i++ )
			delete arrTable[i];

		arrTable.RemoveAll();

	}

	PROCESS_SET_ACTIVE_MSG()
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTumbnailPage::OnWizardBack() 
{	
	PROCESS_WIZARD_BACK_MSG()
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTumbnailPage::OnWizardNext() 
{	
	PROCESS_WIZARD_NEXT_MSG()
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTumbnailPage::OnWizardFinish() 
{	
	PROCESS_WIZARD_FINISH_MSG()
}

/////////////////////////////////////////////////////////////////////////////
void CTumbnailPage::OnSelchangeTumbnailField() 
{
	GetTableFieldsFromDlg( );
}

/////////////////////////////////////////////////////////////////////////////
void CTumbnailPage::OnSelchangeTumbnailCaption() 
{
	GetTableFieldsFromDlg( );
}

// [MAX] Bt: 3478
void CFieldChooserDlg::OnNMClickFields(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT pt = {0};

	::GetCursorPos( &pt );

	m_ctrlFields.ScreenToClient( &pt );

	HTREEITEM hItem = m_ctrlFields.HitTest( pt );

	if( hItem )
	{
		if( m_ctrlFields.ItemHasChildren( hItem ) )
		{
			BOOL bCkeck = m_ctrlFields.GetCheck( hItem );

			if( m_ctrlFields.GetItemState( hItem, TVIS_EXPANDED ) & TVIS_EXPANDED )
				bCkeck = !bCkeck;

			
			HTREEITEM hChildItem = m_ctrlFields.GetChildItem( hItem );



			while( hChildItem )
			{
				m_ctrlFields.SetCheck( hChildItem, bCkeck );
				hChildItem = m_ctrlFields.GetNextItem( hChildItem, TVGN_NEXT );
			}
		}
	}
	*pResult = 0;
}
