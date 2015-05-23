// FilterOrganizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FilterOrganizerDlg.h"
#include "FilterCondition.h"
#include "FilterNameDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CFilterOrganizerDlg dialog


CFilterOrganizerDlg::CFilterOrganizerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterOrganizerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterOrganizerDlg)
	//}}AFX_DATA_INIT

	m_bSelectFilter = false;
}

/////////////////////////////////////////////////////////////////////////////
CFilterOrganizerDlg::~CFilterOrganizerDlg()
{
}


/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterOrganizerDlg)
	DDX_Control(pDX, IDC_CONDITION_LIST, m_ctrlConditionList);
	DDX_Control(pDX, IDC_FILTER_LIST, m_ctrlFilterList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterOrganizerDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterOrganizerDlg)
	ON_LBN_SELCHANGE(IDC_FILTER_LIST, OnSelchangeFilterList)
	ON_LBN_SELCHANGE(IDC_CONDITION_LIST, OnSelchangeConditionList)
	ON_BN_CLICKED(IDC_NEW_FILTER, OnNewFilter)
	ON_BN_CLICKED(IDC_RENAME_FILTER, OnRenameFilter)
	ON_BN_CLICKED(IDC_DELETE_FILTER, OnDeleteFilter)
	ON_BN_CLICKED(IDC_NEW_COND, OnNewCond)
	ON_BN_CLICKED(IDC_EDIT_COND, OnEditCond)
	ON_BN_CLICKED(IDC_COPY_COND, OnCopyCond)
	ON_BN_CLICKED(IDC_REMOVE_COND, OnRemoveCond)
	ON_BN_CLICKED(IDC_LEFT_BRACKET, OnLeftBracket)
	ON_BN_CLICKED(IDC_RIGHT_BRACKET, OnRightBracket)
	ON_BN_CLICKED(IDC_RESET_BRACKETS, OnResetBrackets)
	ON_BN_CLICKED(IDC_SELECT_AND_EXIT, OnSelectAndExit)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterOrganizerDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CFilterOrganizerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();		

//	CHelpDlgImpl::InitHelpImlp( GetSafeHwnd(), "DBaseDoc", "FilterOrganizerDlg" );

	
	m_filterHolder.SetFilterInOrganizerMode( true );	

	nLastActiveFilterIndex = 0;
	nLastActiveConditionIndex = 0;


	BuildFilterArray();		

	int nidx = -1;
	for( int i=0;i<m_filterHolder.GetFilterCount();i++ )
	{
		if( m_strActiveFilter == m_filterHolder.GetFilterName( i ) )
		{
			nidx = i;
			break;
		}
	}

	BuildAppearance();

	if( nidx != -1 )
	{
		m_ctrlFilterList.SetCurSel( nidx );
		OnSelchangeFilterList();
	}

	
	return TRUE;	             
}


/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::SetFilterHolder( IUnknown* punk )
{
	m_ptrFilterHolder = punk;

	/*
	if( m_ptrFilterHolder )
	{
		BSTR bstrActiveFilter = NULL;
		m_ptrFilterHolder->GetActiveFilter( &bstrActiveFilter );
		m_strOldActiveFilter = bstrActiveFilter;
		::SysFreeString( bstrActiveFilter );
	}	
	*/
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::BuildFilterArray()
{

	if( m_ptrFilterHolder == NULL )
		return;
	
	int nFilteCount = -1;
	
	
	m_ptrFilterHolder->GetFiltersCount( &nFilteCount );
	for( int i=0;i<nFilteCount;i++ )
	{
		BSTR bstrFilterName = NULL;
		if( S_OK != m_ptrFilterHolder->GetFilter( i, &bstrFilterName) )
			continue;

		_bstr_t _bstrFilterName(bstrFilterName);

		if( bstrFilterName )
			::SysFreeString( bstrFilterName );				
		
		m_filterHolder.AddFilter( (LPCSTR)_bstrFilterName );		

		int nCondCount = -1;
		
		if( S_OK != m_ptrFilterHolder->GetFilterConditionCount( _bstrFilterName, &nCondCount) )
			continue;

		for( int j=0;j<nCondCount;j++ )
		{
			BSTR bstrTable, bstrField, bstrCondition, bstrValue, bstrValueTo;
			BOOL bAndUnion;
			int nLeftBracket, nRightBracket;

			if( S_OK != m_ptrFilterHolder->GetFilterCondition(_bstrFilterName, j,
								&bstrTable, &bstrField, 
								&bstrCondition, 
								&bstrValue, &bstrValueTo,
								&bAndUnion,
								&nLeftBracket, &nRightBracket					
								) )
			{				
				continue;
			}

			m_filterHolder.AddFilterCondition( (LPCSTR)_bstrFilterName, 
									bstrTable, bstrField, 
									bstrCondition, 
									bstrValue, bstrValueTo,
									bAndUnion, 
									nLeftBracket, nRightBracket );


			if( bstrTable )
				::SysFreeString( bstrTable );

			if( bstrField )
				::SysFreeString( bstrField );

			if( bstrCondition )
				::SysFreeString( bstrCondition );

			if( bstrValue )
				::SysFreeString( bstrValue );

			if( bstrValueTo )
				::SysFreeString( bstrValueTo );
		}
	}

	SetListoxAutoExtent( &m_ctrlConditionList );
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::BuildAppearance()
{	
	
	nLastActiveFilterIndex = m_ctrlFilterList.GetCurSel();		

	m_ctrlFilterList.ResetContent();

	for( int i=0;i<m_filterHolder.GetFilterCount();i++ )
	{
		m_ctrlFilterList.AddString( m_filterHolder.GetFilterName( i ) );
	}

	SetLastActiveFilterIndex();	

	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::SetLastActiveFilterIndex()	
{
	if( nLastActiveFilterIndex < 0 || nLastActiveFilterIndex >= m_ctrlFilterList.GetCount() )
		nLastActiveFilterIndex = 0;

	if( m_ctrlFilterList.GetCount() > 0 )
		m_ctrlFilterList.SetCurSel( nLastActiveFilterIndex );

}

/////////////////////////////////////////////////////////////////////////////
CString CFilterOrganizerDlg::GetActiveFilterName()
{
	if( !::IsWindow( m_ctrlFilterList.GetSafeHwnd() ) ) 
		return "";

	int nItemCount = m_ctrlFilterList.GetCount();
	if( nItemCount < 0 )
		return "";

	int nCurSel = m_ctrlFilterList.GetCurSel();
	if( nCurSel < 0 )
		return "";

	CString strResult;
	
	m_ctrlFilterList.GetText( nCurSel, strResult );


	
	if( m_filterHolder.GetFilter(strResult) == NULL )
		return "";
		

	return strResult;
}

/////////////////////////////////////////////////////////////////////////////
int CFilterOrganizerDlg::GetActiveConditionIndex()
{
	if( !::IsWindow( m_ctrlConditionList.GetSafeHwnd() ) ) 
		return -1;

	int nItemCount = m_ctrlConditionList.GetCount();
	if( nItemCount < 2 )
		return -1;

	int nCurSel = m_ctrlConditionList.GetCurSel();
	
	if( nCurSel == -1 )
		return -1;

	int nCondIndex = nCurSel - 1;

	
	
	int nCondCount = m_filterHolder.GetFilterConditionCount( GetActiveFilterName() );
	if( nCondCount < 0 )
		return -1;
	

	if( nCondIndex < 0 || nCondIndex >= nCondCount )
		return -1;

	return nCondIndex;	
		

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::SetVisibleButton()
{
	CString strActiveFilter = GetActiveFilterName();
	if( strActiveFilter.IsEmpty() )
	{
		GetDlgItem(IDC_RENAME_FILTER)->EnableWindow( FALSE );
		GetDlgItem(IDC_DELETE_FILTER)->EnableWindow( FALSE );
		GetDlgItem(IDC_NEW_COND)->EnableWindow( FALSE );
		GetDlgItem(IDC_EDIT_COND)->EnableWindow( FALSE );
		GetDlgItem(IDC_COPY_COND)->EnableWindow( FALSE );
		GetDlgItem(IDC_REMOVE_COND)->EnableWindow( FALSE );
		GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( FALSE );
		GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( FALSE );
		GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( FALSE );
		GetDlgItem(IDC_SELECT_AND_EXIT)->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem(IDC_NEW_COND)->EnableWindow( TRUE );
		GetDlgItem(IDC_RENAME_FILTER)->EnableWindow( TRUE );
		GetDlgItem(IDC_DELETE_FILTER)->EnableWindow( TRUE );
		GetDlgItem(IDC_SELECT_AND_EXIT)->EnableWindow( TRUE );

		int nCondIndex = GetActiveConditionIndex();
		if( nCondIndex == -1 )
		{			
			GetDlgItem(IDC_EDIT_COND)->EnableWindow( FALSE );
			GetDlgItem(IDC_COPY_COND)->EnableWindow( FALSE );
			GetDlgItem(IDC_REMOVE_COND)->EnableWindow( FALSE );
			GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( FALSE );
			GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( FALSE );
			GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( FALSE );
		}
		else
		{			
			GetDlgItem(IDC_EDIT_COND)->EnableWindow( TRUE );
			GetDlgItem(IDC_COPY_COND)->EnableWindow( TRUE );
			GetDlgItem(IDC_REMOVE_COND)->EnableWindow( TRUE );
			GetDlgItem(IDC_LEFT_BRACKET)->EnableWindow( TRUE );
			GetDlgItem(IDC_RIGHT_BRACKET)->EnableWindow( TRUE );
			GetDlgItem(IDC_RESET_BRACKETS)->EnableWindow( TRUE );
		}
	}

}


/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnSelchangeFilterList() 
{		
	nLastActiveConditionIndex = m_ctrlConditionList.GetCurSel();

	m_ctrlConditionList.ResetContent();
	SetVisibleButton();	

	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	CString strQueryName = "Query";

	sptrIDBaseDocument spIDBDoc;
	GetDBaseDocument( spIDBDoc );
	if( spIDBDoc )
	{
		IUnknown* punk = 0;
		spIDBDoc->GetActiveQuery( &punk );
		if( punk )
		{
			strQueryName = ::GetObjectName( punk );
			punk->Release();
		}
	}

	
	m_ctrlConditionList.AddString( m_filterHolder.GetFilterForOrganizer( m_ptrFilterHolder, strFilter, strQueryName) );

	int nCondCount = m_filterHolder.GetFilterConditionCount( strFilter );
	for( int i=0;i<nCondCount;i++ )
	{
		m_ctrlConditionList.AddString( m_filterHolder.GetFilterConditionForOrganizer( m_ptrFilterHolder, strFilter, i ) );
	}

	SetLastActiveConditionIndex();	

	SetVisibleButton();	


	SetListoxAutoExtent( &m_ctrlConditionList );	

}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::SetLastActiveConditionIndex()
{
	if( nLastActiveConditionIndex < 0 || nLastActiveConditionIndex >= m_ctrlConditionList.GetCount() )
		nLastActiveConditionIndex = 0;

	if( m_ctrlConditionList.GetCount() > 0 )
		m_ctrlConditionList.SetCurSel( nLastActiveConditionIndex );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnSelchangeConditionList() 
{	
	SetVisibleButton();		
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnNewFilter() 
{	

	CFilterNameDlg dlg;
	if( dlg.DoModal() != IDOK )
		return;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if( !m_filterHolder.IsFilterNameUnic( dlg.m_strFilterName ) )
	{
		AfxMessageBox( IDS_FILTER_INVALID_NAME );
		return;
	}

	m_filterHolder.AddFilter( dlg.m_strFilterName );

	
	BuildAppearance();

	m_ctrlFilterList.SetCurSel( m_ctrlFilterList.GetCount() - 1 );
	OnSelchangeFilterList();
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnRenameFilter() 
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CQueryFilter* pFilter = m_filterHolder.GetFilter( GetActiveFilterName() );
	if( pFilter == NULL )
		return;

	CFilterNameDlg dlg;
	dlg.m_strFilterName = pFilter->m_strName;
	if( dlg.DoModal() != IDOK )
		return;

	if( !m_filterHolder.IsFilterNameUnic( dlg.m_strFilterName ) )
	{
		AfxMessageBox( IDS_FILTER_INVALID_NAME );
		return;
	}

	pFilter->m_strName = dlg.m_strFilterName;


	BuildAppearance();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnDeleteFilter() 
{	
	
	
	CQueryFilter* pFilter = m_filterHolder.GetFilter( GetActiveFilterName() );
	if( pFilter == NULL )
		return;

	m_filterHolder.DeleteFilter( GetActiveFilterName() );

	BuildAppearance();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnNewCond() 
{	
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	CFilterConditionDlg dlg;	
	dlg.SetFilterHolder( m_ptrFilterHolder );
	if( dlg.DoModal() != IDOK)
		return;	


	m_filterHolder.AddFilterCondition( strFilter, 
								dlg.m_strTable, dlg.m_strField,
								dlg.m_strCondition, 
								dlg.m_strValue,
								dlg.m_strValueTo,
								!dlg.m_bOrUnion,
								0,
								0);
	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnEditCond() 
{	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondIndex = GetActiveConditionIndex();
	if( nCondIndex < 0 )
		return;

	
	CString strTable, strField, strCondition, strValue, strValueTo;
	BOOL bAndUnion;
	int nLeftBracketCount, nRightBracketCount;

	if( !m_filterHolder.GetFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount
							) )
	{
		return;
	}

	CFilterConditionDlg dlg;	
	dlg.SetFilterHolder( m_ptrFilterHolder );

	dlg.m_strTable = strTable;
	dlg.m_strField = strField;
	

	dlg.m_strValue		= strValue;
	dlg.m_strValueTo	= strValueTo;

	dlg.m_strCondition	= strCondition;
	
	dlg.m_bOrUnion		= !bAndUnion;


	if( dlg.DoModal() != IDOK)
		return;	

	strTable.Empty();
	strField.Empty();
	

	m_filterHolder.EditFilterCondition( strFilter, nCondIndex,
								dlg.m_strTable, dlg.m_strField,
								dlg.m_strCondition, 
								dlg.m_strValue,
								dlg.m_strValueTo,
								!dlg.m_bOrUnion,
								nLeftBracketCount, nRightBracketCount
							);

	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnCopyCond() 
{	
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondIndex = GetActiveConditionIndex();
	if( nCondIndex < 0 )
		return;

	
	CString strTable, strField, strCondition, strValue, strValueTo;
	BOOL bAndUnion;
	int nLeftBracketCount, nRightBracketCount;

	if( !m_filterHolder.GetFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount
							) )
	{
		return;
	}

	m_filterHolder.AddFilterCondition( strFilter, 
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount
							);

	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnRemoveCond() 
{	
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondIndex = GetActiveConditionIndex();
	if( nCondIndex < 0 )
		return;

	m_filterHolder.DeleteFilterCondition( strFilter, nCondIndex );

	OnSelchangeFilterList();	
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnLeftBracket() 
{	
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondIndex = GetActiveConditionIndex();
	if( nCondIndex < 0 )
		return;

	
	CString strTable, strField, strCondition, strValue, strValueTo;
	BOOL bAndUnion;
	int nLeftBracketCount, nRightBracketCount;

	if( !m_filterHolder.GetFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount
							) )
	{
		return;
	}

	m_filterHolder.EditFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount + 1, nRightBracketCount
							);
	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnRightBracket() 
{	
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondIndex = GetActiveConditionIndex();
	if( nCondIndex < 0 )
		return;

	
	CString strTable, strField, strCondition, strValue, strValueTo;
	BOOL bAndUnion;
	int nLeftBracketCount, nRightBracketCount;

	if( !m_filterHolder.GetFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount
							) )
	{
		return;
	}

	m_filterHolder.EditFilterCondition( strFilter, nCondIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bAndUnion,
							nLeftBracketCount, nRightBracketCount + 1
							);
	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnResetBrackets() 
{
	
	
	CString strFilter = GetActiveFilterName();
	if( strFilter.IsEmpty() )
		return;

	int nCondCount = m_filterHolder.GetFilterConditionCount( strFilter );
	for( int i=0;i<nCondCount;i++ )
	{
		CString strTable, strField, strCondition, strValue, strValueTo;
		BOOL bAndUnion;
		int nLeftBracketCount, nRightBracketCount;

		if( !m_filterHolder.GetFilterCondition( strFilter, i,
								strTable, strField,
								strCondition, strValue,
								strValueTo, bAndUnion,
								nLeftBracketCount, nRightBracketCount
								) )
		{
			return;
		}

		m_filterHolder.EditFilterCondition( strFilter, i,
								strTable, strField,
								strCondition, strValue,
								strValueTo, bAndUnion,
								0, 0
								);
	}

	OnSelchangeFilterList();
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnSelectAndExit() 
{	
	m_strActiveFilter = GetActiveFilterName();
	SaveQueryObject();	
	
	m_bSelectFilter = true;	

	OnOK();			
	
}

/////////////////////////////////////////////////////////////////////////////
void CFilterOrganizerDlg::OnOK() 
{
	if( !m_bSelectFilter )
		SaveQueryObject();
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterOrganizerDlg::SaveQueryObject()
{
	
	
	if( m_ptrFilterHolder == NULL )
		return false;

	//Delete old
	int nFilterCount = -1;
	m_ptrFilterHolder->GetFiltersCount( &nFilterCount );
	for( int i=nFilterCount-1;i>=0;i-- )
	{
		BSTR bstrFilter = NULL;
		if( S_OK != m_ptrFilterHolder->GetFilter( i, &bstrFilter ) )
			continue;

		m_ptrFilterHolder->DeleteFilter( bstrFilter );		

		if( bstrFilter )
			::SysFreeString( bstrFilter );
	}

	//Create new
	nFilterCount = m_filterHolder.GetFilterCount();
	for( i=0;i<nFilterCount;i++ )
	{
		_bstr_t bstrNewFilter( m_filterHolder.GetFilterName(i) );
		m_ptrFilterHolder->AddFilter( bstrNewFilter );

		int nCondCount = -1;
		nCondCount = m_filterHolder.GetFilterConditionCount( (LPCSTR)bstrNewFilter );

		for( int j=0;j<nCondCount;j++ )
		{
			CString strTable, strField, strCondition, strValue, strValueTo;
			BOOL bAndUnion;
			int nLeftBracketCount, nRightBracketCount;

			if( !m_filterHolder.GetFilterCondition( (LPCSTR)bstrNewFilter, j,
									strTable, strField,
									strCondition, strValue,
									strValueTo, bAndUnion,
									nLeftBracketCount, nRightBracketCount
									) )
			{
				continue;
			}

			m_ptrFilterHolder->AddFilterCondition( bstrNewFilter,
									_bstr_t((LPCSTR)strTable), _bstr_t((LPCSTR)strField),
									_bstr_t((LPCSTR)strCondition), _bstr_t((LPCSTR)strValue),
									_bstr_t((LPCSTR)strValueTo), bAndUnion,
									nLeftBracketCount, nRightBracketCount);

		}
	}
	

	return true;

}

LRESULT CFilterOrganizerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
/*	if( CHelpDlgImpl::IsHelpMsg( message ) )
	{
		return CHelpDlgImpl::FilterHelpMsg( message, wParam, lParam );
	}*/
	
	return CDialog::WindowProc(message, wParam, lParam);
}
