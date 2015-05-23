#pragma once

#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include "calc_int.h"
#include "\vt5\comps\misc\Calculator\Calculator.h"

struct CParamElement
{
	long lKey;
	bool bExtended;
	ParameterAdditionalInfo AddInfo;
	CParamElement()
	{
		lKey = -1;
	}
	CParamElement(long lKey)
	{
		this->lKey = lKey;
		bExtended = false;
	}
	CParamElement(long lKey, int nPane, int nPhase)
	{
		this->lKey = lKey;
		bExtended = true;
		AddInfo.byPane = (byte)max(0,min(nPane,255));
		AddInfo.byPhase = (byte)max(0,min(nPhase,255));
	}
	bool operator==(CParamElement &pe)
	{
		return lKey == pe.lKey && bExtended == pe.bExtended && (!bExtended ||
			AddInfo.byPane == pe.AddInfo.byPane && AddInfo.byPhase && pe.AddInfo.byPhase);
	}
};


class CUserManualMeasParam
{
	ICalcExpressionPtr	m_sptrCalcExpr;

public:
	CUserManualMeasParam( )
	{
		m_sptrCalcExpr = 0;
	}
	
	~CUserManualMeasParam( )
	{
		m_sptrCalcExpr = 0;
	}
    
    BOOL	Init( CString *pstrExpr )
	{
		if( !pstrExpr )
			return FALSE;

		if( m_sptrCalcExpr == 0 )
		{
			IUnknown *punk = 0;
			if( S_OK != ::CoCreateInstance( clsidCalculatingMachine, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
				return FALSE;
			m_sptrCalcExpr = punk;
			punk->Release( ); 
			punk = 0;
		}
		HRESULT hRes = S_OK;
		// set expr
		BSTR bstr = pstrExpr->AllocSysString( );
		hRes = m_sptrCalcExpr->SetString( bstr );
		if( bstr )
		{
			::SysFreeString( bstr ); bstr = 0;
		}
		if( hRes != S_OK )
			return FALSE;
		// parse
		bstr = 0;
		hRes = m_sptrCalcExpr->Parse( &bstr );
		if( bstr )
		{
			::SysFreeString( bstr );bstr = 0;
		}
		return (hRes == S_OK);
	}

	BOOL	ReturnNecessaryCalcParams( BSTR *pbstrParamsNames, long *plSize )
	{
		if( m_sptrCalcExpr == 0 )
			return FALSE;
		return (m_sptrCalcExpr->ReturnNecessaryCalcParams( pbstrParamsNames, plSize ) == S_OK);
	}

	BOOL	Calculate( double *pfResult, BSTR *pbstrErrDesc )
	{
		if( m_sptrCalcExpr == 0 )
			return FALSE;
		return ( m_sptrCalcExpr->Calculate( pfResult, pbstrErrDesc ) == S_OK );
	}

	BOOL	SetParamsValues( double *pfValues, long	lSize )
	{
        if( m_sptrCalcExpr == 0 )
			return FALSE;
		return (m_sptrCalcExpr->SetParamsValues( pfValues, lSize ) == S_OK );
	}

};

inline void UserManualMeasParam_ex_free( void* pdata )
{
	delete (CUserManualMeasParam *)pdata;
}


class CUserManualMeasGroup :
	public CMeasParamGroupBase
{
	DECLARE_DYNCREATE(CUserManualMeasGroup)
	DECLARE_GROUP_OLECREATE(CUserManualMeasGroup);
public:
	CUserManualMeasGroup(void);
	virtual ~CUserManualMeasGroup(void);


private:
	_list_map_t<CUserManualMeasParam*, long, cmp_long, UserManualMeasParam_ex_free> m_mapKeyToParam;
	_list_map_t< double, long, cmp_long> m_lCalculatedKeys;

	// [vanek] - 20.03.2005
	bool	m_bcalc_in_curr_units;

	bool	get_string_by_key( long lKey, CString *str, char *sEntryName );
	bool	calc_value( ICalcObject2Ptr	&ptrCalc, ICalcObjectContainerPtr &ptrCalcCont, _list_t<CParamElement> &lCalculateKeys, double *pdResult );

	CPtrArray m_arrZombies;

public:
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff );
	virtual bool LoadCreateParam();
	virtual bool OnInitCalculation();
	virtual	bool ReloadState();

    //IPersist
	virtual void GetClassID( CLSID* pClassID );	

	// IPriority
	com_call GetPriority(long *plPriority);
};

inline long GetValueIntWithoutSet( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	try
	{
		_variant_t	var( lDefault );
		if( !CheckInterface( punkDoc, IID_INamedData ) )
			return var;

		_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

		sptrINamedData	sptrData( punkDoc );
		sptrData->SetupSection( bstrSection );
		
		long lEntriesCount = 0;
		sptrData->GetEntriesCount( &lEntriesCount );
        sptrData->GetValue( bstrEntry, &var );
		long lNewEntriesCount = 0;
		sptrData->GetEntriesCount( &lNewEntriesCount );
		if( lNewEntriesCount > lEntriesCount )
			sptrData->DeleteEntry( bstrEntry );

		_VarChangeType( var, VT_I4 );
		return var.lVal;
	}
	catch( ... )
	{
		return lDefault;
	}
}