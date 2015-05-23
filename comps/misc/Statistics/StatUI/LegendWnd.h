#pragma once

#include "\vt5\awin\win_base.h"

#include "misc.h"
#include "hotbuttom.h"
#include "statisticobject.h"

namespace ViewSpace 
{
	class CLegendWnd : public win_impl
	{
		long m_nDescrWidth, m_bPrintMode, m_sum_value_width;
		HCURSOR m_cur_hand;
		HCURSOR m_cur_arrow;

		valarray<long> m_lst_value_width;

		bool m_nRealCenter, m_bEnableLegendArea;

		CHotButtom m_wHotBtn;
		RECT m_rcClient;
		char m_strClassFile[MAX_PATH];

		_list_t<IUnknownPtr> m_list_attached;

		int m_nLineHeight; 
		CFontObj m_lfFont;

//		COLORREF m_clTextColor;
		COLORREF m_clIncorrectColor;

		long m_lParamCount;
		long m_lCustomDataCount;
		CString m_strInvisibleItems;

		CString m_strTitle;
		RECT m_rcTitle;
		CFontObj m_lfFontTitle;
//		COLORREF m_clTextColorTitle;

		// [vanek] SBT:1260 - 20.12.2004
		CString m_strCompareTitle; // Заголовок в режиме сравнения (всегда читается из shell.data!!!)

		valarray<RECT> m_lst_rect_hot_data;
		valarray<COLORREF> m_lst_color_hot_data;
		valarray<CString> m_lst_text_hot_data;
		long m_lhot_data_height;

		valarray<long> m_lst_max_comma_width;
		struct XParamItem
		{
			char m_szParamName[50];
			RECT m_rectParamName;

			struct XParamValue
			{
				char m_szParamValue[80];
				RECT m_rectParamValue;
				long lCommaPart;
				bool m_bIncorrect;

				XParamValue()
				{
					::ZeroMemory( m_szParamValue, sizeof( m_szParamValue )  );
					::ZeroMemory( &m_rectParamValue, sizeof( RECT ) );
					lCommaPart = 0;
					m_bIncorrect = false;
				}

				void operator=( const XParamValue &item )
				{
					::CopyMemory( m_szParamValue, item.m_szParamValue, sizeof( char ) * 80 );
					::CopyMemory( &m_rectParamValue, &item.m_rectParamValue, sizeof( RECT ) );
					lCommaPart = item.lCommaPart;
					m_bIncorrect = item.m_bIncorrect;
				}


			} *m_pparam_values;
			long m_lparam_values_count;

			char m_szParamFormat[10];

			long lKey;
			long lOrder;
			bool bUserValue;
			long lUserID;


			XParamItem()
			{
				::ZeroMemory( m_szParamName, sizeof( m_szParamName ) );
				::ZeroMemory( &m_rectParamName, sizeof( RECT ) );

				::ZeroMemory( m_szParamFormat, sizeof( m_szParamFormat ) );

				bUserValue = false;
				lKey = 0;
				lOrder = 0;
				lUserID = 0;

				m_pparam_values				= 0;
				m_lparam_values_count		= 0;
			}

			~XParamItem()
			{
				if( m_pparam_values )
					delete []m_pparam_values;
				m_pparam_values = 0;
			}

			XParamItem( const XParamItem &item )
			{
				m_pparam_values				= 0;
				m_lparam_values_count		= 0;

				*this = item;
			}
			void operator=( const XParamItem &item )
			{
				::CopyMemory( m_szParamName, item.m_szParamName,  sizeof( char ) * 50 );
				::CopyMemory( &m_rectParamName, &item.m_rectParamName, sizeof( RECT ) );

				::CopyMemory( m_szParamFormat, item.m_szParamFormat, sizeof( char ) * 10 );

				lKey = item.lKey;
				lOrder = item.lOrder;
				bUserValue = item.bUserValue;
				lUserID = item.lUserID;

				if( m_pparam_values )
					delete []m_pparam_values; m_pparam_values = 0;

				m_lparam_values_count = item.m_lparam_values_count;

				if( m_lparam_values_count > 0 )
				{
					m_pparam_values = new XParamValue[m_lparam_values_count];
					for( long i = 0; i < m_lparam_values_count; i++ )
						m_pparam_values[i] = item.m_pparam_values[i];
				}
			}

		} *m_lpParams;
		CRect m_rcMinRect;

		IUnknownPtr m_sptrView;
	public:
		void SetPrintMode() { m_bPrintMode = true; } 
		bool m_bMinSizeCalc;
		void SetClassFile( LPCTSTR pStr ) { strcpy( m_strClassFile, pStr ); }
		LPCTSTR GetClassFile( ) { return m_strClassFile; }

		LRESULT on_destroy() { m_sptrView = 0; return __super::on_destroy(); }
		void set_view_unkn( IUnknownPtr view ) { m_sptrView = view; }
		RECT m_rcFull; // Всё окно
		POINT m_ptOffset;

		CLegendWnd(void);
		~CLegendWnd(void);

		void deinit();
		LRESULT on_paint();
		LRESULT on_erasebkgnd(HDC hDC) { return 1L; }

		void load_from_ndata(  INamedDataPtr sptrND );
		void store_to_ndata( INamedDataPtr sptrND );

		void attach_data();
		void add_attach_data( IUnknownPtr sptrData ) 
		{ 
			if( !m_list_attached.find( sptrData ) )
				m_list_attached.add_tail( sptrData ); 
			size_t nStats=m_list_attached.count();
			m_lst_value_width.resize(nStats,0);
			m_lst_max_comma_width.resize(nStats,0);
			m_lst_rect_hot_data.resize(nStats, NORECT);
			m_lst_color_hot_data.resize(nStats,0);
			m_lst_text_hot_data.resize(nStats,"");
		};

		const CRect& get_min_rect();

		virtual LRESULT on_size(short cx, short cy, ulong fSizeType);
		virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
		void DoDraw( HDC hDC, RECT rcPaint,double fZoom = 1, bool bPrint = false );
		void calc_min_rect( bool bPrintMode );
		
		LRESULT on_mousemove(const _point &point);
		LRESULT on_lbuttondown(const _point &point);
		LRESULT on_setcursor(unsigned code, unsigned hit) { return 0L; };
		
		void clear_attached() 
		{ 
			m_list_attached.clear();
			m_lst_max_comma_width.resize(0);
			m_lst_value_width.resize(0); 
			m_lst_rect_hot_data.resize(0);
			m_lst_color_hot_data.resize(0);
			m_lst_text_hot_data.resize(0);
		}
		LRESULT handle_message(UINT m, WPARAM w, LPARAM l);
		void _resize_child( RECT rect );
		int Print(CDCHandle& pDC, CRect rectTarget, CRect RectDraw);
	protected:
		//virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
		//virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
private:
	bool m_bPrinting;
};
}