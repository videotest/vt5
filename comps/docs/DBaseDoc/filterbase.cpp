#include "stdafx.h"
#include "filterbase.h"
#include "DBaseDoc.h"
#include "constant.h"

#include "FilterNameDlg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CFilterHolder
//
/////////////////////////////////////////////////////////////////////////////
CFilterHolder::CFilterHolder()
{
	m_WorkingFilterStatus = wfsNotDefine;	

	m_strCustomFilterName.LoadString( IDS_FILTER_CUSTOM );
	m_strNoneFilterName.LoadString( IDS_FILTER_NONE );

	m_bFilterInOrganizerMode = false;
	m_bApplyFilter = false;

}

/////////////////////////////////////////////////////////////////////////////
CFilterHolder::~CFilterHolder()
{
	DeInit();
}

/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::DeInit()
{
	for( int i=0;i<m_arrFilters.GetSize();i++ )
		delete m_arrFilters[i];

	m_arrFilters.RemoveAll();

	m_WorkingFilterStatus = wfsNotDefine;	
	m_bFilterInOrganizerMode = false;
	m_bApplyFilter = false;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::Serialize( CArchive& ar )
{		

	long nVersion = 1;	

	if( ar.IsLoading() )
	{
		DeInit();

		ar >> nVersion;

		int nCount = 0;
		ar >> nCount;
		for(int i=0;i<nCount;i++ )
		{			
			CQueryFilter* pFilter = new CQueryFilter;
			pFilter->Serialize( ar );			
			m_arrFilters.Add( pFilter );
			
		}
	}
	else
	{
		ar << nVersion;

		ar << m_arrFilters.GetSize();
		for( int i=0;i<m_arrFilters.GetSize();i++ )
			m_arrFilters[i]->Serialize( ar );

	}
}

/////////////////////////////////////////////////////////////////////////////
int CFilterHolder::GetFilterCount()
{
	if( !GetFilterInOrganizerMode() && 
		(m_WorkingFilterStatus == wfsNew || m_WorkingFilterStatus == wfsSelectedChange) )
		return m_arrFilters.GetSize() + 1;

	return m_arrFilters.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
CQueryFilter* CFilterHolder::GetFilter( CString strFilterName )
{
	if( !GetFilterInOrganizerMode() && 
			(m_WorkingFilterStatus == wfsNew || 
			m_WorkingFilterStatus == wfsSelectedChange ||
			m_WorkingFilterStatus == wfsSelected
			) 
		&& strFilterName == m_strCustomFilterName )
		return &m_workFilter;

	for( int i=0;i<m_arrFilters.GetSize();i++ )
	{
		if( m_arrFilters[i]->m_strName == strFilterName )
			return m_arrFilters[i];
	}

	return NULL;

}

/////////////////////////////////////////////////////////////////////////////
CString CFilterHolder::GetFilterName( int nIndex )
{
	if( nIndex >= 0 && nIndex < m_arrFilters.GetSize() )
	{
		return m_arrFilters[nIndex]->m_strName;
	}
	else
	{
		if( !GetFilterInOrganizerMode() && 
			(m_WorkingFilterStatus == wfsNew || m_WorkingFilterStatus == wfsSelectedChange) 
			&& nIndex == m_arrFilters.GetSize() )
		{
			return m_strCustomFilterName;			
		}
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::AddFilter( CString strFilterName )
{
	if( !IsFilterNameUnic( strFilterName ) )
		return false;
	
	CQueryFilter* pFilter = new CQueryFilter;
	pFilter->m_strName = strFilterName;	

	m_arrFilters.Add( pFilter );
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::EditFilter( CString strFilterName, CString strNewFilterName )
{
	if( !IsFilterNameUnic(strNewFilterName) )
		return false;

	
	CQueryFilter* pFilter = GetFilter( strFilterName );
	
	if( pFilter == NULL )
		return false;

	if( pFilter == &m_workFilter )
		return false;


	pFilter->m_strName = strNewFilterName;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::DeleteFilter( CString strFilterName )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	
	if( pFilter == NULL )
		return false;

	if( pFilter == &m_workFilter )
		return false;

	delete pFilter;

	int nIndex = -1;
	for( int i=0;i<m_arrFilters.GetSize();i++ )
		if( pFilter == m_arrFilters[i] ) nIndex = i;

	if( nIndex  != -1 )
		m_arrFilters.RemoveAt( nIndex );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::IsFilterNameUnic( CString strFilterName )
{
	if( strFilterName == m_strCustomFilterName || strFilterName == m_strNoneFilterName )
		return false;

	for( int i=0;i<m_arrFilters.GetSize();i++ )
		if( m_arrFilters[i]->m_strName == strFilterName ) return false;
	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
int CFilterHolder::GetFilterConditionCount( CString strFilterName )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	if( pFilter == NULL )
		return false;

	return pFilter->m_arrCondition.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::GetFilterCondition( CString strFilterName, int nConditionIndex,
							CString& strTable, CString& strField,
							CString& strCondition, CString& strValue,
							CString& strValueTo, BOOL& bANDUnion,
							int& nLeftBracketCount, int& nRightBracketCount
							)
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	if( pFilter == NULL )
		return false;

	if( nConditionIndex < 0 || nConditionIndex >= pFilter->m_arrCondition.GetSize() )
		return false;

	CFilterCondition* pCond = pFilter->m_arrCondition[ nConditionIndex ];
	
	strTable			= pCond->m_strTable;
	strField			= pCond->m_strField;
	strCondition		= pCond->m_strCondition;
	strValue			= pCond->m_strValue;
	strValueTo			= pCond->m_strValueTo;
	bANDUnion			= pCond->m_bANDunion;
	nLeftBracketCount	= pCond->m_nLeftBracketCount;
	nRightBracketCount	= pCond->m_nRightBracketCount;

	return true;
}




/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::AddFilterCondition( CString strFilterName,
							CString strTable, CString strField,
							CString strCondition, CString strValue,
							CString strValueTo, BOOL bANDUnion,
							int nLeftBracketCount, int nRightBracketCount
							)
{
	CQueryFilter* pFilter = GetFilter( strFilterName );

	if( !GetFilterInOrganizerMode() )
	{
		if( pFilter == NULL )
		{
			ResetWorkingFilter( );	
			m_WorkingFilterStatus = wfsNew;
			m_strActiveFilter = m_strCustomFilterName;
		}
		else
		{	
			if( m_WorkingFilterStatus == wfsNotDefine || m_WorkingFilterStatus == wfsSelected )
			{
				ResetWorkingFilter( );	
				m_workFilter.CopyFrom( pFilter );
				m_strActiveFilter = m_strCustomFilterName;
				m_strFilterNameWorkingFilterCreateFrom = pFilter->m_strName;
			}

			m_WorkingFilterStatus = wfsSelectedChange;
		}

		pFilter = &m_workFilter;
	}
	else
	{
		if( pFilter == NULL )
			return false;

		//m_strSelectedFilter = strFilterName;
	}


	CFilterCondition* pCond = new CFilterCondition;
	
	pCond->m_strTable			= strTable;
	pCond->m_strField			= strField;
	pCond->m_strCondition		= strCondition;
	pCond->m_strValue			= strValue;
	pCond->m_strValueTo			= strValueTo;
	pCond->m_bANDunion			= bANDUnion;
	pCond->m_nLeftBracketCount	= nLeftBracketCount;
	pCond->m_nRightBracketCount	= nRightBracketCount;

	pFilter->m_arrCondition.Add( pCond );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::EditFilterCondition( CString strFilterName, int nConditionIndex,
							CString strTable, CString strField,
							CString strCondition, CString strValue,
							CString strValueTo, BOOL bANDUnion,
							int nLeftBracketCount, int nRightBracketCount
							)
{
	CQueryFilter* pFilter = GetFilter( strFilterName );

	if( !GetFilterInOrganizerMode() )
	{
		if( pFilter == NULL )
			return false;

		if( m_WorkingFilterStatus == wfsNotDefine  || m_WorkingFilterStatus == wfsSelected )
		{
			ResetWorkingFilter( );	
			m_workFilter.CopyFrom( pFilter );
			m_strActiveFilter = m_strCustomFilterName;
			m_strFilterNameWorkingFilterCreateFrom = pFilter->m_strName;
		}

		m_WorkingFilterStatus = wfsSelectedChange;

		pFilter = &m_workFilter;
	}
	else
	{
		if( pFilter == NULL )
			return false;

		//m_strSelectedFilter = strFilterName;
	}

	
	if( nConditionIndex < 0 || nConditionIndex >= pFilter->m_arrCondition.GetSize() )
		return false;


	CFilterCondition* pCond = pFilter->m_arrCondition[ nConditionIndex ];

	pCond->m_strTable			= strTable;
	pCond->m_strField			= strField;
	pCond->m_strCondition		= strCondition;
	pCond->m_strValue			= strValue;
	pCond->m_strValueTo			= strValueTo;
	pCond->m_bANDunion			= bANDUnion;
	pCond->m_nLeftBracketCount	= nLeftBracketCount;
	pCond->m_nRightBracketCount	= nRightBracketCount;	

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::DeleteFilterCondition( CString strFilterName, int nConditionIndex )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	if( pFilter == NULL )
		return false;

	if( !GetFilterInOrganizerMode() )
	{
		if( m_WorkingFilterStatus == wfsNotDefine  || m_WorkingFilterStatus == wfsSelected )
		{
			ResetWorkingFilter( );	
			m_workFilter.CopyFrom( pFilter );
			m_strActiveFilter = m_strCustomFilterName;
			m_strFilterNameWorkingFilterCreateFrom = pFilter->m_strName;
		}

		m_WorkingFilterStatus = wfsSelectedChange;
		
		pFilter = &m_workFilter;
	}
	else
	{
		//m_strSelectedFilter = strFilterName;
	}

	if( nConditionIndex < 0 || nConditionIndex >= pFilter->m_arrCondition.GetSize() )
		return false;


	CFilterCondition* pCond = pFilter->m_arrCondition[ nConditionIndex ];


	int nIndex = -1;
	for( int i=0;i<pFilter->m_arrCondition.GetSize();i++ )
		if( pCond == pFilter->m_arrCondition[i] ) nIndex = i;

	if( nIndex  != -1 )
		pFilter->m_arrCondition.RemoveAt( nIndex );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::AddConditionFromView( CString strTable, CString strField, CString strValue )
{
	/*
	if( (m_WorkingFilterStatus == wfsNew || m_WorkingFilterStatus == wfsSelectedChange) 
		&& nIndex == m_arrFilters.GetSize() )
		*/

	return true;
}



/////////////////////////////////////////////////////////////////////////////
CString CFilterHolder::GetActiveFilter()
{
	return m_strActiveFilter;	
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::SetActiveFilter( CString strActiveFilter )
{
	if( m_strActiveFilter == strActiveFilter )
		return true;

	if( m_strActiveFilter == m_strCustomFilterName )
	{
		if( m_WorkingFilterStatus == wfsNew || 
			m_WorkingFilterStatus == wfsSelectedChange
			)
		{
			/*
			if( AfxMessageBox( IDS_WARNING_FILTER_SAVE, MB_YESNO ) == IDYES )
				if( !SaveWorkingFilter() ) 
					return false;
					*/
		}
	}

	ResetWorkingFilter( );

	if( strActiveFilter == m_strNoneFilterName )
	{
		m_strActiveFilter = m_strNoneFilterName;		
		return true;
	}

	CQueryFilter* pFilter = GetFilter( strActiveFilter );
	
	if( pFilter == NULL ) //None choice
	{
		m_WorkingFilterStatus = wfsNew;		
		m_strActiveFilter = m_strCustomFilterName;
	}

	if( pFilter )
	{
		m_WorkingFilterStatus = wfsSelected;
		m_workFilter.CopyFrom( pFilter );
		m_strActiveFilter = strActiveFilter;
	}	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::SaveWorkingFilter( )
{
	CFilterNameDlg dlg;
	dlg.m_strFilterName = m_strFilterNameWorkingFilterCreateFrom;

	int nCount = GetFilterCount();
	for( int i=0;i<nCount;i++ )
	{
		if( m_strCustomFilterName != GetFilterName(i) && m_strNoneFilterName != GetFilterName(i) )
		dlg.AddFilter( GetFilterName(i) );
	}
	

	if( dlg.DoModal() != IDOK )
		return false;

	if( dlg.m_strFilterName.IsEmpty() )
		return false;

	bool bonly_space = true;
	for( i=0;i<dlg.m_strFilterName.GetLength();i++ )
	{
		if( dlg.m_strFilterName[i] != ' ' ) 
		{
			bonly_space = false;
			break;
		}
	}

	if( bonly_space ) 
		return false;

	if( dlg.m_strFilterName )

	if( dlg.m_strFilterName == m_strCustomFilterName || 
		dlg.m_strFilterName == m_strNoneFilterName )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		AfxMessageBox( IDS_FILTER_RESERVED_WORD_CAN_USE );
		return false;
	}

	CQueryFilter* pFilter = GetFilter( dlg.m_strFilterName );
	if( pFilter == NULL )
	{
		pFilter = new CQueryFilter;
		m_arrFilters.Add( pFilter );
	}

	pFilter->CopyFrom( &m_workFilter );
	pFilter->m_strName = dlg.m_strFilterName;

	ResetWorkingFilter();

	SetActiveFilter( pFilter->m_strName );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::ResetWorkingFilter( )
{
	m_WorkingFilterStatus = wfsNotDefine;
	m_workFilter.DeInit();
	
	m_strActiveFilter.Empty( );
	m_strFilterNameWorkingFilterCreateFrom.Empty( );
}

/////////////////////////////////////////////////////////////////////////////
WorkingFilterStatus CFilterHolder::GetWorkingFilterStatus()
{
	return m_WorkingFilterStatus;

}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::GetFilterInOrganizerMode()
{
	return m_bFilterInOrganizerMode;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::SetFilterInOrganizerMode( bool bFilterInOrganizerMode )
{
	m_bFilterInOrganizerMode = bFilterInOrganizerMode;
}


/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::SetWorkingFilterStatus( WorkingFilterStatus wfs )
{
	m_WorkingFilterStatus = wfs;
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterHolder::GetFilterForSQL( IUnknown* punkDoc, CString strFilterName )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	
	if( pFilter == NULL ) //None choice
		return "";

	return pFilter->GetFilterForSQL( punkDoc );


}

/////////////////////////////////////////////////////////////////////////////
CString CFilterHolder::GetFilterForOrganizer( IUnknown* punkDoc, CString strFilterName, CString strQueryName )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	
	if( pFilter == NULL ) //None choice
		return "";

	return pFilter->GetFilterForOrganizer( punkDoc, strQueryName );
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterHolder::GetFilterConditionForOrganizer( IUnknown* punkDoc, CString strFilterName, int nConditionIndex )
{
	CQueryFilter* pFilter = GetFilter( strFilterName );
	
	if( pFilter == NULL ) //None choice
		return "";

	if( nConditionIndex >=0 && nConditionIndex < pFilter->m_arrCondition.GetSize() )
		return pFilter->m_arrCondition[nConditionIndex]->GetFilterConditionForOrganizer( punkDoc, nConditionIndex != 0 );


	return "";

}


/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::GetIsActiveFilterApply()
{
	if( GetFilter( GetActiveFilter() ) == NULL )
		return false;

	if( !IsValidFilter( GetActiveFilter(), false ) )
		return false;

	return m_bApplyFilter;
}

/////////////////////////////////////////////////////////////////////////////
void CFilterHolder::SetIsActiveFilterApply( bool bApply )
{
	if( GetFilter( GetActiveFilter() ) == NULL )
	{
		m_bApplyFilter = false;
		return;
	}

	
	if( bApply && !IsValidFilter( GetActiveFilter(), false ) )
	{
		m_bApplyFilter = false;
		return;
	}

	m_bApplyFilter = bApply;
}


/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::IsValidFilter( CString strFilter, bool bReportError )
{
	CQueryFilter* pFilter = GetFilter( strFilter );
	if( pFilter == NULL )
	{		
		return false;
	}

	if( pFilter->m_arrCondition.GetSize() < 1 )
	{
		//AfxMessageBox( IDS_WARNING_NO_CONDITION );
		return false;
	}


	int nLeftBracket, nRightBracket;
	nLeftBracket = nRightBracket = 0;
	for( int i=0;i<pFilter->m_arrCondition.GetSize();i++ )
	{
		CFilterCondition* pCond = pFilter->m_arrCondition[i];
		nLeftBracket	+= pCond->m_nLeftBracketCount;
		nRightBracket	+= pCond->m_nRightBracketCount;
	}

	if( nLeftBracket != nRightBracket )
		return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::StoreToData( IUnknown* punkData )
{
	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;

	CString strSection = SECTION_FILTERS;

	VERIFY( S_OK == ptrND->DeleteEntry( _bstr_t( (LPCSTR)strSection )) );	

	for( int i=0;i<m_arrFilters.GetSize();i++ )
	{
		CString strFilterName = m_arrFilters[i]->m_strName;
		if( strFilterName.IsEmpty() )
			continue;

		CString strCurSection = SECTION_FILTERS;
		strCurSection += "\\";
		strCurSection += strFilterName;

		m_arrFilters[i]->StoreToData( punkData, strCurSection );
	}
	
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFilterHolder::LoadFromData( IUnknown* punkData )
{
	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;


	for( int i=0;i<m_arrFilters.GetSize();i++ )
		delete m_arrFilters[i];

	m_arrFilters.RemoveAll();


	BSTR bstr = 0;
	if( S_OK != ptrND->GetCurrentSection( &bstr ) )
		return false;

	_bstr_t bstrCutSect = bstr;	

	if( bstr )
		::SysFreeString( bstr );	bstr = 0;


	CString strSection = SECTION_FILTERS;

	VERIFY( S_OK == ptrND->SetupSection( _bstr_t( (LPCSTR)strSection ) ) );
	long lEntryCount = 0;

	ptrND->GetEntriesCount( &lEntryCount );
	for( i=0;i<lEntryCount;i++ )
	{
		BSTR bstr = 0;
		
		VERIFY( S_OK == ptrND->SetupSection( _bstr_t( (LPCSTR)strSection ) ) );
		if( S_OK != ptrND->GetEntryName( i, &bstr ) )
			continue;

		CString strEntryName = strSection  + CString( "\\" ) + CString( bstr );

		if( bstr )
			::SysFreeString( bstr );	bstr = 0;
		
		CQueryFilter* p = new CQueryFilter;

		if( p->LoadFromData( punkData, strEntryName ) )
			m_arrFilters.Add( p );
		else
			delete p;	p = 0;
			
	}

	VERIFY( S_OK == ptrND->SetupSection( bstrCutSect ) );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
// CQueryFilter
//
/////////////////////////////////////////////////////////////////////////////
CQueryFilter::CQueryFilter()
{
	m_strName.IsEmpty();	
	m_bModifiedFlag = false;
}

/////////////////////////////////////////////////////////////////////////////
CQueryFilter::~CQueryFilter()
{
	DeInit();
}		 

/////////////////////////////////////////////////////////////////////////////
void CQueryFilter::DeInit()
{
	//Deinit
	for( int i=0;i<m_arrCondition.GetSize();i++ )
		delete m_arrCondition[i];

	m_arrCondition.RemoveAll();

	m_strName.Empty();

}

/////////////////////////////////////////////////////////////////////////////
void CQueryFilter::Serialize( CArchive& ar )
{
	long nVersion = 1;
	if( ar.IsLoading() )
	{
		DeInit();

		int nCount;

		ar >> nVersion;
		ar >> m_strName;

		ar >> nCount;
		for( int i=0;i<nCount;i++ )
		{
			CFilterCondition* pCondition = new CFilterCondition;
			pCondition->Serialize( ar );
			m_arrCondition.Add( pCondition );
		}
	}
	else
	{
		ar << nVersion;

		ar << m_strName;

		ar << m_arrCondition.GetSize();
		for( int i=0;i<m_arrCondition.GetSize();i++ )
			m_arrCondition[i]->Serialize( ar );
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CQueryFilter::StoreToData( IUnknown* punkData, CString strSection )
{
	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;

	CString strSectionName = strSection;
	strSectionName += "\\Name";

	_variant_t var = _bstr_t( (LPCSTR)m_strName );		
	ptrND->SetValue( _bstr_t( (LPCSTR)strSectionName ), var );

	for( int i=0;i<m_arrCondition.GetSize();i++ )
	{
		CString strCurSect = strSection;
		strCurSect += "\\conditions\\";

		CString strTemp;
		strTemp.Format( "Cond%d", i );
		strCurSect += strTemp;

		m_arrCondition[i]->StoreToData( punkData, strCurSect );
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CQueryFilter::LoadFromData( IUnknown* punkData, CString strSection )
{
	for( int i=0;i<m_arrCondition.GetSize();i++ )
		delete m_arrCondition[i];

	m_arrCondition.RemoveAll();

	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;

	CString strCondSection = strSection;
	strCondSection += "\\conditions";

	VERIFY( S_OK == ptrND->SetupSection( _bstr_t( (LPCSTR)strCondSection ) ) );
	long lEntryCount = 0;

	ptrND->GetEntriesCount( &lEntryCount );
	for( i=0;i<lEntryCount;i++ )
	{
		BSTR bstr = 0;
		
		{
			_variant_t var;
			CString strSectionName = strSection;
			strSectionName += "\\Name";

			if( S_OK != ptrND->GetValue( _bstr_t( (LPCSTR)strSectionName ), &var ) )
				return false;

			if( var.vt != VT_BSTR )
				return false;

			m_strName = var.bstrVal; 
		}
		
		VERIFY( S_OK == ptrND->SetupSection( _bstr_t( (LPCSTR)strCondSection ) ) );
		if( S_OK != ptrND->GetEntryName( i, &bstr ) )
			continue;

		

		CString strEntryName = strCondSection  + CString( "\\" ) + CString( bstr );

		if( bstr )
			::SysFreeString( bstr );	bstr = 0;
		
		CFilterCondition* p = new CFilterCondition;

		if( p->LoadFromData( punkData, strEntryName ) )
			m_arrCondition.Add( p );
		else
			delete p;	p = 0;			
	}


	return true;
}


/////////////////////////////////////////////////////////////////////////////
CString CQueryFilter::GetFilterForSQL( IUnknown* punkDoc  )
{
	CString strFilter;
	for( int i=0;i<m_arrCondition.GetSize();i++ )
	{
		strFilter += m_arrCondition[i]->GetFilterConditionForSQL( punkDoc, i != 0 );
	}
	return strFilter;
}

/////////////////////////////////////////////////////////////////////////////
CString CQueryFilter::GetFilterForOrganizer( IUnknown* punkDoc, CString strQueryName )
{
	CString strSelectFrom, strRecords, strWhere;	

	strSelectFrom.LoadString( IDS_ORGANIZER_SELECT_FROM );
	strRecords.LoadString( IDS_ORGANIZER_RECORDS );
	strWhere.LoadString( IDS_ORGANIZER_WHERE );

 

	CString strFilter;
	strFilter.Format( "%s %s %s %s",		strSelectFrom, strQueryName, 
											strRecords, strWhere
											);
	if( m_arrCondition.GetSize() < 1 )
	{
		CString strAll;
		strAll.LoadString( IDS_FILTER_SHOW_ALL );

		strFilter.Format( "%s %s %s %s",		strSelectFrom, strQueryName, 
											strAll, strRecords );
	}

	return strFilter;
}


/////////////////////////////////////////////////////////////////////////////
bool CQueryFilter::CopyFrom( CQueryFilter* pSourceFilter )
{
	DeInit();

	if( pSourceFilter == NULL )
		return false;



	for( int i=0;i<pSourceFilter->m_arrCondition.GetSize();i++)
	{
		CFilterCondition* pTargetCondition = new CFilterCondition;
		CFilterCondition* pSourceCondition = pSourceFilter->m_arrCondition[i];

		pTargetCondition->m_strTable			= pSourceCondition->m_strTable;
		pTargetCondition->m_strField			= pSourceCondition->m_strField;
		pTargetCondition->m_strCondition		= pSourceCondition->m_strCondition;	

		pTargetCondition->m_strValue			= pSourceCondition->m_strValue;		
		pTargetCondition->m_strValueTo			= pSourceCondition->m_strValueTo; 			

		pTargetCondition->m_bANDunion			= pSourceCondition->m_bANDunion;

		pTargetCondition->m_nLeftBracketCount	= pSourceCondition->m_nLeftBracketCount;
		pTargetCondition->m_nRightBracketCount	= pSourceCondition->m_nRightBracketCount;

		m_arrCondition.Add( pTargetCondition );
	}	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// CFilterCondition
//
/////////////////////////////////////////////////////////////////////////////
CFilterCondition::CFilterCondition()
{
	m_strTable.Empty();
	m_strField.Empty();

	m_strCondition.Empty();

	m_strValue.Empty();	
	m_strValueTo.Empty();	

	m_bANDunion = TRUE;

	m_nLeftBracketCount		= 0;
	m_nRightBracketCount	= 0;


}

/////////////////////////////////////////////////////////////////////////////
CFilterCondition::~CFilterCondition()
{

}
/////////////////////////////////////////////////////////////////////////////
void CFilterCondition::Serialize( CArchive& ar )
{
	long nVersion = 1;

	if( ar.IsLoading() )
	{
		ar >> nVersion;		

		ar >> m_strTable;
		ar >> m_strField;				

		ar >> m_strCondition;
		
		ar >> m_strValue;					

		ar >> m_strValueTo;			
		
		ar >> m_bANDunion;

		ar >> m_nLeftBracketCount;
		ar >> m_nRightBracketCount;

	}
	else
	{
		ar << nVersion;		

		ar << m_strTable;
		ar << m_strField;				

		ar << m_strCondition;

		ar << m_strValue;
		ar << m_strValueTo;
		
		
		ar << m_bANDunion;

		ar << m_nLeftBracketCount;
		ar << m_nRightBracketCount;
	}

}


bool CFilterCondition::StoreToData( IUnknown* punkData, CString strSection )
{
	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;

	_bstr_t	bstrSect	= strSection + "\\Table";
	CString strValue	= m_strTable;

	_variant_t varValue = _bstr_t( (LPCSTR)strValue );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\Field";
	strValue	= m_strField;

	varValue	= _bstr_t( (LPCSTR)strValue );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\Condition";
	strValue	= m_strCondition;

	varValue	= _bstr_t( (LPCSTR)strValue );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\Value";
	strValue	= m_strValue;

	varValue	= _bstr_t( (LPCSTR)strValue );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\ValueTo";
	strValue	= m_strValueTo;

	varValue	= _bstr_t( (LPCSTR)strValue );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\ANDunion";
	varValue	= long( m_bANDunion );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\LeftBracketCount";
	varValue	= long( m_nLeftBracketCount );		
	ptrND->SetValue( bstrSect, varValue );


	bstrSect	= strSection + "\\RightBracketCount";
	varValue	= long( m_nRightBracketCount );		
	ptrND->SetValue( bstrSect, varValue );


	return true;
}

bool CFilterCondition::LoadFromData( IUnknown* punkData, CString strSection )
{
	INamedDataPtr ptrND( punkData );
	if( ptrND == 0 )
		return false;

	_bstr_t	bstrSect	= strSection + "\\Table";	
	_variant_t varValue;		
	
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_BSTR )	return false;

	m_strTable = varValue.bstrVal;
	varValue.Clear();

	bstrSect	= strSection + "\\Field";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_BSTR )	return false;

	m_strField = varValue.bstrVal;
	varValue.Clear();


	bstrSect	= strSection + "\\Condition";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_BSTR )	return false;

	m_strCondition = varValue.bstrVal;
	varValue.Clear();
	

	bstrSect	= strSection + "\\Value";	;
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_BSTR )	return false;

	m_strValue = varValue.bstrVal;
	varValue.Clear();


	bstrSect	= strSection + "\\ValueTo";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_BSTR )	return false;

	m_strValueTo = varValue.bstrVal;
	varValue.Clear();


	bstrSect	= strSection + "\\ANDunion";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_I4 )	return false;

	m_bANDunion = (long)varValue;
	varValue.Clear();


	bstrSect	= strSection + "\\LeftBracketCount";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_I4 )	return false;

	m_nLeftBracketCount = (long)varValue;
	varValue.Clear();


	bstrSect	= strSection + "\\RightBracketCount";
	if( S_OK != ptrND->GetValue( bstrSect, &varValue ) )	return false;		

	if( varValue.vt != VT_I4 )	return false;

	m_nRightBracketCount = (long)varValue;
	varValue.Clear();


	return true;
}


/////////////////////////////////////////////////////////////////////////////
CString CFilterCondition::GetFilterConditionForOrganizer( IUnknown* punkDoc, bool bInsUnion, bool bInsBracket  )
{
	return GetConditionAsString( punkDoc, bInsUnion, false, bInsBracket );
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterCondition::GetFilterConditionForSQL( IUnknown* punkDoc, bool bInsUnion )
{
	return GetConditionAsString( punkDoc, bInsUnion, true );
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterCondition::GetConditionAsString( IUnknown* punkDoc, bool bInsUnion, bool bForSQL, bool bInsBracket  )
{

	ASSERT( punkDoc );

	CString strFilterAsString;

	CString strLeftBracket, strRightBracket;
	CString strValueOf;
	strValueOf.LoadString( IDS_ORGANIZER_VALUEOF );


	for( int i=0;i<m_nLeftBracketCount;i++ )
		strLeftBracket += "(";

	for( i=0;i<m_nRightBracketCount;i++ )
		strRightBracket += ")";


	bool bFROMCondition = false;
	CString strCondition = m_strCondition;
	strCondition.MakeUpper();
	if( strCondition == "FROM" )
		bFROMCondition = true;

	CString strValue, strValueTo;

	strValue = m_strValue;
	strValueTo = m_strValueTo;
	bool bEmpty = false;
	
	{
		sptrIDBaseDocument spDBaseDoc( punkDoc );
		if( spDBaseDoc )
		{
			FieldType ft = ::GetFieldType( spDBaseDoc, m_strTable, m_strField );
			if( ft == ftString )
			{
				bEmpty = m_strValue.IsEmpty();
				strValue.Format( "'%s'", m_strValue );
				strValueTo.Format( "'%s'", m_strValueTo );

				if( strCondition == "LIKE" )
				{
					strValue = "'%";
					strValue += m_strValue;
					strValue += "%'";
				}				
			}
			else if( ft == ftDigit )
			{
				if( bForSQL )
				{
					int npos = strValue.Find( ',' );
					if( npos != -1 ) strValue.SetAt( npos, '.' );

					npos = strValueTo.Find( ',' );
					if( npos != -1 ) strValueTo.SetAt( npos, '.' );
				}
			}
			else if( ft == ftDateTime )
			{
				COleDateTime dtFrom;				
				COleDateTime dtTo;

				try{
				dtFrom = _variant_t( m_strValue );
				dtTo = _variant_t( m_strValueTo );
				}
				catch(...){}

				if( bForSQL )
				{					  
					strValue.Format( "#%d/%d/%d#", dtFrom.GetMonth(), dtFrom.GetDay(), dtFrom.GetYear() );
					strValueTo.Format( "%f", dtTo.m_dt );
				}
				else
				{
					strValue	= dtFrom.Format( VAR_DATEVALUEONLY );
					strValueTo	= dtTo.Format( VAR_DATEVALUEONLY );
				}
			}
		}
	}



	CString strUnion;
	if( bInsUnion )
	{
		if( !bForSQL )
		{
			if( m_bANDunion )
				strUnion.LoadString( IDS_ORGANIZER_AND );
			else
				strUnion.LoadString( IDS_ORGANIZER_OR );
		}
		else
		{
			if( m_bANDunion )
				strUnion = "AND";
			else
				strUnion = "OR  ";
		}

	}

	if( !bForSQL && !bInsBracket )
	{
		strLeftBracket = strRightBracket = "";
	}

	if( bFROMCondition )
	{
		if( bForSQL )
		{
			strFilterAsString.Format( "%s %s ( %s >= %s AND %s <= %s ) %s", 
					(LPCTSTR)strUnion,
					(LPCTSTR)strLeftBracket,					
					(LPCTSTR)GetAsTableField(),
					(LPCTSTR)strValue, 
					(LPCTSTR)GetAsTableField(),
					(LPCTSTR)strValueTo,
					(LPCTSTR)strRightBracket
					);
		}
		else
		{
			strFilterAsString.Format( "%s %s %s %s %s %s TO %s %s", 
					(LPCTSTR)strUnion,
					(LPCTSTR)strLeftBracket,
					(LPCTSTR)strValueOf,
					(LPCTSTR)GetAsCaption( punkDoc),//m_strField,
					(LPCTSTR)m_strCondition, 
					(LPCTSTR)strValue, (LPCTSTR)strValueTo,
					(LPCTSTR)strRightBracket
					);
		}
	}
	else
	{
		if( bForSQL )
		{
			if (m_strCondition == "=" && bEmpty)
				strFilterAsString.Format( "%s %s %s is null %s", 
						(LPCTSTR)strUnion,
						(LPCTSTR)strLeftBracket,					
						(LPCTSTR)GetAsTableField(), 
						(LPCTSTR)strRightBracket
						);
			else
				strFilterAsString.Format( "%s %s %s %s %s %s", 
						(LPCTSTR)strUnion,
						(LPCTSTR)strLeftBracket,					
						(LPCTSTR)GetAsTableField(), (LPCTSTR)m_strCondition, 
						(LPCTSTR)strValue,
						(LPCTSTR)strRightBracket
						);
		}
		else
		{
			strFilterAsString.Format( "%s %s %s %s %s %s %s", 
					(LPCTSTR)strUnion,
					(LPCTSTR)strLeftBracket,
					(LPCTSTR)strValueOf,
					(LPCTSTR)GetAsCaption( punkDoc),//m_strField,
					(LPCTSTR)m_strCondition, 
					(LPCTSTR)strValue,
					(LPCTSTR)strRightBracket
					);
		}
	}
	

	return strFilterAsString;
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterCondition::GetAsTableField()
{
	if( !m_strTable.IsEmpty() )
	{
		CString strTemp;
		strTemp.Format( "%s.%s", (LPCTSTR)m_strTable, (LPCTSTR)m_strField );
		return strTemp;
	}
	else
	{
		ASSERT( false );
	}
	
	return m_strField;
	
}

/////////////////////////////////////////////////////////////////////////////
CString CFilterCondition::GetAsCaption( IUnknown* punkDoc )
{
	CString strCaption = ::GetFieldCaption( punkDoc, m_strTable, m_strField );
	if( strCaption.IsEmpty() )
	{
		ASSERT( false );
		return m_strField;
	}

	return strCaption;
}