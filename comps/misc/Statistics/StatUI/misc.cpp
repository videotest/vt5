#include "StdAfx.h"
#include "misc.h"
#include "statui_int.h"

bool SetModifiedFlagToObj( IUnknown* punkObj )
{
	INamedDataObject2Ptr ptrNDO( punkObj );

	if( ptrNDO == 0 )
		return false;

	ptrNDO->SetModifiedFlag( TRUE );

	//set modified flag to doc
	IUnknown* punkDoc = 0;
	ptrNDO->GetData( &punkDoc );
	if( punkDoc )
	{
		IFileDataObject2Ptr ptrFDO2 = punkDoc;
		punkDoc->Release();	punkDoc = 0;

		if( ptrFDO2 )
		{
			ptrFDO2->SetModifiedFlag( TRUE );
		}
	}

	return true;
}

CString _get_invisibles( INamedDataPtr sptrData, bool bLegend )
{
	CString ret;

	sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );

	long lCount = 0;
	sptrData->GetEntriesCount( &lCount );

	if( lCount == 0 )
		return "";

	long lParamsCount = lCount;

	for( long i = 0; i < lParamsCount; i++ )
	{
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );
		_bstr_t bstrName;
		sptrData->GetEntryName( i, bstrName.GetAddress() );
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) + "\\" + bstrName );
		
		long lCount = 0;
		sptrData->GetEntriesCount( &lCount );

		if( lCount == 0 )
			continue;

		_variant_t varVis = 3L;
		sptrData->GetValue( _bstr_t( PARAM_VISIBILITY ), &varVis );

		if( (long)varVis == 0L )
			ret += bstrName + CString( ", " );
		else if( bLegend && (long)varVis == 2L )
			ret += bstrName + CString( ", " );
		else if( !bLegend && (long)varVis == 1L )
			ret += bstrName + CString( ", " );
	}

	if( !ret.IsEmpty() )
		ret = ret.Left( ret.GetLength() - 2 );

	return ret;
}

long _get_param_order_by_key(INamedDataPtr sptrData, long lKey, bool bLegend  )
{
	sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );

	long lCount = 0;
	sptrData->GetEntriesCount( &lCount );

	if( lCount == 0 )
		return 0;

	long lParamsCount = lCount;

	for( long i = 0; i < lParamsCount; i++ )
	{
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );
		CString strName;
		strName.Format( "\\%ld", lKey );
		
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT + strName ) );
		
		long lCount = 0;
		sptrData->GetEntriesCount( &lCount );

		if( lCount == 0 )
			continue;

		_variant_t var = 0L;

		if( !bLegend )
			sptrData->GetValue( _bstr_t( PARAM_TABLE_ORDER ), &var );
		else
			sptrData->GetValue( _bstr_t( PARAM_LEGEND_ORDER ), &var );

		return (long)var;

	}
	return 0;
}
