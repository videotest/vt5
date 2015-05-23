#include "StdAfx.h"
#include "chartdrawer.h"
//#include "misc_str.h"

CGDIPlusManager *CChartDrawer::m_pGDIPlusManager = 0;
long CChartDrawer::m_nChartsCounter = 0;

double Canonic(double x1, double y1,double x2, double y2,double x, double y,int sign)
{
	if(x1 != x2 && y1 != y2)
		return sign*x*(y2-y1)/(x2-x1) + sign*(- x1*(y2-y1)/(x2-x1) + y1) - sign*y;
	else if(x1 == x2 && y1 != y2)
	{
		if(y >= y1 && y <= y2 )
			return 0;
		else 
			return 100;
	}
	else if(x1 != x2 && y1 == y2)
	{
		if(x >= x1 && x <= x2 )
			return 0;
		else 
			return 100;
	}
	return 0;
} 

double CanonicY(double x1, double y1,double x2, double y2,double x )
{
	if(x1 != x2 )
	{
		double a = ( y2 - y1 ) / ( x2 - x1 );
		return a * ( x - x1 ) + y1;
	}
	return 0;
} 


CChartDrawer::CChartDrawer(void)
{
	if( !m_pGDIPlusManager )
		m_pGDIPlusManager = new CGDIPlusManager;

	m_nChartsCounter++;
	do_default();
}

void CChartDrawer::do_default( bool bConst )
{
	m_lfMinX = m_lfMinY = 1e308;
	m_lfMaxX = m_lfMaxY = -1e307;

	m_ptOffset.x =  m_ptOffset.y = 0;
	m_bDrawHint = false;
	m_lDrawingFlag = 3;
	m_YDigitsWidth=0;
}

CChartDrawer::~CChartDrawer(void)
{
	m_nChartsCounter--;

	if( m_pGDIPlusManager && !m_nChartsCounter )
	{
		delete m_pGDIPlusManager;
		m_pGDIPlusManager = 0;
	}
}
namespace{
	double RoundEx(double Value)
	{
		int n1, n2, i;
		bool negativ=false;
		double dRoundEx = Value;
		if(0.==Value)
			return Value;
		if(Value < 0.) { 
			Value = abs(Value);
			negativ = true;
		}
		n1 = 0.;
		while (Value < 1){
			Value = Value * 10.;
			n1 = n1 + 1;
		}
		n2 = 0;
		while (Value >= 10.){
			Value = Value / 10.;
			n2 = n2 + 1;
		}
		if(Value > 7.1){
			Value = 10.;
		}else if(Value > 3.2) {
			Value = 5.;
		}else if(Value > 1.4) {
			Value = 2.;
		}else{
			Value = 1.;
		}
		for(i = 1; i<= n1; ++i){
			Value = Value / 10.;
		}
		for(i = 1; i<= n2; ++i){
			Value = Value * 10.;
		}
		if(negativ) {
			dRoundEx = -Value;
		}else{
			dRoundEx = Value;
		}
		return dRoundEx;
	}
}

HRESULT CChartDrawer::Draw( DWORD hDC,  RECT rcDraw )
{
	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	Rect rcClip;

	rcClip.X = rcDraw.left;
	rcClip.Y = rcDraw.top;

	rcClip.Width  = rcDraw.right - rcDraw.left + 2;
	rcClip.Height = rcDraw.bottom - rcDraw.top + 2;

	g.SetClip( rcClip );


	if( m_fRotAngle == 90 || m_fRotAngle == 270 || m_fRotAngle == -90 || m_fRotAngle == -270 )
	{
		m_ptOffset.x = rcDraw.left;
		m_ptOffset.y = rcDraw.top;

		rcDraw.right -= rcDraw.left;
		rcDraw.bottom -= rcDraw.top;

		rcDraw.left = rcDraw.top = 0;

		RECT rc = rcDraw;
		rcDraw.right = rcDraw.left + rc.bottom - rc.top;
		rcDraw.bottom = rcDraw.top + rc.right - rc.left;
	}

	int nSrcW = rcDraw.right - rcDraw.left; 
	int nSrcH = rcDraw.bottom - rcDraw.top; 

	if( m_fRotAngle != 0 )
	{
		g.RotateTransform( m_fRotAngle, MatrixOrderAppend);
		if( m_fRotAngle == 90 || m_fRotAngle == -270 )
			g.TranslateTransform( m_ptOffset.y, -(nSrcH + m_ptOffset.x ) );
		if( m_fRotAngle == 180 || m_fRotAngle == -180 )
			g.TranslateTransform( -( nSrcW + m_ptOffset.y ), -( nSrcH + m_ptOffset.x )  );
		if( m_fRotAngle == 270 || m_fRotAngle == -90 )
			g.TranslateTransform( -( nSrcW + m_ptOffset.y ), m_ptOffset.x );
	}

	if( m_bFillBack )
		g.FillRectangle( &SolidBrush( Color( 0xFF, GetRValue( m_clBackColor ), GetGValue( m_clBackColor ), GetBValue( m_clBackColor ) ) ), rcDraw.left, rcDraw.top, nSrcW, nSrcH );

	bool bAxis = false;

	int nSz = (int)m_Charts.size();
	for( int i = 0; i < nSz; i++ )
	{
		if( m_Charts.ByIndex(i).bAxisX || m_Charts.ByIndex(i).bAxisY )
		{
			bAxis = true;
			break;
		}
	}


	RECT rc = rcDraw;
	m_rcOld = rc;

	if( bAxis ) {
		MeasureAxis( hDC, &rc );
	}
	m_xShiftYAxis=rc.left - rcDraw.left;

	m_rcCorrected = rc;

	m_lDrawingFlag = 1;
	for( i = 0; i < nSz; i++ )
		_draw_graph( hDC, m_Charts.GetKey(i),  rc );

	if( bAxis )
	{
		_draw_axis( hDC, &rcDraw, rc );
	}

	m_lDrawingFlag = 2;
	for( i = 0; i < nSz; i++ )
		_draw_graph( hDC, m_Charts.GetKey(i),  rc );

	for( i = 0; i < nSz; i++ )
		_draw_hint( hDC, m_Charts.GetKey(i),  rc );

	m_lDrawingFlag = 3;

	return S_OK;
}
HRESULT CChartDrawer::_draw_graph( DWORD hDC, long nID,  RECT rcDraw )
{
	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	if( !Chart.bShow )
		return S_FALSE;

	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	Rect rcClip;

	rcClip.X = rcDraw.left;
	rcClip.Y = rcDraw.top;

	rcClip.Width  = rcDraw.right - rcDraw.left + 2;
	rcClip.Height = rcDraw.bottom - rcDraw.top + 2;

	if( !m_bDrawHint )
		g.SetClip( rcClip );


	int nSrcW = rcDraw.right - rcDraw.left; 
	int nSrcH = rcDraw.bottom - rcDraw.top; 

	if( m_fRotAngle != 0 )
	{
		g.RotateTransform( m_fRotAngle, MatrixOrderAppend);
		if( m_fRotAngle == 90 || m_fRotAngle == -270 )
			g.TranslateTransform( m_ptOffset.y, -( m_rcOld.bottom - m_rcOld.top + m_ptOffset.x) );
		if( m_fRotAngle == 180 || m_fRotAngle == -180 )
			g.TranslateTransform( -( m_rcOld.right - m_rcOld.left + m_ptOffset.y ), -( m_rcOld.bottom - m_rcOld.top + m_ptOffset.x ) );
		if( m_fRotAngle == 270 || m_fRotAngle == -90 )
			g.TranslateTransform( -( m_rcOld.right - m_rcOld.left + m_ptOffset.y ), m_ptOffset.x );
	}

	double lfMinX, lfMaxX, lfMinY, lfMaxY;
	_calc_draw_range( nID, lfMinX, lfMaxX, lfMinY, lfMaxY );

	double ScaleX = nSrcW / ( lfMaxX - lfMinX );
	double ScaleY = nSrcH / ( lfMaxY - lfMinY );

	auto_ptr<Pen> pnChart;
	auto_ptr<Bitmap> brush(Chart.ColorizedMap.CreateBitmap( m_rcOld, rcDraw.left, rcDraw.top, nSrcW, nSrcH, ScaleX, ScaleY, lfMinX, lfMinY ));

	if( !brush.get() )
		pnChart.reset(new Pen( Color( 0xFF, GetRValue( Chart.clChartColor ), GetGValue( Chart.clChartColor ), GetBValue( Chart.clChartColor ) ), REAL( Chart.nChartThick ) ));
	else
		pnChart.reset(new Pen( &TextureBrush( (Image*)brush.get() ), REAL( Chart.nChartThick ) ));

	pnChart->SetDashStyle( DashStyle( Chart.lChartStyle ) );

	if( m_fRotAngle == 0 )
		g.TranslateTransform( rcDraw.left, rcDraw.top );

	if( !m_bDrawHint )
	{
		if( m_Charts.GetKey(0) == nID )
		{
			if( m_lDrawingFlag & 1 )
			{
				if( m_dwGridFlags & ccgXGrid )
				{
					Pen pn( Color( 0xFF, GetRValue( m_clGridColor ), GetGValue( m_clGridColor ), GetBValue( m_clGridColor ) ), REAL( 1 ) );

					for( long i = 0; i < m_lGridValsCountX; i++ )
					{
						double x = m_pfXGridVals[i];
						// строю грид лайнз
						if( x != 0 )
							g.DrawLine( &pn, INT( ScaleX * ( x - lfMinX ) ), 
							INT( nSrcH - ScaleY * ( 0 ) - 1 ),
							INT(  ScaleX * ( x - lfMinX ) ), 
							INT( nSrcH - ScaleY * ( lfMaxY - lfMinY ) + 1 ) );

					}

					if( m_dwGridFlags & ccgXEndGrid )
						g.DrawLine( &pn, INT( ScaleX * ( lfMaxX - lfMinX ) ), 
						INT( nSrcH - ScaleY * ( 0 ) - 1 ),
						INT(  ScaleX * ( lfMaxX - lfMinX ) ), 
						INT( nSrcH - ScaleY * ( lfMaxY - lfMinY ) + 1 ) );
				}

				if( m_dwGridFlags & ccgYGrid )
				{
					Pen pn( Color( 0xFF, GetRValue( m_clGridColor ), GetGValue( m_clGridColor ), GetBValue( m_clGridColor ) ), REAL( 1 ) );

					for( long i = 0; i < m_lGridValsCountY; i++ )
					{
						double y = m_pfYGridVals[i];
						// строю грид лайнз
						if( y != 0 )
							g.DrawLine( &pn, INT( ScaleX * ( 0 ) + 1 ), 
							INT( nSrcH - ScaleY * ( y - lfMinY ) ),
							INT(  ScaleX * ( lfMaxX - lfMinX ) - 1 ), 
							INT( nSrcH - ScaleY * ( y - lfMinY )) );

					}
					if( m_dwGridFlags & ccgYEndGrid )
						g.DrawLine( &pn, INT( ScaleX * ( 0 ) ), 
						INT( nSrcH - ScaleY * ( lfMaxY - lfMinY ) ),
						INT(  ScaleX * ( lfMaxX - lfMinX ) ), 
						INT( nSrcH - ScaleY * ( lfMaxY - lfMinY )) );
				}
			}

			if( !m_list_markers.empty() )
			{

				for( vector<x_markers>::iterator it = m_list_markers.begin(); it != m_list_markers.end(); it++ )
				{
					x_markers marker = *it;
					Pen marker_pn( Color( 0xFF, GetRValue( marker.color ), GetGValue( marker.color ), GetBValue( marker.color ) ), REAL( 1 ) );

					INT nx = INT( ScaleX * ( marker.x - lfMinX ) );
					INT ny = INT( nSrcH - ScaleY * ( marker.y - lfMinY ) );

					if( !marker.ltype )
					{
						g.DrawLine( &marker_pn,	long( nx - marker.lsz * m_fTextZoom ), 
							long( ny - marker.lsz * m_fTextZoom ),
							long( nx + marker.lsz * m_fTextZoom ), 
							long( ny + marker.lsz * m_fTextZoom ) );

						g.DrawLine( &marker_pn,	long( nx + marker.lsz * m_fTextZoom ), 
							long( ny - marker.lsz * m_fTextZoom ),
							long( nx - marker.lsz * m_fTextZoom ), 
							long( ny + marker.lsz * m_fTextZoom ) );
					}
					else if( marker.ltype == 1 )
						g.DrawRectangle( &marker_pn, long( nx - marker.lsz * m_fTextZoom ), long( ny - marker.lsz * m_fTextZoom ), long( 2 * marker.lsz * m_fTextZoom ), long( 2 * marker.lsz * m_fTextZoom ) );
					else if( marker.ltype == 2 )
						g.DrawEllipse( &marker_pn,	 long( nx - marker.lsz * m_fTextZoom ), long( ny - marker.lsz * m_fTextZoom), long( 2*marker.lsz * m_fTextZoom ), long( 2*marker.lsz * m_fTextZoom ) );

				}
			}
		}

		if( m_lDrawingFlag & 2 )
		{
			if( !Chart.bGistogr )
			{
				for( long i = 1; i < data.nSz; i++ )
				{
					g.DrawLine( pnChart.get(), INT( ScaleX * ( data.pValuesX[i-1] - lfMinX ) ), 
						INT( nSrcH - ScaleY * ( data.pValuesY[i-1] - lfMinY ) ),
						INT(  ScaleX * ( data.pValuesX[i] - lfMinX ) ), 
						INT( nSrcH - ScaleY * ( data.pValuesY[i] - lfMinY ) ) );

				}
			}
			else
			{
				Color white(255,255,255);
				Color black(0,0,0);

				for( long i = 0; i < data.nSz; i += 2 )
				{
					INT x1 = INT( ScaleX * ( data.pValuesX[i] - lfMinX ) );
					INT x2 = INT( fabs( ScaleX * ( data.pValuesX[i + 1] - data.pValuesX[i] ) ) );

					if( x1 <= 0 && x2 <= 0 )
						continue;

					Rect rect( x1, 
						INT( nSrcH - ScaleY * ( data.pValuesY[i] - lfMinY ) ),
						x2, 
						INT( fabs( ScaleY * ( data.pValuesY[i] - data.pValuesY[i + 1] ) ) ) );

					if(Chart.bHatch){
						g.FillRectangle( &HatchBrush( HatchStyle(Chart.clGistBack % HatchStyleMax),  black, white), rect );
					}else{
						g.FillRectangle( &SolidBrush( Color( 0xFF, GetRValue( Chart.clGistBack ), GetGValue( Chart.clGistBack ), GetBValue( Chart.clGistBack ) ) ), rect );
					}
					g.DrawRectangle( pnChart.get(), rect );
				}
			}
		}
	}
	if( m_bDrawHint )
	{
		for( int i = 0; i < Chart.m_vectHints.size();i++ )
		{
			if( Chart.m_vectHints[i].m_bstrHintText )
			{
				INT x = INT( ScaleX * ( Chart.m_vectHints[i].fX - lfMinX ) +  Chart.m_vectHints[i].xOffset );
				INT y = INT( nSrcH - ScaleY * ( Chart.m_vectHints[i].fY - lfMinY ) +  Chart.m_vectHints[i].yOffset );
				RectF pt_rect( REAL( x ), REAL( y ), 0.0f, 0.0f );

				LOGFONT lf = Chart.m_lfHint;
				lf.lfHeight *= m_fTextZoom;

				Font font( (HDC)hDC, &lf );

				StringFormat formatX;

				formatX.SetFormatFlags( StringFormatFlagsNoWrap );
				formatX.SetAlignment(   StringAlignmentNear );
				formatX.SetTrimming( StringTrimmingNone );

				RectF rcText;

				_bstr_t value = Chart.m_vectHints[i].m_bstrHintText;
				g.MeasureString( value, (int)value.length(), &font, pt_rect, &formatX, &rcText );

				if( Chart.m_vectHints[i].nAlign == 1 )
					formatX.SetAlignment( StringAlignmentCenter );
				else if( Chart.m_vectHints[i].nAlign == 2 )
					formatX.SetAlignment( StringAlignmentFar );

				g.DrawString( value, (int)value.length(), &font, pt_rect, &formatX, &SolidBrush( Color( 0xFF, GetRValue( Chart.clHistTextColor ), GetGValue( Chart.clHistTextColor ), GetBValue( Chart.clHistTextColor ) ) ) );
			}
		}
	}

	return S_OK;
}

HRESULT CChartDrawer::_draw_hint( DWORD hDC, long nID,  RECT rcDraw )
{
	m_bDrawHint = true;
	HRESULT hr = _draw_graph( hDC, nID, rcDraw );
	m_bDrawHint = false;
	return hr;
}

HRESULT CChartDrawer::MeasureAxis( DWORD hDC, RECT *rcDraw)
{
	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	Rect rcClip;

	rcClip.X = rcDraw->left;
	rcClip.Y = rcDraw->top;

	rcClip.Width  = rcDraw->right - rcDraw->left + 2;
	rcClip.Height = rcDraw->bottom - rcDraw->top + 2;

	g.SetClip( rcClip );

	if( m_fRotAngle != 0 )
	{
		g.RotateTransform( m_fRotAngle, MatrixOrderAppend);
		if( m_fRotAngle == 90 || m_fRotAngle == -270 )
			g.TranslateTransform( m_ptOffset.y, -( rcDraw->bottom - rcDraw->top + m_ptOffset.x ) );
		if( m_fRotAngle == 180 || m_fRotAngle == -180 )
			g.TranslateTransform( -( rcDraw->right - rcDraw->left + m_ptOffset.y ), -( rcDraw->bottom - rcDraw->top + m_ptOffset.x ) );
		if( m_fRotAngle == 270 || m_fRotAngle == -90 )
			g.TranslateTransform( -( rcDraw->right - rcDraw->left + m_ptOffset.y ), m_ptOffset.x );
	}

	StringFormat formatX, formatY;

	formatX.SetFormatFlags( StringFormatFlagsNoWrap );
	formatX.SetAlignment(   StringAlignmentCenter );
	formatX.SetTrimming( StringTrimmingNone );

	formatY.SetFormatFlags( StringFormatFlagsNoWrap );
	formatY.SetTrimming( StringTrimmingNone );
	formatY.SetAlignment(   StringAlignmentNear );

	// Calculate max width of rectangle values on y-axe.
	{
		if(-1==m_nLBorder) 
		{
			m_YDigitsWidth=0.;
			// Calculate max width of rectangle values on y-axe.
			int nSz = (int)m_ChartsData.size();
			int i = 0;
			{
				long nKey = m_ChartsData.GetKey(i);
				XChartItem &Chart = m_Charts[nKey];
				if( Chart.bAxisDigitsY  )
				{
					if( Chart.m_pfYAxisValues  )
					{
						for( long i = 0; i < Chart.m_lAxisValuesCountY; i++ )
						{
							double j = Chart.m_pfYAxisValues[i];

							CStringW value;
							if( Chart.bstrFormatDigitsY.GetLength()>0 )
								value.Format(Chart.bstrFormatDigitsY, Chart.m_pfYAxisLabelsValues[i] );
							else
							{
								value=_variant_t(Chart.m_pfYAxisLabelsValues[i]);
							}
							PointF pt( 0., 0.);

							Font *font = 0;

							if( Chart.m_bAxisFontY )
							{
								LOGFONT lf = Chart.m_lfAxisY;
								lf.lfHeight *= m_fTextZoom;
								font = new Font( (HDC)hDC, &lf );
							}
							else
								font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

							RectF rcText;
							g.MeasureString(  value, value.GetLength(), font, pt, &formatY, &rcText );
							m_YDigitsWidth=max(m_YDigitsWidth,rcText.Width);
							delete font;
						}
					}
					else
					{
						double lfMinX, lfMaxX, lfMinY, lfMaxY;
						_calc_draw_range( nKey, lfMinX, lfMaxX, lfMinY, lfMaxY );

						double h = ( lfMaxY - lfMinY ) / m_nDigitCountY;
						for( double j = lfMinY; j < lfMaxY; j += h )
						{
							CStringW value;
							if( Chart.bstrFormatDigitsY.GetLength()>0 )
								value.Format(Chart.bstrFormatDigitsY, j );
							else
							{
								value=_variant_t(j);
							}
							PointF pt( 0., 0.);
							RectF rcText;

							Font *font = 0;

							if( Chart.m_bAxisFontY )
							{
								LOGFONT lf = Chart.m_lfAxisY;
								lf.lfHeight *= m_fTextZoom;
								font = new Font( (HDC)hDC, &lf );
							}
							else
								font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

							g.MeasureString( value, value.GetLength(), font, pt, &formatY, &rcText );
							m_YDigitsWidth=max(m_YDigitsWidth,rcText.Width);
							delete font;
						}
					}
				}
			}
		}else
			m_YDigitsWidth=m_nLBorder;
	}

	rcDraw->left   += m_YDigitsWidth;
	rcDraw->right  -= m_nRBorder;

	rcDraw->top    += m_nTBorder;
	rcDraw->bottom -= m_nBBorder;

	if( m_bAxisOnBorder )
	{
		int nXPos =  0;
		int nYPos =  0;

		int nSz = (int)m_ChartsData.size();
		for( int i = 0; i < nSz; i++ )
		{
			long nKey = m_ChartsData.GetKey(i);
			XChartItem &Chart = m_Charts[nKey];

			if( !Chart.bAxisX && !Chart.bAxisY )
				continue;

			Pen pnAxis( Color( 0xFF, GetRValue( Chart.clAxisColor ), GetGValue( Chart.clAxisColor ), GetBValue( Chart.clAxisColor ) ), REAL( Chart.nAxisThick ) );
			pnAxis.SetDashStyle( DashStyle( Chart.lAxisStyle ) );
			Color br_color( 0xFF, GetRValue( Chart.clTextColor ), GetGValue( Chart.clTextColor ), GetBValue( Chart.clTextColor ) );

			if( Chart.bAxisX && ( !m_bAbsolute || m_bAbsolute && !i ) )
			{
				nXPos += m_nAxisOffsetX;
			}

			if( Chart.bAxisY && ( !m_bAbsolute || m_bAbsolute && !i ) )
			{
				nYPos += m_nAxisOffsetY;
			}
		}
		rcDraw->left   += (nYPos / m_nAxisOffsetY - 1) * m_nAxisOffsetY;
		rcDraw->bottom -= (nXPos / m_nAxisOffsetX - 1) * m_nAxisOffsetX;
	}
	return S_OK;
}

HRESULT CChartDrawer::_draw_axis( DWORD hDC, RECT *rcDraw, const RECT& rc )
{
	const LONG xOffs = rc.left, yOffs = rc.bottom;
	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	Rect rcClip;

	rcClip.X = rcDraw->left;
	rcClip.Y = rcDraw->top;

	rcClip.Width  = rcDraw->right - rcDraw->left + 2;
	rcClip.Height = rcDraw->bottom - rcDraw->top + 2;

	//	g.SetClip( rcClip ); Fixed 4457

	if( m_fRotAngle != 0 )
	{
		g.RotateTransform( m_fRotAngle, MatrixOrderAppend);
		if( m_fRotAngle == 90 || m_fRotAngle == -270 )
			g.TranslateTransform( m_ptOffset.y, -( rcDraw->bottom - rcDraw->top + m_ptOffset.x ) );
		if( m_fRotAngle == 180 || m_fRotAngle == -180 )
			g.TranslateTransform( -( rcDraw->right - rcDraw->left + m_ptOffset.y ), -( rcDraw->bottom - rcDraw->top + m_ptOffset.x ) );
		if( m_fRotAngle == 270 || m_fRotAngle == -90 )
			g.TranslateTransform( -( rcDraw->right - rcDraw->left + m_ptOffset.y ), m_ptOffset.x );
	}

	StringFormat formatX, formatY;

	formatX.SetFormatFlags( StringFormatFlagsNoWrap );
	formatX.SetAlignment(   StringAlignmentCenter );
	formatX.SetTrimming( StringTrimmingNone );

	formatY.SetFormatFlags( StringFormatFlagsNoWrap );
	formatY.SetTrimming( StringTrimmingNone );
	formatY.SetAlignment(StringAlignmentFar);

	if( m_bAxisOnBorder )
	{
		int nXPos =  0;
		int nYPos =  0;

		int nSz = (int)m_ChartsData.size();
		for( int i = 0; i < nSz; i++ )
		{
			long nKey = m_ChartsData.GetKey(i);
			XChartItem &Chart = m_Charts[nKey];

			if( !Chart.bAxisX && !Chart.bAxisY )
				continue;

			Pen pnAxis( Color( 0xFF, GetRValue( Chart.clAxisColor ), GetGValue( Chart.clAxisColor ), GetBValue( Chart.clAxisColor ) ), REAL( Chart.nAxisThick ) );
			pnAxis.SetDashStyle( DashStyle( Chart.lAxisStyle ) );
			Color br_color( 0xFF, GetRValue( Chart.clTextColor ), GetGValue( Chart.clTextColor ), GetBValue( Chart.clTextColor ) );

			if( Chart.bAxisX && ( !m_bAbsolute || m_bAbsolute && !i ) )
			{
				g.DrawLine( &pnAxis, INT( xOffs  ), INT( rc.bottom - nXPos ), 
					INT( rc.right ), INT( rc.bottom - nXPos ) );

				if( Chart.bAxisArrowX  )
				{
					g.DrawLine( &pnAxis, INT( rc.right ), INT( rc.bottom - nXPos ), 
						INT( rc.right - m_nArrowLength*Chart.nAxisThick ), INT( rc.bottom - nXPos - m_nArrowLength*Chart.nAxisThick / 2 ) );
					g.DrawLine( &pnAxis, INT( rc.right ), INT( rc.bottom - nXPos ), 
						INT( rc.right - m_nArrowLength*Chart.nAxisThick ), INT( rc.bottom - nXPos + m_nArrowLength*Chart.nAxisThick / 2 ) );
				}
				if( Chart.bAxisScaleX || Chart.bAxisDigitsX  )
				{
					double lfMinX, lfMaxX, lfMinY, lfMaxY;
					_calc_draw_range( nKey, lfMinX, lfMaxX, lfMinY, lfMaxY );

					double ScaleX = ( rc.right - rc.left ) / ( lfMaxX - lfMinX );

					if( Chart.bAxisScaleX )
					{
						if( Chart.m_pfXAxisLabels )
						{
							for( long i = 0; i < Chart.m_lAxisLabelsCountX; i++ )
							{
								double j = Chart.m_pfXAxisLabels[i];
								if(lfMinX <= j && j <= lfMaxX )
									g.DrawLine( &pnAxis, INT( xOffs + ScaleX*( j - lfMinX ) ), INT( rc.bottom - nXPos - ( m_nScaleDirectionX <= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), 
									INT( xOffs + ScaleX*( j - lfMinX ) ), INT( rc.bottom - nXPos + ( m_nScaleDirectionX >= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ) );
							}
						}
						else
						{
							double h = ( lfMaxX - lfMinX ) / m_nScaleCountX;
							for( double j = lfMinX; j < lfMaxX; j += h )
							{
								g.DrawLine( &pnAxis, INT( xOffs + ScaleX*( j - lfMinX ) ), INT( rc.bottom - nXPos - ( m_nScaleDirectionX <= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), 
									INT( xOffs + ScaleX*( j - lfMinX ) ), INT( rc.bottom - nXPos + ( m_nScaleDirectionX >= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ) );
							}
						}

					}
					if( Chart.bAxisDigitsX )
					{
						auto_ptr<Font> font;
						if( Chart.m_bAxisFontX )
						{
							LOGFONT lf = Chart.m_lfAxisX;
							lf.lfHeight *= m_fTextZoom;
							font.reset(new Font( (HDC)hDC, &lf ));
						}
						else
							font.reset(new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular));

						if( Chart.m_pfXAxisValues )
						{
							for( long i = 0; i < Chart.m_lAxisValuesCountX; i++ )
							{
								double j = Chart.m_pfXAxisValues[i];

								CStringW value;
								if( Chart.bstrFormatDigitsX.GetLength()>0 )
									value.Format(Chart.bstrFormatDigitsX, Chart.m_pfXAxisLabelsValues[i] );
								else
								{
									value=_variant_t(j);
								}
								if(0 <= j && j <= lfMaxX)
								{
									PointF pt( REAL( xOffs + ScaleX*( j - lfMinX ) ), REAL( rc.bottom - nXPos + m_nDigitOffsetX ) );
									RectF rcText;
									g.MeasureString( value, -1, font.get(), pt, &formatX, &rcText );
									pt.X =__min(pt.X, rc.right+10.f - rcText.Width/2.);
									g.DrawString( value, -1, font.get(), pt, &formatX, &SolidBrush( br_color ) );
								}
							}
						}
						else
						{
							double h = ( lfMaxX - lfMinX ) / m_nDigitCountX;
							for( double j = lfMinX; j < lfMaxX; j += h )
							{
								CStringW value;
								if( Chart.bstrFormatDigitsX.GetLength() )
									value.Format(Chart.bstrFormatDigitsX, j );
								else
								{
									value=_variant_t(j);
								}

								PointF pt( REAL( xOffs + ScaleX*( j - lfMinX ) ), REAL( rc.bottom - nXPos + m_nDigitOffsetX ) );
								if( ScaleX*(j-lfMaxX) >= -2.){
									formatX.SetAlignment(StringAlignmentFar);
								}
								g.DrawString( value, value.GetLength(), font.get(), pt, &formatX, &SolidBrush( br_color ) );
							}
						}
					}
				}
				if( Chart.bAxisNameX && Chart.bstrNameX.GetLength()>0 )
				{
					CStringW& value=Chart.bstrNameX;
					PointF pt( REAL( rc.right ), REAL( rc.bottom - nXPos ) );
					RectF rcText;

					Font *name_font = 0;
					if( Chart.m_bAxisNameFontX )
					{
						LOGFONT lf = Chart.m_lfAxisNameX;
						lf.lfHeight *= m_fTextZoom;
						name_font = new Font( (HDC)hDC, &lf );
					}
					else
						name_font = new Font(L"Verdana", 8 * m_fTextZoom, FontStyleRegular );

					g.MeasureString( value, value.GetLength(), name_font, pt, &formatX, &rcText );
					rcText.Offset( -rcText.Width / 2, 0 );
					g.DrawString( value, value.GetLength(), name_font, rcText, &formatX, &SolidBrush( br_color ) );

					delete name_font;
				}
				nXPos += m_nAxisOffsetX;
			}

			if( Chart.bAxisY && ( !m_bAbsolute || m_bAbsolute && !i ) )
			{
				g.DrawLine( &pnAxis, INT( rc.left + nYPos ), INT( rc.top    ), 
					INT( rc.left + nYPos ), INT( yOffs ) );
				if( Chart.bAxisArrowY )
				{
					g.DrawLine( &pnAxis, INT( rc.left + nYPos ), INT( rc.top ), 
						INT( rc.left + nYPos - m_nArrowLength*Chart.nAxisThick / 2 ), INT( rc.top + m_nArrowLength*Chart.nAxisThick ) );
					g.DrawLine( &pnAxis, INT( rc.left + nYPos ), INT( rc.top ), 
						INT( rc.left + nYPos + m_nArrowLength*Chart.nAxisThick / 2 ), INT( rc.top + m_nArrowLength*Chart.nAxisThick ) );
				}
				if( Chart.bAxisScaleY || Chart.bAxisDigitsY )
				{
					double lfMinX, lfMaxX, lfMinY, lfMaxY;
					_calc_draw_range( nKey, lfMinX, lfMaxX, lfMinY, lfMaxY );

					double ScaleY = ( yOffs - rc.top ) / ( lfMaxY - lfMinY );

					if( Chart.bAxisScaleY )
					{
						if( Chart.m_pfYAxisLabels )
						{
							for( long i = 0; i < Chart.m_lAxisLabelsCountY; i++ )
							{
								double j = Chart.m_pfYAxisLabels[i];
								if( lfMinY<=j && j<=lfMaxY )
									g.DrawLine( &pnAxis, INT( rc.left + nYPos - ( m_nScaleDirectionY <= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), INT( yOffs - ScaleY*( j - lfMinY ) ), 
									INT( rc.left + nYPos + ( m_nScaleDirectionY >= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), INT( yOffs - ScaleY*( j - lfMinY ) ) );
							}
						}
						else
						{
							double h = ( lfMaxY - lfMinY ) / m_nScaleCountY;
							for( double j = lfMinY; j < lfMaxY; j += h )
							{
								g.DrawLine( &pnAxis, INT( rc.left + nYPos - ( m_nScaleDirectionY <= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), INT( yOffs - ScaleY*( j - lfMinY ) ), 
									INT( rc.left + nYPos + ( m_nScaleDirectionY >= 1 ? m_nScaleHalfSize : 0 )*Chart.nAxisThick ), INT( yOffs - ScaleY*( j - lfMinY ) ) );
							}
						}
					}

					if( Chart.bAxisDigitsY  )
					{
						auto_ptr<Font> font;

						if( Chart.m_bAxisFontY )
						{
							LOGFONT lf = Chart.m_lfAxisY;
							lf.lfHeight *= m_fTextZoom;
							font.reset(new Font((HDC)hDC, &lf));
						}
						else
							font.reset(new Font(L"Verdana", 6*m_fTextZoom, FontStyleRegular));

						if( Chart.m_pfYAxisValues  )
						{
							for( long i = 0; i < Chart.m_lAxisValuesCountY; i++ )
							{
								double j = Chart.m_pfYAxisValues[i];

								CStringW value;
								if( Chart.bstrFormatDigitsY.GetLength()>0 )
									value.Format(Chart.bstrFormatDigitsY, Chart.m_pfYAxisLabelsValues[i] );
								else
								{
									value=_variant_t(Chart.m_pfYAxisLabelsValues[i]);
								}
								PointF pt( REAL( rc.left + nYPos - m_nDigitOffsetY ), REAL( yOffs - ScaleY*( j - lfMinY ) ) );

								if(0 <= j && j <= lfMaxY)
								{
									RectF rcText;
									g.MeasureString(  value, -1, font.get(), pt, &formatY, &rcText );
									rcText.Offset( 0/*m_YDigitsWidth*/, -rcText.Height / 2 );
									g.DrawString( value, -1, font.get(), rcText, &formatY, &SolidBrush( br_color ) );
								}
							}
						}
						else
						{
							double h = ( lfMaxY - lfMinY ) / m_nDigitCountY;
							for( double j = lfMinY; j < lfMaxY; j += h )
							{
								CStringW value;
								if( Chart.bstrFormatDigitsY.GetLength()>0 )
									value.Format(Chart.bstrFormatDigitsY, j );
								else
								{
									value=_variant_t(j);
								}

								PointF pt( REAL( rc.left + nYPos - m_nDigitOffsetY ), REAL( yOffs - ScaleY*( j - lfMinY ) ) );
								RectF rcText;

								g.MeasureString( value, value.GetLength(), font.get(), pt, &formatY, &rcText );
								rcText.Offset( -(float)m_nLBorder, -rcText.Height / 2 );
								rcText.Width=(float)m_nLBorder;
								g.DrawString( value, value.GetLength(), font.get(), rcText, &formatY, &SolidBrush( br_color ) );

							}
						}
					}
				}
				if( Chart.bAxisNameY && Chart.bstrNameY.GetLength()>0 )
				{
					CStringW& value=Chart.bstrNameY;

					PointF pt( REAL( rc.left + nYPos ), REAL( rc.top ) );
					RectF rcText;

					Font *name_font = 0;
					if( Chart.m_bAxisNameFontY )
					{
						LOGFONT lf = Chart.m_lfAxisNameY;
						lf.lfHeight *= m_fTextZoom;
						name_font = new Font( (HDC)hDC, &lf );
					}
					else
						name_font = new Font(L"Verdana", 8 * m_fTextZoom, FontStyleRegular );

					g.MeasureString( value, value.GetLength(), name_font, pt, &formatX, &rcText );
					rcText.Offset( rcText.Width, -rcText.Height / 2 );
					g.DrawString( value, value.GetLength(), name_font, rcText, &formatX, &SolidBrush( br_color ) );

					delete name_font;
				}
				nYPos += m_nAxisOffsetY;
			}
		}
		rcDraw->left   += (nYPos / m_nAxisOffsetY - 1) * m_nAxisOffsetY;
		rcDraw->bottom -= (nXPos / m_nAxisOffsetX - 1) * m_nAxisOffsetX;
	}
	else
	{
		Pen pnAxis( Color( 0xFF, GetRValue( m_Charts[0].clAxisColor ), GetGValue( m_Charts[0].clAxisColor ), GetBValue( m_Charts[0].clAxisColor ) ), REAL( m_Charts[0].nAxisThick ) );
		pnAxis.SetDashStyle( DashStyle( m_Charts[0].lAxisStyle ) );
		Color br_color( 0xFF, GetRValue( m_Charts[0].clTextColor ), GetGValue( m_Charts[0].clTextColor ), GetBValue( m_Charts[0].clTextColor ) );

		double lfMinX, lfMaxX, lfMinY, lfMaxY;
		_calc_draw_range( m_Charts.GetKey(0), lfMinX, lfMaxX, lfMinY, lfMaxY );

		double ScaleX = ( rc.right - xOffs ) / ( lfMaxX - lfMinX );
		double ScaleY = ( rc.bottom - rc.top ) / ( lfMaxY - lfMinY );

		bool bAxisX = false;
		bool bAxisY = false;
		bool bAxisArrowX = false;
		bool bAxisArrowY = false;
		int nAxisThick = 0;
		bool bAxisScaleX = false;
		bool bAxisDigitsX = false;
		bool bAxisScaleY = false;
		bool bAxisDigitsY = false;

		int nSz = (int)m_ChartsData.size();
		for( int i = 0; i < nSz; i++ )
		{
			XChartItem &Chart = m_Charts.ByIndex(i);

			if( !bAxisX && Chart.bAxisX )
				bAxisX = true;
			if( !bAxisY && Chart.bAxisY )
				bAxisY = true;
			if( !bAxisArrowX && Chart.bAxisArrowX )
				bAxisArrowX = true;
			if( !bAxisArrowY && Chart.bAxisArrowY )
				bAxisArrowY = true;
			if( !nAxisThick && Chart.nAxisThick )
				nAxisThick = Chart.nAxisThick;
			if( !bAxisScaleX && Chart.bAxisScaleX )
				bAxisScaleX = true;
			if( !bAxisDigitsX && Chart.bAxisDigitsX )
				bAxisDigitsX = true;
			if( !bAxisScaleY && Chart.bAxisScaleY )
				bAxisScaleY = true;
			if( !bAxisDigitsY && Chart.bAxisDigitsY )
				bAxisDigitsY = true;
		}

		if( bAxisX )
		{
			g.DrawLine( &pnAxis, INT( rc.left ), INT( rc.bottom + ScaleY * lfMinY ), 
				INT( rc.right), INT( rc.bottom + ScaleY * lfMinY ) );

			if( bAxisArrowX  )
			{
				g.DrawLine( &pnAxis, INT( rc.right ), INT( rc.bottom + ScaleY * lfMinY ), 
					INT( rc.right - m_nArrowLength*nAxisThick ), INT( rc.bottom + ScaleY * lfMinY - m_nArrowLength*nAxisThick / 2 ) );

				g.DrawLine( &pnAxis, INT( rc.right ), INT( rc.bottom + ScaleY * lfMinY ), 
					INT( rc.right - m_nArrowLength*nAxisThick ), INT( rc.bottom + ScaleY * lfMinY + m_nArrowLength*nAxisThick / 2 ) );
			}
			if( bAxisScaleX || bAxisDigitsX  )
			{
				if( bAxisScaleX )
				{
					if( m_Charts[0].m_pfXAxisLabels )
					{
						for( long i = 0; i < m_Charts[0].m_lAxisLabelsCountX; i++ )
						{
							double j = m_Charts[0].m_pfXAxisLabels[i];
							if(0 <= j && j <= lfMaxX)
							g.DrawLine( &pnAxis, INT( rc.left + ScaleX*( j - lfMinX ) ), INT( rc.bottom + ScaleY * lfMinY - ( m_nScaleDirectionX <= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), 
								INT( rc.left + ScaleX*( j - lfMinX ) ), INT( rc.bottom + ScaleY * lfMinY + ( m_nScaleDirectionX >= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ) );
						}
					}
					else
					{
						double h = ( lfMaxX - lfMinX ) / m_nScaleCountX;
						for( double j = lfMinX; j < lfMaxX; j += h )
						{
							g.DrawLine( &pnAxis, INT( rc.left + ScaleX*( j - lfMinX ) ), INT( rc.bottom + ScaleY * lfMinY - ( m_nScaleDirectionX <= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), 
								INT( rc.left + ScaleX*( j - lfMinX ) ), INT( rc.bottom + ScaleY * lfMinY + ( m_nScaleDirectionX >= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ) );
						}
					}

				}
				if( bAxisDigitsX )
				{
					if( m_Charts[0].m_pfXAxisValues  )
					{
						for( long i = 0; i < m_Charts[0].m_lAxisValuesCountX; i++ )
						{
							double j = m_Charts[0].m_pfXAxisValues[i];

							CStringW value;
							if( m_Charts[0].bstrFormatDigitsX.GetLength()>0 )
								value.Format(m_Charts[0].bstrFormatDigitsX, m_Charts[0].m_pfXAxisLabelsValues[i]);
							else
							{
								value=_variant_t(m_Charts[0].m_pfXAxisLabelsValues[i]);
							}

							PointF pt( REAL( rc.left + ScaleX*( j - lfMinX ) ), REAL( rc.bottom + ScaleY * lfMinY  + m_nDigitOffsetX ) );


							Font *font = 0;

							if( m_Charts[0].m_bAxisFontX )
							{
								LOGFONT lf = m_Charts[0].m_lfAxisX;
								lf.lfHeight *= m_fTextZoom;
								font = new Font( (HDC)hDC, &lf );
							}
							else
								font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

							RectF rcText;
							g.MeasureString(value, value.GetLength(), font, pt, &formatX, &rcText );
							rcText.Offset( -rcText.Width / 2, -rcText.Height / 2 );

							g.DrawString( value, value.GetLength(), font, pt, &formatX, &SolidBrush( br_color ) );

							delete font;
						}
					}
					else
					{
						double h = ( lfMaxX - lfMinX ) / m_nDigitCountX;
						for( double j = lfMinX; j < lfMaxX; j += h )
						{
							CStringW value;
							if( m_Charts[0].bstrFormatDigitsX.GetLength()>0 )
								value.Format(m_Charts[0].bstrFormatDigitsX, j );
							else
							{
								value=_variant_t(j);
							}

							PointF pt( REAL( rc.left + ScaleX*( j - lfMinX ) ), REAL( rc.bottom + ScaleY * lfMinY + m_nDigitOffsetX ) );

							Font *font = 0;
							if( m_Charts[0].m_bAxisFontX )
							{
								LOGFONT lf = m_Charts[0].m_lfAxisX;
								lf.lfHeight *= m_fTextZoom;
								font = new Font( (HDC)hDC, &lf );
							}
							else
								font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

							RectF rcText;
							g.MeasureString(value, value.GetLength(), font, pt, &formatX, &rcText );
							rcText.Offset( -rcText.Width / 2, -rcText.Height / 2 );

							g.DrawString( value, value.GetLength(), font, pt, &formatX, &SolidBrush( br_color ) );

							delete font;
						}
					}
				}
			}
			if( m_Charts[0].bAxisNameX && m_Charts[0].bstrNameX.GetLength()>0 )
			{
				_bstr_t value=m_Charts[0].bstrNameX;

				PointF pt( REAL( rc.right ), REAL( rc.bottom + ScaleY * lfMinY ) );
				RectF rcText;

				Font *name_font = 0;
				if( m_Charts[0].m_bAxisNameFontX )
				{
					LOGFONT lf = m_Charts[0].m_lfAxisNameX;
					lf.lfHeight *= m_fTextZoom;
					name_font = new Font( (HDC)hDC, &lf );
				}
				else
					name_font = new Font(L"Verdana", 8 * m_fTextZoom, FontStyleRegular );

				g.MeasureString( value, value.length(), name_font, pt, &formatX, &rcText );
				rcText.Offset( -rcText.Width / 2, 0 );
				g.DrawString( value, value.length(), name_font, rcText, &formatX, &SolidBrush( br_color ) );

				delete name_font;
			}
		}

		if( bAxisY )
		{
			g.DrawLine( &pnAxis, INT( rc.left - ScaleX * lfMinX ), INT( rc.top ), 
				INT( rc.left - ScaleX * lfMinX ), INT( rc.bottom ) );
			if( bAxisArrowY )
			{
				g.DrawLine( &pnAxis, INT( rc.left - ScaleX * lfMinX ), INT( rc.top ), 
					INT( rc.left - ScaleX * lfMinX - m_nArrowLength*nAxisThick / 2 ), INT( rc.top + m_nArrowLength*nAxisThick ) );
				g.DrawLine( &pnAxis, INT( rc.left - ScaleX * lfMinX ), INT( rc.top ), 
					INT( rc.left - ScaleX * lfMinX + m_nArrowLength*nAxisThick / 2 ), INT( rc.top + m_nArrowLength*nAxisThick ) );
			}
			if( bAxisScaleY || bAxisDigitsY )
			{
				if( bAxisScaleY )
				{
					if( m_Charts[0].m_pfYAxisLabels )
					{
						for( long i = 0; i < m_Charts[0].m_lAxisLabelsCountY; i++ )
						{
							double j = m_Charts[0].m_pfYAxisLabels[i];
							g.DrawLine( &pnAxis, INT( rc.left - ScaleX * lfMinX - ( m_nScaleDirectionY <= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), INT( rc.bottom - ScaleY*( j - lfMinY ) ), 
								INT( rc.left - ScaleX * lfMinX + ( m_nScaleDirectionY >= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), INT( rc.bottom - ScaleY*( j - lfMinY ) ) );
						}
					}
					else
					{
						double h = ( lfMaxY - lfMinY ) / m_nScaleCountY;
						for( double j = lfMinY; j < lfMaxY; j += h )
						{
							g.DrawLine( &pnAxis, INT( rc.left - ScaleX * lfMinX - ( m_nScaleDirectionY <= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), INT( rc.bottom - ScaleY*( j - lfMinY ) ), 
								INT( rc.left - ScaleX * lfMinX + ( m_nScaleDirectionY >= 1 ? m_nScaleHalfSize : 0 )*nAxisThick ), INT( rc.bottom - ScaleY*( j - lfMinY ) ) );
						}
					}
				}
				if( bAxisDigitsY )
				{
					Font *font = 0;
					if( m_Charts[0].m_bAxisFontY )
					{
						LOGFONT lf = m_Charts[0].m_lfAxisY;
						lf.lfHeight *= m_fTextZoom;
						font = new Font( (HDC)hDC, &lf );
					}
					else
						font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

					if( m_Charts[0].m_pfYAxisValues )
					{
						Font *font = 0;
						if( m_Charts[0].m_bAxisFontY )
						{
							LOGFONT lf = m_Charts[0].m_lfAxisY;
							lf.lfHeight *= m_fTextZoom;
							font = new Font( (HDC)hDC, &lf );
						}
						else
							font = new Font(L"Verdana", 6 * m_fTextZoom, FontStyleRegular );

						for( long i = 0; i < m_Charts[0].m_lAxisValuesCountY; i++ )
						{
							double j = m_Charts[0].m_pfYAxisValues[i];

							CStringW value;
							if( m_Charts[0].bstrFormatDigitsY.GetLength()>0 )
								value.Format(m_Charts[0].bstrFormatDigitsY, m_Charts[0].m_pfYAxisLabelsValues[i] );
							else
							{
								value=_variant_t(m_Charts[0].m_pfYAxisLabelsValues[i]);
							}
							PointF pt( REAL( rc.left - ScaleX * lfMinX - m_nDigitOffsetY ), REAL( rc.bottom - ScaleY*( j - lfMinY ) ) );
							RectF rcText;

							if(j <= lfMaxY)
							{
								g.MeasureString( value, -1, font, pt, &formatY, &rcText );
								rcText.Offset( -rcText.Width, -rcText.Height / 2 );
								g.DrawString( value, -1, font, rcText, &formatY, &SolidBrush( br_color ) );
							}
						}
					}
					else
					{
						double h = ( lfMaxY - lfMinY ) / m_nDigitCountY;
						for( double j = lfMinY; j < lfMaxY; j += h )
						{
							CStringW value;
							if( m_Charts[0].bstrFormatDigitsY.GetLength()>0 )
								value.Format(m_Charts[0].bstrFormatDigitsY, j );
							else
							{
								value=_variant_t(j);
							}

							PointF pt( REAL( rc.left - ScaleX * lfMinX - m_nDigitOffsetY ), REAL( rc.bottom - ScaleY*( j - lfMinY ) ) );
							RectF rcText;

							g.MeasureString( value, value.GetLength(), font, pt, &formatY, &rcText );
							rcText.Offset( -rcText.Width, -rcText.Height / 2 );
							g.DrawString( value, value.GetLength(), font, rcText, &formatY, &SolidBrush( br_color ) );

						}
					}
					delete font;
				}
			}
			if( m_Charts[0].bAxisNameY && m_Charts[0].bstrNameY.GetLength()>0 )
			{
				_bstr_t value=m_Charts[0].bstrNameY;
				PointF pt( REAL( rc.left - ScaleX * lfMinX ), REAL( rc.top ) );
				RectF rcText;

				Font *name_font = 0;
				if( m_Charts[0].m_bAxisNameFontY )
				{
					LOGFONT lf = m_Charts[0].m_lfAxisNameY;
					lf.lfHeight *= m_fTextZoom;
					name_font = new Font( (HDC)hDC, &lf );
				}
				else
					name_font = new Font(L"Verdana", 8 * m_fTextZoom, FontStyleRegular );

				g.MeasureString( value, value.length(), name_font, pt, &formatX, &rcText );
				rcText.Offset( rcText.Width, -rcText.Height / 2 );
				g.DrawString( value, value.length(), name_font, rcText, &formatX, &SolidBrush( br_color ) );

				delete name_font;
			}
		}


	}
	return S_OK;
}

HRESULT CChartDrawer::_calc_draw_range( int nID, double &lfMinX, double &lfMaxX, double &lfMinY, double &lfMaxY )
{
	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	lfMinX = data.minX; lfMaxX = data.maxX;
	lfMinY = data.minY; lfMaxY = data.maxY;

	if( Chart.bEnableRangeX || Chart.bEnableRangeY )
	{
		if( Chart.bEnableRangeX )
		{ lfMinX = Chart.lfMinX; lfMaxX = Chart.lfMaxX; }
		if( Chart.bEnableRangeY )
		{ lfMinY = Chart.lfMinY; lfMaxY = Chart.lfMaxY; }
	}
	if( m_bAbsolute )
	{
		int nSz = (int)m_Charts.size();
		lfMinX = lfMinY = 1e308;
		lfMaxX = lfMaxY = -1e307;
		for( int i = 0; i < m_ChartsData.size(); i++ )
		{
			XChartDataItem &data = m_ChartsData.ByIndex(i);
			XChartItem &Chart2 = m_Charts.ByIndex(i);

			if( !Chart2.bShow )
				continue;

			if( data.maxY > lfMaxY )
				lfMaxY = data.maxY;
			if( data.minY < lfMinY )
				lfMinY = data.minY;

			if( data.maxX > lfMaxX )
				lfMaxX = data.maxX;
			if( data.minX < lfMinX )
				lfMinX = data.minX;

			if( Chart2.bEnableRangeX )
			{ 
				lfMaxX = Chart2.lfMaxX;
				lfMinX = Chart2.lfMinX;
			}
			if( Chart2.bEnableRangeY )
			{ 
				lfMaxY = Chart2.lfMaxY;
				lfMinY = Chart2.lfMinY;
			}
			if( Chart2.bEnableRangeX || Chart2.bEnableRangeY )
				break;
		}
		m_lfMinX = lfMinX;
		m_lfMinY = lfMinY;
		m_lfMaxX = lfMaxX;
		m_lfMaxY = lfMaxY;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////
HRESULT CChartDrawer::GetToolTipValue( long nID, POINT ptMiceInDC, double *pX, double *pY, short bYfromX, int nHalfHotZone )
{
	if( !pX || !pY )
		return S_FALSE;


	double lfMinX, lfMaxX, lfMinY, lfMaxY;
	_calc_draw_range( nID, lfMinX, lfMaxX, lfMinY, lfMaxY );

	RECT rcDraw = m_rcCorrected;

	if( m_fRotAngle != 0 )
	{
		rcDraw.left   -= m_nLBorder;
		rcDraw.right  += m_nRBorder;
		rcDraw.top    -= m_nBBorder;
		rcDraw.bottom += m_nTBorder;
	}

	if( m_fRotAngle == 90 || m_fRotAngle == -270 )
	{
		rcDraw.left   += m_nBBorder; 
		rcDraw.right  -= m_nTBorder;
		rcDraw.top    += m_nLBorder;
		rcDraw.bottom -= m_nRBorder;


	}
	else if( m_fRotAngle == 180 || m_fRotAngle == -180 )
	{
		rcDraw.left   += m_nRBorder;
		rcDraw.right  -= m_nLBorder;
		rcDraw.top    += m_nTBorder;
		rcDraw.bottom -= m_nBBorder;

	}
	else if( m_fRotAngle == 270 || m_fRotAngle == -90 )
	{
		rcDraw.left   += m_nTBorder;
		rcDraw.right  -= m_nBBorder;
		rcDraw.top    += m_nRBorder;
		rcDraw.bottom -= m_nLBorder;
	}

	double ScaleX = ( rcDraw.right - rcDraw.left ) / ( lfMaxX - lfMinX );
	double ScaleY = ( rcDraw.bottom - rcDraw.top ) / ( lfMaxY - lfMinY );

	if( m_fRotAngle == 90 || m_fRotAngle == 270 || m_fRotAngle == -90 || m_fRotAngle == -270 )
	{
		RECT rc = rcDraw;
		rcDraw.right = rcDraw.left + rc.bottom - rc.top;
		rcDraw.bottom = rcDraw.top + rc.right - rc.left;

	}

	XChartDataItem &data = m_ChartsData[nID];

	POINT ptMice = ptMiceInDC;

	if( !( ptMice.x >= rcDraw.left && ptMice.x <= rcDraw.right && 
		ptMice.y >= rcDraw.top && ptMice.y <= rcDraw.bottom ) )
		return S_FALSE;

	double fxMice = 0, fyMice = 0;

	double fMinDiffX = 1;
	double fMinDiffY = 1;

	{
		double fLen = _hypot( fMinDiffX, fMinDiffY );
		fMinDiffX /= fLen;
		fMinDiffY /= fLen;
	}

	if( m_fRotAngle == 0 )
	{
		int xMice = ptMice.x - rcDraw.left, yMice = rcDraw.bottom - ptMice.y;

		fxMice = xMice / ScaleX + lfMinX;
		fyMice = yMice / ScaleY + lfMinY;

		fMinDiffX = fMinDiffX / ScaleX;
		fMinDiffY = fMinDiffY / ScaleY;
	}
	else if( m_fRotAngle == 90 || m_fRotAngle == -270 )
	{
		int xMice = rcDraw.right - ptMice.x, yMice = ptMice.y - rcDraw.top;

		fxMice = yMice / ScaleX + lfMinX;
		fyMice = xMice / ScaleY + lfMinY;

		double _x = fMinDiffX;
		fMinDiffX = fMinDiffY / ScaleX;
		fMinDiffY = _x / ScaleY;
	}
	else if( m_fRotAngle == 180 || m_fRotAngle == -180 )
	{
		int xMice = rcDraw.right - ptMice.x, yMice = rcDraw.bottom - ptMice.y;

		fxMice = xMice / ScaleX + lfMinX;
		fyMice = yMice / ScaleY + lfMinY;

		fMinDiffX = fMinDiffX / ScaleX;
		fMinDiffY = fMinDiffY / ScaleY;

	}
	else if( m_fRotAngle == 270 || m_fRotAngle == -90 )
	{
		int xMice = rcDraw.right - ptMice.x, yMice = rcDraw.bottom  - ptMice.y;

		fxMice = yMice / ScaleX + lfMinX;
		fyMice = xMice / ScaleY + lfMinY;

		double _x = fMinDiffX;

		fMinDiffX = fMinDiffY / ScaleX;
		fMinDiffY = _x / ScaleY;
	}

	int nId = -1;

	double *pValuesX = data.pValuesX;
	double *pValuesY = data.pValuesY;

	double fX = fxMice, fY = fyMice;


	if( !bYfromX )
	{
		pValuesX = new double[data.nSz];
		pValuesY = new double[data.nSz];

		for( long i = 0; i < data.nSz; i++ )
		{
			pValuesX[i] = data.pValuesY[data.nSz - 1 - i];
			pValuesY[i] = data.pValuesX[data.nSz - 1 - i];
		}

		fX = fyMice;
		fY = fxMice;
	}


	bool bOK = false;

	*pX = fX;
	*pY = fY;

	double fMinLen = 1e+308;
	for( long i = 1; i < data.nSz; i++ )
	{
		double y = CanonicY( pValuesX[i - 1], pValuesY[i - 1], pValuesX[i], pValuesY[i], *pX );

		double a = 0, b = 0;

		if( !bYfromX )
		{
			a = lfMaxX - lfMinX;
			b = lfMaxY - lfMinY;
		}
		else
		{
			a = lfMaxY - lfMinY;
			b = lfMaxX - lfMinX;
		}


		if( fabs( *pY  - y ) * ScaleY <= ScaleY )
		{
			if( fabs( *pX  - ( pValuesX[i - 1] + pValuesX[i] ) / 2 ) * ScaleX <= ScaleX )
			{
				*pY = y;
				bOK = true; 
				break;
			}
		}

		/*
		double fPtLen = 0;

		double k = pValuesY[i] - pValuesY[i - 1];
		double z = pValuesX[i] - pValuesX[i - 1];

		if( fabs( z ) < 0.0001 )
		fPtLen = fabs( fX - pValuesX[i] );
		else
		fPtLen = fabs(( k/z * fX - fY + pValuesY[i] - k/z * pValuesX[i] ) / _hypot( k/z, -1 ));

		if( fPtLen < _hypot( fMinDiffX, fMinDiffY ) )
		{
		fMinLen = fPtLen;
		*pY = y;
		bOK = true;
		break;
		}
		*/
	}

	if( !bYfromX )
	{
		delete []pValuesX;
		delete []pValuesY;
	}

	return bOK ? S_OK : S_FALSE;
}

HRESULT CChartDrawer::SmoothChart(long nID, double fSmoothParam, short bYFromX, ChartLineSmooth dwFlag )
{
	XChartItem &Chart = m_Charts[nID];
	XChartDataItem &data = m_ChartsData[nID];

	if( !data.pValuesX || !data.pValuesY )
		return S_FALSE;

	double *pX = new double[data.nSz], *pY = new double[data.nSz];

	if( bYFromX )
	{
		for( int i = 0; i < data.nSz; i++ )
		{
			pX[i] = data.pValuesX[i];
			pY[i] = data.pValuesY[i];
		}
	}
	else
	{
		for( int i = 0; i < data.nSz; i++ )
		{
			pX[i] = data.pValuesY[i];
			pY[i] = data.pValuesX[i];
		}
	}

	if( dwFlag & csfMovingAverage )
	{
		int nSz = fSmoothParam / 2;
		for( int i = nSz; i < data.nSz - nSz; i++ )
		{
			double fVal = 0;
			for( int j = i - nSz; j <= i + nSz; j++ )
			{
				if( bYFromX )
					fVal += data.pValuesY[j];
				else
					fVal += data.pValuesX[j];
			}
			fVal /= fSmoothParam;

			if( bYFromX )
				data.pValuesY[i] = fVal;
			else
				data.pValuesX[i] = fVal;
		}
	}
	else if( dwFlag & csfExp )
	{
		for( int i = 1; i < data.nSz; i++ )
		{
			if( bYFromX )
				data.pValuesY[i] = fSmoothParam * data.pValuesY[i] + (1 - fSmoothParam )*data.pValuesY[i - 1];
			else
				data.pValuesX[i] = fSmoothParam * data.pValuesX[i] + (1 - fSmoothParam )*data.pValuesX[i - 1];
		}
	}
	else if( dwFlag & csfMediane )
	{
		int nSz = fSmoothParam / 2;
		for( int i = nSz; i < data.nSz - nSz; i++ )
		{
			double *fVals = new double[int(fSmoothParam)];
			for( int j = i - nSz; j <= i + nSz; j++ )
			{
				if( bYFromX )
					fVals[j - (i - nSz)] = data.pValuesY[j];
				else
					fVals[j - (i - nSz)] = data.pValuesX[j];
			}

			for( j = 0; j < fSmoothParam; j++ )
			{
				int nID = j;

				for( int k = j; k < fSmoothParam; k++ )
				{
					if( fVals[k] < fVals[nID] )
						nID = k;
				}
				if( nID != j )
				{
					double t = fVals[nID];
					fVals[nID] = fVals[j];
					fVals[j] = t;
				}
			}
			if( bYFromX )
				data.pValuesY[i] = fVals[int(fSmoothParam) - nSz - 1];
			else
				data.pValuesX[i] = fVals[int(fSmoothParam) - nSz - 1];
		}
	}
	else 
	{
		long nSz = data.nSz * fSmoothParam - fSmoothParam + 1;

		double *A0   = pY;
		double *A1 = new double[data.nSz];
		double *A2 = new double[data.nSz];
		double *A3 = new double[data.nSz];

		A1[0] = ( pY[1] - pY[0] ) / ( pX[1] - pX[0] ); // Первая производная
		if( dwFlag & csfSpline3 )
		{
			double f = ( pY[2] - pY[0] ) / ( ( pX[2] - pX[0] ) ); // Первая производная
			A2[0] = ( A1[0] - f ) / ( pX[1] - pX[0] ); // Вторая производная
		}

		for( int i = 0; i < data.nSz - 1; i++ )
		{
			double s = pX[i + 1] - pX[i];

			if( dwFlag & csfSpline3 )
				A3[i] = ( pY[i + 1] - A0[i] - A1[i] * s - A2[i] * s * s ) / ( s * s * s );

			if( dwFlag & csfSpline2 )
				A2[i] = ( pY[i + 1] - A0[i] - A1[i] * s ) / ( s * s );
			else
				A2[i + 1] = A2[i] + 3*A3[i] * s;

			A1[i + 1] = A1[i] + 2*A2[i] * s;
			if( dwFlag & csfSpline3 )
				A1[i + 1] += 3*A3[i] * s * s;
		}

		delete []data.pValuesX;
		delete []data.pValuesY;

		data.pValuesX = new double[nSz];
		data.pValuesY = new double[nSz];

		for( i = 0; i < nSz; i += fSmoothParam )
		{
			int n = int( i / fSmoothParam );

			if( bYFromX )
			{
				data.pValuesX[i] = pX[n];
				data.pValuesY[i] = pY[n];
			}
			else
			{
				data.pValuesY[i] = pX[n];
				data.pValuesX[i] = pY[n];
			}

			if( n + 1 == data.nSz )
				break;

			if( bYFromX )
			{
				double s = pX[n + 1] - pX[n];
				for( int j = 0; j < fSmoothParam; j++ )
				{
					double h = s / fSmoothParam * j;

					data.pValuesX[i + j] = data.pValuesX[i] + h;

					double t = data.pValuesX[i + j] - data.pValuesX[i];

					data.pValuesY[i + j] = A0[n] + A1[n] * t + A2[n] * t * t;

					if( dwFlag & csfSpline3 )
						data.pValuesY[i + j] += A3[n] * t * t * t;
				}
			}
			else
			{
				double s = pY[n + 1] - pY[n];
				for( int j = 0; j < fSmoothParam; j++ )
				{
					double h = s / fSmoothParam * j;

					data.pValuesY[i + j] = data.pValuesY[i] + h;

					double t = data.pValuesY[i + j] - data.pValuesY[i];

					data.pValuesX[i + j] = A0[n] + A1[n] * t + A2[n] * t * t;

					if( dwFlag & csfSpline3 )
						data.pValuesX[i + j] += A3[n] * t * t * t;
				}
			}
		}

		if( A1 ) delete []A1;
		if( A2 ) delete []A2;
		if( A3 ) delete []A3;

		data.nSz = nSz;
	}

	if( pX )
		delete []pX;

	if( pY )
		delete []pY;

	/*
	data.minY = data.minX = 1e308;
	data.maxY = data.maxX = -1e307;

	for( int i = 0; i < data.nSz; i++ )
	{
	if( data.pValuesY[i] >  data.maxY )
	data.maxY = data.pValuesY[i];
	if( data.pValuesY[i] <  data.minY )
	data.minY = data.pValuesY[i];

	if( data.pValuesX[i] >  data.maxX )
	data.maxX = data.pValuesX[i];
	if( data.pValuesX[i] <  data.minX )
	data.minX = data.pValuesX[i];
	}
	*/
	return S_OK;
}

HRESULT CChartDrawer::LPtoDP( DWORD hDC, POINT *pt )
{
	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	if( !pt )
		return S_FALSE;

	SIZE sizeWE, sizeVE;

	::GetWindowExtEx( (HDC)hDC, &sizeWE );
	::GetViewportExtEx( (HDC)hDC, &sizeVE );

	double fScaleX = sizeWE.cx / double( sizeVE.cx );
	double fScaleY = sizeWE.cy / double( sizeVE.cy );

	PointF ptf( pt->x, pt->y );
	g.TransformPoints( CoordinateSpaceDevice, CoordinateSpacePage, &ptf, 1 );

	pt->x = ptf.X / fScaleX;
	pt->y = ptf.Y / fScaleY;

	return S_OK;
}

HRESULT CChartDrawer::DPtoLP( DWORD hDC, POINT *pt )
{
	Graphics g( (HDC)hDC );
	if( m_nMode != -1 )
		g.SetPageUnit( (Unit)m_nMode );

	if( !pt )
		return S_FALSE;

	SIZE sizeWE, sizeVE;

	::GetWindowExtEx( (HDC)hDC, &sizeWE );
	::GetViewportExtEx( (HDC)hDC, &sizeVE );

	double fScaleX = sizeWE.cx / double( sizeVE.cx );
	double fScaleY = sizeWE.cy / double( sizeVE.cy );


	PointF ptf( pt->x, pt->y );
	g.TransformPoints( CoordinateSpacePage, CoordinateSpaceDevice , &ptf, 1 );

	pt->x = ptf.X / fScaleX;
	pt->y = ptf.Y / fScaleY;

	return S_OK;
}

HRESULT CChartDrawer::ConvertToDC( long nID, RECT rcDraw, double x, double y, POINT *pt )
{
	if( !pt )
		return S_FALSE;

	XChartDataItem &data = m_ChartsData[nID];
	XChartItem &Chart = m_Charts[nID];

	int nSrcW = rcDraw.right - rcDraw.left; 
	int nSrcH = rcDraw.bottom - rcDraw.top; 

	double lfMinX, lfMaxX, lfMinY, lfMaxY;
	_calc_draw_range( nID, lfMinX, lfMaxX, lfMinY, lfMaxY );

	double ScaleX = nSrcW / ( lfMaxX - lfMinX );
	double ScaleY = nSrcH / ( lfMaxY - lfMinY );

	pt->x = INT( ScaleX * ( x - lfMinX ) );
	pt->y = INT( nSrcH - ScaleY * ( y - lfMinY ) );
	return S_OK;
}

HRESULT CChartDrawer::ClearAll()
{
	m_Charts.clear();
	m_ChartsData.clear();
	m_list_markers.clear();

	CChartDrawer::do_default( false );
	IChartAttributesImpl::do_default( false );


	return S_OK;
}

HRESULT CChartDrawer::LogarifmScale( ChartDataType dwFlag  )
{
	double lfMinX = 0, lfMaxX = 0, lfMinY = 0, lfMaxY = 0;

	if( m_bAbsolute )
		_calc_draw_range( m_Charts.GetKey(0), lfMinX, lfMaxX, lfMinY, lfMaxY );

	int nSz = (int)m_Charts.size();
	for( int i = 0; i < nSz; i++ )
	{
		long nID = m_Charts.GetKey(i);

		if( !m_bAbsolute )
			_calc_draw_range( nID, lfMinX, lfMaxX, lfMinY, lfMaxY );

		XChartDataItem &data = m_ChartsData[nID];

		if( dwFlag & cdfDataX && data.pValuesX )
		{
			if( lfMinX <= 0 )
				lfMinX = 0.0001;

			if( lfMaxX <= 0 )
				lfMaxX = 0.0001;

			for( long i = 0; i < data.nSz; i++ )
			{
				if( data.pValuesX[i] > 0 )
					data.pValuesX[i] = log(data.pValuesX[i]/lfMinX) / log(lfMaxX/lfMinX);
				else
					data.pValuesX[i] = 0;
			}

			data.minX = 0;
			data.maxX = 1;

			XChartItem &Chart = m_Charts[nID];
			for( int j = 0; j < Chart.m_vectHints.size();j++ )
				Chart.m_vectHints[j].fX = log(Chart.m_vectHints[j].fX/lfMinX)/log(lfMaxX/lfMinX);
		}

		if( dwFlag & cdfDataY && data.pValuesY )
		{
			if( lfMinY <= 0 )
				lfMinY = 0.0001;

			if( lfMaxY <= 0 )
				lfMaxY = 0.0001;

			for( long i = 0; i < data.nSz; i++ )
			{
				if( data.pValuesY[i] > 0 )
					data.pValuesY[i] = log( data.pValuesY[i]/lfMinY)/log(lfMaxY/lfMinY);
				else
					data.pValuesY[i] = 0;
			}

			data.minY = 0;
			data.maxY = 1;

			XChartItem &Chart = m_Charts[nID];
			for( int j = 0; j < Chart.m_vectHints.size();j++ )
				Chart.m_vectHints[j].fY = log(Chart.m_vectHints[j].fY/lfMinY) / log(lfMaxY/lfMinY);
		}
	}
	return S_OK;
}

HRESULT CChartDrawer::AddMarker( double fX, double fY, long nPixelInSize, COLORREF clColor, ChartMarker dwFlag )
{
	x_markers marker;

	marker.x			= fX;
	marker.y			= fY;
	marker.lsz		= nPixelInSize;
	marker.ltype	= (long)dwFlag;
	marker.color	= clColor;

	m_list_markers.push_back( marker );
	return S_OK;
}
