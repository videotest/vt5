#include "StdAfx.h"
#include "statisticobject.h"
#include "StatUI.h"
#include "tablewnd.h"
#include "statobjectview.h"

namespace ViewSpace 
{
	CTableWnd::CTableWnd() // : m_grid(1), m_gridTotal(2), m_gridClass(0)
		: m_rowLastVisible(-1)
		, m_bPrinting(false)
	{
		m_bPrintMode = false;
		m_lCustomDataCount = 0;
		m_nTblAlign = 1;
		m_bMinSizeCalc = false;
		m_bSetModify = true;
		::ZeroMemory( &m_strClassFile, sizeof( m_strClassFile ) );

		m_bEnableTableArea = true;

		m_xGridOffset = 5;
		m_yGridOffset = 5;
		m_pColumnInfo = 0;

		::ZeroMemory( &m_lfTableFont, sizeof( m_lfTableFont ) );
		::ZeroMemory( &m_lfHeaderTableFont, sizeof( m_lfHeaderTableFont ) );

		strcpy( m_lfTableFont.lfFaceName, "Arial" );
		m_lfTableFont.lfHeight = 10;
		m_lfTableFont.lfWeight = 400;
		m_lfTableFont.lfItalic = 0;
		m_lfTableFont.lfUnderline = 0;
		m_lfTableFont.lfStrikeOut = 0;
		m_lfTableFont.lfCharSet = DEFAULT_CHARSET;
		m_lfTableFont.lfPitchAndFamily = FF_SWISS;

		strcpy( m_lfHeaderTableFont.lfFaceName, "Arial" );
		m_lfHeaderTableFont.lfHeight = 10;
		m_lfHeaderTableFont.lfWeight = 400;
		m_lfHeaderTableFont.lfItalic = 0;
		m_lfHeaderTableFont.lfUnderline = 0;
		m_lfHeaderTableFont.lfStrikeOut = 0;
		m_lfHeaderTableFont.lfCharSet = DEFAULT_CHARSET;
		m_lfHeaderTableFont.lfPitchAndFamily = FF_SWISS;


		::ZeroMemory( &m_rcTitle, sizeof( m_rcTitle ) );
		::ZeroMemory( &m_WndTitleTable.m_lfFontTitle, sizeof( m_WndTitleTable.m_lfFontTitle ) );
		memset( &m_WndTitleTable.m_lfFontTitle, 0, sizeof( LOGFONT ) );
		strcpy( m_WndTitleTable.m_lfFontTitle.lfFaceName, "Arial" );
		m_WndTitleTable.m_lfFontTitle.lfHeight = 10;
		m_WndTitleTable.m_lfFontTitle.lfWeight = 400;
		m_WndTitleTable.m_lfFontTitle.lfItalic = 0;
		m_WndTitleTable.m_lfFontTitle.lfUnderline = 0;
		m_WndTitleTable.m_lfFontTitle.lfStrikeOut = 0;
		m_WndTitleTable.m_lfFontTitle.lfCharSet = DEFAULT_CHARSET;
		m_WndTitleTable.m_lfFontTitle.lfPitchAndFamily = FF_SWISS;
		m_WndTitleTable.m_clTextColorTitle = 0;

		m_strClassNameCol.LoadString( IDS_CLASSNAME ); 
		m_strAllName.LoadString( IDS_TOTAL );


		m_clIncorrectColor = RGB( 255, 0, 0 );
		m_clTextColorHeader = m_clTextColor = 0;
		::ZeroMemory( &m_ptOffset, sizeof( m_ptOffset ) );
	}

	CTableWnd::~CTableWnd()
	{
		deinit();
	}

	void CTableWnd::deinit()
	{
		if( m_pColumnInfo )
			delete []m_pColumnInfo;
		m_pColumnInfo = 0;

		m_lParamCount = 0;
	}

	long CTableWnd::on_paint()
	{
		RECT rcPaint = { 0, 0, 0, 0 };
		::GetUpdateRect( handle(), &rcPaint, false );

		RECT rcPaint2 = { 0, 0, 0, 0 };
		::GetClientRect( handle(), &rcPaint2 );

		PAINTSTRUCT	ps = {0};
		HDC hdcPaint = ::BeginPaint( handle(), &ps );	

		RECT rc = {0};
		GetClientRect( m_hwnd, &rc );

		HBRUSH br = ::GetSysColorBrush(COLOR_BTNFACE);

		::FillRect( hdcPaint, &rc, br );

		DoDraw( hdcPaint, rcPaint2 );

		::EndPaint( handle(), &ps );

		return 1;
	}

	void CTableWnd::DoDraw( HDC hDC, RECT rcPaint, double fZoom, bool bPrint  )
	{
		//		fZoom =1./fZoom;

		int nImageWidth		= rcPaint.right  - rcPaint.left  + 1;
		int nImageHeight	= rcPaint.bottom - rcPaint.top  + 1;

		if( nImageWidth < 1 || nImageHeight < 1 )
			return;	

		if( bPrint && m_list_attached.count() == 0 )
			return;

		COLORREF clBackTextColor = 0;

		if( bPrint )
			clBackTextColor = ::GetSysColor( COLOR_WINDOW );

		COLORREF clBackColor = 0;

		if( bPrint )
			clBackColor = RGB( 255, 255, 255 );

		if( bPrint )
			::SetBkMode( hDC, TRANSPARENT );


		HBRUSH hBrush = ::CreateSolidBrush( clBackColor );

		double _fZoom = 1;
		IScrollZoomSitePtr sptrZ = m_sptrView;
		if( m_bPrintMode ){
			sptrZ->GetZoom( &_fZoom );
			//			_fZoom=1./_fZoom;
		}

		::SetMapMode( hDC, MM_TEXT );	

		{
			RECT rcPaint2 = { 0, 0, nImageWidth, nImageHeight };
			RECT rcClass = {0};
			RECT rcToral = {0};
			RECT rcTitle = m_rcTitle;

			//m_Grid.GetWindowRect( &rcClass );
			//m_gridTotal.get_window_rect( &rcToral );

			//ScreenToClient( m_hwnd, (LPPOINT)&rcClass );
			//ScreenToClient( m_hwnd, ((LPPOINT)&rcClass) + 1 );

			//ScreenToClient( m_hwnd, (LPPOINT)&rcToral );
			//ScreenToClient( m_hwnd, ((LPPOINT)&rcToral) + 1 );

			int xSb = 0;

			::SetBkColor( hDC, clBackTextColor );
			::SetTextColor( hDC, m_clTextColor );

			HBRUSH hBr = ::CreateSolidBrush( RGB(0,255,0) ) ;

			if( !m_WndTitleTable._strTitle.IsEmpty() && m_lParamCount + m_lCustomDataCount )
			{
				LOGFONT lf = m_WndTitleTable.m_lfFontTitle;
				if( !bPrint )
					lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps(hDC,LOGPIXELSY), 72) * _fZoom );
				else
				{
					HDC hdcDispl = ::CreateDC( _T( "DISPLAY" ), 0, 0, 0 );
					lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps(hdcDispl,LOGPIXELSY), 72) * _fZoom );
					::DeleteDC( hdcDispl );
				}

				HFONT hFontTitle = ::CreateFontIndirect( &lf );

				// [vanek] SBT:1204 - 29.11.2004
				rcTitle.top = long( rcTitle.top * fZoom );
				rcTitle.bottom = long( rcTitle.bottom  * fZoom );

				rcTitle.left = long( rcTitle.left * fZoom );
				rcTitle.right = long( rcTitle.right * fZoom );
				// [vanek] - 10.01.2005
				if( (rcTitle.right - rcTitle.left) < nImageWidth )
					rcTitle.right = rcTitle.left + nImageWidth;

				if( bPrint ){
					::OffsetRect( &rcTitle, -rcTitle.left, -rcTitle.top + 1 );
					::SetTextColor( hDC, m_WndTitleTable.m_clTextColorTitle );
					HFONT hFontOld = (HFONT)::SelectObject( hDC, hFontTitle );
					::DrawText( hDC, m_WndTitleTable._strTitle, m_WndTitleTable._strTitle.GetLength(), &rcTitle, DT_CENTER | DT_NOCLIP | DT_NOCLIP );
					::SelectObject( hDC, hFontOld );
				}
				::DeleteObject( hFontTitle );
			}

			HBRUSH hOldBr = (HBRUSH)::SelectObject( hDC, hBr );

			int w = m_rcMinRect.right - m_rcMinRect.left;
			int w2 = get_column_rect( m_Grid.GetColumnCount() - 1 ).right - get_column_rect( 0 ).left;

			LOGFONT lf = m_lfTableFont; 
			int nButtom = 0;
			long lLeftOffset = 0;
			RECT rcTotal = {0};


			if( !bPrint )
			{
				HDC hDC = ::GetDC( 0 );
				lf.lfHeight = long( -MulDiv( lf.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) * fZoom * _fZoom );
				::ReleaseDC( 0, hDC );
			}
			else
				lf.lfHeight = long( lf.lfHeight * fZoom * _fZoom );

			HFONT hFont = ::CreateFontIndirect( &lf );
			HFONT hOldFont = (HFONT)::SelectObject( hDC, hFont );

			::SelectObject( hDC, hOldBr );
			::SelectObject( hDC, hOldFont );

			::DeleteObject( hBr );
			::DeleteObject( hFont );

			//if( !bPrint )
			//{
			//	HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
			//	RECT rc = ScaleRect( m_rcClient, fZoom );
			//	HPEN hPen = ::CreatePen( PS_SOLID, 1, ::GetSysColor( COLOR_BTNSHADOW ) );
			//	HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen ) ;

			//	::Rectangle( hDC, rc.left, rc.top, rc.right, rc.bottom );
			//	::SelectObject( hDC, hOldPen ) ;
			//	::DeleteObject( hPen );
			//	::SelectObject( hDC, hBrush );
			//}
		}				  

		if( !bPrint )
		{
			HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
			RECT rcFull = ScaleRect( m_rcFull, fZoom );
			::Rectangle( hDC, rcFull.left, rcFull.top, rcFull.right - 1, rcFull.bottom - 1 );
			::SelectObject( hDC, hBrush );
		}

		//paint to paint DC
		::DeleteObject( hBrush );

		//prepare to delete objects
	}

	RECT CTableWnd::get_column_rect( int iIndex )
	{
		RECT rc = {0};

		if( iIndex < 0 )
		{
			//HWND hwnd_header = ListView_GetHeader( m_gridClass.handle() );
			//Header_GetItemRect( hwnd_header, iIndex + 1, &rc );

			//::ClientToScreen( hwnd_header, (LPPOINT)&rc );
			//::ClientToScreen( hwnd_header, (LPPOINT)&rc + 1 );
		}
		else
		{
			HWND hwnd_header = ListView_GetHeader( m_Grid);
			Header_GetItemRect( hwnd_header, iIndex, &rc );

			::ClientToScreen( hwnd_header, (LPPOINT)&rc );
			::ClientToScreen( hwnd_header, (LPPOINT)&rc + 1 );
		}

		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		return rc;
	}

	RECT CTableWnd::get_row_rect( int iIndex )
	{
		RECT rc = {0}, rc2 = {0};
		{
			HWND hwnd_header = ListView_GetHeader( m_Grid );
			Header_GetItemRect( hwnd_header, iIndex + 1, &rc );

			::ClientToScreen( hwnd_header, (LPPOINT)&rc );
			::ClientToScreen( hwnd_header, (LPPOINT)&rc + 1 );
			rc2 = rc;
		}

		if( iIndex != 0 )
		{
			RECT rcH = {0};
			HWND hwnd_header = ListView_GetHeader(m_Grid);
			Header_GetItemRect( hwnd_header, 1, &rcH );

			RECT rcFirst = {0};
			ListView_GetSubItemRect( m_Grid, 0, 1, LVIR_BOUNDS, &rcFirst );

			if( iIndex - 1 >= 0 && iIndex-1 < m_Grid.GetItemCount() )
				ListView_GetSubItemRect( m_Grid, iIndex - 1, 1, LVIR_BOUNDS, &rc );
			else
			{
				RECT rc2;	
				ListView_GetSubItemRect( m_Grid, m_Grid.GetItemCount() - 1, 1, LVIR_BOUNDS, &rc2 );
				//				ListView_GetSubItemRect( m_gridTotal.handle(), 0, 1, LVIR_BOUNDS, &rc );

				::OffsetRect( &rc, 0, rc2.bottom - rc.top );
			}
			::OffsetRect( &rc, 0, -rcFirst.top );
			::OffsetRect( &rc, 0, rcH.bottom );

			::ClientToScreen( m_Grid, (LPPOINT)&rc );
			::ClientToScreen( m_Grid, (LPPOINT)&rc + 1 );
		}

		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		::ScreenToClient( handle(), (LPPOINT)&rc2 );
		::ScreenToClient( handle(), (LPPOINT)&rc2 + 1 );

		::OffsetRect( &rc, 0, -rc2.top );

		::OffsetRect( &rc, 0, m_rcTitle.bottom - m_rcTitle.top );

		return rc;
	}

	const CRect& CTableWnd::get_min_rect()
	{
		return 	m_rcMinRect;
	}

	void CTableWnd::load_from_ndata(  INamedDataPtr sptrND )
	{
		deinit();

		m_strInvisibleItems = _get_invisibles( sptrND, 0 );
		if( sptrND == 0 )
			return;

		long lKeySz = ObjectSpace::_standart_param_count( sptrND ) + ObjectSpace::_user_param_count( sptrND );

		if( !lKeySz )
			return;

		/* Invisible items */
		long *plKey_arr = new long[lKeySz];
		if( !m_strInvisibleItems.IsEmpty() )
		{
			CString *strKey = 0;
			long lKCount = 0;

			split( m_strInvisibleItems, ",", &strKey, &lKCount );

			long *plKeys = 0;
			if( strKey && lKCount )
				plKeys = new long[lKCount];

			for( int nInd = 0; nInd < lKCount; nInd++ )
				plKeys[nInd] = atoi( strKey[nInd] );

			delete []strKey;
			strKey = 0;
			int nSkip = 0;
			long lLastKey = ObjectSpace::_standart_param_last( sptrND );
			for( int i = 0; i < lKeySz; i++ )
			{
				long lKey = PARAM_FIRST_PARAM + i;

				if( lKey > lLastKey )
					lKey = lKey - (lLastKey + 1) + PARAM_USERDEFINE_MIN_KEY;

				bool bOK = true;
				for( int nInd = 0; nInd < lKCount; nInd++ )
				{
					if( plKeys[nInd] == lKey )
					{
						bOK = false;
						nSkip++;
						break;
					}
				}

				if( bOK )
					plKey_arr[i - nSkip] = lKey;
			}

			lKeySz -= nSkip;

			if( plKeys )
				delete []plKeys;
			plKeys = 0;
		}
		else
		{
			long lLastKey = ObjectSpace::_standart_param_last( sptrND );
			for( int i = 0; i < lKeySz; i++ )
			{
				plKey_arr[i] = PARAM_FIRST_PARAM + i;

				if( plKey_arr[i] > lLastKey )
					plKey_arr[i] = plKey_arr[i] - (lLastKey + 1) + PARAM_USERDEFINE_MIN_KEY;
			}
		}

		/* Invisible items */
		{
			m_lCustomDataCount = 0;

			sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_TABLE_USERDATA ) );
			sptrND->GetEntriesCount( &m_lCustomDataCount );

			m_mapColumn.clear();
			m_mapColumnName.clear();
			m_mapColumnOrder.clear();

			for( long i = 0; i < m_lCustomDataCount; i++ )
			{
				sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_TABLE_USERDATA ) );
				_bstr_t bstrName;
				sptrND->GetEntryName( i, bstrName.GetAddress() );
				sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_TABLE_USERDATA ) + "\\" + bstrName );

				long lCount = 0;
				sptrND->GetEntriesCount( &lCount );

				if( lCount == 0 )
					continue;

				_variant_t varTitle = "";
				sptrND->GetValue( _bstr_t( ENTRY_NAME ), &varTitle );

				if( varTitle.vt == VT_BSTR )
					m_mapColumnName.set( CString( (char*)_bstr_t( varTitle ) ), i );
				else
					m_mapColumnName.set( CString( "" ), i );


				_variant_t varOrder = 0L;
				sptrND->GetValue( _bstr_t( ENTRY_ORDER ), &varOrder );

				if( varOrder.vt == VT_I4 )
					m_mapColumnOrder.set( (long)varOrder , i );
				else
					m_mapColumnOrder.set( 0, i );


				ROW_TO_VALUE *pMap = new ROW_TO_VALUE;
				for( long j = -1; j < lCount - 1; j++ )
				{
					_variant_t var;

					CString str;
					str.Format( "%s%ld", ENTRY_VALUE, j );

					sptrND->GetValue( _bstr_t( str ), &var );
					if( var.vt == VT_BSTR )
						pMap->set( CString( (char*)_bstr_t( var ) ), j );
					else
						pMap->set( CString( "" ), j );
				}

				m_mapColumn.set( pMap, i );

			}
		}

		if( lKeySz + m_lCustomDataCount )
			m_pColumnInfo = new XColumnInfo[lKeySz + m_lCustomDataCount + 1];

		if( lKeySz + m_lCustomDataCount )
		{
			CComVariant vClassColWidth;
			HRESULT hr=sptrND->GetValue(bstrCLASS_COL_WIDTH,&vClassColWidth);
			if(VT_EMPTY==vClassColWidth.vt)
			{
				char szColumn[15] = {0};
				sprintf( szColumn, "%s%d", COLUMN_COLOR_ID, 0 );
				m_pColumnInfo[0].nWidth = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_WIDTH, szColumn , m_pColumnInfo[0].nWidth );
			}
			else
			{
				m_pColumnInfo[0].nWidth = vClassColWidth.lVal;
			}
		}

		for( int i = 0; i < lKeySz + m_lCustomDataCount; i++ )
		{
			char szColumn[15] = {0};

			sprintf( szColumn, "%s%d", COLUMN_COLOR_ID, i + 1 );

			m_pColumnInfo[i + 1].lIndex = i;

			long lKey = 0;

			if( i < lKeySz ) 
				m_pColumnInfo[i + 1].lParamKey = lKey = plKey_arr[i];

			char szParamSect[200] = {0};

			if( i < lKeySz ) 
				sprintf( szParamSect, "%s\\%ld", SECT_STATUI_STAT_PARAM_ROOT, lKey );

			// Get color width from NamedData
			{
				_variant_t vColColor;
				HRESULT hr=sptrND->GetValue(bstrSTAT_PARAMS+_bstr_t(_variant_t(m_pColumnInfo[i + 1].lParamKey))+"\\Color", &vColColor);
				int n=0;
				if(VT_BSTR==vColColor.vt)
				{
					int r=0,g=0,b=0;
					n=swscanf(vColColor.bstrVal, L"(%d,%d,%d)",&r,&g,&b);
					m_pColumnInfo[i + 1].clColumnColor = RGB(r,g,b);
				}
				if(n<3)
					m_pColumnInfo[i + 1].clColumnColor = ::GetValueColor( sptrND, SECT_STATUI_STAT_TABLE_COLOR, szColumn , m_pColumnInfo[i + 1].clColumnColor );
			}

			IApplicationPtr sptrP = sptrND;
			if( sptrP == 0 )
				strcpy( m_strClassFile, (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CLASS_FILE, m_strClassFile ) );

			// Get column width from NamedData
			{
				_variant_t vColWidth;
				HRESULT hr=sptrND->GetValue(bstrSTAT_PARAMS+_bstr_t(_variant_t(m_pColumnInfo[i + 1].lParamKey))+L"\\Width", &vColWidth);
				if(VT_EMPTY==vColWidth.vt)
					m_pColumnInfo[i + 1].nWidth = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_WIDTH, szColumn , m_pColumnInfo[i + 1].nWidth );
				else
				{
					m_pColumnInfo[i + 1].nWidth = vColWidth.lVal;
				}
			}

			if( i < lKeySz ) 
			{
				strcpy( m_pColumnInfo[i + 1].szName, (char*)::GetValueString( sptrND, szParamSect, PARAM_NAME , 0 ) );
				strcpy( m_pColumnInfo[i + 1].szFormat, (char*)::GetValueString( sptrND, szParamSect, FORMAT_STRING , 0 ) );
				if(0==strlen( m_pColumnInfo[i + 1].szFormat))
					strcpy(m_pColumnInfo[i + 1].szFormat,"%g");
				m_pColumnInfo[i + 1].lOrder = _get_param_order_by_key( sptrND, lKey, 0 );
			}
			else
			{
				_bstr_t name;
				long lPos = m_mapColumnName.find( i - lKeySz );

				m_pColumnInfo[i + 1].lUsedID = i - lKeySz;

				if( lPos )
					strcpy( m_pColumnInfo[i + 1].szName, m_mapColumnName.get( lPos ) );
				else
					strcpy( m_pColumnInfo[i + 1].szName, "" );

				long lPos2 = m_mapColumnOrder.find( i - lKeySz );

				if( lPos2 )
					m_pColumnInfo[i + 1].lOrder = m_mapColumnOrder.get( lPos2 );
				else
					m_pColumnInfo[i + 1].lOrder = 0;

				m_pColumnInfo[i + 1].bUserData = true;

			}
		}

		m_lParamCount = lKeySz;
		delete []plKey_arr;

		m_clIncorrectColor = ::GetValueColor( sptrND, SECT_STATUI_STAT_TABLE, INCORRECT_COLOR , m_clIncorrectColor );

		strcpy( m_lfTableFont.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_NAME , m_lfTableFont.lfFaceName ) );
		m_lfTableFont.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_SIZE, m_lfTableFont.lfHeight );
		m_lfTableFont.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_BOLD, m_lfTableFont.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfTableFont.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_ITALIC, m_lfTableFont.lfItalic ) != 0;
		m_lfTableFont.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_UNDERLINE, m_lfTableFont.lfUnderline ) != 0;
		m_lfTableFont.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_STRIKEOUT, m_lfTableFont.lfStrikeOut) != 0;
		m_clTextColor = ::GetValueColor( sptrND, SECT_STATUI_STAT_TABLE_FONT, TEXT_COLOR , m_clTextColor );
		m_wHotBtn.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_TABLE_AREA , "" ) );
		m_wHotBtn.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_TABLE_AREA , 0 ) );

		strcpy( m_lfHeaderTableFont.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_NAME , m_lfHeaderTableFont.lfFaceName ) );
		m_lfHeaderTableFont.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_SIZE, m_lfHeaderTableFont.lfHeight );
		m_lfHeaderTableFont.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_BOLD, m_lfHeaderTableFont.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfHeaderTableFont.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_ITALIC, m_lfHeaderTableFont.lfItalic ) != 0;
		m_lfHeaderTableFont.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_UNDERLINE, m_lfHeaderTableFont.lfUnderline ) != 0;
		m_lfHeaderTableFont.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_STRIKEOUT, m_lfHeaderTableFont.lfStrikeOut) != 0;
		m_clTextColorHeader = ::GetValueColor( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, TEXT_COLOR , m_clTextColorHeader );


		m_WndTitleTable._strTitle = (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE, CHART_TITLE , m_WndTitleTable._strTitle );
		m_WndTitleTable._strTitle = _change_chars( m_WndTitleTable._strTitle, "\\n", "\r\n" );
		strcpy( m_WndTitleTable.m_lfFontTitle.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_NAME , m_WndTitleTable.m_lfFontTitle.lfFaceName ) );
		m_WndTitleTable.m_lfFontTitle.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_SIZE, m_WndTitleTable.m_lfFontTitle.lfHeight );
		m_WndTitleTable.m_lfFontTitle.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_BOLD, m_WndTitleTable.m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_WndTitleTable.m_lfFontTitle.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_ITALIC, m_WndTitleTable.m_lfFontTitle.lfItalic ) != 0;
		m_WndTitleTable.m_lfFontTitle.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_UNDERLINE, m_WndTitleTable.m_lfFontTitle.lfUnderline ) != 0;
		m_WndTitleTable.m_lfFontTitle.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_STRIKEOUT, m_WndTitleTable.m_lfFontTitle.lfStrikeOut) != 0;
		m_WndTitleTable.m_clTextColorTitle = ::GetValueColor( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, TEXT_COLOR , m_WndTitleTable.m_clTextColorTitle );

		m_strClassNameCol = (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE, CLASS_COL_TITLE , m_strClassNameCol ); 
		m_strAllName = (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE, ALL_WORD , m_strAllName );

		m_list_classes.clear();
		{
			CString str_data = (char*)::GetValueString( sptrND, SECT_STATUI_STAT_TABLE, VISIBLE_CLASSES, "" );

			CString *pstr = 0;
			long lcount = 0;
			split( str_data, ",", &pstr, &lcount );

			for( long i = 0; i < lcount; i++ )
				m_list_classes.add_tail( atoi( pstr[i] ) );

			if( pstr )
				delete []pstr;
		}


		m_bEnableTableArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_TABLE_STAT_AREA, m_bEnableTableArea ) != 0;
		m_nTblAlign = ::GetValueInt( sptrND, SECT_STATUI_STAT_TABLE, TABLE_ALIGN, m_nTblAlign );

		IApplicationPtr sptrP = sptrND;
		if( sptrP == 0 )
			strcpy( m_strClassFile, (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CLASS_FILE, m_strClassFile ) );
	}

	void CTableWnd::store_to_ndata( INamedDataPtr sptrND )
	{
		if( sptrND == 0 )
			return;

		if( !( m_lParamCount + m_lCustomDataCount ) )
			return;
		{
			char szColumn[15] = {0};
			sprintf( szColumn, "%s%d", COLUMN_COLOR_ID, 0 );

			HRESULT hr=sptrND->SetValue(bstrCLASS_COL_WIDTH, _variant_t(m_pColumnInfo[0].nWidth));
//			::SetValue( sptrND, SECT_STATUI_STAT_TABLE_WIDTH, szColumn , (long)m_pColumnInfo[0].nWidth );
		}

		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
			_bstr_t bstrFullKey=bstrSTAT_PARAMS+_bstr_t(_variant_t(m_pColumnInfo[i + 1].lParamKey));

			SetValueColor(sptrND,bstrFullKey,"Color",_variant_t(m_pColumnInfo[i + 1].clColumnColor));
			HRESULT hr=sptrND->SetValue(bstrFullKey+L"\\Width",_variant_t(m_pColumnInfo[i + 1].nWidth));
		}

		::SetValueColor( sptrND, SECT_STATUI_STAT_TABLE, INCORRECT_COLOR , m_clIncorrectColor );

		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_NAME , m_lfTableFont.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_SIZE, (long)m_lfTableFont.lfHeight );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_BOLD, (long)(m_lfTableFont.lfWeight > 400 ? 1 : 0) );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_ITALIC, (long)m_lfTableFont.lfItalic );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_UNDERLINE, (long)m_lfTableFont.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_FONT, FONT_STRIKEOUT, (long)m_lfTableFont.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_STAT_TABLE_FONT, TEXT_COLOR , m_clTextColor );

		if( m_list_classes.count() )
		{
			CString str_data;
			for( long lpos = m_list_classes.head(); lpos; lpos = m_list_classes.next( lpos ) )
			{
				long lclass = m_list_classes.get( lpos );

				CString str;
				str.Format( "%d", lclass );

				if( str_data.IsEmpty() )
					str_data = str;
				else
					str_data += "," + str;
			}

			::SetValue( sptrND, SECT_STATUI_STAT_TABLE, VISIBLE_CLASSES, str_data );
		}

		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_NAME , m_lfHeaderTableFont.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_SIZE, (long)m_lfHeaderTableFont.lfHeight );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_BOLD, (long)(m_lfHeaderTableFont.lfWeight > 400 ? 1 : 0) );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_ITALIC, (long)m_lfHeaderTableFont.lfItalic );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_UNDERLINE, (long)m_lfHeaderTableFont.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, FONT_STRIKEOUT, (long)m_lfHeaderTableFont.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_STAT_TABLE_HEADER_FONT, TEXT_COLOR , m_clTextColorHeader );

		::SetValue( sptrND, SECT_STATUI_STAT_TABLE, CHART_TITLE , _change_chars( m_WndTitleTable._strTitle, "\r\n", "\\n" ) );

		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_NAME , m_WndTitleTable.m_lfFontTitle.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_SIZE, (long)( m_WndTitleTable.m_lfFontTitle.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_BOLD, (long)( m_WndTitleTable.m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_ITALIC, (long)m_WndTitleTable.m_lfFontTitle.lfItalic );
		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_UNDERLINE, (long)m_WndTitleTable.m_lfFontTitle.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, FONT_STRIKEOUT, (long)m_WndTitleTable.m_lfFontTitle.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_TABLE_TITLE_FONT_ROOT, TEXT_COLOR , m_WndTitleTable.m_clTextColorTitle );


		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_TABLE_AREA , m_wHotBtn.GetAction() );
		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_TABLE_STAT_AREA, (long)m_bEnableTableArea );

		::SetValue( sptrND, SECT_STATUI_STAT_TABLE, CLASS_COL_TITLE , m_strClassNameCol ); 
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE, ALL_WORD , m_strAllName );
		::SetValue( sptrND, SECT_STATUI_STAT_TABLE, TABLE_ALIGN , m_nTblAlign );
	}	  

	bool CTableWnd::create( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass )
	{
		style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		if( !__super::create_ex( style, rect, pszTitle, parent, hmenu, pszClass
						, WS_EX_CLIENTEDGE ) )
			return false;

		DWORD dwStyle =	WS_CHILD | WS_VISIBLE | LVS_REPORT;

		DWORD dwStyleEx = 0;

		HWND hWnd=m_WndTitleTable.Create(m_hwnd,const_cast<RECT&>(rect));
		DWORD dw=SetClassLong(m_WndTitleTable,GCLP_HBRBACKGROUND,
			(LONG)::GetSysColorBrush(COLOR_BTNFACE));

		HWND m_hWndClient=m_Grid.Create(m_hwnd, const_cast<RECT&>(rect)
			, CListViewCtrl::GetWndClassName()
			, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL
			, 0 /*WS_EX_CLIENTEDGE*/);
		m_Grid.SetExtendedListViewStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

		m_wHotBtn.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
		return true;
	}

	void CTableWnd::_resize_child( RECT rect )
	{
		m_rcFull = rect;

		{
			RECT rc = {0};
			GetWindowRect( m_hwnd, &rc );
			m_ptOffset = *((POINT *)&rc);
			::ScreenToClient( GetParent( m_hwnd ), &m_ptOffset );
		}

		::InflateRect( &rect, -m_xGridOffset, -m_yGridOffset );

		m_rcClient = rect;
		::InflateRect( &m_rcClient, 2, 2 );

		int nW = 100;

		if( m_pColumnInfo && m_pColumnInfo[0].nWidth )
			nW = m_pColumnInfo[0].nWidth;

		int xClassW = nW, hy = 25
			, xSb = ::GetSystemMetrics( SM_CXVSCROLL )
			, ySb = ::GetSystemMetrics( SM_CYHSCROLL );

		int bBtnSz = 15;

		if( !m_bEnableTableArea )
			bBtnSz = 0;

		RECT _rcData1 = {0};

		if((HWND)m_Grid)
		{
			m_Grid.GetWindowRect(&_rcData1 );
			::ScreenToClient( handle(), (LPPOINT)&_rcData1 );
			::ScreenToClient( handle(), (LPPOINT)&_rcData1 + 1 );
		}

		RECT rcHotBtn = { rect.right - bBtnSz, rect.top, rect.right, rect.top + bBtnSz };

		::OffsetRect(&m_rcTitle,-m_rcTitle.left,-m_rcTitle.top);
		::OffsetRect(&m_rcTitle,rect.left,rect.top);
		m_rcTitle.right=rcHotBtn.left;

		rect.top=m_rcTitle.bottom;

		double _fZoom = 1.;
		if( m_bPrintMode )
		{
			IScrollZoomSitePtr sptrZ = m_sptrView;
			sptrZ->GetZoom( &_fZoom );
			rect = ScaleRect( rect , 1. / 2. / _fZoom );
		}

		RECT rcData  = { rect.left, rect.top + bBtnSz + 2, rect.right, rect.bottom - hy };

		int w = rcData.bottom - rcData.top;

		int w2 = 0;  //full height of ListView with all rows
		int nCount = m_Grid.GetItemCount();

		RECT rc = {0};
		m_Grid.GetItemRect(0, &rc, LVIR_BOUNDS );
		//		int ySbH=(rc.right-rc.left < rcData.right-rcData.left)?0:ySb;

		RECT rcHeader = {0};
		Header_GetItemRect( ListView_GetHeader(m_Grid), 0, &rcHeader );
		
		int iSB_HorHeight=0;
		{
				SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
				BOOL b=m_Grid.GetScrollInfo(SB_HORZ, &si);
				if((int)si.nPage<si.nMax-si.nMin)
					iSB_HorHeight += ySb;
		}


		w2 = rcHeader.bottom - rcHeader.top 
			+ (rc.bottom-rc.top)*nCount + iSB_HorHeight + 4;

		if( w2 < w )
		{
			rcData.bottom = rcData.top + w2 + 2;

			int _y = m_rcClient.bottom + rect.top;
			int __y = rcData.bottom + rcData.top;

			int _offset = 0;

			if( m_nTblAlign == 0 )
				_offset = 0;
			else if( m_nTblAlign == 1 )
				_offset = _y / 2 - __y / 2;
			else if( m_nTblAlign == 2 )
				_offset = _y - __y;

			::OffsetRect( &rcData, 0, _offset );
			//			::OffsetRect( &rcHotBtn, 0, _offset );
			::OffsetRect( &m_rcTitle, 0, _offset );

			if((HWND)m_Grid){
				//m_Grid.SendMessage(WM_HSCROLL,SB_LEFT,0);
				m_Grid.SendMessage(WM_VSCROLL,SB_TOP,0);
				m_Grid.MoveWindow(&rcData);
			}
			if( m_wHotBtn.handle() )
				m_wHotBtn.move_window( rcHotBtn );
		}
		else
		{
			int rowsSpace = rcData.bottom-rcData.top - 
				(rcHeader.bottom - rcHeader.top + iSB_HorHeight + 8);
			int nFullRows= ::IsRectEmpty(&rc)? 0 : rowsSpace/(rc.bottom-rc.top);
			int hGrid=rcHeader.bottom - rcHeader.top 
				+ (rc.bottom-rc.top)*nFullRows + iSB_HorHeight + 8;
			rcData.bottom=rcData.top+hGrid;
			if((HWND)m_Grid){
				//m_Grid.SendMessage(WM_HSCROLL,SB_LEFT,0);
				m_Grid.SendMessage(WM_VSCROLL,SB_TOP,0);
				m_Grid.MoveWindow( &rcData );
				//				::InvalidateRect(m_Grid.GetParent(),NULL,FALSE);
			}
			if( m_wHotBtn.handle() )
				m_wHotBtn.move_window(rcHotBtn);
		}
		{
			RECT rc=m_rcTitle;
			m_WndTitleTable.MoveWindow(&rc);
		}

		if( m_bEnableTableArea )
			::ShowWindow( m_wHotBtn.handle(), SW_SHOW );
		else
			::ShowWindow( m_wHotBtn.handle(), SW_HIDE );

		if( !(m_lParamCount + m_lCustomDataCount) )
		{
			::ShowWindow( m_Grid, SW_HIDE );
		}
		else
		{
			::ShowWindow( m_Grid, SW_SHOW );
			::ShowWindow( m_wHotBtn.handle(), SW_SHOW );
			::InvalidateRect(m_hwnd,0,FALSE);
		}
	}

	long CTableWnd::on_size( short cx, short cy, ulong fSizeType )
	{
		RECT rc;
		::GetClientRect( m_hwnd, &rc );
		if(IsRectEmpty(&rc))
			return 1;


		if((HWND)m_Grid)
			_resize_child( rc );
		return __super::on_size( cx, cy, fSizeType );
	}

	void CTableWnd::attach_data()
	{
		if( m_list_attached.count() == 0 )
			return;

		if( !(m_lParamCount + m_lCustomDataCount) )
			return;

		{
			m_Grid.DeleteAllItems();
			m_Grid.DeleteAllColumns();
			m_bSetModify = false;
		}

//		_ASSERTE(m_Grid.GetColumnCount()==0);

		bool bOK = false;

		IStatObjectDispPtr sptrStatObject = m_sptrAttached;

		long lClassSz = 0;
		sptrStatObject->GetClassCount( &lClassSz );

		_list_t<long> lst_classes;

		for( int i = 0; i < lClassSz; i++ )
		{
			long lClassNum = 0;
			sptrStatObject->GetClassValue( i, &lClassNum );

			if( m_list_classes.count() )
			{
				if( m_list_classes.find( lClassNum ) )
					lst_classes.add_tail( lClassNum );
			}
			else
				lst_classes.add_tail( lClassNum );
		}

		lClassSz = lst_classes.count();
		long *plclasses = new long[lClassSz];
		for( long lpos = lst_classes.head(), i = 0; lpos; lpos = lst_classes.next( lpos ), i++  )
			plclasses[i] = lst_classes.get( lpos );


		LOGFONT lfTable = m_lfTableFont;
		if( m_bPrintMode )
		{
			HDC hDC = ::GetDC( 0 );
			lfTable.lfHeight = long( -MulDiv( lfTable.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) );
			::ReleaseDC( 0, hDC );
		}
		else
		{
			HDC hDC = ::GetDC( handle() );
			lfTable.lfHeight = long( -MulDiv( lfTable.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) );
			::ReleaseDC( handle(), hDC );
		}

		LOGFONT lfTableHeader = m_lfHeaderTableFont;
		if( m_bPrintMode )
		{
			HDC hDC = ::GetDC( 0 );
			lfTableHeader.lfHeight = long( -MulDiv( lfTableHeader.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) );
			::ReleaseDC( 0, hDC );
		}
		else
		{
			HDC hDC = ::GetDC( handle() );
			lfTableHeader.lfHeight = long( -MulDiv( lfTableHeader.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) );
			::ReleaseDC( handle(), hDC );
		}

		m_Grid.Header_SetFont( &lfTableHeader );

		/* Order - begin */

		{
			long *plArrOrder = new long[m_lParamCount + m_lCustomDataCount];

			for( int i = 1; i < m_lParamCount + m_lCustomDataCount + 1; i++ )
				plArrOrder[i-1] = m_pColumnInfo[i].lOrder;

			sort_templ2( m_pColumnInfo + 1, plArrOrder, m_lParamCount + m_lCustomDataCount , 1 );

			delete []plArrOrder;
		}
		/* Order - end */	  

		// Fill left top cell
		{
			LVCOLUMN col={LVCF_FMT|LVCF_WIDTH|LVCF_ORDER|LVCF_SUBITEM|LVCF_TEXT, LVCFMT_CENTER};
			col.cx=m_pColumnInfo[0].nWidth;
			col.iOrder=0;
			col.iSubItem=0;
			col.pszText=(LPTSTR)(LPCTSTR)m_strClassNameCol;
			m_Grid.InsertColumn(0, &col);
		}

		int rowTotal=lClassSz;
		int nCols=m_lParamCount + m_lCustomDataCount+1;
		int nRows=lClassSz+1;

		// Fill left column
		m_strVals.resize(nRows, nCols);
		m_arrClassItem.resize(nRows);
		for( int i = 0; i < lClassSz; i++ )
		{
			CLVItem& info=m_arrClassItem[i];
			long lClassNum = 0;
			lClassNum = plclasses[i];

			m_strVals[i][0]=((ObjectSpace::CStatisticObject*)(IStatObjectDisp*)sptrStatObject)->get_class_name( lClassNum );
			//			info.SetBkColor(::GetSysColor(COLOR_BTNFACE));
			info.SetBkColor(RGB(234,230,224));
			info.SetForeColor(((ObjectSpace::CStatisticObject*)(IStatObjectDisp*)sptrStatObject)->get_class_color(lClassNum));

			LRESULT b=m_Grid.InsertItem(i,LPSTR_TEXTCALLBACK);
			DWORD dw;
			if(b<0)
				dw=GetLastError();
		}

		// fill params values
		int nFirstRow = -1;
		for( int i = 0; i < lClassSz; i++ )
		{
			long lClassNum = 0;
			//			sptrStatObject->GetClassValue( i, &lClassNum );
			lClassNum = plclasses[i];

			for( int j = 0; j < m_lParamCount + m_lCustomDataCount; j++ )
			{
				long lKey = 0;

				if( !m_pColumnInfo[j + 1].bUserData )
					lKey =  m_pColumnInfo[j + 1].lParamKey;

				double fValue = 0;
				HRESULT hr = S_OK;
				if( !m_pColumnInfo[j + 1].bUserData )
				{
					hr = sptrStatObject->GetValueInUnit2( lClassNum, lKey, &fValue );
					if( nFirstRow == -1 )
					{
						double fCount = 0;
						if( sptrStatObject->GetValueInUnit2( lClassNum, PARAM_COUNT, &fCount ) == S_OK )
						{
							if( fCount > 0 )
								nFirstRow = i;
						}
					}
				}

				CString szClassName;

				if( hr == S_OK )
				{
					if( !m_pColumnInfo[j + 1].bUserData )
						szClassName.Format(m_pColumnInfo[j + 1].szFormat, fValue );
					else
						szClassName=_get_user_value( m_pColumnInfo[j + 1].lUsedID, i);
				}
				else
				{

					if( hr == S_FALSE )
						szClassName.LoadString( App::handle(), IDS_NOVAL);
					else
						szClassName.LoadString( App::handle(), IDS_NOEXPR);
				}

				m_strVals[i][j+1]=szClassName;
				LVITEM lvi={LVIF_TEXT};
				lvi.pszText=LPSTR_TEXTCALLBACK;
				lvi.iItem=i;
				lvi.iSubItem=j+1;
				m_Grid.SetItem(&lvi);	   
			}

		}	

		if( nFirstRow == -1 )
			nFirstRow = 0;

		IStatObjectPtr sptrSt = sptrStatObject;

		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
			long lKey = 0;

			if( !m_pColumnInfo[i + 1].bUserData )
				lKey = m_pColumnInfo[i + 1].lParamKey;

			_bstr_t bstrUnit; 
			if( !m_pColumnInfo[i + 1].bUserData )
				sptrSt->GetParamInfo( lKey, 0, bstrUnit.GetAddress() );

			char name[MAX_PATH] = {0};

			strcpy( name, m_pColumnInfo[i + 1].szName );

			if( bstrUnit.length() > 0 )
			{
				strcat( name, ", " );
				strcat( name, (char*)bstrUnit );
			}

			LVCOLUMN col={LVCF_FMT|LVCF_WIDTH|LVCF_ORDER|LVCF_SUBITEM|LVCF_TEXT, LVCFMT_CENTER};
			col.cx=m_pColumnInfo[i+1].nWidth;
			col.iOrder=i+1;
			col.iSubItem=i+1;
			col.pszText=name;
			m_Grid.InsertColumn(i+1, &col);
		}

		// fill total row
		CLVItem& info=m_arrClassItem[rowTotal];
		m_strVals[rowTotal][0]=m_strAllName;
		//		info.SetBkColor(::GetSysColor(COLOR_BTNFACE));
		info.SetBkColor(RGB(234,230,224));
		info.SetForeColor(0);

		LRESULT b=m_Grid.InsertItem(rowTotal,LPSTR_TEXTCALLBACK);

		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
			long lKey = 0;

			if( !m_pColumnInfo[i + 1].bUserData )
				lKey = m_pColumnInfo[i + 1].lParamKey;

			double fValue = 0;
			HRESULT hr = S_OK;

			if( !m_pColumnInfo[i + 1].bUserData )
				hr = sptrStatObject->GetValueGlobalInUnit2( lKey, &fValue );

			CString szClassName;

			if( hr == S_OK )
			{
				if( !m_pColumnInfo[i + 1].bUserData )
					szClassName.Format(m_pColumnInfo[i + 1].szFormat, fValue );
				else
					szClassName=_get_user_value( m_pColumnInfo[i + 1].lUsedID, -1);
			}
			else
			{
				if( hr == S_FALSE )
					szClassName.LoadString( App::handle(), IDS_NOVAL);
				else
					szClassName.LoadString( App::handle(), IDS_NOEXPR);
			}
			m_strVals[rowTotal][i+1]=szClassName;
			LVITEM lvi={LVIF_TEXT};
			lvi.pszText=LPSTR_TEXTCALLBACK;
			lvi.iItem=rowTotal;
			lvi.iSubItem=i+1;
			m_Grid.SetItem(&lvi);	   
		}

		if( m_bPrintMode )
		{
			//			m_Grid.SetFocused( 0, 1 );
		}
		else
		{
			LVCOLUMN colEmpty={LVCF_ORDER|LVCF_SUBITEM};
			colEmpty.mask=LVCF_ORDER|LVCF_SUBITEM;
			colEmpty.iOrder=INT_MAX;
			colEmpty.iSubItem=INT_MAX;
			int colEmp=m_Grid.InsertColumn(INT_MAX, &colEmpty);
			LRESULT lr=m_Grid.GetColumn(colEmp,&colEmpty);

			_ASSERTE(nRows==m_Grid.GetItemCount());
//			_ASSERTE(nCols==m_Grid.GetColumnCount());
			SCROLLINFO si={sizeof(si)};
			si.fMask=SIF_POS|SIF_RANGE|SIF_PAGE;
			si.nPos=1;
			si.nMin=1;
			si.nPage=1;
			si.nMax=nCols-1;//-nAfterRight;
//			_ASSERTE(nCols==m_Grid.GetColumnCount());

			RECT rc = {0};
			::GetClientRect( m_hwnd, &rc );
			_resize_child( rc );

			// Disable scroll bars
			si.fMask=SIF_DISABLENOSCROLL;
			BOOL b=m_Grid.SetScrollInfo(SB_HORZ, &si, FALSE);
			b=m_Grid.SetScrollInfo(SB_VERT, &si, FALSE);

			//m_grid.SetFocused( lClassSz - 1, 1 );
			//m_gridClass.SetFocused( lClassSz - 1, 1 );

			//m_grid.SetFocused( nFirstRow, 1 );
			//m_gridClass.SetFocused( nFirstRow, 1 );
		}

		m_bSetModify = true;
		delete []plclasses;

	}

	void CTableWnd::calc_min_rect( bool bPrintMode )
	{
		if( !m_bMinSizeCalc )
		{
			RECT rc = { 0, 0, 200, 200 };
			RECT rcGrids = {0};
			if( bPrintMode )
			{
				RECT rcNeed = {0};
				int iCol = m_Grid.GetColumnCount();
				for( int i = 0; i < iCol; i++ )
				{
					RECT rcC = get_column_rect( i );
					rcNeed = MaxRect( rcNeed, rcC );
				}
				rcNeed.top = rcNeed.bottom = 0;

				int iRow = m_Grid.GetItemCount();

				for( int i = 0; i < iRow; i++ )
				{
					RECT rcR = get_row_rect( i );
					RECT rc2 = MaxRect( rcNeed, rcR );

					rcNeed.top = rc2.top;
					rcNeed.bottom = rc2.bottom;
				}
				rc = rcNeed;

				::OffsetRect( &rc, -rc.left, -rc.top );
				rcGrids.bottom = rc.bottom;
			}
			else
				rcGrids.bottom = 150;

			HDC hDCDisp = 0;
			if( bPrintMode )
				hDCDisp = ::CreateDC( "DISPLAY", 0, 0, 0 );
			else
				hDCDisp = ::GetDC( handle() );

			LOGFONT lf = m_WndTitleTable.m_lfFontTitle;
			lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hDCDisp, LOGPIXELSY), 72));

			HFONT hFontTitle = ::CreateFontIndirect( &lf );

			HFONT hFontOld = (HFONT)::SelectObject( hDCDisp, hFontTitle );

			::ZeroMemory( &m_rcTitle, sizeof( m_rcTitle ) );

			if( !m_WndTitleTable._strTitle.IsEmpty() )
			{
				::DrawText( hDCDisp, m_WndTitleTable._strTitle, m_WndTitleTable._strTitle.GetLength(), &m_rcTitle, DT_CALCRECT | DT_TOP | DT_LEFT );
				InflateRect( &m_rcTitle , 2, 0 );
				::OffsetRect( &m_rcTitle, -m_rcTitle.left, -m_rcTitle.top );
			}

			rcGrids.bottom += m_rcTitle.bottom;
			m_rcMinRect = MaxRect( rc, m_rcTitle );
			m_rcMinRect = MaxRect( m_rcMinRect, rcGrids );

			if( !bPrintMode )
				m_rcMinRect.right += 3 * ::GetSystemMetrics( SM_CXVSCROLL );

			if( bPrintMode )
			{
				//  				::InflateRect( &m_rcMinRect, 5, 3 );
				::OffsetRect( &m_rcMinRect, -m_rcMinRect.left, -m_rcMinRect.top );
			}

			::SelectObject( hDCDisp, hFontOld );
			::DeleteObject( hFontTitle );

			if( bPrintMode )
				::DeleteDC( hDCDisp );
			else
				::ReleaseDC( handle(), hDCDisp );

			m_bMinSizeCalc = true;
			{
				RECT rc;
				::GetClientRect( m_hwnd, &rc );
				_resize_child( rc );
			}
		}
	}

	CString CTableWnd::_get_user_value( long lCol, long lRow )
	{
		long lColPos = m_mapColumn.find( lCol );
		if( lColPos )
		{
			ROW_TO_VALUE *map = m_mapColumn.get( lColPos );
			if( map )
			{
				long lRowPos = map->find( lRow );
				if( lRowPos )
					return map->get( lRowPos );
			}
		}
		return "";
	}

	long CTableWnd::on_hscroll( unsigned scrollCode, unsigned pos, HWND hwndScroll )
	{
		LRESULT lr=m_Grid.OnHScroll((int)scrollCode, (short)pos, hwndScroll);
		return 1;
		//		return __super::on_vscroll( scrollCode, pos, hwndScroll );
	}

	long CTableWnd::on_vscroll( unsigned code, unsigned pos, HWND hwndScroll )
	{
		//::SendMessage( m_gridClass.handle(), WM_VSCROLL, (WPARAM)MAKELONG( code, pos ), (LPARAM)hwndScroll );
		//::SendMessage( m_grid.handle(), WM_VSCROLL, (WPARAM)MAKELONG( code, pos ), (LPARAM)hwndScroll );
		return __super::on_vscroll( code, pos, hwndScroll );
	}

	long CTableWnd::on_notify( uint idc, NMHDR *pnmhdr )
	{
		switch(pnmhdr->code)
		{
		default:
			return notify_reflect();
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO& nmv = *(NMLVDISPINFO*)pnmhdr;
				if(LVIF_TEXT& nmv.item.mask){
					SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
					BOOL b=m_Grid.GetScrollInfo(SB_VERT, &si);
					int nCols = m_strVals[1]-m_strVals[0];
					int row=nmv.item.iItem;
					//					__trace("Draw rows %d %d\n",si.nPos,row);
					if(row == si.nPos+si.nPage-1){
						row=m_strVals.size()/nCols-1;
					}
					int& col=nmv.item.iSubItem;
					if(col<nCols){
						nmv.item.pszText=(LPTSTR)(LPCTSTR)m_strVals[row][col];
						col=col;
					}
				}
			}
			return 0;
		case NM_CUSTOMDRAW:
			{
				NMCUSTOMDRAW& nmCustomDraw=*(NMCUSTOMDRAW*)pnmhdr;
				NMLVCUSTOMDRAW& nmLvCd=*(NMLVCUSTOMDRAW*)pnmhdr;
				switch(nmCustomDraw.dwDrawStage)
				{
				default:
					return notify_reflect();
				case CDDS_PREPAINT:
					return CDRF_NOTIFYSUBITEMDRAW;
				case CDDS_ITEMPREPAINT:
					return CDRF_NOTIFYSUBITEMDRAW;
				case CDDS_ITEMPREPAINT | CDDS_SUBITEM :
					{
						SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
						BOOL b=m_Grid.GetScrollInfo(SB_VERT, &si);
						int nCols = m_strVals[1]-m_strVals[0];
						int row=(int)nmCustomDraw.dwItemSpec;
						if(row == si.nPos+si.nPage-1){
							row=m_strVals.size()/nCols-1;
							//b=m_Grid.EnsureVisible(row,FALSE);
						}
						int& col=nmLvCd.iSubItem;
						if(col<nCols){
							if(0==col){
								nmLvCd.clrText = m_arrClassItem[row].GetForeColor();
								nmLvCd.clrTextBk= m_arrClassItem[row].GetBkColor();
							}else{
								nmLvCd.clrTextBk=RGB(255,255,255);
								if(m_strVals[row][col]==_T("---"))
									nmLvCd.clrText = RGB(255,0,0);
								else
									nmLvCd.clrText = 0;
							}
						}
					}
					return CDRF_NOTIFYSUBITEMDRAW;
				}
			}
		case HDN_ITEMCHANGEDA:
		case HDN_ITEMCHANGEDW:
			{
				if(!m_Grid.bBlockChanged){
					LPNMHEADER pNMHEADER=(LPNMHEADER)pnmhdr;
					if( (pNMHEADER->pitem->mask & HDI_WIDTH) == HDI_WIDTH )
					{
						int iColumn = pNMHEADER->iItem;
						int nCols=m_lParamCount + m_lCustomDataCount+1;
						_ASSERTE(0<=iColumn && iColumn<=nCols);
						if(0<=iColumn && iColumn<nCols){
							int& iWidth=pNMHEADER->pitem->cxy;
							m_pColumnInfo[iColumn].nWidth = iWidth;
							if(iWidth>0){
								HRESULT hr;
								if(INamedDataPtr pND=::GetAppUnknown()){
									if(0==iColumn){
										hr=pND->SetValue(bstrCLASS_COL_WIDTH, _variant_t(m_pColumnInfo[0].nWidth));
									}else{
										_bstr_t bstrFullKey=bstrSTAT_PARAMS+_bstr_t(_variant_t(m_pColumnInfo[iColumn].lParamKey));
										hr=pND->SetValue(bstrFullKey+L"\\Width",_variant_t(m_pColumnInfo[iColumn].nWidth));
									}
									_ASSERTE(!hr);
								}else _ASSERTE(!"Invalid Named Data");

								if(INamedDataPtr pND=m_sptrAttached){
									if(0==iColumn){
										hr=pND->SetValue(bstrCLASS_COL_WIDTH, _variant_t(m_pColumnInfo[0].nWidth));
									}else{
										_bstr_t bstrFullKey=bstrSTAT_PARAMS+_bstr_t(_variant_t(m_pColumnInfo[iColumn].lParamKey));
										hr=pND->SetValue(bstrFullKey+L"\\Width",_variant_t(m_pColumnInfo[iColumn].nWidth));
									}
									_ASSERTE(!hr);
								}else _ASSERTE(!"Invalid Named Data");
							}
						}
					}
				}
				return 0;
			}
		case HDN_ITEMCHANGINGA:
		case HDN_ITEMCHANGINGW:
			{
				if(!m_Grid.bBlockChanged){
					LPNMHEADER pNMHEADER=(LPNMHEADER)pnmhdr;
					if( (pNMHEADER->pitem->mask & HDI_WIDTH) == HDI_WIDTH )
					{
						int iColumn = pNMHEADER->iItem;
						int nCols=m_lParamCount + m_lCustomDataCount+1;
						_ASSERTE(0<=iColumn && iColumn<=nCols);
						if(0==iColumn && iColumn<nCols){
							int& iWidth=pNMHEADER->pitem->cxy;
							RECT rc;
							m_Grid.GetClientRect(&rc);
							if(iWidth>rc.right-rc.left-1)
								iWidth=rc.right-rc.left-1;
						}
					}
				}
				return 0;
			}
		case HDN_BEGINTRACKA:
		case HDN_BEGINTRACKW:
			{
				return 0;
			}
		case HDN_ENDTRACKA:
		case HDN_ENDTRACKW:
			{
				return notify_reflect();
			}
		case HDN_TRACKA:
		case HDN_TRACKW:
			{
				return 0;
			}
		}
	}

	long CTableWnd::handle_message( UINT m, WPARAM w, LPARAM l )
	{
		switch( m )
		{
		case WM_USER_MIN_SIZE:
			{
				CRect rect=get_min_rect();
				return MAKELRESULT(rect.Width(),rect.Height());
			}break;
		}
		return __super::handle_message( m, w, l );
	}

	int CTableWnd::GetTableWidth() const
	{
		int lVirtualWidth = 0;
		int iColCount = m_strVals.GetColsCount();
		for (int i = 0; i < iColCount; i++)
		{
			lVirtualWidth += m_pColumnInfo[i].nWidth;
		}

		return lVirtualWidth;
	}

	int CTableWnd::GetTableHeight() const
	{
		CRect rcHeader; Header_GetItemRect( m_Grid.GetHeader(), 0, &rcHeader );
		CRect rc; m_Grid.GetItemRect(0, &rc, LVIR_BOUNDS );
		int nRows=m_strVals.GetRowsCount();
		return rcHeader.Height() + rc.Height() * nRows;
	}

	bool CTableWnd::GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
	{
		bool bContinue = false;
		long nWidth = GetTableWidth();
		if(nWidth<=0)
		{
			nNewUserPosX += nReturnWidth;
			return bContinue;
		}

		m_bPrinting = true;

		// grid is longer that given max_width
		if (nWidth > nUserPosX + nMaxWidth)
		{
			// 2 cases: a) first call 
			// in this case given user_pos == 0 && given new_usr_pos == 0
			//			b) next call 
			// in this case user_pos undefined (previous new_usr_pos) && given new_usr_pos undefined
			// calc last from previous printed column
			LVHITTESTINFO hitTestInfo={{nUserPosX, 0}}; m_Grid.HitTest(&hitTestInfo);
			if (hitTestInfo.iSubItem < 0)
				hitTestInfo.iSubItem = 0;

			if (hitTestInfo.iItem < 0) // error
			{
				hitTestInfo.iItem = 0;
			}

			nNewUserPosX = nUserPosX;
			CRect cellRect;
			m_Grid.GetSubItemRect(hitTestInfo.iItem, hitTestInfo.iSubItem, LVIR_BOUNDS, &cellRect);
			nNewUserPosX = cellRect.left;

			int nLastCol = hitTestInfo.iSubItem;
			for (nLastCol = hitTestInfo.iSubItem; nLastCol < m_Grid.GetColumnCount(); nLastCol++)
			{
				int nW = m_Grid.GetColumnWidth(nLastCol);
				// check column is full visible
				if ((nNewUserPosX + nW) > (nUserPosX + nMaxWidth))
					break;
				// add column width to return_width 
				nNewUserPosX += nW;
			}
			if (hitTestInfo.iSubItem >= nLastCol) // too long column
			{
				nReturnWidth = nMaxWidth;
				nNewUserPosX = nNewUserPosX + nReturnWidth + 1;
			}
			else
			{
				nReturnWidth = nNewUserPosX - nUserPosX;
				nNewUserPosX += 1;
			}
			bContinue = true;
		}
		else// grid is no shorter that given max_width
		{
			// calc newUserPosX & return width
			// 2 cases: a) whole grid is shorter that given max_width
			// in this case given user_pos == 0 && given new_usr_pos undefined
			//			b) last part of grid is shorter than given max_width
			// in this case user_pos undefined (0 || previous new_usr_pos) && given new_usr_pos undefined

			if (nUserPosX == 0)// for case a:
			{
				nReturnWidth = nWidth - nUserPosX;
				nNewUserPosX = nWidth + 1;
				m_bPrinting = false;
				return false;
			}

			// else for case b:
			// get first cell
			LVHITTESTINFO hitTestInfo={{nNewUserPosX, 0}}; m_Grid.HitTest(&hitTestInfo);
			if (hitTestInfo.iSubItem < 0)
				hitTestInfo.iSubItem = 0;

			if (hitTestInfo.iItem < 0) // error
			{
				hitTestInfo.iItem = 0;
			}

			nNewUserPosX = nUserPosX;
			CRect cellRect;
			m_Grid.GetSubItemRect(hitTestInfo.iItem, hitTestInfo.iSubItem, LVIR_BOUNDS, &cellRect);
			nNewUserPosX = cellRect.left;

			int nLastCol = hitTestInfo.iSubItem;
			for (nLastCol = hitTestInfo.iSubItem; nLastCol < m_Grid.GetColumnCount(); nLastCol++)
				nNewUserPosX += m_Grid.GetColumnWidth(nLastCol);

			nReturnWidth = nNewUserPosX - nUserPosX;
			nNewUserPosX += 1;
			bContinue = false;
		}
		m_bPrinting = false;
		return bContinue;
	}

	bool CTableWnd::GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
	{
		bool bContinue = false;
		int nHeight = GetTableHeight();
		if(nHeight<=0)
		{
			nNewUserPosY += nReturnHeight;
			return bContinue;
		}

		m_bPrinting = true;
		// grid is longer that given max_height
		if (nHeight > nUserPosY + nMaxHeight)
		{
			// calc last printed column
			LVHITTESTINFO hitTestInfo={{0, nUserPosY}}; m_Grid.HitTest(&hitTestInfo);
			if (hitTestInfo.iSubItem < 0)

				if (hitTestInfo.iSubItem < 0)
					hitTestInfo.iSubItem = 0;

			if (hitTestInfo.iItem < 0) 
			{
				hitTestInfo.iItem = 0;
			}

			nNewUserPosY = nUserPosY;
			CRect cellRect;
			m_Grid.GetSubItemRect(hitTestInfo.iItem, hitTestInfo.iSubItem, LVIR_BOUNDS, &cellRect);
			nNewUserPosY = cellRect.top;

			int nLastRow = hitTestInfo.iItem;
			for (nLastRow = hitTestInfo.iItem; nLastRow < m_Grid.GetItemCount(); nLastRow++)
			{
				CRect rcRow;
				m_Grid.GetItemRect(hitTestInfo.iItem, &rcRow, LVIR_BOUNDS);
				int nH = rcRow.Height();
				if ((nNewUserPosY + nH) > (nUserPosY + nMaxHeight))
					break;
				nNewUserPosY += nH;
			}
			nReturnHeight = nNewUserPosY - nUserPosY;

			if (hitTestInfo.iItem >= nLastRow) // too long row
			{
				nReturnHeight = nMaxHeight;
				nNewUserPosY = nUserPosY + nReturnHeight + 1;
			}
			else
			{
				nReturnHeight = nNewUserPosY - nUserPosY;
				nNewUserPosY += 1;
			}

			bContinue = true;
		}
		else// grid is not higher than given max_height
		{
			// calc newUserPosY & return рушпре
			// 2 cases: a) whole grid is shorter that given max_height
			// in this case given user_pos == 0 && given new_usr_pos undefined
			//			b) last part of grid is shorter than given max_height
			// in this case user_pos undefined (0 || previous new_usr_pos) && given new_usr_pos undefined

			if (nUserPosY == 0)// for case a:
			{
				nReturnHeight = nHeight - nUserPosY;
				nNewUserPosY = nHeight + 1;
				m_bPrinting = false;
				return false;

			}
			// else for case b:
			// get first cell
			LVHITTESTINFO hitTestInfo={{0, nNewUserPosY}}; m_Grid.HitTest(&hitTestInfo);
			if (hitTestInfo.iSubItem < 0)
				if (hitTestInfo.iSubItem < 0)
					hitTestInfo.iSubItem = 0;

			if (hitTestInfo.iItem < 0) // error
			{
				hitTestInfo.iItem = 0;
			}

			nNewUserPosY = nUserPosY;
			CRect cellRect;
			m_Grid.GetSubItemRect(hitTestInfo.iItem, hitTestInfo.iSubItem, LVIR_BOUNDS, &cellRect);
			nNewUserPosY = cellRect.top;

			int nLastRow = hitTestInfo.iItem;
			//			for (nLastRow = hitTestInfo.iItem; nLastRow < GetRowCount(); nLastRow++)
			//				nNewUserPosY += GetRowHeight(nLastRow);

			nReturnHeight = nNewUserPosY - nUserPosY;
			nNewUserPosY += 1;
			bContinue = false;

		}

		m_bPrinting = false;
		return bContinue;
	}

	int CTableWnd::Print(CDCHandle& dc, CRect rectTarget, CRect RectDraw)
	{
		if (!dc || !m_pColumnInfo)
			return 0;

		m_bPrinting = true;

		int nSave = dc.SaveDC();

		dc.SetMapMode(MM_ANISOTROPIC);
		dc.SetWindowOrg(RectDraw.left,0);
		dc.SetWindowExt(RectDraw.Width()+1, RectDraw.Height()+2);
		dc.SetViewportExt(rectTarget.Size());
		dc.SetViewportOrg(rectTarget.TopLeft());

		CRgn ClipRegion;
		if (ClipRegion.CreateRectRgnIndirect(rectTarget))
			dc.SelectClipRgn(ClipRegion);

		ClipRegion.DeleteObject();
		///////////////////////
		{
			if( m_bPrinting && m_list_attached.count() == 0 )
				return 0;

			COLORREF clBackTextColor = 0;

			if( m_bPrinting )
				clBackTextColor = ::GetSysColor( COLOR_WINDOW );

			COLORREF clBackColor = 0;

			clBackColor = RGB( 255, 255, 255 );

			::SetBkMode( dc, TRANSPARENT );


			HBRUSH hBrush = ::CreateSolidBrush( clBackColor );

			double _fZoom = 1;
			IScrollZoomSitePtr sptrZ = m_sptrView;
			if( m_bPrintMode ){
				sptrZ->GetZoom( &_fZoom );
			}

			{
				RECT rcClass = {0};

				HBRUSH hBr = ::CreateSolidBrush( RGB(0,255,0) ) ;

				HBRUSH hOldBr = (HBRUSH)::SelectObject( dc, hBr );

				int w = m_rcMinRect.right - m_rcMinRect.left;
				int w2 = get_column_rect( m_Grid.GetColumnCount() - 1 ).right - get_column_rect( 0 ).left;

				LOGFONT lf = m_lfTableFont; 
				int nButtom = 0;
				long lLeftOffset = 0;
				RECT rcTotal = {0};
				if( m_bPrinting && m_lParamCount + m_lCustomDataCount ) 
				{
					int nMode = ::SetBkMode( dc, TRANSPARENT );
					{
						RECT rcCli = {0};

						nButtom = rcCli.top - nButtom;

						long lCol = m_Grid.GetColumnCount();
						HWND hwnd_header=m_Grid.GetHeader();
						RECT rcHeader0;
						Header_GetItemRect( hwnd_header, 0, &rcHeader0);
						long lHeaderHeight = rcHeader0.bottom-rcHeader0.top;
						const int nRows=m_Grid.GetItemCount();
						int iHeightStep;
						{
							CRect rcItem0;
							BOOL b=m_Grid.GetItemRect(0, &rcItem0, LVIR_BOUNDS);
							OffsetRect(&rcItem0,0,-rcItem0.top);
							iHeightStep=rcItem0.bottom-rcItem0.top;
						}
						int  x=0;
						for( int i = 0; i < lCol; i++ )
						{
							RECT rcHdrItem = {0};
							HDITEM hdi={HDI_TEXT|HDI_WIDTH|HDI_ORDER};
							hdi.cchTextMax=MAX_PATH*sizeof(TCHAR);
							hdi.pszText=(TCHAR*)_alloca(hdi.cchTextMax);
							BOOL b=Header_GetItem(hwnd_header,i,&hdi);

							Header_GetItemRect( hwnd_header, i, &rcHdrItem );
							rcHdrItem.right=rcHdrItem.left+hdi.cxy;

							// Header
							{
								::SetTextColor( dc, m_pColumnInfo[i].clColumnColor);

								LOGFONT lfHeaderTableFont = m_lfHeaderTableFont;
								HFONT hFnt = ::CreateFontIndirect( &lfHeaderTableFont );
								HFONT hOldFont = (HFONT)::SelectObject( dc, hFnt );

								::DrawText( dc, hdi.pszText, -1, &rcHdrItem , 
									DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
								::SelectObject( dc, hOldFont );
								::DeleteObject( hFnt );
								::InflateRect( &rcHdrItem, +1, +1 );
							}

							// class rows
							for( int j = 0; j < nRows; j++ )
							{
								RECT rcCell = {rcHdrItem.left,rcHdrItem.bottom
									,rcHdrItem.right, rcHdrItem.bottom+iHeightStep };
								OffsetRect(&rcCell,0,j*iHeightStep);

								::SetTextColor( dc, m_pColumnInfo[i].clColumnColor);

								LOGFONT lfTbl=m_lfTableFont;
								HFONT hFnt = ::CreateFontIndirect(&lfTbl);
								HFONT hOldFont = (HFONT)::SelectObject( dc, hFnt );
								::InflateRect( &rcCell,-1,-1);
								::DrawText( dc, m_strVals[j][i], -1, &rcCell , DT_SINGLELINE | DT_VCENTER | DT_CENTER );
								::SelectObject( dc, hOldFont );
								::DeleteObject( hFnt );
							}
							x += m_pColumnInfo[i].nWidth;
							dc.MoveTo(x-1,RectDraw.top);
							dc.LineTo(x-1,RectDraw.bottom);
						}
						dc.MoveTo(RectDraw.left,RectDraw.top);
						dc.LineTo(RectDraw.left,RectDraw.bottom);

						int y;
						for (int j=0;j<2;++j){
							y = RectDraw.top + j*(lHeaderHeight);
							dc.MoveTo(RectDraw.left,y);
							dc.LineTo(RectDraw.right,y);
						}
						for( int j = 0; j < nRows; j++ )
						{
							y += iHeightStep;
							dc.MoveTo(RectDraw.left,y);
							dc.LineTo(RectDraw.right,y);
						}
					}
					::SetBkMode( dc, nMode );
				}

				::SelectObject( dc, hOldBr );

				::DeleteObject( hBr );
			}				  

			//paint to paint DC
			::DeleteObject( hBrush );
		}
		return 0;
	}
}
