#include "stdafx.h"
#include "usermanualmeasgroup.h"
#include "units_const.h"
#include "Params.h"
#include "ScriptNotifyInt.h"
#include "misc_utils.h"

CIntIniValue s_EnableNegative( "\\measurement\\UserParamSettings", "EnableNegativeResult", 1 );

#define	sMainSection	sMainMeasParamsSection

static CParamElement ParseParamString(const char *pszParamString)
{
	long lKey = atol(pszParamString);
	const char *pc = strchr(pszParamString, ':');
	if (pc)
	{
		int nPane = atoi(pc+1);
		int nPhase = 0;
		const char *pcomma = strchr(pc+1, ',');
		if (pcomma)
			nPhase = atoi(pcomma+1);
		return CParamElement(lKey, nPane, nPhase);
	}
	else
		return CParamElement(lKey);
};

IMPLEMENT_DYNCREATE(CUserManualMeasGroup, CMeasParamGroupBase)

CUserManualMeasGroup::CUserManualMeasGroup(void)
{
	_OleLockApp( this );

	m_sName = "UserManual";

	// [vanek] - 20.03.2005
	m_bcalc_in_curr_units = false;

	// delete all
	m_lCalculatedKeys.clear( );
}

CUserManualMeasGroup::~CUserManualMeasGroup(void)
{
	// delete all
	m_lCalculatedKeys.clear( );
	m_mapKeyToParam.clear( );

	for (int i = 0; i < m_arrZombies.GetSize(); i++)
	{
		ParameterDescriptor	*pdescr = (ParameterDescriptor	*)m_arrZombies.GetAt(i);
		if( pdescr->bstrName )::SysFreeString( pdescr->bstrName );
		if( pdescr->bstrUserName )::SysFreeString( pdescr->bstrUserName );
		if( pdescr->bstrDefFormat )::SysFreeString( pdescr->bstrDefFormat );
		delete pdescr;
	}

	_OleUnlockApp( this );
}

// {0859FEA3-600D-4a6c-86CD-CC40B3AB95A5}
IMPLEMENT_GROUP_OLECREATE( CUserManualMeasGroup, "Objects.UserManualMeasGroup", 
0x859fea3, 0x600d, 0x4a6c, 0x86, 0xcd, 0xcc, 0x40, 0xb3, 0xab, 0x95, 0xa5);

bool	CUserManualMeasGroup::get_string_by_key( long lKey, CString *str, char *sEntryName )
{
	str->Empty( );
	POSITION pos = 0;
	if( !m_mapKeyToPosition.Lookup( lKey, pos ) )
		return false;
		
	ParameterDescriptor*	pdescr = (ParameterDescriptor*) m_listParamDescr.GetAt( pos );
	if( !pdescr )
		return false;

	_bstr_t bstrSection = sMainSection;
	bstrSection += "\\" + _bstr_t( pdescr->bstrName );
	*str = ::GetValueString( ::GetAppUnknown( ), bstrSection, sEntryName, "");
	return true;
 }

bool	CUserManualMeasGroup::calc_value( ICalcObject2Ptr	&ptrCalc, ICalcObjectContainerPtr &ptrCalcCont, _list_t<CParamElement> &lCalculateKeys, double *pfResult )
{	
	*pfResult = 0;
	TPOS lPos = 0;
	CParamElement CurElem = lCalculateKeys.get( lCalculateKeys.tail( ) );
	long lCurrKey = CurElem.lKey; // последний ключ в lCalculateKeys - ключ параметра, для которого надо посчитать
	// && на || - бага по поводу расчета, когда заданы ключи "чужих" параметров
	if( ( (lCurrKey < MEAS_USER_PARAMS_FIRST_KEY) || (lCurrKey > MEAS_USER_PARAMS_LAST_KEY) ) )
	{	// чужой посчитанный
        
		// [vanek] - 21.03.2005
		double fcoeff_to_units = 1.;
		if( m_bcalc_in_curr_units && ptrCalcCont != 0 )
		{
            ParameterContainer	*pi = 0;
			ptrCalcCont->ParamDefByKey(lCurrKey, &pi);
			if( pi != 0 ) 
				fcoeff_to_units = pi->pDescr->fCoeffToUnits;
		}

		bool bretval = true;
		if( CurElem.bExtended )
			bretval = ( S_OK == ptrCalc->GetValueEx( lCurrKey, &CurElem.AddInfo, pfResult ) );
		else 
			bretval = ( S_OK == ptrCalc->GetValue( lCurrKey, pfResult ));

		if( bretval )
			*pfResult *= fcoeff_to_units;

		return bretval;
	}
	else if( (lPos = m_lCalculatedKeys.find( lCurrKey )) )
	{	// свой посчитанный
		*pfResult = m_lCalculatedKeys.get( lPos );
		return true;
	}
	
	// свой, но еще не посчитан
	CUserManualMeasParam *pParam = 0;
	TPOS lParamPos = m_mapKeyToParam.find(lCurrKey);
	if( lParamPos )
		pParam = m_mapKeyToParam.get( lParamPos );
	else
	{
		// get expr string from namedata
		CString strExpr;
		if( !get_string_by_key( lCurrKey, &strExpr, "ExprString" ) )
			return false;
		if(	strExpr.Left(1) == "#" )
		{ // если начинается с # - строка закодирована
			strExpr.Delete(0,1);
			// раскодирование
			//int n = strExpr.GetLength()/2;
			//CString str;
			//const char* sz = strExpr;
			//for(int i=0; i<n; i++)
			//{
			//	char c = sz[i*2];
			//	int n1 = ( c + (c>'9' ? 10-'A' : 0-'0') ) & 15;
			//	c = sz[i*2+1];
			//	int n2 = ( c + (c>'9' ? 10-'A' : 0-'0') ) & 15;
			//	int n = n1*16 + n2;
			//	n ^= 0x55;
			//	str += char(n);
			//}
			//strExpr = str;
		}
		pParam = new CUserManualMeasParam;

		if( ! pParam->Init( &strExpr ) )
		{
			delete pParam;
			pParam = 0;
		}

		m_mapKeyToParam.set( pParam, lCurrKey ); // запомним в мапе (если не проинитился - запомним и это, будет указатель=0)
	}

	if( !pParam )
		return false;

	// get params
	BSTR *pbstrParams = 0;
	long lParamsSize = 0;

	if( !pParam->ReturnNecessaryCalcParams( 0, &lParamsSize ) ) 
		return false;

	pbstrParams = new BSTR[lParamsSize];

	if( !pParam->ReturnNecessaryCalcParams( pbstrParams, &lParamsSize ) ) 
	{
		if( pbstrParams )
			delete[] pbstrParams; pbstrParams= 0;
		
		return false;
	}

	bool lRes = true;
	long lKeyParam = 0;
    _list_t<CParamElement> TempCalculateKeys;

	if( lParamsSize > 0 )
	{
		// copy to temp
		for (TPOS lPos = lCalculateKeys.head(); lPos; lPos = lCalculateKeys.next(lPos))
			TempCalculateKeys.add_tail( lCalculateKeys.get( lPos ) );

		double *pParamsValues = 0;
		pParamsValues = new double[ lParamsSize ];

		// calculate parameters of expr
		for( long lIndex = 0; lIndex < lParamsSize; lIndex++ )
		{
//			lKeyParam = atol( _bstr_t( pbstrParams[lIndex] ) );
			CParamElement pe = ParseParamString(_bstr_t( pbstrParams[lIndex] ));
			if( TempCalculateKeys.find( pe ) )
			{	// find circular reference
				lRes = false;	
				break;
			}
			TempCalculateKeys.add_tail( pe );
			if( !calc_value( ptrCalc, ptrCalcCont, TempCalculateKeys, &pParamsValues[lIndex] ) )
			{
				lRes = false;
				break;
			}
		}

		for( long lIndex = 0; lIndex < lParamsSize; lIndex++ )
			::SysFreeString( pbstrParams[lIndex] );
		TempCalculateKeys.clear( );

		if( lRes )
			lRes = pParam->SetParamsValues( pParamsValues, lParamsSize ) ? true : false;

		if( pParamsValues )
		{
			delete[] pParamsValues;
			pParamsValues = 0;
		}

	}

	if( pbstrParams )
	{
		delete[] pbstrParams; 
		pbstrParams = 0;
	}

	if( lRes )
	{
		BSTR bstr = 0;
        if ( lRes = (pParam->Calculate( pfResult, &bstr ) == TRUE) ) 
				m_lCalculatedKeys.set( *pfResult, lCurrKey );	// add to list of calculated params
		if( bstr )
		{
			::SysFreeString( bstr );
			bstr = 0;
		}
	}
	
	pParam = 0;
	return lRes;
}

bool CUserManualMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	// [vanek] - 21.03.2005
	ICalcObjectContainerPtr sptrCOC;
	INamedDataObject2Ptr	sptrNDO2 = punkCalcObject;
	if( sptrNDO2 != 0 )
	{
		IUnknown *punkParent = 0;
        sptrNDO2->GetParent( &punkParent );
		sptrCOC = punkParent;
		if( punkParent )
			punkParent->Release();
		punkParent = 0;
	}


	ICalcObject2Ptr	ptrCalc = punkCalcObject;
	_list_t<CParamElement> CalculateKeys;
	double fResult = 0;
	POSITION pos;

	for( pos = m_listParamDescr.GetHeadPosition( ); pos != NULL;  )
	{	
		fResult = 0;
		CalculateKeys.clear( );
		ParameterDescriptor*	pdescr = (ParameterDescriptor*) m_listParamDescr.GetNext( pos );
		if( pdescr->lEnabled )
		{
			CalculateKeys.add_head( CParamElement(pdescr->lKey) );
			if( calc_value( ptrCalc, sptrCOC, CalculateKeys, &fResult ) )
			{
				if (fResult < 0. && !s_EnableNegative)
				{
					ptrCalc->RemoveValue( pdescr->lKey );
					IScriptSitePtr	sptrSS(::GetAppUnknown());
					if (sptrSS != 0)
					{
						_bstr_t bstrEvent("UserMeasureParams_OnNegativeResult");
						_variant_t varKey((long)pdescr->lKey);
						sptrSS->Invoke(bstrEvent, &varKey, 1, 0, fwAppScript);
					}
				}
				else
					ptrCalc->SetValue( pdescr->lKey, fResult );
			}
			else
				ptrCalc->RemoveValue( pdescr->lKey );
		}
		else
			m_mapKeyToParam.remove_key( pdescr->lKey );
	}

	// delete all
	m_lCalculatedKeys.clear( );

	return true;
}


bool CUserManualMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff )
{
	bstrType = GetValueString( GetAppUnknown(), szRelativeUnits, szUnitName, szDefRelativeUnits );
	fCoeff = GetValueDouble( GetAppUnknown(), szRelativeUnits, szUnitCoeff, 1 );
	return true;
}

bool CUserManualMeasGroup::LoadCreateParam()
{
	CMap<long,long,bool,bool> mapFoundKeys;

	// [vanek] - 20.03.2005
	m_bcalc_in_curr_units = (0 != ::GetValueInt( ::GetAppUnknown(), "\\measurement\\UserParamSettings", "CalcUserParamsInCurrUnits", 0));

    long lCount = ::GetEntriesCount( ::GetAppUnknown(), sMainSection );

	for( long lPos = 0; lPos < lCount; lPos++ )
	{
		CString sParamsSection = sMainSection,
				sEntryName = ::GetEntryName( ::GetAppUnknown(), sMainSection, lPos );

		sParamsSection += "\\" + sEntryName;
		long lKey = GetValueIntWithoutSet( ::GetAppUnknown(), sParamsSection, "Key", (MEAS_USER_PARAMS_LAST_KEY + 1) ); 
		if( ( lKey >= MEAS_USER_PARAMS_FIRST_KEY ) && ( lKey <= MEAS_USER_PARAMS_LAST_KEY ) )
		{
			POSITION pos = 0;
			if( !m_mapKeyToPosition.Lookup( lKey, pos ) )
				DefineParameter( lKey, sEntryName, ::GetValueString( ::GetAppUnknown( ), sParamsSection, "Format", "" ) );
			mapFoundKeys.SetAt(lKey,true);
		}
	}

	POSITION pos = m_listParamDescr.GetHeadPosition();
	while( pos )
	{
		POSITION posPrev = pos;
		ParameterDescriptor	*pdescr = (ParameterDescriptor	*)m_listParamDescr.GetNext( pos );
		bool b;
		if (!mapFoundKeys.Lookup(pdescr->lKey,b))
		{
			m_mapKeyToPosition.RemoveKey(pdescr->lKey);
			m_arrZombies.Add(pdescr);
//			delete pdescr;
			m_listParamDescr.RemoveAt(posPrev);
		}
	}


	return true;
}

bool CUserManualMeasGroup::OnInitCalculation()
{
	CIniValue::Reset(); // Перед пересчетом заново прочитать данные
	// подгрузка параметров
	LoadCreateParam( );
	m_mapKeyToParam.clear( );

	return CMeasParamGroupBase::OnInitCalculation();
}

bool CUserManualMeasGroup::ReloadState()
{
	// подгрузка параметров
	LoadCreateParam( );
	m_mapKeyToParam.clear( );

	return __super::ReloadState();
}


//////////////////////////////////////////////////////////
//IPersist
void CUserManualMeasGroup::GetClassID( CLSID* pClassID )
{
	if(pClassID==0) return; //так, на всякий пожарный
	memcpy( pClassID, &guid, sizeof(CLSID) );
}


//////////////////////////////////////////////////////
// IPriority
HRESULT CUserManualMeasGroup::GetPriority(long *plPriority)
{
	*plPriority = -10;
	return S_OK;
}