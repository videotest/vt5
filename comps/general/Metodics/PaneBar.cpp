// PaneBar.cpp : implementation file
//

#include "stdafx.h"
#include "Metodics.h"
#include "PaneBar.h"
#include "misc.h"

#include "GDIPlusManager.h"

namespace MetodicsTabSpace
{
namespace EnviromentClasses
{


#define HEIGHT_PANEHEADER 39
#define PANE_IMAGE_SIZE 32

#define HEIGHT_PANEHEADER_SMALL 20
#define PANE_IMAGE_SIZE_SMALL 16

#define CTRL_START_ID 10000
#define SCROLL_BTN_WIDTH 10
#define EMPTY_PANE_HEIGHT 5//15
#define ICON_TEXT_OFFSET 5
#define PANE_ITEM_OFFSET_BOTTOM 2
#define PANE_ITEM_OFFSET_TOP 2

#define PANE_ITEM_IMAGE_SIZE 18
#define PANE_ITEM_IMAGE_SIZE_SMALL 18

#define PANE_TEXT_OFFSET 10
#define PANE_TEXT_OFFSET_RIGHT 10

#define PANE_GROUP_OFFSET PANE_IMAGE_SIZE_SMALL

#define PANE_ITEM_OFFSET_LEFT 10
#define PANE_ITEM_OFFSET_LEFT_SMALL 5

#define ITEM_DRAW_ACTIVE ( 1 << 0 )
#define ITEM_DRAW_OVER (   1 << 1 )
#define ITEM_DRAW_CHECKED (   1 << 2 )

#define SHOW_TIMER 0x1789
#define HIDE_TIMER 0x1790

#define MOUSE_TRACE 1

//#define USE_POPUP_MENU

HICON CPaneBar::m_hicon_group_open = 0;
HWND CPaneItem::m_hwnd_open_group = 0;
CPaneItem *CPaneItem::m_pactive_item = 0;

bool CPaneItem::m_bhelp_mode = false;

bool bDragDrop=false;
IActionInfo *CPaneItem::sptrActionInfo;

// vanek : drawing disabled icons - 27.02.2005
CImageDrawer CPaneItem::m_ImgDrawer;

bool file_exist( const char *psz )
{
	HANDLE	h = ::CreateFile( psz, 0, 0, 0, OPEN_EXISTING, 0, 0 );
	if( h == INVALID_HANDLE_VALUE )
		return false;

	::CloseHandle( h );

	return true;
}

void set_status_text( const char *psz )
{
	IApplicationPtr sptrApp = ::GetAppUnknown();
	static CString str_prev_status;
	
	if( sptrApp != 0 )
	{
		IUnknown *punkSBar = 0;
		sptrApp->GetStatusBar( &punkSBar );

		IWindowPtr ptrWindow = punkSBar;
		if( punkSBar )
			punkSBar->Release(); punkSBar = 0;

		if( ptrWindow )
		{
			HWND hsbar = 0;
			ptrWindow->GetHandle( (void**)&hsbar );

			if( psz )
			{
				if( str_prev_status != psz )
				{
					if( hsbar )
						::SendMessage( hsbar, WM_SETTEXT, 0, (LPARAM)( psz ) );
					str_prev_status = psz;
				}
			}
			else
			{
				str_prev_status.Empty();
				if( hsbar )
					::SendMessage( hsbar, WM_SETTEXT, 0, 0 );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CPaneItem::CPaneItem()
{
	::ZeroMemory( &m_ptOffset, sizeof( m_ptOffset ) );
	::ZeroMemory( &m_rcItem, sizeof( m_rcItem ) );

	::ZeroMemory( &m_rcImage, sizeof( m_rcImage ) );

	m_rcImage.right = m_rcImage.bottom = PANE_ITEM_IMAGE_SIZE;

	m_hIcon = 0;
	m_nMainWidth = 0;
	m_bEmptyCommand = false;
	m_nid = 0;
	m_bbreaker = false;
	m_psub_items = 0;
	m_bcustom_icon = false;
	m_cl_back_color = 0;
	m_bsum_mode = false;

	m_cl_color_breaker_1 = ::GetValueColor( ::GetAppUnknown(), "\\Methodics", "BreakerColor_up", ::GetSysColor( COLOR_3DFACE ) );
	m_cl_color_breaker_2 = ::GetValueColor( ::GetAppUnknown(), "\\Methodics", "BreakerColor_down", ::GetSysColor(  COLOR_3DHILIGHT ) );

	flag_activeItem=false;
	m_bDropItemPane=false;
	yHeightItem=0;

	bDragDrop=false;
}

CPaneItem::~CPaneItem()
{
	if( m_hIcon )
		VERIFY( ::DestroyIcon( m_hIcon ) );
	m_hIcon=0;

	if( m_psub_items )
		delete m_psub_items;
}

void CPaneItem::do_draw( HDC hDC, DWORD dwFlags, bool bCalcOnly )
{
  	RECT rcText = { m_ptOffset.x, m_ptOffset.y, 0, 0 };
	
 	COLORREF crText = ::SetTextColor( hDC, ::GetSysColor( COLOR_BTNTEXT ) );
	COLORREF crBk = ::SetBkColor( hDC, ::GetSysColor( COLOR_BTNFACE ) );

	COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
	COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

	COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

	COLORREF clrLight2 = RGB (GetRValue(clrBk) - ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2)*0.5,
								GetGValue(clrBk) - ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2)*0.5,
								GetBValue(clrBk) - ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2)*0.5);
	COLORREF clrLight3 = RGB (GetRValue(clrHighlight) - ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2) * 0.5,
								GetGValue(clrHighlight) - ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2) * 0.5,
 								GetBValue(clrHighlight) - ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2) * 0.5 );

	HFONT hFontOld = (HFONT)::SelectObject( hDC, GetStockObject( DEFAULT_GUI_FONT ) );


	::DrawText( hDC, m_strActionUserName, m_strActionUserName.GetLength(), &m_rcItem, DT_SINGLELINE | DT_CALCRECT );

 	if( !bCalcOnly )
		m_rcItem.right = m_rcItem.left/* + m_nMainWidth*/;
	rcText = m_rcItem;
	::OffsetRect( &rcText, 0, PANE_ITEM_OFFSET_TOP );


	::OffsetRect( &rcText, m_ptOffset.x + m_rcImage.right + ICON_TEXT_OFFSET + ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ), m_ptOffset.y );

	rcText.right = rcText.left + m_nMainWidth - 20 - ( m_psub_items ? PANE_IMAGE_SIZE_SMALL : 0 );

	if( m_bbreaker )
	{
		HPEN hPen = ::CreatePen( PS_SOLID, 1, m_cl_color_breaker_1 );
		HPEN hPen2 = ::CreatePen( PS_SOLID, 1, m_cl_color_breaker_2 );

		POINT pt = { 0 };

		HPEN hold = (HPEN)::SelectObject( hDC, hPen );

 		int nh = ( get_virtual_rect().bottom - get_virtual_rect().top ) / 2;
		::MoveToEx( hDC, rcText.left, rcText.top + nh, &pt );
		::LineTo( hDC, rcText.right, rcText.top + nh );

		::SelectObject( hDC, hPen2 );

		::MoveToEx( hDC, rcText.left, rcText.top + nh + 1, &pt );
		::LineTo( hDC, rcText.right, rcText.top + nh + 1 );

		::SelectObject( hDC, hold );
		VERIFY(::DeleteObject( hPen ));
		VERIFY(::DeleteObject( hPen2 ));

		return;
	}


	DWORD dwFlag = get_action_flag();

	DWORD dwStateFlag = !( dwFlag & afEnabled ) ? DSS_DISABLED : 0;

	if( m_bEmptyCommand && m_psub_items )
		dwStateFlag = 0;

	BOOL m_bEnablePane=false;
	if(m_psub_items)
	{
		for( long lPos = m_psub_items->m_listItems.head(); lPos; lPos = m_psub_items->m_listItems.next( lPos ) )
		{
			CPaneItem *pItem = m_psub_items->m_listItems.get( lPos );
			if( pItem->is_empty() )
				continue;
	
			DWORD dwFlags = 0;
			DWORD dw = pItem->get_action_flag();
			if(dw & afEnabled)
				m_bEnablePane=true;
		}
		if(!m_bEnablePane)
		{
			dwStateFlag = DSS_DISABLED;
			
		}
	}

 	if( dwFlag & afChecked )
		dwFlags |= ITEM_DRAW_CHECKED;

 	int cy = 18;//max( rcText.bottom - m_ptOffset.y, 16 );
 	RECT rc_draw = { m_ptOffset.x - 1 + ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ), m_ptOffset.y + PANE_ITEM_OFFSET_TOP - 1, rcText.right + PANE_IMAGE_SIZE_SMALL + 1, m_ptOffset.y + cy + PANE_ITEM_OFFSET_TOP -1 }; 

	yHeightItem=rc_draw.bottom-rc_draw.top;
 	//::InflateRect( &rc_draw, 2, 2 ); 

	if( !bCalcOnly )
		if( dwFlags &  ITEM_DRAW_ACTIVE && dwStateFlag!=DSS_DISABLED)
		{
			::FillRect( hDC, &rc_draw,  CBrush( clrLight ) );

			HPEN hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
			HPEN hold = (HPEN)::SelectObject( hDC, hPen );
			HBRUSH holdB = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );

			::Rectangle( hDC, rc_draw.left, rc_draw.top, rc_draw.right, rc_draw.bottom );

			::SelectObject( hDC, hold );
			::SelectObject( hDC, holdB );
			
			VERIFY(::DeleteObject( hPen ));
		}
		else if( dwFlags &  ITEM_DRAW_CHECKED && dwStateFlag!=DSS_DISABLED)
		{
			::FillRect( hDC, &rc_draw,  CBrush( clrLight2 ) );

			HPEN hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
			HPEN hold = (HPEN)::SelectObject( hDC, hPen );
			HBRUSH holdB = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );

			::Rectangle( hDC, rc_draw.left, rc_draw.top, rc_draw.right, rc_draw.bottom );

			::SelectObject( hDC, hold );
			::SelectObject( hDC, holdB );
			
			VERIFY(::DeleteObject( hPen ));
		}

	int nBkMode = ::SetBkMode( hDC, TRANSPARENT );

  	if( !bCalcOnly )
	{
		HBRUSH hbrush = ::CreateSolidBrush( clrLight3 );
		if( dwFlags &  ITEM_DRAW_OVER && dwStateFlag!=DSS_DISABLED)
	 		::FillRect( hDC, &rc_draw, hbrush );

		if( dwStateFlag == DSS_DISABLED )
	  		::DrawState( hDC, 0, 0, LPARAM( (LPCTSTR)m_strActionUserName), m_strActionUserName.GetLength(), rcText.left, rcText.top, rcText.right - rcText.left - 3, rcText.bottom - rcText.top, DST_TEXT | dwStateFlag  );
		else
	  		::DrawText( hDC, (LPCTSTR)m_strActionUserName, m_strActionUserName.GetLength(), &rcText, DT_SINGLELINE | DT_END_ELLIPSIS  );

		VERIFY(::DeleteObject( hbrush ));
	}

	SIZE size = { m_rcImage.right, m_rcImage.bottom };
  	if( !bCalcOnly )
	{
		// vanek : drawing disabled icons - 27.02.2005
		RECT rc_draw = {0};
		rc_draw.left = m_ptOffset.x + ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ) + 2;
		rc_draw.top = m_ptOffset.y + 2;
		rc_draw.right = rc_draw.left + size.cx;
		rc_draw.bottom = rc_draw.top + size.cy;
        
		bool bdrawn_disabled = false;
		if( !(dwFlag & afEnabled) || (!m_bEnablePane && m_psub_items) )
			bdrawn_disabled = m_ImgDrawer.DrawImage( hDC, m_hIcon, rc_draw, (idfScalingImage | idfUseAlpha) );			
		if( !bdrawn_disabled )
			::DrawState( hDC, 0, 0, (LPARAM)m_hIcon, 0, rc_draw.left, rc_draw.top, size.cx, size.cy, DST_ICON | dwStateFlag );
	}

	if( !bCalcOnly )
		if( dwFlags &  ITEM_DRAW_OVER && dwStateFlag!=DSS_DISABLED)
		{
 			HPEN hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
			HPEN hold = (HPEN)::SelectObject( hDC, hPen );
			HBRUSH holdB = (HBRUSH)::SelectObject( hDC, GetStockObject( NULL_BRUSH ) );

			::Rectangle( hDC, rc_draw.left, rc_draw.top, rc_draw.right, rc_draw.bottom );

			::SelectObject( hDC, hold );
			::SelectObject( hDC, holdB );
			
			VERIFY(::DeleteObject( hPen ));
		}

	if( m_psub_items )
	{
		RECT rc_text = rcText; 

		rc_text.left = rc_text.right + 1;
		rc_text.right = rc_text.left + PANE_IMAGE_SIZE_SMALL;

		bool bdrawn_disabled = false;
		if( !m_bEnablePane ) 
			bdrawn_disabled = m_ImgDrawer.DrawImage( hDC, CPaneBar::m_hicon_group_open, rc_text, (idfScalingImage | idfUseAlpha) );			
		if( !bdrawn_disabled )
			::DrawState( hDC, 0, 0, (LPARAM)CPaneBar::m_hicon_group_open, 0, rc_text.left, rc_text.top, rc_text.right - rc_text.left, rc_text.bottom - rc_text.top, DST_ICON | dwStateFlag );
	}

	::SetBkMode( hDC, nBkMode );

	::SetTextColor( hDC, crText );
	::SetBkColor( hDC, crBk );
	::SelectObject( hDC, hFontOld );
}

RECT CPaneItem::get_virtual_rect()
{
	RECT rcText = { m_ptOffset.x, m_ptOffset.y, m_ptOffset.x + max( m_rcItem.right, m_nMainWidth ) + m_rcImage.right + ICON_TEXT_OFFSET + ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ), m_ptOffset.y + max( max( m_rcItem.bottom, m_rcImage.bottom ), PANE_ITEM_IMAGE_SIZE ) + PANE_ITEM_OFFSET_BOTTOM };
	return rcText;
}

void CPaneItem::set_main_width( long lWidth ) 
 { 
//	 if( m_bEmptyCommand )
//		 return;

  	m_nMainWidth =  lWidth - ICON_TEXT_OFFSET - 2 * PANE_TEXT_OFFSET_RIGHT - ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ) - 2; 
}

void CPaneItem::store_content( INamedData *punkND, CString strRoot )
{
	::SetValue( punkND, strRoot, ENTRY_ITEM_NAME, m_strActionName );

	::SetValue( punkND, strRoot, ENTRY_ITEM_CUSTOM_NAME, m_str_custom_icon );
	::SetValueColor( punkND, strRoot, ENTRY_ITEM_CUSTOM_ICON_BACK_COLOR, m_cl_back_color );

	if( m_psub_items )
	{
		long lSubCount = m_psub_items->get_item_count();
		::SetValue( punkND, strRoot, "Sub" ENTRY_ITEM_COUNT, (long)lSubCount );

		for( long i = 0; i < lSubCount; i++ )
		{
			CPaneItem *pItem = m_psub_items->get_item( i );
			CString strSect;
			strSect.Format( "\\SubItem" "%d", i );

			pItem->store_content( punkND, strRoot + strSect );
		}
	}
}

void CPaneItem::load_content( INamedData *punkND, CString strRoot )
{
	m_strActionName = ::GetValueString( punkND, strRoot, ENTRY_ITEM_NAME, m_strActionName );

	if( m_strActionName.Find("---" ) != -1 )
	{
		m_bbreaker = true;
		return;
	}

	IApplicationPtr sptApp = ::GetAppUnknown();
	
	IUnknown *punkActionMan = 0;
	sptApp->GetActionManager( &punkActionMan );

	IActionManagerPtr sptrActionMan = punkActionMan;
	punkActionMan->Release(); punkActionMan = 0;

	IUnknown *punkAI = 0;
	sptrActionMan->GetActionInfo( _bstr_t( m_strActionName ), &punkAI );

	IActionInfoPtr sptrAI = punkAI;

	m_str_custom_icon = ::GetValueString( punkND, strRoot, ENTRY_ITEM_CUSTOM_NAME, m_str_custom_icon );
	m_cl_back_color = ::GetValueColor( punkND, strRoot, ENTRY_ITEM_CUSTOM_ICON_BACK_COLOR, m_cl_back_color );

	CString strPath = ::GetValueString( ::GetAppUnknown(), "\\Paths", "States", "" );
	if( !m_str_custom_icon.IsEmpty() )
	{
		_trace_file( LOG_FILE, "[~] Custom icon %s for %s is set", m_str_custom_icon, m_strActionName );
		m_bcustom_icon = true;
	}

	if( !m_str_custom_icon.IsEmpty() && !file_exist( strPath + m_str_custom_icon ) )
	{
		m_bcustom_icon = false;
		_trace_file( LOG_FILE, "[-] Custom icon for %s is not found", m_strActionName );
	}
	else
		_trace_file( LOG_FILE, "[+] Custom icon for %s is found", m_strActionName );

	if( punkAI )
	{
		punkAI->Release(); punkAI = 0;
		create_from_info( sptrAI ); 
	}
	else
	{
		m_bEmptyCommand = true;

		m_strActionName	= m_strActionName; 
		m_strActionUserName =  m_strActionName;
		m_strActionDescr	= m_strActionName;

		m_rcImage.right =   PANE_IMAGE_SIZE_SMALL;
		m_rcImage.bottom = PANE_IMAGE_SIZE_SMALL;

		m_nid = ::GetValueInt( punkND, strRoot, ENTRY_ITEM_HELPID, 0 );

		if( m_bcustom_icon )
			create_from_info( 0 ); 
	}

	long lSubCount = ::GetValueInt( punkND, strRoot, "Sub" ENTRY_ITEM_COUNT );

	if( lSubCount )
	{
		m_psub_items = new CPaneItemPlace;
		m_psub_items->sub_mode( true );
	}

	HDC hDipl = ::GetDC( 0 );

	POINT pt = { 0, 0 };
	RECT rcItem = { 0 };

	for( long i = 0; i < lSubCount; i++ )
	{
		CPaneItem *pItem = new CPaneItem;

		CString strSect;
		strSect.Format( "\\SubItem" "%d", i );

		pItem->load_content( punkND, strRoot + strSect );
		pItem->set_sub_mode( true );

		pItem->do_draw( hDipl, 0, true );
		rcItem = pItem->get_virtual_rect();

		if( !pItem->is_empty() )
		{
			pItem->set_offset( pt );
			pt.y += rcItem.bottom;
		}

		m_psub_items->add_item( pItem );
	}

	::ReleaseDC( 0, hDipl );

}

void CPaneItem::create_from_info( IActionInfo *punkInfo )
{
	DWORD	dwCmdCode = 0, dwDllCode = 0;
	IBitmapProviderPtr sptrProv = 0;

	if( punkInfo )
	{
		_bstr_t bstrActionName, bstrActionUserName, bstrHelpString;
		punkInfo->GetCommandInfo( bstrActionName.GetAddress(), bstrActionUserName.GetAddress(), bstrHelpString.GetAddress(), 0 );

		m_strActionName		= (char *)bstrActionName;
		m_strActionUserName = (char *)bstrActionUserName;
		m_strActionDescr	= (char *)bstrHelpString;

		_bstr_t bstrDefName;
		IUnknown *punkProvider = 0;
		punkInfo->GetTBInfo( bstrDefName.GetAddress(), &punkProvider );

		punkInfo->GetRuntimeInformaton( &dwDllCode, &dwCmdCode );


		int nid = 0;  
		punkInfo->GetLocalID( &nid );
		m_nid = nid;

		sptrProv = punkProvider;
		if( punkProvider )
			punkProvider->Release(); punkProvider = 0;
	}

	if( !m_bcustom_icon )
	{
		if( sptrProv )
		{
			long lIndex = 0;
			long lBitmap = 0;

			sptrProv->GetBmpIndexFromCommand( dwCmdCode, dwDllCode, &lIndex, &lBitmap );

			if( lBitmap != -1 )
			{
				HBITMAP hBitMap = 0;
 				sptrProv->GetBitmap( lBitmap, (HANDLE *)&hBitMap );

				BITMAP bmp = {0};
   				::GetObject( hBitMap, sizeof( BITMAP ), &bmp );

				m_rcImage.right =   bmp.bmHeight;
				m_rcImage.bottom = bmp.bmHeight;

				// vanek : 27.02.2005
				if( m_rcImage.right < 16 )
					m_rcImage.right = 16;

				// [vanek] - 06.03.2005
				m_rcImage.bottom = m_rcImage.right;
				
     			HIMAGELIST hImageList = ::ImageList_Create( m_rcImage.right, m_rcImage.bottom, ILC_COLOR24 | ILC_MASK,	 ( bmp.bmWidth + 1) / ( m_rcImage.right  ), 1 );

				COLORREF cl_transparent = ::GetSysColor( COLOR_BTNFACE );

				HDC hdc_display = ::GetDC( 0 );
				
				HDC hdc_offscreen = ::CreateCompatibleDC( hdc_display );
				HDC hdc_offscreen2 = ::CreateCompatibleDC( hdc_display );

				// [vanek] - 06.03.2005
				int nwidth = bmp.bmWidth, 
					nheight = m_rcImage.bottom;

				// [vanek] - 06.03.2005
				HBITMAP hcompatible_bitmap = ::CreateCompatibleBitmap( hdc_offscreen, nwidth, nheight );
				
				HBITMAP hold_bitmap = (HBITMAP)::SelectObject( hdc_offscreen, hcompatible_bitmap );
				HBITMAP hold_bitmap2 = (HBITMAP)::SelectObject( hdc_offscreen2, hBitMap );

                // initalize whole area with 0's
				// [vanek] - 06.03.2005
				::PatBlt( hdc_offscreen, 0, 0,nwidth, nheight, WHITENESS);
  				
				// create mask based on color bitmap
				// convert this to 1's
				::SetBkColor( hdc_offscreen, cl_transparent );
				::SetBkColor( hdc_offscreen2, cl_transparent );

				// [vanek] - 06.03.2005
				::BitBlt( hdc_offscreen, 0, 0, nwidth, nheight,	hdc_offscreen2, 0, 0, SRCCOPY);
				
				::SelectObject( hdc_offscreen, hold_bitmap );
				::SelectObject( hdc_offscreen2, hold_bitmap2 );

	// 			::ImageList_AddMasked( hImageList, hBitMap, cl_transparent);
 				::ImageList_Add( hImageList, hBitMap, hcompatible_bitmap );
	//			m_hIcon = ::ImageList_GetIcon( hImageList, lIndex, ILD_MASK );
				m_hIcon = ::ImageList_GetIcon( hImageList, lIndex, ILD_NORMAL );

				VERIFY(::DeleteObject( hcompatible_bitmap ));

				::DeleteDC( hdc_offscreen );
				::DeleteDC( hdc_offscreen2 );

				::ReleaseDC( 0, hdc_display );

				_trace_file( LOG_FILE, "[~] Use standart icon for %s", m_strActionName );
														 
				::ImageList_Destroy( hImageList );
			}
		}
	}
	else
	{
		CString strPath = ::GetValueString( ::GetAppUnknown(), "\\Paths", "States", "" );
 		HBITMAP himage = (HBITMAP)::LoadImage( 0, strPath + m_str_custom_icon, IMAGE_BITMAP, PANE_IMAGE_SIZE_SMALL, PANE_IMAGE_SIZE_SMALL, LR_LOADFROMFILE | LR_LOADTRANSPARENT );

		if( !himage )
			_trace_file( LOG_FILE, "[-] Can't load custom icon for %s / last error: %d", m_strActionName, ::GetLastError() );

		BITMAP bmp = {0};
   		::GetObject( himage, sizeof( BITMAP ), &bmp );
     	HIMAGELIST hImageList = ::ImageList_Create( PANE_IMAGE_SIZE_SMALL, PANE_IMAGE_SIZE_SMALL, ILC_COLOR24 | ILC_MASK,	 1, 1 );
		COLORREF cl_transparent = m_cl_back_color;
		 ::ImageList_AddMasked( hImageList, himage, cl_transparent);
		m_hIcon = ::ImageList_GetIcon( hImageList, 0, ILD_TRANSPARENT );

		if( m_hIcon )
			_trace_file( LOG_FILE, "[+] Use custom icon for %s", m_strActionName );
		else
			_trace_file( LOG_FILE, "[-] Can't use custom icon for %s / last error: %d", m_strActionName, ::GetLastError() );

		::ImageList_Destroy( hImageList );
		VERIFY(::DeleteObject( himage ));
	}
}

void CPaneItem::execute_cmd()
{
	if( m_bEmptyCommand || m_bbreaker || CBCGToolBar::IsCustomizeMode() )
		return;

	IApplicationPtr sptApp = ::GetAppUnknown();
	
	IUnknown *punkActionMan = 0;
	sptApp->GetActionManager( &punkActionMan );

	IActionManagerPtr sptrActionMan = punkActionMan;
	punkActionMan->Release(); punkActionMan = 0;

	::FireEvent( ::GetAppUnknown(), szEventBeforeRecordMethodStep, 0, 0, (LPVOID)((LPCTSTR)m_strActionName), m_strActionName.GetLength() );
	sptrActionMan->ExecuteAction( _bstr_t( m_strActionName  ), 0, 0 );
	::FireEvent( ::GetAppUnknown(), szEventAfterRecordMethodStep, 0, 0,  (LPVOID)((LPCTSTR)m_strActionName), m_strActionName.GetLength() );
}

DWORD  CPaneItem::get_action_flag()
{
	if( m_bEmptyCommand && m_psub_items )
		return afEnabled;

	IUnknown *punkMan = 0;

	IApplicationPtr sptrApp = ::GetAppUnknown();
	sptrApp->GetActionManager( &punkMan );

	IActionManagerPtr sptrMan =	 punkMan;
	punkMan->Release();
	
	DWORD dwFlag = 0;
	sptrMan->GetActionFlags( _bstr_t(  m_strActionName ) , &dwFlag );
	return dwFlag;
}

void CPaneItem::on_mouse_over( POINT pt, HWND hwnd )
{
	if(flag_activeItem)
		return;
	POINT pt_cur = {0};
	::GetCursorPos( &pt_cur );
	
	if( m_hwnd_open_group && ::WindowFromPoint( pt_cur ) != m_hwnd_open_group )
	{

		RECT brRect = {0};
		GetWindowRect(m_hwnd_open_group, &brRect);
		
		if( m_pactive_item != this /*|| pt_cur.y>=brRect.top+yHeightItem*/)
		{
			yHeightItem=0;
#ifdef MOUSE_TRACE
			_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSEOVER. Destroy popup menu in mouse_over %x", m_hwnd_open_group );
#endif

		/*	BOOL fLB=false;
			if(flag_activeItem)
			{
				BOOL bLButtonIsDown = (::GetKeyState(VK_LBUTTON) < 0);
				if(!bLButtonIsDown)
					return;
				else
				{
					fLB=true;
					flag_activeItem=false;
				}
			}*/
			if(/*!fLB && */!flag_activeItem)
			{
			/*int i*/nCountItem=(int)GetTickCount();
			bool flag=false;
			flag_activeItem=true;
			while(!flag)
			{
				BOOL bLButtonIsDown = (::GetKeyState(VK_LBUTTON) < 0);
				if(bLButtonIsDown)
					flag=true;
				if(/*i*/nCountItem+500<=(int)GetTickCount())		
					flag=true;
			}
			flag_activeItem=false;
			}
			/*if(!flag_activeItem)
			{
				NumGetTick=(int)GetTickCount();
				flag_activeItem=true;
			}
			else
			{
				if(NumGetTick+800<=(int)GetTickCount())
					flag_activeItem=false;
			}
			if(flag_activeItem)
				return;*/

			::GetCursorPos( &pt_cur );
			if( m_hwnd_open_group && ::WindowFromPoint( pt_cur ) != m_hwnd_open_group )
			{
				if( m_pactive_item != this )
				{
					::DestroyWindow( m_hwnd_open_group );
					m_hwnd_open_group = 0;   
					m_pactive_item = 0;
					::UpdateWindow(hwnd);
				}
			}
		}
	}
	
	BOOL m_bEnablePane=false;
	if(m_psub_items)
	{
		for( long lPos = m_psub_items->m_listItems.head(); lPos; lPos = m_psub_items->m_listItems.next( lPos ) )
		{
			CPaneItem *pItem = m_psub_items->m_listItems.get( lPos );
			if( pItem->is_empty() )
				continue;
	
			DWORD dwFlags = 0;
			DWORD dw = pItem->get_action_flag();
			if(dw & afEnabled)
				m_bEnablePane=true;
		}
	}

	if(!m_bDropItemPane)
		if( !m_psub_items || !m_bEnablePane)
			return;

	HDC hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );
	HFONT hFontOld = (HFONT)::SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) );

	::DrawText( hdc, m_strActionUserName, m_strActionUserName.GetLength(), &m_rcItem, DT_SINGLELINE | DT_CALCRECT );

	RECT rcText = m_rcItem;
	::OffsetRect( &rcText, m_ptOffset.x + m_rcImage.right + ICON_TEXT_OFFSET + ( !m_bsum_mode ? PANE_ITEM_OFFSET_LEFT : PANE_ITEM_OFFSET_LEFT_SMALL ), m_ptOffset.y );
	rcText.right = rcText.left + m_nMainWidth - 20 - ( m_psub_items ? PANE_IMAGE_SIZE_SMALL : 0 );

	rcText.left = rcText.right + 1 + PANE_IMAGE_SIZE_SMALL;
	rcText.right = rcText.left + PANE_IMAGE_SIZE_SMALL;

 	if( m_psub_items && !m_psub_items->handle()/* && ::PtInRect( &rcText, pt )*/ )
	{
		HDC hDipl = ::GetDC( 0 );

		int nmax = 0;
		for( long lPos = m_psub_items->m_listItems.head(); lPos; lPos = m_psub_items->m_listItems.next( lPos ) )
		{
			CPaneItem *pItem = m_psub_items->m_listItems.get( lPos );

			pItem->m_nMainWidth = 0;
			pItem->do_draw( hDipl, 0, true );
			RECT rcItem = pItem->get_virtual_rect();
			nmax = max( nmax, rcItem.right - rcItem.left );
		}

		::ReleaseDC( 0, hDipl );

		m_psub_items->set_main_width( nmax + 50 );

		RECT rc_wnd  = m_psub_items->get_virtual_rect();

		::ClientToScreen( hwnd, (LPPOINT)&rcText );
		::ClientToScreen( hwnd, (LPPOINT)&rcText + 1 );

		::OffsetRect( &rc_wnd, rcText.left, rcText.top );

 		rc_wnd.bottom += 2;

#ifdef USE_POPUP_MENU
		m_psub_items->create( /*WS_VISIBLE | */WS_POPUP, rc_wnd, 0, hwnd );
#else
		IApplicationPtr sptrApp = GetAppUnknown();

 		if( sptrApp )
		{
			HWND hwndMain = 0;
			sptrApp->GetMainWindowHandle( &hwndMain );

 			if( hwndMain )
			{
				::ScreenToClient( hwndMain, (LPPOINT)&rc_wnd );
				::ScreenToClient( hwndMain, (LPPOINT)&rc_wnd + 1 );
			}
 			m_psub_items->create_ex( WS_CHILD, rc_wnd, 0, hwndMain, 0, 0, WS_EX_TOPMOST ); 
	 		::SetWindowPos( m_psub_items->handle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
		}
#endif

		int ntime = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "PopupMenuTimeInterval", 250 );
		::SetTimer( m_psub_items->handle(), SHOW_TIMER, ntime, 0 );
		//::SetFocus( m_psub_items->handle() );

#ifdef MOUSE_TRACE
		_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSEOVER. Set new open pane %x", m_psub_items->handle() );
#endif
		m_hwnd_open_group = m_psub_items->handle();
		m_pactive_item = this;

	}

	::DeleteDC( hdc );
}

void CPaneItem::on_mouse_leave()
{
	set_status_text( 0 );
}

//////////////////////////////////////////////////////////////////////////
CPaneItemPlace::CPaneItemPlace()
{
	m_brunned = false;
	m_bsub_mode = false;
	m_pCtrl = 0;

	m_ptPrevMouse.x = m_ptPrevMouse.y = -10000;

	m_hActive = 0;
	m_hHand = ::LoadCursor( AfxGetResourceHandle(), MAKEINTRESOURCE( IDC_CURSOR_HAND ) );

	m_bLDown = false;

	m_ptDown.x = m_ptDown.y = -10;

	::ZeroMemory( &m_ptOver, sizeof( m_ptOver ) );
	::ZeroMemory( &m_rcOver, sizeof( m_rcOver ) );

	m_bMouseOver = false;
	m_bToolTipVisible=false;
	iKeyDown=-1;
	//fKeyDown=false;
	iCountPaint=0;
	flag_alpha=false;
}

CPaneItemPlace::~CPaneItemPlace()
{
	::DeleteObject( m_hHand );
}


RECT CPaneItemPlace::get_virtual_rect()
{
	RECT rc = { 0, 0, 0, m_listItems.head() ? 0 : EMPTY_PANE_HEIGHT  };

	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );

		if( pItem->is_empty() )
			continue;

		RECT rcItem = pItem->get_virtual_rect();

		rc.right = max( rc.right, rcItem.right );
		rc.bottom += rcItem.bottom - rcItem.top;
	}

	return rc;
}

CPaneItemPlace *CPaneItemPlace::get_sub_place_from_handle( HWND hwnd )
{
	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		if( pItem->m_psub_items && pItem->m_psub_items->handle() == hwnd )
			return pItem->m_psub_items;
	}
	return 0;
}


long CPaneItemPlace::on_timer( ulong lEvent )
{
	if( lEvent == SHOW_TIMER )
	{
		if( !::IsWindowVisible( handle() ) )
		{
			::ShowWindow( handle(), SW_SHOW );
			::KillTimer( handle(), SHOW_TIMER );
		}
	}
	else if( lEvent == HIDE_TIMER )
	{
		if( CPaneItem::m_bhelp_mode )
		{
			IUnknownPtr ptr_u( ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow ), false );
			IMainWindowPtr sptr_w = ptr_u;
			if( sptr_w )
			{
				BOOL bhelp_mode = 0;
				sptr_w->IsHelpMode( &bhelp_mode );

				if( !bhelp_mode )
				{
					POINT pt_cur = {0};
					::GetCursorPos( &pt_cur );

					RECT rc_window = {0};
					::GetWindowRect( handle(), &rc_window );
					if( !::PtInRect( &rc_window, pt_cur ) )
					{
#ifdef MOUSE_TRACE
						_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSELEAVE. Kill Help mode timer %x", handle() );
#endif
						::KillTimer( handle(), HIDE_TIMER );
						CPaneItem::m_bhelp_mode = false;

						HWND hparent = ::GetParent( handle() );
						::DestroyWindow( handle() );
						//::SetForegroundWindow( hparent );
						CPaneItem::m_hwnd_open_group = 0;
						CPaneItem::m_pactive_item = 0;

						return 0;
					}
				}
			}
		}
	}
	return win_impl::on_timer( lEvent );
}

long CPaneItemPlace::on_erasebkgnd( HDC hDC )
{
	//if(fKeyDown)
	//{
	//	fKeyDown=false;
		//return 0L;
	//}

 	RECT rect = {0};
	::GetClientRect( handle(), &rect );

	bool bIsOpen = ::SendMessage( ::GetParent( handle() ), WM_IS_PANE_OPEN, 0, 0 );

 	COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
	COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

	COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

 	if( !bIsOpen && !m_bsub_mode )
		clrLight = ::GetSysColor( COLOR_BTNFACE );

  	HBRUSH hBrush = ::CreateSolidBrush( clrLight );
	::FillRect( hDC, &rect, hBrush );

	if( bIsOpen )
		::DrawEdge( hDC, &rect, BDR_SUNKENOUTER, BF_RECT );

	if( m_bsub_mode )
		::DrawEdge( hDC, &rect, BDR_RAISEDINNER, BF_RECT );

	VERIFY(::DeleteObject( (HGDIOBJ )hBrush ));
	return 0L; 
}
long CPaneItemPlace::on_paint()
{
	BYTE keydown[256];
	BOOL bPaint=false;
	if(::GetKeyboardState(keydown))
	{
		if(iKeyDown>VK_XBUTTON2)
		{
			int res=::GetKeyState(iKeyDown);
			if(!(res&0x8000))
			{
				iKeyDown=-1;
				//fKeyDown=true;
				
				return 0L;
			}
			//iCountPaint++;
		}
		else
			if(iKeyDown==-1)
			{
				for(int i=VK_XBUTTON2+1;i<256;i++)
				{
					if(keydown[i]!=0)
					{
						int res=::GetKeyState(i);
						if(res&0x8000)
						{
							//fKeyDown=true;
							iKeyDown=i;
							return 0L;
						}
					}
				}
			}
	}
	//if(iCountPaint>0)
	//	bPaint=true;

	RECT rcClient = {0};
	::GetClientRect( handle(), &rcClient );

	PAINTSTRUCT ps = {0};
	HDC hDC = ::BeginPaint( handle(), &ps );

	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		if( pItem->is_empty() )
			continue;

		RECT rcItem = pItem->get_virtual_rect();

	//	if( rcItem.top < rcClient.top || rcItem.bottom > rcClient.bottom )
	//		continue;

		//if(bPaint)
		//	flag_alpha=true;	
		
		DWORD dwFlags = 0;

		DWORD dw = pItem->get_action_flag();
		if( dw & afEnabled && ::PtInRect( &rcItem, m_ptDown ) && !m_bMove )
			dwFlags |= ITEM_DRAW_ACTIVE;

 		if( dw & afChecked && ::PtInRect( &rcItem, m_ptDown ) && !m_bMove )
			dwFlags |= ITEM_DRAW_CHECKED;

		RECT rc_w = {0};
	
		if( pItem->m_psub_items && pItem->m_psub_items->handle() )
			::GetWindowRect( pItem->m_psub_items->handle(), &rc_w );

		::ScreenToClient( handle(), (LPPOINT)&rc_w );
		::ScreenToClient( handle(), (LPPOINT)&rc_w + 1 );

   		if( dw & afEnabled )
			if( m_bMouseOver &&  ::PtInRect( &rcItem, m_ptOver ) || 
				pItem->m_psub_items && ( rc_w.top == rcItem.top ) && ( pItem->m_psub_items->handle() || pItem->m_psub_items->is_runned() ) )
 				dwFlags |= ITEM_DRAW_OVER;

		//if(!bPaint)
		//{
		//	if(!flag_alpha)
		pItem->do_draw( hDC, dwFlags );
		//	else
		//		flag_alpha=false;
		//}
		//else
		//	if(dw & afEnabled)
		//		pItem->do_draw( hDC, dwFlags );
	}
	//if(bPaint && iCountPaint>1)
	//	iCountPaint=0;
	::EndPaint( handle(), &ps );
	return 0L;
}

void CPaneItemPlace::check_scroll( bool *pbUpScroll, bool *pbDownScroll )
{
	*pbUpScroll = false;
	*pbDownScroll = false;

	RECT rcClient = {0};
	::GetClientRect( handle(), &rcClient );

	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		if( pItem->is_empty() )
			continue;

		RECT rcItem = pItem->get_virtual_rect();

		if( !*pbUpScroll && rcItem.top < rcClient.top )
			*pbUpScroll = true;

		if( !*pbDownScroll && rcItem.bottom > rcClient.bottom )
			*pbDownScroll = true;
	}
}


int CPaneItemPlace::get_row_height()
{
	long lPos = m_listItems.head();

	if( !lPos )
		return EMPTY_PANE_HEIGHT;

	CPaneItem *pItem = m_listItems.get( lPos );

	if( !pItem )
		return EMPTY_PANE_HEIGHT;

	RECT rc = pItem->get_virtual_rect();

	return rc.bottom - rc.top;
}

void CPaneItemPlace::offset_items( POINT ptOffset )
{
 	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		if( pItem->is_empty() )
			continue;

		POINT pt = pItem->get_offset();
		pt.x += ptOffset.x;
		pt.y += ptOffset.y;

		pItem->set_offset( pt );
		
	}
}

CPaneItem *CPaneItemPlace::get_item( long lIndex )
{
	long lSkip = 0;
	for( long lPos = m_listItems.head(), i = 0; lPos; lPos = m_listItems.next( lPos ), i++ )
	{
		if( (i - lSkip) == lIndex )
		{
			CPaneItem *pItem = m_listItems.get( lPos );
			if( pItem->is_empty() )
			{
				lSkip++;
				continue;
			}
			return pItem;
		}
	}
	return 0;
}

long CPaneItemPlace::on_lbuttondown( const _point &point )
{
	m_bMove = false;
	
	::SetCapture( handle() );
	m_hActive = ::GetCursor();

	POINT pt = m_ptDown;
	::ClientToScreen( handle(), &pt );

	CPaneItem *pItem = get_item_by_pos( pt );

	if( pItem )
	{
		RECT rcItem = pItem->get_virtual_rect();
		if( !EqualRect( &rcItem, &m_rcOver ) )
 			::InvalidateRect( handle(),  &rcItem, TRUE );
	}

	m_ptDown = point;
	m_bLDown = true;

 	::InvalidateRect( handle(),  &m_rcOver, TRUE );

	return 0L;
}

long CPaneItemPlace::on_lbuttonup( const _point &point )
{
 	::SetCursor( m_hActive );

	if( ::GetCapture() != handle() ) return 0L; // если buttondown был не у нас - все идут лесом
	::ReleaseCapture();
	
	RECT rcOver = m_rcOver;

	if( !m_bMove )
	{
		POINT pt = point;
		::ClientToScreen( handle(), &pt );

		CPaneItem *pItem = get_item_by_pos( pt );

		if( pItem )
		{
			DWORD dwFlag = pItem->get_action_flag();

 			if( dwFlag & afEnabled )
			{
				if( m_bsub_mode )
				{
					::DestroyWindow( handle() );
					m_bMouseOver = 0;
					::ZeroMemory( &m_ptOver, sizeof( m_ptOver ) );
				}

				if( !m_bsub_mode )
				{
					::EnableWindow( handle(), FALSE );
					HWND hwnd = ::GetParent(handle()); // таб вместе со своим заголовком
					hwnd = ::GetParent(hwnd); // окошко со всеми табами
					::EnableWindow( hwnd, FALSE ); // SBT 1295, 27.07.2005 build 88
				}
				m_brunned = true;
				pItem->execute_cmd();
				m_brunned = false;
				if( !m_bsub_mode )
				{
					::EnableWindow( handle(), TRUE );
					HWND hwnd = ::GetParent(handle()); // таб вместе со своим заголовком
					hwnd = ::GetParent(hwnd); // окошко со всеми табами
					::EnableWindow( hwnd, TRUE );
				}

			}
		}
	}
	m_ptDown.x = m_ptDown.y = -10;
	m_bLDown = false;
	m_bMove = false;

	::InvalidateRect( handle(), 0, FALSE );
	::InvalidateRect( handle(), &rcOver, TRUE );
	return 0L;
}

long CPaneItemPlace::on_mousemove( const _point &point )
{
	m_bMouseOver = true;
	m_ptOver = point;

	if( m_ptPrevMouse.x == -10000 || m_ptPrevMouse.y == -10000 )
		m_ptPrevMouse = point;

	{
		POINT pt = point;
		::ClientToScreen( handle(), &pt );

		CPaneItem *pItem = get_item_by_pos( pt );
 		if( pItem )
		{
#ifdef MOUSE_TRACE
			if( !m_bsub_mode )
				_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSEMOVE. Mouse move %x", handle() );
			else
				_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSEMOVE. Sub mode mouse move %x", handle() );
#endif

			RECT rcItem = pItem->get_virtual_rect();
			if( !EqualRect( &m_rcOver, &rcItem ) )
			{
				::InvalidateRect( handle(), &m_rcOver, TRUE );
				m_rcOver = rcItem;
				::InvalidateRect( handle(), &m_rcOver, TRUE );
				m_bToolTipVisible=false;
			}
 			pItem->on_mouse_over( m_ptPrevMouse, handle() );
  			CString strTip = pItem->m_strActionDescr;

			int nIndex = -1;
			if( ( nIndex = strTip.Find( "\n" ) ) != -1 )
				strTip = strTip.Right( strTip.GetLength() - nIndex - 1 );

			if( m_pCtrl && !m_bToolTipVisible)
			{
				m_bToolTipVisible=true;
				m_pCtrl->UpdateTipText( strTip, CWnd::FromHandle( handle() ), 1500 );
				m_pCtrl->Activate( true );
			}

			if( nIndex != -1 )
			{
				CString str_text = pItem->m_strActionDescr.Left( nIndex );
				set_status_text( (const char *)str_text );
			}
			else
				set_status_text( 0 );
		}
	}

	TRACKMOUSEEVENT		csTME = {0};

	csTME.cbSize = sizeof( csTME );
	csTME.dwFlags = TME_LEAVE;
	csTME.hwndTrack = handle();

	::_TrackMouseEvent( &csTME );

	m_ptPrevMouse = point;

	return 0L;
}

long CPaneItemPlace::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_HELPHITTEST )
	{
		if( m_bsub_mode )
		{
			POINT pt_screen = { 0 };
			::GetCursorPos( &pt_screen );

			CPaneItem *pitem = get_item_by_pos( pt_screen );

			if( pitem )
				return pitem->get_id();
		}
	}
 	if( m == WM_IDLEUPDATECMDUI )
	{
		if(iKeyDown==-1)
			::InvalidateRect( handle(), 0, TRUE );
	}
	if( m_pCtrl->GetSafeHwnd() )
	{
		switch( m )
		{
			//case WM_LBUTTONDOWN:
			//case WM_RBUTTONDOWN:
			//case WM_MBUTTONDOWN:
			//case WM_LBUTTONUP:
			//case WM_MBUTTONUP:
			//case WM_RBUTTONUP:
			case WM_MOUSEMOVE:
			{
				//m_bToolTipVisible=false;
				DWORD pos = ::GetMessagePos();
				POINT pt = { LOWORD(pos), HIWORD(pos) };

				MSG Msg = {0};
				Msg.hwnd = handle();
				Msg.message = m;
				Msg.lParam = l;
				Msg.wParam = w;
				Msg.pt = pt;
				Msg.time = ::GetMessageTime();

				m_pCtrl->RelayEvent( &Msg );
			}
		}
	}

		if( m == WM_MOUSELEAVE )
		{
			POINT pt_cur = {0};
 			::GetCursorPos( &pt_cur );

			set_status_text( 0 );
			if( !m_bsub_mode && CPaneItem::m_hwnd_open_group && ::WindowFromPoint( pt_cur ) != CPaneItem::m_hwnd_open_group )
			{	
				IUnknownPtr ptr_u( ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow ), false );
				IMainWindowPtr sptr_w = ptr_u;
				if( sptr_w )
				{
					BOOL bhelp_mode = 0;
					sptr_w->IsHelpMode( &bhelp_mode );

					if( bhelp_mode )
					{
 						CPaneItem::m_bhelp_mode = true;
#ifdef MOUSE_TRACE
						_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSELEAVE. Enter Help Mode in simple mode %x", CPaneItem::m_hwnd_open_group );
#endif
						::SetTimer( CPaneItem::m_hwnd_open_group, HIDE_TIMER, 10, 0 );
						return 0;
					}
				}

 				if( !CPaneItem::m_bhelp_mode )
				{
#ifdef MOUSE_TRACE
					_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSELEAVE. Destroy pup menu %x", CPaneItem::m_hwnd_open_group );
#endif
 					::DestroyWindow( CPaneItem::m_hwnd_open_group );
//					::SetForegroundWindow( handle() );
					CPaneItem::m_hwnd_open_group = 0;
					CPaneItem::m_pactive_item = 0;	  
				}
			}

  			m_bMouseOver = false;
 			::InvalidateRect( handle(), &m_rcOver, TRUE );
			::ZeroMemory( &m_rcOver, sizeof( m_rcOver ) );
			::ZeroMemory( &m_ptOver, sizeof( m_ptOver ) );

			POINT pt = m_ptPrevMouse;
			::ClientToScreen( handle(), &pt );

			CPaneItem *pItem = get_item_by_pos( pt );
			if( pItem )
			{
				pItem->on_mouse_leave();
			}

 			if( m_bsub_mode )
			{
				set_status_text( 0 );

				IUnknownPtr ptr_u( ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow ), false );
				IMainWindowPtr sptr_w = ptr_u;
				if( sptr_w )
				{
					BOOL bhelp_mode = 0;
					sptr_w->IsHelpMode( &bhelp_mode );

					if( bhelp_mode )
					{
#ifdef MOUSE_TRACE
						_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSELEAVE. Enter Help Mode in pupup menu %x", handle() );
#endif
						CPaneItem::m_bhelp_mode = true;
						::SetTimer( handle(), HIDE_TIMER, 10, 0 );
						return 0;
					}
				}

				if( !CPaneItem::m_bhelp_mode )
				{
					HWND hparent = ::GetParent( handle() );

#ifdef MOUSE_TRACE
					_trace_file( LOG_FILE, "[~] PaneItemPlace::WM_MOUSELEAVE. Destroy pup menu in sub pane %x", handle() );
#endif
					::DestroyWindow( handle() );
					::InvalidateRect( hparent, 0, TRUE );
				}
				return 1L;
			}
		}

		if( m == WM_NCDESTROY )
		{
			::ZeroMemory( &m_ptOver, sizeof( m_ptOver ) );

				if( m_pCtrl && m_pCtrl->GetSafeHwnd() )
				delete m_pCtrl;
				m_pCtrl = 0;
				m_bToolTipVisible=false;
		}

		if( m == WM_CREATE )
		{
			long lRes = __super::handle_message( m, w, l );

			m_pCtrl = new CToolTipCtrl;
			m_pCtrl->Create( /*CWnd::FromHandle( handle() )*/0, TTS_ALWAYSTIP );
			m_pCtrl->SetMaxTipWidth( SHRT_MAX );

			::SendMessage( m_pCtrl->GetSafeHwnd(), TTM_SETDELAYTIME,   TTDT_AUTOPOP, 2000/*SHRT_MAX*/ );
			::SendMessage( m_pCtrl->GetSafeHwnd(), TTM_SETDELAYTIME,   TTDT_INITIAL, SHRT_MAX );
  			::SendMessage( m_pCtrl->GetSafeHwnd(), TTM_SETDELAYTIME,   TTDT_RESHOW,  200 );

			RECT rc =  { 0, 0, 5000, 5000 };
			m_pCtrl->AddTool( CWnd::FromHandle( handle() ), LPSTR_TEXTCALLBACK, &rc, 1500 );
			m_pCtrl->Activate( false );

			return lRes;
		}

		if(m == WM_NCHITTEST)
		{
			int bResult;
			bResult = GetKeyState(VK_LBUTTON);
			if(bResult & 0x8000)
			{
				POINT pt_cur = {0};
 				::GetCursorPos( &pt_cur );
				CPaneItem *pItem = get_item_by_pos( pt_cur );
				if( pItem )
				{
					DWORD dw = pItem->get_action_flag();
					if(dw & afEnabled)
						pItem->m_bDropItemPane=true;
					pItem->on_mouse_over(pt_cur,handle());
					pItem->m_bDropItemPane=false;
				}
			}
			else
			{
				POINT pt_cur = {0};
 				::GetCursorPos( &pt_cur );
				if(bDragDrop)
				{
					bDragDrop=false;
					if( CPaneItem::m_hwnd_open_group && ::WindowFromPoint( pt_cur ) == CPaneItem::m_hwnd_open_group )
					{
						if(CPaneItem::sptrActionInfo)
							add_item( CPaneItem::sptrActionInfo, pt_cur );
					}
				}
			}
		}
		return __super::handle_message( m, w, l );
}

CPaneItem* CPaneItemPlace::get_item_by_pos( POINT ptScreen )
{
	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		RECT rcItem = pItem->get_virtual_rect();

		::ClientToScreen( handle(), (LPPOINT)&rcItem );
		::ClientToScreen( handle(), (LPPOINT)&rcItem + 1 );

		if( ::PtInRect( &rcItem, ptScreen ) )
			return pItem;
	}
	return 0;
}

void CPaneItemPlace::store_content( INamedData *punkND, CString strRoot )
{
	::SetValue( punkND, strRoot, ENTRY_ITEM_COUNT, m_listItems.count() );

	for( long lPos = m_listItems.head(), i = 0; lPos; lPos = m_listItems.next( lPos ), i++ )
	{
		CPaneItem *pItem = m_listItems.get( lPos );

		CString strSect;
		strSect.Format( SECT_ITEM "%d", i );

		pItem->store_content( punkND, strRoot + strSect );
	}
}

void CPaneItemPlace::load_content( INamedData *punkND, CString strRoot )
{
	long lCount = ::GetValueInt( punkND, strRoot, ENTRY_ITEM_COUNT );
	HDC hDipl = ::GetDC( 0 );

	POINT pt = {0, 0 };
	RECT rcItem = {0};

	for( long i = 0; i < lCount; i++ )
	{
		CPaneItem *pItem = new CPaneItem;

		CString strSect;
		strSect.Format( SECT_ITEM "%d", i );

		pItem->load_content( punkND, strRoot + strSect );

		pItem->do_draw( hDipl, 0, true );
		rcItem = pItem->get_virtual_rect();

		m_listItems.add_tail( pItem );
		
		if( !pItem->is_empty() )
		{
			pItem->set_offset( pt );
			pt.y += rcItem.bottom;
		}
	}
	::ReleaseDC( 0, hDipl );
}

void CPaneItemPlace::add_item( IActionInfo *punkActionInfo, POINT pt )
{
	CPaneItem *pItemNew = new CPaneItem;

	pItemNew->create_from_info( punkActionInfo );

	HDC hDipl = ::GetDC( 0 );
	pItemNew->do_draw( hDipl, 0, true );
	::ReleaseDC( 0, hDipl );

	int nHeight = get_row_height();
	
	bool bOk = false;

	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );

		RECT rcItem = pItem->get_virtual_rect();

		::ClientToScreen( handle(), (LPPOINT)&rcItem );
		::ClientToScreen( handle(), (LPPOINT)&rcItem + 1 );

 		if( PtInRect( &rcItem, pt ) )
		{
			for( long lPos2 =  m_listItems.next( lPos ), i = 0; lPos2; lPos2 = m_listItems.next( lPos2 ) )
			{
				CPaneItem *pItem2 = m_listItems.get( lPos2 );
				RECT rcItem2 = pItem2->get_virtual_rect();

				POINT ptOffset = { rcItem2.left, rcItem2.top + nHeight };
				pItem2->set_offset( ptOffset );
			}

			rcItem = pItem->get_virtual_rect();
			POINT ptOffset = { rcItem.left, rcItem.top + nHeight };
			pItemNew->set_offset( ptOffset );

			m_listItems.insert_after( pItemNew, lPos );
			bOk = true;
			break;
		}
	}

	if( !bOk )
	{
		long lPos = m_listItems.tail();

		if( lPos )
		{
			CPaneItem *pItem = m_listItems.get( lPos );
			RECT rcItem = pItem->get_virtual_rect();

			POINT ptOffset = { rcItem.left, rcItem.top + nHeight };
			pItemNew->set_offset( ptOffset );
		}
		else
		{
			POINT ptOffset = { 0 };
			pItemNew->set_offset( ptOffset );
		}

		m_listItems.add_tail( pItemNew );
	}

	::PostMessage( ::GetParent( ::GetParent( handle() ) ), WM_NEED_RECALC, -1, 0 );
	::PostMessage( ::GetParent( ::GetParent( handle() ) ), WM_NEED_RECALC, GetDlgCtrlID( ::GetParent( handle() ) ), 0 );
}

void CPaneItemPlace::clear_items()
{
	m_listItems.clear();
}

long CPaneItemPlace::on_size( short cx, short cy, ulong fSizeType )
{
	for( long lPos = m_listItems.head(); lPos; lPos = m_listItems.next( lPos ) )
	{
		CPaneItem *pItem = m_listItems.get( lPos );
		if( pItem->is_empty() )
			continue;
		pItem->set_main_width( cx );
	}

	return 0L;
}

void CPaneItemPlace::remove_item( long lIndex )
{
	long lSkip = 0;
	for( long lPos = m_listItems.head(), i = 0; lPos; lPos = m_listItems.next( lPos ), i++ )
	{
		if( ( i - lSkip ) == lIndex )
		{
			for( long lPos2 = m_listItems.next( lPos ); lPos2; lPos2 = m_listItems.next( lPos2 ) )
			{
				CPaneItem *pItem = m_listItems.get( lPos2 );
				if( pItem->is_empty() )
				{
					lSkip++;
					continue;
				}
				RECT rcItem = pItem->get_virtual_rect(); 

				POINT ptOffset = { rcItem.left, 2 * rcItem.top - rcItem.bottom };

				pItem->set_offset( ptOffset );
			}

			m_listItems.remove( lPos );
			break;
		}
	}
}

void CPaneItemPlace::add_help_items( CStringArrayEx *pstrs, int &nidx, int nlevel )
{
	IApplicationPtr sptApp = ::GetAppUnknown();
	
	IUnknown *punkActionMan = 0;
	sptApp->GetActionManager( &punkActionMan );

	IActionManagerPtr sptrActionMan = punkActionMan;
	punkActionMan->Release(); punkActionMan = 0;

	for( long lPos = m_listItems.head(), i = 0; lPos; lPos = m_listItems.next( lPos ), i++ )
	{
		CPaneItem *pItem = m_listItems.get( lPos );

		if( pItem->m_strActionName == "----" )
			continue;

		IUnknown *punkAI = 0;
		sptrActionMan->GetActionInfo( _bstr_t( pItem->m_strActionName ), &punkAI );


		IHelpInfoPtr ptrHelp = punkAI;
		if( punkAI )
			punkAI->Release(); punkAI = 0;

		CString	str_module_name;
		CString	str_topic_name;
		CString str_prefix;

		if( ptrHelp )
		{
			BSTR	bstr_file = 0, bstr_topic = 0, bstr_prefix = 0;
			DWORD	dw_flags = 0;

			IHelpInfo2Ptr sptr_help2 = ptrHelp;
			
			if( sptr_help2 != 0 )
				sptr_help2->GetHelpInfo2( &bstr_file, &bstr_topic, &dw_flags, &bstr_prefix );
			else
				ptrHelp->GetHelpInfo( &bstr_file, &bstr_topic, &dw_flags );

			str_module_name = bstr_file;
			str_topic_name = bstr_topic;
			str_prefix = bstr_prefix;

			::SysFreeString( bstr_file );	bstr_file = 0;
			::SysFreeString( bstr_topic );	bstr_topic = 0;
			::SysFreeString( bstr_prefix );	bstr_prefix = 0;
		}
		else
		{
			str_module_name = "metodics";
			str_topic_name = pItem->m_strActionName;
			str_topic_name.Replace( " ", "_" );
		}

		char	sz_name[_MAX_PATH] = {0};
		::_splitpath( str_module_name, 0, 0, sz_name, 0 );

 		CString	str_link;

		CString str_chm_name( sz_name );		
		if( str_prefix.GetLength() )
		{
			if( str_prefix == _T("ax") )
				str_chm_name = _T("Forms");
			else if( str_prefix == _T("as") )
				str_chm_name = _T("Scripts");
		}


		CString str_help_path( _T("") );
		str_help_path = GetDirectory( "HelpPath", "Help" );

		// вначале пытаемся найти файл <str_chm_file> = "<help_path>\<str_chm_name>.chm"
		CString str_chm_file = str_help_path;
		str_chm_file += str_chm_name;
 		str_chm_file += _T( ".chm" );

		
		str_link += "<li><a href=\"ms-its:";
		//str_link += "<li><a href=\"mk:@MSITStore:";

		CString str_folder_name = str_chm_name;
		if( !file_exist( str_chm_file ) )
			str_chm_name = ::GetValueString( GetAppUnknown(), "\\General", "ProgramName", "Main" );
		
		//str_link += str_help_path;
		str_link += str_chm_name;
		str_link += ".chm::/";
		str_link += str_folder_name;
		str_link += "/";
		str_link += str_topic_name;
		str_link += ".html";
		str_link += "\">";						

   		for( int l = 0; l < nlevel; l++ )
			str_link += "&nbsp;&nbsp;&nbsp;";

		str_link += pItem->m_strActionUserName;
		str_link += "</a></li>";

		pstrs->InsertAt( nidx++, str_link );

		if( pItem->m_psub_items )
		{
			pItem->m_psub_items->add_help_items( pstrs, nidx, nlevel + 1 );
		}
	}
}

void CPaneItemPlace::on_context_help()
{
	//get the help directory

	CString	strPathName = GetDirectory( "HelpPath", "Help" );
	strPathName += "list_template.html";

	//parse html file
	CStringArrayEx	strs;

	try {
	strs.ReadFile( strPathName );
	}
	catch( CException *pe ) 
	{
		pe->ReportError();
		pe->Delete();
	}

	if( strs.GetSize() == 0 )
		return;

	int	idx1 = strs.Find( "<!-- list start -->" );
	int	idx2 = strs.Find( "<!-- list end -->" );

	if( idx1 == -1 || idx2 == -1 || idx2 < idx1+1 )
		return;

	strs.RemoveAt( idx1+1, idx2-idx1-2 );
	int	idx = idx1+1;

	strs.InsertAt( idx++, "<ul>" );

	add_help_items( &strs, idx, 1 );
	
	strs.InsertAt( idx++, "</ul>" );

	strs.WriteFile( strPathName );
	//HelpDisplayTopic( "text_menu", 0, "text_menu", "$GLOBAL_main" );
	HelpDisplayTopic( "text_menu", 0, "SubMetodicsTab_menu", "$GLOBAL_main" );
}

//////////////////////////////////////////////////////////////////////////
CPane::CPane() : m_btnUp( true ), m_btnDown( false )
{
	m_cl_back_color = 0;
}

CPane::~CPane()
{
}
bool CPane::create( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass )
{
	bool bRet = __super::create( style, rect, pszTitle, parent, hmenu, pszClass );
	
	if( bRet )
	{
		RECT rcBtn = { 0, 0, rect.right - rect.left, rect.bottom - rect.top };

		m_TitleBar.create( WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_OWNERDRAW |  BS_PUSHBUTTON , rcBtn, pszTitle, handle(), (HMENU)( (UINT_PTR)( hmenu ) * 100 ), _T("BUTTON") );

		HFONT hFontOld = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
		::SendMessage( m_TitleBar.handle(), WM_SETFONT, WPARAM( hFontOld ), true );

		RECT rect = {0};
		::GetClientRect( handle(), &rect );
	}

	return bRet;
}

long CPane::on_size( short cx, short cy, ulong fSizeType )
{
	int dy = cy;
	if( m_TitleBar.handle() )
	{
		RECT rc = {0};
		::GetWindowRect( m_TitleBar.handle(), &rc );
		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		::SetWindowPos( m_TitleBar.handle(), 0, rc.left, rc.top, cx, rc.bottom - rc.top, 0 );

		dy -=	 rc.bottom - rc.top;
	}
	
	if( m_ItemPlace.handle() )
	{
		RECT rc = {0};
		::GetWindowRect( m_ItemPlace.handle(), &rc );
		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		::SetWindowPos( m_ItemPlace.handle(), 0, rc.left, rc.top, cx, rc.bottom - rc.top, 0 );
		dy -=	 rc.bottom - rc.top;
	}

/*
	if( m_btnUp.handle() )
	{
		RECT rc = {0};
		::GetWindowRect( m_btnUp.handle(), &rc );
		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		::SetWindowPos( m_btnUp.handle(), 0, rc.left, rc.top, cx, rc.bottom - rc.top, 0 );
	}

	if( m_btnDown.handle() )
	{
		RECT rc = {0};
		::GetWindowRect( m_btnDown.handle(), &rc );
		::ScreenToClient( handle(), (LPPOINT)&rc );
		::ScreenToClient( handle(), (LPPOINT)&rc + 1 );

		::SetWindowPos( m_btnDown.handle(), 0, rc.left, rc.top, cx, rc.bottom - rc.top, 0 );
	}
	*/

	return 0L;
}

long CPane::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_IS_PANE_OPEN )
		return (long)is_open();
	else if( m == WM_UPDATESCROLL )
	{
		bool bUp = false, bDown = false;
		m_ItemPlace.check_scroll( &bUp, &bDown );

		if( bUp || bDown )
		{
			::EnableWindow( m_btnUp.handle(), bUp );
			::EnableWindow( m_btnDown.handle(), bDown );
		}
		return 0L;
	}
	else if( m == WM_COMMAND && l )
	{
		if( HWND( l ) == m_TitleBar.handle() )
		{
			SCROLLINFO info = {0};
 			info.cbSize				= sizeof( SCROLLINFO );
			info.fMask				= SIF_ALL;
 			::GetScrollInfo( ::GetParent( ::GetParent( handle() ) ), SB_VERT,&info );

			::SendMessage( ::GetParent( handle() ), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, 0 ), -1 );
			if( !m_ItemPlace.handle() )
				::SendMessage( ::GetParent( handle() ), WM_NEED_RECALC, GetDlgCtrlID( handle() ), 0 );
			else
				::SendMessage( ::GetParent( handle() ), WM_NEED_RECALC, -1, 0 );

			SCROLLINFO info2 = {0};
 			info2.cbSize				= sizeof( SCROLLINFO );
			info2.fMask				= SIF_ALL;

 			::GetScrollInfo( ::GetParent( ::GetParent( handle() ) ), SB_VERT,&info2 );

			::SendMessage( ::GetParent( handle() ), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, min( info.nPos, info2.nMax - info2.nPage + 1 ) ), 0 );
		}
		else if( HWND( l ) == m_btnUp.handle() )
		{
			POINT pt = { 0, m_ItemPlace.get_row_height() };
			m_ItemPlace.offset_items( pt );

			::InvalidateRect( m_ItemPlace.handle(), 0, TRUE );

			handle_message( WM_UPDATESCROLL, 0, 0 );
		}
		else if( HWND( l ) == m_btnDown.handle() )
		{
			POINT pt = { 0, -m_ItemPlace.get_row_height() };
			m_ItemPlace.offset_items( pt );

			::InvalidateRect( m_ItemPlace.handle(), 0, TRUE );

			handle_message( WM_UPDATESCROLL, 0, 0 );
		}
	}
	else if( m == WM_NEED_RECALC )
	{
		SCROLLINFO info = {0};
 		info.cbSize				= sizeof( SCROLLINFO );
		info.fMask				= SIF_ALL;
 		::GetScrollInfo( ::GetParent( ::GetParent( handle() ) ), SB_VERT,&info );

		::SendMessage( ::GetParent( handle() ), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, 0 ), -1 );

		::SendMessage( ::GetParent( handle() ), WM_NEED_RECALC, w, l );

		SCROLLINFO info2 = {0};
 		info2.cbSize				= sizeof( SCROLLINFO );
		info2.fMask				= SIF_ALL;

 		::GetScrollInfo( ::GetParent( ::GetParent( handle() ) ), SB_VERT,&info2 );

		::SendMessage( ::GetParent( handle() ), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, min( info.nPos, info2.nMax - info2.nPage + 1 ) ), 0 );
	}


	return __super::handle_message( m, w, l );
}

void CPane::open_pane( bool bOpen )
{
	if( bOpen )
	{
 		RECT rcNeed = m_ItemPlace.get_virtual_rect();

		RECT rc = {0};
 		::GetClientRect( m_TitleBar.handle(), &rc );
		
		rc.top = rc.bottom + SCROLL_BTN_WIDTH + 1;

		int h = ::SendMessage( ::GetParent( handle() ), WM_GET_MAX_SIZE, 0, 0 ) + 2;

		rc.bottom = rc.top + min( rcNeed.bottom - rcNeed.top, h);


		long lItemH = m_ItemPlace.get_row_height();
		int nCount = 0;
		
		if( lItemH )
			nCount = ( rc.bottom - rc.top ) / lItemH;

   		rc.bottom = rc.top + nCount * lItemH;
		
		m_ItemPlace.create( WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, 0, handle(), ( HMENU )( ::GetDlgCtrlID( handle() ) * 101 ), 0 );


		bool bUp = false, bDown = false;
		m_ItemPlace.check_scroll( &bUp, &bDown );

		int iBottom = 0;

		if( bUp || bDown )
		{
			RECT rcUp = { 0, rc.top - SCROLL_BTN_WIDTH, rc.right - rc.left, rc.top };
			RECT rcDown = { 0, rc.bottom, rc.right - rc.left, rc.bottom + SCROLL_BTN_WIDTH };

			m_btnUp.create( WS_VISIBLE|WS_CHILD|WS_TABSTOP| BS_OWNERDRAW |  BS_PUSHBUTTON | BS_FLAT, rcUp, "...", handle(), ( HMENU )( ::GetDlgCtrlID( handle() ) * 102 ), _T("BUTTON") );
			m_btnDown.create( WS_VISIBLE|WS_CHILD|WS_TABSTOP| BS_OWNERDRAW | BS_PUSHBUTTON |BS_FLAT , rcDown, "...", handle(), ( HMENU )( ::GetDlgCtrlID( handle() ) * 103 ), _T("BUTTON") );

			handle_message( WM_UPDATESCROLL, 0, 0 );
			iBottom = rcDown.bottom;
		}
		else
		{
			::OffsetRect( &rc, 0, -SCROLL_BTN_WIDTH - 1 );
			::MoveWindow( m_ItemPlace.handle(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true  );
			iBottom = rc.bottom;
		}

		RECT rcWnd = {0};
		::GetWindowRect( handle(), &rcWnd );

		::ScreenToClient( ::GetParent( handle() ), (LPPOINT)&rcWnd );
		::ScreenToClient( ::GetParent( handle() ), (LPPOINT)&rcWnd + 1 );

		rcWnd.bottom = rcWnd.top + iBottom;
		::SetWindowPos( handle(), 0, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 0 );

		InvalidateRect( m_TitleBar.handle(), 0, TRUE );
	}
	else
	{
		if( m_ItemPlace.handle() )
		{
			::DestroyWindow( m_ItemPlace.handle() );

			CPaneItem *pItem =  m_ItemPlace.get_item( 0 );
			
			if( pItem )
			{
				POINT pt = pItem->get_offset();

				pt.x *= -1;
				pt.y *= -1;

			   m_ItemPlace.offset_items( pt );
			}

			::DestroyWindow( m_btnUp.handle() );
			::DestroyWindow( m_btnDown.handle() );

			RECT rc = {0};
			::GetClientRect( m_TitleBar.handle(), &rc );

			RECT rcWnd = {0};
			::GetWindowRect( handle(), &rcWnd );

			::ScreenToClient( ::GetParent( handle() ), (LPPOINT)&rcWnd );
			::ScreenToClient( ::GetParent( handle() ), (LPPOINT)&rcWnd + 1 );

			rcWnd.bottom = rcWnd.top + rc.bottom - rc.top;
			::SetWindowPos( handle(), 0, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 0 );
		}

		InvalidateRect( m_TitleBar.handle(), 0, TRUE );
	}
}


RECT CPane::get_virtual_rect()
{
	//включить в размер
	RECT rcNeed = m_ItemPlace.get_virtual_rect();

	RECT rc = {0};
	::GetClientRect( m_TitleBar.handle(), &rc );

	rcNeed.bottom += 1 + rc.bottom;

	return rcNeed;
}

RECT CPane::get_virtual_rect_lite()
{
	RECT rc = {0};
	::GetClientRect( m_TitleBar.handle(), &rc );

	return rc;
}


void CPane::store_content( INamedData *punkND, CString strRoot )
{
	CString strName;
	int nBuffer = ::GetWindowTextLength( m_TitleBar.handle() );
	::GetWindowText( m_TitleBar.handle(), strName.GetBufferSetLength( nBuffer ), nBuffer + 1 );
	::SetValueColor( punkND, strRoot, ENTRY_ITEM_CUSTOM_ICON_BACK_COLOR, m_cl_back_color );

	::SetValue( punkND, strRoot, ENTRY_PANE_NAME, strName );

	::SetValue( punkND, strRoot, ENTRY_IMAGE_NAME, m_strFile );

	strName.ReleaseBuffer();
	m_ItemPlace.store_content( punkND, strRoot );
}

void CPane::load_content( INamedData *punkND, CString strRoot )
{
	CString strName = ::GetValueString( punkND, strRoot, ENTRY_PANE_NAME, "" );
	::SetWindowText( m_TitleBar.handle(), strName );

	CString strFile = ::GetValueString( punkND, strRoot, ENTRY_IMAGE_NAME, "" );
	CString strPath = ::GetValueString( ::GetAppUnknown(), "\\Paths", "States", "" );
	m_cl_back_color = ::GetValueColor( punkND, strRoot, ENTRY_ITEM_CUSTOM_ICON_BACK_COLOR, m_cl_back_color );

	m_strFile = strFile;

	if(HBITMAP hBitmap = (HBITMAP)::LoadImage( 0, strPath + strFile, IMAGE_BITMAP, PANE_IMAGE_SIZE, PANE_IMAGE_SIZE, LR_LOADFROMFILE | LR_LOADTRANSPARENT ))
	{
		HIMAGELIST hImageList = ::ImageList_Create( PANE_IMAGE_SIZE, PANE_IMAGE_SIZE, ILC_COLOR24 | ILC_MASK,	 1, 1 );
		::ImageList_AddMasked( hImageList, hBitmap, m_cl_back_color );
		HICON hIcon = ::ImageList_GetIcon( hImageList, 0, ILD_NORMAL );

		::ImageList_Destroy( hImageList );
		VERIFY(::DeleteObject( hBitmap ));
		m_TitleBar.set_icon( hIcon );
	}
	m_ItemPlace.load_content( punkND, strRoot );
}

long CPane::on_erasebkgnd( HDC hDC )
{
 	RECT rect = {0};
	::GetClientRect( handle(), &rect );

 	bool bIsOpen = is_open();

 	COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
	COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

	COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

	if( !bIsOpen )
		clrLight = ::GetSysColor( COLOR_BTNFACE );

	HBRUSH hBrush = ::CreateSolidBrush( clrLight );
	::FillRect( hDC, &rect, hBrush );

	if( bIsOpen)
		::DrawEdge( hDC, &rect, BDR_SUNKENOUTER, BF_RECT );

	VERIFY(::DeleteObject( (HGDIOBJ )hBrush ));
	return 0L; 
}

bool CPane::open_mtd_pane( CString str_name, bool bopen )
{
	if( str_name[0] != '\\' )
		str_name = "\\" + str_name;
		
 	if( m_str_pane == str_name )
	{
 		::PostMessage( handle(), WM_NEED_RECALC, -1, 0 );
		if( bopen )
			::PostMessage( handle(), WM_NEED_RECALC, GetDlgCtrlID( handle() ), 0 );

	//	open_pane( bopen );
		return true ;
	}
	return false;
}

void CPane::on_context_help()
{
	//get the help directory

	CString	strPathName = GetDirectory( "HelpPath", "Help" );
	strPathName += "list_template.html";

	//parse html file
	CStringArrayEx	strs;

	try {
	strs.ReadFile( strPathName );
	}
	catch( CException *pe ) 
	{
		pe->ReportError();
		pe->Delete();
	}

	if( strs.GetSize() == 0 )
		return;

	int	idx1 = strs.Find( "<!-- list start -->" );
	int	idx2 = strs.Find( "<!-- list end -->" );

	if( idx1 == -1 || idx2 == -1 || idx2 < idx1+1 )
		return;

	strs.RemoveAt( idx1+1, idx2-idx1-2 );
	int	idx = idx1+1;

	strs.InsertAt( idx++, "<ul>" );

	CPaneItemPlace *pplace = &m_ItemPlace;
	if( pplace )
		pplace->add_help_items( &strs, idx, 1 );
	
	strs.InsertAt( idx++, "</ul>" );

	strs.WriteFile( strPathName );
	//HelpDisplayTopic( "text_menu", 0, "text_menu", "$GLOBAL_main" );
	HelpDisplayTopic( "text_menu", 0, "MetodicsTab_menu", "$GLOBAL_main" );
	/*if(m_str_pane=="\\General\\Pane0")
		HelpDisplayTopic( "dlg", 0, "dlg_enhance", "$GLOBAL_main" );
	if(m_str_pane=="\\General\\Pane1")
		HelpDisplayTopic( "dlg", 0, "dlg_transform_general", "$GLOBAL_main" );
	if(m_str_pane=="\\General\\Pane2")
		HelpDisplayTopic( "dlg", 0, "dlg_enhancement", "$GLOBAL_main" );
	if(m_str_pane=="\\General\\Pane3")
		HelpDisplayTopic( "Objects", 0, "dlg_objects", "$GLOBAL_main" );
	if(m_str_pane=="\\General\\Pane4")
		HelpDisplayTopic( "dlg", 0, "dlg_stat", "$GLOBAL_main" );
	if(m_str_pane=="\\General\\Pane5")
		HelpDisplayTopic( "dlg", 0, "dlg_stat", "$GLOBAL_main" );*/
}




//////////////////////////////////////////////////////////////////////////
long CPaneScrollBtn::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_DRAWITEM )
		return on_notify_reflect( (NMHDR*)pmsg, plProcessed );
	return __super::handle_reflect_message( pmsg, plProcessed );
}

long CPaneScrollBtn::on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
{
	if( ( (MSG *)pnmhdr )->message ==  WM_DRAWITEM )
	{
 		LPDRAWITEMSTRUCT pdrw = (LPDRAWITEMSTRUCT)( ( (MSG *)pnmhdr )->lParam );

		COLORREF clrBk = ::GetSysColor( COLOR_BTNSHADOW );

		RECT rcClient = {0};
		::GetClientRect( handle(), &rcClient );

		int nW = 5, nH = 5;
		int nw = ( rcClient.right - rcClient.left ) / 2;
		int nTop = ( rcClient.bottom - nH ) / 2;

		HRGN hRgn = 0;
		HRGN hRgn2 = 0;

		BOOL bEnableW = IsWindowEnabled( handle() );

		if( m_bVert )
		{
			POINT pts[3] = { rcClient.left + nw, nTop + 1, rcClient.left + nw - nW, nTop + nH + 1, rcClient.left + nw + nW, nTop + nH + 1 };
			POINT pts2[3] = { rcClient.left + nw - 1, nTop, rcClient.left + nw - nW - 1, nTop + nH, rcClient.left + nw + nW - 1, nTop + nH };
			hRgn = ::CreatePolygonRgn( pts, 3, WINDING );
			hRgn2 = ::CreatePolygonRgn( pts2, 3, WINDING );
		}
		else
		{
			POINT pts[3] = { rcClient.left + nw, nTop + nH + 1, rcClient.left + nw - nW, nTop + 1, rcClient.left + nw + nW, nTop + 1 };
			POINT pts2[3] = { rcClient.left + nw - 1, nTop + nH, rcClient.left + nw - nW - 1, nTop, rcClient.left + nw + nW - 1, nTop };

			hRgn = ::CreatePolygonRgn( pts, 3, WINDING );
			hRgn2 = ::CreatePolygonRgn( pts2, 3, WINDING );
		}

		if( hRgn )
		{
			if( bEnableW )
				::FillRgn( pdrw->hDC, hRgn, CBrush( ::GetSysColor( COLOR_BTNTEXT ) ) );
			else
			{
				::FillRgn( pdrw->hDC, hRgn2, CBrush( ::GetSysColor( COLOR_BTNHILIGHT ) ) );
				::FillRgn( pdrw->hDC, hRgn, CBrush( ::GetSysColor( COLOR_BTNSHADOW ) ) );
			}
		}

		if( m_bMiceOver )
		{
			CDC *pDc = CDC::FromHandle( pdrw->hDC );

			if( !m_bDown )
				pDc->Draw3dRect( &rcClient, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
			else
				pDc->Draw3dRect( &rcClient, ::GetSysColor( COLOR_BTNSHADOW ), ::GetSysColor( COLOR_BTNHILIGHT ) );

		}
		else
		{
			HPEN hPen = 0;
		
			if( bEnableW )
				hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNTEXT ) );
			else
				hPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );

			HPEN hold = (HPEN)::SelectObject( pdrw->hDC, hPen );
			HBRUSH holdB = (HBRUSH)::SelectObject( pdrw->hDC, GetStockObject( NULL_BRUSH ) );

			::Rectangle( pdrw->hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );

			::SelectObject( pdrw->hDC, hold );
			::SelectObject( pdrw->hDC, holdB );
			
			VERIFY(::DeleteObject( hPen ));
		}

		if( hRgn )
			VERIFY(::DeleteObject( hRgn ));
		if( hRgn2 )
			VERIFY(::DeleteObject( hRgn2 ));

		*plProcessed = 1;
	}

	return __super::on_notify_reflect( pnmhdr, plProcessed );
}

long CPaneScrollBtn::on_erasebkgnd( HDC hDC )
{
	return 0L;
	RECT rect = {0};
	::GetClientRect( handle(), &rect );

	HBRUSH hBrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );
	::FillRect( hDC, &rect, hBrush );

	VERIFY(::DeleteObject( (HGDIOBJ )hBrush ));
	return 0L; 
}

long  CPaneScrollBtn::on_mousemove( const _point &point )
{
	if( !m_bMiceOver )
		InvalidateRect( handle(), 0, TRUE );

	m_bMiceOver = true;
	TRACKMOUSEEVENT		csTME = {0};

	csTME.cbSize = sizeof( csTME );
	csTME.dwFlags = TME_LEAVE;
	csTME.hwndTrack = handle();

	::_TrackMouseEvent( &csTME );

	return __super::on_mousemove( point );
}

long  CPaneScrollBtn::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_MOUSELEAVE )
		return on_mouseleave();
	return __super::handle_message( m, w, l );
}

long  CPaneScrollBtn::on_mouseleave()
{
	m_bMiceOver = false;
	::InvalidateRect( handle(), 0, TRUE );
	return 1L;
}

long CPaneScrollBtn::on_lbuttondown( const _point &point )
{
	m_bDown = true;
	::InvalidateRect( handle(), 0, TRUE );
	return __super::on_lbuttondown( point );
}

long CPaneScrollBtn::on_lbuttonup( const _point &point )
{
	m_bDown = false;
	::InvalidateRect( handle(), 0, TRUE );
	return __super::on_lbuttonup( point );
}


//////////////////////////////////////////////////////////////////////////
CPaneTitle::CPaneTitle()
{
	m_bMiceOver = false;
	m_hIcon = 0;
}

CPaneTitle::~CPaneTitle()
{
	if( m_hIcon )
		VERIFY( ::DestroyIcon( m_hIcon ) );
	m_hIcon = 0;
}

long CPaneTitle::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_DRAWITEM )
		return on_notify_reflect( (NMHDR*)pmsg, plProcessed );
	return __super::handle_reflect_message( pmsg, plProcessed );
}

long CPaneTitle::on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
{
 	if( ( (MSG *)pnmhdr )->message ==  WM_DRAWITEM )
	{
 		LPDRAWITEMSTRUCT pdrw = (LPDRAWITEMSTRUCT)( ( (MSG *)pnmhdr )->lParam );

		CDC*	pDC = CDC::FromHandle( pdrw->hDC );

		bool bIsOpen = ::SendMessage( ::GetParent( handle() ), WM_IS_PANE_OPEN, 0, 0 );

 		COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
		COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

		COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								 GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								 GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

		int nOff = ( HEIGHT_PANEHEADER  - PANE_IMAGE_SIZE ) / 2;
		RECT rcItem = pdrw->rcItem;
		RECT rcImage = { pdrw->rcItem.left, pdrw->rcItem.top, pdrw->rcItem.left + ( HEIGHT_PANEHEADER ), pdrw->rcItem.top + ( HEIGHT_PANEHEADER ) };


		rcItem.left	+= ( HEIGHT_PANEHEADER ) + 1;

		if( bIsOpen )
		{
			pDC->FillRect( &rcItem, &CBrush( clrLight ) );
			pDC->FillRect( &rcImage, &CBrush( clrLight ) );
		}
		
 		if( m_bMiceOver )
		{
 			if( !bIsOpen )
			{
				pDC->Draw3dRect( &rcImage, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
				pDC->Draw3dRect( &rcItem, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
			}
			else if( bIsOpen )
			{
				pDC->FillRect( &rcImage, &CBrush( clrBk ) );
				pDC->FillRect( &rcItem, &CBrush( clrBk ) );
			}
		}

		if( !bIsOpen )
			pDC->Draw3dRect( &rcImage, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
		else
			pDC->Draw3dRect( &rcImage, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT) );

		if( bIsOpen )
			pDC->Draw3dRect( &rcItem, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT) );

 		CString strText;

		int nLen = ::GetWindowTextLength( handle() );
		::GetWindowText( handle(), strText.GetBufferSetLength( nLen ), nLen + 1 ) ;

		COLORREF clTextOld = pDC->SetTextColor( ::GetSysColor( COLOR_BTNTEXT ) );
 		int nOldMode = pDC->SetBkMode( TRANSPARENT );

		RECT rcText = rcItem;
		rcText.left += PANE_TEXT_OFFSET;

		pDC->DrawText( strText, &rcText, DT_SINGLELINE | DT_VCENTER );

		pDC->SetBkMode( nOldMode );
		pDC->SetTextColor( clTextOld );

		strText.ReleaseBuffer();
		if( m_hIcon )
		{
			if( !bIsOpen )
				::DrawState( pdrw->hDC, 0, 0, (LPARAM)m_hIcon, 0, rcImage.left + nOff, rcImage.top + nOff, rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, DST_ICON );
			else
			{
//				::DrawState( pdrw->hDC, 0, 0, (LPARAM)m_hIcon, 0, rcImage.left + nOff + 1, rcImage.top + nOff + 1, rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, DST_ICON | DSS_DISABLED );
				::DrawState( pdrw->hDC, 0, 0, (LPARAM)m_hIcon, 0, rcImage.left + nOff, rcImage.top + nOff, rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, DST_ICON );
			}
		}

		*plProcessed = 1;
	}

	return __super::on_notify_reflect( pnmhdr, plProcessed );
}

long	CPaneTitle::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_MOUSELEAVE )
		return on_mouseleave();
	return __super::handle_message( m, w, l );
}

long CPaneTitle::on_mousemove( const _point &point )
{
	if( !m_bMiceOver )
		InvalidateRect( handle(), 0, TRUE );

	m_bMiceOver = true;
	TRACKMOUSEEVENT		csTME = {0};

	csTME.cbSize = sizeof( csTME );
	csTME.dwFlags = TME_LEAVE;
	csTME.hwndTrack = handle();

	::_TrackMouseEvent( &csTME );

	return __super::on_mousemove( point );
}
long CPaneTitle::on_mouseleave()
{
	m_bMiceOver= false;
	InvalidateRect( handle(), 0, TRUE );
	return 1L;
}

long CPaneTitle::on_lbuttonup( const _point &point )
{
	return __super::on_lbuttonup( point );
}

//////////////////////////////////////////////////////////////////////////

CPaneBar::CPaneBar()
{
	m_strClassName = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		(HCURSOR)::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)COLOR_WINDOW,
		NULL);

	m_nLastPaneID = CTRL_START_ID;
	m_block_send = false;

	bLoadContent=false;

	m_hicon_group_open	= (HICON)::LoadImage( module::hrc(), MAKEINTRESOURCE( IDI_ICON_OPEN ), IMAGE_ICON, PANE_IMAGE_SIZE_SMALL, PANE_IMAGE_SIZE_SMALL, 0 );
}

CPaneBar::~CPaneBar()
{
	if( m_hicon_group_open )
		VERIFY(::DestroyIcon( m_hicon_group_open ));
	m_hicon_group_open = 0;
}


CPane*	CPaneBar::AddNewPane( CString strName, long lInsertAfterIndex )
{
	CPane *ppane = new CPane( );

	RECT rect = {0};
	CPane *pLastPane = 0;
	

	if( m_PaneList.count() > 0 )
		pLastPane = m_PaneList.get( m_PaneList.tail() );
	
	if( pLastPane )
	{
		::GetWindowRect( pLastPane->handle(), &rect );
		
		::ScreenToClient( handle(), (LPPOINT)&rect );
		::ScreenToClient( handle(), (LPPOINT)&rect + 1 );

		rect.top = rect.bottom + 1;
		rect.bottom = rect.top + HEIGHT_PANEHEADER;
	}
	else
	{
		::GetWindowRect( handle(), &rect );
		
		rect.bottom = HEIGHT_PANEHEADER;
		rect.right = rect.right - rect.left;
		rect.left = rect.top = 0;
	}


	CString sPaneName = strName;
	
	if( sPaneName.IsEmpty() )
		sPaneName.Format( "Pane %d", m_PaneList.count( ) );

	ppane->create( WS_VISIBLE|WS_CHILD, rect, sPaneName, handle(), (HMENU)((UINT_PTR)_GetNewPaneID()) );
	ppane->set_name( sPaneName );

	if( lInsertAfterIndex < 0 || lInsertAfterIndex >= m_PaneList.count( ) )
		m_PaneList.add_tail( ppane );
	else
	{
		for( long lPos = m_PaneList.head(), i = 0; lPos; lPos = m_PaneList.next( lPos ), i++ )
		{
			if( i == lInsertAfterIndex )
			{
				m_PaneList.insert_after( ppane, lPos );
				break;
			}
		}
	}
	return   ppane;
}

long CPaneBar::on_size( short cx, short cy, ulong fSizeType )
{
	SCROLLINFO info = {0};
 	info.cbSize				= sizeof( SCROLLINFO );
	info.fMask				= SIF_ALL;
	::GetScrollInfo( ::GetParent( handle() ), SB_VERT,&info );
	
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *ppane = m_PaneList.get( lPos );

		RECT rcClient = {0};
		::GetWindowRect( ppane->handle(), &rcClient );
		::ScreenToClient( handle(), (LPPOINT)&rcClient );
		::ScreenToClient( handle(), (LPPOINT)&rcClient + 1 );

		::SetWindowPos( ppane->handle(), 0, rcClient.left, rcClient.top, cx, rcClient.bottom - rcClient.top, 0 );
	}
	
	return 0L;
}

long CPaneBar::on_erasebkgnd( HDC hDC )
{
  	RECT rect = {0};
	::GetClientRect( handle(), &rect );
	COLORREF clrBk = ::GetSysColor( COLOR_3DFACE );
	COLORREF clrHighlight =::GetSysColor(  COLOR_3DHILIGHT );

	COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

	clrLight = ::GetSysColor( COLOR_BTNFACE );

 	HBRUSH hBrush = ::CreateSolidBrush( clrLight );
	::FillRect( hDC, &rect, hBrush );

	VERIFY(::DeleteObject( (HGDIOBJ )hBrush ));
	return 0L; 
}

long CPaneBar::handle_message( UINT m, WPARAM w, LPARAM l )
{
   	if( m == WM_NEED_RECALC )
	{
  		long lMainIndex = (long)w;
 		long lStatusParam = (long)l;

 		RECT rcPrev = {0};
		RECT rcFull = {0};

		bool block = m_block_send;

		for( long lPos = m_PaneList.head(), lIndex = 0; lPos; lPos = m_PaneList.next( lPos ), lIndex++ )
		{
			CPane *ppane = m_PaneList.get( lPos );

			if( !lStatusParam )
			{
				if( GetDlgCtrlID( ppane->handle() ) == lMainIndex )
					ppane->open_pane( true );
				else
					ppane->open_pane( false );
			}	
			else
			{
    			if( ppane->is_open()  )
				{
  					RECT rcClient = {0};
					::GetWindowRect( ppane->handle(), &rcClient );

					::ScreenToClient( handle(), (LPPOINT)&rcClient );
					::ScreenToClient( handle(), (LPPOINT)&rcClient + 1 );
			
					RECT rc_pane = ppane->get_virtual_rect();

 					::ClientToScreen( ppane->handle(), (LPPOINT)&rc_pane  );
					::ClientToScreen( ppane->handle(), (LPPOINT)&rc_pane + 1  );

					::ScreenToClient( handle(), (LPPOINT)&rc_pane );
					::ScreenToClient( handle(), (LPPOINT)&rc_pane + 1 );

  					int h = rc_pane.bottom - rc_pane.top - 2;

					::SetWindowPos( ppane->handle(), 0, rcClient.left, rcClient.top, rcClient.right - rcClient.left, h, 0 );
				}
			}

			RECT rcClient = {0};
			::GetWindowRect( ppane->handle(), &rcClient );

			::ScreenToClient( handle(), (LPPOINT)&rcClient );
			::ScreenToClient( handle(), (LPPOINT)&rcClient + 1 );

			if( !lIndex )
			{
				if(bLoadContent)
				{
					bLoadContent=false;
					int h=rcClient.bottom - rcClient.top;
					rcClient.top=0;
					rcClient.bottom=h;
				}

				::InvalidateRect( ppane->handle(), 0, TRUE );
				::SetWindowPos( ppane->handle(), 0, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 0 );
				rcPrev = rcClient;
				::UnionRect( &rcFull, &rcFull, &rcClient );
			}
			else
			{
 				int h = rcClient.bottom - rcClient.top;

				rcClient.top = rcPrev.bottom + 0;
				rcClient.bottom = rcClient.top + h;

				::InvalidateRect( ppane->handle(), 0, TRUE );
				::SetWindowPos( ppane->handle(), 0, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 0 );

				rcPrev = rcClient;
				::UnionRect( &rcFull, &rcFull, &rcClient );
			}
		}
				
		_update_scroll_info( rcFull );

		m_block_send = block;
		
		return 0L;
	}
	else if( m == WM_PREV_PANE_WIDTH )
	{
		long lMainIndex = (long)w;

		RECT rcPrev = {0};

		for( long lPos = m_PaneList.head(), lIndex = 0; lPos; lPos = m_PaneList.next( lPos ), lIndex++ )
		{
			CPane *ppane = m_PaneList.get( lPos );

			RECT rcClient = {0};
			::GetWindowRect( ppane->handle(), &rcClient );

			::ScreenToClient( handle(), (LPPOINT)&rcClient );
			::ScreenToClient( handle(), (LPPOINT)&rcClient + 1 );

			if( GetDlgCtrlID( ppane->handle() ) != lMainIndex )
				::UnionRect( &rcPrev, &rcPrev, &rcClient );
			else
				return rcPrev.bottom - rcPrev.top;
		}
		return 0L;
	}
	else if( m == WM_GET_MAX_SIZE )
	{
		RECT rcClient = {0};
		::GetClientRect( handle(), &rcClient );

		rcClient.bottom = 10000;
		return rcClient.bottom;

	}
	else if( m == WM_VSCROLL )
	{
		unsigned code			= LOWORD( w );
		int pos			= HIWORD( w );
		HWND hwndScroll		= (HWND)l;

		if( l < 0 )
			pos *= -1;

 		int npos = ::GetScrollPos( ::GetParent( handle() ), SB_VERT );
		int npos_prev = npos;

		SCROLLINFO info = {0};

		info.cbSize				= sizeof( SCROLLINFO );
		info.fMask				= SIF_ALL;

		::GetScrollInfo(  ::GetParent( handle() ), SB_VERT, &info );

		if( code == SB_LINEDOWN )
			npos++;
		else if( code == SB_LINEUP )
			npos--;
		else if( code == SB_PAGEDOWN )
			npos += min( info.nPage, (info.nMax - info.nPage) - npos_prev );
		else if( code == SB_PAGEUP )
			npos -= min( info.nPage, npos_prev );

		else if( code == SB_THUMBTRACK )
			npos = pos;

		::SetScrollPos(::GetParent( handle() ), SB_VERT, npos, TRUE );

		int dy = npos_prev - npos;

		if( dy > 0 && npos_prev > info.nMin || dy < 0 && npos_prev <  (info.nMax - info.nPage)  )
		{

			for( long lPos = m_PaneList.head(), lIndex = 0; lPos; lPos = m_PaneList.next( lPos ), lIndex++ )
			{
				CPane *ppane = m_PaneList.get( lPos );
				
				RECT rcClient = {0};
				::GetWindowRect( ppane->handle(), &rcClient );

				::ScreenToClient( handle(), (LPPOINT)&rcClient );
				::ScreenToClient( handle(), (LPPOINT)&rcClient + 1 );

				::OffsetRect( &rcClient, 0, dy );

				RECT rc_invalidate = rcClient;
   				if( dy < 0 )
				{
					rc_invalidate.top = rc_invalidate.bottom + dy;
					rc_invalidate.bottom = rc_invalidate.top - dy;
				}
				else
				{
					rc_invalidate.bottom = rc_invalidate.top + dy;
					rc_invalidate.top = rc_invalidate.bottom - dy;
				}

				//::SetWindowPos( ppane->handle(), 0, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 0 );
				::MoveWindow( ppane->handle(), rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE );
	 			::InvalidateRect( handle(), &rc_invalidate, TRUE );
			}
		}
	}

	return __super::handle_message( m, w, l );
}

#include <math.h>
void CPaneBar::_update_scroll_info( RECT rc )
{
	SCROLLINFO info = {0};

 	info.cbSize				= sizeof( SCROLLINFO );
	info.fMask				= SIF_ALL;

	::GetScrollInfo( ::GetParent( handle() ), SB_VERT, &info );

	RECT rcCl = {0};
	::GetClientRect( handle(), &rcCl );

  	info.nMin					= 0;
	info.nMax 				= rc.bottom - rc.top;
 	info.nPage				= rcCl.bottom - rcCl.top;
							
 	::SetScrollInfo( ::GetParent( handle() ), SB_VERT, &info, TRUE );
}

CPane* CPaneBar::get_item( long lIndex )
{
	for( long lPos = m_PaneList.head(), i = 0; lPos; lPos = m_PaneList.next( lPos ), i++ )
	{
		if( i == lIndex )
			return m_PaneList.get( lPos );
	}
	return 0;
}

CPane* CPaneBar::get_item_by_pos( POINT ptScreen )
{
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *pPane = m_PaneList.get( lPos );

		RECT rcPane = {0};
		::GetWindowRect( pPane->handle(), &rcPane );

		if( ::PtInRect( &rcPane, ptScreen ) )
			return pPane;
	}
	return 0;
}

void CPaneBar::remove_item( long lIndex )
{
	for( long lPos = m_PaneList.head(), i = 0; lPos; lPos = m_PaneList.next( lPos ), i++ )
	{
		if( i == lIndex )
		{
			m_PaneList.remove( lPos );
			break;
		}
	}
}

void CPaneBar::store_content( INamedData *punkND, CString str_root  )
{
	INamedDataPtr sptrND = punkND;
	
	::SetValue( punkND, str_root, ENTRY_PANE_COUNT, m_PaneList.count() );
	for( long lPos = m_PaneList.head(), i = 0; lPos; lPos = m_PaneList.next( lPos ), i++ )
	{
		CPane *pPane = m_PaneList.get( lPos );

		if( pPane )
		{
			CString strPaneIndex;
			strPaneIndex.Format( str_root + SECT_PANE "%d", i );
			pPane->store_content( punkND, strPaneIndex );
		}
	}
}

void CPaneBar::load_content( INamedData *punkND, CString str_root )
{
	INamedDataPtr sptrND = punkND;
	
	bLoadContent=true;

	clear_items();

	long lCount = ::GetValueInt( punkND, str_root, ENTRY_PANE_COUNT, m_PaneList.count() );

	for( long i = 0; i < lCount; i++ )
	{
		CString strPaneIndex;
		strPaneIndex.Format( str_root + SECT_PANE "%d", i );
		CPane *pPane = AddNewPane( strPaneIndex );

		if( pPane )
			pPane->load_content( punkND, strPaneIndex );
	}
	handle_message( WM_VSCROLL, MAKELONG( SB_THUMBTRACK, 0 ), 0 );
	handle_message	( WM_NEED_RECALC, 0, 1 );
}

void CPaneBar::clear_items()
{
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *pPane = m_PaneList.get( lPos );
		pPane->clear_items();
		::DestroyWindow( pPane->handle() );
	}
	m_PaneList.clear();
}

RECT CPaneBar::get_virtual_rect()
{
	RECT rc_ret = {0};

	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *pPane = m_PaneList.get( lPos );

		RECT rc_pane = {0};
		
		if( pPane->is_open() )
			rc_pane = pPane->get_virtual_rect();
		else
			rc_pane = pPane->get_virtual_rect_lite();


		rc_ret.left		= rc_pane.left;
		rc_ret.right		= rc_pane.right;

		rc_ret.top		= rc_pane.top;
		rc_ret.bottom	+= rc_pane.bottom;
	}

	if( m_PaneList.count() )
		rc_ret.bottom += 2;

	return rc_ret;
}

CPaneBar* CPaneBar::get_bar_by_pos( POINT ptScreen )
{
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *pPane = m_PaneList.get( lPos );

		RECT rcPane = {0};
		::GetWindowRect( pPane->handle(), &rcPane );

		if( ::PtInRect( &rcPane, ptScreen ) )
			return this;
	}

	if( handle() )
	{
		RECT rc_bar = {0};
		::GetWindowRect( handle(), &rc_bar );

		if( ::PtInRect( &rc_bar, ptScreen ) )
			return this;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////
LRESULT CPaneBar::help_hit_test( WPARAM wParam, LPARAM lParam )
{
	POINT pt_screen = { 0 };
	::GetCursorPos( &pt_screen );

	HWND hwnd = ::WindowFromPoint( pt_screen );

	CPaneItemPlace *ppane = get_pane_place_from_handle( hwnd );

	if( ppane )
	{
		CPaneItem *pitem = ppane->get_item_by_pos( pt_screen );

		if( pitem && !pitem->m_psub_items )
			return pitem->get_id();
		else if( pitem && pitem->m_psub_items )
		{
			pitem->m_psub_items->on_context_help();
			return DWORD( -2 );
		}
	}
	else
	{
		for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
		{
			CPane *ppane = m_PaneList.get( lPos );

			RECT rc_title = {0};
			::GetWindowRect( ppane->m_TitleBar.handle(), &rc_title );
			if( ::PtInRect( &rc_title, pt_screen ) )
			{
				ppane->on_context_help();
				return DWORD( -2 );
			}
		}
	}

	return 0;
}

bool CPaneBar::open_mtd_pane( CString str_name, bool bopen )
{
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *ppane = m_PaneList.get( lPos );

		if( ppane->open_mtd_pane( str_name, bopen ) )
			return true;
	}
	return false;
	}
////////////////////////////////////////////////////////////////////

CPaneItemPlace *CPaneBar::get_pane_place_from_handle( HWND hwnd )
{
	for( long lPos = m_PaneList.head(); lPos; lPos = m_PaneList.next( lPos ) )
	{
		CPane *ppane = m_PaneList.get( lPos );

		if( ppane->get_item_place()->handle() == hwnd )
			return ppane->get_item_place();

		ppane->get_item_place()->get_sub_place_from_handle( hwnd );
	}
	return 0;
}

void CPane::setDragDrop(IActionInfo *sptrActionInfoMtb)
{
	CPaneItem::sptrActionInfo=sptrActionInfoMtb;
	if(CPaneItem::sptrActionInfo)
		bDragDrop=true;
	else
		bDragDrop=false;
}
////////////////////////////////////////////////////////////////////
}
}