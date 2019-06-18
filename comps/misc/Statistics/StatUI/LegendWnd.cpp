#include "StdAfx.h"
#include "statisticobject.h"
#include "StatUI.h"
#include "statobjectview.h"
#include "legendwnd.h"

#define NCOLUMN_OFFSET 10
#define NCOLUMN_COLOR_AREA_HEIGHT 15

namespace ViewSpace 
{
	CLegendWnd::CLegendWnd(void)
		: m_bPrinting(false)
	{
		m_cur_hand = ::LoadCursor( 0, IDC_HAND );
		m_cur_arrow = ::LoadCursor( 0, IDC_ARROW );

		m_bPrintMode = m_nRealCenter = false;
		m_sum_value_width = m_nDescrWidth = 0;
		m_bEnableLegendArea = 1;
		m_lParamCount = 0;
		m_lCustomDataCount = 0;
		m_bMinSizeCalc = 0;

		m_lhot_data_height = NCOLUMN_COLOR_AREA_HEIGHT;

		m_lpParams = 0;
		m_nLineHeight = 20;

		::ZeroMemory( &m_rcMinRect, sizeof( RECT ) );
	
		strcpy( m_lfFont.lfFaceName, "Arial" );
		m_lfFont.lfHeight = 10;
		m_lfFont.lfWeight = 400;
		m_lfFont.lfItalic = 0;
		m_lfFont.lfUnderline = 0;
		m_lfFont.lfStrikeOut = 0;
		m_lfFont.lfCharSet = DEFAULT_CHARSET;
		m_lfFont.lfPitchAndFamily = FF_SWISS;

		m_lfFont.m_clrText = 0;
		m_clIncorrectColor = RGB( 255, 0, 0 );
		::ZeroMemory( &m_ptOffset, sizeof( m_ptOffset ) );
		::ZeroMemory( &m_strClassFile, sizeof( m_strClassFile ) );

		::ZeroMemory( &m_rcTitle, sizeof( m_rcTitle ) );
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
		m_lfFontTitle.m_clrText = 0;

		m_strCompareTitle.LoadString( IDS_LEGEND_TITLE_COMPARE );
	}

	CLegendWnd::~CLegendWnd(void)
	{
		if( m_cur_hand )
			::DeleteObject( m_cur_hand );
		m_cur_hand = 0;

		if( m_cur_arrow )
			::DeleteObject( m_cur_arrow );
		m_cur_arrow = 0;

		deinit();
	}

	void CLegendWnd::deinit()
	{
		if( m_lpParams )
			delete []m_lpParams;
		m_lpParams = 0;
	}


	long CLegendWnd::on_paint()
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
	
	void CLegendWnd::DoDraw( HDC hDC, RECT rcPaint,double fZoom, bool bPrint )
	{
		int nImageWidth		= rcPaint.right  - rcPaint.left  + 1;
		int nImageHeight	= rcPaint.bottom - rcPaint.top  + 1;

		if( nImageWidth < 1 || nImageHeight < 1 )
			return;	

		if( bPrint && m_list_attached.count() == 0 )
			return;

		COLORREF clBackColor = 0;
		if( !bPrint )
			clBackColor = ::GetSysColor( COLOR_BTNFACE );
		else
			clBackColor = RGB( 255, 255, 255 );
		SolidBrush brushBack(Color(GetRValue(clBackColor),GetGValue(clBackColor),GetBValue(clBackColor)));
	
		double _fZoom = 1;

		HDC hdcDispl = ::CreateDC( _T( "DISPLAY" ), 0, 0, 0 );


		Gdiplus::Graphics g(hDC); g.SetPageUnit(UnitPixel);
		COLORREF clrBtnShadow=::GetSysColor( COLOR_BTNSHADOW );
		Pen penColorBtnShadow(Color(GetRValue(clrBtnShadow),GetGValue(clrBtnShadow),GetBValue(clrBtnShadow)));

		{
			Rect rcPaint2 ( 0, 0, nImageWidth, nImageHeight );

			if( !bPrint ){
				g.FillRectangle(&brushBack,rcPaint2);
			}

			::SetBkColor( hDC, clBackColor );
			{
				if( m_bPrintMode ){
	 				m_lfFont.SetHeight(long(-MulDiv( m_lfFont.lfHeight, GetDeviceCaps( hdcDispl, LOGPIXELSY), 72) * fZoom * _fZoom ));
				}else{
	 				m_lfFont.SetHeight(long(-MulDiv( m_lfFont.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) * fZoom * _fZoom ));
				}
			}

  		::SetBkMode( hDC, TRANSPARENT );

			int nDelta = 0;
   		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
			{
   			RECT rc1 = ScaleRect(m_lpParams[i].m_rectParamName, fZoom);

				RECT _rcCalc = {0};
				CStringW wstr(m_lpParams[i].m_szParamName);
 				m_lfFont.MeasureString( g, wstr, &_rcCalc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) | DT_CALCRECT ); 

				m_lfFont.DrawString( g, wstr, &rc1, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) ); 


				RECT rc2 = {0};

				for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
				{
					rc2 = m_lpParams[i].m_pparam_values[lid].m_rectParamValue;

					::OffsetRect( &rc2, m_lpParams[i].m_pparam_values[lid].lCommaPart - m_lst_max_comma_width[lid], 0 );

					rc2 = ScaleRect( rc2, fZoom );

					rc2.top = long( rc2.top * _fZoom );
 					rc2.bottom = long( rc2.bottom * _fZoom );

 					rc2.right = long( rc2.left * _fZoom + ( rc2.right - rc2.left ) * _fZoom );

  				if( m_lpParams[i].m_pparam_values[lid].m_bIncorrect )
						m_lfFont.DrawString( g, CStringW(m_lpParams[i].m_pparam_values[lid].m_szParamValue), &rc2, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ), m_clIncorrectColor ); 
					else
						m_lfFont.DrawString( g, CStringW(m_lpParams[i].m_pparam_values[lid].m_szParamValue), &rc2, DT_RIGHT | DT_VCENTER | DT_SINGLELINE |( bPrint ? DT_NOCLIP : 0 ) ); 

					if( i == 0 )
					{
						{
							{
								COLORREF cl_data = m_lst_color_hot_data[lid];
								RECT rc_data = ScaleRect(m_lst_rect_hot_data[lid], fZoom);
 								CString str_data = m_lst_text_hot_data[lid];

								long dx = rc_data.right - rc_data.left;
								
								rc_data.right = rc2.right; 
								rc_data.left = (long)(rc_data.right - dx*0.8);

 								rc_data.top			= long( rc_data.top * _fZoom );
								rc_data.bottom	= long( rc_data.bottom * _fZoom );

								m_lfFont.DrawString( g, CStringW(str_data),  &rc_data, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS );
							}
						}
					}
				}

				RECT _rc = rcPaint;
				_rc.left += 5;
				_rc.right -= 5;

				_rc = ScaleRect( _rc, fZoom );
				_rc.bottom = __max( rc2.bottom, rc1.bottom );

				{
					g.DrawLine(&penColorBtnShadow, _rc.left, _rc.bottom, _rc.right, _rc.bottom );
				}
			}

			// [vanek] SBT:1260 - 20.12.2004
			CString strTitle = 1 < m_list_attached.count() ? m_strCompareTitle : m_strTitle;
 			if( !strTitle.IsEmpty() )
			{
	 			::SetBkMode( hDC, TRANSPARENT );

				if( m_bPrintMode )
 					m_lfFontTitle.SetHeight(int(-MulDiv( m_lfFontTitle.lfHeight, GetDeviceCaps( hdcDispl, LOGPIXELSY), 72) * fZoom * _fZoom));
				else
 					m_lfFontTitle.SetHeight(int(-MulDiv( m_lfFontTitle.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) * fZoom * _fZoom ));

				RECT rcTitle=m_rcTitle;
				::OffsetRect( &rcTitle, -rcTitle.left + 5, -rcTitle.top + 5);
				rcTitle = ScaleRect( rcTitle, fZoom * _fZoom );
				::OffsetRect( &rcTitle, int( -nDelta * _fZoom ), 0 );
				rcTitle.right=rcPaint.right;
				m_lfFontTitle.DrawString( g, CStringW(strTitle), &rcTitle, DT_CENTER | DT_VCENTER | ( bPrint ? DT_NOCLIP : 0 ) );
			}

			//if( !bPrint )
			//{
			//	//HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
			//	RECT rc = ScaleRect( m_rcClient, fZoom );
			//	g.DrawRectangle(&penColorBtnShadow, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
			//	//::SelectObject( hDC, hBrush );
			//}
		}
		if( !bPrint )
		{
			//HBRUSH hBrush = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
			RECT rcFull = ScaleRect( m_rcFull, fZoom );
			g.DrawRectangle(&penColorBtnShadow, rcFull.left, rcFull.top, rcFull.right-rcFull.left - 1, rcFull.bottom-rcFull.top - 1 );
			//::SelectObject( hDC, hBrush );
		}
		
		::DeleteDC( hdcDispl );

	}

	const CRect& CLegendWnd::get_min_rect()
	{
		return m_rcMinRect;
	}

	void CLegendWnd::load_from_ndata(  INamedDataPtr sptrND )
	{
		deinit();



		m_strInvisibleItems = _get_invisibles( sptrND, 1 );
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
		

		m_lParamCount = lKeySz;


		{
			m_lCustomDataCount = 0;

			sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_LEGEND_USERDATA ) );
			sptrND->GetEntriesCount( &m_lCustomDataCount );
			m_lCustomDataCount /= 3;
		}
	
		if( !(m_lParamCount + m_lCustomDataCount ) )
			m_lpParams=0;
		else
			m_lpParams = new XParamItem[ m_lParamCount + m_lCustomDataCount ];

		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
			m_lpParams[i].m_lparam_values_count = m_list_attached.count();
			if( m_lpParams[i].m_pparam_values )
				delete m_lpParams[i].m_pparam_values;
			m_lpParams[i].m_pparam_values = new XParamItem::XParamValue[m_lpParams[i].m_lparam_values_count];
		}

		for( int i = 0; i < m_lParamCount; i++ )
		{
			long lKey = plKey_arr[i];

			char szParamSect[200];

			sprintf( szParamSect, "%s\\%ld", SECT_STATUI_STAT_PARAM_ROOT, lKey );


			strcpy( m_lpParams[i].m_szParamName, (char*)::GetValueString( sptrND, szParamSect, PARAM_NAME , 0 ) );
			strcpy( m_lpParams[i].m_szParamFormat, (char*)::GetValueString( sptrND, szParamSect, FORMAT_STRING , "%lg" ) );
			if(0==strlen(m_lpParams[i].m_szParamFormat))
				strcpy( m_lpParams[i].m_szParamFormat,"%lg");
			m_lpParams[i].lKey = lKey;
		}


		for( long lid=0, lpos_lst = m_list_attached.head(); 
			lid<m_list_attached.count(); 
			++lid, lpos_lst=m_list_attached.next(lpos_lst))
		{
			m_lst_color_hot_data[lid]=( ::GetValueColor( m_list_attached.get( lpos_lst ), SECT_STATUI_STAT_LEGEND, SAMPLING_COLOR, RGB( 255, 255, 0 ) ));
			m_lst_text_hot_data[lid]=( (char*)::GetValueString( m_list_attached.get( lpos_lst ), SECT_STATUI_STAT_LEGEND, SAMPLING_NAME, "" ));
		}

		if( m_lCustomDataCount > 0 )
		{
			for( int i = m_lParamCount; i < m_lParamCount + m_lCustomDataCount; i++ )
			{
				CString strName;
				CString strValue;
				CString strOrder;

				m_lpParams[i].lUserID = i - m_lParamCount;
				
				strName.Format( "%s%d",  ENTRY_NAME,  m_lpParams[i].lUserID );
				strValue.Format( "%s%d", ENTRY_VALUE, m_lpParams[i].lUserID );
				strOrder.Format( "%s%d", ENTRY_ORDER, m_lpParams[i].lUserID );

				strcpy( m_lpParams[i].m_szParamName, (char*)::GetValueString( sptrND, SECT_STATUI_STAT_LEGEND_USERDATA, strName, 0 ) );

				for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
				{
					strcpy( m_lpParams[i].m_pparam_values[lid].m_szParamValue, (char*)::GetValueString( m_list_attached.get( lpos_lst ), SECT_STATUI_STAT_LEGEND_USERDATA, strValue, 0 ) );
					m_lpParams[i].m_pparam_values[lid].m_bIncorrect = false;
				}

				m_lpParams[i].lOrder = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_USERDATA, strOrder, m_lpParams[i].lOrder );
				m_lpParams[i].bUserValue = true;
			}
		}

		/* Order - begin */
		{
			long *plArrOrder = new long[m_lParamCount + m_lCustomDataCount];

			for( int i = 0; i < lKeySz; i++ )
				plArrOrder[i] = _get_param_order_by_key( sptrND, plKey_arr[i], 1 );
			for( int i = m_lParamCount; i < m_lParamCount + m_lCustomDataCount; i++ )
				plArrOrder[i] = m_lpParams[i].lOrder;

			sort_templ2( m_lpParams, plArrOrder, m_lParamCount + m_lCustomDataCount , 1 );

			delete []plArrOrder;
		}
		/* Order - end */


		delete []plKey_arr;

		strcpy( m_lfFont.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_NAME , m_lfFont.lfFaceName ) );
		m_lfFont.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_SIZE, m_lfFont.lfHeight );
		m_lfFont.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_BOLD, m_lfFont.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFont.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_ITALIC, m_lfFont.lfItalic ) != 0;
		m_lfFont.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_UNDERLINE, m_lfFont.lfUnderline ) != 0;
		m_lfFont.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_STRIKEOUT, m_lfFont.lfStrikeOut) != 0;

		m_lfFont.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_STAT_LEGEND_FONT, TEXT_COLOR , m_lfFont.m_clrText );
		m_clIncorrectColor = ::GetValueColor( sptrND, SECT_STATUI_STAT_LEGEND , INCORRECT_COLOR , m_clIncorrectColor );

		m_bEnableLegendArea = ::GetValueInt( sptrND, SECT_STATUI_ROOT, ENABLE_STAT_PARAM_AREA, m_bEnableLegendArea ) != 0;

		IApplicationPtr sptrP = sptrND;
		if( sptrP == 0 )
			strcpy( m_strClassFile, (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CLASS_FILE, m_strClassFile ) );

		m_wHotBtn.SetAction( (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, FORM_NAME_LEGEND_AREA , "" ) );
		m_wHotBtn.SetIndex( ::GetValueInt( sptrND, SECT_STATUI_ROOT, INDEX_LEGEND_AREA , 0 ) );

		m_strTitle = (char*)::GetValueString( sptrND, SECT_STATUI_STAT_LEGEND, CHART_TITLE , m_strTitle );
		m_strTitle = _change_chars( m_strTitle, "\\n", "\r\n" );

		// [vanek] SBT:1260 - 20.12.2004
		if( 1 < m_list_attached.count() )
		{	// более одного объекта - режим сравнения
			m_strCompareTitle = (LPCTSTR)::GetValueString(sptrND, SECT_STATCMP_STAT_LEGEND, CHART_TITLE, m_strCompareTitle );
			m_strCompareTitle = _change_chars( m_strCompareTitle, "\\n", "\r\n" );
		}

		strcpy( m_lfFontTitle.lfFaceName, (char*)::GetValueString( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_NAME , m_lfFontTitle.lfFaceName ) );
		m_lfFontTitle.lfHeight = ::GetValueInt( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_SIZE, m_lfFontTitle.lfHeight );
		m_lfFontTitle.lfWeight = ::GetValueInt( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_BOLD, m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
		m_lfFontTitle.lfItalic = ::GetValueInt( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_ITALIC, m_lfFontTitle.lfItalic ) != 0;
		m_lfFontTitle.lfUnderline = ::GetValueInt( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_UNDERLINE, m_lfFontTitle.lfUnderline ) != 0;
		m_lfFontTitle.lfStrikeOut = ::GetValueInt( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_STRIKEOUT, m_lfFontTitle.lfStrikeOut) != 0;
		m_lfFontTitle.m_clrText = ::GetValueColor( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, TEXT_COLOR , m_lfFontTitle.m_clrText );

	}
	
	void CLegendWnd::store_to_ndata( INamedDataPtr sptrND )
	{
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_NAME , m_lfFont.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_SIZE, (long)( m_lfFont.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_BOLD, (long)( m_lfFont.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_ITALIC, (long)m_lfFont.lfItalic );
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_UNDERLINE, (long)m_lfFont.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_FONT, FONT_STRIKEOUT, (long)m_lfFont.lfStrikeOut);

		::SetValueColor( sptrND, SECT_STATUI_STAT_LEGEND_FONT, TEXT_COLOR , m_lfFont.m_clrText );
		::SetValueColor( sptrND, SECT_STATUI_STAT_LEGEND , INCORRECT_COLOR , m_clIncorrectColor );
		::SetValueColor( sptrND, SECT_STATUI_STAT_LEGEND, SAMPLING_COLOR, m_lst_color_hot_data[0]);
		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND, SAMPLING_NAME, m_lst_text_hot_data[0]);

		::SetValue( sptrND, SECT_STATUI_ROOT, FORM_NAME_LEGEND_AREA , m_wHotBtn.GetAction() );

		::SetValue( sptrND, SECT_STATUI_ROOT, ENABLE_STAT_PARAM_AREA, (long)m_bEnableLegendArea );

		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
			if( !m_lpParams[i].bUserValue )
				continue;

			CString strName;
			CString strValue;
			CString strOrder;

			strName.Format( "%s%d", ENTRY_NAME,  m_lpParams[i].lUserID );
			strValue.Format( "%s%d", ENTRY_VALUE, m_lpParams[i].lUserID );
			strOrder.Format( "%s%d", ENTRY_ORDER, m_lpParams[i].lUserID );

			::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_USERDATA, strName, m_lpParams[i].m_szParamName );
			::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_USERDATA, strValue, m_lpParams[i].m_pparam_values[0].m_szParamValue );
			::SetValue( sptrND, SECT_STATUI_STAT_LEGEND_USERDATA, strOrder, (long)m_lpParams[i].lOrder );
		}

		::SetValue( sptrND, SECT_STATUI_STAT_LEGEND, CHART_TITLE , _change_chars( m_strTitle, "\r\n", "\\n" ) );

		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_NAME , m_lfFontTitle.lfFaceName );
		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_SIZE, (long)( m_lfFontTitle.lfHeight ) );
		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_BOLD, (long)( m_lfFontTitle.lfWeight > 400 ? 1 : 0 ) );
		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_ITALIC, (long)m_lfFontTitle.lfItalic );
		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_UNDERLINE, (long)m_lfFontTitle.lfUnderline );
		::SetValue( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, FONT_STRIKEOUT, (long)m_lfFontTitle.lfStrikeOut);
		::SetValueColor( sptrND, SECT_STATUI_LEGEND_TITLE_FONT_ROOT, TEXT_COLOR , m_lfFontTitle.m_clrText);

	}

	void CLegendWnd::attach_data()
	{
		if( m_list_attached.count() == 0 )
			return;

		m_bMinSizeCalc = false;

		for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
		{
			IStatObjectDispPtr sptrStatObject = m_list_attached.get( lpos_lst );
			IStatObjectPtr sptrSt = sptrStatObject;

			for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
			{
				if( m_lpParams[i].bUserValue )
					continue;

				long lKey = m_lpParams[i].lKey;

				double fValue = 0;

				HRESULT hr = sptrStatObject->GetValueGlobalInUnit2( lKey, &fValue );
 				if( hr == S_OK )
				{
					_bstr_t bstrUnit; 
					sptrSt->GetParamInfo( lKey, 0, bstrUnit.GetAddress() );
					
					if( bstrUnit.length() > 0 && !lid )
					{
						strcat( m_lpParams[i].m_szParamName, ", " );
						strcat( m_lpParams[i].m_szParamName, (char*)bstrUnit );
					}
					if(fabs(fValue)>1e20 || (fabs(fValue)<1e-20 && fValue!=0.)) {
						sprintf( m_lpParams[i].m_pparam_values[lid].m_szParamValue,  "%.6e", fValue );
					}else{
						sprintf( m_lpParams[i].m_pparam_values[lid].m_szParamValue,  m_lpParams[i].m_szParamFormat, fValue );
					}
					m_lpParams[i].m_pparam_values[lid].m_bIncorrect = false;
				}
				else
				{
					if( hr == S_FALSE )
						::LoadString( App::handle(), IDS_NOVAL,	m_lpParams[i].m_pparam_values[lid].m_szParamValue, sizeof( m_lpParams[i].m_pparam_values[lid].m_szParamValue ) );
					else
						::LoadString( App::handle(), IDS_NOEXPR,	m_lpParams[i].m_pparam_values[lid].m_szParamValue, sizeof( m_lpParams[i].m_pparam_values[lid].m_szParamValue ) );

					m_lpParams[i].m_pparam_values[lid].m_bIncorrect = true;
				}
			}
		}
	}

	void CLegendWnd::calc_min_rect( bool bPrintMode )
	{
		if( !m_bMinSizeCalc )
		{
 			m_rcMinRect.right = 0;

 			HDC hDC =::GetDC( 0 );
			Gdiplus::Graphics g(hDC); g.SetPageUnit(UnitPixel);

 			m_lfFont.SetHeight(-MulDiv( m_lfFont.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72));

			int w  = 0;
			int w2 = 0;
			m_nLineHeight = 0;

			int nLen = 0, nID = -1;

			long labs_max_comma_width = 0;
			for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
			{
				RECT rc = {0}, rc2 = {0};
				m_lfFont.MeasureString( g, CStringW(m_lpParams[i].m_szParamName), &rc, DT_LEFT | DT_TOP | DT_SINGLELINE );

				w = __max( rc.right, w );

				for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
				{
					RECT rc3 = {0};
					m_lfFont.MeasureString( g, CStringW(m_lpParams[i].m_pparam_values[lid].m_szParamValue), &rc3, DT_LEFT | DT_TOP | DT_SINGLELINE );

					rc2.top = rc3.top;
					rc2.bottom = rc3.bottom;

					rc2.left = rc3.left;
					rc2.right += rc3.right + NCOLUMN_OFFSET;

					m_lst_value_width[lid]=__max( m_lst_value_width[lid], rc3.right + NCOLUMN_OFFSET );
				}

				w2 = __max( rc2.right, w2 );

				m_nLineHeight = __max( rc.bottom - rc.top, m_nLineHeight );
				m_nLineHeight = __max( rc2.bottom - rc2.top, m_nLineHeight );

 				for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
				{
					CString str = m_lpParams[i].m_pparam_values[lid].m_szParamValue;
					
					int nIndex = -1;
					nIndex = str.Find( "." );

					if( nIndex == -1 )
						nIndex = str.Find( "," );

					if( nIndex == -1 )
						nIndex = str.Find( "E" );

					if( nIndex == -1 )
						nIndex = str.Find( "e" );

					if( nIndex != -1 )
					{
						if( nLen < str.GetLength() - nIndex - 1 )
						{
							nLen = __max( nLen, str.GetLength() - nIndex - 1 );
							nID = i;
						}
						str = str.Right( str.GetLength() - nIndex );

						if( !str.IsEmpty() )
						{
							RECT rc3 = {0};
							m_lfFont.MeasureString( g, CStringW(str), &rc3, DT_LEFT | DT_TOP | DT_SINGLELINE );

							RECT rc4 = {0};
							m_lfFont.MeasureString( g, CStringW("aa"), &rc4, DT_LEFT | DT_TOP | DT_SINGLELINE );

							m_lpParams[i].m_pparam_values[lid].lCommaPart = rc3.right - rc3.left - rc4.right;
						}
					}
 					if( nID != -1 )
					{
						m_lst_max_comma_width[lid]=__max( m_lst_max_comma_width[lid], m_lpParams[nID].m_pparam_values[lid].lCommaPart );
					}
				}
			}

			labs_max_comma_width = 0;
			labs_max_comma_width = m_lst_max_comma_width.sum();				
			labs_max_comma_width *= m_lst_max_comma_width.size() + 1; 

			m_sum_value_width = w;

			if( w2 > w )
				w = w2;

			/*for( long lp = m_lst_value_width.head(), lc = 0; lp; lp = m_lst_value_width.next( lp ), lc++ )
			{
				if( m_lst_value_width.get( lp ) > w )
					w = __max( w, m_lst_value_width.get( lp ) );
			}  */


   		m_rcMinRect.right = w + w2 + 15 + 15 + 10 + labs_max_comma_width;

			m_nDescrWidth = w;

			if( m_list_attached.count() > 1 )
				m_rcMinRect.right -= m_rcMinRect.right / 2 - m_nDescrWidth;

			m_lhot_data_height = 0;
			if( m_list_attached.count() > 1 )
			{
				RECT rc_hot = {0};
				m_lfFont.MeasureString( g, CStringW("QQ"), &rc_hot, DT_TOP | DT_LEFT );
				InflateRect( &rc_hot, 2, 0 );
				m_lhot_data_height = __max( rc_hot.bottom, m_lhot_data_height );
			}

			// [vanek] SBT:1260 - 20.12.2004
			CString strTitle = 1 < m_list_attached.count() ? m_strCompareTitle : m_strTitle;

			::DrawText( hDC, strTitle , strTitle .GetLength(), &m_rcTitle, DT_CALCRECT | DT_TOP | DT_LEFT );
			InflateRect( &m_rcTitle , 2, 0 );
			::OffsetRect( &m_rcTitle, -m_rcTitle.left, -m_rcTitle.top );


			{
				LOGFONT lf = m_lfFontTitle;
 				lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72) );

				HFONT hFontTitle = ::CreateFontIndirect( &lf );

				HFONT hFontOld = (HFONT)::SelectObject( hDC, hFontTitle );

				::ZeroMemory( &m_rcTitle, sizeof( m_rcTitle ) );

 				if( !strTitle .IsEmpty() )
				{
 					::DrawText( hDC, strTitle , strTitle .GetLength(), &m_rcTitle, DT_CALCRECT | DT_TOP | DT_LEFT );
					InflateRect( &m_rcTitle , 2, 0 );
					::OffsetRect( &m_rcTitle, -m_rcTitle.left, -m_rcTitle.top );
				}

				m_rcMinRect.right = __max( m_rcMinRect.right, m_rcTitle.right );

				::SelectObject( hDC, hFontOld );
				::DeleteObject( hFontTitle );
			}

			::ReleaseDC( 0, hDC );

 			m_rcMinRect.bottom = m_rcTitle.bottom + ( m_lParamCount + m_lCustomDataCount )  * m_nLineHeight + 20 + 15 + m_lhot_data_height;
 			m_bMinSizeCalc = true;
			{
				RECT rc;
				::GetClientRect( m_hwnd, &rc );
				_resize_child( rc );
			}
		}
	}

	bool CLegendWnd::create( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass )
	{
		
		if( !__super::create_ex( style, rect, pszTitle, parent, hmenu, pszClass, WS_EX_CLIENTEDGE ))
			return false;

		m_wHotBtn.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );

		return true;
	}

	void CLegendWnd::_resize_child( RECT rect )
	{
		{
			RECT rc = {0};
			GetWindowRect( m_hwnd, &rc );
			m_ptOffset = *((POINT *)&rc);
			::ScreenToClient( GetParent( m_hwnd ), &m_ptOffset );
		}

		int nBtn = 15;

  		RECT rcMin = get_min_rect();
		
		m_rcFull = rect;
		::InflateRect( &rect, -5, -5 );

		m_rcClient = rect;

		RECT rcHotBtn = { m_rcClient.right - nBtn, m_rcClient.top, m_rcClient.right, m_rcClient.top + nBtn };
	
		//if( m_bMinSizeCalc && m_wHotBtn.handle() )
		m_wHotBtn.move_window( rcHotBtn );

		if( m_bMinSizeCalc )
		{
			if( m_bEnableLegendArea )
				::ShowWindow( m_wHotBtn.handle(), SW_SHOW );
			//else
			//	::ShowWindow( m_wHotBtn.handle(), SW_HIDE );
		}

		int _h = m_rcTitle.bottom - m_rcTitle.top;
		m_rcTitle.top = m_rcClient.top;
 		m_rcTitle.bottom = m_rcTitle.top + _h;

   	m_rcTitle.left = m_rcClient.left;
		m_rcTitle.right = m_rcClient.right;

		RECT rcText = m_rcClient;
		::InflateRect( &rcText, -nBtn, 0 );
    	rcText.top += __max( nBtn, m_rcTitle.bottom ) ;

		double _fZoom = 1;

 		int nCenterOffset = 0;
		int w = 0;
		if( m_list_attached.count() > 1 )
			// [vanek] : пытаемся отцентроваться в заданном прямоугольнике - 23.12.2004
			w = int((rcText.right + rcText.left)/2/_fZoom - (m_rcMinRect.right - m_rcMinRect.left - 30)/2 + m_nDescrWidth + nCenterOffset);
		else
			w = int( __max( ( rcText.right + rcText.left ) / 2/ _fZoom, rcText.left + m_nDescrWidth + nCenterOffset ) );


 		for( int i = 0; i < m_lParamCount + m_lCustomDataCount; i++ )
		{
   			m_lpParams[i].m_rectParamName.right = w - nCenterOffset;

			m_lpParams[i].m_rectParamName.left = m_lpParams[i].m_rectParamName.right - m_nDescrWidth;
			m_lpParams[i].m_rectParamName.top = m_lhot_data_height + rcText.top + i * m_nLineHeight;
			m_lpParams[i].m_rectParamName.bottom = m_lhot_data_height + rcText.top + (i + 1) * m_nLineHeight;

			long llast_x = m_lpParams[i].m_rectParamName.right;
			for( long lpos_lst = m_list_attached.head(), lid = 0; lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ), lid++ )
			{
				m_lpParams[i].m_pparam_values[lid].m_rectParamValue.left = llast_x + nCenterOffset * 2 + m_lst_max_comma_width[lid];
				long nValueWidth = m_lst_value_width[lid];

				m_lpParams[i].m_pparam_values[lid].m_rectParamValue.top = m_lhot_data_height + rcText.top + i * m_nLineHeight;
				m_lpParams[i].m_pparam_values[lid].m_rectParamValue.right = m_lpParams[i].m_pparam_values[lid].m_rectParamValue.left + nValueWidth;
				m_lpParams[i].m_pparam_values[lid].m_rectParamValue.bottom = m_lhot_data_height + rcText.top + (i + 1) * m_nLineHeight;
				llast_x = m_lpParams[i].m_pparam_values[lid].m_rectParamValue.right + NCOLUMN_OFFSET;

				RECT rc_hot_data = { m_lpParams[i].m_pparam_values[lid].m_rectParamValue.left, rcText.top, m_lpParams[i].m_pparam_values[lid].m_rectParamValue.right, rcText.top + m_lhot_data_height };
				m_lst_rect_hot_data[lid]=rc_hot_data;
			}

		}
	}

	long CLegendWnd::on_size( short cx, short cy, ulong fSizeType )
	{
		RECT rc;
		::GetClientRect( m_hwnd, &rc );
		
		_resize_child( rc );
		::InvalidateRect(m_hwnd,0,FALSE);

		return __super::on_size( cx, cy, fSizeType );
	}

	long CLegendWnd::on_mousemove( const _point &point )
	{
		for( unsigned lp_rect =0;  lp_rect<m_lst_rect_hot_data.size(); ++lp_rect )
		{
			RECT rc_data = m_lst_rect_hot_data[lp_rect];

			if( ::PtInRect( &rc_data, point ) )
			{
				::SetCursor( m_cur_hand );
				break;
			}
			else
				::SetCursor( m_cur_arrow );
		}
		
		return __super::on_mousemove( point );
	}

	long CLegendWnd::on_lbuttondown( const _point &point )
	{
		for( unsigned lid =0;  lid<m_lst_rect_hot_data.size(); ++lid )
		{
			RECT rc_data = m_lst_rect_hot_data[lid];

			if( ::PtInRect( &rc_data, point ) )
			{
				::SendMessage( ::GetParent( handle() ), (WM_USER + 0x5546), lid, 0 );
				break;
			}
		}

		return __super::on_lbuttondown( point );
	}

	long CLegendWnd::handle_message( UINT m, WPARAM w, LPARAM l )
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

	int CLegendWnd::Print(CDCHandle& dc, CRect rectTarget, CRect RectDraw)
	{
		if (!dc)
			return 0;

		m_bPrinting = true;

		calc_min_rect(m_bPrinting);
		m_bMinSizeCalc=false;
		const CRect& rcMin=get_min_rect();
		double fX=double(RectDraw.Width())/rcMin.Width();
		double fY=double(RectDraw.Height())/rcMin.Height();
		double fZ=__min(fX,fY);

		CRect rcLog=rcMin;
		if(fZ>=1.){
			rcLog=CRect(0, 0, (int)ceil(double(RectDraw.Size().cx)*fZ),
				(int)ceil(double(RectDraw.Size().cy)*fZ));
		}else{
				rcLog=rcMin;
		}

		_resize_child(rcLog);

		dc.SetMapMode(MM_ANISOTROPIC);
		dc.SetWindowOrg(rcLog.left,0);
//		rcLog.right += rcLog.Width();
		dc.SetWindowExt(rcLog.Size());

		dc.SetViewportExt(rectTarget.Size());
		dc.SetViewportOrg(rectTarget.TopLeft());

		CRgn ClipRegion;
		if (ClipRegion.CreateRectRgnIndirect(rectTarget))
			dc.SelectClipRgn(ClipRegion);
		ClipRegion.DeleteObject();

		DoDraw( dc, rcLog, 1., true );

		return 0;
	}
}