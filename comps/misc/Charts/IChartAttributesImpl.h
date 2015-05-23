#pragma once
#include <wtypes.h>

#include "ifaces.h"

#include "misc.h"
#include "gdiplusmanager.h"
class IChartAttributesImpl : public IChartAttributes
{
protected:
	int m_nScaleDirectionX;
	int m_nScaleDirectionY;

	int m_nDigitOffsetX;
	int m_nDigitOffsetY;

	int m_nMode;
	double m_fTextZoom;
	// Общие цвета
	COLORREF m_clBackColor;
	COLORREF m_clTargetColor;
	COLORREF m_clGridColor;

	bool m_bFillBack;
	bool m_bTargetVert;
	bool m_bTargetHorz;

	ULONG m_lTargetStyle;
	long m_nTargetThick;

	bool m_bAxisOnBorder;
	bool m_bAbsolute;

	POINT m_ptTarget;

	long m_nAxisOffsetX;
	long m_nAxisOffsetY;
	long m_nArrowLength;
	long m_nScaleHalfSize;
	long m_nScaleCountX;
	long m_nScaleCountY;
	long m_nDigitCountX;
	long m_nDigitCountY;
	long m_nLBorder;
	long m_nRBorder;
	long m_nTBorder;
	long m_nBBorder;
	
	double m_fRotAngle;

	DWORD m_dwGridFlags;
	double *m_pfXGridVals;
	double *m_pfYGridVals;
	long m_lGridValsCountX;
	long m_lGridValsCountY;

	struct XHintText
	{
		BSTR m_bstrHintText;
		double fX,fY;
		int nAlign;
		int xOffset, yOffset;
		XHintText()
		{
			xOffset = yOffset = 0;
			m_bstrHintText = 0;
			fX = 0;
			fY = 0;
			nAlign = 0;
		}
		~XHintText()
		{
			if( m_bstrHintText )
				::SysFreeString( m_bstrHintText );
			m_bstrHintText = 0;

			fX = 0;
			fY = 0;
			xOffset = yOffset = 0;

		}
		XHintText( const XHintText &it)
		{
			m_bstrHintText = 0;
			fX = 0;
			fY = 0;
			nAlign = 0;
			xOffset = yOffset = 0;
			*this = it;
		}
		void operator=( const XHintText &it )
		{
			if( m_bstrHintText )
				::SysFreeString( m_bstrHintText );
			m_bstrHintText = 0;

			m_bstrHintText = ::SysAllocString( it.m_bstrHintText );

			fX = it.fX;
			fY = it.fY;
			nAlign = it.nAlign;
			xOffset = it.xOffset;
			yOffset = it.yOffset;
		}
	};

	struct XChartItem
	{
		// цвета
		COLORREF clChartColor, clAxisColor, clTextColor, clGistBack, clHistTextColor;
		double lfMinX, lfMaxX;
		double lfMinY, lfMaxY;
		bool bEnableRangeX,bEnableRangeY, bGistogr;

		bool bAxisX, bAxisY, bAxisArrowX, bAxisArrowY;
		bool bAxisNameX, bAxisNameY;
		bool bAxisDigitsX, bAxisDigitsY;
		bool bAxisScaleX, bAxisScaleY;
		ULONG lChartStyle, lAxisStyle;
		long nChartThick, nAxisThick;

		CStringW bstrFormatDigitsX, bstrFormatDigitsY, bstrNameX, bstrNameY;
		bool bShow;

		double *m_pfXAxisLabels;
		double *m_pfYAxisLabels;
		long m_lAxisLabelsCountX;
		long m_lAxisLabelsCountY;

		double *m_pfXAxisValues;
		double *m_pfYAxisValues;
		double *m_pfXAxisLabelsValues;
		double *m_pfYAxisLabelsValues;
		long m_lAxisValuesCountX;
		long m_lAxisValuesCountY;

		LOGFONT m_lfAxisX;
		LOGFONT m_lfAxisY;
		bool m_bAxisFontX;
		bool m_bAxisFontY;

		LOGFONT m_lfAxisNameX;
		LOGFONT m_lfAxisNameY;
		bool m_bAxisNameFontX;
		bool m_bAxisNameFontY;

		LOGFONT m_lfHint;
		bool bHatch;

		std::vector<XHintText> m_vectHints;

		XChartItem()
		{
			bHatch=false;
			lfMinX = lfMinY = lfMaxX = lfMaxY = 0;
			clHistTextColor = clGistBack = clTextColor = clChartColor = clAxisColor = 0;

			bAxisX = bAxisY = bAxisArrowX = bAxisArrowY = bAxisNameX = 
			bAxisNameY = bAxisDigitsX = bAxisDigitsY = 
			bAxisScaleX = bAxisScaleY = bEnableRangeY = bEnableRangeX = 0;
			bGistogr = bShow = 0;

			::ZeroMemory( &m_lfHint, sizeof( m_lfHint ) );

			::ZeroMemory( &m_lfAxisX, sizeof( m_lfAxisX ) );
			::ZeroMemory( &m_lfAxisY, sizeof( m_lfAxisY ) );

			m_bAxisFontX = false;
			m_bAxisFontY = false;

			::ZeroMemory( &m_lfAxisNameX, sizeof( m_lfAxisNameX ) );
			::ZeroMemory( &m_lfAxisNameY, sizeof( m_lfAxisNameY ) );

			m_bAxisNameFontX = false;
			m_bAxisNameFontY = false;

			lChartStyle = lAxisStyle = 0;
			nChartThick = nAxisThick = 1;

//			bstrNameX = bstrNameY = bstrFormatDigitsX = bstrFormatDigitsY = 0;

			m_pfXAxisLabels = 0;
			m_pfYAxisLabels = 0;
			m_lAxisLabelsCountX = 0;
			m_lAxisLabelsCountY = 0;

			m_pfXAxisValues = 0;
			m_pfYAxisValues = 0;
			m_pfXAxisLabelsValues = 0;
			m_pfYAxisLabelsValues = 0;
			m_lAxisValuesCountX = 0;
			m_lAxisValuesCountY = 0;
		}

		~XChartItem()
		{
			//if( bstrFormatDigitsX )
			//	::SysFreeString( bstrFormatDigitsX );
			//if( bstrFormatDigitsY )
			//	::SysFreeString( bstrFormatDigitsY );
			//if( bstrNameX )
			//	::SysFreeString( bstrNameX );
			//if( bstrNameY )		    
			//	::SysFreeString( bstrNameY );

			if( m_pfXAxisLabels )
				delete []m_pfXAxisLabels;
			m_pfXAxisLabels = 0;

			if( m_pfYAxisLabels )
				delete []m_pfYAxisLabels;
			m_pfYAxisLabels = 0;

			m_lAxisLabelsCountX = 0;
			m_lAxisLabelsCountY = 0;

			if( m_pfXAxisValues )
				delete []m_pfXAxisValues;
			m_pfXAxisValues = 0;


			if( m_pfXAxisLabelsValues )
				delete []m_pfXAxisLabelsValues;
			m_pfXAxisLabelsValues = 0;

			if( m_pfYAxisLabelsValues )
				delete []m_pfYAxisLabelsValues;
			m_pfYAxisLabelsValues = 0;

			if( m_pfYAxisValues )
				delete []m_pfYAxisValues;
			m_pfYAxisValues = 0;

			m_lAxisValuesCountX = 0;
			m_lAxisValuesCountY = 0;

			m_vectHints.clear();
		}
		
		void operator=( const XChartItem &chart )
		{
			bHatch=chart.bHatch;
			lfMinX = chart.lfMinX;
			lfMaxX = chart.lfMaxX;
			lfMinY = chart.lfMinY;
			lfMaxY = chart.lfMaxY;

			bEnableRangeX = chart.bEnableRangeX;
			bEnableRangeY = chart.bEnableRangeY;

			clChartColor = chart.clChartColor;
			clAxisColor = chart.clAxisColor;
			clTextColor = chart.clTextColor;
			clGistBack = chart.clGistBack;
			clHistTextColor = chart.clHistTextColor;

			bAxisX = chart.bAxisX;
			bAxisY = chart.bAxisY;

			bAxisArrowX = chart.bAxisArrowX;
			bAxisArrowY = chart.bAxisArrowY;

			bAxisNameX = chart.bAxisNameX;
			bAxisNameY = chart.bAxisNameY;

			bAxisDigitsX = chart.bAxisDigitsX;
			bAxisDigitsY = chart.bAxisDigitsY;

			bAxisScaleX = chart.bAxisScaleX;
			bAxisScaleY = chart.bAxisScaleY;

			lChartStyle = chart.lChartStyle;
			lAxisStyle = chart.lAxisStyle;

			nChartThick = chart.nChartThick;
			nAxisThick = chart.nAxisThick;

			bShow = chart.bShow;
			bGistogr = chart.bGistogr;

			//if( bstrFormatDigitsX )
			//{ ::SysFreeString( bstrFormatDigitsX ); bstrFormatDigitsX = 0; }

			//if( bstrFormatDigitsY )
			//{ ::SysFreeString( bstrFormatDigitsY ); bstrFormatDigitsY = 0; }

			//if( bstrNameX )
			//{::SysFreeString( bstrNameX ); bstrNameX = 0; }
			//if( bstrNameY )		    
			//{::SysFreeString( bstrNameY ); bstrNameY = 0; }

			//if( chart.bstrFormatDigitsX )
			//	bstrFormatDigitsX = ::SysAllocString( chart.bstrFormatDigitsX );

			//if( chart.bstrFormatDigitsY )
			//	bstrFormatDigitsY = ::SysAllocString( chart.bstrFormatDigitsY );

			//if( bstrNameX )
			//	bstrNameX = ::SysAllocString( chart.bstrNameX );
			//if( bstrNameY )
			//	bstrNameY = ::SysAllocString( chart.bstrNameY );
			
			ColorizedMap = chart.ColorizedMap;
			m_lfHint = chart.m_lfHint;

			m_vectHints.clear();

			for( int i = 0; i < chart.m_vectHints.size(); i++ )
				m_vectHints.push_back( chart.m_vectHints[i] );

			{
				if( m_pfXAxisLabels )
					delete []m_pfXAxisLabels;
				m_pfXAxisLabels = 0;

				if( m_pfYAxisLabels )
					delete []m_pfYAxisLabels;
				m_pfYAxisLabels = 0;

				m_lAxisLabelsCountX = 0;
				m_lAxisLabelsCountY = 0;

				if( m_pfXAxisValues )
					delete []m_pfXAxisValues;
				m_pfXAxisValues = 0;

				if( m_pfYAxisValues )
					delete []m_pfYAxisValues;
				m_pfYAxisValues = 0;

				if( m_pfXAxisLabelsValues )
					delete []m_pfXAxisLabelsValues;
				m_pfXAxisLabelsValues = 0;

				if( m_pfYAxisLabelsValues )
					delete []m_pfYAxisLabelsValues;
				m_pfYAxisLabelsValues = 0;

				m_lAxisValuesCountX = 0;
				m_lAxisValuesCountY = 0;
			}

			{
				m_lAxisLabelsCountX = chart.m_lAxisLabelsCountX;
				m_lAxisLabelsCountY = chart.m_lAxisLabelsCountY;

				m_pfXAxisLabels = new double[m_lAxisLabelsCountX];
				m_pfYAxisLabels = new double[m_lAxisLabelsCountY];

				::CopyMemory( m_pfXAxisLabels, chart.m_pfXAxisLabels, sizeof( double ) * m_lAxisLabelsCountX );
				::CopyMemory( m_pfYAxisLabels, chart.m_pfYAxisLabels, sizeof( double ) * m_lAxisLabelsCountY );

				m_lAxisValuesCountX = chart.m_lAxisValuesCountX;
				m_lAxisValuesCountY = chart.m_lAxisValuesCountY;

				m_pfXAxisValues = new double[m_lAxisValuesCountX];
				m_pfYAxisValues = new double[m_lAxisValuesCountY];

				::CopyMemory( m_pfXAxisValues, chart.m_pfXAxisValues, sizeof( double ) * m_lAxisValuesCountX );
				::CopyMemory( m_pfYAxisValues, chart.m_pfYAxisValues, sizeof( double ) * m_lAxisValuesCountY );

				m_pfXAxisLabelsValues = new double[m_lAxisValuesCountX];
				m_pfYAxisLabelsValues = new double[m_lAxisValuesCountY];

				::CopyMemory( m_pfXAxisLabelsValues, chart.m_pfXAxisLabelsValues, sizeof( double ) * m_lAxisValuesCountX );
				::CopyMemory( m_pfYAxisLabelsValues, chart.m_pfYAxisLabelsValues, sizeof( double ) * m_lAxisValuesCountY );
			}

			::CopyMemory( &m_lfAxisX, &chart.m_lfAxisX, sizeof( m_lfAxisX ) );
			::CopyMemory( &m_lfAxisY, &chart.m_lfAxisY, sizeof( m_lfAxisY ) );

			m_bAxisFontX = chart.m_bAxisFontX;
			m_bAxisFontY = chart.m_bAxisFontY;

			::CopyMemory( &m_lfAxisNameX, &chart.m_lfAxisNameX, sizeof( m_lfAxisNameX ) );
			::CopyMemory( &m_lfAxisNameY, &chart.m_lfAxisNameY, sizeof( m_lfAxisNameY ) );

			m_bAxisNameFontX = chart.m_bAxisNameFontX;
			m_bAxisNameFontY = chart.m_bAxisNameFontY;
		}

		class XColorizedMap
		{	
			struct XColorizedItem
			{
				double fMinX, fMaxX;
				double fMinY, fMaxY;
				DWORD Color;
				XColorizedItem()
				{
					fMinX = fMaxX = 0;
					fMinY = fMaxY = 0;
					Color = 0;
				}
				XColorizedItem( DWORD lColor, double lfMinX, double lfMinY, double lfMaxX, double lfMaxY )
				{
					fMinX = lfMinX;
					fMaxX = lfMaxX;

					fMinY = lfMinY;
					fMaxY = lfMaxY;

					Color = lColor;
				}

				void operator=( const XColorizedItem &item )
				{
					fMinX  = item.fMinX;
					fMaxX  = item.fMaxX;
					
					fMinY  = item.fMinY;
					fMaxY  = item.fMaxY;
					
					Color = item.Color;
				}

			};
			std::vector<XColorizedItem> m_Range;
			XChartItem *m_pThis;
		public:
			XColorizedMap()								 { m_pThis = 0;		  }
			void operator=( const XColorizedMap &map)
			{
				m_Range.clear();

				size_t nSz = map.m_Range.size();
				for( size_t i = 0; i < nSz; i++ )
					m_Range.push_back( map.m_Range[i] );

				m_pThis = map.m_pThis;
			}

			void Clear()
			{
				m_pThis = 0;
				m_Range.clear();
			}

			void Set( XChartItem *pThis, DWORD lColor, double lfMinX, double lfMinY, double lfMaxX, double lfMaxY )
			{ 
				m_pThis = pThis;
				m_Range.push_back( XColorizedItem( lColor, lfMinX, lfMinY, lfMaxX, lfMaxY ) );  
			}

			Bitmap *CreateBitmap( RECT rcReal, int ptX, int ptY, int nW, int nH, double ScaleX, double ScaleY, double lfMinX, double lfMinY ) // Удалить после использования
			{
				if( !m_Range.size() )
					return 0;

				typedef std::vector<XColorizedItem>::iterator IT;

				Bitmap *img = new Bitmap( rcReal.right - rcReal.left + 1, rcReal.bottom - rcReal.top + 1);
				Graphics g( img );

				SolidBrush brChart( Color( 0xFF, GetRValue( m_pThis->clChartColor ), GetGValue( m_pThis->clChartColor ), GetBValue( m_pThis->clChartColor ) ) );
				g.FillRectangle( &brChart, 0, 0, rcReal.right - rcReal.left + 1, rcReal.bottom - rcReal.top + 1 );
				
				g.TranslateTransform( ptX - rcReal.left, ptY - rcReal.top );

				for( IT it = m_Range.begin(); it != m_Range.end(); it++ )
				{
					XColorizedItem &item = *it;

					int nX  = INT( ScaleX * ( item.fMinX - lfMinX ) );
					int nX2 = INT( ScaleX * ( item.fMaxX - lfMinX ) );

					int nY  = INT( nH - ScaleY * ( item.fMinY - lfMinY ) );
					int nY2 = INT( nH - ScaleY * ( item.fMaxY - lfMinY ) );

					if( nX < 0 )
						nX = 0;

					if( nX > nW )
						nX = nW;

					if( nY2 < 0 )
						nY2 = 0;

					if( nY > nH )
						nY = nH;


					SolidBrush br( Color( 0xFF, GetRValue( item.Color ), GetGValue( item.Color ), GetBValue( item.Color ) ) );
					g.FillRectangle( &br, nX, nY2, nX2 - nX, nY - nY2 );
				}

				return img;
			}
		};
		XColorizedMap ColorizedMap;
	};
public:
	_map_t2<long,XChartItem> m_Charts;
	int m_xShiftYAxis;
	IChartAttributesImpl()
		: m_xShiftYAxis(0)
	{ 
		do_default();
	}

	void IChartAttributesImpl::do_default( bool bConst = true )
	{
		if( !bConst )
		{
			if( m_pfXGridVals )
				delete []m_pfXGridVals;
			m_lGridValsCountX = 0;
			if( m_pfYGridVals )
				delete []m_pfYGridVals;
			m_pfYGridVals = 0;
			m_lGridValsCountY = 0;
		}

		m_nMode = -1;
		m_nScaleDirectionX = 1;
		m_nScaleDirectionY = 1;

		m_nDigitOffsetX = 0;
		m_nDigitOffsetY = 0;


		m_fTextZoom = 1;
		m_ptTarget.x = m_ptTarget.y = 0; 
		m_clBackColor = m_clTargetColor = 0;
		m_clGridColor = 0;

		m_bFillBack = m_bTargetVert = m_bTargetHorz = 0;
		m_lTargetStyle = 0;
		m_nTargetThick = 1;
		m_bAxisOnBorder = 0;
		m_bAbsolute = 0;

		m_nAxisOffsetX = 15;
		m_nAxisOffsetY = 30;
		m_nArrowLength = 8;
		m_nScaleHalfSize = 2;
		m_nScaleCountX = m_nScaleCountY = m_nDigitCountX = m_nDigitCountY = 20;
		m_nBBorder = 15;
		m_nLBorder = -1; 
		m_nRBorder = m_nTBorder = 5;
		m_fRotAngle = 0;

		m_dwGridFlags = 0;
		
		m_pfXGridVals = 0;
		m_pfYGridVals = 0;
		m_lGridValsCountX = 0;
		m_lGridValsCountY = 0;
	}
	
	~IChartAttributesImpl() 
	{
		if( m_pfXGridVals )
			delete []m_pfXGridVals;
		m_lGridValsCountX = 0;
		if( m_pfYGridVals )
			delete []m_pfYGridVals;
		m_pfYGridVals = 0;
		m_lGridValsCountY = 0;
	}

	// Установа/Получение цвета. 
	com_call SetColor( DWORD lColor, CommonChartColors dwFlag );
	com_call GetColor( DWORD *lColor, CommonChartColors dwFlag );

	com_call SetChartColor( long nID, DWORD lColor, ChartColors dwFlag );
	com_call GetChartColor(long nID, DWORD *lColor, ChartColors dwFlag );

	// Установа/Получение общего.
	com_call SetStyle( CommonChartViewParam dwFlag );
	com_call GetStyle( CommonChartViewParam *dwFlag );

	com_call SetChartStyle(long nID, ChartViewParam dwFlag );
	com_call GetChartStyle(long nID, ChartViewParam *dwFlag );

	// Установа/Получение стиля линии.
	com_call SetLineStyle( ULONG lStyle, CommonChartLineStyles dwFlag );
	com_call GetLineStyle( ULONG *lStyle, CommonChartLineStyles dwFlag );

	com_call SetChartLineStyle(long nID, ULONG lStyle, ChartLineStyles dwFlag );
	com_call GetChartLineStyle(long nID, ULONG *lStyle, ChartLineStyles dwFlag );

	// Установа/Получение толoщины линии.
	com_call SetLineThick( long nThick,   CommonChartLineThickness dwFlag );
	com_call GetLineThick( long *nThick, CommonChartLineThickness dwFlag );

	com_call SetChartLineThick(long nID, long nThick,   ChartLineThickness dwFlag );
	com_call GetChartLineThick (long nID, long *nThick, ChartLineThickness dwFlag );

	// Область графика на которую будут рисовать
	com_call SetChartRange( long nID, double lfMin, double lfMax, ChartRange dwFlag );
	com_call GetChartRange( long nID, double *lfMin, double *lfMax, ChartRange dwFlag );

	com_call SetRangeState( long nID, int bEnable, ChartRange dwFlag );
	com_call GetRangeState( long nID, int *bEnable, ChartRange dwFlag );

	// Установа положение визира.
	com_call SetTargetPos( POINT ptPos );

	// Возвражает текст с визирной координатой.
	com_call SetTextFormat( long nID, BSTR bstrFormat, ChartTextFormat dwFlag );

	// Удаляет график и данные связанные с ним.
	com_call RemoveChart( long nID );

	// Удаляет график и данные связанные с ним.
	com_call ShowChart( long nID, int bShow );

	// Установа/Получение jбозначение осей оси.
	com_call SetAxisText( long nID, BSTR bstrText, ChartAxis dwFlag );
	com_call GetAxisText( long nID, BSTR *bstrText, ChartAxis dwFlag );

	// Установа/Получение различный параметров.
	com_call SetConstParams( long nVal, CommonChartConst dwFlag );
	com_call GetConstParams( long *nVal, CommonChartConst dwFlag );

	// Установа/Получение различный параметров.
	com_call SetColorizedMap( long nID, DWORD lColor, double lfMinX, double lfMinY, double lfMaxX, double lfMaxY );
	com_call ClearColorized(  long nID );

	com_call EnableGridLines( CommonChartGrid dwFlags );
	com_call SetGridLines( double *fGridVals, long lCount, CommonChartGrid dwFlags );

	com_call SetScaleLabels( long nID, double *pfVals, long lCount, ChartAxis dwFlags );
	com_call SetScaleTextLabels( long nID, double *pfVals, double *pfLabels, long lCount, ChartAxis dwFlags );

	com_call SetAxisTextFont( long nID, BYTE *plfFont, ChartAxis dwFlags, ChartFont dwFlag2 );

	com_call RemoveHints(/* [in] */long nID );
	com_call SetHintText( long nID, double fX, double fY, BSTR bstrHint, int nAlign, int XOffsetInPixel, int YOffsetInPixel  );
	com_call SetHintFont( long nID, BYTE *plfFont );

	com_call SetTextZoom( double fZoom );
	com_call GetTextZoom( double *pfZoom );
	com_call SetPixelUnit( /*[in]*/ int nUnit );
};
