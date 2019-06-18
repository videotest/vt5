#pragma once

#include "\vt5\awin\win_base.h"
#include "\vt5\controls2\listctrl_.h"

#include "misc.h"
#include "hotbuttom.h"

#include "WGridView.h"
#include "statisticobject.h"

#define bstrCLASS_COL_WIDTH CComBSTR(L"\\StatUI\\StatTable\\ClassColWidth")
#define bstrDOC_COL_WIDTH CComBSTR(L"\\StatUI\\StatTable\\DocColWidth")
#define bstrSTAT_PARAMS  _bstr_t(L"\\StatUI\\StatParams\\")

namespace ViewSpace 
{

	class CCmpStatObjectView;

	class CCmpTableWnd : public win_impl
	{
		COL_TO_ROW m_mapColumn;
		ROW_TO_VALUE m_mapColumnName;
		ROW_TO_VALUE2 m_mapColumnOrder;

		IUnknownPtr m_sptrAttached;

		CRect m_rcMinRect;
		char m_strClassFile[MAX_PATH];

//		CStatGrid m_grid, m_gridTotal, m_gridClass;
		friend CCmpTableWnd;
		friend CCmpStatObjectView;

		CTitleTableWnd m_WndTitleTable;
		friend CTitleTableWnd;

		_list_t<long> m_list_classes;

		bool m_bEnableTableArea, m_bPrintMode;

		RECT m_rcClient;

		int m_xGridOffset, m_yGridOffset;
		LOGFONT m_lfTableFont;
		LOGFONT m_lfHeaderTableFont;
		COLORREF m_clIncorrectColor, m_clTextColor, m_clTextColorHeader;

		CString m_strInvisibleItems;

		CString m_strClassNameCol, m_strAllName;

		long m_nTblAlign;

		long m_lParamCount;

		struct XColumnInfo
		{
			COLORREF clColumnColor;
			char szFormat[10];
			char szName[50];
			long nWidth;
			long lOrder;
			bool bUserData;
			long lParamKey;
			long lUsedID;
			long lIndex;

			XColumnInfo()
			{
				::ZeroMemory( szFormat, sizeof( szFormat ) );
				::ZeroMemory( szName, sizeof( szName )  );

				bUserData = false;
				lOrder = 0;
				clColumnColor = 0;
				nWidth = 100;
				lParamKey = 0;
				lIndex = lUsedID = 0;
			}

			XColumnInfo( const XColumnInfo &item )
			{
				*this = item;
			}

			void operator=( const XColumnInfo &item )
			{
				::CopyMemory( szFormat, item.szFormat,  sizeof( char ) * 10 );
				::CopyMemory( szName, item.szName,  sizeof( char ) * 50 );

				lOrder = item.lOrder;
				clColumnColor = item.clColumnColor;
				nWidth = item.nWidth;
				bUserData = item.bUserData;
				lParamKey = item.lParamKey;
				lUsedID = item.lUsedID;
				lIndex = item.lIndex;
			}


		} *m_pColumnInfo;

//		_list_t<IUnknownPtr> m_list_attached;

		friend class CStatGrid;
		IUnknownPtr m_sptrView;
		
//		CString m_strTitle;
		//LOGFONT m_lfFontTitle;
		//COLORREF m_clTextColorTitle;

		long m_lCustomDataCount;
	public:
		CCmpStatObjectView& _rStatCmpView;
		RECT m_rcTitle;
//		CHotButtom m_wHotBtn;
		CWGridView m_Grid;

		void SetPrintMode() { m_bPrintMode = true; } 
		bool m_bMinSizeCalc;
		bool m_bSetModify;
		void SetClassFile( LPCTSTR pStr ) { strcpy( m_strClassFile, pStr ); }
		LPCTSTR GetClassFile( ) { return m_strClassFile; }

		void calc_min_rect( bool bPrintMode );
		long on_destroy() { m_sptrView = 0; return __super::on_destroy(); }
		void set_view_unkn( IUnknownPtr view ) { m_sptrView = view; }
		RECT get_column_rect( int iIndex );
		RECT get_row_rect( int iIndex );

		RECT m_rcFull; // Всё окно
		POINT m_ptOffset;

		CCmpTableWnd();
		~CCmpTableWnd();

		void deinit();

		void set_modify() 
		{ 
			//if( m_list_attached.count() ) 
			//{
			//	for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
			//	{
			//		IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
			//		if( ptr_object )
			//			SetModifiedFlagToObj( ptr_object ); 
			//	}
			//}
		}
		long on_paint();

		void load_from_ndata(  INamedDataPtr sptrND );
		void store_to_ndata( INamedDataPtr sptrND );

		virtual long on_size( short cx, short cy, ulong fSizeType );
		virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
		const CRect& get_min_rect();

		void attach_data();
		void add_attach_data( IUnknownPtr sptrData );

		void DoDraw( HDC hDC, RECT rcPaint, double fZoom = 1, bool bPrint = false );
		long on_erasebkgnd( HDC hDC )
		{
			return 1;//__super::on_erasebkgnd(hDC);
		}
		long on_hscroll( unsigned code, unsigned pos, HWND hwndScroll );
		long on_vscroll( unsigned code, unsigned pos, HWND hwndScroll );
		long on_notify(uint idc, NMHDR *pnmhdr);

//		void clear_attached() { m_list_attached.clear(); }
		long handle_message(UINT m, WPARAM w, LPARAM l);
		void _resize_child( RECT rect );
	protected:
		CString _get_user_value( long lCol, long lRow );
		//print functions
		virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
		virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
		virtual int Print(CDCHandle& pDC, CRect rectTarget, CRect RectDraw);
private:
	valarray<CLVItem> m_arrClassItem;
	StringMatrix m_strVals;
	int m_rowLastVisible;
	bool m_bPrinting;
	int GetTableWidth() const;
	int GetTableHeight() const;
};
}