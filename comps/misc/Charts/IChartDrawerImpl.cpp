#include "StdAfx.h"
#include "ichartdrawerimpl.h"
#include <math.h>

HRESULT IChartDrawerImpl::SetData(long nID, double*pValues, long nCount, ChartDataType dwFlag )
{
	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & cdfDataGist )
		Chart.bGistogr = true;

	if( dwFlag & cdfDataX )
	{
		data = XChartDataItem( pValues, 0, nCount );

		for( long i = 0; i < nCount; i++ )
		{
			if( pValues[i] >  data.maxX )
				data.maxX = pValues[i];
			if( pValues[i] <  data.minX )
				data.minX = pValues[i];
		}

		if( data.pValuesY )
			Chart.bShow = true;
	}
	else if( dwFlag & cdfDataY )
	{
		data = XChartDataItem( 0, pValues, nCount );
		for( long i = 0; i < nCount; i++ )
		{
			if( pValues[i] >  data.maxY )
				data.maxY = pValues[i];
			if( pValues[i] <  data.minY )
				data.minY = pValues[i];
		}

		if( data.pValuesX )
			Chart.bShow = true;
	}

	return S_OK;
}
HRESULT IChartDrawerImpl::SetData2(long nID, VARIANT PtrToFunct, double lfMinX, double lfMaxX, double lfStep )
{
	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	double(*Func)(double) = (double(*)(double))PtrToFunct.byref;

	data.nSz = int( ( lfMaxX - lfMinX ) / lfStep );

	double *pValuesX = new double[ data.nSz ];
	double *pValuesY = new double[ data.nSz ];

	long lCount = 0;
	for( double fStep = lfMinX; fStep <= lfMaxX; fStep += lfStep,lCount++ )
	{
		pValuesX[lCount] = fStep;
		pValuesY[lCount] = Func( pValuesX[lCount] );
	}

	data = XChartDataItem( pValuesX, pValuesY, data.nSz );

	for( long i = 0; i < data.nSz; i++ )
	{
		if( pValuesY[i] >  data.maxY )
			data.maxY = pValuesY[i];
		if( pValuesY[i] <  data.minY )
			data.minY = pValuesY[i];
	}

	Chart.bShow = true;


	delete []pValuesX;
	delete []pValuesY;

	data.minX = lfMinX;
	data.maxX = lfMaxX;

	return S_OK;
}
HRESULT IChartDrawerImpl::GetData(long nID, double**pValues, long *nCount, ChartDataType dwFlag )
{
	XChartDataItem &data = m_ChartsData[nID];
	if( !pValues || !nCount )
		return S_FALSE;

	*nCount = data.nSz;

	if( dwFlag & cdfDataX )
		*pValues = data.pValuesX;
	else if( dwFlag & cdfDataY )
		*pValues = data.pValuesY;

	return S_OK;
}

HRESULT IChartDrawerImpl::SetExternData(long nID, double*pValues, long nCount, ChartDataType dwFlag )
{
	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & cdfDataX )
	{
		data.bExternX = true;
		data.pValuesX = pValues;
		for( long i = 0; i < nCount; i++ )
		{
			if( pValues[i] >  data.maxX )
				data.maxX = pValues[i];
			if( pValues[i] <  data.minX )
				data.minX = pValues[i];
		}
		Chart.bShow = true;
	}
	else if( dwFlag & cdfDataY )
	{
		data.bExternY = true;
		data.pValuesY = pValues;

		for( long i = 0; i < nCount; i++ )
		{
			if( pValues[i] >  data.maxY )
				data.maxY = pValues[i];
			if( pValues[i] <  data.minY )
				data.minY = pValues[i];
		}
		Chart.bShow = true;
	}

	return S_OK;
}
