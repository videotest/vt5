#pragma once

#include <atlstr.h>
bool SetModifiedFlagToObj( IUnknown* punkObj );

template<typename TData> static void sort_templ( TData *pArr, long lSz, unsigned uiFlag )
{
	for( long i = 0; i < lSz; i++ )
	{
		long lID = -1;
		TData Extr = pArr[i];

		for( long j = i; j < lSz; j++ )
		{
			if( uiFlag == 0 ) // Desc
			{
				if( pArr[j] > Extr )
				{
					Extr = pArr[j];
					lID = j;
				}
			}
			else if( uiFlag == 1 ) // Asc
			{
				if( pArr[j] < Extr )
				{
					Extr = pArr[j];
					lID = j;
				}
			}
		}
		
		if( lID != -1 )
		{
			TData tmp = pArr[i];
			pArr[i] = pArr[lID];
			pArr[lID] = tmp;
		}
	}
}

template<typename TData, typename TData2> static void sort_templ2( TData *pArr, TData2 *pParam, long lSz, unsigned uiFlag )
{
	for( long i = 0; i < lSz; i++ )
	{
		long lID = -1;
		TData2 Extr = pParam[i];

		for( long j = i; j < lSz; j++ )
		{
			if( uiFlag == 0 ) // Desc
			{
				if( pParam[j] > Extr )
				{
					Extr = pParam[j];
					lID = j;
				}
			}
			else if( uiFlag == 1 ) // Asc
			{
				if( pParam[j] < Extr )
				{
					Extr = pParam[j];
					lID = j;
				}
			}
		}
		
		if( lID != -1 )
		{
			TData tmp = pArr[i];
			pArr[i] = pArr[lID];
			pArr[lID] = tmp;

			TData2 tmp2 = pParam[i];
			pParam[i] = pParam[lID];
			pParam[lID] = tmp2;
		}
	}
}

CString _get_invisibles( INamedDataPtr sptrND, bool bLegend );
long _get_param_order_by_key( INamedDataPtr sptrData,long lKey, bool bLegend  );
