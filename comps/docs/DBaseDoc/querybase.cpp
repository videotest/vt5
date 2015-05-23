#include "stdafx.h"
#include "querybase.h"

#include "resource.h"
#include "FilterNameDlg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CBaseQuery
//
/////////////////////////////////////////////////////////////////////////////
CBaseQuery::CBaseQuery()
{
	
	m_bSimpleQuery = true;

	/*
	m_strBaseTable.Empty();		
	m_strNativeSQL.Empty();
	m_bUseNativeSQL = false;	

	m_bUseFilterRowInGrid	= FALSE;
	m_bUseSortRowInGrid	= FALSE;

	m_WorkingFilterStatus = wfsNotDefine;	

	m_strCustomFilterName.LoadString( IDS_FILTER_CUSTOM );
	m_strNoneFilterName.LoadString( IDS_FILTER_NONE );

	m_bFilterInOrganizerMode = false;

	m_bApplyFilter = false;
	*/

}

/////////////////////////////////////////////////////////////////////////////
CBaseQuery::~CBaseQuery()
{
	DeInit();
}

/////////////////////////////////////////////////////////////////////////////
void CBaseQuery::DeInit()
{
	for( int i=0;i<m_arrFields.GetSize();i++ )
		delete m_arrFields[i];

	m_arrFields.RemoveAll();

	DeleteSortFields();

	DeleteGridFields();
	
}

/////////////////////////////////////////////////////////////////////////////
void CBaseQuery::DeleteSortFields()
{
	for( int i=0;i<m_arrSortFields.GetSize();i++ )
		delete m_arrSortFields[i];

	m_arrSortFields.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CBaseQuery::DeleteGridFields()
{
	for( int i=0;i<m_arrGridFields.GetSize();i++ )
		delete m_arrGridFields[i];

	m_arrGridFields.RemoveAll();
}



/////////////////////////////////////////////////////////////////////////////
void CBaseQuery::Serialize( CStreamEx& ar )
{		

	long nVersion = 3;	

	if( ar.IsLoading() )
	{
		DeInit();

		ar >> nVersion;

		ar >> m_strBaseTable;				
		ar >> m_bSimpleQuery;
		ar >> m_strSQL;	

		//load grid fields
		if( nVersion >= 2 )
		{
			int nCount = 0;
			ar >> nCount;

			for( int i=0;i<nCount;i++ )
			{
				CGridField* p = new CGridField;
				p->Serialize( ar );	
				m_arrGridFields.Add( p );
			}
		}

		//load sort fields: ver. 3
		if( nVersion >= 3 )
		{
			int nCount = 0;
			ar >> nCount;

			for( int i=0;i<nCount;i++ )
			{
				CSortField* p = new CSortField;
				p->Serialize( ar );
				m_arrSortFields.Add( p );
			}
		}
	}
	else
	{
		ar << nVersion;

		ar << m_strBaseTable;				
		ar << m_bSimpleQuery;
		ar << m_strSQL;	

		//save grid fields
		int nCount = m_arrGridFields.GetSize();
		ar << nCount;
		for( int i=0;i<nCount;i++ )
			m_arrGridFields[i]->Serialize( ar );

		//save sort fields
		nCount = m_arrSortFields.GetSize();
		ar << nCount;
		for( int i=0;i<nCount;i++ )
			m_arrSortFields[i]->Serialize( ar );
	}
}

/////////////////////////////////////////////////////////////////////////////
CString	CBaseQuery::GetSQL( IUnknown* punkFilterHolder )
{
	CString strSimleSQL;

	strSimleSQL.Format( "select * from %s", m_strBaseTable );


	CString srtFilter;
	//filter
	IDBaseFilterHolderPtr _ptr( punkFilterHolder );
	if( _ptr )
	{

		IDBaseFilterHolder* pIFH = _ptr;
		
		BOOL bApply = FALSE;
		pIFH->GetIsActiveFilterApply( &bApply );

		
		if( bApply )
		{			
			BSTR bstr = 0;
			pIFH->GetActiveFilter( &bstr );	
			_bstr_t bstrActiveFilter = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;
			
			pIFH->GetFilterForSQL( bstrActiveFilter, &bstr );
				
			srtFilter = bstr;

			if( bstr )
				::SysFreeString( bstr );	bstr = 0;
		}
	}

	CString strSort;

	//sort
	if( m_arrSortFields.GetSize() > 0 )
	{		
		for( int i=0;i<m_arrSortFields.GetSize();i++ )
		{
			if( i != 0 )
				strSort += ",";

			CSortField* psf = m_arrSortFields[i];

			strSort += psf->m_strTable;
			strSort += ".";
			
			strSort += psf->m_strField;

			if( psf->m_bAcsending )
				strSort += " ASC";
			else
				strSort += " DESC";
		}
	}	

	CString strSQL;

	if( m_bSimpleQuery )	
		strSQL = strSimleSQL;	
	else
		strSQL = m_strSQL;

	strSQL.MakeLower();

	//filter
	if( !srtFilter.IsEmpty() )
	{		
		int nIndex = strSQL.Find( " where " );
		if( nIndex != -1 )
		{
			CString str;
			str  = " (";
			str += srtFilter;
			str += " ) and ";

			strSQL.Insert( nIndex + 6, str );
		}
		else
		{
			CString str;
			str  = " where (";
			str += srtFilter;
			str += " ) ";						

			int nIndexOrder = strSQL.Find( " order " );
			if( nIndexOrder != -1 )
			{
				strSQL.Insert( nIndex, str );
			}
			else
			{
				strSQL += str;
			}
		}
	}

	//sort
	if( !strSort.IsEmpty() )
	{
		int nIndex = strSQL.Find( " order by " );

		if( nIndex != -1 )
		{
			CString str;			
			str = strSort;
			str += ", ";

			strSQL.Insert( nIndex + 10, str );

		}
		else
		{
			CString str;
			str  = " order by ";
			str += strSort;
			str += "";

			strSQL += str;
		}

	}


	return strSQL;
}

