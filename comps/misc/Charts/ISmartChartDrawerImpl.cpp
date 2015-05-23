#include "StdAfx.h"
#include "ismartchartdrawerimpl.h"

HRESULT ISmartChartDrawerImpl::CalcValues( long nID, BSTR bstrExpr, double fMin, double fMax, double fStep )
{
	if( !bstrExpr )
		return S_FALSE;

   /*
	IUnknown *punkCalc = 0;

	::CoCreateInstance( CLSID_Calculator, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punkCalc );

	if( !punkCalc )
		return S_FALSE;

	m_sptrCalc = sptrCalc;
	ICalcPtr sptrCalc = punkCalc;
	punkCalc->Release();

	if( sptrCalc == 0 )
		return S_FALSE;

	sptrCalc->SetString( m_bstrExpr );
	*/

	long nCount = ( fMax - fMin ) / fStep; 

	double *xVal = new double[nCount];
	double *yVal = new double[nCount];

	for( long i = 0; i < nCount; i++ )
		xVal[i] = fMin + i * fStep;

	for( long i = 0; i < nCount; i++ )
	{
//		sptrCalc->SetParams( &xVal[i], 1 );
//		sptrCalc->Calc();

		double fVal = 0;
//		sptrCalc->GetValue( &fVal );

		yVal[i] = fVal;
	}


	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	data = XChartDataItem( xVal, yVal, nCount );
	for( long i = 0; i < nCount; i++ )
	{
		if( xVal[i] >  data.maxX )
			data.maxX = xVal[i];
		if( xVal[i] <  data.minX )
			data.minX = xVal[i];

		if( yVal[i] >  data.maxY )
			data.maxY = yVal[i];
		if( yVal[i] <  data.minY )
			data.minY = yVal[i];
	}

	delete []xVal;
	delete []yVal;
	Chart.bShow = true;

	return S_OK;
}
