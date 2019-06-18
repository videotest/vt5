#include "misc_utils.h"
#include "\vt5\awin\profiler.h"

/*************************************************************************************/
inline HRESULT CClassifyContainerImpl::SetName( /*[in] */BSTR bstrName )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !bstrName )
		return S_FALSE;
	
	m_strName = bstrName;
	return S_OK;
}

inline HRESULT CClassifyContainerImpl::GetName(/*[out] */BSTR *pbstrName )
{
	if( !pbstrName )
		return S_FALSE;
	
	*pbstrName = m_strName.AllocSysString();
	return S_OK;
}
/*************************************************************************************/
inline HRESULT CClassifyContainerImpl::SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz )
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

inline HRESULT CClassifyContainerImpl::GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz )
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

inline HRESULT CClassifyContainerImpl::RemoveClasses()
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

inline HRESULT CClassifyContainerImpl::GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams )
{
	long lPos = m_mapClassParams.find( lClass );
	if( lPos > 0 )
	{
		*ppParams = m_mapClassParams.get( lPos );
		return S_OK;
	}

	return S_FALSE;
}

inline HRESULT CClassifyContainerImpl::SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	m_mapClassParams.set( pfParams, lClass );
	return S_OK;
}
/*************************************************************************************/
inline HRESULT CClassifyContainerImpl::SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz )
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

inline HRESULT CClassifyContainerImpl::GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz )
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

inline HRESULT CClassifyContainerImpl::RemoveKeys()
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

inline HRESULT CClassifyContainerImpl::GetKeyParams( /*[in] */long lClass,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz )
{
	if( !ppfParams && !plSz )
		return S_FALSE;

	long lPos = m_mapKeyParams[lClass].find( lKey );
	if( lPos > 0 )
	{
		XKeyItem* item = m_mapKeyParams[lClass].get( lPos );

		if( !ppfParams && plSz )
		{
			*plSz = item->GetSize();
			return S_OK;
		}
		::CopyMemory( *ppfParams, item->GetData(), sizeof( double ) * ( *plSz ) );
		return S_OK;
	}

	return S_FALSE;
}

inline HRESULT CClassifyContainerImpl::SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long lSz )
{
	if( m_dwFlag & IDCP_READONLY )
		return S_FALSE;

	if( !pfParams )
		return S_FALSE;

	XKeyItem* item = new XKeyItem( lSz );

	::CopyMemory( item->GetData(), pfParams, sizeof( double ) * lSz );
	m_mapKeyParams[lClass].set( item, lKey );

	return S_OK;
}
/*************************************************************************************/
inline HRESULT CClassifyContainerImpl::GetFlags( /*[out]*/ DWORD *pdwFlag )
{
	if( !pdwFlag )
		return S_FALSE;

	*pdwFlag = m_dwFlag;

	return S_OK;
}

inline HRESULT CClassifyContainerImpl::SetFlags( /*[in]*/ DWORD dwFlag )
{
	m_dwFlag = dwFlag;
	return S_OK;
}
/*************************************************************************************/
inline HRESULT CClassifyContainerImpl::Load( /*[in]*/ BSTR bstrFile )
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

inline HRESULT CClassifyContainerImpl::Store( /*[in]*/ BSTR bstrFile )
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
inline HRESULT CClassifyContainerImpl::Start()
{
	m_arObjectList.clear();
	return S_OK;
}

inline HRESULT CClassifyContainerImpl::Process( /*[in]*/ IUnknown *punkObject )
{
	PROFILE_TEST( "CClassifyContainerImpl::Process" );

	ICalcObjectPtr sptrObject = punkObject;

	if( sptrObject == 0 )
		return S_FALSE;

	long lClassCount = m_lSz2;
	//GetClasses( 0, &lClassCount );

	if( !lClassCount )
		return S_FALSE;

	long *lClassArr = m_plClasses;//new long[lClassCount];

	if( !lClassArr )
		return S_FALSE;

	//GetClasses( &lClassArr, &lClassCount );

	if( m_quels.size() != lClassCount )
	{
		m_quels.clear();
		m_quels.alloc( lClassCount );
	}
	int *pQuels = m_quels.ptr();//new int[lClassCount];	

	if( !pQuels )
		return S_FALSE;

	::ZeroMemory( pQuels, sizeof( int ) * lClassCount );

	//add to map. Need use pointer to class in map, list, etc to skip copy constructor use
	XObjectItem* pitem = new XObjectItem;
	pitem->sptrObject = punkObject;
	pitem->guidKey = ::GetKey( punkObject );
	m_arObjectList.set( pitem, &pitem->guidKey );

	long lKeyCount = m_lSz;
	long *lKeysArr = m_plKeys;

	for( int cl = 0; cl < lClassCount; cl++ )
	{
		if( !lKeyCount || !lKeysArr )
			continue;
		/*long lKeyCount = 0;
		GetKeys( 0, &lKeyCount );

		if( !lKeyCount )
			continue;

		long *lKeysArr = new long[lKeyCount];

		if( !lKeysArr )
			continue;

		GetKeys( &lKeysArr, &lKeyCount );
		*/

		for( int k = 0; k < lKeyCount; k++ )
		{
			//if( lSz != 2 || !pParams )
			//	continue;
			
			/*long lSz = 0;
			GetKeyParams( lClassArr[cl], lKeysArr[k], 0, &lSz );

			if( lSz != 2 )
				continue;

			double *pParams = new double[lSz];

			GetKeyParams( lClassArr[cl], lKeysArr[k], &pParams, &lSz );
			*/

			XKeyItem* pkey_item = find_key_item( lClassArr[cl], lKeysArr[k] );
			if( !pkey_item )
				continue;

			long lSz = pkey_item->GetSize();
			double *pParams = pkey_item->GetData();			
			
			if( lSz != 2 || !pParams )
				continue;
			///// Checking.... ///////////////////////
			double fVal = 0;
			if( sptrObject->GetValue( lKeysArr[k], &fVal ) == S_OK )
			{
				// vanek - 31.10.2003
				BOOL bValueInClassInterval = FALSE;	          
                if( bValueInClassInterval = (fVal >= pParams[0] && fVal <= pParams[1]) )
					pQuels[cl]++;
                if( IsNeedDescrObj( pitem->sptrObject, cl ) )
				{
                    CString strObjDescrByKey( _T("") );
					strObjDescrByKey.Format( "%d:", lKeysArr[k] );
					strObjDescrByKey = strObjDescrByKey + CString(_variant_t(fVal)) + _T(":");
					if( bValueInClassInterval )
						strObjDescrByKey += _T( "=" );
					else
						strObjDescrByKey += (fVal < pParams[0]) ? _T( "<" ) : _T( ">" );

					if( !m_strDescription.IsEmpty() )
							m_strDescription += _T( ";" );
					m_strDescription += strObjDescrByKey;				                 
				}
			}
			else
			{
                
			}
			////////////////////////////

			//delete []pParams;
		}
		//delete []lKeysArr;
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

	//long lKeyCount = 0;
	//GetKeys( 0, &lKeyCount );

	if( nMax != lKeyCount )
		pitem->lClass = -1;
	else
	{
		if( nMaxID != -1 )
			pitem->lClass = lClassArr[nMaxID];
		else
			pitem->lClass = -1;
	}

	//delete []pQuels;
	//delete []lClassArr;

	//m_arObjectList.add_tail( item );

	return S_OK;
}
inline HRESULT CClassifyContainerImpl::GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass )
{
	PROFILE_TEST( "CClassifyContainerImpl::GetObjectClass" );

	if( !punkObject || !plClass )
		return S_FALSE;

	GuidKey guidKey = ::GetKey( punkObject );

	long lpos = m_arObjectList.find( &guidKey );
	if( !lpos )
		return S_FALSE;

	XObjectItem* pitem = m_arObjectList.get( lpos );
	*plClass = pitem->lClass;
	return S_OK;

	/*
	for( long lPos = m_arObjectList.head(); lPos; lPos = m_arObjectList.next( lPos ) )
	{
		XObjectItem* pitem = m_arObjectList.get( lPos );
		if( pitem->guidKey == guidKey )
		{
			*plClass = pitem->lClass;
			return S_OK;
		}
	}	
	return S_FALSE;
	*/
}

/*************************************************************************************/
inline CString CClassifyContainerImpl::_get_value_string( CString strFileName, CString strSection, CString strKey )
{
	TCHAR szT[4096];
	DWORD dw = ::GetPrivateProfileString( strSection, strKey,
		"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
	if( dw > 0 )
		return szT;
	return "";
}

inline void CClassifyContainerImpl::_set_value_string( CString strFileName, CString strSection, CString strKey, CString strValue )
{
	WritePrivateProfileString( strSection, strKey, strValue, strFileName ); 
}

/*************************************************************************************/
// IClassifyProxy2
inline HRESULT CClassifyContainerImpl::Teach()
{
	return E_NOTIMPL;
}

/*************************************************************************************/
// IClassifyInfoProxy
inline HRESULT CClassifyContainerImpl::PrepareObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass )
{
	if( !punkObject )
		return S_FALSE;

	m_sptrDescrObj = punkObject;
	m_lDescrClass = lClass;
	InitDescription();
	return S_OK;
}

inline HRESULT CClassifyContainerImpl::GetObjectDescrByClass( /*[out]*/ BSTR *pbstrDescription )
{
	if( !pbstrDescription || m_sptrDescrObj == 0 )        
		return S_FALSE;
    
	*pbstrDescription = m_strDescription.AllocSysString();
	return S_OK;
}

inline BOOL CClassifyContainerImpl::IsNeedDescrObj( IUnknown *punkCurrObj, long lCurrClass )
{
    return ( punkCurrObj && lCurrClass == m_lDescrClass && punkCurrObj == m_sptrDescrObj );
}

inline CClassifyContainerImpl::XKeyItem* CClassifyContainerImpl::find_key_item( long lclass, long lkey )
{
	if( !m_mapKeyParams || lclass < 0 || lclass > m_lSz2 )	return 0;

	long lpos = m_mapKeyParams[lclass].find( lkey );
	if( !lpos )		return 0;

	XKeyItem* pitem = m_mapKeyParams[lclass].get( lpos );
	return pitem;
}

//inline HRESULT CClassifyContainerImpl::GetObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass, /*[out]*/ BSTR *pbstrDescription )
/*{
    if( !punkObject || !pbstrDescription )        
		return S_FALSE;

	ICalcObjectPtr sptrObject = punkObject;

	if( sptrObject == 0 )
		return S_FALSE;

    CString strDescription( _T("") ), 
			strSeparator( _T(";") );

	if ( lClass != -1 )
	{
        long lKeyCount = 0;
		GetKeys( 0, &lKeyCount );

		if( lKeyCount )
		{
			long *lKeysArr = new long[lKeyCount];

			if( lKeysArr )
			{
				GetKeys( &lKeysArr, &lKeyCount );

				for( int k = 0; k < lKeyCount; k++ )
				{
					long lSz = 0;
					GetKeyParams( lClass, lKeysArr[k], 0, &lSz );

					if( !lSz )
						continue;

					double *pParams = new double[lSz];

					GetKeyParams( lClass, lKeysArr[k], &pParams, &lSz );

					///// Checking.... ///////////////////////
					double fVal = 0;
					if( sptrObject->GetValue( lKeysArr[k], &fVal ) == S_OK )
					{
						if( !strDescription.IsEmpty() )
							strDescription += strSeparator;

						CString strRes( _T("") );
						strRes.Format( "%d:", lKeysArr[k] );
		                
						if( fVal < pParams[0] ) 
							strRes += _T("<");                         					
						else if( fVal > pParams[lSz - 1] )
							strRes += _T(">");                         					
						else 
							strRes += _T("=");                         					

						strDescription += strRes;
					}
					////////////////////////////

					delete []pParams;
				}
				delete []lKeysArr;        		
			}
		}
	} 

	*pbstrDescription = strDescription.AllocSysString();
	return S_OK;
}

*/