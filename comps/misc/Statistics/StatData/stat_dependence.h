#pragma once
#include "win_view.h"
#include "impl_help.h"
#include "PropBagImpl.h"

#include "hot_btn.h"
#include "charts_int.h"
#include "resource.h"

#include "statistics.h"
#define NULLING( param ) ::ZeroMemory( &param, sizeof( param ) );


namespace STAT_DEPENDENCE
{

	class stat_dependence_view : public CWinViewBase, 
				public IPrintView,
				public IPersist,
				public CNamedObjectImpl,
				public CHelpInfoImpl,
				public CNamedPropBagImpl,
				public _dyncreate_t<stat_dependence_view>
{
		IChartDrawerPtr m_sptr_chart;

		hot_btn m_wnd_hot_btn_chart;
		hot_btn m_wnd_hot_btn_axis;
		hot_btn m_wnd_hot_btn_yparam;
		hot_btn m_wnd_hot_btn_xparam;

		struct x_wnd_params_data
		{
			int cx;
			int cy;
			bool bshowen;
			bool bprint_mode;
			bool block_recalc;
			bool berror;
			CString m_str_error_descr;
			CString m_str_error_descr2;

			x_wnd_params_data()
			{
				m_str_error_descr.LoadString( IDS_NO_DATA );
				m_str_error_descr2.LoadString( IDS_NO_DEPENDANCE );
				cx = cy = 0;
				bshowen = false;
				bprint_mode = false;
				block_recalc = false;
				berror = false;
			}
		} m_wnd_data;

		struct x_wnd_layout
		{
			int title_w;
			int title_h;

			int xparam_w;
			int xparam_h;

			int yparam_w;
			int yparam_h;

			int legend_text_w;
			int legend_text_h;

			int chart_border_w;
			int chart_border_h;

			struct x_legend_layout
			{
				int legend_value_w;
				int legend_value_h;

				x_legend_layout()
				{
					legend_value_w = 0;
					legend_value_h = 0;
				}
			};

			_list_t<x_legend_layout> m_lst_legend_layout;

			int chart_w;
			int chart_h;

			x_wnd_layout()
			{
				title_w = 0;
				title_h = 0;

				legend_text_w = 0;
				legend_text_h = 0;

				xparam_w	= 0;
				xparam_h	= 0;

				yparam_w	= 0;
				yparam_h	= 0;

				chart_w = 0;
				chart_h = 0;

				chart_border_w	=	0;
				chart_border_h	=	0;
			}
		} m_wnd_layout;

		struct x_chart_params
		{
			_list_t<COLORREF> m_lst_chart_color;

			CString str_title;

			//[max]Установки для формата значений по осям
			bool m_buse_custom_format_x;
			bool m_buse_custom_format_y;

			CString m_str_custom_format_x;
			CString m_str_custom_format_y;



			bool buse_loga_scale_x;
			bool buse_loga_scale_y;

			long nx_param_key;
			long ny_param_key;
			bool bx_grid;
			bool by_grid;

 			double fx_scale_factor; 
			double fy_scale_factor;

			double fx_scale_factor_values; 
			double fy_scale_factor_values;

 			double fx_scale_factor_grid; 
			double fy_scale_factor_grid;

 			COLORREF cl_grid_color;
			bool bshow_axis_value_x;
			bool bshow_axis_value_y;

			bool bshow_axis_labels_x;
			bool bshow_axis_labels_y;

			LOGFONT lf_font_title;
			COLORREF cltext_color_title;

			LOGFONT lf_font_legend;
			COLORREF cltext_color_legend;

			LOGFONT lf_font_axis_chart;
			COLORREF cltext_color_axis;

			LOGFONT lf_font_param;
			COLORREF cltext_color_param;

			bool benable_chart_area;
			bool benable_axis_area;
			bool benable_xparam_area;
			bool benable_yparam_area;

			bool bdraw_end_chart_grid_line_x;
			bool bdraw_end_chart_grid_line_y;

			long ndependence_type;
			long nmodel_type;

			bool buser_range_x;
			double frange_min_x;
			double frange_max_x;

			bool buser_range_y;
			double frange_min_y;
			double frange_max_y;

			bool bConnectingLines;
			COLORREF clr_conn_lines;

			struct x_marker_type
			{
				long nmarker_type;
				long nmarker_size;
				COLORREF clmarker_color;
				x_marker_type()
				{
					clmarker_color  = 0;
					nmarker_type				= 2;
					nmarker_size				= 2;
				}
			};
			_list_t<x_marker_type> m_lst_marker;

			x_chart_params()
			{
				init();
			}

			void init()
			{
				nx_param_key = 0;
				ny_param_key = 0;

				str_title.Empty();
				
				benable_chart_area = true;
				benable_axis_area = true;
				benable_xparam_area = true;
				benable_yparam_area = true;

				::ZeroMemory( &lf_font_title, sizeof( lf_font_title ) );
				memset( &lf_font_title, 0, sizeof( LOGFONT ) );
				strcpy( lf_font_title.lfFaceName, "Arial" );
				lf_font_title.lfHeight = 10;
				lf_font_title.lfWeight = 400;
				lf_font_title.lfItalic = 0;
				lf_font_title.lfUnderline = 0;
				lf_font_title.lfStrikeOut = 0;
				lf_font_title.lfCharSet = DEFAULT_CHARSET;
				lf_font_title.lfPitchAndFamily = FF_SWISS;


				::ZeroMemory( &lf_font_legend, sizeof( lf_font_legend ) );
				memset( &lf_font_legend, 0, sizeof( LOGFONT ) );
				strcpy( lf_font_legend.lfFaceName, "Arial" );
				lf_font_legend.lfHeight = 10;
				lf_font_legend.lfWeight = 400;
				lf_font_legend.lfItalic = 0;
				lf_font_legend.lfUnderline = 0;
				lf_font_legend.lfStrikeOut = 0;
				lf_font_legend.lfCharSet = DEFAULT_CHARSET;
				lf_font_legend.lfPitchAndFamily = FF_SWISS;

				::ZeroMemory( &lf_font_axis_chart, sizeof( lf_font_axis_chart ) );
				memset( &lf_font_axis_chart, 0, sizeof( LOGFONT ) );
				strcpy( lf_font_axis_chart.lfFaceName, "Arial" );
				lf_font_axis_chart.lfHeight = 6;
				lf_font_axis_chart.lfWeight = 400;
				lf_font_axis_chart.lfItalic = 0;
				lf_font_axis_chart.lfUnderline = 0;
				lf_font_axis_chart.lfStrikeOut = 0;
				lf_font_axis_chart.lfCharSet = DEFAULT_CHARSET;
				lf_font_axis_chart.lfPitchAndFamily = FF_SWISS;

				::ZeroMemory( &lf_font_param, sizeof( lf_font_param ) );
				memset( &lf_font_param, 0, sizeof( LOGFONT ) );
				strcpy( lf_font_param.lfFaceName, "Arial" );
				lf_font_param.lfHeight = 10;
				lf_font_param.lfWeight = 400;
				lf_font_param.lfItalic = 0;
				lf_font_param.lfUnderline = 0;
				lf_font_param.lfStrikeOut = 0;
				lf_font_param.lfCharSet = DEFAULT_CHARSET;
				lf_font_param.lfPitchAndFamily = FF_SWISS;

				cltext_color_title = 0;
				cltext_color_legend = 0;
				cltext_color_axis = 0;
				cltext_color_param = 0;

				bx_grid = 0; 
				by_grid = 0;

				fx_scale_factor = 25;
				fy_scale_factor = 25;

				buse_loga_scale_x = 0;
				buse_loga_scale_y = 0;

				m_buse_custom_format_x = 0;
				m_buse_custom_format_y = 0;


				fx_scale_factor_values = 50;
				fy_scale_factor_values = 50;

				fx_scale_factor_grid = 50;
				fy_scale_factor_grid = 50;

				cl_grid_color = 0;

				bshow_axis_value_x = 1;
				bshow_axis_value_y = 1;

				bshow_axis_labels_x = 1;
				bshow_axis_labels_y = 1;

				bdraw_end_chart_grid_line_x	= false;
				bdraw_end_chart_grid_line_y	= false;

				ndependence_type		= 1;
				nmodel_type				= 0;

				buser_range_x		= 0;
				frange_min_x		= 0;
				frange_max_x		= 0;

				buser_range_y		= 0;
				frange_min_y		= 0;
				frange_max_y		= 0;

				bConnectingLines = false;
				clr_conn_lines = 0;
			}

		} m_chart_params;

		RECT m_rc_title;
		RECT m_rc_xparam;
		RECT m_rc_yparam;
		RECT m_rc_chart;

		// [vanek] BT2000:4071 display object's name - 09.12.2004
		RECT m_rc_name_text;
		RECT m_rc_count_text;
		RECT m_rc_model_text;
		RECT m_rc_corel_text;


		RECT m_rc_min_rect;

		CString m_sz_xname_param;
		CString m_sz_yname_param;

		CString m_sz_name_text;
		CString m_sz_count_text;
		CString m_sz_model_text;
		CString m_sz_corel_text;

		struct x_legend_data
		{
			RECT rc_name;
			RECT rc_count_value;
			RECT rc_model_value;
			RECT rc_corel_value;

			CString sz_name;
			CString sz_count_value;
			CString sz_model_value;
			CString sz_corel_value;

			x_legend_data()
			{
				_init();
			}

			x_legend_data( const x_legend_data &item )
			{
				_init();
				*this = item;
			}
			void operator=( const x_legend_data &item )
			{
				rc_name = item.rc_name;
				rc_count_value = item.rc_count_value;
				rc_model_value = item.rc_model_value;
				rc_corel_value = item.rc_corel_value;

				sz_name = item.sz_name;
				sz_count_value = item.sz_count_value;
				sz_model_value = item.sz_model_value;
				sz_corel_value = item.sz_corel_value;
			}

			void _init()
			{
				NULLING( rc_name );
				NULLING( rc_count_value );
				NULLING( rc_model_value );
				NULLING( rc_corel_value );

				sz_name.Empty();
				sz_count_value.Empty();
				sz_model_value.Empty();
				sz_corel_value.Empty();
			}
		};

		_list_t<x_legend_data> m_lst_legend;

		CString m_str_max_val_x;
		CString m_str_max_val_y;
		long m_nchart_values_y;
		_list_t<IUnknownPtr> m_list_attached;

		// [vanek] - 23.12.2004
		bool	m_bPrintMode;
public:
	stat_dependence_view(void);
	~stat_dependence_view(void);

	virtual IUnknown*	DoGetInterface( const IID &iid );

	// [vanek] : IView - 24.12.2004
	com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/)
	{
		HRESULT hr = CWinViewBase::GetObjectFlags( bstrObjectType, pdwFlags );
        
		if( _bstr_t(bstrObjectType) == _bstr_t(szTypeStatTable) )
			*pdwFlags |= ofAllSelected;

		return hr;
	}
	
	route_unknown();
protected:
	virtual void		DoAttachObjects();	

	//message handlers
	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	virtual long		on_create( CREATESTRUCT *pcs );
	virtual long		on_destroy();
	virtual long		on_paint();
	virtual long		on_erase_background( HDC hDC );		//WM_ERASEBKGND
	virtual long		on_size( int cx, int cy );			//WM_SIZE	
	virtual long		on_context_menu( HWND hWnd, int x, int y );	//WM_CONTEXTMENU
	
	//interface overridden
	com_call			GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );	
	virtual void		OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//Print support
	com_call			GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
	com_call			GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
	com_call			Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );

	//Persist Impl
	com_call			GetClassID(CLSID *pClassID ); 

	com_call			GetFirstVisibleObjectPosition( long *plpos );
	com_call			GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos );
	/////////////////////////////////////////////////////////////////////////////////////////
protected:
		void _load_from_ndata(  INamedDataPtr sptr_nd );
		void _store_to_ndata( INamedDataPtr sptr_nd );
		void _recalc_layout();


		void _attach_data();
		void _add_attach_data( IUnknownPtr sptr_data ) 
		{ 
			if( !m_list_attached.find( sptr_data ) )
				m_list_attached.add_tail( sptr_data ); 
		};

		RECT _get_min_rect()
		{
			return  m_rc_min_rect;
		}

		void _do_draw( HDC hdc, RECT rc_paint, bool bcalc = false, double fzoom = 1, bool bprint = false );
		void _calc_min_rect( bool bprint_mode );

		void _clear_attached() { m_list_attached.clear(); }
		bool _is_ndata_empty( IUnknownPtr sptr );
		void _set_modify();

		void _init();
		void _deinit();

		double _get_f( int nmodel, double fa, double fb, double x );
		CString _get_f_str( int nmodel, double fa, double fb );

		double _get_correl( IStatTable *punk_table, long lmodel, double *pa, double *pb, bool *pberrr );
		void _manage_labels(  long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y, double fcoef_x, double fcoef_y  );
		void _manage_values(long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y
			, double fcoef_x, double fcoef_y, map_meas_params& map_meas_params);
		void _manage_grid( long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y, double fcoef_x, double fcoef_y );
};
/********************************************************************/
};
