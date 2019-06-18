#pragma once

#include "\vt5\awin\win_base.h"
#include "charts_int.h"
#include "hotbuttom.h"
#include <atlstr.h>
#include "tooltip.h"
#include "\vt5\awin\misc_map.h"

namespace ViewSpace 
{
	using namespace ObjectSpace;
	class CCmpChartWnd : public win_impl
	{
//	public:
		int m_iError;
		bool m_bPrintMode;
		long m_nClassStep;
		long m_nMinClass;
		int m_LogScaleStep;
		double m_fCoefParam;

		CString m_strClassName;
		bool m_bGistFilled;
		_list_t<int> m_vectParams;
		_map_t<CString,long, cmp_long> m_vectParamsStr;
		_map_t<RECT,long, cmp_long> m_vectParamsRect;
		double m_fMinYValue;
		double m_fMaxYValue;
    double m_fMinXValue;
		double m_fMaxXValue;

		long *m_arrClasses;
		float m_nMaxYChart;
		CString m_strChartX, m_strChartY;
		char m_strClassFile[MAX_PATH];
		long m_nParamsHeight;
		_list_t<long> m_list_classes;

		bool m_b_use_custom_format_x;
		bool m_b_use_custom_format_y;

		CString m_str_custom_format_x;
 		CString m_str_custom_format_y;


		CToolTip32 *m_ptooltip;
		CToolTip32 *m_ptooltip2;
		bool m_bUseLogaScaleX;

		bool m_bDrawEndChartGridLineX, m_bDrawEndChartGridLineY;

		bool m_bShowSingleColorBar,m_bSteppedParamChart;
		COLORREF m_clBarSingleColor;

		CString *m_pstrCustomParams; long m_lCustomParamCount;
		double *m_parrParamChartMin,*m_parrParamChartMax;

		long m_lClassCount;

		CRect m_rcMinRect;
		RECT m_rcClient; // Доступная для рисования часть
		RECT m_rcChart; // График

		int m_nHotZone;

		int m_nYTitle;
		int m_nY_XParam;
		int m_nY_Legend;

		RECT m_rcXParam; // Вертикальный параметр
		RECT m_rcYParam; // Горизонтальный параметр
		RECT m_rcLegend; // Горизонтальный параметр
		RECT m_rcTitle;  // Место под название
		CString m_strTitle; // Заголовок

// Calculated for minimum window sizes
		RECT m_rcMinXParam; // Вертикальный параметр
		RECT m_rcMinYParam; // Горизонтальный параметр
		RECT m_rcMinLegend; // Горизонтальный параметр
		RECT m_rcMinTitle;  // Место под название
		RECT m_rcMinChart; // График

		// [vanek] SBT:1260 - 20.12.2004
		CString m_strCompareTitle; // Заголовок в режиме сравнения (всегда читается из shell.data!!!)

		//CHotButtom m_wHotBtnChart;
		//CHotButtom m_wHotBtnAxis;
		//CHotButtom m_wHotBtnYParam;
		//CHotButtom m_wHotBtnXParam;

		bool m_bEnableChartArea;
		bool m_bEnableAxisArea;
		bool m_bEnableXParamArea;
		bool m_bEnableYParamArea;


		IChartDrawerPtr m_sptrChart;
//		_list_t<IUnknownPtr> m_list_attached;
//		_list_t<COLORREF> m_list_color_chart;
//		_list_t<COLORREF> m_list_color_curve;

		CString m_szXNameParam;
		CString m_szYNameParam;

		// Настройки осей графика
		long m_nXAxisType; // Класс или параметр
		double m_fBarWidth; // Ширина столбцов гистограммы.
		double m_fBarWidthSpace; // Ширина между столбцами в процентах от ширины столбца
		int m_nGrayScaleLevel;
		
		long m_nChartType; // Тип графика. Значение: [0(плотность вероятности);1(куммулята);2(плотность вероятности для логарифма сл. вел.)]
		long m_nChartViewType; //Вид графика. Значение: [0(столбовой);1(линейный)] Default: 0
		COLORREF m_clChartColor; // Цвет отображения линейного графика
	  bool m_bShowGrayScale; // Вид раскраски столбцов. 
		long m_nShowLegend; // Показывать легенду. Значение: [0(легенда);1(подпись)] Default: 0

		bool m_bShowBeginEnd; // Качало, конец. Значение: [0;1] Default: 1
		bool m_bShowStep; // Шаг. Значение: [0;1] Default: 1
		bool m_bShowClassCount; // Кол-во классов. Значение: [0;1] Default: 1
		bool m_bShowCheckModel; // Показывать критерий проверки. Значение: [0;1] Default: 1
		long m_nCheckModelType; // Тип критерия проверки. Значение: [0(хи-квадрат);1(Фишер)] Default: 0
		CString m_strUserSignature; // Подпись пользователя

		bool m_bShowAxisValueX, m_bShowAxisValueY;
		bool m_bShowAxisLabelsX, m_bShowAxisLabelsY;

		double m_fX_AxisBegin;
		double m_fX_AxisEnd;
		bool m_bX_UseValueRange;

		double m_fY_AxisBegin;
		double m_fY_AxisEnd;	                                      
		bool m_bY_UseValueRange;

		bool m_bShowCurve;
		long m_lUniformDistribution;
		COLORREF m_clCurveColor;
		double m_fCurveSharp;

		bool m_bShowUserCurve;
		COLORREF m_clUserCurveColor;
		double m_fUserCurveMX;
		double m_fUserCurveDX;

		HCURSOR m_cur_hand;
		HCURSOR m_cur_arrow;

		CString m_strChartBeginEnd;
		CString m_strChartBeginEndVal;
		CString m_strChartStep;
		CString m_strChartStepVal;
		CString m_strChartClasses;
		CString m_strChartClassesVal;
		CString m_strTableNumber;
		CStringW m_strTableNumberVal;
		CString m_szDBClickScript;

		long m_nXParamKey; // Тип параметра.
		long m_nYParamKey; // Тип параметра.
		bool m_bConvertToCalibr;

		CFontObj m_lfFontTitle;
		CFontObj m_lfFontSignature;
		CFontObj m_lfFontLegend;
		CFontObj m_lfFontAxisChart;
		CFontObj m_lfFontHint;
		CFontObj m_lfFontParam;
		CFontObj m_lfFontParam2;

	  bool m_bXGrid;
	  bool m_bYGrid;

		double m_fXScaleFactor;
		double m_fYScaleFactor;

		double m_fXScaleFactorValues;
		double m_fYScaleFactorValues;

		double m_fXScaleFactorGrid;
		double m_fYScaleFactorGrid;

		COLORREF m_clGridColor;
		IUnknownPtr m_sptrView;

		long m_lTextOffsetX;
		long m_lTextOffsetY;
	public:
		CCmpStatObjectView& _rStatCmpView;
		void SetPrintMode() { m_bPrintMode = true; } 
		bool m_bMinSizeCalc;

		// [vanek] BT2000:4160 - 14.12.2004
		bool m_bShowErrorMsg;

		// A.M. BT 4584. Copy from parent's m_dwViews
		DWORD m_dwViews;

		void SetClassFile( LPCTSTR pStr ) { strcpy( m_strClassFile, pStr ); }
		LPCTSTR GetClassFile( ) { return m_strClassFile; }

		long on_destroy() 
		{ 
			m_sptrView = 0; 

			m_ptooltip->Activate( 0 );
			m_ptooltip2->Activate( 0 );

			delete m_ptooltip;
			delete m_ptooltip2;

			return __super::on_destroy(); 
		}
		void set_view_unkn( IUnknownPtr view ) { m_sptrView = view; }
		RECT m_rcFull; // Всё окно
		POINT m_ptOffset;

		CCmpChartWnd();
		~CCmpChartWnd();
		long on_paint();
//		long on_erasebkgnd( HDC hDC ) { return 1L; }
		long on_mousemove( const _point &point );
		long on_lbuttonup( const _point &point );
		long on_lbuttondblclk( const _point &point );

		void load_from_ndata(  INamedDataPtr sptrND );
		void store_to_ndata( INamedDataPtr sptrND );

		void attach_data();

		const CRect& get_min_rect();
		void _calc_gist_value();
		long on_size( short cx, short cy, ulong fSizeType );
		virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
		void DoDraw( HDC hDC, RECT rcPaint, RECT *rcCalc = 0,double fZoom = 1, bool bPrint = false );
		void calc_min_rect( BOOL bPrintMode );



		long handle_message( UINT m, WPARAM w, LPARAM l )
		{
			switch( m )
			{
			case WM_SETCURSOR:
				return 0L;
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MOUSEMOVE:
				{
					DWORD pos = ::GetMessagePos();
					POINT pt = { LOWORD(pos), HIWORD(pos) };

					MSG Msg = {0};
					Msg.hwnd = handle();
					Msg.message = m;
					Msg.lParam = l;
					Msg.wParam = w;
					Msg.pt = pt;
					Msg.time = ::GetMessageTime();

					//					m_ptooltip->RelayEvent( &Msg );
					//					m_ptooltip2->RelayEvent( &Msg );
					break;
				}
			case WM_USER_MIN_SIZE:
				{
					CRect rect=get_min_rect();
					return MAKELRESULT(rect.Width(),rect.Height());
				}break;
			}
			return __super::handle_message( m, w, l );
		}

		void _resize_child( RECT rect );
		virtual int Print(CDCHandle& pDC, CRect rectTarget, CRect RectDraw);
	protected:
		CString _get_format( UINT nParamKey );
		bool _get_loga_scale(  dblArray& Vals, double fMinVal, double fMaxVal, double dStep);
		double _calc_chi_square( IUnknownPtr sptrO, long *plClassArray, long lClassCount  );
		double _calc_fisher(	long *plClassArray, long lClassCount  );

		void _calc_hint_arr();

		//virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
		//virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	private:
		HFONT m_hFontSignature;
		int m_DCDispLOGPIXELSY;
		int m_DCLOGPIXELSY;
		bool m_bPrinting;
		BOOL m_bColorHatch;
};
}