#include "stdafx.h"
#include "classifier.h"
#include "class_utils.h"
#include "math.h"

CManualClassifier::CManualClassifier(void)
{
}

CManualClassifier::~CManualClassifier(void)
{
}

IUnknown *CManualClassifier::DoGetInterface( const IID &iid )
{
	if( iid == IID_IClassifyParams )
		return (IClassifyParams *)this;
	else if( iid == IID_IClassifyProxy )
		return (IClassifyProxy *)this;
	else if( iid == IID_IClassifyInfoProxy ) // vanek - 31.10.2003
		return (IClassifyInfoProxy *)this;

	return ComObjectBase::DoGetInterface( iid );
}


////////////////////////////////////////////////////////////////////////
CAutomaticClassifier::CAutomaticClassifier(void)
{
	m_lDivisionType = 0;
	m_fDivisionValue = 0;

	m_bUseRange = 0;
	m_fMinVal = 0;
	m_fMaxVal = 0;
}

CAutomaticClassifier::~CAutomaticClassifier(void)
{
}

IUnknown *CAutomaticClassifier::DoGetInterface( const IID &iid )
{
	if( iid == IID_IClassifyParams )
		return (IClassifyParams *)this;
	else if( iid == IID_IClassifyProxy )
		return (IClassifyProxy *)this;
	else if( iid == IID_IClassifyInfoProxy ) // vanek - 31.10.2003
		return (IClassifyInfoProxy *)this;

	return __super::DoGetInterface( iid );
}

HRESULT CAutomaticClassifier::Process( /*[in]*/ IUnknown *punkObject )
{
	/*
	XObjectItem item;

	item.sptrObject = punkObject;
	item.lClass = -1;
	item.guidKey = ::GetKey( punkObject );

	m_arObjectList.add_tail( item );
	*/

	XObjectItem* pitem = new XObjectItem;
	pitem->sptrObject = punkObject;
	pitem->lClass = -1;
	pitem->guidKey = ::GetKey( punkObject );
	m_arObjectList.set( pitem, &pitem->guidKey );

	return S_OK;
}

HRESULT CAutomaticClassifier::Finish()
{
	long lClassSz = 0;
	GetClasses( 0, &lClassSz );

	if( !lClassSz )
		return S_FALSE;

	long *lClasses = new long[lClassSz];

	if( !lClasses )
		return S_FALSE;

	GetClasses( &lClasses, &lClassSz );

	long lKeySz = 0;
	GetKeys( 0, &lKeySz );

	if( !lKeySz )
		return false;

	long *lKeys = new long[lKeySz];
	
	if( !lKeys )
		return false;

	GetKeys( &lKeys, &lKeySz );

	double *pMinVal = new double[lKeySz];
	double *pMaxVal = new double[lKeySz];

	double *pMinPosVal = new double[lKeySz];
	double *pMaxNegVal = new double[lKeySz];

	for( long k = 0; k < lKeySz; k++ )
	{
		pMinVal[k] = 1e308;
		pMaxVal[k] = -1e307;
		pMinPosVal[k] = 1e308;
		pMaxNegVal[k] = -1e307;
	}

	long lObjCount = 0;
	for (TPOS lPos = m_arObjectList.head(); lPos; lPos = m_arObjectList.next(lPos))
	{
		lObjCount++;
		XObjectItem* pitem = m_arObjectList.get( lPos );
		ICalcObjectPtr sptrObject = pitem->sptrObject;
		if( sptrObject != 0 )
		{
			for( long k = 0; k < lKeySz; k++ )
			{
				double fVal = 0;
				if( sptrObject->GetValue( lKeys[k], &fVal ) != S_OK )
					continue;


				if( fVal < pMinVal[k] )
					pMinVal[k] = fVal;
				if( fVal > pMaxVal[k] )
					pMaxVal[k] = fVal;

				if( fVal>0 && fVal < pMinPosVal[k] )
					pMinPosVal[k] = fVal;
				if( fVal<0 && fVal > pMaxNegVal[k] )
					pMaxNegVal[k] = fVal;

				if( m_bUseRange )
				{
//					if( pMinVal[k] < m_fMinVal )
						pMinVal[k] = m_fMinVal;
						if(m_fMinVal>0) pMinPosVal[k] = m_fMinVal;

//					if( pMaxVal[k] > m_fMaxVal )
						pMaxVal[k] = m_fMaxVal;
						if(m_fMaxVal<0) pMaxNegVal[k] = m_fMaxVal;
				}
			}
		}
	}
	if (!m_bUseRange)
		for( long k = 0; k < lKeySz; k++ )
		{ // чуть расширим диапазон для автомата - дабы крайние точки точно попали в него
			double delta = (pMaxVal[k] - pMinVal[k]) * 1e-6;
			if( m_lDivisionType == 1 )
				delta = min(delta, m_fDivisionValue*0.01);
			pMinVal[k] -= delta;
			pMaxVal[k] += delta;
			pMinPosVal[k] *= (1 - 1e-6);
			pMaxNegVal[k] *= (1 - 1e-6);
		}


	{
		CString strPath;
		strPath.Format( "\\Classify\\%s", short_filename( m_strFileName.GetBuffer( m_strFileName.GetLength() ) ) );

		INamedDataPtr sptrND = ::GetAppUnknown();
		sptrND->DeleteEntry( _bstr_t( strPath ) );
	}

	_ptr_t2<double> afDivValues(lKeySz);
	_ptr_t2<double> afSteps(lKeySz);
	for( long k = 0; k < lKeySz; k++ )
	{
		if( m_lDivisionType == 0 || m_lDivisionType == 2 )
			m_fDivisionValue = lClassSz;

		if( m_fDivisionValue == 0 )
			m_fDivisionValue = lClassSz;

		// 0 - диапазон от min до max делится на m_fDivisionValue частей
		double fStep = ( pMaxVal[k] - pMinVal[k] ) / m_fDivisionValue;

		// 1 - шаг зажан
		if( m_lDivisionType == 1 )
			fStep = m_fDivisionValue;

		// 0 - диапазон от ln(min) до ln(max) делится на m_fDivisionValue частей
		if( m_lDivisionType == 2 )
		{
			//added '&& lObjCount>1' bugfix 5350
			if( pMinPosVal[k]>0 && pMaxVal[k]>pMinPosVal[k] && lObjCount>1)
				fStep = log( pMaxVal[k] / pMinPosVal[k] ) / m_fDivisionValue;
			else
				fStep = 0;
		}

		SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "Step", fStep );

		if( m_lDivisionType == 2 )
			SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "GeomStep", exp(fStep) );
		else
			SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "GeomStep", 0.0 );

		afDivValues[k] = m_fDivisionValue;
		afSteps[k] = fStep;
	}

	bool bLimitsSaved = false;
	for( TPOS lPos = m_arObjectList.head(); lPos; lPos = m_arObjectList.next( lPos ))
	{
		XObjectItem* pitem = m_arObjectList.get( lPos );
		ICalcObjectPtr sptrObject = pitem->sptrObject;

		int *pQuels = new int[lClassSz];
		::ZeroMemory( pQuels, sizeof( int ) * lClassSz );

		if( sptrObject != 0 )
		{
			for( long k = 0; k < lKeySz; k++ )
			{
				double fVal = 0;
				if( sptrObject->GetValue( lKeys[k], &fVal ) != S_OK )
					continue;

#if 1
				m_fDivisionValue = afDivValues[k];
				double fStep = afSteps[k];
#else
				if( m_lDivisionType == 0 || m_lDivisionType == 2 )
					m_fDivisionValue = lClassSz;

				if( m_fDivisionValue == 0 )
					m_fDivisionValue = lClassSz;

				// 0 - диапазон от min до max делится на m_fDivisionValue частей
				double fStep = ( pMaxVal[k] - pMinVal[k] ) / m_fDivisionValue;

				// 1 - шаг зажан
				if( m_lDivisionType == 1 )
					fStep = m_fDivisionValue;

				// 0 - диапазон от ln(min) до ln(max) делится на m_fDivisionValue частей
				if( m_lDivisionType == 2 )
				{
					if( pMinPosVal[k]>0 && pMaxVal[k]>pMinPosVal[k] )
						fStep = log( pMaxVal[k] / pMinPosVal[k] ) / m_fDivisionValue;
					else
						fStep = 0;
				}

				SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "Step", fStep );

				if( m_lDivisionType == 2 )
					SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "GeomStep", exp(fStep) );
				else
					SetValue( GetAppUnknown(), "\\Classify\\AutomaticClassifier", "GeomStep", 0.0 );
#endif

				for( long cl = 0; cl < lClassSz; cl++ )
				{
					double a = 0;
					double b = 0;

					if( m_lDivisionType == 2 )
					{
						a = pMinPosVal[k]*exp(fStep*cl);
						b = pMinPosVal[k]*exp(fStep*(cl+1));

						if( a >= pMaxVal[k] )
							continue;
						
/*						if( m_bUseRange )
						{
							if( a < m_fMinVal )
								a = m_fMinVal;

							if( b > m_fMaxVal )
								b = m_fMaxVal;
						}
*/
					}
					else
					{
						a = pMinVal[k] + cl * fStep;
						b = pMinVal[k] + ( cl + 1 ) * fStep;

						if( a >= pMaxVal[k] )
							continue;

						if( ( cl + 1 ) == lClassSz )
							b = pMaxVal[k];

/*						if( m_bUseRange )
						{
							if( a < m_fMinVal )
								a = m_fMinVal;

							if( b > m_fMaxVal )
								b = m_fMaxVal;
						}
*/
					}

					if( !bLimitsSaved )
					{
						CString strInterval, strPath;

						strPath.Format( "\\Classify\\%s\\%d\\class%d", short_filename( m_strFileName.GetBuffer( m_strFileName.GetLength() ) ),  lKeys[k], cl );
						strInterval.Format( "%e,%e", a, b );
						::SetValue( ::GetAppUnknown(), 0,	strPath, strInterval );
						m_strFileName.ReleaseBuffer();
					}

					// vanek - 31.10.2003
					BOOL bValueInClassInterval = FALSE;	          
					if( bValueInClassInterval = (fVal >= a && fVal <= b) )
						pQuels[cl]++;
									
					if( IsNeedDescrObj( pitem->sptrObject, cl ) )
					{
                        CString strObjDescrByKey( _T("") );
						strObjDescrByKey.Format( "%d:", lKeys[k] );
						strObjDescrByKey = strObjDescrByKey + CString(_variant_t(fVal)) + _T(":");
						if( bValueInClassInterval )
							strObjDescrByKey += _T( "=" );
						else
							strObjDescrByKey += (fVal < a) ? _T( "<" ) : _T( ">" );

						if( !m_strDescription.IsEmpty() )
								m_strDescription += _T( ";" );
						m_strDescription += strObjDescrByKey;				                 
					}
				}
				if (!bLimitsSaved)
					bLimitsSaved = true;
			}
		}

		int nMax = -1, nMaxID = -1;
		for( int i = 0; i < lClassSz; i++ )
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
			pitem->lClass = -1;
		else
		{
			if( nMaxID != -1 )
				pitem->lClass = lClasses[nMaxID];
			else
				pitem->lClass = -1;
		}
		delete []pQuels;
	}

	if( lClasses )
		delete []lClasses;

	if( lKeys )
		delete []lKeys;

	if( pMinVal )
		delete []pMinVal;

	if( pMaxVal ) 
		delete []pMaxVal;

	if( pMinPosVal )
		delete []pMinPosVal;

	if( pMaxNegVal ) 
		delete []pMaxNegVal;

	return S_FALSE;
}

HRESULT CAutomaticClassifier::Load( /*[in]*/ BSTR bstrFile )
{
	CString strFileName( bstrFile );
	m_strFileName = strFileName;

	CString strPROGID = _get_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID );
	
	if( strPROGID.IsEmpty() )
		return S_FALSE;

	m_strPROGID = strPROGID;

	m_strName = _get_value_string( strFileName, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_NAME );

	if( m_strName.IsEmpty() )
		m_strName = strPROGID;

	CString strRange = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "UseRange" );
	if( !strRange.IsEmpty() )
		m_bUseRange = atol( strRange ) != 0;

	CString strRangeMin = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "RangeMinVal" );
	if( !strRangeMin.IsEmpty() )
		m_fMinVal = atof( strRangeMin );

	CString strRangeMax = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "RangeMaxVal" );
	if( !strRangeMax.IsEmpty() )
		m_fMaxVal = atof( strRangeMax );


	CString strDivision = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "DivisionType" );
	if( !strDivision.IsEmpty() )
		m_lDivisionType = atol( strDivision );

	CString strDivisionVal = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "DivisionValue" );
	if( !strDivisionVal.IsEmpty() )
		m_fDivisionValue = atof( strDivisionVal );


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
	}



	delete []Strs;
	
	return S_OK;
}

HRESULT CAutomaticClassifier::Store( /*[in]*/ BSTR bstrFile )
{
	HRESULT hr = __super::Store( bstrFile );
	if( hr == S_OK )
	{
		CString strFileName = bstrFile;

		CString strRange = _get_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "UseRange" );
		if( !strRange.IsEmpty() )
			m_bUseRange = atol( strRange ) != 0;

		CString strRangeMin;
		strRangeMin.Format( "%lf", m_fMinVal );
		_set_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "RangeMinVal", strRangeMin );

		CString strRangeMax;
		strRangeMax.Format( "%lf", m_fMaxVal );
		_set_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "RangeMaxVal", strRangeMax );

		CString strDivision;
		strDivision.Format( "%ld", m_lDivisionType );
		_set_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "DivisionType", strDivision );

		CString strDivisionVal;
		strDivision.Format( "%lf", m_fDivisionValue );
		_set_value_string( strFileName, ID_SECT_CLASSIFIER_INFO, "DivisionValue", strDivisionVal );
	}
	return hr;
}

// IClassifyInfoProxy
HRESULT CAutomaticClassifier::GetObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass, /*[out]*/ BSTR *pbstrDescription )
{
    
    return S_OK;    
}