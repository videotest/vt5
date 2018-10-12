#include "StdAfx.h"
#include "StatUI.h"
#include "statistics.h"
#include "statisticobject.h"
#include "statobjectview.h"
#include "misc.h"
#include "float.h"

#define MAX_SCALE_FACTOR_X					400
#define MAX_SCALE_FACTOR_VALUES_X		200
#define MAX_SCALE_FACTOR_GRID_X			400

#define MAX_SCALE_FACTOR_Y				   400
#define MAX_SCALE_FACTOR_VALUES_Y	   200
#define MAX_SCALE_FACTOR_GRID_Y		   400

namespace {
	int  round(double num) {
			return int((num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5));
	}
}

namespace ViewSpace 
{

	inline CString _find_in_meas_param( INamedDataPtr sptrNData, CString strSect,long ParamKey, CString strName )
	{
		CString strRet;
		if( sptrNData == 0 )
			return strRet;

		sptrNData->SetupSection( _bstr_t( strSect ) );

		long lCount = 0;
		sptrNData->GetEntriesCount( &lCount );

		for( long i = 0; i < lCount; i++ )
		{
			_bstr_t bstr;
			sptrNData->GetEntryName( i, bstr.GetAddress() );

			if( CString( (char*)bstr ) == "Key" )
			{
				_variant_t var;
				sptrNData->GetValue( _bstr_t( "Key" ), &var );

				if( var.vt == VT_I4 )
				{
					if( var.lVal == ParamKey )
					{
						_variant_t var;
						sptrNData->GetValue( _bstr_t( strName ), &var );

						if( var.vt == VT_BSTR )
							strRet = var.bstrVal;

						return strRet;
					}
				}
			}
		}

		for( long i = 0; i < lCount; i++ )
		{
			_bstr_t bstr;
			sptrNData->GetEntryName( i, bstr.GetAddress() );
			strRet = _find_in_meas_param( sptrNData, strSect + "\\" + (char*)bstr, ParamKey, strName );

			sptrNData->SetupSection( _bstr_t( strSect ) );			

			if( !strRet.IsEmpty() )
				break;
		}
		return strRet;
	}

	CChartWnd::CChartWnd()
		:m_hFontSignature(0)
		, m_DCDispLOGPIXELSY(0)
		, m_DCLOGPIXELSY(0)
		, m_bPrinting(false)
	{
		m_bPrintMode = false;
		m_iError = 0;
		m_LogScaleStep = 3;
		m_fMinYValue = m_fMaxYValue = m_fMinXValue = m_fMaxXValue = 0;

		m_arrClasses = 0;
		m_nMaxYChart = 0.f;
		::ZeroMemory( &m_strClassFile, sizeof( m_strClassFile ) );

		m_nYTitle = 0;
		m_nY_XParam = 0;
		m_nY_Legend = 0;

		m_b_use_custom_format_x = false;
		m_b_use_custom_format_y = false;

		m_str_custom_format_x = m_str_custom_format_y = "%g";


		{
			RECT rc = { 0, 0, 200, 200 };
			m_rcMinRect = rc;
		}
		m_bMinSizeCalc = false;
		m_lClassCount = 0;
		m_nHotZone = 1;

		m_bSteppedParamChart = false;
		m_parrParamChartMin = m_parrParamChartMax = 0;
		m_pstrCustomParams = 0;


		IUnknown *punk = 0;

		::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punk );

		m_sptrChart = punk;

		if( punk )
			punk->Release();

		m_rcFull = m_rcClient = m_rcChart = NORECT;


		m_nXAxisType = 0;
		m_nChartType = 0;
		m_nChartViewType = 0;

		m_bShowGrayScale = false;
		m_nGrayScaleLevel = 128;

		m_nXParamKey = 0;
		m_nYParamKey = PARAM_COUNT;

		m_nParamsHeight = 10;
		m_fCoefParam = 1;

		m_nClassStep = 1;
		m_nMinClass = 0;



		m_fBarWidth = 20;

		m_clChartColor = RGB( 255, 0, 0 );

		m_bShowSingleColorBar = false;
		m_clBarSingleColor = RGB(255,0,0);


		m_rcTitle = m_rcXParam = m_rcYParam = m_rcLegend = NORECT;

		m_nShowLegend = 0;

		m_bShowBeginEnd = true;
		m_bShowStep = true;
		m_bShowClassCount = true;
		m_bShowCheckModel = true;
		m_nCheckModelType = 0;

		m_bEnableChartArea = true;
		m_bEnableAxisArea = true;
		m_bEnableXParamArea = true;
		m_bEnableYParamArea = true;

		m_strChartBeginEnd.LoadString( IDS_CHART_BEGIN_END );
		m_strChartStep.LoadString( IDS_CHART_STEP );
		m_strChartClasses.LoadString( IDS_CHART_CLASSES );
		m_strChartModel.LoadString( IDS_CHART_MODEL );
		m_strClassName.LoadString( IDS_CLASSES );

		// [vanek] SBT:1260 - 20.12.2004
		m_strCompareTitle.LoadString( IDS_CHART_TITLE_COMPARE );

		m_bConvertToCalibr = true;

		::ZeroMemory( &m_lfFontTitle, sizeof( m_lfFontTitle ) );
		memset( &m_lfFontTitle, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontTitle.lfFaceName, "Arial" );
		m_lfFontTitle.lfHeight = 10;
		m_lfFontTitle.lfWeight = 400;
		m_lfFontTitle.lfItalic = 0;
		m_lfFontTitle.lfUnderline = 0;
		m_lfFontTitle.lfStrikeOut = 0;
		m_lfFontTitle.lfCharSet = DEFAULT_CHARSET;
		m_lfFontTitle.lfPitchAndFamily = FF_SWISS;

		::ZeroMemory( &m_lfFontSignature, sizeof( m_lfFontSignature ) );
		memset( &m_lfFontSignature, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontSignature.lfFaceName, "Arial" );
		m_lfFontSignature.lfHeight = 10;
		m_lfFontSignature.lfWeight = 400;
		m_lfFontSignature.lfItalic = 0;
		m_lfFontSignature.lfUnderline = 0;
		m_lfFontSignature.lfStrikeOut = 0;
		m_lfFontSignature.lfCharSet = DEFAULT_CHARSET;

		::ZeroMemory( &m_lfFontLegend, sizeof( m_lfFontLegend ) );
		memset( &m_lfFontLegend, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontLegend.lfFaceName, "Arial" );
		m_lfFontLegend.lfHeight = 10;
		m_lfFontLegend.lfWeight = 400;
		m_lfFontLegend.lfItalic = 0;
		m_lfFontLegend.lfUnderline = 0;
		m_lfFontLegend.lfStrikeOut = 0;
		m_lfFontLegend.lfCharSet = DEFAULT_CHARSET;
		m_lfFontLegend.lfPitchAndFamily = FF_SWISS;

		HDC hDCDisp = CreateDC( "DISPLAY", 0, 0, 0 );
		m_DCDispLOGPIXELSY=GetDeviceCaps(hDCDisp,LOGPIXELSY);
		::DeleteDC(hDCDisp);
		m_DCLOGPIXELSY=m_DCDispLOGPIXELSY;

		::ZeroMemory( &m_lfFontAxisChart, sizeof( m_lfFontAxisChart ) );
		memset( &m_lfFontAxisChart, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontAxisChart.lfFaceName, "Arial" );
		m_lfFontAxisChart.lfHeight = 6;
		m_lfFontAxisChart.lfWeight = 400;
		m_lfFontAxisChart.lfItalic = 0;
		m_lfFontAxisChart.lfUnderline = 0;
		m_lfFontAxisChart.lfStrikeOut = 0;
		m_lfFontAxisChart.lfCharSet = DEFAULT_CHARSET;
		m_lfFontAxisChart.lfPitchAndFamily = FF_SWISS;

		::ZeroMemory( &m_lfFontHint, sizeof( m_lfFontHint ) );
		memset( &m_lfFontHint, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontHint.lfFaceName, "Arial" );
		m_lfFontHint.lfHeight = 10;
		m_lfFontHint.lfWeight = 400;
		m_lfFontHint.lfItalic = 0;
		m_lfFontHint.lfUnderline = 0;
		m_lfFontHint.lfStrikeOut = 0;
		m_lfFontHint.lfCharSet = DEFAULT_CHARSET;
		m_lfFontHint.lfPitchAndFamily = FF_SWISS;

		::ZeroMemory( &m_lfFontParam, sizeof( m_lfFontParam ) );
		memset( &m_lfFontParam, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontParam.lfFaceName, "Arial" );
		m_lfFontParam.lfHeight = 10;
		m_lfFontParam.lfWeight = 400;
		m_lfFontParam.lfItalic = 0;
		m_lfFontParam.lfUnderline = 0;
		m_lfFontParam.lfStrikeOut = 0;
		m_lfFontParam.lfCharSet = DEFAULT_CHARSET;
		m_lfFontParam.lfPitchAndFamily = FF_SWISS;

		::ZeroMemory( &m_lfFontParam2, sizeof( m_lfFontParam2 ) );
		memset( &m_lfFontParam2, 0, sizeof( LOGFONT ) );
		strcpy( m_lfFontParam2.lfFaceName, "Arial" );
		m_lfFontParam2.lfHeight = 10;
		m_lfFontParam2.lfWeight = 400;
		m_lfFontParam2.lfItalic = 0;
		m_lfFontParam2.lfUnderline = 0;
		m_lfFontParam2.lfStrikeOut = 0;
		m_lfFontParam2.lfCharSet = DEFAULT_CHARSET;
		m_lfFontParam2.lfPitchAndFamily = FF_SWISS;
		m_lfFontParam2.lfOrientation = 0;
		m_lfFontParam2.lfEscapement = 0;

		m_lfFontTitle.m_clrText = 0;
		m_lfFontLegend.m_clrText = 0;
		m_lfFontAxisChart.m_clrText = 0;
		m_lfFontSignature.m_clrText = 0;
		m_lfFontHint.m_clrText = 0;
		m_lfFontParam.m_clrText = 0;
		m_lfFontParam2.m_clrText = 0;

		m_bXGrid = 0; 
		m_bYGrid = 0;

		m_fXScaleFactor = 25;
		m_fYScaleFactor = 25;

		m_bUseLogaScaleX = 0;

		m_fXScaleFactorValues = 50;
		m_fYScaleFactorValues = 50;

		m_fXScaleFactorGrid = 50;
		m_fYScaleFactorGrid = 50;

		m_clGridColor = 0;

		m_bShowAxisValueX = 1;
		m_bShowAxisValueY = 1;

		m_bShowAxisLabelsX = 1;
		m_bShowAxisLabelsY = 1;

		m_lTextOffsetX = 0;
		m_lTextOffsetY = 0;


		m_fX_AxisBegin = 0;
		m_fX_AxisEnd = 0;
		m_bX_UseValueRange = false;

		m_fY_AxisBegin = 0;
		m_fY_AxisEnd = 0;
		m_bY_UseValueRange = false;

		m_bShowCurve = false;
		m_lUniformDistribution = 0;
		m_clCurveColor = 0;
		::ZeroMemory( &m_ptOffset, sizeof( m_ptOffset ) );

		m_bShowUserCurve = false;
		m_clUserCurveColor = RGB( 0, 0, 255 );
		m_fUserCurveMX = 0;
		m_fUserCurveDX = 1;

		m_cur_hand = ::LoadCursor( 0, IDC_HAND );
		m_cur_arrow = ::LoadCursor( 0, IDC_ARROW );

		m_bDrawEndChartGridLineX	= false;
		m_bDrawEndChartGridLineY	= false;

		m_bShowErrorMsg = false;
		m_dwViews = 0;
	}

	CChartWnd::~CChartWnd()
	{
		if( m_cur_hand )
			::DeleteObject( m_cur_hand );
		m_cur_hand = 0;

		if( m_cur_arrow )
			::DeleteObject( m_cur_arrow );
		m_cur_arrow = 0;

		if( m_arrClasses )
			delete []m_arrClasses; m_arrClasses = 0;

		if( m_parrParamChartMin )
			delete []m_parrParamChartMin; m_parrParamChartMin = 0;

		if( m_parrParamChartMax )
			delete []m_parrParamChartMax; m_parrParamChartMax = 0;

		if( m_pstrCustomParams )
			delete []m_pstrCustomParams; m_pstrCustomParams = 0;
		if(m_hFontSignature)
			::DeleteObject(m_hFontSignature);
	}

	LRESULT CChartWnd::on_paint()
	{
		RECT rcPaint = { 0, 0, 0, 0 };
		::GetUpdateRect( handle(), &rcPaint, false );

		RECT rcPaint2 = { 0, 0, 0, 0 };
		::GetClientRect( handle(), &rcPaint2 );

		PAINTSTRUCT	ps = {0};
		HDC hdcPaint = ::BeginPaint( handle(), &ps );	

		HDC	hdcMemory = ::CreateCompatibleDC( hdcPaint );

		int nImageWidth		= rcPaint.right  - rcPaint.left  + 1;
		int nImageHeight	= rcPaint.bottom - rcPaint.top  + 1;

		BITMAPINFOHEADER	bmih = {0};

		bmih.biBitCount = 24;	
		bmih.biWidth	= nImageWidth;
		bmih.biHeight	= nImageHeight;							   
		bmih.biSize		= sizeof( bmih );
		bmih.biPlanes	= 1;							   

		byte	*pdibBits = 0;
		HBITMAP	hDIBSection = ::CreateDIBSection( hdcMemory, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

		HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hdcMemory, hDIBSection );

		::SetViewportOrgEx( hdcMemory, -rcPaint.left, -rcPaint.top, 0 );
		::SetBrushOrgEx( hdcMemory, -rcPaint.left % 8, -rcPaint.top % 8, 0 );

		DoDraw( hdcMemory, rcPaint2 );

		::SetViewportOrgEx( hdcMemory, 0, 0, 0 );

		::SetDIBitsToDevice( hdcPaint, rcPaint.left, rcPaint.top, nImageWidth, nImageHeight,
			0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

		::SelectObject( hdcMemory, hOldBitmap );
		::DeleteObject( hDIBSection );
		::DeleteObject( hdcMemory );

		::EndPaint( handle(), &ps );

		return 1;
	}

	const static int __min = 300;

	void CChartWnd::DoDraw( HDC hDC, RECT rcPaint, RECT *rcCalc, double fZoom, bool bPrint )
	{
		if( bPrint && m_list_attached.count() == 0 )
			return;

		if( m_iError)
		{
			if(!rcCalc )
			{
				HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );

				if( !bPrint )
				{
					::FillRect( hDC, &rcPaint, hbrush );
					::SetBkColor( hDC, ::GetSysColor( COLOR_BTNFACE ) );
				}

				CString strErrorDescr((LPCTSTR)m_iError);
				::SetBkMode(hDC,TRANSPARENT);
				{
					Gdiplus::Graphics g(hDC); g.SetPageUnit(UnitPixel);
					if( !bPrint )
						m_lfFontLegend.SetHeight(long(-MulDiv( m_lfFontLegend.lfHeight, m_DCLOGPIXELSY, 72) * fZoom ));
					else {
						m_lfFontLegend.SetHeight(long(-MulDiv( m_lfFontLegend.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom ));
					}
					m_lfFontLegend.DrawString(g, CStringW(strErrorDescr), &m_rcFull, DT_CENTER|DT_VCENTER);
				}

				if( !bPrint )
				{
					RECT rcFull=m_rcFull;
					HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
					::Rectangle( hDC, rcFull.left, rcFull.top, rcFull.right - 1, rcFull.bottom - 1 );
					::SelectObject( hDC, hBrush );
				}
				::DeleteObject( hbrush );	hbrush = 0;
			}else{
				CString strErrorDescr((LPCTSTR)m_iError);
				Gdiplus::Graphics g(hDC); g.SetPageUnit(UnitPixel);
				m_lfFontLegend.MeasureString(g, CStringW(strErrorDescr), rcCalc, DT_CENTER|DT_VCENTER);
			}
			return;
		}

		int nImageWidth		= rcPaint.right  - rcPaint.left  + 1;
		int nImageHeight	= rcPaint.bottom - rcPaint.top  + 1;

		RECT rcSrc={0};

		if( nImageWidth < 1 || nImageHeight < 1 )
			return;	

		COLORREF clBackColor = 0;

		if( bPrint )
			clBackColor = RGB( 255, 255, 255 );
		else
			clBackColor = ::GetSysColor( COLOR_BTNFACE );

		HBRUSH hBrush = ::CreateSolidBrush( clBackColor );
		//		::SetMapMode( hDC, MM_TEXT );	
		{
			//			HFONT hOldFont = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);
			RECT rcPaint2 = { 0, 0, nImageWidth, nImageHeight };
			if( !rcCalc && !bPrint )
				::FillRect( hDC, &rcPaint2, hBrush );

			if( !rcCalc )
			{
				IChartAttributesPtr ptrChartAtrr = m_sptrChart;

				double fZ = 0;
				ptrChartAtrr->GetTextZoom( &fZ );

				long nR = 0, nL = 0, nT = 0, nB = 0, nSh = 0;

				ptrChartAtrr->GetConstParams( &nL, ccfLBorder );
				ptrChartAtrr->GetConstParams( &nB, ccfBBorder );
				ptrChartAtrr->GetConstParams( &nT, ccfTBorder );
				ptrChartAtrr->GetConstParams( &nR, ccfRBorder );
				ptrChartAtrr->GetConstParams( &nSh, ccfScaleHalfSize );

				//				ptrChartAtrr->SetConstParams( long( nL * fZoom * _fZoom + 0.5 ), ccfLBorder );
				ptrChartAtrr->SetConstParams( long( nB * fZoom + 0.5 ), ccfBBorder );
				ptrChartAtrr->SetConstParams( long( nT * fZoom + 0.5 ), ccfTBorder );
				ptrChartAtrr->SetConstParams( long( nR * fZoom + 0.5 ), ccfRBorder );
				ptrChartAtrr->SetConstParams( long( nSh * fZoom + 0.5 ), ccfScaleHalfSize );

				ptrChartAtrr->SetTextZoom( fZoom );
				m_sptrChart->Draw( (DWORD)hDC, ScaleRect( m_rcChart, fZoom ) );
				ptrChartAtrr->SetTextZoom( fZ );

				//				ptrChartAtrr->SetConstParams( nL, ccfLBorder );
				ptrChartAtrr->SetConstParams( nB, ccfBBorder );
				ptrChartAtrr->SetConstParams( nT, ccfTBorder );
				ptrChartAtrr->SetConstParams( nR, ccfRBorder );
				ptrChartAtrr->SetConstParams( nSh, ccfScaleHalfSize );
			}
			{
				Gdiplus::Graphics g(hDC); g.SetPageUnit(UnitPixel);

				if( !rcCalc && !bPrint )
					::SetBkColor( hDC, clBackColor ); 

				if( !rcCalc && bPrint )
					::SetBkMode( hDC, TRANSPARENT );


				if( !bPrint )
					m_lfFontTitle.SetHeight(long(-MulDiv( m_lfFontTitle.lfHeight, m_DCLOGPIXELSY, 72)  * fZoom ));
				else
					m_lfFontTitle.SetHeight(long(-MulDiv( m_lfFontTitle.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom ));

				if( !bPrint )
					m_lfFontLegend.SetHeight(long(-MulDiv( m_lfFontLegend.lfHeight, m_DCLOGPIXELSY, 72) * fZoom ));
				else {
					m_lfFontLegend.SetHeight(long(-MulDiv( m_lfFontLegend.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom * 0.9));
				}

				if( !bPrint )
					m_lfFontParam.SetHeight(long(-MulDiv( m_lfFontParam.lfHeight, m_DCLOGPIXELSY, 72) * fZoom ));
				else
					m_lfFontParam.SetHeight(long(-MulDiv( m_lfFontParam.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom ));

				if( !bPrint )
					m_lfFontParam2.SetHeight(long(-MulDiv( m_lfFontParam2.lfHeight, m_DCLOGPIXELSY, 72) * fZoom ));
				else
					m_lfFontParam2.SetHeight(long(-MulDiv( m_lfFontParam2.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom ));

				RECT rcTitle={0}, rcChart={0}, rcXParam={0}, rcLegend={0}, rcYParam={0};

				// [vanek] SBT:1260 - 20.12.2004
				CString strTitle = 1 < m_list_attached.count() ? m_strCompareTitle : m_strTitle;
				if( !rcCalc ){
					m_lfFontTitle.DrawString(g, CStringW(strTitle), &ScaleRect( m_rcTitle, fZoom ), DT_CENTER | DT_NOCLIP | DT_WORDBREAK);
				}else{
					rcSrc = *rcCalc;
					RECT rc = ScaleRect( rcSrc, fZoom );
					if( !strTitle.IsEmpty() )
					{
						m_lfFontTitle.MeasureString(g, CStringW(strTitle), &rc, DT_CENTER | DT_NOCLIP | DT_WORDBREAK );

						rcTitle=rc;
						rc = ScaleRect( rc, fZoom );

						m_nYTitle = rc.bottom - rc.top;
						*rcCalc = MaxRect( *rcCalc, rc );
					}
					else
						m_nYTitle = 0;
				}

				if( m_nShowLegend == 0 && ( m_bShowStep || m_bShowClassCount || m_bShowBeginEnd || m_bShowCheckModel ) )
				{
					RECT rcStep;
					if( rcCalc ){
						rcStep = ScaleRect( rcSrc, fZoom );
					}else{
						rcStep = ScaleRect( m_rcLegend, fZoom );
					}

					RECT rcClasses = rcStep;

					RECT rcRange = rcStep;
					RECT rcModel = rcStep;

					if( m_bShowBeginEnd && m_bShowCheckModel || m_bShowClassCount && m_bShowStep )
						rcStep.bottom = rcStep.top + ( rcStep.bottom - rcStep.top ) / 2;
					else if( !m_bShowStep )
						rcStep.bottom = rcStep.top;

					if( m_bShowBeginEnd && m_bShowCheckModel || m_bShowClassCount && m_bShowStep )
						rcRange.bottom = rcRange.top + ( rcRange.bottom - rcRange.top ) / 2;
					else if( !m_bShowBeginEnd )
						rcRange.bottom = rcRange.top;

					rcClasses.top = rcStep.bottom;
					rcModel.top   = rcRange.bottom;

					rcClasses.right = rcStep.right = rcRange.left =  rcStep.left + ( rcRange.right - rcRange.left ) / 2;
					rcModel.left = rcRange.left;

					::InflateRect( &rcStep, int(-5 * fZoom), 0 );
					::InflateRect( &rcRange, int(-5 * fZoom), 0 );

					::InflateRect( &rcClasses, int(-5 * fZoom), 0 );
					::InflateRect( &rcModel, int(-5 * fZoom), 0 );

					int y1 = 0, y2 = 0, y3 = 0, y4 = 0;

					int xMax = 0;

					RECT rcCell = {0};

					if( m_bShowStep )
					{
						if( !rcCalc )
						{
							m_lfFontLegend.DrawString( g, CStringW(m_strChartStep), &rcStep, DT_LEFT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
							m_lfFontLegend.DrawString( g, CStringW(m_strChartStepVal), &rcStep, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
						}
						else
						{
							RECT rc = rcStep;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartStep), &rc, DT_LEFT  | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP  );
							int nR = int( rc.right + 5 * fZoom );
							int tmp = rc.bottom - rc.top;
							xMax = __max( xMax, rc.right );
							*rcCalc = MaxRect( *rcCalc, rc );

							rc = rcStep;
							rc.left = nR; rc.bottom=rc.top + tmp;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartStepVal), &rc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
							xMax = __max( xMax, rc.right );
							*rcCalc = MaxRect( *rcCalc, rc );
							y1 = __max( rc.bottom - rc.top, tmp );
						}
					}

					if( m_bShowClassCount )
					{
						if( !rcCalc )
						{
							m_lfFontLegend.DrawString( g, CStringW(m_strChartClasses), &rcClasses, DT_LEFT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
							m_lfFontLegend.DrawString( g, CStringW(m_strChartClassesVal), &rcClasses, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
						}
						else
						{
							RECT rc = rcClasses;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartClasses), &rc, DT_LEFT  | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
							int nR = int( rc.right + 5 * fZoom );
							int tmp = rc.bottom - rc.top;

							*rcCalc = MaxRect( *rcCalc, rc );

							rc = rcClasses;
							rc.left = nR;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartClassesVal), &rc, DT_LEFT  | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
							xMax = int( __max( xMax, rc.right ) + 5 * fZoom );
							*rcCalc = MaxRect( *rcCalc, rc );
							y2 = __max( rc.bottom - rc.top, tmp );
						}
					}

					int xMax2 = 0;
					if( m_bShowBeginEnd )
					{
						if( !rcCalc )
						{
							m_lfFontLegend.DrawString( g, CStringW(m_strChartBeginEnd), &rcRange, DT_LEFT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
							m_lfFontLegend.DrawString( g, CStringW(m_strChartBeginEndVal), &rcRange, DT_RIGHT | DT_VCENTER | DT_SINGLELINE  |( bPrint ? DT_NOCLIP : 0 ) );
						}
						else
						{
							RECT rc = rcStep;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartBeginEnd), &rc, DT_LEFT  | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
							int nR = int( rc.right );
							*rcCalc = MaxRect( *rcCalc, rc );
							int tmp = rc.bottom - rc.top;

							//rc = rcRange;
							rc.left = rc.right;

							m_lfFontLegend.MeasureString(g, CStringW(m_strChartBeginEndVal), &rc, DT_LEFT  | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );

							xMax2 = __max( xMax2, rc.right );
							*rcCalc = MaxRect( *rcCalc, rc );

							y3 = __max( rc.bottom - rc.top, tmp );
						}
					}

					if( m_bShowCheckModel )
					{
						if( !rcCalc )
						{
							m_lfFontLegend.DrawString( g, CStringW(m_strChartModel), &rcModel, DT_LEFT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
							m_lfFontLegend.DrawString( g, CStringW(m_strChartModelVal), &rcModel, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
						}
						else
						{
							RECT rc = rcStep;
							m_lfFontLegend.MeasureString(g, CStringW(m_strChartModel), &rc, DT_LEFT | DT_SINGLELINE  | DT_TOP | DT_CALCRECT | DT_NOCLIP );
							int nR = int( rc.right);
							int tmp = rc.bottom - rc.top;
							*rcCalc = MaxRect( *rcCalc, rc );

							rc = rcModel;
							rc.left = nR;
							m_lfFontLegend.MeasureString(g, m_strChartModelVal, &rc, DT_LEFT | DT_SINGLELINE  | DT_TOP | DT_CALCRECT | DT_NOCLIP );
							xMax2 = int( __max( xMax2, rc.right )  );
							*rcCalc = MaxRect( *rcCalc, rc );

							y4 = __max( rc.bottom - rc.top, tmp );
						}
					}

					if( rcCalc )
					{
						m_nY_Legend = __max( y1 + y2 , y3 + y4 );
						xMax2 = rcStep.left + 2 * __max( xMax, xMax2 );

						xMax2 = long(xMax2);
						m_nY_Legend = long( m_nY_Legend); 


						if( xMax2 > rcCalc->right )
							rcCalc->right = xMax2;
						RECT rcMin={0,0,xMax2,m_nY_Legend};
						rcLegend = rcMin;
					}
				}
				else if( m_nShowLegend == 1 && !m_strUserSignature.IsEmpty() ) 
				{
					if( !bPrint )
						m_lfFontSignature.SetHeight(long(-MulDiv( m_lfFontSignature.lfHeight, m_DCLOGPIXELSY, 72) * fZoom));
					else
						m_lfFontSignature.SetHeight(long(-MulDiv( m_lfFontSignature.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom));

					if( !rcCalc ){
						m_lfFontSignature.DrawString(g, CStringW(m_strUserSignature), &ScaleRect(m_rcLegend,fZoom), DT_CENTER | DT_VCENTER | DT_WORDBREAK |( bPrint ? DT_NOCLIP : 0 ) );
					}
					else
					{
						RECT rc = ScaleRect( rcSrc, fZoom );
						m_lfFontSignature.MeasureString(g, CStringW(m_strUserSignature), &rc, DT_LEFT | DT_WORDBREAK | DT_CALCRECT | DT_NOCLIP );
						rcLegend=rc;
						//				rc = ScaleRect(rc, _fZoom );
						m_nY_Legend = rc.bottom - rc.top;
						rcCalc->bottom=rcCalc->top+m_nY_Legend;
						*rcCalc = MaxRect(*rcCalc, rc );
					}
				} else {
					m_nY_Legend = 0;
					rcLegend = NORECT;
				}		// end if showLegend


				RECT rcT = {0};
				m_lfFontParam2.MeasureString(g, CStringW(m_szYNameParam), &rcT, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT |( bPrint ? DT_NOCLIP : 0 ) );

				int nXParam = 0;
				if(m_szXNameParam.GetLength()>0){
					if( !rcCalc )
						m_lfFontParam.DrawString( g, CStringW(m_szXNameParam), &ScaleRect( m_rcXParam, fZoom ), DT_CENTER | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) );
					else
					{
						RECT rc = {0,0, rcSrc.right-rcSrc.left - (rcT.bottom - rcT.top), 0};
						m_lfFontParam.MeasureString(g, CStringW(m_szXNameParam), &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
						rcXParam=rc;
						nXParam = rc.right;
						m_nY_XParam = rc.bottom - rc.top;
						rcCalc->bottom += m_nY_XParam;
					}
				}else{
					m_nY_XParam = 0;
					rcXParam=NORECT;
				}

				RECT rc2 = m_rcYParam;
				rc2.top = long( ( m_rcYParam.bottom + m_rcYParam.top ) / 2 + long( rcT.right - rcT.left ) / 2 / fZoom );
				if( rcCalc )
				{
					rc2.top = m_rcYParam.top;
					rc2.bottom = rc2.top + rcT.right - rcT.left;
					rcCalc->bottom += ( rcT.right - rcT.left ) + 2*m_nY_XParam;
				}

				if( !rcCalc ){
					m_lfFontParam2.DrawString( g, CStringW(m_szYNameParam), &ScaleRect( m_rcYParam, fZoom ),
						0x10000000|DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOCLIP  );
				}
				else
				{
					RECT rc = ScaleRect( rc2, fZoom );
					m_lfFontParam2.MeasureString(g, CStringW(m_szYNameParam), &rc,  0x10000000|DT_CENTER|DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT  );
					rcYParam = _rect(0,0,rcT.bottom,rcT.right);
					m_nY_XParam = rc.bottom - rc.top;
					rc.right = long( rc.left + m_nY_XParam + nXParam);
					*rcCalc = MaxRect( *rcCalc, rc );
				}

				if( rcCalc )
				{
					if( !bPrint )
						m_lfFontAxisChart.SetHeight(long(-MulDiv( m_lfFontAxisChart.lfHeight, m_DCLOGPIXELSY, 72) * fZoom ));
					else
						m_lfFontAxisChart.SetHeight(long(-MulDiv( m_lfFontAxisChart.lfHeight, m_DCDispLOGPIXELSY, 72) * fZoom ));

					RECT rcClc = {0};

					m_lfFontAxisChart.MeasureString(g, CStringW(m_strChartY), &rcClc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
					RECT rcClc2 = {0};
					m_lfFontAxisChart.MeasureString(g, CStringW(_T("0")), &rcClc2, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );

					IChartAttributesPtr sptrAttr = m_sptrChart;
					if( sptrAttr )
					{
						if( m_bShowAxisValueY && m_strChartY.GetLength() )
						{
							sptrAttr->SetConstParams( rcClc.bottom - rcClc.top, ccfTBorder );
						}
						else
						{
							sptrAttr->SetConstParams( 5, ccfTBorder );
						}
					}
					int h = int( 2*(rcClc.bottom - rcClc.top) * m_nMaxYChart );

					if( !m_bShowAxisValueY )
						h = 0;

					int ySz = 2*(rcClc.bottom - rcClc.top);
					::ZeroMemory( &rcClc, sizeof( RECT ) );

					if( m_bShowAxisValueX )
					{
						m_lfFontAxisChart.MeasureString(g, CStringW(m_strChartX), &rcClc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP );
						if( sptrAttr )
							sptrAttr->SetConstParams( __max( (rcClc.bottom - rcClc.top ), ySz )  * 2 + m_lTextOffsetX, ccfBBorder );
					}

					int w = ( rcClc.right - rcClc.left )+ 96;
					h = __max(h, rcYParam.bottom - rcYParam.top);
					rcYParam.top=rcYParam.bottom;
					rcYParam.bottom=rcTitle.bottom+h;
					rcChart = _rect(_point(rcYParam.right-rcYParam.left,rcYParam.bottom),_size(w,h));
					RECT rc = rcYParam;
					rc.right = rc.left + w;
					rc.bottom = long( rc.top + h*m_fCoefParam );


					int ww=rcSrc.right-rcSrc.left;
					ww=__max(ww, rcTitle.right-rcTitle.left);
					ww=__max(ww, (rcYParam.right-rcYParam.left)+(rcChart.right-rcChart.left));
					ww=__max(ww, (rcYParam.right-rcYParam.left)+rcXParam.right-rcXParam.left);
					ww=__max(ww, rcLegend.right-rcLegend.left);

					int hh=rcTitle.bottom-rcTitle.top
						+ rcChart.bottom-rcChart.top
						+ rcXParam.bottom-rcXParam.top
						+ rcLegend.bottom-rcLegend.top;

					rcCalc->right = rcCalc->left + (int)(ww);
					rcCalc->bottom = rcCalc->top + (int)(hh);
				}

				if( !rcCalc )
				{
					if( m_bEnableYParamArea && !bPrint )
						::DrawFocusRect( hDC, &m_rcYParam );

					if( m_bEnableXParamArea && !bPrint )
						::DrawFocusRect( hDC, &m_rcXParam );
				}

			}
		}

		if( !rcCalc )
		{
			if( !bPrint )
			{
				HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
				RECT rcFull = ScaleRect( m_rcFull, fZoom );
				::Rectangle( hDC, rcFull.left, rcFull.top, rcFull.right - 1, rcFull.bottom - 1 );
				::SelectObject( hDC, hBrush );
			}
		}

		::DeleteObject( hBrush );
	}

	const CRect& CChartWnd::get_min_rect()
	{
		return m_rcMinRect;
	}

	void CChartWnd::load_from_ndata(  INamedDataPtr sptrND )
	{
		m_wHotBtnChart.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_CHART_AREA, "" ) );
		m_wHotBtnAxis.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_AXIS_AREA, "" ) );
		m_wHotBtnYParam.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_YPARAM_AREA, "" ) );
		m_wHotBtnXParam.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_XPARAM_AREA, "" ) );

		m_wHotBtnChart.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_CHART_AREA, 0 ) );
		m_wHotBtnAxis.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_AXIS_AREA, 0 ) );
		m_wHotBtnYParam.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_YPARAM_AREA, 0 ) );
		m_wHotBtnXParam.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_XPARAM_AREA, 0 ) );

		m_strClassName = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT, CLASSES_NAME, m_strClassName );
		m_nChartType = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, CHART_TYPE, m_nChartType );
		m_nChartViewType = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, CHART_VIEW_TYPE, m_nChartViewType );
		m_clChartColor = ::GetValueColor( sptrND, SECT_STATUI_CHART_ROOT, CHART_COLOR, m_clChartColor );


		m_list_color_chart.clear();
		if( m_list_attached.count() > 1 )
		{
			m_list_color_chart.add_tail( m_clChartColor );

			for (TPOS lpos_lst = m_list_attached.next(m_list_attached.head()); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst))
			{
				INamedDataPtr sptr_nd = m_list_attached.get( lpos_lst );
				COLORREF cl_chart = ::GetValueColor( sptr_nd, SECT_STATUI_CHART_ROOT, CHART_COLOR, m_clChartColor );
				m_list_color_chart.add_tail( cl_chart );
			}
		}

		m_bShowGrayScale = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_GRAY_SCALE, (long)m_bShowGrayScale ) != 0;
		m_nGrayScaleLevel = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, GRAY_SCALE_LEVEL, m_nGrayScaleLevel );

		m_bShowSingleColorBar = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_SINGLE_COLORED_BAR, (long)m_bShowSingleColorBar ) != 0;
		m_clBarSingleColor = ::GetValueColor( sptrND, SECT_STATUI_CHART_ROOT, BAR_SINGLE_COLOR, m_clBarSingleColor );

		m_strTitle = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT, CHART_TITLE, m_strTitle );
		m_nShowLegend = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_LEGEND, (long)m_nShowLegend );

		m_nClassStep = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, CLASS_STEP, (long)m_nClassStep );
		m_nMinClass = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, MIN_CLASS_INDEX, (long)m_nMinClass );


		m_strTitle = _change_chars( m_strTitle, "\\n", "\r\n" );

		// [vanek] SBT:1260 - 20.12.2004
		if( 1 < m_list_attached.count() )
		{	// более одного объекта - режим сравнения
			m_strCompareTitle = (LPCTSTR)::GetValueString(sptrND, SECT_STATCMP_CHART_ROOT, CHART_TITLE, m_strCompareTitle );
			m_strCompareTitle = _change_chars( m_strCompareTitle, "\\n", "\r\n" );
		}

		m_bShowBeginEnd = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_BEGIN_END, (long)m_bShowBeginEnd ) != 0;
		m_bShowStep = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_STEP, (long)m_bShowStep ) != 0;
		m_bShowClassCount = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CLASS_COUNT, (long)m_bShowClassCount ) != 0;
		m_bShowCheckModel = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CHECK_MODEL, (long)m_bShowCheckModel ) != 0;
		m_nCheckModelType = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, CHECK_MODEL_TYPE, (long)m_nCheckModelType );
		m_strUserSignature = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT, USER_SIGNATURE, m_strUserSignature );

		m_strUserSignature = _change_chars( m_strUserSignature, "\\n", "\r\n" );

		m_fBarWidth = ::GetValueDouble( sptrND, SECT_STATUI_CHART_ROOT, BAR_WIDTH, m_fBarWidth );

		m_bShowCurve = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CURVE, (long)m_bShowCurve ) != 0; 
		m_lUniformDistribution = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, UNIFORM_DISTRIBUTION, (long)m_lUniformDistribution );
		m_clCurveColor = ::GetValueColor( sptrND, SECT_STATUI_CHART_ROOT, CURVE_COLOR, m_clCurveColor );

		m_bShowUserCurve = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, SHOW_USERCURVE, (long)m_bShowUserCurve ) != 0;
		m_clUserCurveColor = ::GetValueColor( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_COLOR, m_clUserCurveColor );
		m_fUserCurveMX = ::GetValueDouble( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_MX, m_fUserCurveMX ); 
		m_fUserCurveDX = ::GetValueDouble( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_DX, m_fUserCurveDX ); 

		m_list_classes.clear();
		{
			CString str_data = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT, VISIBLE_CLASSES, "" );
			CString *pstr = 0;
			long lcount = 0;
			split( str_data, ",", &pstr, &lcount );

			for( long i = 0; i < lcount; i++ )
				m_list_classes.add_tail( atoi( pstr[i] ) );

			if( pstr )
				delete []pstr;
		}


		// [vanek] - 18.11.2004
		m_list_color_curve.clear();
		if( m_list_attached.count() > 1 )
		{
			m_list_color_curve.add_tail( m_clCurveColor );

			for (TPOS lpos_lst = m_list_attached.next(m_list_attached.head()); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst))
			{
				INamedDataPtr sptr_nd = m_list_attached.get( lpos_lst );
				COLORREF cl_curve = ::GetValueColor( sptr_nd, SECT_STATUI_CHART_ROOT, CURVE_COLOR, m_clCurveColor );
				m_list_color_curve.add_tail( cl_curve );
			}
		}

		m_b_use_custom_format_x = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_CUSTOM_FORMAT, m_b_use_custom_format_x ) != 0;
		m_b_use_custom_format_y = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_USE_CUSTOM_FORMAT, m_b_use_custom_format_y ) != 0;

		m_str_custom_format_x = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_CUSTOM_FORMAT, m_str_custom_format_x );
		m_str_custom_format_y = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_CUSTOM_FORMAT, m_str_custom_format_y );

		m_bUseLogaScaleX = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_LOGA_SCALE, m_bUseLogaScaleX ) != 0;

		m_nXAxisType = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_AXIS_TYPE, m_nXAxisType );
		m_nXParamKey = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, m_nXParamKey );
		m_nYParamKey = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_PARAM_KEY, m_nYParamKey );
		switch(m_nYParamKey){
			default:
				m_nYParamKey=PARAM_COUNT;
				break;
			case PARAM_COUNT:
			case PARAM_AREAPERC:
			case PARAM_COUNTPERC:
			case PARAM_3DPART:
				break;
		}
		m_bXGrid = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_GRID, m_bXGrid ) != 0; 
		m_bYGrid = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_GRID, m_bYGrid ) != 0;
		m_bConvertToCalibr = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, CONVERT_TO_CALIBR, m_bConvertToCalibr ) != 0;

		m_nParamsHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, CHART_PARAM_HEIGHT, m_nParamsHeight );

		m_fXScaleFactor = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR, m_fXScaleFactor ); 
		m_fYScaleFactor = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR, m_fYScaleFactor );

		m_fXScaleFactorValues = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR_VALUES, m_fXScaleFactorValues ); 
		m_fYScaleFactorValues = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR_VALUES, m_fYScaleFactorValues );

		m_fXScaleFactorGrid = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR_GRID, m_fXScaleFactorGrid ); 
		m_fYScaleFactorGrid = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR_GRID, m_fYScaleFactorGrid );

		m_clGridColor = ::GetValueColor( sptrND, SECT_STATUI_CHART_AXIS_ROOT, GRID_COLOR, m_clGridColor );
		m_bShowAxisValueX = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SHOW_AXIS_VALUES, m_bShowAxisValueX ) != 0;
		m_bShowAxisValueY = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SHOW_AXIS_VALUES, m_bShowAxisValueY ) != 0;

		m_bShowAxisLabelsX = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SHOW_AXIS_LABELS, m_bShowAxisLabelsX ) != 0;
		m_bShowAxisLabelsY = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SHOW_AXIS_LABELS, m_bShowAxisLabelsY ) != 0;

		m_lTextOffsetX = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_TEXT_OFFSET, m_lTextOffsetX );
		m_lTextOffsetY = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_TEXT_OFFSET, m_lTextOffsetY );


		m_fX_AxisBegin = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_BEGIN, m_fX_AxisBegin );
		m_fX_AxisEnd = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_END, m_fX_AxisEnd );
		m_bX_UseValueRange = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_VALUE_RANGE, m_bX_UseValueRange ) != 0;

		m_fY_AxisBegin = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_BEGIN, m_fY_AxisBegin );
		m_fY_AxisEnd = ::GetValueDouble( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_END, m_fY_AxisEnd );
		m_bY_UseValueRange = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_USE_VALUE_RANGE, m_bY_UseValueRange ) != 0;

		strcpy( m_lfFontTitle.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_NAME , m_lfFontTitle.lfFaceName ) );
		m_lfFontTitle.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_SIZE, m_lfFontTitle.lfHeight );
		m_lfFontTitle.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_BOLD, m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontTitle.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_ITALIC, m_lfFontTitle.lfItalic ) != 0;
		m_lfFontTitle.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_UNDERLINE, m_lfFontTitle.lfUnderline ) != 0;
		m_lfFontTitle.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_STRIKEOUT, m_lfFontTitle.lfStrikeOut) != 0;
		m_lfFontTitle.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, TEXT_COLOR , m_lfFontTitle.m_clrText );
		m_lfFontTitle.Delete();

		strcpy( m_lfFontLegend.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_NAME , m_lfFontLegend.lfFaceName ) );
		m_lfFontLegend.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_SIZE, m_lfFontLegend.lfHeight );
		m_lfFontLegend.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_BOLD, m_lfFontLegend.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontLegend.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_ITALIC, m_lfFontLegend.lfItalic ) != 0;
		m_lfFontLegend.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_UNDERLINE, m_lfFontLegend.lfUnderline ) != 0;
		m_lfFontLegend.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_STRIKEOUT, m_lfFontLegend.lfStrikeOut) != 0;
		m_lfFontLegend.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, TEXT_COLOR , m_lfFontLegend.m_clrText );
		m_lfFontLegend.Delete();

		strcpy( m_lfFontAxisChart.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_NAME , m_lfFontAxisChart.lfFaceName ) );
		m_lfFontAxisChart.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_SIZE, m_lfFontAxisChart.lfHeight );
		m_lfFontAxisChart.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_BOLD, m_lfFontAxisChart.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontAxisChart.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_ITALIC, m_lfFontAxisChart.lfItalic ) != 0;
		m_lfFontAxisChart.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_UNDERLINE, m_lfFontAxisChart.lfUnderline ) != 0;
		m_lfFontAxisChart.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_STRIKEOUT, m_lfFontAxisChart.lfStrikeOut) != 0;
		m_lfFontAxisChart.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, TEXT_COLOR , m_lfFontAxisChart.m_clrText );
		m_lfFontAxisChart.Delete();

		strcpy( m_lfFontHint.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_NAME , m_lfFontHint.lfFaceName ) );
		m_lfFontHint.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_SIZE, m_lfFontHint.lfHeight );
		m_lfFontHint.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_BOLD, m_lfFontHint.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontHint.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_ITALIC, m_lfFontHint.lfItalic ) != 0;
		m_lfFontHint.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_UNDERLINE, m_lfFontHint.lfUnderline ) != 0;
		m_lfFontHint.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_STRIKEOUT, m_lfFontHint.lfStrikeOut) != 0;
		m_lfFontHint.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, TEXT_COLOR , m_lfFontHint.m_clrText );
		m_lfFontHint.Delete();

		strcpy( m_lfFontParam.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_NAME , m_lfFontParam.lfFaceName ) );
		m_lfFontParam.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_SIZE, m_lfFontParam.lfHeight );
		m_lfFontParam.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_BOLD, m_lfFontParam.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontParam.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_ITALIC, m_lfFontParam.lfItalic ) != 0;
		m_lfFontParam.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_UNDERLINE, m_lfFontParam.lfUnderline ) != 0;
		m_lfFontParam.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_STRIKEOUT, m_lfFontParam.lfStrikeOut) != 0;
		m_lfFontParam.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, TEXT_COLOR , m_lfFontParam.m_clrText );
		m_lfFontParam.Delete();

		m_lfFontParam2.lfHeight =	m_lfFontParam.lfHeight;
		m_lfFontParam2.lfWeight =	m_lfFontParam.lfWeight;
		m_lfFontParam2.lfItalic =	m_lfFontParam.lfItalic;
		m_lfFontParam2.lfUnderline = m_lfFontParam.lfUnderline;
		m_lfFontParam2.lfStrikeOut = m_lfFontParam.lfStrikeOut;
		m_lfFontParam2.m_clrText = m_lfFontParam.m_clrText;
		m_lfFontParam.Delete();

		strcpy( m_lfFontSignature.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_NAME , m_lfFontSignature.lfFaceName ) );
		m_lfFontSignature.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_SIZE, m_lfFontSignature.lfHeight );
		m_lfFontSignature.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_BOLD, m_lfFontSignature.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontSignature.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_ITALIC, m_lfFontSignature.lfItalic ) != 0;
		m_lfFontSignature.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_UNDERLINE, m_lfFontSignature.lfUnderline ) != 0;
		m_lfFontSignature.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_STRIKEOUT, m_lfFontSignature.lfStrikeOut) != 0;
		m_lfFontSignature.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, TEXT_COLOR , m_lfFontSignature.m_clrText );
		m_lfFontSignature.Delete();

		m_bEnableChartArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_CHART_AREA, m_bEnableChartArea ) != 0;
		m_bEnableAxisArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_AXIS_AREA, m_bEnableAxisArea ) != 0;
		m_bEnableXParamArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_X_PARAMS_AREA, m_bEnableXParamArea ) != 0;
		m_bEnableYParamArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_Y_PARAMS_AREA, m_bEnableYParamArea ) != 0;

		m_szDBClickScript = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT, DISTRIBUTION_SCRIPT , m_szDBClickScript );
		m_nHotZone = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, HALF_HOT_ZONE , m_nHotZone );

		m_bDrawEndChartGridLineX = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_X , m_bDrawEndChartGridLineX ) != 0;
		m_bDrawEndChartGridLineY = ::GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_Y , m_bDrawEndChartGridLineY ) != 0;

		if( m_pstrCustomParams )
			delete []m_pstrCustomParams; m_pstrCustomParams = 0;

		m_bSteppedParamChart = ::GetValueInt( sptrND, SECT_STATUI_CHART_ROOT, USE_CUSTOM_INTERVALS, m_bSteppedParamChart ) != 0;

		if( m_bSteppedParamChart )
		{
			sptrND->SetupSection( _bstr_t( SECT_STATUI_CHART_ROOT SECT_CUSTOM_INTERVALS ) );
			m_lCustomParamCount = 0;
			sptrND->GetEntriesCount( &m_lCustomParamCount );

			if( m_lCustomParamCount )
			{
				m_pstrCustomParams = new CString[m_lCustomParamCount];

				for( int i = 0; i < m_lCustomParamCount; i++ )
				{
					CString str;
					str.Format( "class%d", i );
					m_pstrCustomParams[i] = (char*)::GetValueString( sptrND, SECT_STATUI_CHART_ROOT SECT_CUSTOM_INTERVALS, str, "" );
				}
			}
		}

		IApplicationPtr sptrP = sptrND;
		if( sptrP == 0 )
			strcpy( m_strClassFile, (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CLASS_FILE, m_strClassFile ) );
	}

	void CChartWnd::store_to_ndata( INamedDataPtr sptrND )
	{
		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_CHART_AREA, m_wHotBtnChart.GetAction() );
		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_AXIS_AREA, m_wHotBtnAxis.GetAction() );
		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_XPARAM_AREA, m_wHotBtnYParam.GetAction() );
		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_YPARAM_AREA, m_wHotBtnXParam.GetAction() );

		::SetValue( sptrND, SECT_STATUI_ROOT, INDEX_CHART_AREA, m_wHotBtnChart.GetIndex() );
		::SetValue( sptrND, SECT_STATUI_ROOT, INDEX_AXIS_AREA, m_wHotBtnAxis.GetIndex() );
		::SetValue( sptrND, SECT_STATUI_ROOT, INDEX_YPARAM_AREA, m_wHotBtnYParam.GetIndex() );
		::SetValue( sptrND, SECT_STATUI_ROOT, INDEX_XPARAM_AREA, m_wHotBtnXParam.GetIndex() );



		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CLASSES_NAME, m_strClassName );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CHART_TYPE, (long)m_nChartType );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CHART_VIEW_TYPE, (long)m_nChartViewType );
		::SetValueColor( sptrND, SECT_STATUI_CHART_ROOT, CHART_COLOR, m_clChartColor );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_GRAY_SCALE, (long)m_bShowGrayScale );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, GRAY_SCALE_LEVEL, (long)m_nGrayScaleLevel );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CHART_TITLE, _change_chars( m_strTitle, "\r\n", "\\n" ) );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_LEGEND, (long)m_nShowLegend );
		::SetValueColor( sptrND, SECT_STATUI_CHART_ROOT, CURVE_COLOR, m_clCurveColor );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CLASS_STEP, (long)m_nClassStep );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, MIN_CLASS_INDEX, (long)m_nMinClass );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_BEGIN_END, (long)m_bShowBeginEnd );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_STEP, (long)m_bShowStep );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CLASS_COUNT, (long)m_bShowClassCount );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CHECK_MODEL, (long)m_bShowCheckModel );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CHECK_MODEL_TYPE, (long)m_nCheckModelType );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, USER_SIGNATURE, _change_chars( m_strUserSignature, "\r\n", "\\n" ) );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_CURVE, (long)m_bShowCurve ); 
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, UNIFORM_DISTRIBUTION, (long)m_lUniformDistribution);

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_USERCURVE, (long)m_bShowUserCurve );
		::SetValueColor( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_COLOR, m_clUserCurveColor );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_MX, (double)m_fUserCurveMX ); 
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, USERCURVE_DX, (double)m_fUserCurveDX ); 


		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, BAR_WIDTH, m_fBarWidth );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, CHART_PARAM_HEIGHT, m_nParamsHeight );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_LOGA_SCALE, (long)m_bUseLogaScaleX );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_AXIS_TYPE, (long)m_nXAxisType );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, (long)m_nXParamKey );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_PARAM_KEY, (long)m_nYParamKey );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_GRID, (long)m_bXGrid ); 
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_GRID, (long)m_bYGrid );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, CONVERT_TO_CALIBR, (long)m_bConvertToCalibr );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_CUSTOM_FORMAT, (long)m_b_use_custom_format_x );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_USE_CUSTOM_FORMAT, (long)m_b_use_custom_format_y );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_CUSTOM_FORMAT, m_str_custom_format_x );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_CUSTOM_FORMAT, m_str_custom_format_y );


		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR, m_fXScaleFactor ); 
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR, m_fYScaleFactor );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR_VALUES, m_fXScaleFactorValues ); 
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR_VALUES, m_fYScaleFactorValues );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_TEXT_OFFSET, m_lTextOffsetX );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_TEXT_OFFSET, m_lTextOffsetY );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SCALE_FACTOR_GRID, m_fXScaleFactorGrid ); 
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SCALE_FACTOR_GRID, m_fYScaleFactorGrid );

		::SetValueColor( sptrND, SECT_STATUI_CHART_AXIS_ROOT, GRID_COLOR, m_clGridColor );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SHOW_AXIS_VALUES, (long)m_bShowAxisValueX );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SHOW_AXIS_VALUES, (long)m_bShowAxisValueY );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_SHOW_AXIS_LABELS, (long)m_bShowAxisLabelsX );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_SHOW_AXIS_LABELS, (long)m_bShowAxisLabelsY );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_BEGIN, m_fX_AxisBegin );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_END, m_fX_AxisEnd );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_USE_VALUE_RANGE, (long)m_bX_UseValueRange );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_BEGIN, m_fY_AxisBegin );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_END, m_fY_AxisEnd );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_ROOT, Y_USE_VALUE_RANGE, (long)m_bY_UseValueRange );


		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_NAME , m_lfFontTitle.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_SIZE, (long)( m_lfFontTitle.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_BOLD, (long)( m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_ITALIC, (long)m_lfFontTitle.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontTitle.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontTitle.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_TITLE_FONT_ROOT, TEXT_COLOR , m_lfFontTitle.m_clrText );

		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_NAME , m_lfFontLegend.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_SIZE, (long)( m_lfFontLegend.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_BOLD, (long)( m_lfFontLegend.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_ITALIC, (long)m_lfFontLegend.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontLegend.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontLegend.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_LEGEND_FONT_ROOT, TEXT_COLOR , m_lfFontLegend.m_clrText );

		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_NAME , m_lfFontAxisChart.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_SIZE, (long)( m_lfFontAxisChart.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_BOLD, (long)( m_lfFontAxisChart.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_ITALIC, (long)m_lfFontAxisChart.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontAxisChart.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontAxisChart.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_AXIS_FONT_ROOT, TEXT_COLOR , m_lfFontAxisChart.m_clrText );

		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_NAME , m_lfFontHint.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_SIZE, (long)( m_lfFontHint.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_BOLD, (long)( m_lfFontHint.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_ITALIC, (long)m_lfFontHint.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontHint.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontHint.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_HINT_FONT_ROOT, TEXT_COLOR , m_lfFontHint.m_clrText );

		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_NAME , m_lfFontParam.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_SIZE, (long)( m_lfFontParam.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_BOLD, (long)( m_lfFontParam.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_ITALIC, (long)m_lfFontParam.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontParam.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontParam.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_PARAM_FONT_ROOT, TEXT_COLOR , m_lfFontParam.m_clrText );

		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_NAME , m_lfFontSignature.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_SIZE, (long)( m_lfFontSignature.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_BOLD, (long)( m_lfFontSignature.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_ITALIC, (long)m_lfFontSignature.lfItalic );
		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontSignature.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontSignature.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_CHART_SIGNATURE_FONT_ROOT, TEXT_COLOR , m_lfFontSignature.m_clrText );

		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_CHART_AREA, (long)m_bEnableChartArea );
		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_AXIS_AREA, (long)m_bEnableAxisArea );
		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_X_PARAMS_AREA, (long)m_bEnableXParamArea );
		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_Y_PARAMS_AREA, (long)m_bEnableYParamArea );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, DISTRIBUTION_SCRIPT , m_szDBClickScript );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, HALF_HOT_ZONE , (long)m_nHotZone );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, DRAW_END_CHART_GRID_LINE_X , (long)m_bDrawEndChartGridLineX );
		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, DRAW_END_CHART_GRID_LINE_Y , (long)m_bDrawEndChartGridLineY );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, SHOW_SINGLE_COLORED_BAR, (long)m_bShowSingleColorBar );
		::SetValueColor( sptrND, SECT_STATUI_CHART_ROOT, BAR_SINGLE_COLOR, m_clBarSingleColor );

		::SetValue( sptrND, SECT_STATUI_CHART_ROOT, USE_CUSTOM_INTERVALS, (long)m_bSteppedParamChart );

		if( m_list_classes.count() )
		{
			CString str_data;
			for (TPOS lpos = m_list_classes.head(); lpos; lpos = m_list_classes.next(lpos))
			{
				long lclass = m_list_classes.get( lpos );

				CString str;
				str.Format( "%d", lclass );

				if( str_data.IsEmpty() )
					str_data = str;
				else
					str_data += "," + str;
			}

			::SetValue( sptrND, SECT_STATUI_CHART_ROOT, VISIBLE_CLASSES, str_data );
		}

		if( m_bSteppedParamChart )
		{
			if( m_pstrCustomParams )
			{
				for( int i = 0; i < m_lCustomParamCount; i++ )
				{
					CString str;
					str.Format( "class%d", i );
					::SetValue( sptrND, SECT_STATUI_CHART_ROOT SECT_CUSTOM_INTERVALS, str, m_pstrCustomParams[i] );
				}
			}
		}
	}

	void CChartWnd::attach_data( )
	{
		if( m_list_attached.count() == 0 )
			return;

		m_bMinSizeCalc = false;
		m_bGistFilled = false;
		m_vectParams.clear();
		m_vectParamsStr.clear();
		m_vectParamsRect.clear();

		bool bOK = false;

		_calc_gist_value();
		{
			RECT rc = {0};
			::GetClientRect( m_hwnd, &rc );

			_resize_child( rc );
			::InvalidateRect( handle(), 0, 0 );
		}
	}

	void CChartWnd::calc_min_rect( BOOL /*bPrintMode*/ )
	{
		if( !m_bMinSizeCalc )
		{
			{
				m_nYTitle = 0;
				m_nY_XParam = 0;
				m_nY_Legend = 0;
				RECT rc = {0, 0, 100, 100 };
			}

			HDC hdc = ::GetDC( handle() );

			RECT rcMin = {0,0,1,0};

			DoDraw( hdc, m_rcFull, &rcMin );
			int MinWidth=rcMin.right-rcMin.left;

			for(int it=0; it<3; ++it){
				int w=rcMin.right-rcMin.left;
				int h=rcMin.bottom-rcMin.top;
				if( h>w || w > h*16/9 && w>MinWidth ){
					double S=w*h;
					int wNew=(int)sqrt(S*4./3.);
					if(wNew<MinWidth)
						wNew=MinWidth;
					RECT rc={0,0,wNew,0};
					rcMin=rc;
				}else break;
				DoDraw( hdc, m_rcFull, &rcMin );
			}

			m_rcMinRect=rcMin;

			m_rcMinRect = rcMin;
			if( ( m_rcMinRect.bottom - m_rcMinRect.top ) < __min )
				m_rcMinRect.bottom = m_rcMinRect.top + __min;
			if( ( m_rcMinRect.right - m_rcMinRect.left ) < __min )
				m_rcMinRect.right = m_rcMinRect.left + __min;

			::InflateRect(&m_rcMinRect,15,15);

			::OffsetRect( &m_rcMinRect, -m_rcMinRect.left, -m_rcMinRect.top );

			::ReleaseDC( handle(), hdc );

			m_bMinSizeCalc = true;
			{
				RECT rc;
				::GetClientRect( m_hwnd, &rc );
				_resize_child( rc );
			}
		}
	}

	void CChartWnd::_calc_gist_value()
	{
		bool bUseLogaScaleX = m_bUseLogaScaleX && m_nXAxisType;

		m_sptrChart->ClearAll();
		IChartAttributesPtr ptrChartAtrr = m_sptrChart;

		ptrChartAtrr->SetPixelUnit( 2 );

		ptrChartAtrr->SetConstParams( 0, ccfScaleDirectionY ) ;
		ptrChartAtrr->SetConstParams( 2, ccfScaleDirectionX ) ;

		ptrChartAtrr->SetConstParams( m_lTextOffsetX, ccfTextOffsetX ) ;
		ptrChartAtrr->SetConstParams( 0, ccfTextOffsetY ) ;


		IStatObjectDispPtr sptr_stat_disp = m_list_attached.get( m_list_attached.head() );
		IStatObjectPtr sptr_stat_object = m_list_attached.get( m_list_attached.head() );
		INamedDataPtr pStatObjND=sptr_stat_object;
		ObjectSpace::CStatisticObject* rso=(ObjectSpace::CStatisticObject*)((IStatObject*)sptr_stat_object);
		map_meas_params&	map_meas_params=rso->m_map_meas_params;

		double fCoefX = 1;
		CString strRangeFormat;
		{
			if( !m_nXAxisType )
				m_szXNameParam = m_strClassName;
			else if(ParameterDescriptor_ex* pmp = map_meas_params(m_nXParamKey)){
				fCoefX = pmp->fCoeffToUnits;
				m_szXNameParam = pmp->bstrUserName && *pmp->bstrUserName ? pmp->bstrUserName : pmp->bstrName;
				if(pmp->bstrUnit){
					m_szXNameParam += CString(", ") + pmp->bstrUnit;
				}
				strRangeFormat = pmp->bstrDefFormat;
			}
			if(0==strRangeFormat.GetLength())
				strRangeFormat="%g";
		}

		ptrChartAtrr->SetStyle( CommonChartViewParam( cfAxisOnBorder | cfAbsolute ) );

		if( m_bXGrid || m_bYGrid )
		{
			ptrChartAtrr->SetColor( m_clGridColor, ccfGridColor );
			ptrChartAtrr->EnableGridLines( CommonChartGrid( 
				( m_bXGrid ? ccgXGrid : 0 )
				| ( m_bYGrid ? ccgYGrid : 0 ) 
				| ( m_bDrawEndChartGridLineX ? ccgXEndGrid : 0 )  
				| ( m_bDrawEndChartGridLineY ? ccgYEndGrid : 0 ) ) );
		}



		double fCoefY = 1;
		{
			sptr_stat_object->GetParamInfo( m_nYParamKey, &fCoefY, 0);
		}


		{
			long lClassCount = 0;
			sptr_stat_disp->GetClassCount( &lClassCount );
			m_lClassCount = lClassCount;
		}

		m_iError = IDS_ALL_YVALUES_ZERO;
		for (TPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst))
		{
			IStatObjectDispPtr ptr_object = m_list_attached.get( lpos_lst );

			for( int i = 0; i < m_lClassCount; i++ )
			{
				long lClass = 0;
				ptr_object->GetClassValue( i, &lClass ); 

				double fValY = 0;
				ptr_object->GetValueInUnit2( lClass, m_nYParamKey, &fValY );

				if( fValY != 0 )
				{
					m_iError = 0;
					break;
				}

			}
		}

		long *plClassArray = new long[m_lClassCount];

		for( long i = 0; i < m_lClassCount; i++ )
		{
			long lClass = 0;
			sptr_stat_disp->GetClassValue( i, &lClass ); 
			plClassArray[i] = lClass;
		}

		if( m_arrClasses )
			delete []m_arrClasses; m_arrClasses = 0;

		if( m_parrParamChartMin )
			delete []m_parrParamChartMin; m_parrParamChartMin = 0;

		if( m_parrParamChartMax )
			delete []m_parrParamChartMax; m_parrParamChartMax = 0;

		if( _is_stepped_param_chart() )
		{
			m_parrParamChartMin = new double[m_lClassCount];
			m_parrParamChartMax = new double[m_lClassCount];

			::ZeroMemory( m_parrParamChartMin, sizeof( double ) * m_lClassCount );
			::ZeroMemory( m_parrParamChartMax, sizeof( double ) * m_lClassCount );

			for( int i = 0; i < m_lClassCount; i++ )
			{
				if( i < m_lCustomParamCount )
				{
					CString str = m_pstrCustomParams[i];

					if( !str.IsEmpty() )
					{
						int nIndex = str.Find( "," );
						if( nIndex != -1 )
						{
							CString strA = str.Left( nIndex );
							CString strB = str.Right( str.GetLength() -  nIndex - 1 );

							m_parrParamChartMin[i]  = atof( strA );
							m_parrParamChartMax[i] = atof( strB );
						}
					}
				}
				else
					break;
			}
		}

		m_strChartClassesVal.Format( "%ld", m_lClassCount );

		if( m_nCheckModelType == 0 ){
			m_strChartModelVal.LoadString( IDS_PARAM_CHI2_PROB );
			m_strChartModelVal += L" \x03c7\x00b2";
		}else
			m_strChartModelVal.LoadString( IDS_FISHER );


		if( !m_nXAxisType ) 
		{	// От класса
			strRangeFormat="";
			m_strChartStepVal.Format( strRangeFormat, m_fBarWidth );
			//if( !m_bX_UseValueRange )
			//	m_strChartBeginEndVal.Format( "( " + strRangeFormat + " - " + strRangeFormat + " )", 0.0, double( m_fBarWidth * m_lClassCount ) );
			//else
			m_strChartBeginEndVal.Format( "( " + strRangeFormat + " - " + strRangeFormat + " )", m_fX_AxisBegin * fCoefX, m_fX_AxisEnd * fCoefX );
		}
		else	
		{	// От параметра
			// [vanek] BT2000:4207 - 16.12.2004
			m_strChartStepVal.Format( strRangeFormat, m_fBarWidth * fCoefX );
			if( m_bX_UseValueRange )
				m_strChartBeginEndVal.Format( "( " + strRangeFormat + " - " + strRangeFormat + " )", m_fX_AxisBegin * fCoefX, m_fX_AxisEnd * fCoefX );
		}


		{
			CString str;
			str.Format( "\\%ld", m_nYParamKey );
			m_szYNameParam = (char*)::GetValueString( m_list_attached.get( m_list_attached.head() ), SECT_STATUI_STAT_PARAM_ROOT + str,  PARAM_NAME, "" );
		}

		{
			_bstr_t bstrUnit; 
			sptr_stat_object->GetParamInfo( m_nYParamKey, 0, bstrUnit.GetAddress() );

			if( bstrUnit.length() > 0 )
			{
				m_szYNameParam += ", ";
				m_szYNameParam += (char*)bstrUnit;
			}
		}

		double fXMinVal = 1e308, fXMaxVal = -1e307;
		for (TPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst))
		{
			IStatObjectDispPtr ptr_object = m_list_attached.get( lpos_lst );

			if( m_nXAxisType  ) // От параметра
			{
				for( long i = 0; i < m_lClassCount; i++ )	// Проверка на наличие пересечений
				{
					double fMin = 0, fMax = 0;

					ptr_object->GetValueInUnit2( plClassArray[i], PARAM_MIN, &fMin );
					ptr_object->GetValueInUnit2( plClassArray[i], PARAM_MAX, &fMax );

					bool bBreak = false;

					{
						long *pArr = new long[m_lClassCount];
						::CopyMemory( pArr, plClassArray, sizeof( long ) * m_lClassCount );
						for( long i = 0; i < m_lClassCount; i++ )
						{
							double fMin = 0; int nID = -1;
							ptr_object->GetValueInUnit2( pArr[i], PARAM_MIN, &fMin );

							for( long j = i; j < m_lClassCount; j++ )
							{
								double fValue = 0;
								ptr_object->GetValueInUnit2( pArr[j], PARAM_MIN, &fValue );
								if( fValue < fMin )
								{
									fMin = fValue;
									nID = j; 
								}
							}

							if( nID != - 1 )
							{
								long lClass = pArr[nID];
								pArr[nID] = pArr[i];
								pArr[i] = lClass;
							}
						}

						double fMin = 0, fMax = 0;
						ptr_object->GetValueGlobalInUnit2( PARAM_MIN, &fMin );
						ptr_object->GetValueGlobalInUnit2( PARAM_MAX, &fMax );

						if( fMin < fXMinVal )
							fXMinVal = fMin; 

						if( fMax > fXMaxVal )
							fXMaxVal = fMax;


						delete []pArr;
					}
					if( bBreak )
						break;
				}

				// Compute left and right x edges of Chart
				if(m_bX_UseValueRange){
					double _Min=m_fX_AxisBegin * ( !m_nXAxisType ? 1 : fCoefX );
					if(!(bUseLogaScaleX && _Min<=0.))
						fXMinVal=_Min;
				}
				fXMaxVal = !m_bX_UseValueRange ? fXMaxVal : m_fX_AxisEnd * ( !m_nXAxisType ? 1 : fCoefX );
				if(bUseLogaScaleX){
					if(fXMinVal<=0.){
						fXMinVal=fXMaxVal/1000.;
						if(fXMinVal<=0.){
							m_iError=IDS_LOGSCALE_MAX_INVALID;
							return;
						}
					}
					if(m_bShowAxisValueX){
						dblArray ValsX;
						_get_loga_scale( ValsX, fXMinVal, fXMaxVal, 10.);
						fXMinVal=ValsX[0];
						fXMaxVal=ValsX[ValsX.size()-1];
					}
				}

				if( !m_bX_UseValueRange )
					m_strChartBeginEndVal.Format( "( " + strRangeFormat + " - " + strRangeFormat + " )", fXMinVal, fXMaxVal );
				else
					m_strChartBeginEndVal.Format( "( " + strRangeFormat + " - " + strRangeFormat + " )", m_fX_AxisBegin * fCoefX, m_fX_AxisEnd * fCoefX );

				if( !_is_stepped_param_chart() )
				{
					if( !m_nChartViewType ) // Если столбовой
					{	
						if( !(1==m_nChartType) )
						{
							// 
							for( long i = 0; i < m_lClassCount; i++ )
							{
								double fValY = 0; int nID = -1;
								ptr_object->GetValueInUnit2( plClassArray[i], m_nYParamKey, &fValY );

								for( long j = i + 1; j < m_lClassCount; j++ )
								{
									double fValueY = 0;
									ptr_object->GetValueInUnit2( plClassArray[j], m_nYParamKey, &fValueY );
									if( fValueY > fValY )
									{
										fValY = fValueY;
										nID = j; 
									}
								}

								if( nID != - 1 )
								{
									long lClass = plClassArray[nID];
									plClassArray[nID] = plClassArray[i];
									plClassArray[i] = lClass;
								}
							}
						}
					}
					else
					{
						// Сортировка по в порядке вохрастания иксов
						if( !(1==m_nChartType) )
						{
							for( long i = 0; i < m_lClassCount; i++ )
							{
								double fMin = 0, fVar = 0; int nID = -1;
								ptr_object->GetValueInUnit2( plClassArray[i], PARAM_MIN, &fMin );
								ptr_object->GetValueInUnit2( plClassArray[i], PARAM_VAR, &fVar );

								for( long j = i + 1; j < m_lClassCount; j++ )
								{
									double fValue = 0, fValue2 = 0;
									ptr_object->GetValueInUnit2( plClassArray[j], PARAM_MIN, &fValue );
									ptr_object->GetValueInUnit2( plClassArray[j], PARAM_VAR, &fValue2 );

									if( fValue + fValue2/2 > fMin + fVar/2 )
									{
										fMin = fValue;
										fVar = fValue2;
										nID = j; 
									}
								}

								if( nID != - 1 )
								{
									long lClass = plClassArray[nID];
									plClassArray[nID] = plClassArray[i];
									plClassArray[i] = lClass;
								}
							}

							long lSkip = 0;
							for( long i = 0; i < m_lClassCount; i++ )
							{
								double fValue = 0;
								ptr_object->GetValueInUnit2( plClassArray[i], PARAM_COUNT, &fValue );

								if( fValue == 0 )
									lSkip++;
							}

							m_lClassCount -= lSkip;
						}
					}
				}
			}
		}

		m_arrClasses = new long[m_lClassCount];
		memcpy( m_arrClasses, plClassArray, sizeof( long ) * m_lClassCount );

		{	   
			m_strChartModelVal += "( ";

			if( m_nCheckModelType == 0 )
			{
				double fValue = _calc_chi_square(  m_list_attached.get( m_list_attached.head() ), plClassArray, m_lClassCount );
				CString strVal;
				if( fValue != -1 )
					strVal.Format( _get_format( PARAM_CHI2_PROB ), fValue );
				else
					strVal=_T("-");
				m_strChartModelVal += strVal + " )";
			}
			else
			{
				double fValue = _calc_fisher( plClassArray, m_lClassCount  );
				CString strVal;
				strVal.Format( "%lf", fValue );
				m_strChartModelVal += strVal + " )";
			}
		}


		// build data for histogram or linear graph
		double fYMaxVal = -1e307;
		double fYMinVal = 1e307;
		_list_t<double> m_max_y;
		long lStatID=0;
		for (TPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst), lStatID++)
			m_max_y.add_tail( -1e307 );
		lStatID = 0;
		for (TPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst), lStatID++)
		{
			double fCummu = 0; 
			IStatObjectDispPtr ptr_object = m_list_attached.get( lpos_lst );

			if( !m_nChartViewType ) 
			{ 
				// строю столбовую диаграмму
				for( long i = 0; i < m_lClassCount * 2; i += 2 )
				{
					bool bDashDraw = false;
					double fValue = 0;
					ptr_object->GetValueInUnit2( plClassArray[i/2], m_nYParamKey, &fValue );

					double pYAxis[2];

					if( !(1==m_nChartType) )
						pYAxis[0] = fValue;
					else
					{ // Строю куммуляту
						pYAxis[0] = fValue + fCummu;
						fCummu = pYAxis[0];
					}
					pYAxis[1] = 0;

					if( pYAxis[0] > fYMaxVal )
						fYMaxVal = pYAxis[0];

					// [vanek] : UseRange по Y - 24.11.2004
					if( pYAxis[0] < fYMinVal )
						fYMinVal = pYAxis[0];

					{long id = 0; for (TPOS lp = m_max_y.head(); lp; lp = m_max_y.next(lp), id++)
					{
						if (lStatID == id)
						{
							m_max_y.set(__max(pYAxis[0], m_max_y.get(lp)), lp);
							break;
						}
					}}

					double pXAxis[2];

					if( !m_nXAxisType )
					{
						pXAxis[0] = (i / 2)* m_fBarWidth;
						pXAxis[1] = ( (i / 2) + 1 ) * m_fBarWidth;

						if( pXAxis[0] < fXMinVal )
							fXMinVal = pXAxis[0];
						if( pXAxis[1] < fXMinVal )
							fXMinVal = pXAxis[1];

						if( pXAxis[0] > fXMaxVal )
							fXMaxVal = pXAxis[0];
						if( pXAxis[1] > fXMaxVal )
							fXMaxVal = pXAxis[1];
					}
					else
					{
						double fValue = 0, fValue2 = 0;

						if( !_is_stepped_param_chart() )
						{
							ptr_object->GetValueInUnit2( plClassArray[i/2], PARAM_MIN, &fValue );
							ptr_object->GetValueInUnit2( plClassArray[i/2], PARAM_MAX, &fValue2 );
						}
						else
						{
							fValue = m_parrParamChartMin[i/2]   * fCoefX;
							fValue2 = m_parrParamChartMax[i/2] * fCoefX;
						}

						if( fValue != 0 && fabs( fValue - fValue2 ) < 0.0000001 )
						{
							bDashDraw = true;
							if( bUseLogaScaleX )
							{
								//fValue *= 0.99;
								fValue2 *= 1.01;
							}else{
								double fmin = 0, fmax = 0;
								if( m_bX_UseValueRange )
								{
									fmin = m_fX_AxisBegin;
									fmax = m_fX_AxisEnd;
								}else{
									ptr_object->GetValueGlobalInUnit2( PARAM_MIN, &fmin );
									ptr_object->GetValueGlobalInUnit2( PARAM_MAX, &fmax );
								}
								double r = ( fmax - fmin ) * 0.005;
								fValue -= r / 2;
								fValue2 += r / 2;
							}
						}

						pXAxis[0] = fValue;
						pXAxis[1] = fValue2;
					}

					if( m_list_classes.count() )
					{
						if( m_list_classes.find( plClassArray[i/2] ) )
						{
							m_sptrChart->SetData( plClassArray[i/2] + lStatID, pXAxis, 2, ChartDataType( cdfDataX | cdfDataGist ) );
							m_sptrChart->SetData( plClassArray[i/2] + lStatID, pYAxis, 2, ChartDataType( cdfDataY | cdfDataGist ) );
						}
						else
						{
							double fvals[2] = { pXAxis[0], pXAxis[1] };
							pXAxis[0] = pXAxis[1] = 0;
							m_sptrChart->SetData( plClassArray[i/2] + lStatID, pXAxis, 2, ChartDataType( cdfDataX | cdfDataGist ) );
							m_sptrChart->SetData( plClassArray[i/2] + lStatID, pXAxis, 2, ChartDataType( cdfDataY | cdfDataGist ) );
							pXAxis[0] = fvals[0];
							pXAxis[1] = fvals[1];
						}
					}
					else
					{
						m_sptrChart->SetData( plClassArray[i/2] + lStatID, pXAxis, 2, ChartDataType( cdfDataX | cdfDataGist ) );
						m_sptrChart->SetData( plClassArray[i/2] + lStatID, pYAxis, 2, ChartDataType( cdfDataY | cdfDataGist ) );
					}


					bool bMed = 0, bMid = 0, bMode = 0;

					DWORD dwDigs  = 0;

					if( m_bShowAxisValueX && m_nXAxisType )
						dwDigs |= cfAxisDigitsX;
					if( m_bShowAxisValueY )
						dwDigs |= cfAxisDigitsY;

					DWORD dwDigs2  = 0;

					if( m_bShowAxisLabelsX )
						dwDigs2 |= cfAxisScaleX;
					if( m_bShowAxisLabelsY )
						dwDigs2 |= cfAxisScaleY;

					ptrChartAtrr->SetChartStyle( plClassArray[i/2] + lStatID, ChartViewParam( cfAxisX | cfAxisY | dwDigs | dwDigs2 ) );

					if( m_nXAxisType && bDashDraw )
						ptrChartAtrr->SetChartLineStyle( plClassArray[i/2] + lStatID, PS_SOLID/*PS_DASHDOT*/, clfChartStyle );

					if( !m_bShowGrayScale )
					{
						if( m_bShowSingleColorBar )
						{
							ptrChartAtrr->SetChartColor( plClassArray[i/2] + lStatID, m_clBarSingleColor, ccfGistFillColor );
						}
						else
						{
							ptrChartAtrr->SetChartColor( plClassArray[i/2] + lStatID, 
								rso->get_class_color( plClassArray[i/2] ),
								ccfGistFillColor );
						}
					}
					else
					{
						ptrChartAtrr->SetChartColor( plClassArray[i/2] + lStatID, RGB( m_nGrayScaleLevel, m_nGrayScaleLevel, m_nGrayScaleLevel ), ccfGistFillColor );
					}

					ptrChartAtrr->SetChartColor( plClassArray[i/2] + lStatID, m_lfFontAxisChart.m_clrText, ccfAxisText );

					LOGFONT lf = m_lfFontAxisChart;
					HDC hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
					lf.lfHeight = -MulDiv( lf.lfHeight, m_DCLOGPIXELSY, 72);
					::DeleteDC( hDC );

					ptrChartAtrr->SetAxisTextFont( plClassArray[i/2] + lStatID, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );
					// On DoDraw
				}
			}
			else
			{	// строю линейный график
				double *pYAxis = new double[( m_lClassCount - 1 ) * 2];
				double *pXAxis = new double[( m_lClassCount - 1 ) * 2];

				//double _fMinY = 1e308;
				for( long i = 0; i < ( m_lClassCount - 1 ) * 2; i += 2 )
				{
					double fValue = 0;
					ptr_object->GetValueInUnit2( plClassArray[i / 2], m_nYParamKey, &fValue );

					double fValue2 = 0;

					ptr_object->GetValueInUnit2( plClassArray[i / 2 + 1], m_nYParamKey, &fValue2 );

					if( !(1==m_nChartType) )
					{
						pYAxis[i] = fValue;
						pYAxis[i + 1] = fValue2;
					}
					else
					{
						pYAxis[i] = fValue + fCummu;
						fCummu = pYAxis[i];
						pYAxis[i + 1] = fCummu + fValue2;
					}

					if( pYAxis[i] > fYMaxVal )
						fYMaxVal = pYAxis[i];

					if( pYAxis[i + 1] > fYMaxVal )
						fYMaxVal = pYAxis[i + 1];

					// [vanek] : UseRange по Y - 24.11.2004
					if( pYAxis[i + 1] < /*_fMinY*/ fYMinVal )
						/*_fMinY*/ fYMinVal = pYAxis[i + 1];

						if( pYAxis[i] < /*_fMinY*/ fYMinVal )
							/*_fMinY*/ fYMinVal = pYAxis[i];

						{long id = 0; for (TPOS lp = m_max_y.head(); lp; lp = m_max_y.next(lp), id++)
						{
							if (lStatID == id)
							{
								m_max_y.set(__max(pYAxis[i], m_max_y.get(lp)), lp);
								m_max_y.set(__max(pYAxis[i + 1], m_max_y.get(lp)), lp);
								break;
							}
						}}

							if( !m_nXAxisType )
							{
								pXAxis[i] = (i / 2) * m_fBarWidth + m_fBarWidth * 0.5;
								pXAxis[i+1] = ( (i / 2) + 1 ) * m_fBarWidth + m_fBarWidth * 0.5;

								if( pXAxis[i] - m_fBarWidth * 0.5 < fXMinVal )
									fXMinVal = pXAxis[i] - m_fBarWidth * 0.5;
								if( pXAxis[i+1] - m_fBarWidth * 0.5 < fXMinVal )
									fXMinVal = pXAxis[i+1] - m_fBarWidth * 0.5;

								if( pXAxis[i] + m_fBarWidth * 0.5 > fXMaxVal )
									fXMaxVal = pXAxis[i] + m_fBarWidth * 0.5;
								if( pXAxis[i+1] + m_fBarWidth * 0.5 > fXMaxVal )
									fXMaxVal = pXAxis[i+1] + m_fBarWidth * 0.5;
							}
							else
							{
								double fValue = 0, fValue2 = 0;
								double _fValue = 0, _fValue2 = 0;

								if( !_is_stepped_param_chart() )
								{
									ptr_object->GetValueInUnit2( plClassArray[i/2], PARAM_MIN, &fValue );
									ptr_object->GetValueInUnit2( plClassArray[i/2], PARAM_MAX, &fValue2 );

									ptr_object->GetValueInUnit2( plClassArray[i/2+1], PARAM_MIN, &_fValue );
									ptr_object->GetValueInUnit2( plClassArray[i/2+1], PARAM_MAX, &_fValue2 );
								}
								else
								{
									fValue = m_parrParamChartMin[i/2] * fCoefX;
									fValue2 = m_parrParamChartMax[i/2] * fCoefX;

									_fValue = m_parrParamChartMin[i/2+1] * fCoefX;
									_fValue2 = m_parrParamChartMax[i/2+1] * fCoefX;
								}

								pXAxis[i] = ( fValue + fValue2 ) / 2;
								pXAxis[i+1] = ( _fValue + _fValue2 ) / 2;
							}
				}



				m_sptrChart->SetData( plClassArray[0] + lStatID, pXAxis, ( m_lClassCount - 1 ) * 2, cdfDataX );
				m_sptrChart->SetData( plClassArray[0] + lStatID, pYAxis, ( m_lClassCount - 1 ) * 2, cdfDataY );

				delete []pXAxis;
				delete []pYAxis;

				DWORD dwDigs  = 0;

				if( m_bShowAxisValueX && m_nXAxisType )
					dwDigs |= cfAxisDigitsX;
				if( m_bShowAxisValueY )
					dwDigs |= cfAxisDigitsY;

				if( m_bShowAxisLabelsX )
					dwDigs |= cfAxisScaleX;
				if( m_bShowAxisLabelsY )
					dwDigs |= cfAxisScaleY;

				ptrChartAtrr->SetChartStyle( plClassArray[0] + lStatID, ChartViewParam( cfAxisX | cfAxisY | dwDigs ) );

				if( m_list_color_chart.count() > 1 )
				{
					long _qq = 0;
					for (TPOS lp = m_list_color_chart.head(); lp; lp = m_list_color_chart.next(lp), _qq++)
					{
						if( _qq == lStatID )
						{
							ptrChartAtrr->SetChartColor( plClassArray[0] + lStatID, m_list_color_chart.get( lp ), ccfChartColor );
							break;
						}
					}
				}
				else
					ptrChartAtrr->SetChartColor( plClassArray[0] + lStatID, m_clChartColor, ccfChartColor );

				ptrChartAtrr->SetChartColor( plClassArray[0] + lStatID, m_lfFontAxisChart.m_clrText, ccfAxisText );

				LOGFONT lf = m_lfFontAxisChart;
				HDC hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
				lf.lfHeight = -MulDiv( lf.lfHeight, m_DCLOGPIXELSY, 72);
				::DeleteDC( hDC );

				ptrChartAtrr->SetAxisTextFont( plClassArray[0] + lStatID, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );
			}
		}

		// [vanek] : UseRange по Y - 24.11.2004
		if( m_bY_UseValueRange )
		{
			// [vanek] SBT:1248 т.к. ограничения заданы в СИ, домножаем на коэффициент - 22.12.2004 
			fYMinVal = m_fY_AxisBegin * fCoefY;
			fYMaxVal = m_fY_AxisEnd * fCoefY;
		}
		else{
			if(fYMinVal>0.)
				fYMinVal = 0.; // сбрасываем в ноль минимальное значение по Y
		}

		if( m_bShowAxisValueX )
		{
			CString strAllRow;
			int nMaxSz = 0;

			if( !m_nXAxisType )
			{
				long lStart = m_nMinClass * 2;
				long lEnd  = m_lClassCount * 2;

				//				if( m_nChartViewType )
				//					lEnd =  ( m_lClassCount - 1 ) * 2;

				int nSkip = 0, lCounter = 0;
				for( long i = lStart; i < lEnd; i += 2*m_nClassStep )
				{
					double x = ( i / 2 ) * m_fBarWidth + m_fBarWidth * 0.5;

					if( m_bX_UseValueRange )
					{
						if( x < m_fX_AxisBegin || x > m_fX_AxisEnd )
						{
							nSkip++;
							continue;
						}
					}

					LOGFONT lfHint = m_lfFontAxisChart;
					HDC hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
					lfHint.lfHeight = -MulDiv( lfHint.lfHeight, m_DCLOGPIXELSY, 72);
					::DeleteDC( hDC );

					CString str = ((ObjectSpace::CStatisticObject*)(IStatObject*)sptr_stat_object)
						->get_class_name( plClassArray[i / 2] );

					if( str.GetLength() > nMaxSz )
					{
						strAllRow = str;
						nMaxSz = str.GetLength();
					}
					lCounter++;

					bool bOK = true;

					ptrChartAtrr->SetHintText( plClassArray[0], x, 0, _bstr_t( str ), 1, 0, m_lTextOffsetX );
					ptrChartAtrr->SetHintFont( plClassArray[0], (BYTE*)&lfHint );
					ptrChartAtrr->SetChartColor( plClassArray[0], m_lfFontAxisChart.m_clrText, ccfHintTextColor );
				}
				strAllRow += "00";
				m_strChartX.Empty();
				for( long i = 0; i < lCounter; i++ )
					m_strChartX += strAllRow;
			}
		}

		if(m_bShowAxisLabelsY)
		{
			long lCountY = 1+round( ( fYMaxVal - fYMinVal ) / ( m_fYScaleFactor * fCoefY ) );
			double *pfValsY = 0;

			if(0 < lCountY &&  lCountY < MAX_SCALE_FACTOR_Y)
				pfValsY = new double[lCountY];

			if( !pfValsY && m_bShowAxisLabelsY )
			{
				if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_LABEL_ERROR_Y ); 
				m_iError = IDS_AXIS_LABEL_ERROR_Y;
			}
			if( pfValsY && m_bShowAxisLabelsY )
				for( long z = 0; z < lCountY; z++ )
					pfValsY[z] = fYMinVal + z * m_fYScaleFactor * fCoefY; 

			if( pfValsY && m_bShowAxisLabelsY )
				ptrChartAtrr->SetScaleLabels( plClassArray[0], pfValsY, lCountY, cafAxisY );
			delete []pfValsY;
		}

		if( m_bShowAxisLabelsX)
		{
			if(m_nXAxisType)
			{
				if(!bUseLogaScaleX)	 // Разметка по оси
				{
					double fStep = m_fXScaleFactor * (!m_nXAxisType ? 1. : fCoefX );
					long lCountX = round((fXMaxVal-fXMinVal)/fStep);
					if(fXMinVal+fStep*lCountX <= fXMaxVal + fStep*0.01)
						++lCountX;
					double *pfValsX = 0;

					if(0 < lCountX && lCountX < MAX_SCALE_FACTOR_X)
						pfValsX = new double[lCountX];

					if( !pfValsX && m_bShowAxisLabelsX )
					{
						if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_LABEL_ERROR_X ); 
						m_iError = IDS_AXIS_LABEL_ERROR_X;
					}
					if( pfValsX)
						for( long z = 0; z < lCountX; z++ )
							pfValsX[z] = fXMinVal + z * fStep; 
					if( pfValsX && m_bShowAxisLabelsX )
						ptrChartAtrr->SetScaleLabels( plClassArray[0], pfValsX, lCountX, cafAxisX );

					delete []pfValsX;
				}
				else
				{
					dblArray ValsX;
					_get_loga_scale( ValsX, fXMinVal, fXMaxVal, m_fXScaleFactor);
					ValsX = std::log10(ValsX/fXMinVal) / log10(fXMaxVal/fXMinVal);
					ptrChartAtrr->SetScaleLabels( plClassArray[0], &ValsX[0], ValsX.size(), cafAxisX );
				}
			}else
				ptrChartAtrr->SetConstParams(m_lClassCount, ccfScaleCountX );
		}else{
			ChartViewParam styles;
			ptrChartAtrr->GetChartStyle(0, &styles );
			ptrChartAtrr->SetChartStyle(0, ChartViewParam(~cfAxisScaleX & styles));
		}

		m_strChartY = "W";
		m_nMaxYChart = (float)((fYMaxVal - fYMinVal)/(m_fYScaleFactorValues * fCoefY));

		if(m_bShowAxisValueY)
		{
			long lCountY = 1+round( ( fYMaxVal - fYMinVal ) / ( m_fYScaleFactorValues * fCoefY ) );
			double *pfValsY = 0;

			if(0 < lCountY && lCountY < MAX_SCALE_FACTOR_VALUES_Y)
				pfValsY = new double[lCountY];
			if( !pfValsY && m_bShowAxisValueY )
			{
				if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_VALUES_ERROR_Y ); 
				m_iError = IDS_AXIS_VALUES_ERROR_Y;
			}
			if( pfValsY && m_bShowAxisValueY )
				for( long z = 0; z < lCountY; z++ )
					pfValsY[z] = /*0*/fYMinVal + z * m_fYScaleFactorValues * fCoefY; 
			if( pfValsY && m_bShowAxisValueY )
				ptrChartAtrr->SetScaleTextLabels( plClassArray[0], pfValsY, pfValsY, lCountY, cafAxisY );
			delete []pfValsY;

			if( pfValsY && m_bShowAxisValueY )
				for( long z = 0; z < lCountY; z++ )
				{
					CString value;
					if( !m_b_use_custom_format_y )
						value=_variant_t( pfValsY[z] );
					else
						value.Format(m_str_custom_format_y, pfValsY[z] );

					if( value.GetLength() > m_strChartY.GetLength() )
						m_strChartY = value ;
				}
		}

		if(m_nXAxisType && m_bShowAxisValueX)
		{
			CString strAllRow;
			int nMaxSz = 0;

			CString str;
			if(ParameterDescriptor_ex* pmp=map_meas_params(m_nXParamKey))
			{
				str = pmp->bstrDefFormat;
			}
			if(0==str.GetLength())
				str="%g";

			if( !m_b_use_custom_format_x )
			{
				str = getFormatString(m_fXScaleFactorValues * fCoefX);
			}
			else
				str = m_str_custom_format_x;

			if( m_nXAxisType )
				m_strChartX.Empty();

			int x = 0;
			CString sMax;

			if( !bUseLogaScaleX )   // Значения по оси
			{
				long lCountX = 1 + (long)floor((fXMaxVal-fXMinVal)/(m_fXScaleFactorValues * fCoefX));
				double *pfValsX = 0;

				if(0 < lCountX && lCountX < MAX_SCALE_FACTOR_VALUES_X)
					pfValsX = new double[lCountX];

				if(!pfValsX)
				{
					if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_VALUES_ERROR_X ); 
					m_iError = IDS_AXIS_VALUES_ERROR_X;
				}else{

					for( long z = 0; z < lCountX; z++ )
					{
						pfValsX[z] = fXMinVal + z * m_fXScaleFactorValues  * fCoefX; 

						strAllRow.Format( str, pfValsX[z] );
						if( strAllRow.GetLength() > x )
						{
							x = strAllRow.GetLength();
							sMax = strAllRow;
						}
					}
					ptrChartAtrr->SetScaleTextLabels( plClassArray[0], pfValsX, pfValsX, lCountX, cafAxisX );
					{
						sMax += "00";
						for( long i = 0; i < lCountX; i++ )
							m_strChartX += sMax;
					}
					delete []pfValsX;
				}
			}
			else
			{
				dblArray ValsX;
				_get_loga_scale( ValsX, fXMinVal, fXMaxVal, 10.);
				dblArray ValsX2 = ValsX;
				ValsX = std::log10(ValsX/fXMinVal) / log10(fXMaxVal/fXMinVal);

				for( unsigned z = 0; z < ValsX2.size(); z++ )
				{
					CString strVal;

					if( !m_b_use_custom_format_x )
					{
						strVal = getFormatString(ValsX2[z]);
					}
					else
						strVal = m_str_custom_format_x;

					strAllRow.Format( strVal, ValsX2[z] );

					if( strAllRow.GetLength() > x )
					{
						x = strAllRow.GetLength();
						sMax = strAllRow;
						str = strVal;
					}
				}

				if( m_nXAxisType )
				{
					sMax += "00";
					for( unsigned i = 0; i < ValsX2.size(); i++ )
						m_strChartX += sMax;
				}
				ptrChartAtrr->SetScaleTextLabels( plClassArray[0], &ValsX[0], &ValsX2[0], ValsX.size(), cafAxisX );
			}

			CString strYP;// = _get_format( m_nYParamKey );
			CString strXP;// = _find_in_meas_param( ::GetAppUnknown(), "\\measurement\\parameters", m_nXParamKey, "Format" );

			CString format;
			if( !m_b_use_custom_format_y )
			{
				//				strYP = getFormatString( m_fYScaleFactorValues * fCoefY );
			}
			else
				strYP = m_str_custom_format_y;

			if( !m_b_use_custom_format_x )
			{
				//				strXP = getFormatString( m_fXScaleFactorValues * fCoefX );
			}
			else
				strXP = m_str_custom_format_x;

			if( bUseLogaScaleX )
				strXP = str;

			ptrChartAtrr->SetTextFormat( plClassArray[0], _bstr_t( strYP ), cffDigitsY );
			ptrChartAtrr->SetTextFormat( plClassArray[0], _bstr_t( strXP ), cffDigitsX );
		}

		if( m_bShowCurve && m_nXAxisType )
		{
			double fDisp = 1;
			double fMX = 0;

			if( m_lUniformDistribution == 1 )
			{
				if( sptr_stat_disp->GetValueGlobalInUnit2( PARAM_MATHXLOG, &fMX ) != S_OK )
				{
					m_bShowCurve = 0;
					CString str, str2;
					str.LoadString( IDS_NO_LOG );
					str2.LoadString( IDS_NO_LOG_TITLE );

					::MessageBox( handle(), str, str2, MB_OK | MB_ICONWARNING );
				}
			}
		}

		lStatID = 0;
		for (TPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next(lpos_lst), lStatID++)
		{
			IStatObjectDispPtr ptr_object = m_list_attached.get( lpos_lst );
			if( m_bShowCurve && m_nXAxisType )
			{
				using namespace ObjectSpace;
				double fStdDev = 1;
				double fMX = 0;

				if( m_lUniformDistribution == 0 )
				{
					HRESULT hr=rso->GetParamVal2(ClassID(AllClasses,AllImages), PARAM_MX, 0, fMX);
					if(hr) continue;
					hr=rso->GetParamVal2(ClassID(AllClasses,AllImages), PARAM_STDDEV, 0, fStdDev);
					if(hr) continue;
					fMX *= fCoefX;
					fStdDev *= fCoefX;
				}
				else
				{
					ptr_object->GetValueGlobalInUnit2( PARAM_MATHXLOG, &fMX );
					fMX = log(fMX);
					ptr_object->GetValueGlobal2( PARAM_DXLOG, &fStdDev );
					fStdDev = M_LN10*sqrt(fStdDev);
				}

				long lCount = long(m_lClassCount) * 50;
				dblArray pArrX(lCount), pArrY(lCount);

				// A.M. fix. SBT 1414.
				double fXStartChart = fXMinVal, fXEndChart = fXMaxVal;
				if (_is_stepped_param_chart())
				{
					fXStartChart = m_parrParamChartMin[0]   * fCoefX;
					fXEndChart = m_parrParamChartMax[m_lClassCount-1] * fCoefX;
				}

				pArrX[0]=fXStartChart;
				if( m_lUniformDistribution == 1 ){
					double fLogStep =log(fXEndChart/fXStartChart)/(lCount - 1);	
					double fMult=exp(fLogStep);
					for( int i = 1; i < lCount; ++i)
						pArrX[i] = pArrX[i-1] * fMult;
					double y=log(fXStartChart);
					for( int i = 0; i < lCount; ++i, y+=fLogStep){
						switch (m_nChartType){
							default:
							case 0:
								pArrY[i] = FLogNormal( pArrX[i], fMX, fStdDev );
								break;
							case 1:
								pArrY[i] = FuncNormDistrib( (y - fMX) / fStdDev);
								break;
							case 2: {
								double z = (y - fMX)/fStdDev;
								pArrY[i] = M_1_SQRT_2_PI / fStdDev * exp(-z*z/2.); 
											}
											break;
						}
					}
				}else{
					double fStep = ( fXEndChart - fXStartChart ) / (lCount - 1) ;
					for( int i = 1; i < lCount; i++ )
						pArrX[i] = pArrX[i-1] + fStep;
					for( int i = 0; i < lCount; i++ ){
						if( m_nChartType == 1)
							pArrY[i] = FuncNormDistrib( (pArrX[i] - fMX ) / fStdDev);
						else
							pArrY[i] = Fnormal( ( pArrX[i] - fMX ) / fStdDev, 0, 1 );
					}
				}

				double f_max = pArrY.max();
				double f_min = 0.0; //pArrY.min();

				double fmax_y = fYMaxVal;
				{long id = 0; for (TPOS lp = m_max_y.head(); lp; lp = m_max_y.next(lp), id++)
				{
					if (lStatID == id)
					{
						fmax_y = m_max_y.get(lp);
						break;
					}
				}}

				for( int i = 0; i < lCount; i++ )
					pArrY[i] = ( pArrY[i] - f_min ) * fmax_y / (f_max - f_min);


				m_sptrChart->SetData( 1500 + lStatID, &pArrX[0], lCount, cdfDataX );
				m_sptrChart->SetData( 1500 + lStatID, &pArrY[0], lCount, cdfDataY );

				if( m_list_color_curve.count() > 1 )
				{
					long _qq = 0;
					for (TPOS lp = m_list_color_curve.head(); lp; lp = m_list_color_curve.next(lp), _qq++)
					{
						if( _qq == lStatID )
						{
							ptrChartAtrr->SetChartColor( 1500 + lStatID, m_list_color_curve.get( lp ), ccfChartColor );
							break;
						}
					}
				}
				else
					ptrChartAtrr->SetChartColor( 1500 + lStatID, m_clCurveColor, ccfChartColor );

				ptrChartAtrr->SetChartColor( 1500 + lStatID, m_lfFontAxisChart.m_clrText, ccfAxisText );

				LOGFONT lf = m_lfFontAxisChart;
				HDC hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
				lf.lfHeight = -MulDiv( lf.lfHeight, m_DCLOGPIXELSY, 72);
				::DeleteDC( hDC );

				ptrChartAtrr->SetAxisTextFont( 1500 + lStatID, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );
			}
		}

		if( m_bShowUserCurve && m_nXAxisType )
		{
			long lCount = long(m_lClassCount) * 50;

			dblArray pArrX(lCount), pArrY(lCount);

			// A.M. fix. SBT 1414.
			double fXStartChart = fXMinVal, fXEndChart = fXMaxVal;
			//if (_is_stepped_param_chart())
			//{
			//	fXStartChart = m_parrParamChartMin[0]   * fCoefX;
			//	fXEndChart = m_parrParamChartMax[m_lClassCount-1] * fCoefX;
			//}

			double fStep = ( fXEndChart - fXStartChart ) / (lCount - 1) ;

			for( int i = 0; i < lCount; i++ )
				pArrX[i] = fXStartChart + i * fStep;

			for( int i = 0; i < lCount; i++ )
			{
				pArrY[i] = Fnormal( ( pArrX[i] - m_fUserCurveMX ) / sqrt( m_fUserCurveDX ), 0, 1 );

				if( m_nChartType == 1 && i > 0 )
					pArrY[i] = pArrY[i] + pArrY[i - 1];
			}
			double f_max = pArrY.max();
			double f_min = pArrY.min();

			double fmax_y = fYMaxVal;
			{ long id = 0; for (TPOS lp = m_max_y.head(); lp; lp = m_max_y.next(lp), id++)
			{
				if (lStatID == id)
				{
					fmax_y = m_max_y.get(lp);
					break;
				}
			}} 

			for( int i = 0; i < lCount; i++ )
				pArrY[i] = ( pArrY[i] - f_min ) * fmax_y / (f_max - f_min);


			m_sptrChart->SetData( 1600 + lStatID, &pArrX[0], lCount, cdfDataX );
			m_sptrChart->SetData( 1600 + lStatID, &pArrY[0], lCount, cdfDataY );

			ptrChartAtrr->SetChartColor( 1600 + lStatID, m_clUserCurveColor, ccfChartColor );
			ptrChartAtrr->SetChartColor( 1600 + lStatID, m_lfFontAxisChart.m_clrText, ccfAxisText );

			LOGFONT lf = m_lfFontAxisChart;
			HDC hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
			lf.lfHeight = -MulDiv( lf.lfHeight, m_DCLOGPIXELSY, 72);
			::DeleteDC( hDC );

			ptrChartAtrr->SetAxisTextFont( 1600 + lStatID, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );
		}

		if( m_bX_UseValueRange )
		{
			if( !bUseLogaScaleX )
			{
				ptrChartAtrr->SetChartRange( plClassArray[0], m_fX_AxisBegin * ( !m_nXAxisType ? 1 : fCoefX ), m_fX_AxisEnd * ( !m_nXAxisType ? 1 : fCoefX ), crfRangeX );
				ptrChartAtrr->SetRangeState( plClassArray[0], true, crfRangeX );
			}
			else
			{
				double a = 0.;
				double b = 1.;

				ptrChartAtrr->SetChartRange( plClassArray[0], a, b, crfRangeX );
				ptrChartAtrr->SetRangeState( plClassArray[0], true, crfRangeX );
			}
		}
		else
		{
			if( !bUseLogaScaleX )
			{
				ptrChartAtrr->SetChartRange( plClassArray[0], fXMinVal, fXMaxVal, crfRangeX );
				ptrChartAtrr->SetRangeState( plClassArray[0], true, crfRangeX );
			}
			else
			{
				double a = 0.;
				double b = 1.;

				ptrChartAtrr->SetChartRange( plClassArray[0], a, b, crfRangeX );
				ptrChartAtrr->SetRangeState( plClassArray[0], true, crfRangeX );
			}
		}

		if( bUseLogaScaleX )
		{

			double a = 0,b = 0;
			ptrChartAtrr->GetChartRange( plClassArray[0], &a, &b, crfRangeX );
			ptrChartAtrr->SetChartRange( plClassArray[0], fXMinVal, fXMaxVal, crfRangeX );

			m_sptrChart->LogarifmScale( cdfDataX );

			ptrChartAtrr->SetChartRange( plClassArray[0], a, b, crfRangeX );
		}

		if( m_bXGrid || m_bYGrid )
		{

			if( !bUseLogaScaleX )
			{
				long lCountX = 1+round((fXMaxVal-fXMinVal ) / ( m_fXScaleFactorGrid * (!m_nXAxisType ? 1 : fCoefX )));
				double *pfValsX = 0;

				if(0 < lCountX && lCountX < MAX_SCALE_FACTOR_GRID_X)
					pfValsX = new double[lCountX];

				if( !pfValsX && m_bXGrid )
				{
					if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_GRID_ERROR_X ); 
					m_iError = IDS_AXIS_GRID_ERROR_X;
				}

				// [vanek] : UseRange по Y - 24.11.2004
				long lCountY = 1+round( ( fYMaxVal - fYMinVal ) / ( m_fYScaleFactorGrid * fCoefY ) );
				double *pfValsY = 0;

				if(0 < lCountY && lCountY < MAX_SCALE_FACTOR_GRID_Y)
					pfValsY = new double[lCountY];

				if( !pfValsY && m_bYGrid )
				{
					if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_GRID_ERROR_Y ); 
					m_iError = IDS_AXIS_GRID_ERROR_Y;
				}

				if( pfValsX && m_bXGrid )
					for( long i = 0; i < lCountX; i++ )
						pfValsX[i] = fXMinVal + i * m_fXScaleFactorGrid * ( !m_nXAxisType ? 1. : fCoefX ); 

				// [vanek] : UseRange по Y - 24.11.2004
				if( pfValsY && m_bYGrid )
					for( long i = 0; i < lCountY; i++ )
						pfValsY[i] = /*0*/ fYMinVal + i * m_fYScaleFactorGrid * fCoefY; 

				if( pfValsX && m_bXGrid )
					ptrChartAtrr->SetGridLines( pfValsX, lCountX, ccgXGrid );
				if( pfValsY && m_bYGrid )
					ptrChartAtrr->SetGridLines( pfValsY, lCountY, ccgYGrid );

				delete []pfValsX;
				delete []pfValsY;
			}
			else
			{
				dblArray ValsX;
				_get_loga_scale( ValsX, fXMinVal, fXMaxVal, m_fXScaleFactorGrid);
				ValsX = std::log10(ValsX/fXMinVal) / log10(fXMaxVal/fXMinVal);
				ptrChartAtrr->SetGridLines( &ValsX[0], ValsX.size(), ccgXGrid );

				// [vanek] : UseRange по Y - 24.11.2004
				long lCountY = 1+round( ( fYMaxVal - /*0*/ fYMinVal ) / ( m_fYScaleFactorGrid * fCoefY ) );
				double *pfValsY = 0;

				if(0 < lCountY && lCountY < MAX_SCALE_FACTOR_GRID_Y)
					pfValsY = new double[lCountY];

				if( !pfValsY && m_bYGrid )
				{
					if( m_bShowErrorMsg ) ErrorMessage( m_hwnd, IDS_AXIS_GRID_ERROR_Y ); 
					m_iError = IDS_AXIS_GRID_ERROR_Y;
				}

				// [vanek] : UseRange по Y - 24.11.2004
				if( pfValsY && m_bYGrid )
					for( long z = 0; z < lCountY; z++ )
						pfValsY[z] = /*0*/ fYMinVal + z * m_fYScaleFactorGrid * fCoefY; 

				if( pfValsY && m_bYGrid )
					ptrChartAtrr->SetGridLines( pfValsY, lCountY, ccgYGrid );

				delete []pfValsY;
			}
		}

		// [vanek] : UseRange по Y - 24.11.2004
		m_fMinYValue = fYMinVal;
		m_fMaxYValue = fYMaxVal;
		m_fMinXValue = fXMinVal;
		m_fMaxXValue = fXMaxVal;

		ptrChartAtrr->SetChartRange( plClassArray[0], /*0*/fYMinVal, fYMaxVal , crfRangeY );
		ptrChartAtrr->SetRangeState( plClassArray[0], true, crfRangeY );

		m_bGistFilled = true;
		delete []plClassArray;
	}

	void CChartWnd::_resize_child( RECT rect )
	{
		if(rect.right-rect.left<m_rcMinRect.right-m_rcMinRect.left 
			|| rect.bottom-rect.top<m_rcMinRect.bottom-m_rcMinRect.top)
		{
			//			_ASSERT(!"Invalid given rect has unsufficient height");
		}

		m_rcFull = rect;

		::InflateRect( &rect, int(-5), int( -5) );

		m_rcClient = rect;
		m_rcChart = m_rcClient;

		::InflateRect( &m_rcChart, int( -5), int( -5) );

		HDC hDC = ::GetDC( handle() );
		RECT rc2=m_rcChart;
		DoDraw( hDC, m_rcFull, &rc2);
		ReleaseDC(handle(),hDC);


		int nBtn = 15;

		if( !m_nYTitle )
			m_nYTitle = nBtn;

		if( !m_nY_XParam )
			m_nY_XParam = nBtn;

		m_rcTitle = m_rcChart;

		m_rcTitle.bottom = long( m_rcTitle.top + m_nYTitle);
		m_rcChart.top = m_rcTitle.bottom;

		m_rcLegend = m_rcYParam = m_rcXParam = m_rcChart;

		m_rcLegend.top = long( m_rcLegend.bottom - m_nY_Legend);

		m_rcXParam.bottom = m_rcLegend.top;
		m_rcXParam.top = long( m_rcXParam.bottom - m_nY_XParam);
		m_rcYParam.bottom = m_rcChart.bottom = m_rcXParam.top;

		m_rcYParam = m_rcChart;

		m_rcYParam.right = long( m_rcYParam.left + m_nY_XParam);
		m_rcTitle.left = m_rcChart.left;
		m_rcChart.left = m_rcYParam.right;

		m_rcXParam.left = m_rcYParam.right;

		IChartAttributesPtr sptrAttr = m_sptrChart;
		long dy = 0;

		if( sptrAttr )
			sptrAttr->GetConstParams( &dy, ccfBBorder );

		RECT rcHotBtnChart = { m_rcClient.right - nBtn, m_rcClient.top, m_rcClient.right, m_rcClient.top + nBtn };

		if( m_bMinSizeCalc && m_wHotBtnChart.handle() )
			m_wHotBtnChart.move_window( rcHotBtnChart );

		if( m_bMinSizeCalc )
		{
			if( m_bEnableChartArea )
				::ShowWindow( m_wHotBtnChart.handle(), SW_SHOW );
			else
				::ShowWindow( m_wHotBtnChart.handle(), SW_HIDE );
		}

		RECT rcHotBtnXParam = { m_rcXParam.right - nBtn, long( m_rcXParam.top + ( m_nY_XParam- nBtn ) / 2 ), m_rcXParam.right, long( m_rcXParam.top + nBtn + ( m_nY_XParam- nBtn ) / 2 ) };

		if( m_bMinSizeCalc && m_wHotBtnXParam.handle() )
			m_wHotBtnXParam.move_window( rcHotBtnXParam );

		if( m_bMinSizeCalc )
		{
			if( m_bEnableXParamArea )
				::ShowWindow( m_wHotBtnXParam.handle(), SW_SHOW );
			else
				::ShowWindow( m_wHotBtnXParam.handle(), SW_HIDE );
		}

		RECT rcHotBtnYParam = { long( m_rcYParam.right - nBtn -  ( m_nY_XParam-nBtn ) / 2 ), m_rcYParam.top - nBtn, long( m_rcYParam.right - ( m_nY_XParam-nBtn ) / 2 ), m_rcYParam.top };

		if( m_bMinSizeCalc && m_wHotBtnYParam.handle() )
			m_wHotBtnYParam.move_window( rcHotBtnYParam );

		if( m_bMinSizeCalc )
		{
			if( m_bEnableYParamArea )
				::ShowWindow( m_wHotBtnYParam.handle(), SW_SHOW );
			else
				::ShowWindow( m_wHotBtnYParam.handle(), SW_HIDE );
		}

		RECT rcHotBtnAxis = { m_rcChart.right - ( m_bEnableAxisArea ? nBtn : 0 ), m_rcChart.bottom - nBtn - nBtn/2 - dy, m_rcChart.right, m_rcChart.bottom - nBtn/2 - dy };

		if( m_bMinSizeCalc && m_wHotBtnAxis.handle() )
			m_wHotBtnAxis.move_window( rcHotBtnAxis );

		if( m_bMinSizeCalc )
		{
			if( m_bEnableAxisArea )
				::ShowWindow( m_wHotBtnAxis.handle(), SW_SHOW );
			else
				::ShowWindow( m_wHotBtnAxis.handle(), SW_HIDE );
		}

		m_rcXParam.right = m_rcChart.right = m_rcTitle.right = rcHotBtnAxis.left;

		if( m_bMinSizeCalc )
		{
			_calc_hint_arr();
		}
	}

	LRESULT CChartWnd::on_size(short cx, short cy, ulong fSizeType)
	{
		RECT rc = {0};
		::GetClientRect( m_hwnd, &rc );

		_resize_child( rc );
		::InvalidateRect(m_hwnd,0,FALSE);

		return __super::on_size( cx, cy, fSizeType );
	}

	bool CChartWnd::create( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass )
	{
		if(!__super::create_ex(style, rect, pszTitle, parent, hmenu, pszClass, WS_EX_CLIENTEDGE))
			return false;
		m_ptooltip = new CToolTip32;
		m_ptooltip2 = new CToolTip32;
		//		m_ptooltip->CreateTooltip( handle(), TTS_ALWAYSTIP );
		//		m_ptooltip2->CreateTooltip( handle(), TTS_ALWAYSTIP );

		m_ptooltip->SetMaxTipWidth( SHRT_MAX );
		::SendMessage( m_ptooltip->handle(), TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX );
		::SendMessage( m_ptooltip->handle(), TTM_SETDELAYTIME,   TTDT_INITIAL, 200 );
		::SendMessage( m_ptooltip->handle(), TTM_SETDELAYTIME,   TTDT_RESHOW,  200 );

		m_ptooltip2->SetMaxTipWidth( SHRT_MAX );
		::SendMessage( m_ptooltip2->handle(), TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX );
		::SendMessage( m_ptooltip2->handle(), TTM_SETDELAYTIME,   TTDT_INITIAL, 200 );
		::SendMessage( m_ptooltip2->handle(), TTM_SETDELAYTIME,   TTDT_RESHOW,  200 );

		m_wHotBtnChart.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );

		m_wHotBtnXParam.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
		m_wHotBtnYParam.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
		m_wHotBtnAxis.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" ); 

		return true;
	}

	bool CChartWnd::_get_loga_scale( dblArray& Vals, double fMinVal, double fMaxVal, double dStep )
	{
		int nStep;
		if(dStep <1.)
			nStep=1;
		else if(dStep>10.)
			nStep=10;
		else
			nStep=(int)dStep;
		if(fMinVal<=0.)
			fMinVal=fMaxVal/1000.;

		double a = pow( 10., floor( log10( fMinVal ) ) );
		vector<double> vals(1);
		vals[0]=a;		
		double c=a;
		for(int i=0;; ++i, c *= 10.)
		{
			for( int k = 0; k < 10; k += nStep )
			{
				double d = c * (0==k?1:k);
				if(d<=fMinVal){
					vals[0]=d;
				}else{
					vals.push_back(d);
					if(d>=fMaxVal)
						goto EndFill;
				}
			}
		}
EndFill:
		Vals.resize(vals.size()); 
		std::copy(vals.begin(),vals.end(),&Vals[0]);
		return true;
	}

	double CChartWnd::_calc_chi_square( IUnknownPtr sptrO, long *plClassArray, long lClassCount  )
	{
		IStatObjectDispPtr sptrDisp = sptrO;

		double fVal = 0;

		HRESULT hr;
		if( m_lUniformDistribution == 1 )
			hr = sptrDisp->GetValueGlobalInUnit2( PARAM_CHI2_PROB_LOG, &fVal );
		else
			hr = sptrDisp->GetValueGlobalInUnit2( PARAM_CHI2_PROB, &fVal );

		if( hr != S_OK )
			return -1;

		return fVal;
	}

	double CChartWnd::_calc_fisher( long *plClassArray, long lClassCount  )
	{
		return 0;
	}

	LRESULT CChartWnd::on_lbuttonup(const _point &point)
	{
		if( !m_nChartViewType && m_arrClasses ) 
		{
			IChartMiscHelperPtr sptrHelper = m_sptrChart;

			IStatObjectDispPtr sptrStat = m_list_attached.get( m_list_attached.head() );//m_sptrAttached;

			if( sptrStat == 0 )
				return 0;

			for( long i = m_lClassCount - 1; i >= 0; i-- )
			{
				long lClass = m_arrClasses[i];

				double *pYAxis = 0;
				double *pXAxis = 0;
				long lc = 0;
				m_sptrChart->GetData( lClass, &pXAxis, &lc, cdfDataX );
				m_sptrChart->GetData( lClass, &pYAxis, &lc, cdfDataY );

				RECT rcChart = m_rcChart;
				IChartAttributesPtr sptrAttr = sptrHelper;

				long nVal = 0;

				sptrAttr->GetConstParams( &nVal, ccfLBorder );
				rcChart.left += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfBBorder );
				rcChart.bottom -= nVal ;

				sptrAttr->GetConstParams( &nVal, ccfTBorder );
				rcChart.top += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfRBorder );
				rcChart.right -= nVal ;

				RECT rcTip = {0};
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[0], pYAxis[0], (LPPOINT)&rcTip );
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[1], pYAxis[1], (LPPOINT)&rcTip + 1 );

				::OffsetRect( &rcTip, rcChart.left, rcChart.top );

				CStatObjectView* pStatObjectView=
					(CStatObjectView*)((char*)this-offsetof(CStatObjectView,m_wndChart));
				if( ::PtInRect( &rcTip, point )&& (vtTableView & pStatObjectView->m_dwViews))
				{
					if(IStatObjectViewPtr pStatViewI=m_sptrView){
						if(CStatObjectView* pCStatObjectView=(CStatObjectView*)(IStatObjectView*)pStatViewI){
							int iItemHot=pCStatObjectView->m_wndTable.m_Grid.SetItemState(lClass
								,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
							BOOL b=pCStatObjectView->m_wndTable.m_Grid.EnsureVisible(lClass,FALSE);
							pCStatObjectView->m_wndTable.m_Grid.SetFocus();

							::SetCursor( m_cur_hand );
						}
					}
					break;
				}
				else
					::SetCursor( m_cur_arrow );
			}
		}

		return 1L;
	}

	LRESULT CChartWnd::on_lbuttondblclk(const _point &point)
	{
		if( !m_nChartViewType && m_arrClasses ) 
		{
			IChartMiscHelperPtr sptrHelper = m_sptrChart;

			IStatObjectDispPtr sptrStat = m_list_attached.get( m_list_attached.head() );//m_sptrAttached;

			if( sptrStat == 0 )
				return 0;

			for( long i = m_lClassCount - 1; i >= 0; i-- )
			{
				long lClass = m_arrClasses[i];

				double *pYAxis = 0;
				double *pXAxis = 0;
				long lc = 0;
				m_sptrChart->GetData( lClass, &pXAxis, &lc, cdfDataX );
				m_sptrChart->GetData( lClass, &pYAxis, &lc, cdfDataY );

				RECT rcChart = m_rcChart;
				IChartAttributesPtr sptrAttr = sptrHelper;

				long nVal = 0;

				sptrAttr->GetConstParams( &nVal, ccfLBorder );
				rcChart.left += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfBBorder );
				rcChart.bottom -= nVal ;

				sptrAttr->GetConstParams( &nVal, ccfTBorder );
				rcChart.top += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfRBorder );
				rcChart.right -= nVal ;

				RECT rcTip = {0};
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[0], pYAxis[0], (LPPOINT)&rcTip );
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[1], pYAxis[1], (LPPOINT)&rcTip + 1 );

				::OffsetRect( &rcTip, rcChart.left, rcChart.top );

				if( ::PtInRect( &rcTip, point ) )
				{
					if( !m_szDBClickScript.IsEmpty() )
					{
						::ExecuteScript( _bstr_t( m_szDBClickScript ), "CChartWnd::on_lbuttondblclk" );
						break;
					}
				}
			}
		}

		return __super::on_lbuttondblclk( point );
	}

	LRESULT CChartWnd::on_mousemove(const _point &point)
	{
		if( m_bShowCurve && m_nXAxisType )
		{
			double fX = 0, fY = 0;
			HRESULT hr = m_sptrChart->GetToolTipValue( 1500, point, &fX, &fY,1 ,m_nHotZone );

			if( hr == S_OK )
			{
				CString str;
				str.Format( "( %.3lf, %.3lf )", fX, fY );

				m_ptooltip2->UpdateTipText( str, handle(), 1500 );
				m_ptooltip2->Activate( true );

				RECT rcTool = { point.x - m_nHotZone, point.y - m_nHotZone, point.x + m_nHotZone, point.y + m_nHotZone };
				m_ptooltip2->SetToolRect( handle(), 1500, &rcTool );

				m_ptooltip->Activate( false );
			}
			else
			{
				m_ptooltip->Activate( true );
				m_ptooltip2->Activate( false );
			}
		}

		if( !m_nChartViewType && m_arrClasses ) 
		{
			IChartMiscHelperPtr sptrHelper = m_sptrChart;

			IStatObjectDispPtr sptrStat = m_list_attached.get( m_list_attached.head() );

			for( long i = m_lClassCount - 1; i >= 0; i-- )
			{
				long lClass = m_arrClasses[i];

				double *pYAxis = 0;
				double *pXAxis = 0;
				long lc = 0;
				m_sptrChart->GetData( lClass, &pXAxis, &lc, cdfDataX );
				m_sptrChart->GetData( lClass, &pYAxis, &lc, cdfDataY );

				RECT rcChart = m_rcChart;
				IChartAttributesPtr sptrAttr = sptrHelper;

				long nVal = 0;

				sptrAttr->GetConstParams( &nVal, ccfLBorder );
				rcChart.left += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfBBorder );
				rcChart.bottom -= nVal ;

				sptrAttr->GetConstParams( &nVal, ccfTBorder );
				rcChart.top += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfRBorder );
				rcChart.right -= nVal ;

				RECT rcTip = {0};
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[0], pYAxis[0], (LPPOINT)&rcTip );
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[1], pYAxis[1], (LPPOINT)&rcTip + 1 );

				::OffsetRect( &rcTip, rcChart.left, rcChart.top );

				CStatObjectView* pStatObjectView=
					(CStatObjectView*)((char*)this-offsetof(CStatObjectView,m_wndChart));
				if( ::PtInRect( &rcTip, point ) && (vtTableView & pStatObjectView->m_dwViews) )
				{
					::SetCursor( m_cur_hand );
					return __super::on_mousemove( point );
				}
			}
		}

		::SetCursor( m_cur_arrow );


		return __super::on_mousemove( point );
	}

	void CChartWnd::_calc_hint_arr()
	{
		m_ptooltip->DeleteAll( handle() );
		m_ptooltip2->DeleteAll( handle() );

		IStatObjectDispPtr sptrStat;
		{
			TPOS posStat = m_list_attached.head();
			if(posStat) sptrStat=m_list_attached.get(posStat);
			if(0==sptrStat) return;
		}

		if( !m_nChartViewType && m_arrClasses ) 
		{
			IChartMiscHelperPtr sptrHelper = m_sptrChart;
			m_ptooltip->AddTool( handle(), LPSTR_TEXTCALLBACK, 0, m_lClassCount + 2 );

			for( long i = m_lClassCount - 1; i >= 0; i-- )
			{
				long lClass = m_arrClasses[i];

				double *pYAxis = 0;
				double *pXAxis = 0;
				long lc = 0;
				m_sptrChart->GetData( lClass, &pXAxis, &lc, cdfDataX );
				m_sptrChart->GetData( lClass, &pYAxis, &lc, cdfDataY );

				RECT rcChart = m_rcChart;
				IChartAttributesPtr sptrAttr = sptrHelper;

				long nVal = 0;

				sptrAttr->GetConstParams( &nVal, ccfLBorder );
				rcChart.left += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfBBorder );
				rcChart.bottom -= nVal ;

				sptrAttr->GetConstParams( &nVal, ccfTBorder );
				rcChart.top += nVal ;

				sptrAttr->GetConstParams( &nVal, ccfRBorder );
				rcChart.right -= nVal ;

				RECT rcTip = {0};
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[0], pYAxis[0], (LPPOINT)&rcTip );
				sptrHelper->ConvertToDC( lClass, rcChart, pXAxis[1], pYAxis[1], (LPPOINT)&rcTip + 1 );

				::OffsetRect( &rcTip, rcChart.left, rcChart.top );

				if( !m_nXAxisType )
					m_ptooltip->AddTool( handle(),
					((ObjectSpace::CStatisticObject*)(IStatObjectDisp*)sptrStat)->get_class_name(lClass),
					&rcTip, lClass + 1 );
				else
				{
					CString str;
					str.Format( "( %lf - %lf )", pXAxis[0], pXAxis[1] );
					m_ptooltip->AddTool( handle(), str, &rcTip, m_lClassCount + 1 );
				}
			}
		}

		if( m_bShowCurve && m_nXAxisType )
		{
			m_ptooltip2->AddTool( handle(), " ", 0, 1500 );
			m_ptooltip2->Activate( false );
		}
	}

	bool	CChartWnd::_is_stepped_param_chart()
	{
		// [vanek] : игнорируем установку UseCustomIntervals, если количеcтво объектов более одного (сравнение) - 22.11.2004
		if( 1 < m_list_attached.count() ) return false;
		return m_bSteppedParamChart;
	}

	CString CChartWnd::_get_format( UINT nParamKey )
	{
		CString strFormat;
		strFormat.Format( "%ld", long( nParamKey ) );
		strFormat = SECT_STATUI_STAT_PARAM_ROOT "\\" + strFormat;
		strFormat = (char*)::GetValueString( /*m_sptrAttached*/m_list_attached.get( m_list_attached.head() ), strFormat, FORMAT_STRING, "%g" );
		if(""==strFormat)
			strFormat="%g";
		return strFormat;
	}

	int CChartWnd::Print(CDCHandle& dc, CRect rectTarget, CRect RectDraw)
	{
		if (!dc)
			return 0;

		m_bPrinting = true;

		IScrollZoomSitePtr ptr_zoom = (LPUNKNOWN)m_sptrView;
		double fzoom = 1;
		ptr_zoom->GetZoom( &fzoom );

		SIZE sz_client = {0};
		ptr_zoom->GetClientSize( &sz_client );
		RECT rcRect = { 0, 0, long( sz_client.cx * fzoom ), long( sz_client.cy * fzoom ) };

		int nwidth = rcRect.right - rcRect.left;

		{
			const CRect& rcMin=get_min_rect();
			double fX=double(RectDraw.right-RectDraw.left)/rcMin.Width();
			double fY=double(RectDraw.bottom-RectDraw.top)/rcMin.Height();
			double fZ=__min(fX,fY);

			CRect rcLog;
			if(fZ>=1.){
				rcLog=CRect(0, 0, (int)ceil(double(RectDraw.Size().cx)*fZ),
					(int)ceil(double(RectDraw.Size().cy)*fZ));
			}else{
				rcLog=rcMin;
			}

			_resize_child(rcLog);

			dc.SetMapMode(MM_ANISOTROPIC);
			dc.SetWindowOrg(rcLog.left,0);
			dc.SetWindowExt(rcLog.Size());
			dc.SetViewportExt(rectTarget.Size());
			dc.SetViewportOrg(rectTarget.TopLeft());

			CRgn ClipRegion;
			if (ClipRegion.CreateRectRgnIndirect(rectTarget))
				dc.SelectClipRgn(ClipRegion);
			ClipRegion.DeleteObject();

			DoDraw( dc, rcLog, 0, 1., true );

			return 0;
		}
	}
}