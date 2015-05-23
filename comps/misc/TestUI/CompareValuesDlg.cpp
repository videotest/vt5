// CompareValuesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "CompareValuesDlg.h"


// CCompareValuesDlg dialog

IMPLEMENT_DYNAMIC(CCompareValuesDlg, CDialog)
CCompareValuesDlg::CCompareValuesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompareValuesDlg::IDD, pParent)
{
	m_pErrDescr = 0;
	m_sptrDocCmp = 0;
	m_sptrDocSave = 0;
}

CCompareValuesDlg::~CCompareValuesDlg()
{
	m_sptrDocCmp = 0;
	m_sptrDocSave = 0;

	m_err_items.clear( );
}

void CCompareValuesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ARRAYS, m_ctrlCombo);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}


BEGIN_MESSAGE_MAP(CCompareValuesDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ARRAYS, OnCbnSelchangeComboArrays)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, OnNMCustomdrawList1)
END_MESSAGE_MAP()


// CCompareValuesDlg message handlers
void CCompareValuesDlg::SetTestParam( TEST_ERR_DESCR *pErrDescr )
{
    if( !pErrDescr )
		return;
	
	m_pErrDescr = pErrDescr;
}

BOOL CCompareValuesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( !m_pErrDescr )
		return FALSE;

	// load documents
	CLSID	clsidData = {0};

    if( ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) != S_OK  )
		return 0;

	if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_sptrDocCmp ) != S_OK )
		return 0;
    if( m_sptrDocCmp == 0 )
		return 0;

	if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&m_sptrDocSave ) != S_OK )
		return 0;
    if( m_sptrDocSave == 0 )
		return 0;

    IFileDataObjectPtr	sptrFFCmp = m_sptrDocCmp;
    if( sptrFFCmp == 0 )
		return 0;

	CString strNum, strPath = (char *)_bstr_t( m_pErrDescr->bstrPath );

	if( 0 != (int)(m_pErrDescr->nTryIndex) )
		strNum.Format( ".%x", (int)(m_pErrDescr->nTryIndex) );

 	sptrFFCmp->LoadFile( _bstr_t( strPath + _T("\\TDC_") + 
		::GetValueString( GetAppUnknown(), "\\General", "Language", "en" ) + strNum + _T(".image") ) );

	IFileDataObjectPtr	sptrFFSave = m_sptrDocSave;
	if( sptrFFSave == 0 )
		return 0;

	sptrFFSave->LoadFile( _bstr_t( strPath + _T("\\TDS_") + 
		::GetValueString( GetAppUnknown(), "\\General", "Language", "en" ) + strNum + _T(".image") ) );

	m_ctrlList.SetExtendedStyle( LVS_EX_GRIDLINES /*| LVS_EX_FULLROWSELECT*/ );
	// create columns
	CString str_column_title( _T("") );
	str_column_title.LoadString( IDS_VAR_NAME );
	m_ctrlList.InsertColumn( 0, str_column_title, LVCFMT_LEFT, 150 );
	str_column_title.LoadString( IDS_VAR_VALUE_SAMPLE );
	m_ctrlList.InsertColumn( 1, str_column_title, LVCFMT_CENTER, 150 );
	str_column_title.LoadString( IDS_VAR_VALUE_CURR_TEST );
	m_ctrlList.InsertColumn( 2, str_column_title, LVCFMT_CENTER, 150 );

	// fill combobox
	m_ctrlCombo.Clear();
	for( int i = 0; i < m_pErrDescr->lszArrays; i++ )
		m_ctrlCombo.AddString( _bstr_t(m_pErrDescr->pbstrArrNames[ i ]) );
    
	m_ctrlCombo.SetCurSel( m_pErrDescr->lszArrays ? 0 : -1 );
	OnCbnSelchangeComboArrays();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL	CCompareValuesDlg::fill_list( CString str_array_name )
{
	if( m_sptrDocCmp == 0 || m_sptrDocSave == 0 )
		return FALSE;

    IUnknown *punk_obj_cmp = 0;
	punk_obj_cmp = ::GetObjectByName( m_sptrDocCmp, _bstr_t(str_array_name), _bstr_t( _T(szTypeTestDataArray) ) );
	ITestDataArrayPtr sptr_tda_cmp = punk_obj_cmp;
	if( punk_obj_cmp )
	{
		punk_obj_cmp->Release( );
		punk_obj_cmp = 0;
	}
	if( sptr_tda_cmp == 0 )
		return FALSE;

	IUnknown *punk_obj_save = 0;
	punk_obj_save = ::GetObjectByName( m_sptrDocSave, _bstr_t(str_array_name), _bstr_t( _T(szTypeTestDataArray) ) );
	ITestDataArrayPtr sptr_tda_save = punk_obj_save;
	if( punk_obj_save  )
	{
		punk_obj_save ->Release( );
		punk_obj_save = 0;
	}
	if( sptr_tda_save == 0 )
		return FALSE;

    
	// clear list
	m_ctrlList.DeleteAllItems( );
	m_err_items.clear( );

	// adding items
    long litem_count = 0;
	sptr_tda_save->GetItemCount( &litem_count );
	long ladditional_items = 0;
	for( long litem = 0; litem < litem_count; litem ++ )
	{
		_bstr_t bstrt_item_name( _T("") ),
				bstrt_item_name2( _T("") );

		sptr_tda_save->GetItemName( (UINT)(litem), bstrt_item_name.GetAddress( ) );
		sptr_tda_cmp->GetItemName( (UINT)(litem), bstrt_item_name2.GetAddress( ) );

		_variant_t	vart_item_val, 
					vart_item_val2;

		sptr_tda_save->GetItem( (UINT)(litem), vart_item_val.GetAddress() );
		sptr_tda_cmp->GetItem( (UINT)(litem), vart_item_val2.GetAddress( ) );

		if( bstrt_item_name == bstrt_item_name2 )                				
		{
			m_ctrlList.InsertItem( litem + ladditional_items, bstrt_item_name );

			
			// [vanek] SBT: 910 compare items using compare expression - 27.05.2004
			long lequal_items = 0;
			sptr_tda_save->CompareItem( (UINT)(litem), vart_item_val2, &lequal_items );
			if( !lequal_items )
				m_err_items.add_tail( litem + ladditional_items );
            
			vart_item_val.ChangeType( VT_BSTR );
			vart_item_val2.ChangeType( VT_BSTR );

			m_ctrlList.SetItemText( litem + ladditional_items, 1, _bstr_t(vart_item_val) );
			m_ctrlList.SetItemText( litem + ladditional_items, 2, _bstr_t(vart_item_val2) );
		}
		else
		{
			m_ctrlList.InsertItem( litem + ladditional_items, bstrt_item_name );
            m_err_items.add_tail( litem + ladditional_items );
            vart_item_val.ChangeType( VT_BSTR );
			m_ctrlList.SetItemText( litem + ladditional_items, 1,_bstr_t(vart_item_val) );
			m_ctrlList.SetItemText( litem + ladditional_items, 2, _T("---") );

            ladditional_items ++;
			m_ctrlList.InsertItem( litem + ladditional_items, bstrt_item_name2 );
			m_err_items.add_tail( litem + ladditional_items );
			vart_item_val2.ChangeType( VT_BSTR );
			m_ctrlList.SetItemText( litem + ladditional_items, 1, _T("---") );
			m_ctrlList.SetItemText( litem + ladditional_items, 2, _bstr_t(vart_item_val2) );	
		}

	}

    return TRUE;
}

void CCompareValuesDlg::OnCbnSelchangeComboArrays()
{
	CString str_name_array( _T("") );
	m_ctrlCombo.GetLBText( m_ctrlCombo.GetCurSel(  ), str_name_array );
	fill_list( str_name_array );
}

void CCompareValuesDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pLVNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	
	switch( pLVNMCD->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			long litem = (long)(pLVNMCD->nmcd.dwItemSpec);
			if( m_err_items.find( litem ) )
			{
				//pLVNMCD->clrTextBk = RGB( 255,0,0);
				pLVNMCD->clrText = RGB( 255,0,0);

				*pResult = CDRF_NEWFONT;
			}
			else
				*pResult = 0;
		}
		break;
	default:
		*pResult = 0;
	}
}
