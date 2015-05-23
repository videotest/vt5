// FilterCombo.cpp : implementation file
//

#include "stdafx.h"
#include "dbcontrols.h"
#include "FilterCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterCombo

CFilterCombo::CFilterCombo()
{
	
}


/////////////////////////////////////////////////////////////////////////////
CFilterCombo::~CFilterCombo()
{
}


BEGIN_MESSAGE_MAP(CFilterCombo, CComboBox)
	//{{AFX_MSG_MAP(CFilterCombo)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDS_MENU_ADD_FILTER, OnAddFilter)
	ON_COMMAND(IDS_MENU_EDIT_FILTER, OnEditFilter)
	ON_COMMAND(IDS_MENU_EDIT_FILTER, OnDeleteFilter)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterCombo message handlers

/////////////////////////////////////////////////////////////////////////////
sptrIQueryObject CFilterCombo::GetActiveQuery()
{	
	sptrIDBControl spDBControl( GetParent()->GetControllingUnknown() );

	ASSERT( spDBControl != NULL );

	if( spDBControl == NULL )
		return NULL;

	IUnknown* pUnkDoc = NULL;
	spDBControl->GetDBaseDocument( &pUnkDoc );
	if( pUnkDoc == NULL )
		return NULL;

	sptrIDBaseDocument spDBDoc( pUnkDoc );
	if( spDBDoc == NULL )
		return NULL;
	
	spDBDoc->Release();

	IUnknown* pUnkQuery = NULL;
	spDBDoc->GetActiveQuery( &pUnkQuery );
	if( pUnkQuery == NULL )
		return NULL;

	sptrIQueryObject spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return NULL;

	return spQuery;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterCombo::BuildAppearance( bool bSetSelectionToFirstItem )
{
	ResetContent();


	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	sptrIDBControl spDBControl( GetParent()->GetControllingUnknown() );
	ASSERT( spDBControl != NULL );

	if( spDBControl == NULL )
		return;


	BSTR bstrTable, bstrField;	
	spDBControl->GetTableName( &bstrTable );
	spDBControl->GetFieldName( &bstrField );
	CString strTable, strField;
	strTable = bstrTable;
	strField = bstrField;

	/*
	int nCount = 0;
	spQuery->GetFilterCountByFieldTable( bstrTable, bstrField, &nCount );

	for( int i=0;i<nCount;i++ )
	{
		int nIndex = 0;
		spQuery->GetFilterIndexByFieldTable(bstrTable, bstrField, i, &nIndex);
		if( nIndex == -1 )
			continue;

		BSTR bsrtFilterAsString;
		if( S_OK != spQuery->GetFilterAsShortString( nIndex, &bsrtFilterAsString) )
			continue;

		CString strFilterAsString = bsrtFilterAsString;
		::SysFreeString( bsrtFilterAsString );

		int nAddIndex = AddString( strFilterAsString );
		SetItemData( nAddIndex, (DWORD)nIndex );
	}

	::SysFreeString( bstrTable );
	::SysFreeString( bstrField );	

	if( bSetSelectionToFirstItem )
		SetCurSel( 0 );

  */

}

/////////////////////////////////////////////////////////////////////////////
void CFilterCombo::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CString strAddFilter, strEditFilter, strDeleteFilter;

	strAddFilter.LoadString( IDS_MENU_ADD_FILTER );
	strEditFilter.LoadString( IDS_MENU_EDIT_FILTER );
	strDeleteFilter.LoadString( IDS_MENU_DELETE_FILTER );	

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( MF_ENABLED | MF_STRING, IDS_MENU_ADD_FILTER, strAddFilter );
	menu.AppendMenu( MF_ENABLED | MF_STRING, IDS_MENU_EDIT_FILTER, strEditFilter );
	menu.AppendMenu( MF_ENABLED | MF_STRING, IDS_MENU_EDIT_FILTER, strDeleteFilter );
		
	ClientToScreen( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );
	
	//CComboBox::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CFilterCombo::OnAddFilter()
{

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	int nIndex = GetCurSel();


	sptrIDBControl spDBControl( GetParent()->GetControllingUnknown() );
	ASSERT( spDBControl != NULL );

	if( spDBControl == NULL )
		return;


	BSTR bstrTable, bstrField;	
	spDBControl->GetTableName( &bstrTable );
	spDBControl->GetFieldName( &bstrField );


	
	/*int nFilterCount = 0;
	spQuery->GetFilterCountByFieldTable( bstrTable, bstrField, &nFilterCount );
	

	CString strTable, strField;
	strTable = bstrTable;
	strField = bstrField;

	::SysFreeString( bstrTable );
	::SysFreeString( bstrField );	

	if( S_OK != spQuery->AddFilterVisible( _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ) ) )
	{
		return;
	}

	BuildAppearance( );
	SetCurSel( nFilterCount );
	*/

}

/////////////////////////////////////////////////////////////////////////////
void CFilterCombo::OnEditFilter()
{	

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	int nIndex = GetCurSel();
	if( nIndex < 0 )
		return;

	DWORD dwData = GetItemData( nIndex );

	/*
	if( spQuery->EditFilterVisible( (int)dwData ) != S_OK )
		return;
		*/

	BuildAppearance( );
	SetCurSel( nIndex );
}

/////////////////////////////////////////////////////////////////////////////
void CFilterCombo::OnDeleteFilter()
{

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	int nIndex = GetCurSel();
	if( nIndex < 0 )
		return;

	DWORD dwData = GetItemData( nIndex );

	/*
	if( spQuery->DeleteFilter( (int)dwData ) != S_OK )
		return;
		*/

	BuildAppearance( );
	SetCurSel( nIndex -1 );
}


/////////////////////////////////////////////////////////////////////////////

int CFilterCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(lf) );
	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );	
	if( pFont )
	{
		pFont->GetLogFont( &lf );
		m_Font.CreatePointFontIndirect( &lf );
		SetFont( &m_Font );
	}

	
	
	return 0;
}
