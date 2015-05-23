#include "stdafx.h"
#include "classifycontainerimpl.h"
#include "misc_utils.h"

/*************************************************************************************/
HRESULT CClassifyContainerImpl::SetName( /*[in] */BSTR bstrName )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !bstrName )
		return S_FALSE;
	
	m_strName = bstrName;
	return S_OK;
}

HRESULT CClassifyContainerImpl::GetName(/*[out] */BSTR *pbstrName )
{
	if( !pbstrName )
		return S_FALSE;
	
	*pbstrName = m_strName.AllocSysString();
	return S_OK;
}
/*************************************************************************************/
HRESULT CClassifyContainerImpl::SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !plClasses || !lSz )
		return S_FALSE;

	if( m_plClasses )
		delete []m_plClasses;
	
	m_lSz2 = lSz;
	m_plClasses = new long[lSz];

	CopyMemory( m_plClasses, plClasses, sizeof( long ) * lSz );
	return S_OK;
}

HRESULT CClassifyContainerImpl::GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz )
{
	if( !pplClasses && !plSz )
		return S_FALSE;

	if( !pplClasses && plSz )
	{
		*plSz = m_lSz2;
		return S_OK;
	}

	::CopyMemory( *pplClasses, m_plClasses, sizeof( long ) * m_lSz2 );
	return S_OK;
}

HRESULT CClassifyContainerImpl::RemoveClasses()
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( m_plClasses )
		delete []m_plClasses;
	m_plClasses = 0;
	m_lSz2 = 0;
	m_mapClassParams.clear();
	return S_OK;
}

HRESULT CClassifyContainerImpl::GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams )
{
	long lPos = m_mapClassParams.find( lClass );
	if( lPos > 0 )
	{
		*ppParams = m_mapClassParams.get( lPos );
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CClassifyContainerImpl::SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	m_mapClassParams.set( pfParams, lClass );
	return S_OK;
}
/*************************************************************************************/
HRESULT CClassifyContainerImpl::SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !plKeys || !lSz )
		return S_FALSE;

	if( m_plKeys )
		delete []m_plKeys;
	
	m_lSz = lSz;
	m_plKeys = new long[lSz];

	::CopyMemory( m_plKeys, plKeys, sizeof( long ) * lSz );
	return S_OK;
}

HRESULT CClassifyContainerImpl::GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz )
{
	if( !pplKeys && !plSz )
		return S_FALSE;

	if( !pplKeys && plSz )
	{
		*plSz = m_lSz;
		return S_OK;
	}

	::CopyMemory( *pplKeys, m_plKeys, sizeof( long ) * m_lSz );
	return S_OK;
}

HRESULT CClassifyContainerImpl::RemoveKeys()
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( m_plKeys )
		delete []m_plKeys;
	m_plKeys = 0;
	m_lSz = 0;

	if( m_mapKeyParams )
		delete []m_mapKeyParams;
	m_mapKeyParams = 0;
	return S_OK;
}

HRESULT CClassifyContainerImpl::GetKeyParams( /*[in] */long lClass,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz )
{
	if( !ppfParams && !plSz )
		return S_FALSE;

	long lPos = m_mapKeyParams[lClass].find( lKey );
	if( lPos > 0 )
	{
		XKeyItem item = m_mapKeyParams[lClass].get( lPos );

		if( !ppfParams && plSz )
		{
			*plSz = item.GetSize();
			return S_OK;
		}
		::CopyMemory( *ppfParams, item.GetData(), sizeof( double ) * ( *plSz ) );
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CClassifyContainerImpl::SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long lSz )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !pfParams )
		return S_FALSE;

	XKeyItem item( lSz );

	::CopyMemory( item.GetData(), pfParams, sizeof( double ) * lSz );
	m_mapKeyParams[lClass].set( item, lKey );

	return S_OK;
}
/*************************************************************************************/
HRESULT CClassifyContainerImpl::GetFlags( /*[out]*/ DWORD *pdwFlag )
{
	if( !pdwFlag )
		return S_FALSE;

	*pdwFlag = m_dwFlag;

	return S_OK;
}

HRESULT CClassifyContainerImpl::SetFlags( /*[in]*/ DWORD dwFlag )
{
	m_dwFlag = dwFlag;
	return S_OK;
}
/*************************************************************************************/
HRESULT CClassifyContainerImpl::Load( /*[in]*/ BSTR bstrFile )
{
	CString strFileName( bstrFile );

	CString strPROGID = _get_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID );
	
	if( strPROGID.IsEmpty() )
		return S_FALSE;

	m_strPROGID = strPROGID;

	m_strName = _get_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_NAME );

	if( m_strName.IsEmpty() )
		m_strName = strPROGID;

	CString strClassCount = _get_value_string( strFileName, ID_SECT_CLASSES_INFO, ID_ENTR_CLASS_COUNT );

	if( strClassCount.IsEmpty() )
		return S_FALSE;

	m_lSz2 = atol( strClassCount );
	
	if( m_plClasses )
		delete []m_plClasses;
	m_plClasses = new long[m_lSz2];
	
	m_mapKeyParams = new XKeyMap[m_lSz2];
	//////////////////////////////////////////////////////////////////////////////////////////
	for( int i = 0; i < m_lSz2; i++ )
	{
		CString strEntry;
		m_plClasses[i] = i;
		strEntry.Format( "%d", i );

		CString strInfo = _get_value_string( strFileName, ID_SECT_CLASSES_INFO, strEntry );

		if( strInfo.IsEmpty() )
			continue;
		
		int nPos = strInfo.Find( ')' );
		nPos = strInfo.Find( ',', nPos );

		CString strColor = strInfo.Left( nPos );
		CString strName  = strInfo.Right( strInfo.GetLength() - nPos - 1 );

		int r = 0, g = 0, b = 0;
		::sscanf( strColor, "(%d, %d, %d)", &r, &g, &b );

		CLASSINFO lpClasses;

		strcpy( lpClasses.szName, strName ); 
		lpClasses.clColor = RGB( r, g, b );

		SetClassParams( i, lpClasses );
	}
	//////////////////////////////////////////////////////////////////////////////////////////
	CString strKeyEnum = _get_value_string( strFileName, ID_SECT_KEY_INFO, ID_ENTR_KEYS_ENUM );

	CString *Strs = 0;
	long lSz = 0;
	split( strKeyEnum, "," ,&Strs, &lSz );

	if( m_plKeys )
		delete []m_plKeys;

	m_plKeys = new long[lSz];
	m_lSz = lSz;

	for( int i = 0; i < lSz; i++ )
	{
		Strs[i] = Strs[i].TrimLeft().TrimRight();
		m_plKeys[i] = atoi( Strs[i] );

		int _nCount = m_lSz2;
		for( int z = 0; z < _nCount; z++ )
		{
			CString str;
			str.Format( "%d", z );
			CString strParams = _get_value_string( strFileName, ID_SECT_KEY_DATA + Strs[i], str );

			if( strParams.IsEmpty() )
				continue;
				
			CString *_pStrs = 0;
			long _lSz = 0;
			split( strParams, "," ,&_pStrs, &_lSz );

			double *pfParams = new double[_lSz];

			for( int k = 0; k < _lSz; k++ )
				pfParams[k] = atof( _pStrs[k] );

			SetKeyParams( z, m_plKeys[i], pfParams, _lSz );

			delete []pfParams;

			delete []_pStrs;
		}
	}

	delete []Strs;
	
	return S_OK;
}

HRESULT CClassifyContainerImpl::Store( /*[in]*/ BSTR bstrFile )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	CString strFileName( bstrFile );

	_set_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID, m_strPROGID );
	_set_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_NAME, m_strName );

	CString strClassCount;
	strClassCount.Format( "%ld", m_lSz2 );
	_set_value_string( strFileName, ID_SECT_CLASSES_INFO, ID_ENTR_CLASS_COUNT, strClassCount );

	for( int i = 0; i < m_lSz2; i++ )
	{
		CString strEntry;
		m_plClasses[i] = i;
		strEntry.Format( "%d", i );

		CLASSINFO lpClasses;

		GetClassParams( i, &lpClasses );
		
		CString strColor;
		strColor.Format( "(%d, %d, %d)", GetRValue( lpClasses.clColor ), GetGValue( lpClasses.clColor ), GetBValue( lpClasses.clColor ) );

		CString strName = lpClasses.szName;

		_set_value_string( strFileName, ID_SECT_CLASSES_INFO, strEntry, strColor + "," + strName );
	}

	CString strKeyEnum;

	for( int i = 0; i < m_lSz; i++ )
	{
		CString str;
		str.Format( "%ld", m_plKeys[i] );

		strKeyEnum += str;

		if( i != m_lSz - 1 )
			strKeyEnum += ",";
	}

	_set_value_string( strFileName, ID_SECT_KEY_INFO, ID_ENTR_KEYS_ENUM, strKeyEnum );

	for( int z = 0; z < m_lSz; z++ )
	{ 
		CString str;
		str.Format( "%d", z );
			
		double *_pfKeyParams = 0;
		long _lSzKey = 0;

		CString Strs;

		Strs.Format( "%lf", m_plKeys[i] );
		GetKeyParams( z, m_plKeys[i], 0, &_lSzKey );

		_pfKeyParams = new double[_lSzKey];

		GetKeyParams( z, m_plKeys[i], &_pfKeyParams, &_lSzKey );

		CString strParams;

		for( int q = 0; q < _lSzKey; q++ )
		{
			CString str;
			str.Format( "%lf", _pfKeyParams[q] );

			strParams += str;

			if( q != _lSzKey - 1 )
				strParams += ",";
		}

		delete []_pfKeyParams;

		_set_value_string( strFileName, ID_SECT_KEY_DATA + Strs, str, strParams );
	}
	return S_OK;
}
HRESULT CClassifyContainerImpl::Start()
{
	m_arObjectList.clear();
	return S_OK;
}
HRESULT CClassifyContainerImpl::Process( /*[in]*/ IUnknown *punkObject )
{
	XObjectItem item;
	item.sptrObject = punkObject;

	ICalcObjectPtr sptrObject = punkObject;

	if( sptrObject == 0 )
		return S_FALSE;

	long lClassCount = 0;
	GetClasses( 0, &lClassCount );

	if( !lClassCount )
		return S_FALSE;

	long *lClassArr = new long[lClassCount];

	if( !lClassArr )
		return S_FALSE;

	GetClasses( &lClassArr, &lClassCount );

	int *pQuels = new int[lClassCount];

	if( !pQuels )
		return S_FALSE;

	::ZeroMemory( pQuels, sizeof( int ) * lClassCount );
	
	for( int cl = 0; cl < lClassCount; cl++ )
	{
		long lKeyCount = 0;
		GetKeys( 0, &lKeyCount );

		if( !lKeyCount )
			continue;

		long *lKeysArr = new long[lKeyCount];

		if( !lKeysArr )
			continue;

		GetKeys( &lKeysArr, &lKeyCount );

		for( int k = 0; k < lKeyCount; k++ )
		{
			long lSz = 0;
			GetKeyParams( lClassArr[cl], lKeysArr[k], 0, &lSz );

			if( !lSz )
				continue;

			double *pParams = new double[lSz];

			GetKeyParams( lClassArr[cl], lKeysArr[k], &pParams, &lSz );

			///// Checking.... ///////////////////////
			double fVal = 0;
			sptrObject->GetValue( lKeysArr[k], &fVal );

			if( fVal >= pParams[0] && fVal <= pParams[1] )
				pQuels[cl]++;
			////////////////////////////

			delete []pParams;
		}
		delete []lKeysArr;
	}

	int nMax = 0, nMaxID = -1;
	for( int i = 0; i < lClassCount; i++ )
	{
		if( pQuels[i] > nMax )
		{
			nMax = pQuels[i];
			nMaxID = i;
		}
	}

	long lKeyCount = 0;
	GetKeys( 0, &lKeyCount );

	if( nMax != lKeyCount )
		item.lClass = -1;
	else
	{
		if( nMaxID != -1 )
			item.lClass = lClassArr[nMaxID];
		else
			item.lClass = -1;
	}

	delete []pQuels;
	delete []lClassArr;

	m_arObjectList.add_tail( item );

	return S_OK;
}
HRESULT CClassifyContainerImpl::GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass )
{
	if( !punkObject || !plClass )
		return S_FALSE;

	for( long lPos = m_arObjectList.head(); lPos; lPos = m_arObjectList.next( lPos ) )
	{
		XObjectItem item = m_arObjectList.get( lPos );
		if( ::GetKey( item.sptrObject ) == ::GetKey( punkObject ) )
		{
			*plClass = item.lClass;
			return S_OK;
		}
	}
	return S_FALSE;
}

/*************************************************************************************/
CString CClassifyContainerImpl::_get_value_string( CString strFileName, CString strSection, CString strKey )
{
	TCHAR szT[4096];
	DWORD dw = ::GetPrivateProfileString( strSection, strKey,
		"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
	if( dw > 0 )
		return szT;
	return "";
}

void CClassifyContainerImpl::_set_value_string( CString strFileName, CString strSection, CString strKey, CString strValue )
{
	WritePrivateProfileString( strSection, strKey, strValue, strFileName ); 
}
/*************************************************************************************/