#include "stdafx.h"
#include "resource.h"
#include "docman.h"
#include "dbasedoc.h"






//Filters
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::SetFilterInOrganizerMode( BOOL bSetFilterInOrganizerMode )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, SetFilterInOrganizerMode)
	{	
		pThis->m_filterHolder.SetFilterInOrganizerMode( bSetFilterInOrganizerMode ? true : false );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterInOrganizerMode( BOOL* pbSetFilterInOrganizerMode )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterInOrganizerMode)
	{	
		*pbSetFilterInOrganizerMode = ( pThis->m_filterHolder.GetFilterInOrganizerMode() ? true : false );
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFiltersCount( int* pnCount )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFiltersCount )
	{	
		*pnCount = pThis->m_filterHolder.GetFilterCount();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilter( int nFilterIndex, BSTR* pbstrName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilter)
	{	
		*pbstrName = pThis->m_filterHolder.GetFilterName( nFilterIndex ).AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::AddFilter( BSTR bstrName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, AddFilter)
	{	
		if( !pThis->m_filterHolder.AddFilter( bstrName ) )
			return S_FALSE;

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::EditFilter( BSTR bsrtFilterName, BSTR bstrName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, EditFilter)
	{	
		if( !pThis->m_filterHolder.EditFilter( bsrtFilterName, bstrName) )
			return S_FALSE;

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::DeleteFilter( BSTR bsrtFilterName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, DeleteFilter)
	{	
		if( !pThis->m_filterHolder.DeleteFilter( bsrtFilterName ) )
			return S_FALSE;		

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterForSQL( BSTR bsrtFilterName, BSTR* pbstrSQL )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterForSQL)
	{	
		*pbstrSQL = pThis->m_filterHolder.GetFilterForSQL( pThis->GetControllingUnknown(), bsrtFilterName ).AllocSysString();

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterForOrganizer( BSTR bsrtFilterName, BSTR* pbstrOrganizer )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterForOrganizer)
	{	
		CString strQueryName;

		INamedObject2Ptr spNO2( pThis->GetActiveQuery() );
		if( spNO2 != NULL )
		{
			BSTR bstrName;
			spNO2->GetName( &bstrName );
			strQueryName = bstrName;

			if( bstrName )
				::SysFreeString( bstrName );
		}				

		*pbstrOrganizer = pThis->m_filterHolder.GetFilterForOrganizer( pThis->GetControllingUnknown(), bsrtFilterName, strQueryName ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::IsValidFilter( BSTR bstrFilterName, BOOL bReportError, BOOL* pbValid )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterForOrganizer)
	{	
		*pbValid = ( pThis->m_filterHolder.IsValidFilter( bstrFilterName, 
										(bReportError ? true : false) 
									) ? TRUE : FALSE );
		return S_OK;
	}
	_catch_nested;
}



//Filter Condition
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterConditionCount( BSTR bsrtFilterName, int* pnCount )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterConditionCount)
	{	
		*pnCount = pThis->m_filterHolder.GetFilterConditionCount( bsrtFilterName );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
							BSTR* pbstrTable, BSTR* pbstrField,
							BSTR* pbstrCondition, BSTR* pbstrValue,
							BSTR* pbstrValueTo, BOOL* pbANDunion,
							int* pnLeftBracketCount, int* pnRightBracketCount
							)
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterCondition)
	{	
		
		CString strTable;
		CString strField;
		CString strCondition;
		CString strValue;
		CString strValueTo;
		BOOL bANDUnion;
		int nLeftBracketCount;
		int nRightBracketCount;

		if( !pThis->m_filterHolder.GetFilterCondition( bsrtFilterName, nConditionIndex,
							strTable, strField,
							strCondition, strValue,
							strValueTo, bANDUnion,
							nLeftBracketCount, nRightBracketCount ) )
		{
			return S_FALSE;
		}
		
		*pbstrTable				= strTable.AllocSysString( );
		*pbstrField				= strField.AllocSysString( );
		*pbstrCondition			= strCondition.AllocSysString( );
		
		*pbstrValue				= strValue.AllocSysString( );
		*pbstrValueTo			= strValueTo.AllocSysString( );

		*pbANDunion				= bANDUnion;

		*pnLeftBracketCount		= nLeftBracketCount;
		*pnRightBracketCount	= nRightBracketCount;		
		

		return S_OK;
	}
	_catch_nested;
}
							
							
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::AddFilterCondition( BSTR bsrtFilterName,
							BSTR bstrTable, BSTR bstrField,
							BSTR bstrCondition, BSTR bstrValue,
							BSTR bstrValueTo, BOOL bANDunion,
							int nLeftBracketCount, int nRightBracketCount
							)
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, AddFilterCondition)
	{	

		if( !pThis->m_filterHolder.AddFilterCondition( bsrtFilterName,
							bstrTable, bstrField,
							bstrCondition, bstrValue,
							bstrValueTo, bANDunion,
							nLeftBracketCount, nRightBracketCount ) )
		{
			if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
			{
				pThis->ReBuildFilterChooserList();
				::RefreshFilterPropPage();				
			}

			return S_FALSE;
		}


		if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
		{
			pThis->ReBuildFilterChooserList();
			//::RefreshFilterPropPage();
			
		}

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}



////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::EditFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
							BSTR bstrTable, BSTR bstrField,
							BSTR bstrCondition, BSTR bstrValue,
							BSTR bstrValueTo, BOOL bANDunion,
							int nLeftBracketCount, int nRightBracketCount
							)
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, EditFilterCondition)
	{	

		if( !pThis->m_filterHolder.EditFilterCondition( bsrtFilterName, nConditionIndex,
							bstrTable, bstrField,
							bstrCondition, bstrValue,
							bstrValueTo, bANDunion,
							nLeftBracketCount, nRightBracketCount ) )
		{
			if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
			{
				pThis->ReBuildFilterChooserList();
				::RefreshFilterPropPage();
			}
			return S_FALSE;
		}

		if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
		{
			pThis->ReBuildFilterChooserList();
			::RefreshFilterPropPage();
		}

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::DeleteFilterCondition( BSTR bsrtFilterName, int nConditionIndex)
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, DeleteFilterCondition)
	{	

		if( !pThis->m_filterHolder.DeleteFilterCondition( bsrtFilterName, nConditionIndex ) )
		{
			if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
			{
				pThis->ReBuildFilterChooserList();
				::RefreshFilterPropPage();
			}

			return S_FALSE;
		}
		
		if( !pThis->m_filterHolder.GetFilterInOrganizerMode())
		{
			pThis->ReBuildFilterChooserList();
			::RefreshFilterPropPage();
		}

		SetModifiedFlagToDoc( pThis->GetControllingUnknown() );

		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetFilterConditionForOrganizer( BSTR bsrtFilterName, int nConditionIndex,
							BSTR* pbstrCondition
							) 
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetFilterConditionForOrganizer)
	{	

		*pbstrCondition = pThis->m_filterHolder.GetFilterConditionForOrganizer(
						pThis->GetControllingUnknown(),
						bsrtFilterName, nConditionIndex ).AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::SetActiveFilter( BSTR  bstrFilterName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, SetActiveFilter)
	{	
		
		CString strNoneFilter;
		strNoneFilter.LoadString( IDS_FILTER_NONE );

		CString strNewFilterName = bstrFilterName;
		

		BOOL bApply;
		GetIsActiveFilterApply( &bApply );

		pThis->m_filterHolder.SetActiveFilter( bstrFilterName );


		if( strNoneFilter == strNewFilterName )
		{
			SetIsActiveFilterApply( FALSE );	
		}
		else
		{
			SetIsActiveFilterApply( TRUE );	
		}

	
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );	

		
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetActiveFilter( BSTR* pbstrFilterName )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetActiveFilter)
	{	
		*pbstrFilterName = pThis->m_filterHolder.GetActiveFilter().AllocSysString( );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::SaveWorkingFilter( )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, SaveWorkingFilter)
	{	
		if( !pThis->m_filterHolder.SaveWorkingFilter( ) )
			return S_FALSE;

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetWorkingFilterStatus( short* pnStatus )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetWorkingFilterStatus)
	{	
		*pnStatus = (short)pThis->m_filterHolder.GetWorkingFilterStatus();
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::SetIsActiveFilterApply( BOOL bApply )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, SetIsActiveFilterApply)
	{	
		pThis->m_filterHolder.SetIsActiveFilterApply( (bApply ? true : false ) );
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterHolder::GetIsActiveFilterApply( BOOL* pbApply )
{
	_try_nested(CDBaseDocument, DBaseFilterHolder, GetIsActiveFilterApply)
	{	
		*pbApply  = ( pThis->m_filterHolder.GetIsActiveFilterApply() ? TRUE : FALSE );
		return S_OK;
	}
	_catch_nested;
}







/////////////////////////////////////////////////////////////////////////////
//
//
//
//IDBaseFilterProvider
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterProvider::ReBuildFilterChooserList()
{
	_try_nested(CDBaseDocument, DBaseFilterProvider, ReBuildFilterChooserList );
	{
		pThis->ReBuildFilterChooserList( );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterProvider::OnFilterChooserChange( HWND hWnd )
{
	_try_nested(CDBaseDocument, DBaseFilterProvider, FilterChooserChange );
	{
		pThis->OnFilterChooserChange( hWnd );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterProvider::OnAddFilterChooser( HWND hWnd )
{
	_try_nested(CDBaseDocument, DBaseFilterProvider, OnAddFilterChooser );
	{
		pThis->OnAddFilterChooser( hWnd );

		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseFilterProvider::OnRemoveFilterChooser( HWND hWnd )
{
	_try_nested(CDBaseDocument, DBaseFilterProvider, OnRemoveFilterChooser );
	{
		pThis->OnRemoveFilterChooser( hWnd );

		return S_OK;
	}
	_catch_nested;
}

bool CDBaseDocument::OnFilterChooserChange( HWND hWnd )
{
	if( !::IsWindow( hWnd ))
		return false;

	IDBaseFilterHolderPtr ptrFH = GetControllingUnknown();
	if( ptrFH == NULL )	
		return false;

	LRESULT lItem = ::SendMessage( hWnd, CB_GETCURSEL, 0L, 0L );
	char szBuf[255];
	::ZeroMemory( &szBuf, sizeof(szBuf) );
	::SendMessage( hWnd, CB_GETLBTEXT , (WPARAM)lItem, (LPARAM)(LPCSTR)szBuf );


	//Need recover
	
	ptrFH->SetActiveFilter( _bstr_t(szBuf) );		

	ReBuildFilterChooserList();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnAddFilterChooser( HWND hWnd )
{
	ReBuildFilterChooserList();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnRemoveFilterChooser( HWND hWnd )
{
	ReBuildFilterChooserList();
	return true;

}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::ReBuildFilterChooserList()
{	
	if( !ResetFilterChoosers() )
		return false;


	if( !FillFilterChoosers() )
		return false;


	if( !SetActiveFilterToFilterChoosers() )
		return false;

	::RefreshFilterPropPage();

	ForceAppIdleUpdate();

	return true;
}



/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::ResetFilterChoosers()
{
	CArray<HWND, HWND> arComboHwnd;

	if( !GetFilterChooserHwndArray( arComboHwnd ) )
		return false;

	for( int i=0;i<arComboHwnd.GetSize();i++ )
		::SendMessage( arComboHwnd[i], CB_RESETCONTENT, 0L, 0L );

	arComboHwnd.RemoveAll();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::FillFilterChoosers()
{
	CArray<HWND, HWND> arComboHwnd;	


	if( !GetFilterChooserHwndArray( arComboHwnd ) )
		return false;

	
	IDBaseFilterHolderPtr ptrFH = GetControllingUnknown();
	if( ptrFH == NULL )
	{
		arComboHwnd.RemoveAll();
		return false;
	}

	for( int i=0;i<arComboHwnd.GetSize();i++ )
	{
		//Add NONE
		CString strNoneFilter;
		strNoneFilter.LoadString( IDS_FILTER_NONE );
		::SendMessage( arComboHwnd[i], CB_ADDSTRING , 0L, 
						(LPARAM) (LPCTSTR)strNoneFilter );
		int nCount = 0;

		//Need recover
		
		ptrFH->GetFiltersCount( &nCount );
		for( int j=0;j<nCount;j++ )
		{
			
			BSTR bstrFilterName = NULL;
			if( S_OK == ptrFH->GetFilter( j, &bstrFilterName) )
			{
				CString strFilterName = bstrFilterName;

				if( bstrFilterName )
					::SysFreeString( bstrFilterName );

				::SendMessage( arComboHwnd[i], CB_ADDSTRING , 0L, 
						(LPARAM) (LPCTSTR)strFilterName );
			}
		}
		
		
	}	

	arComboHwnd.RemoveAll();

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::SetActiveFilterToFilterChoosers()
{
	CArray<HWND, HWND> arComboHwnd;

	if( !GetFilterChooserHwndArray( arComboHwnd ) )
		return false;


	IDBaseFilterHolderPtr ptrFH = GetControllingUnknown();

	CString strActiveFilter;

	CString strNoneFilter;
	strNoneFilter.LoadString( IDS_FILTER_NONE );

	CString strCustomFilter;
	strCustomFilter.LoadString( IDS_FILTER_CUSTOM );


	//need recover
	
	BSTR bstrActiveFilter = NULL;
	if( ptrFH != NULL && S_OK == ptrFH->GetActiveFilter( &bstrActiveFilter ) )
	{
		strActiveFilter = bstrActiveFilter;

		if( bstrActiveFilter )
			::SysFreeString( bstrActiveFilter );			
	}
	else
	{
		strActiveFilter = strNoneFilter;

	}

	if( strActiveFilter.IsEmpty() )
		strActiveFilter = strNoneFilter;


	/*
	BOOL bActiveFilterApply = FALSE;
	if( ptrFH != NULL  )
		ptrFH->GetIsActiveFilterApply( &bActiveFilterApply );


	//new apply filter essure
	if( !bActiveFilterApply )//&& strActiveFilter != strCustomFilter )// not fixed [paul] 3.10.2001 cos BugTrack 1857
		strActiveFilter = strNoneFilter;
		*/


	for( int i=0;i<arComboHwnd.GetSize();i++ )
	{
		::SendMessage( arComboHwnd[i], CB_SELECTSTRING, 0L, 
				(LPARAM) (LPCTSTR)strActiveFilter );
	}
	

	arComboHwnd.RemoveAll();

	return true;

}


bool CDBaseDocument::GetFilterChooserHwndArray( CArray<HWND, HWND>& arComboHwnd )
{
	IUnknown	*punkUserInterface = ::_GetOtherComponent( ::GetAppUnknown(), IID_IUserInterface );
	if( punkUserInterface == NULL )
		return false;

	sptrIUserInterface sptrUI( punkUserInterface );
	punkUserInterface->Release();
	if( sptrUI == NULL )
		return false;

	long	lPos =0;
	sptrUI->GetFirstComboButton( &lPos );	

	while( lPos )
	{
		IUnknown *punkInfo = NULL;
		HWND hWnd = NULL;

		sptrUI->GetNextComboButton( &hWnd, &punkInfo, &lPos );
		
		if( punkInfo == NULL )
			continue;

		sptrIActionInfo sptrAI( punkInfo );
		punkInfo->Release();
		if( sptrAI == NULL )
			continue;

		CString strActionName;
		BSTR bstrActionName = NULL;
		BSTR bsrtTmp1, bsrtTmp2, bsrtTmp3;
		bsrtTmp1 = bsrtTmp2 = bsrtTmp3 = NULL;
		sptrAI->GetCommandInfo( &bstrActionName, &bsrtTmp1, &bsrtTmp2, &bsrtTmp3 );
		strActionName = bstrActionName;

		if( bstrActionName )
			::SysFreeString( bstrActionName );

		if( bsrtTmp1 )
			::SysFreeString( bsrtTmp1 );

		if( bsrtTmp2 )
			::SysFreeString( bsrtTmp2 );

		if( bsrtTmp3 )
			::SysFreeString( bsrtTmp3 );

		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		if( strActionName != GetActionNameByResID( IDS_CHOOSE_FILTER ) )
			continue;

		if( ::IsWindow( hWnd ) )
			arComboHwnd.Add( hWnd );		
	}

	return true;
}
