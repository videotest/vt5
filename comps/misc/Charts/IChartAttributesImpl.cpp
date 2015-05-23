#include "StdAfx.h"
#include "ichartattributesimpl.h"

/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetColor( DWORD lColor, CommonChartColors dwFlag )
{
	if( dwFlag & ccfBackColor )
		m_clBackColor = lColor;
	if( dwFlag & ccfTargetColor )
		m_clTargetColor = lColor;
	if( dwFlag & ccfGridColor )
		m_clGridColor = lColor;

	return S_OK;
}
HRESULT IChartAttributesImpl::GetColor( DWORD *lColor, CommonChartColors dwFlag )
{
	if( !lColor )
		return S_FALSE;

	if( dwFlag & ccfBackColor )
		*lColor = m_clBackColor;
	else if( dwFlag & ccfTargetColor )
		*lColor = m_clTargetColor;
	else if( dwFlag & ccfGridColor )
		*lColor = m_clGridColor;

	return S_OK;
}
//<--------------------------------------------------------------<<<
HRESULT IChartAttributesImpl::SetChartColor( long nID, DWORD lColor, ChartColors dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & ccfChartColor )
		Chart.clChartColor = lColor;
	if( dwFlag & ccfAxisColor )
		Chart.clAxisColor = lColor;
	if( dwFlag & ccfAxisText )
		Chart.clTextColor = lColor;
	if( dwFlag & ccfGistFillColor )
		Chart.clGistBack = lColor;
	if( dwFlag & ccfHintTextColor )
		Chart.clHistTextColor = lColor;
	
	return S_OK;
}
HRESULT IChartAttributesImpl::GetChartColor(long nID, DWORD *lColor, ChartColors dwFlag )
{
	if( !lColor )
		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & ccfChartColor )
		*lColor = Chart.clChartColor;
	else if( dwFlag & ccfAxisColor )
		*lColor = Chart.clAxisColor;
	else if( dwFlag & ccfAxisText )
		*lColor = Chart.clTextColor;
	else if( dwFlag & ccfGistFillColor )
		*lColor = Chart.clGistBack;
	else if( dwFlag & ccfHintTextColor )
		*lColor = Chart.clHistTextColor;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetStyle( CommonChartViewParam dwFlag )
{
	m_bFillBack   = ( dwFlag & cfBack ) == cfBack;
	m_bTargetVert = ( dwFlag & cfTargetVert ) == cfTargetVert;
	m_bTargetHorz = ( dwFlag & cfTargetHorz ) == cfTargetHorz;

	m_fRotAngle = 0;
	if( dwFlag & cf90CW )
		m_fRotAngle += 90;
	if( dwFlag & cf90CCW )
		m_fRotAngle -= 90;
	if( dwFlag & cf180CW )
		m_fRotAngle += 180;
	if( dwFlag & cf180CCW )
		m_fRotAngle -= 180;

	m_bAxisOnBorder = ( dwFlag & cfAxisOnBorder ) == cfAxisOnBorder;
	m_bAbsolute = ( dwFlag & cfAbsolute ) == cfAbsolute;

	return S_OK;
}
HRESULT IChartAttributesImpl::GetStyle( CommonChartViewParam *dwFlag )
{
	DWORD dw = 0;
	
	if( m_bFillBack )
		dw |= cfBack;
	if( m_bTargetVert )
		dw |= cfTargetVert;
	if( m_bTargetHorz )
		dw |= cfTargetHorz;
	*dwFlag=CommonChartViewParam(dw);
	return S_OK;
}
//<--------------------------------------------------------------<<<
HRESULT IChartAttributesImpl::SetChartStyle(long nID, ChartViewParam dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	Chart.bAxisX	  = ( dwFlag & cfAxisX ) == cfAxisX;
	Chart.bAxisY	  = ( dwFlag & cfAxisY ) == cfAxisY;
	Chart.bAxisArrowX = ( dwFlag & cfAxisArrowX ) == cfAxisArrowX;
	Chart.bAxisArrowY = ( dwFlag & cfAxisArrowY ) == cfAxisArrowY;
	Chart.bAxisNameX  = ( dwFlag & cfAxisNameX ) == cfAxisNameX;
	Chart.bAxisNameY  = ( dwFlag & cfAxisNameY ) == cfAxisNameY;

	Chart.bAxisDigitsX = ( dwFlag & cfAxisDigitsX ) == cfAxisDigitsX;
	Chart.bAxisDigitsY = ( dwFlag & cfAxisDigitsY ) == cfAxisDigitsY;

	Chart.bAxisScaleX = ( dwFlag & cfAxisScaleX ) == cfAxisScaleX;
	Chart.bAxisScaleY = ( dwFlag & cfAxisScaleY ) == cfAxisScaleY;
	Chart.bHatch = ( dwFlag & cfHatch ) == cfHatch;
	return S_OK;
}
HRESULT IChartAttributesImpl::GetChartStyle(long nID, ChartViewParam *dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	DWORD dw = 0;
	
	if( Chart.bAxisX )
		dw |= cfAxisX;
	if( Chart.bAxisY )
		dw |= cfAxisY;
	if( Chart.bAxisArrowX )
		dw |= cfAxisArrowX;
	if( Chart.bAxisArrowY )
		dw |= cfAxisArrowY;
	if( Chart.bAxisNameX )
		dw |= cfAxisNameX;
	if( Chart.bAxisNameY )
		dw |= cfAxisNameY;

	if( Chart.bAxisDigitsX )
		dw |= cfAxisDigitsX;
	if( Chart.bAxisDigitsY )
		dw |= cfAxisDigitsY;

	if( Chart.bAxisScaleX )
		dw |= cfAxisScaleX;
	if( Chart.bAxisScaleY )
		dw |= cfAxisScaleY;
	if( Chart.bHatch  )
		dw |= cfHatch;

	*dwFlag=ChartViewParam(dw);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetLineStyle( ULONG lStyle, CommonChartLineStyles dwFlag )
{
	if( dwFlag & clfTargetStyle )
		m_lTargetStyle = lStyle;
//	if....
	return S_OK;
}
HRESULT IChartAttributesImpl::GetLineStyle( ULONG *lStyle, CommonChartLineStyles dwFlag )
{
	if( !lStyle )
		return S_FALSE;
	if( dwFlag & clfTargetStyle )
		*lStyle = m_lTargetStyle;
//	else if....
	return S_OK;
}
//<--------------------------------------------------------------<<<
HRESULT IChartAttributesImpl::SetChartLineStyle(long nID, ULONG lStyle, ChartLineStyles dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & clfChartStyle )
		Chart.lChartStyle = lStyle;
	if( dwFlag & clfAxisStyle )
		Chart.lAxisStyle = lStyle;

	return S_OK;
}
HRESULT IChartAttributesImpl::GetChartLineStyle(long nID, ULONG *lStyle, ChartLineStyles dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & clfChartStyle )
		*lStyle = Chart.lChartStyle;
	else if( dwFlag & clfAxisStyle )
		*lStyle = Chart.lAxisStyle;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetLineThick( long nThick, CommonChartLineThickness dwFlag )
{
	if( dwFlag & ctfTargetThick )
		m_nTargetThick = nThick;
//	if....
	return S_OK;
}
HRESULT IChartAttributesImpl::GetLineThick( long *nThick, CommonChartLineThickness dwFlag )
{
	if( !nThick )
		return S_FALSE;
	if( dwFlag & ctfTargetThick )
		*nThick = m_nTargetThick;
//	else if....
	return S_OK;
}
//<--------------------------------------------------------------<<<
HRESULT IChartAttributesImpl::SetChartLineThick(long nID, long nThick,   ChartLineThickness dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	if( dwFlag & ctfChartThick )
		Chart.nChartThick = nThick;
	if( dwFlag & ctfAxisThick )
		Chart.nAxisThick = nThick;
	return S_OK;
}
HRESULT IChartAttributesImpl::GetChartLineThick (long nID, long *nThick, ChartLineThickness dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( !nThick )
		return S_FALSE;

	if( dwFlag & ctfChartThick )
		*nThick = Chart.nChartThick;
	else if( dwFlag & ctfAxisThick )
		*nThick = Chart.nAxisThick;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetChartRange( long nID, double lfMin, double lfMax, ChartRange dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	if( dwFlag & crfRangeX )
	{
		Chart.lfMinX = lfMin;
		Chart.lfMaxX = lfMax;
	}
	else if( dwFlag & crfRangeY )
	{
		Chart.lfMinY = lfMin;
		Chart.lfMaxY = lfMax;
	}
	return S_OK;
}
HRESULT IChartAttributesImpl::GetChartRange( long nID, double *lfMin, double *lfMax, ChartRange dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	if( !lfMin || !lfMax )
		return S_FALSE;

	if( dwFlag & crfRangeX )
	{
		*lfMin = Chart.lfMinX;
		*lfMax = Chart.lfMaxX;
	}
	else if( dwFlag & crfRangeY )
	{
		*lfMin = Chart.lfMinY;
		*lfMax = Chart.lfMaxY;
	}

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetRangeState( long nID, int bEnable, ChartRange dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( dwFlag & crfRangeX )
		Chart.bEnableRangeX = bEnable != 0 ? true : false;
	if( dwFlag & crfRangeY )
		Chart.bEnableRangeY = bEnable != 0 ? true : false;
	return S_OK;
}
HRESULT IChartAttributesImpl::GetRangeState( long nID, int *bEnable, ChartRange dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( !bEnable )
		return S_FALSE;

	if( dwFlag & crfRangeX )
		*bEnable = Chart.bEnableRangeX;
	else if( dwFlag & crfRangeY )
		*bEnable = Chart.bEnableRangeY;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetTargetPos( POINT ptPos )
{
	m_ptTarget = ptPos;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetTextFormat( long nID, BSTR bstrFormat, ChartTextFormat dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	if( !bstrFormat )
		return S_FALSE;

	if( dwFlag & cffDigitsX )
	{
		//if( Chart.bstrFormatDigitsX )
		//	::SysFreeString( Chart.bstrFormatDigitsX );
		Chart.bstrFormatDigitsX = bstrFormat;
	}
	if( dwFlag & cffDigitsY )
	{
		//if( Chart.bstrFormatDigitsY )
		//	::SysFreeString( Chart.bstrFormatDigitsY );
		Chart.bstrFormatDigitsY = bstrFormat;
	}
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::RemoveChart( long nID )
{
	XChartItem &Chart = m_Charts[nID];

	m_Charts.erase( nID ) ;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::ShowChart( long nID, int bShow )
{
	XChartItem &Chart = m_Charts[nID];
	Chart.bShow = bShow != 0 ? true : false;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetAxisText( long nID, BSTR bstrText, ChartAxis dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( !bstrText )
		return S_FALSE;

	if( dwFlag & cafAxisX )
	{
		//if( Chart.bstrNameX )
		//	::SysFreeString( Chart.bstrNameX );
		Chart.bstrNameX = bstrText;
	}
	else if( dwFlag & cafAxisY )
	{
		//if( Chart.bstrNameY )
		//	::SysFreeString( Chart.bstrNameY );
		Chart.bstrNameY = bstrText;
	}
	return S_OK;
}
HRESULT IChartAttributesImpl::GetAxisText( long nID, BSTR *bstrText, ChartAxis dwFlag )
{
	XChartItem &Chart = m_Charts[nID];

	if( !bstrText )
		return S_FALSE;

	if( dwFlag & cafAxisX )
		*bstrText = Chart.bstrNameX.AllocSysString();
	else if( dwFlag & cafAxisY )
		*bstrText = Chart.bstrNameY.AllocSysString();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetConstParams( long nVal, CommonChartConst dwFlag )
{
	if( dwFlag & ccfScaleDirectionX )
		m_nScaleDirectionX = nVal;
	if( dwFlag & ccfScaleDirectionY )
		m_nScaleDirectionY = nVal;
	if( dwFlag & ccfAxisOffsetX )
		m_nAxisOffsetX = nVal;
	if( dwFlag & ccfAxisOffsetY )
		m_nAxisOffsetY = nVal;
	if( dwFlag & ccfArrowLength )
		m_nArrowLength = nVal;
	if( dwFlag & ccfScaleHalfSize )
		m_nScaleHalfSize = nVal;
	if( dwFlag & ccfScaleCountX )
		m_nScaleCountX = nVal;
	if( dwFlag & ccfScaleCountY )
		m_nScaleCountY = nVal;
	if( dwFlag & ccfDigitCountX )
		m_nDigitCountX = nVal;
	if( dwFlag & ccfDigitCountY )
		m_nDigitCountY = nVal;
	if( dwFlag & ccfLBorder )
		m_nLBorder = nVal;
	if( dwFlag & ccfRBorder )
		m_nRBorder = nVal;
	if( dwFlag & ccfTBorder )
		m_nTBorder = nVal;
	if( dwFlag & ccfBBorder )
		m_nBBorder = nVal;

	if( dwFlag & ccfTextOffsetX )
		m_nDigitOffsetX = nVal;
	if( dwFlag & ccfTextOffsetY )
		m_nDigitOffsetY = nVal;

	return S_OK;
}
HRESULT IChartAttributesImpl::GetConstParams( long *nVal, CommonChartConst dwFlag )
{
	if( dwFlag & ccfScaleDirectionX )
		*nVal = m_nScaleDirectionX;
	else if( dwFlag & ccfScaleDirectionY )
		*nVal = m_nScaleDirectionY;
	else if( dwFlag & ccfAxisOffsetX )
		*nVal = m_nAxisOffsetX;
	else if( dwFlag & ccfAxisOffsetY )
		*nVal = m_nAxisOffsetY;
	else if( dwFlag & ccfArrowLength )
		*nVal = m_nArrowLength;
	else if( dwFlag & ccfScaleHalfSize )
		*nVal = m_nScaleHalfSize;
	else if( dwFlag & ccfScaleCountX )
		*nVal = m_nScaleCountX;
	else if( dwFlag & ccfScaleCountY )
		*nVal = m_nScaleCountY;
	else if( dwFlag & ccfDigitCountX )
		*nVal = m_nDigitCountX;
	else if( dwFlag & ccfDigitCountY )
		*nVal = m_nDigitCountY;
	else if( dwFlag & ccfLBorder ){
		if(-1==m_nLBorder)
			*nVal= m_xShiftYAxis;
		else
			*nVal = m_nLBorder;
	}else if( dwFlag & ccfRBorder )
		*nVal = m_nRBorder;
	else if( dwFlag & ccfTBorder )
		*nVal = m_nTBorder;
	else if( dwFlag & ccfBBorder )
		*nVal = m_nBBorder;
	else if( dwFlag & ccfTextOffsetX )
		*nVal = m_nDigitOffsetX;
	else if( dwFlag & ccfTextOffsetY )
		*nVal = m_nDigitOffsetY;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetColorizedMap( long nID, DWORD lColor, double lfMinX, double lfMinY, double lfMaxX, double lfMaxY )
{
	XChartItem &Chart = m_Charts[nID];
	Chart.ColorizedMap.Set( &m_Charts[nID], lColor, lfMinX, lfMinY, lfMaxX, lfMaxY );
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::ClearColorized(  long nID )
{
	XChartItem &Chart = m_Charts[nID];
	Chart.ColorizedMap.Clear();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::EnableGridLines( CommonChartGrid dwFlags )
{
	m_dwGridFlags = dwFlags;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetGridLines( double *fGridVals, long lCount, CommonChartGrid dwFlags )
{
	if( !fGridVals || !lCount )
		return S_FALSE;

	if( !( dwFlags & ccgXGrid || dwFlags & ccgYGrid ) )
		return S_FALSE;

	if( dwFlags & ccgXGrid )
	{
		if( m_pfXGridVals )
			delete []m_pfXGridVals;
		m_pfXGridVals = 0;

		m_pfXGridVals = new double[lCount];
		::CopyMemory( (void *)m_pfXGridVals, (void*)fGridVals, sizeof( double ) * lCount );
		m_lGridValsCountX = lCount;
	}

	if( dwFlags & ccgYGrid )
	{
		if( m_pfYGridVals )
			delete []m_pfYGridVals;
		m_pfYGridVals = 0;

		m_pfYGridVals = new double[lCount];
		::CopyMemory( (void *)m_pfYGridVals, (void*)fGridVals, sizeof( double ) * lCount );
		m_lGridValsCountY = lCount;
	}

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
HRESULT IChartAttributesImpl::SetScaleLabels( long nID, double *pfVals, long lCount, ChartAxis dwFlags )
{
	if( !pfVals || dwFlags == 0 )
		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	if( dwFlags & cafAxisX )
	{
		if( Chart.m_pfXAxisLabels )
			delete []Chart.m_pfXAxisLabels;
		Chart.m_pfXAxisLabels = 0;

		Chart.m_lAxisLabelsCountX = lCount;

		Chart.m_pfXAxisLabels = new double[lCount];
		::CopyMemory( Chart.m_pfXAxisLabels, pfVals, sizeof( double ) * lCount );
	}

	if( dwFlags & cafAxisY )
	{
		if( Chart.m_pfYAxisLabels )
			delete []Chart.m_pfYAxisLabels;
		Chart.m_pfYAxisLabels = 0;

		Chart.m_lAxisLabelsCountY = lCount;

		Chart.m_pfYAxisLabels = new double[lCount];
		::CopyMemory( Chart.m_pfYAxisLabels, pfVals, sizeof( double ) * lCount );
	}

	return S_OK;
}

HRESULT IChartAttributesImpl::SetScaleTextLabels( long nID, double *pfVals, double *pfLabels, long lCount, ChartAxis dwFlags )
{
	if( !pfVals || dwFlags == 0 )
 		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	if( dwFlags & cafAxisX )
	{
		if( Chart.m_pfXAxisValues )
			delete []Chart.m_pfXAxisValues;
		Chart.m_pfXAxisValues = 0;

		if( Chart.m_pfXAxisLabelsValues )
			delete []Chart.m_pfXAxisLabelsValues;
		Chart.m_pfXAxisLabelsValues = 0;
		
		Chart.m_lAxisValuesCountX = lCount;

		Chart.m_pfXAxisValues = new double[lCount];
		::CopyMemory( Chart.m_pfXAxisValues, pfVals, sizeof( double ) * lCount );

		Chart.m_pfXAxisLabelsValues = new double[lCount];
		::CopyMemory( Chart.m_pfXAxisLabelsValues, pfLabels, sizeof( double ) * lCount );
	}

	if( dwFlags & cafAxisY )
	{
		if( Chart.m_pfYAxisValues )
			delete []Chart.m_pfYAxisValues;
		Chart.m_pfYAxisValues = 0;

		
		if( Chart.m_pfYAxisLabelsValues )
			delete []Chart.m_pfYAxisLabelsValues;
		Chart.m_pfYAxisLabelsValues = 0;

		Chart.m_lAxisValuesCountY = lCount;

		Chart.m_pfYAxisValues = new double[lCount];
		::CopyMemory( Chart.m_pfYAxisValues, pfVals, sizeof( double ) * lCount );

		Chart.m_pfYAxisLabelsValues = new double[lCount];
		::CopyMemory( Chart.m_pfYAxisLabelsValues, pfLabels, sizeof( double ) * lCount );
	}

	return S_OK;
}

HRESULT IChartAttributesImpl::SetAxisTextFont( long nID, BYTE *plfFont, ChartAxis dwFlags, ChartFont dwFlag2 )
{
	if( !plfFont )
		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	if( dwFlags & cafAxisX )
	{
		if( dwFlags & cfnDigit )
		{
			Chart.m_bAxisFontX = true;
			::CopyMemory( &Chart.m_lfAxisX, (LOGFONT *)plfFont, sizeof( LOGFONT ) );
		}
		
		if( dwFlags & cfnAxisName )
		{
			Chart.m_bAxisNameFontX = true;
			::CopyMemory( &Chart.m_lfAxisNameX, (LOGFONT *)plfFont, sizeof( LOGFONT ) );
		}
	}
	
	if( dwFlags & cafAxisY )
	{
		if( dwFlags & cfnDigit )
		{
			Chart.m_bAxisFontY = true;
			::CopyMemory( &Chart.m_lfAxisY, (LOGFONT *)plfFont, sizeof( LOGFONT ) );
		}

		if( dwFlags & cfnAxisName )
		{
			Chart.m_bAxisNameFontY = true;
			::CopyMemory( &Chart.m_lfAxisNameY, (LOGFONT *)plfFont, sizeof( LOGFONT ) );
		}
	}
	return S_OK;
}

HRESULT IChartAttributesImpl::SetHintText( long nID, double fX, double fY, BSTR bstrHint, int nAlign, int XOffsetInPixel, int YOffsetInPixel )
{
	if( !bstrHint )
		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	XHintText text;
	text.fX = fX;
	text.fY = fY;
	text.m_bstrHintText = ::SysAllocString( bstrHint );
	text.nAlign = nAlign;
	text.xOffset = XOffsetInPixel;
	text.yOffset = YOffsetInPixel;

	Chart.m_vectHints.push_back( text );

	return S_OK;
}

HRESULT IChartAttributesImpl::RemoveHints(/* [in] */long nID )
{
	XChartItem &Chart = m_Charts[nID];
	Chart.m_vectHints.clear();
	return S_OK;
}

HRESULT IChartAttributesImpl::SetHintFont( long nID, BYTE *plfFont )
{
	if( !plfFont )
		return S_FALSE;

	XChartItem &Chart = m_Charts[nID];

	Chart.m_lfHint = *( ( LOGFONT *)plfFont );

	return S_OK;
}

HRESULT IChartAttributesImpl::SetTextZoom( double fZoom )
{
	m_fTextZoom = fZoom;
	return S_OK;
}

HRESULT IChartAttributesImpl::GetTextZoom( double *pfZoom )
{
	if( !pfZoom )
		return S_FALSE;

	*pfZoom = m_fTextZoom;

	return S_OK;
}

HRESULT IChartAttributesImpl::SetPixelUnit( int nUnit )
{
	m_nMode = nUnit;
	return S_OK;
}
